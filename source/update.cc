
#include "update.hh"

#include <3rd/log.hh>

#include <ui/confirm.hh>
#include <ui/base.hh>

#include "installgui.hh"
#include "install.hh"
#include "queue.hh"
#include "proxy.hh"
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
	if(nver == VERSION)
		return false;

	bool update;
	ui::RenderQueue queue;

	ui::builder<ui::Confirm>(ui::Screen::bottom, PSTRING(update_to, nver), update)
		.y(80.0f).add_to(queue);

	queue.render_finite();

	if(!update)
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

	install::gui::net_cia(hsapi::update_location(nver), true, tid);
	return true;
}

