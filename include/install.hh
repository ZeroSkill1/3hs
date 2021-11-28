
#ifndef inc_game_hh
#define inc_game_hh

#include <functional>
#include <string>
#include <3ds.h>

#include "hsapi.hh"


enum Destination
{
	DEST_CTRNand, DEST_TWLNand,
	DEST_Sdmc,
};

typedef std::function<void(u64 /* done */, u64 /* total */)> prog_func;
typedef std::function<std::string(Result&)> get_url_func;
static void default_prog_func(u64, u64)
{ }

static inline get_url_func makeurlwrap(const std::string& url)
{
	return [url](Result& r) -> std::string {
		r = 0;
		return url;
	};
}


Result httpcSetProxy(httpcContext *context, u16 port, u32 proxylen, const char *proxy,
	u32 usernamelen, const char *username, u32 passwordlen, const char *password);

// C++ wrapper
static inline Result httpcSetProxy(httpcContext *context, u16 port,
	const std::string& proxy, const std::string& username, const std::string& password)
{
	return httpcSetProxy(
		context, port, proxy.size(), proxy.c_str(),
		username.size(), username.size() == 0 ? nullptr : username.c_str(),
		password.size(), password.size() == 0 ? nullptr : password.c_str()
	);
}

FS_MediaType to_mediatype(Destination dest);
Destination detect_dest(u64 tid);

static inline FS_MediaType detect_media(u64 tid)
{ return to_mediatype(detect_dest(tid)); }

namespace install
{
	Result net_cia(get_url_func get_url, u64 tid, prog_func prog = default_prog_func,
		bool reinstallable = false);
	Result hs_cia(const hsapi::FullTitle& meta, prog_func prog = default_prog_func,
		bool reinstallable = false);
}

#endif

