
#ifndef inc_extmeta_hh
#define inc_extmeta_hh

#include "hsapi.hh"

/* returns if the user wants to continue installing this title or not */
bool show_extmeta(const hsapi::FullTitle& title);
bool show_extmeta_lazy(const hsapi::Title& base);

#endif

