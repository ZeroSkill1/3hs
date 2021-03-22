
#include <3ds.h>

#include <sys/stat.h>

#include <ui/scrollingText.hh>
#include <ui/konami.hh>
#include <ui/text.hh>
#include <ui/core.hh>
#include <ui/list.hh>

#include <3rd/log.hh>
#include <hs.hh>

#include "selectors.hh"
#include "update.hh"

#include "ui/sprite.hh"

#ifdef __RELEASE__
# define LOG_LEVEL plog::info
#else
# define LOG_LEVEL plog::verbose
#endif

void init_services()
{
	romfsInit();
	fsInit();
	amInit();
}

void exit_services()
{
	romfsExit();
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
	plog::init(LOG_LEVEL, "/3ds/3hs/3hs.log");

	if(!ui::global_init())
	{
		lfatal << "ui::global_init() failed, this should **never** happen";
		ui::global_deinit();
		return 1;
	}

	ui::wid()->push_back("version", new ui::Text(ui::mk_right_WText(VERSION, 3.0f, 5.0f, ui::constants::FSIZE, ui::constants::FSIZE, ui::Scr::bottom)), ui::Scr::bottom);
	ui::wid()->push_back("header_desc", new ui::Text(ui::mk_center_WText("The ultimate 3DS content preservation service.", 30.0f)), ui::Scr::top);
	ui::wid()->push_back("curr_action_desc", new ui::Text(ui::mk_center_WText("Loading...", 45.0f)), ui::Scr::top);
	ui::wid()->push_back("header", new ui::Text(ui::mk_center_WText("hShop", 0.0f, 1.0f, 1.0f)), ui::Scr::top);
	ui::wid()->push_back("konami", new ui::Konami(), ui::Scr::top);
	quick_global_draw();

//	ui::wid()->get<ui::Konami>("konami")->show_bunny();

	if(osGetWifiStrength() == 0)
	{
		lwarning << "No wifi found, waiting for wifi";

		ui::wid()->get<ui::Text>("curr_action_desc")
			->replace_text("Please connect to wifi and restart the app");
		ui::Keys keys; ui::Widgets dummy;
		// 0 = NO wifi at all
		while(ui::framenext(keys) && osGetWifiStrength() == 0)
			ui::framedraw(dummy, keys);
	}

#ifdef __RELEASE__
	// Check if luma is installed
	// 1. Citra is used; not compatible
	// 2. Other cfw used; not supported
    Handle lumaCheck;
	if (R_FAILED(svcConnectToPort(&lumaCheck, "hb:ldr"))) {
		lfatal << "Luma3DS is not installed, user is using an unsupported CFW or running in Citra";
		ui::wid()->get<ui::Text>("curr_action_desc")->replace_text("Luma3DS is not installed on this system");
		ui::wid()->push_back("msg1", new ui::Text(ui::mk_center_WText("Please install Luma3DS on a real 3DS", 78.0f)), ui::Scr::top);
		standalone_main_breaking_loop();
		ui::global_deinit();
		hs::global_deinit();
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
		return 2;
	}

#ifdef __RELEASE__
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

//	ui::wid()->push_back("test", new ui::ScrollingText(20, 20, "hello! this is a long string that doesn't fit on your screen ...."), ui::Scr::bottom);
//	ui::wid()->get<ui::ScrollingText>("test")->start_scroll();
//	standalone_main_loop();

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
		return 3;
	}

	while(aptMainLoop())
	{
		long id = sel::cat(indx);
		linfo << "Broke out of game select loop, result code/id: " << id;

		if(id < 0) break;
		if(id != 0) sel::game(id);
	}

	hs::global_deinit();
	ui::global_deinit();
	exit_services();
	return 0;
}
