
#include "install.hh"

#include <net_common.hh>
#include <curl/curl.h>
#include <3rd/log.hh>
#include <malloc.h>

static LightLock g_mtx;


static Result write_to_fd(game::__cb_data *cdata, void *data, u32 *write, size_t size, u32 flags = FS_WRITE_FLUSH)
{
	return FSFILE_Write(cdata->handle, write, cdata->offset, data, size, FS_WRITE_FLUSH);
}

/*static void __sum(game::__cb_data *data, u32 written, u32 size, u32 nmemb, Result res)
{
	lverbose << "Write summary";
	lverbose << "====================";
	lverbose << "data->handle : " << data->handle;
	lverbose << "written      : " << written;
	lverbose << "data->offset : " << data->offset;
	lverbose << "size * nmemb : " << size * nmemb;
	lverbose << "result       : " << std::hex << res;
	lverbose << "====================";
}*/

static size_t curl_install_cia_callback(char *ptr, size_t size, size_t nmemb, void *userdata)
{
	game::__cb_data *data = (game::__cb_data *) userdata;
	size = size * nmemb; // Don't want to write this the entire time...
	u32 written = 0;
	u32 doff = 0;
	Result res;

	// We can write from the buffer
	if(data->bufsiz + size > DBUFSZ)
	{
		char *write = new char[DBUFSZ];
		memcpy(write, data->databuf, data->bufsiz);
		memcpy(write + data->bufsiz, ptr, DBUFSZ - data->bufsiz);

		size -= DBUFSZ - data->bufsiz;
		doff = DBUFSZ - data->bufsiz;
		data->bufsiz = 0;

		if(R_FAILED(res = write_to_fd(data, write, &written, DBUFSZ)))
		{
			lerror << "Failed writing(0): " << std::hex << res;
			return 0;
		}

		delete [] write;
	}

	// Split it up
	for(size_t i = 0; i < size / DBUFSZ; ++i)
	{
		if(R_FAILED(write_to_fd(data, ptr + doff, &written, DBUFSZ)))
		{
			lerror << "Failed writing(1): " << std::hex << res;
			return 0;
		}
		size -= written;
		doff += written;
		written = 0;
	}

	// Copy rest in buffer
	memcpy(data->databuf + data->bufsiz, ptr + doff, size);
	data->bufsiz += size;
	return size * nmemb;
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
	CURL *curl = curl_easy_init();

	struct curl_slist *headers = NULL;
	headers = curl_slist_append(headers, CURL_UA);
	if(from != 0 && to != 0)
	{
		headers = curl_slist_append(headers, (std::string("Range: bytes=") + std::to_string(from)
			+ "-" + std::to_string(to)).c_str());
	}
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

	game::__cb_data data;
	data.handle = ciaHandle;
	data.offset = from;

	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_install_cia_callback);
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

