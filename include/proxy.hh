
#ifndef inc_proxy_h
#define inc_proxy_h

#include <curl/curl.h>
#include <3ds.h>


namespace proxy
{
	Result apply(httpcContext *context);
	void apply(CURL *curl);
	void init();
}


#endif

