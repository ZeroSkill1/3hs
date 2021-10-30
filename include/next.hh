
#ifndef inc_next_hh
#define inc_next_hh

#include "hsapi.hh"

#include <string>

#define next_cat_exit ((const std::string *) 0)
#define next_sub_back ((const std::string *) 0)
#define next_sub_exit ((const std::string *) 1)

#define next_gam_back ((hsapi::hid) -1)
#define next_gam_exit ((hsapi::hid) -2)


namespace next
{
	const std::string *sel_cat();
	const std::string *sel_sub(const std::string& cat);
	hsapi::hid sel_gam(std::vector<hsapi::Title>& titles);
}

#endif

