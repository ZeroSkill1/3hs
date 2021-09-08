
#ifndef inc_proxy_h
#define inc_proxy_h

#include <curl/curl.h>


namespace proxy
{
	void apply(CURL *curl);
	void init();
}


#endif

