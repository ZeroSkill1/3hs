
#include "game.hh"

#include <curl/curl.h>
#include <3rd/log.hh>

static LightLock g_mtx;


void game::game(int id)
{
	/* TODO: Implement game downloading/installing logic here */
}


static size_t curl_install_cia_callback(char *ptr, size_t size, size_t nmemb, void *userdata)
{
	game::__cb_data *data = (game::__cb_data *) userdata;
	LightLock_Lock(&g_mtx);

	lverbose << "Got " << (nmemb * size) << "b of title data, curr offset = " << data->offset;

	u32 written = 0;
	u32 rwrite = 0;
	// Write until everything is written;
	// We only get one chance to use this data
	do
	{
		lverbose << "Gonna write " << size * nmemb << " of title data";
		FSFILE_Write(data->handle, &written, data->offset, ptr, size * nmemb, FS_WRITE_FLUSH);
		data->offset += written;
		rwrite += written;
	} while (rwrite < size * nmemb);
	lverbose << "rwrite(" << rwrite << ") < size * nmemb (" << size * nmemb << ")";

	LightLock_Unlock(&g_mtx);
	return size * nmemb;
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
