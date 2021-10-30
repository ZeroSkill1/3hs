
#include "update.hh"

#define VERSION "v" FULL_VERSION

#include <curl/curl.h>
#include <3rd/log.hh>

#include <ui/confirm.hh>
#include <ui/core.hh>

#include "net_common.hh"
#include "install.hh"
#include "hsapi.hh"
#include "queue.hh"
#include "i18n.hh"


bool update_app()
{
	if(envIsHomebrew())
	{
		linfo << "Used 3dsx version. Not checking for updates";
		return false;
	}

	std::string nver;
	if(R_FAILED(hsapi::get_latest_version_string(nver)))
		return false; // We will then error at index

	linfo << "Fetched new version " << nver << ", current version is " << VERSION;
	if(nver == FULL_VERSION)
		return false;

	ui::Widgets wids; bool shouldUpdate = false;
	wids.push_back("confirmation", new ui::Confirm(PSTRING(update_to, nver), shouldUpdate), ui::Scr::bottom);
	generic_main_breaking_loop(wids);

	if(!shouldUpdate)
	{
		linfo << "User declined update";
		return false;
	}

	u64 tid = 0x0; Result res = 0;
	if(R_FAILED(res = APT_GetProgramID(&tid)))
	{
		lerror << "APT_GetProgramID(...): " << std::hex << res;
		return false;
	}

	process_uri(hsapi::update_location(nver), true, tid_to_str(tid));
	return true;
}

