
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <3ds.h>
#include <stdlib.h>
#include <malloc.h>

#include <iostream>
#include <curl/curl.h>

#include <ui/core.hh>
#include <ui/list.hh>

#include <hs.hh>

#include "selectors.hh"
#include "game.hh"


int main(int argc, char* argv[])
{
	gfxInitDefault();
	consoleInit(GFX_TOP, NULL);
	ui::global_init();
	hs::global_init();

	ui::clear(ui::Scr::bottom);

	{ ui::Widgets w; }

	hs::Index indx = hs::Index::get();
//	if(!index_failed(indx)) return -1;

	while(aptMainLoop())
	{
		int id = sel::cat(indx);
		if(id < 0) break;
		if(id != 0) game::game(id);
	}

	hs::global_deinit();
	ui::global_deinit();
	return 0;
}
