
#ifndef inc_next_hh
#define inc_next_hh

#include "hs.hh"

#include <string>

#define next_cat_exit ""

#define next_sub_back "1"
#define next_sub_exit "2"

#define next_gam_back -1
#define next_gam_exit -2

namespace next
{
	std::string sel_cat();
	std::string sel_sub(std::string cat);
	hs::shid sel_gam(std::vector<hs::Title>& titles);
}

#endif
