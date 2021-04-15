
#include "install.hh"

#include <net_common.hh>
#include <curl/curl.h>
#include <3rd/log.hh>
#include <malloc.h>

static LightLock g_mtx;


static Result write_to_fd(game::cia_net_data *cdata, u32 *written, u32 flags = FS_WRITE_FLUSH)
{
	return FSFILE_Write(cdata->cia, written, cdata->index, cdata->buffer, CIA_NET_BUFSIZE, flags);
}

static void __sum(game::cia_net_data *data, size_t size, size_t avail, u32 written, Result res)
{
	lverbose << "Write summary";
	lverbose << "====================";
	lverbose << "data->cia     : " << data->cia;
	lverbose << "rsize         : " << size;
	lverbose << "data->index   : " << data->index;
	lverbose << "data->bufSize : " << data->bufSize;
	lverbose << "available     : " << avail;
	lverbose << "written       : " << written;
	lverbose << "result        : " << std::hex << res;
	lverbose << "====================";
}

static size_t curl_install_cia_callback(char *ptr, size_t size, size_t nmemb, void *userdata)
{
		game::cia_net_data *udata = (game::cia_net_data *) userdata;
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

			lverbose << "Copying into udata->buffer " << cpySize << " bytes, srcPos=" << srcPos << ",bufSize=" << udata->bufSize;

			// Copy the remainder in, if we don't have enough to fill it in completely
			// Just fill what we can
			memcpy(udata->buffer + udata->bufSize, ptr + srcPos, cpySize);
			udata->bufSize += cpySize;
			avail -= cpySize;

			// Hey! we need to clear the buffer
			if(udata->bufSize == CIA_NET_BUFSIZE)
			{
				// How many bytes did we write? Should always be CIA_NET_BUFSIZE
				u32 written = 0; Result res = write_to_fd(udata, &written);
				__sum(udata, rsize, avail, written, res);

				if(R_FAILED(res))
				{
					lerror << "Failed writing data: " << std::hex << res;
					return 0;
				}

				udata->index += udata->bufSize; // = CIA_NET_BUFSIZE
				udata->bufSize = 0;
			}
		}

	// Success!
	return rsize;
}

int potential_exit_callback(void *clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow)
{
	hidScanInput();
	if(hidKeysDown() & KEY_B)
		return 1;
	return 0;
}

static size_t curl_install_cia_callback_safe(char *ptr, size_t size, size_t nmemb, void *userdata)
{
	LightLock_Lock(&g_mtx);
	size_t ret = curl_install_cia_callback(ptr, size, nmemb, userdata);
	LightLock_Unlock(&g_mtx);
	return ret;
}

void game::start_mutex()
{
	LightLock_Init(&g_mtx);
	// This shouldn't be necessairy, we'll do it anyways
	LightLock_Unlock(&g_mtx);
}

void game::clean_mutex()
{
	close(g_mtx);
}

void game::single_install_thread(std::string url, Handle ciaHandle, size_t from, size_t to)
{
	static_assert(CIA_NET_BUFSIZE % 64 == 0, "$CIA_NET_BUFSIZE must be divisable by 64");
	CURL *curl = curl_easy_init();

	struct curl_slist *headers = NULL;
	headers = curl_slist_append(headers, CURL_UA);
	if(from != 0 && to != 0)
	{
		headers = curl_slist_append(headers, (std::string("Range: bytes=") + std::to_string(from)
			+ "-" + std::to_string(to)).c_str());
	}
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

	game::cia_net_data data;
	data.cia = ciaHandle;

//	curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, potential_exit_callback);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_install_cia_callback);
	curl_easy_setopt(curl, CURLOPT_BUFFERSIZE, CIA_NET_BUFSIZE);
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);

	CURLcode res = curl_easy_perform(curl);
	if(res != CURLE_OK)
	{
		lerror << "Curl error: " << res;
	}
	curl_easy_cleanup(curl);
}

void game::balance_range(const hs::Title *meta, game::balance_t *rrange, size_t size)
{
	size_t *ranges = new size_t[size];
	for(size_t i = 0; i < size; ++i)
		ranges[i] = (meta->size / size) * (i + 1);

	rrange[0] = std::make_tuple(0, ranges[0]);

	for(size_t i = 1; i < size - 1; ++i)
		rrange[i] = std::make_tuple(ranges[i - 1] + 1, ranges[i]);

	rrange[size - 1] = std::make_tuple(ranges[size - 2] + 1, meta->size);

	/* rrange:
	 * Size: 3347
	 * 0 -> 1673
	 * 1674-> 3347
	 * * * * * * */
	delete [] ranges;
}

static void install_cia_thread(void *arg)
{
	game::__install_cia_thread_data *data = (game::__install_cia_thread_data *) arg;
	llog << "FROM: " << std::get<0>(data->balance) << "-" << std::get<1>(data->balance);

	game::single_install_thread(hs::get_download_link(data->meta), data->cia,
		std::get<0 /* from */>(data->balance), std::get<1 /* to */>(data->balance));
	delete data;
}

void game::single_thread_install(hs::Title *meta)
{
	/* TODO: Same as game::install_cia(...) */
	FS_MediaType dest = MEDIATYPE_SD;
	Handle cia;

	llog << "Start CIA res: " << std::hex << AM_StartCiaInstall(dest, &cia);
	lverbose << "cia handle: " << cia;

	game::single_install_thread(hs::get_download_link(meta), cia);

	llog << "Install res: " << std::hex << AM_FinishCiaInstall(cia);
}

void game::install_cia(const hs::Title *meta, size_t threads)
{
	game::balance_t *balance = new game::balance_t[threads];
	game::balance_range(meta, balance, threads);
	Thread *rthreads = new Thread[threads];

	IF_PLOG(plog::verbose)
	{
		for(size_t i = 0; i < threads; ++i)
		{
			lverbose << "Threads[" << i << "]: " << std::get<0>(balance[i])
				<< " -> " << std::get<1>(balance[i]);
		}
	}

	/* TODO: Get install location from cat (?), tid (?) */
	/* TODO: Test if we have all the required space */
	/* TODO: Cancel cia installation */
	FS_MediaType dest = MEDIATYPE_SD;
	Handle cia;

	AM_StartCiaInstall(dest, &cia);
	start_mutex();

	s32 thpr = 0; svcGetThreadPriority(&thpr, CUR_THREAD_HANDLE);
	for(size_t i = 0; i < threads; ++i)
	{
		// Prepare a struct to pass, gets freed in the thread, we can just make it a pointer so it doesn't auto delete
		game::__install_cia_thread_data *data = new game::__install_cia_thread_data;
		data->balance = balance[i + 1];
		data->written = 0;
		data->cia = cia;

		rthreads[i] = threadCreate(install_cia_thread, (void *) data, 0, thpr - 1, -2, false);
	}

	/* TODO: Create a struct to pass on to all threads to ensure there is no
			Race condition here (time taken > U64_MAX), make the main thread (this one!)
			Write to a progress bar while waiting */
	for(unsigned long long i = 1; i != 0; ++i); // Waits for i to overflow
	llog << "TIMER IS DONE.";

	// Ensure thread is finished (it should already be at this point) + free
	for(size_t i = 0; i < threads; ++i)
	{
		threadJoin(rthreads[i], U64_MAX);
		threadFree(rthreads[i]);
		linfo << "Killed thread " << i;
	}

	AM_FinishCiaInstall(cia);
	clean_mutex();

	// i think this is unnecessairy?
	// delete [] rthreads;
	delete [] balance;
}

