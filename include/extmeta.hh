
#ifndef inc_extmeta_hh
#define inc_extmeta_hh

#include <vector>

#include "hsapi.hh"


/* returns if the user wants to continue installing this title or not.
 * lazy loads full title (for prodcode and version) */
bool show_extmeta_lazy(std::vector<hsapi::Title>& titles, hsapi::hid id,
	hsapi::FullTitle *full = nullptr);
bool show_extmeta_lazy(const hsapi::Title& base, hsapi::FullTitle *full = nullptr);
/* returns if the user wants to continue installing this title or not */
bool show_extmeta(const hsapi::FullTitle& title);

#endif

