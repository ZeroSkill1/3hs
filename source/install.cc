/*
 * Copyright (C) 2021-2022 MyPasswordIsWeak
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include "settings.hh"
#include "install.hh"
#include "thread.hh"
#include "update.hh" /* includes net constants */
#include "error.hh"
#include "proxy.hh"
#include "panic.hh"
#include "ctr.hh"
#include "log.hh"

#include <3ds.h>

#define BUFSIZE 0x80000


enum class ITC // inter thread communication
{
	normal, exit, timeoutscr
};

typedef struct cia_net_data
{
	// We write to this handle
	Handle cia;
	// At what index are we writing __the cia__ now?
	u32 index = 0;
	// Total cia size
	u32 totalSize = 0;
	// Messages back and forth the UI/Install thread
	ITC itc = ITC::normal;
	// Buffer. allocated on heap for extra storage
	u8 *buffer = nullptr;
	// Tells second thread to wake up
	Handle eventHandle;
} cia_net_data;


static Result i_install_net_cia(std::string url, cia_net_data *data, size_t from, httpcContext *pctx)
{
	Result res = 0;
#define CHECKRET(expr) do { if(R_FAILED(res = ( expr ) )) { httpcCancelConnection(pctx); httpcCloseContext(pctx); return res; } } while(0)

	/* configure */
	if(R_FAILED(res = httpcOpenContext(pctx, HTTPC_METHOD_GET, url.c_str(), 0)))
		return res;
	CHECKRET(httpcSetSSLOpt(pctx, SSLCOPT_DisableVerify));
	CHECKRET(httpcSetKeepAlive(pctx, HTTPC_KEEPALIVE_ENABLED));
	CHECKRET(httpcAddRequestHeaderField(pctx, "Connection", "Keep-Alive"));
	CHECKRET(httpcAddRequestHeaderField(pctx, "User-Agent", USER_AGENT));
	CHECKRET(proxy::apply(pctx));

	if(from != 0)
	{
		CHECKRET(httpcAddRequestHeaderField(pctx, "Range", ("bytes=" + std::to_string(from) + "-").c_str()));
	}

	CHECKRET(httpcBeginRequest(pctx));

	u32 status = 0, dled = from;
	CHECKRET(httpcGetResponseStatusCode(pctx, &status));
	vlog("Download status code: %lu", status);

	// Do we want to redirect?
	if(status / 100 == 3)
	{
		char newurl[2048];
		CHECKRET(httpcGetResponseHeader(pctx, "location", newurl, sizeof(newurl)));
		newurl[sizeof(newurl) - 1] = '\0';
		std::string redir(newurl);

		vlog("Redirected to %s", redir.c_str());
		httpcCancelConnection(pctx);
		httpcCloseContext(pctx);
		return i_install_net_cia(redir, data, from, pctx);
	}

	// Are we resuming and does the server support range?
	if(from != 0 && status != 206)
	{
		httpcCancelConnection(pctx);
		httpcCloseContext(pctx);
		return APPERR_NORANGE;
	}
	// Bad status code
	else if(status != 200)
	{
		httpcCancelConnection(pctx);
		httpcCloseContext(pctx);
		return APPERR_NON200;
	}

	if(from == 0)
		CHECKRET(httpcGetDownloadSizeState(pctx, nullptr, &data->totalSize));
	/* else data->totalSize is already known */
	if(data->totalSize == 0)
	{
#ifndef RELEASE
		char buffer[0x6000];
		u32 total;
		httpcDownloadData(pctx, (u8 *) buffer, sizeof(buffer), &total);
		buffer[total] = '\0';
		dlog("API data on '%s' (probably json):\n%s", url.c_str(), buffer);
#endif
		httpcCancelConnection(pctx);
		httpcCloseContext(pctx);
		return APPERR_NOSIZE;
	}
	svcSignalEvent(data->eventHandle);

	// Install.
	u32 remaining = data->totalSize - from;
	u32 dlnext = BUFSIZE > remaining ? remaining : BUFSIZE;
	u32 written = 0;

	while(data->index != data->totalSize)
	{
		if((R_FAILED(res = httpcReceiveDataTimeout(pctx, data->buffer, dlnext, 30000000000L))
			&& res != (Result) HTTPC_RESULTCODE_DOWNLOADPENDING)
			|| R_FAILED(res = httpcGetDownloadSizeState(pctx, &dled, nullptr)))
		{
			dlog("aborted http connection due to error: %08lX.", res);
			svcSignalEvent(data->eventHandle);
			httpcCancelConnection(pctx);
			httpcCloseContext(pctx);
			return res;
		}
#define CHK_EXIT \
		if(data->itc == ITC::exit) \
		{ \
			dlog("aborted http connection due to ITC::exit"); \
			svcSignalEvent(data->eventHandle); \
			httpcCancelConnection(pctx); \
			httpcCloseContext(pctx); \
			break; \
		} 
		CHK_EXIT
		dlog("Writing to cia handle, size=%lu,index=%lu,totalSize=%lu", dlnext, data->index, data->totalSize);
		/* we don't need to add the FS_WRITE_FLUSH flag because AM just ignores write flags... */
		res = FSFILE_Write(data->cia, &written, data->index, data->buffer, dlnext, 0);
		CHK_EXIT
#undef CHK_EXIT
		remaining = data->totalSize - dled - from;
		data->index += dlnext;

		dlnext = BUFSIZE > remaining ? remaining : BUFSIZE;
		svcSignalEvent(data->eventHandle);
	}

	svcSignalEvent(data->eventHandle);
	httpcCloseContext(pctx);
	return 0;
#undef CHECKRET
}

static void i_install_loop_thread_cb(Result& res, get_url_func get_url, cia_net_data& data, httpcContext& hctx)
{
	std::string url;

	if(!get_settings()->resumeDownloads)
	{
		if((url = get_url(res)) == "")
		{
			elog("failed to fetch url: %08lX", res);
			return;
		}
		res = i_install_net_cia(url, &data, 0, &hctx);
		return;
	}

	// install loop
	while(true)
	{
		url = get_url(res);
		if(R_SUCCEEDED(res))
			res = i_install_net_cia(url, &data, data.index, &hctx);

		// User pressed start
		if(data.itc == ITC::exit)
		{
			res = APPERR_CANCELLED;
			break;
		}

		if(R_FAILED(res)) { elog("Failed in install loop. ErrCode=0x%08lX", res); }
		if(R_MODULE(res) == RM_HTTP)
		{
			ilog("timeout. ui::timeoutscreen() is up.");
			// Does the user want to stop?

			data.itc = ITC::timeoutscr;
			if(ui::timeoutscreen(PSTRING(netcon_lost, "0x" + pad8code(res)), 10))
			{
				// this signals that we want to cancel the installation later on
				res = APPERR_CANCELLED;
				data.itc = ITC::exit;
				break;
			}

			data.itc = ITC::normal;
			/* we want to redraw screen */
			svcSignalEvent(data.eventHandle);
			continue;
		}

		// Installation was a fail or succeeded, so we stop
		break;
	}
}

static Result i_install_resume_loop(get_url_func get_url, Handle ciaHandle, prog_func prog)
{
	Result res;
	cia_net_data data;
	data.buffer = new u8[BUFSIZE];
	data.cia = ciaHandle;
	if(R_FAILED(res = svcCreateEvent(&data.eventHandle, RESET_STICKY)))
		return res;

	httpcContext hctx;

	// Install thread
	ctr::thread<Result&, get_url_func, cia_net_data&, httpcContext&> th
		(i_install_loop_thread_cb, res, get_url, data, hctx);

	// UI Loop
	while(!th.finished())
	{
		if(data.itc != ITC::timeoutscr)
		{
			if(svcWaitSynchronization(data.eventHandle, 0) == 0)
			{
				/* draw & clear event */
				svcClearEvent(data.eventHandle);
				prog(data.index, data.totalSize);
			}

			hidScanInput();
			/* perhaps make this entire loop event-based?
			 * extern Handle hidEvents[5];
			 * svcWaitSynchronizationN() */
			if(!aptMainLoop() || (hidKeysDown() & (KEY_B | KEY_START)))
			{
				httpcCancelConnection(&hctx); /* aborts if in http  */
				data.itc = ITC::exit;
				break;
			}
			gspWaitForVBlank();
		}
		else /* wait until timeout is done but don't clear event; we have to draw immediately after */
			svcWaitSynchronization(data.eventHandle, U64_MAX);
	}

	th.join();
	svcCloseHandle(data.eventHandle);

	delete [] data.buffer;
	return res;
}

static Result i_install_hs_cia(const hsapi::FullTitle& meta, prog_func prog, bool reinstallable)
{
	ctr::Destination media = ctr::detect_dest(meta.tid);
	u64 freeSpace = 0;
	Result res;

	if(R_FAILED(res = ctr::get_free_space(media, &freeSpace)))
		return res;

	if(meta.size > freeSpace)
		return APPERR_NOSPACE;

	// Check if we are NOT on a n3ds and the game is n3ds exclusive
	bool isNew = false;
	if(R_FAILED(res = APT_CheckNew3DS(&isNew)))
		return res;

	if(!isNew && meta.prod.rfind("KTR-", 0) == 0)
		return APPERR_NOSUPPORT;

	return install::net_cia([meta](Result& res) -> std::string {
		std::string ret;
		if(R_FAILED(res = hsapi::get_download_link(ret, meta)))
			return "";
		return ret;
	}, meta.tid, prog, reinstallable);
}


// public api

Result install::net_cia(get_url_func get_url, hsapi::htid tid, prog_func prog, bool reinstallable)
{
	FS_MediaType dest = ctr::mediatype_of(tid);
	Result ret;
	if(reinstallable)
	{
		// Ask ninty why this stupid restriction is in place
		// Basically reinstalling the CURRENT cia requires you
		// To NOT delete the cia but instead have a higher version
		// and just install like normal
		FS_MediaType selfmt;
		u64 selftid;
		if(R_FAILED(ret = APT_GetAppletInfo((NS_APPID) envGetAptAppId(), &selftid, (u8 *) &selfmt, nullptr, nullptr, nullptr)))
			return ret;
		if(envIsHomebrew() || selftid != tid || dest != selfmt)
		{
			if(R_FAILED(ret = ctr::delete_if_exist(tid)))
				return ret;
		}
	}

	else
	{
		if(ctr::title_exists(tid))
			return APPERR_NOREINSTALL;
	}

	Handle cia;
	ret = AM_StartCiaInstall(dest, &cia);
	ilog("AM_StartCiaInstall(...): 0x%08lX", ret);
	if(R_FAILED(ret)) return ret;

	aptSetHomeAllowed(false);
	float oldrate = C3D_FrameRate(2.0f);
	ret = i_install_resume_loop(get_url, cia, prog);
	C3D_FrameRate(oldrate);
	aptSetHomeAllowed(true);
	if(R_FAILED(ret))
	{
		AM_CancelCIAInstall(cia);
		svcCloseHandle(cia);
		return ret;
	}

	ret = AM_FinishCiaInstall(cia);
	svcCloseHandle(cia);
	ilog("AM_FinishCiaInstall(...): 0x%08lX", ret);

	return ret;
}

Result install::hs_cia(const hsapi::FullTitle& meta, prog_func prog, bool reinstallable)
{
	return i_install_hs_cia(meta, prog, reinstallable);
}

// HTTPC

// https://3dbrew.org/wiki/HTTPC:SetProxy
Result httpcSetProxy(httpcContext *context, u16 port, u32 proxylen, const char *proxy,
	u32 usernamelen, const char *username, u32 passwordlen, const char *password)
{
	u32 *cmdbuf = getThreadCommandBuffer();

	cmdbuf[0]  = IPC_MakeHeader(0x000D, 0x5, 0x6); // 0x000D0146
	cmdbuf[1]  = context->httphandle;
	cmdbuf[2]  = proxylen;
	cmdbuf[3]  = port & 0xFFFF;
	cmdbuf[4]  = usernamelen;
	cmdbuf[5]  = passwordlen;
	cmdbuf[6]  = (proxylen << 14) | 0x2;
	cmdbuf[7]  = (u32) proxy;
	cmdbuf[8]  = (usernamelen << 14) | 0x402;
	cmdbuf[9]  = (u32) username;
	cmdbuf[10] = (passwordlen << 14) | 0x802;
	cmdbuf[11] = (u32) password;

	Result ret = 0;
	if(R_FAILED(ret = svcSendSyncRequest(context->servhandle)))
		return ret;

	return cmdbuf[1];
}

