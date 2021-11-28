
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
	const std::string *sel_cat(size_t *cursor = nullptr);
	const std::string *sel_sub(const std::string& cat, size_t *cursor = nullptr);
	hsapi::hid sel_gam(std::vector<hsapi::Title>& titles, size_t *cursor = nullptr);
}

#endif

