
#ifndef inc_util_hh
#define inc_util_hh

#include <string>


namespace next
{
	/* returns if we were previously focussed */
	bool set_focus(bool focus);
}

std::string toggle_focus(std::string text);
std::string swap_desc(std::string text);
void toggle_focus();

#endif

