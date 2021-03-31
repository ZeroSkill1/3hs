
#ifndef __game_hh__
#define __game_hh__

#include <string>
#include <tuple>

#include <3ds.h>
#include <hs.hh>

// We could probably get away with less
#define STKSZ (4 * 1024)

namespace game
{
	typedef struct __cb_data
	{
		Handle handle;
		u64 offset;
	} __cb_data;

 
	typedef std::tuple<size_t /* From */, size_t /* To */> balance_t;

	typedef struct __install_cia_thread_data
	{
		balance_t balance;		
		size_t written;
	} __install_cia_thread_data;

	/* balance_t rrange = array */
	void balance_range(const hs::Title *meta, balance_t *rrange, size_t size);
	void install_cia(const hs::Title *meta, size_t threads = 2);

	void single_install_thread(std::string from, Handle cia, size_t offset = 0, size_t to = 0);
	void clean_mutex();
	void start_mutex();
}

#endif
