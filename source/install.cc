
#include "install.hh"
#include "seed.hh"

#include <net_common.hh>
#include <curl/curl.h>
#include <3rd/log.hh>
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
	u8 buffer[CIA_NET_BUFSIZE];
	// At what index are we writing __the cia__ now?
	u32 index = 0;
} cia_net_data;


static Result write_cia_data(cia_net_data *cdata, u32 *written, u32 flags = FS_WRITE_FLUSH)
{
	return FSFILE_Write(cdata->cia, written, cdata->index, cdata->buffer, cdata->bufSize, flags);
}

static size_t curl_install_cia_callback(char *ptr, size_t size, size_t nmemb, void *userdata)
{
	cia_net_data *udata = (cia_net_data *) userdata;
	size_t rsize = size * nmemb;
	size_t avail = rsize; // This will decrement
	size_t srcPos = 0; // From where in `ptr` do we want to copy?

	// We need to split it up in chunks ...
	while(avail != 0)
	{
		// How many bytes do we still need to write to the buffer to get it completed?
		size_t remainder = CIA_NET_BUFSIZE - udata->bufSize;
		// Remainder = 100
		// available = 200
		// First round: write remainding 100 bytes, available-=100, clear buffer
		// Second round: write 100 bytes to buffer and do nothing after
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
				lerror << "Failed writing data: " << std::hex << res;
				udata->err = res;
				return 0;
			}

			udata->index += udata->bufSize; // = CIA_NET_BUFSIZE
			udata->bufSize = 0;
		}
	}

	// Success!
	return rsize;
}

static int potential_exit_callback(void *clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow)
{
	hidScanInput();
	return hidKeysDown() & (KEY_B | KEY_START)
}

static Result i_install_net_cia(std::string url, Handle ciaHandle, size_t from = 0, size_t to = 0)
{

	/* TODO: Get install location from cat (?), tid (?) */
	/* TODO: Test if we have all the required space */
	/* TODO: Cancel cia installation */

	static_assert(CIA_NET_BUFSIZE % 64 == 0, "$CIA_NET_BUFSIZE must be divisable by 64");
	CURL *curl = curl_easy_init();

	struct curl_slist *headers = NULL;
	headers = curl_slist_append(headers, CURL_UA);
	// = We want to contintue
	if(from != 0 && to != 0)
	{
		headers = curl_slist_append(headers, (std::string("Range: bytes=") + std::to_string(from)
			+ "-" + std::to_string(to)).c_str());
	}

	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_install_cia_callback);

	cia_net_data data;
	std::memset(data.buffer, 0x00, CIA_NET_BUFSIZE);
	data.cia = ciaHandle;

	curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, potential_exit_callback);
	curl_easy_setopt(curl, CURLOPT_BUFFERSIZE, CIA_NET_BUFSIZE);
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);
	curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0);

	CURLcode res = curl_easy_perform(curl);
	if(res != CURLE_OK)
	{
		lerror << "Curl error: " << res;
		return (Result) res;
	}
	curl_easy_cleanup(curl);

	// One final write...
	if(data.bufSize != 0)
	{
		u32 written = 0; Result res2 = write_cia_data(&data, &written);
		if(R_FAILED(res2))
		{
			lerror << "Failed writing data: " << std::hex << res;
			return res2;
		}
	}

	return 0;
}

static FS_MediaType detect_dest(hs::Title *meta)
{
	// TODO: Actually detect
	((void) meta);
	return MEDIATYPE_SD;
}

// public api

Result install_net_cia(std::string url, FS_MediaType dest)
{
	Handle cia = 0; Result ret;
	if(R_FAILED(ret = AM_StartCiaInstall(dest, &cia)))
		return ret;

	ret = i_install_net_cia(url, cia);
	if(ret == CURLE_ABORTED_BY_CALLBACK)
		return AM_CancelCiaInstall(cia);
	else if(ret != 0) // If everything went ok in i_install_net_cia, we return a 0
		return ret;

	return AM_FinishCiaInstall(cia);
}

Result install_hs_cia(hs::Title *meta)
{
	return install_net_cia(hs::get_download_link(meta), detect_dest(meta));
}

