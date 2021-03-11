
#ifndef __cats_hh__
#define __cats_hh__

#include <ui/core.hh>
#include <ui/list.hh>

#include <hs.hh>

#include <string>



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
			cat_nullptr = -2,
		};
	}

	bool game(std::string cat, std::string subcat, int& id);
	bool subcat(hs::Index& indx, std::string cat, int& id);
	/* Returns -x when exit is needed, 0 for NO_GAME = cancel */
	int cat(hs::Index& indx);
}

#endif
