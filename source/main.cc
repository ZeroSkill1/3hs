
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
	if(!hs::global_init())
	{
		ui::global_deinit();
		hs::global_deinit();
		return 2;
	}

	hs::Index indx = hs::Index::get();
// if(!index_failed(indx)) return 3;


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
