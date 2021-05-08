
#ifndef __game_hh__
#define __game_hh__

#include <string>
#include <hs.hh>
#include <3ds.h>

// 8KiB
#define CIA_NET_BUFSIZE 0x2000

#define NET_CURL_ERR(r) ((r) > 0)
#define NET_3DS_ERR(r) ((r) < 0)
#define NET_OK(r) ((r) == 0)


Result install_net_cia(std::string url, FS_MediaType dest = MEDIATYPE_SD);
Result install_hs_cia(hs::Title *meta);

#endif

