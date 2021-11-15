
#include <3ds.h>

#include <algorithm>

#include <ui/image_button.hh>
#include <ui/list.hh>
#include <ui/core.hh>
#include <ui/base.hh>

#include <widgets/indicators.hh>
#include <widgets/konami.hh>
#include <widgets/meta.hh>

#include <3rd/plog/Appenders/ColorConsoleAppender.h>
#include <net_common.hh>
#include <3rd/log.hh>

#include "build/settings_icon.h"
#include "build/search_icon.h"
#include "build/more_icon.h"

#include "lumalocale.hh"
#include "settings.hh"
#include "log_view.hh"
#include "extmeta.hh"
#include "update.hh"
#include "search.hh"
#include "queue.hh"
#include "panic.hh"
#include "about.hh"
#include "proxy.hh"
#include "hsapi.hh"
#include "more.hh"
#include "next.hh"
#include "i18n.hh"
#include "seed.hh"
#include "util.hh"

#ifdef RELEASE
# define LOG_LEVEL plog::info
#else
# define LOG_LEVEL plog::verbose
#endif

#include <ui/base.hh>


int main(int argc, char* argv[])
{
	((void) argc);
	((void) argv);

	plog::init(LOG_LEVEL, LOGFILE);
	linfo << "version=" FULL_VERSION;

#ifndef RELEASE
	// Colored logs appender for gdb
	consoleDebugInit(debugDevice_SVC);

	static plog::ColorConsoleAppender<plog::TxtFormatter> colorAppender(plog::streamStdErr);
	plog::get()->addAppender(&colorAppender);
#endif


	bool isLuma = false;
	panic_if_err_3ds(init_services(isLuma));
	if(!ui::global_init())
	{
		lfatal << "ui::global_init() failed, this should **never** happen";
		ui::global_deinit();
		return 1;
	}

#ifdef RELEASE
	// Check if luma is installed
	// 1. Citra is used; not compatible
	// 2. Other cfw used; not supported
	Handle lumaCheck;
	if(!isLuma)
	{
		lfatal << "Luma3DS is not installed, user is using an unsupported CFW or running in Citra";
		ui::RenderQueue queue;

		ui::builder<ui::next::Text>(ui::Screen::top, STRING(luma_not_installed))
			.x(ui::layout::center_x).y(45.0f)
			.add_to(queue);
		ui::builder<ui::next::Text>(ui::Screen::top, STRING(install_luma))
			.x(ui::layout::center_x).under(queue.back())
			.add_to(queue);

		queue.render_finite_button(KEY_START | KEY_B);
		ui::global_deinit();
		exit_services();
		return 3;
	}
	svcCloseHandle(lumaCheck);
#endif

	init_seeddb();
	ensure_settings();
	proxy::init();

	osSetSpeedupEnable(true); // speedup for n3dses

	/* new ui setup */
 	ui::builder<ui::next::Text>(ui::Screen::top, "") /* text is not immediately set */
		.x(ui::layout::center_x)
		.y(4.0f)
		.tag(ui::tag::action)
		.add_to(ui::RenderQueue::global());

	/* buttons */
	ui::builder<ui::next::Button>(ui::Screen::bottom, ui::SpriteStore::get_by_id(ui::sprite::settings_dark))
		.connect(ui::next::Button::click, []() -> bool {
			ui::RenderQueue::global()->render_and_then(show_settings);
			return true;
		})
		.connect(ui::next::Button::nobg)
		.wrap()
		.x(5.0f)
		.y(210.0f)
		.tag(ui::tag::settings)
		.add_to(ui::RenderQueue::global());

	ui::builder<ui::next::Button>(ui::Screen::bottom, ui::SpriteStore::get_by_id(ui::sprite::more_dark))
		.connect(ui::next::Button::click, []() -> bool {
			ui::RenderQueue::global()->render_and_then(show_more);
			return true;
		})
		.connect(ui::next::Button::nobg)
		.wrap()
		.right(ui::RenderQueue::global()->back())
		.y(210.0f)
		.tag(ui::tag::more)
		.add_to(ui::RenderQueue::global());

	ui::builder<ui::next::Button>(ui::Screen::bottom, ui::SpriteStore::get_by_id(ui::sprite::search_dark))
		.connect(ui::next::Button::click, []() -> bool {
			ui::RenderQueue::global()->render_and_then(show_search);
			return true;
		})
		.connect(ui::next::Button::nobg)
		.wrap()
		.right(ui::RenderQueue::global()->back())
		.y(210.0f)
		.tag(ui::tag::search)
		.add_to(ui::RenderQueue::global());

	ui::builder<ui::next::Button>(ui::Screen::bottom, STRING(queue))
		.connect(ui::next::Button::click, []() -> bool {
			ui::RenderQueue::global()->render_and_then(show_queue);
			return true;
		})
		.connect(ui::next::Button::nobg)
		.wrap()
		.right(ui::RenderQueue::global()->back())
		.y(210.0f)
		.tag(ui::tag::queue)
		.add_to(ui::RenderQueue::global());

	ui::wid()->push_back("version", new ui::Text(ui::mk_right_WText(VERSION, 3.0f, 5.0f, 0.4f, 0.4f, ui::Scr::bottom)), ui::Scr::bottom);
	ui::wid()->push_back("header_desc", new ui::Text(ui::mk_center_WText(STRING(banner), 30.0f)), ui::Scr::top);
	ui::wid()->push_back("header", new ui::Text(ui::mk_center_WText("hShop", 0.0f, 1.0f, 1.0f)), ui::Scr::top);
	ui::wid()->push_back("curr_action_desc", new ui::Text(ui::mk_center_WText("", 45.0f)), ui::Scr::top); // the first message is caused by ui::loading()
	ui::wid()->push_back("size_indicator", new ui::FreeSpaceIndicator());
	ui::wid()->push_back("time_indicator", new ui::TimeIndicator());
	ui::wid()->push_back("konami", new ui::Konami(), ui::Scr::top);
	ui::wid()->push_back("net_indicator", new ui::NetIndicator());

#ifdef RELEASE
	ui::wid()->push_back("batt_indicator", new ui::BatteryIndicator());
#endif

	// DRM Check
#ifdef DEVICE_ID
	u32 devid = 0;
	PS_GetDeviceId(&devid);
	// DRM Check failed
	if(devid != DEVICE_ID)
	{
		lerror << "Piracyception";
		(* (int *) nullptr) = 0xdeadbeef;
	}
#endif
	// end DRM Check

	ui::wid()->push_back("settings",
		new ui::ImageButton(
			SHEET("settings_icon"),
			settings_icon_settings_light_idx,
			settings_icon_settings_dark_idx,
			10, 210, 30, 230
		), ui::Scr::bottom
	);

	ui::wid()->push_back("more",
		new ui::ImageButton(
			SHEET("more_icon"),
			more_icon_more_light_idx,
			more_icon_more_dark_idx,
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

	ui::wid()->push_back("queue", new ui::Button(STRING(queue), 100, 210, 170, 230), ui::Scr::bottom);
	ui::wid()->get<ui::Button>("queue")->autoadjust_x(10.0f);

	ui::wid()->get<ui::ImageButton>("settings")->set_on_click([]() -> ui::Results {
		ui::end_frame(); show_settings(); return ui::Results::end_early;
	});

	ui::wid()->get<ui::ImageButton>("more")->set_on_click([]() -> ui::Results {
		ui::end_frame(); show_more(); return ui::Results::end_early;
	});

	ui::wid()->get<ui::ImageButton>("search")->set_on_click([]() -> ui::Results {
		ui::end_frame(); show_search(); return ui::Results::end_early;
	});

	ui::wid()->get<ui::Button>("queue")->set_on_click([](bool) -> ui::Results {
		ui::end_frame(); show_queue(); return ui::Results::end_early;
	});

	// TODO: Add net status widget
	// TODO: Add logs button

	if(!hsapi::global_init())
	{
		lfatal << "hsapi::global_init() failed";
		panic(STRING(fail_init_networking));
		return 2;
	}

#ifdef RELEASE
	// If we updated ...
	llog << "Checking for updates";
	if(update_app())
	{
		llog << "Updated from " VERSION;
		hsapi::global_deinit();
		ui::global_deinit();
		exit_services();
		return 0;
	}
#endif

	Result res = hsapi::call(hsapi::fetch_index);
	if(R_FAILED(res))
	{
		error_container errcont = get_error(res);
		report_error(errcont, "Failed to fetch index, dns fucked? Server down?");
		panic(PSTRING(fail_fetch_index, errcont.sDesc));
		return 3;
	}

	lverbose << "Done fetching index.";

	size_t catptr = 0, subptr = 0, gamptr = 0;
	const std::string *associatedcat = nullptr;
	const std::string *associatedsub = nullptr;

	// Old logic was cursed, made it a bit better :blobaww:
	while(aptMainLoop())
	{
cat:
		const std::string *cat = next::sel_cat(&catptr);
		// User wants to exit app
		if(cat == next_cat_exit) break;
		llog << "NEXT(c): " << *cat;

sub:
		if(associatedcat != cat) subptr = 0;
		associatedcat = cat;

		const std::string *sub = next::sel_sub(*cat, &subptr);
		if(sub == next_sub_back) goto cat;
		if(sub == next_sub_exit) break;
		llog << "NEXT(s): " << *sub;

		std::vector<hsapi::Title> titles;
		if(R_FAILED(hsapi::call(hsapi::titles_in, titles, *cat, *sub)))
			goto sub;

 gam:
 		if(associatedsub != sub) gamptr = 0;
		associatedsub = sub;

		hsapi::hid id = next::sel_gam(titles, &gamptr);
		if(id == next_gam_back) goto sub;
		if(id == next_gam_exit) break;

		llog << "NEXT(g): " << id;

		hsapi::FullTitle meta;
		if(show_extmeta_lazy(titles, id, &meta))
		{
			toggle_focus();
			process_hs(meta);
			luma::set_gamepatching();
			toggle_focus();
		}

		goto gam;
	}


	llog << "Goodbye, app deinit";
	hsapi::global_deinit();
	ui::global_deinit();
	exit_services();
	return 0;
}
