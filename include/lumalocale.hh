
#ifndef inc_lumalocale_hh
#define inc_lumalocale_hh

#include <3ds/types.h>


namespace luma
{
	void set_locale(u64 tid);
	/* sets gamepatching and prompts for reboot if required */
	void maybe_set_gamepatching();
}

#endif

