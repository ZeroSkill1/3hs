
#ifndef __next_hh__
#define __next_hh__

#include "hs.hh"

#include <string>

#define next_cat_exit ""

#define next_sub_back "1"
#define next_sub_exit "2"

#define next_gam_back -1
#define next_gam_exit -2

namespace next
{
	std::string sel_cat(hs::Index& index);
	std::string sel_sub(hs::Index& index, std::string cat);
	long int sel_gam(std::string cat, std::string sub);
}

#endif
