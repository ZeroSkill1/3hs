
#ifndef inc_installgui_hh
#define inc_installgui_hh

/* previously all of this logic was contained in
 * queue.cc/hh but i moved it here because it
 * was kind of a mess */

#include <string>
#include <3ds.h>

#include "install.hh"
#include "hsapi.hh"


namespace install
{
	namespace gui
	{
		/* the interactive bool disables any "press a to continue" or similar */
		Result net_cia(const std::string& url, u64 tid, bool interactive = true);
		Result hs_cia(const hsapi::FullTitle& meta, bool interactive = true);
	}
}

#endif

