
#include "settings.hh"
#include "install.hh"
#include "thread.hh"
#include "update.hh" /* includes net constants */
#include "error.hh"
#include "proxy.hh"
#include "panic.hh"
#include "seed.hh"
#include "ctr.hh"

#include <3rd/log.hh>
#include <malloc.h>
#include <3ds.h>

#define CHECKRET(expr) if(R_FAILED( res = ( expr ) )) return res
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
} cia_net_data;


static Result i_install_net_cia(std::string url, cia_net_data *data, size_t from)
{
	httpcContext ctx;
	Result res = 0;

	/* configure */
	CHECKRET(httpcOpenContext(&ctx, HTTPC_METHOD_GET, url.c_str(), 0));
	CHECKRET(httpcSetSSLOpt(&ctx, SSLCOPT_DisableVerify));
	CHECKRET(httpcSetKeepAlive(&ctx, HTTPC_KEEPALIVE_ENABLED));
	CHECKRET(httpcAddRequestHeaderField(&ctx, "Connection", "Keep-Alive"));
	CHECKRET(httpcAddRequestHeaderField(&ctx, "User-Agent", USER_AGENT));
	CHECKRET(proxy::apply(&ctx));

	if(from != 0)
	{
		CHECKRET(httpcAddRequestHeaderField(&ctx, "Range", ("bytes=" + std::to_string(from) + "-").c_str()));
	}

	CHECKRET(httpcBeginRequest(&ctx));

	u32 status = 0, dled = from;
	CHECKRET(httpcGetResponseStatusCode(&ctx, &status));
	lverbose << "Download status code: " << status;

	// Do we want to redirect?
	if(status / 100 == 3)
	{
		char newurl[2048];
		httpcGetResponseHeader(&ctx, "location", newurl, 2048);
		std::string redir(newurl);

		lverbose << "Redirected to " << redir;
		httpcCloseContext(&ctx);
		return i_install_net_cia(redir, data, from);
	}

	// Are we resuming and does the server support range?
	if(from != 0 && status != 206)
		return APPERR_NORANGE;

	if(from == 0)
		CHECKRET(httpcGetDownloadSizeState(&ctx, nullptr, &data->totalSize));
	if(data->totalSize == 0)
		return APPERR_NOSIZE;

	// Install.
	u32 remaining = data->totalSize - from;
	u32 dlnext = BUFSIZE > remaining ? remaining : BUFSIZE;
	u32 written = 0;

	while(data->index != data->totalSize)
	{
		if((R_FAILED(res = httpcReceiveDataTimeout(&ctx, data->buffer, dlnext, 30000000000L))
			&& res != (Result) HTTPC_RESULTCODE_DOWNLOADPENDING)
			|| R_FAILED(res = httpcGetDownloadSizeState(&ctx, &dled, nullptr)))
		{
			// Error
			httpcCancelConnection(&ctx);
			httpcCloseContext(&ctx);
			return res;
		}

		if(data->itc == ITC::exit)
			break;

		lverbose << "Writing to cia handle, size=" << dlnext << ",index=" << data->index << ",totalSize=" << data->totalSize;
		CHECKRET(FSFILE_Write(data->cia, &written, data->index, data->buffer, dlnext, FS_WRITE_FLUSH));

		remaining = data->totalSize - dled - from;
		data->index += dlnext;

		dlnext = BUFSIZE > remaining ? remaining : BUFSIZE;
	}

	return 0;
}

static void i_install_loop_thread_cb(Result& res, get_url_func get_url, cia_net_data& data)
{
	std::string url = get_url(res);

	if(!get_settings()->resumeDownloads)
	{
		res = i_install_net_cia(url, &data, 0);
		return;
	}

	// install loop
	while(true)
	{
		if(url != "") // url == "" means we failed to fetch the url
			res = i_install_net_cia(get_url(res), &data, data.index);

		// User pressed start
		if(data.itc == ITC::exit)
		{
			res = APPERR_CANCELLED;
			break;
		}

		if(R_FAILED(res)) { lerror << "Failed in install loop. ErrCode=0x" << std::hex << res; }
		if(R_MODULE(res) == RM_HTTP)
		{
			llog << "timeout. ui::timeoutscreen() is up.";
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
			url = get_url(res);
			continue;
		}

		// Installation was a fail, so we stop
		break;
	}
}

static Result i_install_resume_loop(get_url_func get_url, Handle ciaHandle, prog_func prog)
{
	cia_net_data data;
	data.buffer = new u8[BUFSIZE];
	data.cia = ciaHandle;

	Result res = 0;

	// Install thread
	ctr::thread<Result&, get_url_func, cia_net_data&> th
		(i_install_loop_thread_cb, res, get_url, data);

	// UI Loop
	u64 prev = 0;
	while(!th.finished())
	{
		if(data.itc != ITC::timeoutscr)
		{
			if(data.index != prev)
			{
				prog(data.index, data.totalSize);
				prev = data.index;
			}

			hidScanInput();
			if(hidKeysDown() & (KEY_B | KEY_START))
			{
				data.itc = ITC::exit;
				break;
			}
		}
	}

	th.join();

	delete [] data.buffer;
	return res;
}


static Destination detect_dest(const hsapi::Title& meta)
{ return detect_dest(meta.tid); }

// https://www.3dbrew.org/wiki/Titles#Title_IDs
Destination detect_dest(u64 tid)
{
	u16 cat = (tid >> 32) & 0xFFFF;
	// Install on nand on (DlpChild, System, TWL), else install on SD
	return (cat & (0x1 | 0x10 | 0x8000))
		? (cat & 0x8000 ? DEST_TWLNand : DEST_CTRNand)
		: DEST_Sdmc;
}

FS_MediaType to_mediatype(Destination dest)
{
	return dest == DEST_Sdmc ? MEDIATYPE_SD : MEDIATYPE_NAND;
}

static Result i_install_hs_cia(const hsapi::FullTitle& meta, prog_func prog, bool reinstallable)
{
	Destination media = detect_dest(meta);
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
	if(reinstallable)
	{
		// Ask ninty why this stupid restriction is in place
		// Basically reinstalling the CURRENT cia requires you
		// To NOT delete the cia but instead have a higher version
		// and just install like normal
		u64 selftid = 0;
		if(!(R_FAILED(APT_GetProgramID(&selftid)) || selftid == tid))
		{
			Result res = 0;
			if(R_FAILED(res = ctr::delete_if_exist(tid)))
				return res;
		}
	}

	else
	{
		if(ctr::title_exists(tid))
			return APPERR_NOREINSTALL;
	}

	Handle cia; Result ret;
	ret = AM_StartCiaInstall(detect_media(tid), &cia);
	linfo << "AM_StartCiaInstall(...): " << ret;
	if(R_FAILED(ret)) return ret;

	ret = i_install_resume_loop(get_url, cia, prog);
	if(R_FAILED(ret))
	{
		AM_CancelCIAInstall(cia);
		return ret;
	}

	ret = AM_FinishCiaInstall(cia);
	linfo << "AM_FinishCiaInstall(...): " << ret;

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

