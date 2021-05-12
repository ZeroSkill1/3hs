
#ifndef __game_hh__
#define __game_hh__

#include <functional>
#include <string>
#include <hs.hh>
#include <3ds.h>

// 8KiB
#define CIA_NET_BUFSIZE 0x4000

#define NET_CURL_ERR(r) ((r) > 0)
#define NET_3DS_ERR(r) ((r) < 0)
#define NET_OK(r) ((r) == 0)

typedef std::function<void(u64 /* done */, u64 /* total */)> prog_func;
static void default_prog_func(u64, u64)
{ }


Result install_net_cia(std::string url, prog_func prog = default_prog_func, bool reinstallable = false, std::string tid = "", FS_MediaType dest = MEDIATYPE_SD);
Result install_hs_cia(hs::FullTitle *meta, prog_func prog = default_prog_func, bool reinstallable = false);

#endif

