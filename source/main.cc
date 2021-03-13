
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <3ds.h>
#include <stdlib.h>
#include <malloc.h>

#include <iostream>
#include <curl/curl.h>

#include <ui/text.hh>
#include <ui/core.hh>
#include <ui/list.hh>

#include <hs.hh>
#include "citro2d.h"

#include "selectors.hh"
#include "game.hh"


int main(int argc, char* argv[])
{
	romfsInit();
	if(!ui::global_init())
	{
		ui::global_deinit();
		return 1;
	}

	ui::wid()->push_back("header_desc", new ui::Text(ui::mk_center_WText("The ultimate 3DS content preservation service.", 40.0f)), ui::Scr::top);
	ui::wid()->push_back("curr_action_desc", new ui::Text(ui::mk_center_WText("Loading...", 60.0f)), ui::Scr::top);
	ui::wid()->push_back("header", new ui::Text(ui::mk_center_WText("hShop", 0.0f, 1.0f, 1.0f)), ui::Scr::top);
	quick_global_draw();

	if(osGetWifiStrength() == 0)
	{
		ui::wid()->get<ui::Text>("curr_action_desc")
			->replace_text("Please connect to wifi and restart the app");
		ui::Keys keys; ui::Widgets dummy;
		// 0 = NO wifi at all
		while(ui::framenext(keys) && osGetWifiStrength() == 0)
			ui::framedraw(dummy, keys);
	}

	if(!hs::global_init())
	{
		ui::wid()->get<ui::Text>("curr_action_desc")
			->replace_text("Failed to init networking");
		standalone_main_loop();
		ui::global_deinit();
		hs::global_deinit();
		return 2;
	}


	hs::Index indx;
	indx = hs::Index::get();

	if(index_failed(indx))
	{
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
	romfsExit();
	return 0;
}
