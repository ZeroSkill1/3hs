
#include <ui/core.hh>
#include <ui/text.hh>
#include <ui/util.hh>

#include "util.hh"


void show_help()
{
	toggle_focus();

	ui::Widgets wids;

	wids.push_back("text_top", new ui::WrapText(
		"To download a game go to the main menu\n"
		"and press " GLYPH_A " on a category.\n"
		"After that you can press " GLYPH_A " again\n"
		"to select a subcategory. Finally, press " GLYPH_A "\n"
		"to select a game and it will start downloading\n"
		"& install. To add a game to the queue, press" GLYPH_Y "\n"
		"on a title. To view all queue entries click on the\n"
		"\"Queue\" button on the bottom of your screen.\n"
	), ui::Scr::top);

	wids.push_back("text_bot", new ui::WrapText(
		"You can press " GLYPH_X " on a queue entry\n"
		"to remove it from the queue. To search\n"
		"for a title. click the bottom left\n"
		"search icon. To modify the settings\n"
		"click the gear icon. You can\n"
		"press " GLYPH_DPAD " to move through lists.\n"
		"\n"
		"\n"
		"\n"
		"Press " GLYPH_A " to continue"
	), ui::Scr::bottom);

	wids.get<ui::WrapText>("text_top")->set_basey(60.0f);
	wids.get<ui::WrapText>("text_top")->center();

	wids.get<ui::WrapText>("text_bot")->set_basey(30.0f);
	wids.get<ui::WrapText>("text_bot")->center();

	wids.push_back(new ui::PressToContinue(KEY_B | KEY_A));
	generic_main_breaking_loop(wids);

	toggle_focus();
}

