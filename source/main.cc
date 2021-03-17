
#include <3ds.h>

#include <sys/stat.h>

#include <ui/scrollingText.hh>
#include <ui/text.hh>
#include <ui/core.hh>
#include <ui/list.hh>

#include <3rd/log.hh>
#include <hs.hh>

#include "selectors.hh"
#include "update.hh"
#include "game.hh"


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
	plog::init(plog::verbose, "/3ds/3hs/3hs.log");

	if(!ui::global_init())
	{
		lfatal << "ui::global_init() failed, this should **never** happen";
		ui::global_deinit();
		return 1;
	}

	ui::wid()->push_back("header_desc", new ui::Text(ui::mk_center_WText("The ultimate 3DS content preservation service.", 40.0f)), ui::Scr::top);
	ui::wid()->push_back("version", new ui::Text(ui::mk_right_WText(VERSION, 3.0f, 5.0f, 0.5f, 0.5f, ui::Scr::bottom)), ui::Scr::bottom);
	ui::wid()->push_back("curr_action_desc", new ui::Text(ui::mk_center_WText("Loading...", 60.0f)), ui::Scr::top);
	ui::wid()->push_back("header", new ui::Text(ui::mk_center_WText("hShop", 0.0f, 1.0f, 1.0f)), ui::Scr::top);
	quick_global_draw();

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

#ifndef __NO_UPDATE
	// If we updated ...
	llog << "Checking for updates";
	/*if(update_app())
	{
		llog << "Updated from " VERSION;
		hs::global_deinit();
		ui::global_deinit();
		exit_services();
		return 0;
	}*/
#endif

	ui::wid()->push_back("test", new ui::ScrollingText(20, 20, "hello! this is a long string that doesn't fit on your screen ...."), ui::Scr::bottom);
	ui::wid()->get<ui::ScrollingText>("test")->start_scroll();
	standalone_main_loop();

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
		int id = sel::cat(indx);
		if(id < 0) break;
		if(id != 0) game::game(id);
	}

	hs::global_deinit();
	ui::global_deinit();
	exit_services();
	return 0;
}
