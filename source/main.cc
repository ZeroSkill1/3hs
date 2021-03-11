
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <3ds.h>
#include <stdlib.h>
#include <malloc.h>

#include <iostream>
#include <curl/curl.h>

#include <ui/core.hh>
#include <ui/list.hh>

#include <hs.hh>


#define STACKSIZE (4 * 1024)

#define URL "http://download3.erista.me/games/china/0004000000119300.cia"
#define MTT
#undef MTT

size_t stub(char *ptr, size_t size, size_t nmemb, void *userdata)
{
	return size * nmemb;
}


void thread_2()
{
	CURL *curl = curl_easy_init();
	struct curl_slist *headers = NULL;
	headers = curl_slist_append(headers, "Range: bytes=0-104857600");

	curl_easy_setopt(curl, CURLOPT_URL, URL);
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, stub);
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

	CURLcode res = curl_easy_perform(curl);
	curl_slist_free_all(headers);
	curl_easy_cleanup(curl);

	if(res != CURLE_OK)
	{
		printf("Thread_2: %s\n", curl_easy_strerror(res));
	}
}

int main(int argc, char* argv[])
{
	gfxInitDefault();
	consoleInit(GFX_TOP, NULL);
	ui::global_init();
	hs::global_init();
	romfsInit();

	ui::clear(ui::Scr::bottom);

	hs::Index indx = hs::Index::get();
	if(!index_failed(indx))
		std::cout << "hShop has " << indx.totalTitles << " titles!" << std::endl;
	else std::cout << "Error: " << index_error(indx) << std::endl;


	ui::Widgets wids;
	ui::List<std::string> listWid(
		[](std::string& other) { return other; },
		[&](size_t index) -> void {
		}
	); listWid.name("main_list");

	for(hs::Category& cat : indx.categories)
		listWid.append(cat.displayName);

	wids.push_back(listWid);

	ui::Keys keys;
	while(ui::framenext(keys))
	{
		ui::framedraw(wids, keys);
	}



	hs::global_deinit();
	ui::global_deinit();

	return 0;
}
