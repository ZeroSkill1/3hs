
#ifndef __game_hh__
#define __game_hh__

#include <string>
#include <3ds.h>


namespace game
{
	typedef struct __cb_data
	{
		Handle handle;
		u64 offset;
	} __cb_data;

	void single_install_thread(std::string from, Handle cia, size_t offset = 0, size_t to = 0);
	void clean_mutex();
	void start_mutex();
}

#endif
