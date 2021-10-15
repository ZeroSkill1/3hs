
#include <ui/base.hh>

#include "util.hh"


void show_help()
{
	bool focus = next::set_focus(true);
	ui::RenderQueue queue;

	ui::builder<ui::next::Text>(ui::Screen::top,
			"To download a game go to the main menu\n"
			"and press " UI_GLYPH_A " on a category.\n"
			"After that you can press " UI_GLYPH_A " again\n"
			"to select a subcategory. Finally, press " UI_GLYPH_A "\n"
			"to select a game and it will start downloading\n"
			"& install. To add a game to the queue, press" UI_GLYPH_Y "\n"
			"on a title. To view all queue entries click on the\n"
			"\"Queue\" button on the bottom of your screen.\n")
		.x(ui::layout::center_x)
		.y(40.0f)
		.add_to(queue);

	ui::builder<ui::next::Text>(ui::Screen::bottom,
			"You can press " UI_GLYPH_X " on a queue entry\n"
			"to remove it from the queue. To search\n"
			"for a title click the bottom left\n"
			"search icon. To modify the settings\n"
			"click the gear icon. You can\n"
			"press " UI_GLYPH_DPAD " to move through lists.\n"
			"\n"
			"\n"
			"\n"
			"Press " UI_GLYPH_A " to continue")
		.x(ui::layout::center_x)
		.y(20.0f)
		.add_to(queue);

	queue.render_finite_button(KEY_A | KEY_B);
	next::set_focus(focus);
}

