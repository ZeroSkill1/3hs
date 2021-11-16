
#ifndef inc_util_hh
#define inc_util_hh

#include <ui/base.hh>
#include <string>


namespace next
{
	/* returns if we were previously focussed */
	bool set_focus(bool focus);
	/* sets the action description and returns the old one */
	std::string set_desc(const std::string& nlabel);
}

std::string toggle_focus(std::string text);
std::string swap_desc(std::string text);
void toggle_focus();

#endif

