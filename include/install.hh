
#ifndef inc_game_hh
#define inc_game_hh

#include <functional>
#include <string>
#include <hs.hh>
#include <3ds.h>

// 128 KiB
#define CIA_NET_BUFSIZE 0x20000

#define NET_CURL_ERR(r) ((r) > 0)
#define NET_3DS_ERR(r) ((r) < 0)
#define NET_OK(r) ((r) == 0)

enum Destination
{
	DEST_CTRNand, DEST_TWLNand,
	DEST_Sdmc,
};

typedef std::function<void(u64 /* done */, u64 /* total */)> prog_func;
typedef std::function<std::string()> get_url_func;
static void default_prog_func(u64, u64)
{ }

static inline get_url_func makeurlwrap(const std::string& url)
{
	return [url]() -> std::string {
		return url;
	};
}


Destination detect_dest(const std::string& tid);
FS_MediaType to_mediatype(Destination dest);

Result delete_if_exist(u64 tid, FS_MediaType media = MEDIATYPE_SD);
Result delete_title(u64 tid, FS_MediaType media = MEDIATYPE_SD);
bool title_exists(u64 tid, FS_MediaType media = MEDIATYPE_SD);

std::string tid_to_str(u64 tid);
u64 str_to_tid(std::string tid);

Result install_net_cia(get_url_func get_url, prog_func prog = default_prog_func, bool reinstallable = false, std::string tid = "", FS_MediaType dest = MEDIATYPE_SD);
Result install_net_cia(get_url_func get_url, prog_func prog = default_prog_func, bool reinstallable = false, u64 tid = 0x0, FS_MediaType dest = MEDIATYPE_SD);
Result install_hs_cia(hs::FullTitle *meta, prog_func prog = default_prog_func, bool reinstallable = false);

#endif

