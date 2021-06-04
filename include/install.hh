
#ifndef inc_game_hh
#define inc_game_hh

#include <functional>
#include <string>
#include <hs.hh>
#include <3ds.h>

// 32 KiB
#define CIA_NET_BUFSIZE 0x10000 // 0x8000

#define NET_CURL_ERR(r) ((r) > 0)
#define NET_3DS_ERR(r) ((r) < 0)
#define NET_OK(r) ((r) == 0)

typedef std::function<void(u64 /* done */, u64 /* total */)> prog_func;
static void default_prog_func(u64, u64)
{ }

Result delete_if_exist(u64 tid, FS_MediaType media = MEDIATYPE_SD);
Result delete_title(u64 tid, FS_MediaType media = MEDIATYPE_SD);
bool title_exists(u64 tid, FS_MediaType media = MEDIATYPE_SD);

std::string tid_to_str(u64 tid);
u64 str_to_tid(std::string tid);

Result install_net_cia(std::string url, prog_func prog = default_prog_func, bool reinstallable = false, std::string tid = "", FS_MediaType dest = MEDIATYPE_SD);
Result install_net_cia(std::string url, prog_func prog = default_prog_func, bool reinstallable = false, u64 tid = 0x0, FS_MediaType dest = MEDIATYPE_SD);
Result install_hs_cia(hs::FullTitle *meta, prog_func prog = default_prog_func, bool reinstallable = false);

#endif

