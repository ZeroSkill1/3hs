
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

Destination detect_dest(const std::string& tid);
Destination detect_dest(u64 tid);

FS_MediaType to_mediatype(Destination dest);

Result delete_if_exist(u64 tid, FS_MediaType media = MEDIATYPE_SD);
Result delete_title(u64 tid, FS_MediaType media = MEDIATYPE_SD);
bool title_exists(u64 tid, FS_MediaType media = MEDIATYPE_SD);

u64 str_to_tid(const std::string& tid);
std::string tid_to_str(u64 tid);

Result install_net_cia(get_url_func get_url, prog_func prog = default_prog_func, bool reinstallable = false, u64 tid = 0x0, FS_MediaType dest = MEDIATYPE_SD);
Result install_hs_cia(const hsapi::FullTitle& meta, prog_func prog = default_prog_func, bool reinstallable = false);

#endif

