
#ifndef inc_proxy_h
#define inc_proxy_h

#include <string>
#include <3ds.h>


namespace proxy
{
	typedef struct Params
	{
		std::string host;
		u16 port = 0;

		std::string username;
		std::string password;
	} Params;

	Result apply(httpcContext *context);
	void init();

	bool validate(const Params& p);
	Params& proxy();
	bool is_set();
	void write();
	void clear();
}


#endif

