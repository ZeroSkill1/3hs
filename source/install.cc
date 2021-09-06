
#include "settings.hh"
#include "install.hh"
#include "error.hh"
#include "seed.hh"

#include <widgets/indicators.hh>
#include <net_common.hh>
#include <curl/curl.h>
#include <3rd/log.hh>
#include <ui/util.hh>
#include <malloc.h>
#include <3ds.h>


typedef struct cia_net_data
{
	// Last error
	Result err = 0;
	// We write to this handle
	Handle cia;
	// How many bytes are in this->buffer?
	u32 bufSize = 0;
	// Temp store for leftovers
	// allocated on heap for more storage
	u8 *buffer = nullptr;
	// At what index are we writing __the cia__ now?
	u64 index = 0;
	// Progress callback
	prog_func prog = [](u64, u64) -> void { };
	// Total cia size
	u64 totalSize = 0;
	// CURL handle
	CURL *curl = nullptr;
} cia_net_data;


static Result write_cia_data(cia_net_data *cdata, u32 *written, u32 flags = FS_WRITE_FLUSH)
{
	return FSFILE_Write(cdata->cia, written, cdata->index, cdata->buffer, cdata->bufSize, flags);
}

static u64 get_curl_total_size(CURL *curl)
{
	u64 ret = 0;
	curl_easy_getinfo(curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD_T, &ret);
	return ret;
}

static size_t curl_install_cia_callback(char *ptr, size_t size, size_t nmemb, void *userdata)
{
	cia_net_data *udata = (cia_net_data *) userdata;
	size_t rsize = size * nmemb;
	size_t avail = rsize; // This will decrement
	size_t srcPos = 0; // From where in `ptr` do we want to copy?

	// Get total size
	if(udata->totalSize == 0) udata->totalSize = get_curl_total_size(udata->curl);

	// We need to split it up in chunks ...
	while(avail != 0)
	{
		// How many bytes do we still need to write to the buffer to get it completed?
		size_t remainder = CIA_NET_BUFSIZE - udata->bufSize;
		// Remainder = 100
		// available = 200
		// First iteration: write remainding 100 bytes, available-=100, clear buffer
		// Second iteration: write 100 bytes to buffer and do nothing after
		size_t cpySize = remainder > avail ? avail : remainder;

		// Copy the remainder in, if we don't have enough to fill it in completely
		// Just fill what we can
		memcpy(udata->buffer + udata->bufSize, ptr + srcPos, cpySize);
		udata->bufSize += cpySize;
		srcPos += cpySize;
		avail -= cpySize;

		// Hey! we need to clear the buffer
		if(udata->bufSize == CIA_NET_BUFSIZE)
		{
			// How many bytes did we write? Should always be CIA_NET_BUFSIZE
			u32 written = 0; Result res = write_cia_data(udata, &written);

			if(R_FAILED(res))
			{
				udata->err = res;
				return 0;
			}

			udata->index += udata->bufSize; // = CIA_NET_BUFSIZE
			udata->bufSize = 0;

			// Exec progress callback here
			udata->prog(udata->index, udata->totalSize);
		}
	}

	// Success!
	return rsize;
}

static int progress_cb(void *, curl_off_t, curl_off_t, curl_off_t, curl_off_t)
{
	hidScanInput(); return (hidKeysDown() | hidKeysHeld()) & (KEY_B | KEY_START);
}

static Result i_install_net_cia(std::string url, cia_net_data *data, size_t from)
{
	static_assert(CIA_NET_BUFSIZE % 64 == 0, "$CIA_NET_BUFSIZE must be dividable by 64");
	CURL *curl = curl_easy_init();

	data->curl = curl;

	struct curl_slist *headers = NULL;
	headers = curl_slist_append(headers, CURL_UA);
	// = We want to contintue
	if(from != 0)
	{
		headers = curl_slist_append(headers, (std::string("Range: bytes=") + std::to_string(from)
			+ "-").c_str());
	}

	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_install_cia_callback);

	curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, progress_cb);
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, data);
	curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
	curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 3);

	CURLcode res = curl_easy_perform(curl);
	curl_easy_cleanup(curl);

	// If we have an error here, a potentional curl error will
	// most likely be a reaction to this error, so its more imporant
	if(data->err != 0)
	{
		lerror << "FSFILE_Write(...): " << std::hex << data->err;
		return data->err;
	}

	if(res != CURLE_OK)
	{
		lerror << "curl_easy_perform(...): " << res;
		return (Result) res;
	}

	// One final write...
	if(data->bufSize != 0)
	{
		u32 written = 0;
		Result sres = write_cia_data(data, &written);

		if(R_FAILED(sres))
		{
			lerror << "FSFILE_Write(...): " << std::hex << sres;
			return sres;
		}
	}

	return 0;
}

static Result i_install_resume_loop(std::function<std::string()> get_url, Handle ciaHandle, prog_func prog)
{
	cia_net_data data;
	data.buffer = new u8[CIA_NET_BUFSIZE];
	data.cia = ciaHandle;
	data.prog = prog;

	std::string url = get_url();
	linfo << "Installing cia from <" << url << ">.";

	Result res = 0;

	if(!get_settings()->resumeDownloads)
	{
		res = i_install_net_cia(url, &data, 0);
		delete [] data.buffer;
		return res;
	}

	// install loop
	while(true)
	{
		if(url != "") // url == "" means we failed to fetch the url 
			res = i_install_net_cia(get_url(), &data, data.index + data.bufSize);
		if(res > 0 /* curl error */ && res != CURLE_ABORTED_BY_CALLBACK)
		{
			// Does the user want to stop?
			if(ui::timeoutscreen(STRING(netcon_lost), 10))
			{
				// this signals that we want to cancel the installation later on
				res = CURLE_ABORTED_BY_CALLBACK;
				break;
			}
			url = get_url();
			continue;
		}
		break;
	}

	delete [] data.buffer;
	return res;
}


static Destination detect_dest(hs::Title *meta)
{ return detect_dest(meta->tid); }

// https://www.3dbrew.org/wiki/Titles#Title_IDs
Destination detect_dest(const std::string& tid)
{
	u16 cat = std::stoul(tid.substr(4, 4), nullptr, 16);
	// Install on nand on (DlpChild, System, TWL), else install on SD
	return (cat & (0x1 | 0x10 | 0x8000))
		? (cat & 0x8000 ? DEST_TWLNand : DEST_CTRNand)
		: DEST_Sdmc;
}

FS_MediaType to_mediatype(Destination dest)
{
	return dest == DEST_Sdmc ? MEDIATYPE_SD : MEDIATYPE_NAND;
}

static Result i_install_hs_cia(hs::FullTitle *meta, prog_func prog, bool reinstallable)
{
	Destination media = detect_dest(meta);
	u64 freeSpace = 0;
	Result res;

	if(R_FAILED(res = get_free_space(media, &freeSpace)))
		return res;

	if(meta->size > freeSpace)
		return APPERR_NOSPACE;

	// Check if we are NOT on a n3ds and the game is n3ds exclusive
	bool isNew = false;
	if(R_FAILED(res = APT_CheckNew3DS(&isNew)))
		return res;

	if(!isNew && meta->prod.rfind("KTR-", 0) == 0)
		return APPERR_NOSUPPORT;

	return install_net_cia([meta]() -> std::string { return hs::get_download_link(meta); }, prog, reinstallable, meta->tid, to_mediatype(media));
}


// public api

// This func returns true
// on failure because there are probably
// worse problems if it fails to query titles
bool title_exists(u64 tid, FS_MediaType media)
{
	u32 tcount = 0;

	if(R_FAILED(AM_GetTitleCount(media, &tcount)))
		return true;

	u64 *tids = new u64[tcount];
	if(R_FAILED(AM_GetTitleList(&tcount, MEDIATYPE_SD, tcount, tids)))
		goto fail;

	for(size_t i = 0; i < tcount; ++i)
	{
		if(tids[i] == tid)
			goto fail;
	}


	delete [] tids;
	return false;
fail:
	delete [] tids;
	return true;
}

Result delete_title(u64 tid, FS_MediaType media)
{
	Result ret = 0;

	if(R_FAILED(ret = AM_DeleteTitle(media, tid))) return ret;
	if(R_FAILED(ret = AM_DeleteTicket(tid))) return ret;

	// Reloads the databases
	if(media == MEDIATYPE_SD) ret = AM_QueryAvailableExternalTitleDatabase(NULL);
	return ret;
}

Result delete_if_exist(u64 tid, FS_MediaType media)
{
	if(title_exists(tid, media))
		return delete_title(tid, media);
	return 0;
}

std::string tid_to_str(u64 tid)
{
	if(tid == 0) return "";
	char buf[17]; snprintf(buf, 17, "%016llX", tid);
	return buf;
}

u64 str_to_tid(std::string tid)
{
	return strtoull(tid.c_str(), nullptr, 16);
}

Result install_net_cia(get_url_func get_url, prog_func prog, bool reinstallable, u64 tid, FS_MediaType dest)
{
	return install_net_cia(get_url, prog, reinstallable, tid_to_str(tid), dest);
}

Result install_net_cia(get_url_func get_url, prog_func prog, bool reinstallable, std::string tid, FS_MediaType dest)
{
	if(reinstallable && tid != "")
	{
		u64 itid = str_to_tid(tid);
		// Ask ninty why this stupid restriction is in place
		// Basically reinstalling the CURRENT cia requires you
		// To NOT delete the cia but instead have a higher version
		// and just install like normal
		u64 selftid = 0;
		if(!(R_FAILED(APT_GetProgramID(&selftid)) || selftid == itid))
		{
			Result res = 0;
			if(R_FAILED(res = delete_if_exist(itid)))
				return res;
		}
	}

	else if(tid != "")
	{
		u64 itid = str_to_tid(tid);
		if(title_exists(itid))
			return APPERR_NOREINSTALL;
	}

	Handle cia; Result ret;
	ret = AM_StartCiaInstall(dest, &cia); linfo << "AM_StartCiaInstall(...): " << ret;
	if(R_FAILED(ret)) return ret;

	ret = i_install_resume_loop(get_url, cia, prog);
	if(ret == CURLE_ABORTED_BY_CALLBACK)
	{ AM_CancelCIAInstall(cia); return APPERR_CANCELLED; }
	if(!NET_OK(ret)) // If everything went ok in i_install_resume_loop, we return a 0
	{ AM_CancelCIAInstall(cia); return ret; }

	ret = AM_FinishCiaInstall(cia);
	linfo << "AM_FinishCiaInstall(...): " << ret;
	return ret;
}

Result install_hs_cia(hs::FullTitle *meta, prog_func prog, bool reinstallable)
{
	return i_install_hs_cia(meta, prog, reinstallable);
}

