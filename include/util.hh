
#ifndef inc_util_hh
#define inc_util_hh

#include <ui/base.hh>
#include <string>


/* returns if we were previously focussed */
bool set_focus(bool focus);
/* sets the action description and returns the old one */
std::string set_desc(const std::string& nlabel);

#endif

