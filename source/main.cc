
#include <3ds.h>

#include <ui/press_to_continue.hh>
#include <ui/image_button.hh>
#include <ui/scrollingText.hh>
#include <ui/progress_bar.hh>
#include <ui/confirm.hh>
#include <ui/button.hh>
#include <ui/swkbd.hh>
#include <ui/text.hh>
#include <ui/core.hh>
#include <ui/list.hh>

#include <widgets/indicators.hh>
#include <widgets/konami.hh>
#include <widgets/meta.hh>

#include <net_common.hh>
#include <3rd/log.hh>
#include <hs.hh>

#include "build/settings_icon.h"
#include "build/search_icon.h"
#include "build/about_icon.h"

#include "settings.hh"
#include "install.hh"
#include "update.hh"
#include "search.hh"
#include "queue.hh"
#include "error.hh"
#include "about.hh"
#include "util.hh"
#include "seed.hh"
#include "next.hh"

#ifdef RELEASE
# define LOG_LEVEL plog::info
#else
# define LOG_LEVEL plog::verbose
#endif

void init_services()
{
#ifdef RELEASE // Not implmented in citra
	mcuHwcInit();
#endif

	romfsInit();
	aptInit();
	fsInit();
	amInit();
}

void exit_services()
{
#ifdef RELEASE
	mcuHwcExit();
#endif

	romfsExit();
	aptExit();
	fsExit();
	amExit();
}

void ensure_logs_dir()
{
	mkdir("/3ds", 0777);
	mkdir("/3ds/3hs", 0777);
}

int main(int argc, char* argv[])
{
	init_services();
	ensure_logs_dir();
	init_seeddb();
	ensure_settings();

	plog::init(LOG_LEVEL, "/3ds/3hs/3hs.log");
	linfo << "App version: " FULL_VERSION;

	if(!ui::global_init())
	{
		lfatal << "ui::global_init() failed, this should **never** happen";
		ui::global_deinit();
		exit_services();
		return 1;
	}

	ui::wid()->push_back("version", new ui::Text(ui::mk_right_WText(VERSION, 3.0f, 5.0f, ui::constants::FSIZE, ui::constants::FSIZE, ui::Scr::bottom)), ui::Scr::bottom);
	ui::wid()->push_back("header_desc", new ui::Text(ui::mk_center_WText("The ultimate 3DS content preservation service.", 30.0f)), ui::Scr::top);
	ui::wid()->push_back("curr_action_desc", new ui::Text(ui::mk_center_WText("Loading ...", 45.0f)), ui::Scr::top);
	ui::wid()->push_back("header", new ui::Text(ui::mk_center_WText("hShop", 0.0f, 1.0f, 1.0f)), ui::Scr::top);
	ui::wid()->push_back("size_indicator", new ui::FreeSpaceIndicator());
	ui::wid()->push_back("konami", new ui::Konami(), ui::Scr::top);
	ui::wid()->push_back("net_indicator", new ui::NetIndicator());

#ifdef RELEASE
	ui::wid()->push_back("batt_indicator", new ui::BatteryIndicator());
#endif

	ui::wid()->push_back("settings",
		new ui::ImageButton(
			SHEET("settings_icon"),
			settings_icon_settings_light_idx,
			settings_icon_settings_dark_idx,
			10, 210, 30, 230
		), ui::Scr::bottom
	);

	ui::wid()->push_back("about",
		new ui::ImageButton(
			SHEET("about_icon"),
			about_icon_about_light_idx,
			about_icon_about_dark_idx,
			40, 210, 60, 230
		), ui::Scr::bottom
	);

	ui::wid()->push_back("search",
		new ui::ImageButton(
			SHEET("search_icon"),
			search_icon_search_light_idx,
			search_icon_search_dark_idx,
			70, 210, 90, 230
		), ui::Scr::bottom
	);

	ui::wid()->push_back("queue", new ui::Button("Queue", 100, 210, 170, 230), ui::Scr::bottom);

	ui::wid()->get<ui::ImageButton>("settings")->set_on_click([]() -> ui::Results {
		ui::end_frame(); show_settings(); ui::wid()->get<ui::FreeSpaceIndicator>("size_indicator")->update(); // Setting may have changed
		return ui::Results::end_early;
	});

	ui::wid()->get<ui::ImageButton>("about")->set_on_click([]() -> ui::Results {
		ui::end_frame(); show_about(); return ui::Results::end_early;
	});

	ui::wid()->get<ui::ImageButton>("search")->set_on_click([]() -> ui::Results {
		ui::end_frame(); show_search(); return ui::Results::end_early;
	});

	ui::wid()->get<ui::Button>("queue")->set_on_click([]() -> ui::Results {
		ui::end_frame(); show_queue(); return ui::Results::end_early;
	});

	// TODO: Add time & date widget
	// TODO: Add net status widget
	// TODO: Add search button
	// TODO: Add logs button

	quick_global_draw();

	if(osGetWifiStrength() == 0)
	{
		lwarning << "No wifi found, waiting for wifi";

		ui::wid()->get<ui::Text>("curr_action_desc")->replace_text("Please connect to wifi and restart the app");
		ui::Keys keys; ui::Widgets dummy;
		// 0 = NO wifi at all
		while(ui::framenext(keys) && osGetWifiStrength() == 0)
			ui::framedraw(dummy, keys);
	}

#ifdef RELEASE
	// Check if luma is installed
	// 1. Citra is used; not compatible
	// 2. Other cfw used; not supported
	Handle lumaCheck;
	if(R_FAILED(svcConnectToPort(&lumaCheck, "hb:ldr")))
	{
		lfatal << "Luma3DS is not installed, user is using an unsupported CFW or running in Citra";
		ui::wid()->get<ui::Text>("curr_action_desc")->replace_text("Luma3DS is not installed on this system");
		ui::wid()->push_back("msg1", new ui::Text(ui::mk_center_WText("Please install Luma3DS on a real 3DS", 78.0f)), ui::Scr::top);
		standalone_main_breaking_loop();
		ui::global_deinit();
		hs::global_deinit();
		exit_services();
		return 3;
	}
	svcCloseHandle(lumaCheck);
#endif

	if(!hs::global_init())
	{
		lfatal << "hs::global_init() failed";
		ui::wid()->get<ui::Text>("curr_action_desc")
			->replace_text("Failed to init networking");
		standalone_main_loop();
		ui::global_deinit();
		hs::global_deinit();
		exit_services();
		return 2;
	}

#ifdef RELEASE
	// If we updated ...
	llog << "Checking for updates";
	if(update_app())
	{
		llog << "Updated from " VERSION;
		hs::global_deinit();
		ui::global_deinit();
		exit_services();
		return 0;
	}
#endif

	hs::Index indx;
	llog << "Fetching index";
	indx = hs::Index::get();

	if(index_failed(indx))
	{
		lfatal << "Failed to fetch index, dns fucked? Server down? " << index_error(indx);
		ui::wid()->get<ui::Text>("curr_action_desc")
			->replace_text(std::string("Couldn't load index: ") + index_error(indx));
		standalone_main_loop();
		ui::global_deinit();
		hs::global_deinit();
		exit_services();
		return 3;
	}

	ui::setup_meta(&indx);

	// Old logic was cursed, made it a bit better :blobaww:
	while(aptMainLoop())
	{
cat:
		std::string cat = next::sel_cat(indx);
		// User wants to exit app
		if(cat == next_cat_exit) break;
		llog << "NEXT(c): " << cat;

sub:
		std::string sub = next::sel_sub(indx, cat);
		if(sub == next_sub_back) goto cat;
		if(sub == next_sub_exit) break;
		llog << "NEXT(s): " << sub;

 gam:
		long int id = next::sel_gam(cat, sub);
		if(id == next_gam_back) goto sub;
		if(id == next_gam_exit) break;
		llog << "NEXT(g): " << id;

		toggle_focus();
		process_hs(id);
		toggle_focus();
		goto gam;
	}


	llog << "Goodbye, app deinit";
	hs::global_deinit();
	ui::global_deinit();
	exit_services();
	return 0;
}
