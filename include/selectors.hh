
#ifndef __cats_hh__
#define __cats_hh__

#include <ui/core.hh>
#include <ui/list.hh>

#include <hs.hh>

#include <string>

#define NUMTHREADS 2


namespace sel
{
	// So g++ doesn't give a warning that i
	// Can't assign the enum to an int, i
	// can bitch
	namespace Results
	{
		enum Results
		{
			cancel = 0,
			exit = -1,
			cat_nullptr = -2, /* unused */
			prev = -3,
		};
	}

	bool game(std::string cat, std::string subcat, long& id);
	bool subcat(hs::Index& indx, hs::Category cat, long& id);
	/* Returns -x when exit is needed, 0 for NO_GAME = cancel */
	long cat(hs::Index& indx);

	void game(hs::id_t id);
}

#endif
