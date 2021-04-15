
#ifndef __game_hh__
#define __game_hh__

#include <string>
#include <tuple>

#include <3ds.h>
#include <hs.hh>

// We could probably get away with less
#define STKSZ (4 * 1024)
// 2 MiB
#define CIA_NET_BUFSIZE 0x2000

namespace game
{
	typedef struct __cb_data
	{
		char databuf[CIA_NET_BUFSIZE];
		size_t bufsiz;

		Handle handle;
		u64 offset;
	} __cb_data;

	typedef std::tuple<size_t /* From */, size_t /* To */> balance_t;

	typedef struct __install_cia_thread_data
	{
		balance_t balance;
		hs::Title *meta;
		size_t written;
		Handle cia;
	} __install_cia_thread_data;

	typedef struct cia_net_data
	{
		// We write to this handle
		Handle cia;
		// How many bytes are in this->buffer?
		u32 bufSize = 0;
		// Temp store for leftovers
		u8 buffer[CIA_NET_BUFSIZE];
		// At what index are we writing __the cia__ now?
		u32 index = 0;
	} cia_net_data;

	void balance_range(const hs::Title *meta, game::balance_t *rrange, size_t size);
	void install_cia(const hs::Title *meta, size_t threads = 2);
	void single_thread_install(hs::Title *meta);

	void single_install_thread(std::string url, Handle ciaHandle, size_t from = 0, size_t to = 0);
	void clean_mutex();
	void start_mutex();
}

#endif

