
#include "install.hh"

#include <curl/curl.h>
#include <3rd/log.hh>

static LightLock g_mtx;


static size_t curl_install_cia_callback(char *ptr, size_t size, size_t nmemb, void *userdata)
{
	game::__cb_data *data = (game::__cb_data *) userdata;
	LightLock_Lock(&g_mtx);

	u32 written = 0;
	u32 rwrite = 0;
	// Write until everything is written;
	// We only get one chance to use this data
	do
	{
		FSFILE_Write(data->handle, &written, data->offset, ptr, size * nmemb, FS_WRITE_FLUSH);
		data->offset += written;
		rwrite += written;
	} while (rwrite < size * nmemb);

	LightLock_Unlock(&g_mtx);
	return rwrite;
}

void game::start_mutex()
{
	LightLock_Init(&g_mtx);
}

void game::clean_mutex()
{
	close(g_mtx);
}

void game::single_install_thread(std::string from, Handle ciaHandle, size_t offset, size_t to)
{
	CURL *curl = curl_easy_init();

	struct curl_slist *headers = NULL;
	if(offset != 0 && to != 0)
	{
		curl_slist_append(headers, (std::string("Range: bytes=") + std::to_string(offset)
			+ "-" + std::to_string(to)).c_str());
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
	}

	linfo << "Downloading/installing from " << offset << "b to " << to << "b";

	game::__cb_data data;
	data.handle = ciaHandle;
	data.offset = offset;

	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_install_cia_callback);
	curl_easy_setopt(curl, CURLOPT_URL, from.c_str());
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);

	curl_easy_perform(curl);
	if(headers != NULL) curl_slist_free_all(headers);
	curl_easy_cleanup(curl);
}

void game::balance_range(const hs::Title *meta, game::balance_t *rrange, size_t size)
{
	size_t *ranges = new size_t[size];
	for(size_t i = 0; i < size; ++i)
	{
		size_t now = (meta->size / size) * (i + 1);
		lverbose << "Now[" << i <<"]: " << now << ", meta->size: " << meta->size
			<< ", size: " << size;
		ranges[i] = now;
	}

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
	game::balance_t *balance = (game::balance_t *) arg;
	size_t from, to; std::tie(from, to) = *balance;
	llog << "NEW THREAD";
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

	s32 thpr = 0; svcGetThreadPriority(&thpr, CUR_THREAD_HANDLE);
	for(size_t i = 0; i < threads - 1; ++i)
		rthreads[i] = threadCreate(install_cia_thread, (void *) &balance[i + 1], 0, thpr - 1, -2, false);

	/* TODO: Create a struct to pass on to all threads to ensure there is no
			Race condition here (time taken > U64_MAX), make the main thread (this one!)
			Write to a progress bar while waiting */

	// Ensure thread is finished (it should already be at this point) + free
	for(size_t i = 0; i < threads; ++i)
	{
		threadJoin(rthreads[i], U64_MAX);
		threadFree(rthreads[i]);
	}	

	// i think this is unnecessairy?
	// delete [] rthreads;
	delete [] balance;
}

