
#include "widgets/konami.hh"
#include "build/bun.h"
#include "ui/text.hh"

#include <3rd/log.hh>

// $ file gfx/img/bun.png 
// gfx/img/bun.png: PNG image data, 150 x 150, 8-bit/color RGBA, non-interlaced
#define BUN_X (150)
#define BUN_Y (150)

#define KONCODE_SIZE (sizeof(konKeys) / sizeof(size_t))
static size_t konKeys[] = {
	KEY_UP, KEY_UP, KEY_DOWN, KEY_DOWN,
	KEY_LEFT, KEY_RIGHT, KEY_LEFT, KEY_RIGHT,
	KEY_B, KEY_A,
};


ui::Konami::Konami() : Widget("konami")
{
	// Bunny is created when its needed;
	// No need to configure it here
}

ui::Results ui::Konami::draw(ui::Keys& keys, ui::Scr)
{
	if(this->currKey == KONCODE_SIZE)
	{
		this->end_early();
		this->show_bunny();
		return ui::Results::end_early;
	}
	else if(keys.kDown & konKeys[this->currKey])
		++this->currKey;
	else if(keys.kDown != 0)
		this->currKey = 0;

	return ui::Results::go_on;
}

void ui::Konami::show_bunny()
{
	linfo << "User found easter egg :blobaww:";
	this->currKey = 0;

	ui::StandaloneSprite *bunny = new ui::StandaloneSprite(SHEET("bun"), bun_bun_idx);
	bunny->get_sprite()->set_pos(
		SCREEN_WIDTH(ui::Scr::top) / 2 - BUN_X / 2 - 20, 0
	);

	ui::Widgets wids;
	wids.push_back("text", new ui::Text(ui::mk_center_WText("Congrats! You found the hShop bunny", 20,
		ui::constants::FSIZE, ui::constants::FSIZE, ui::Scr::bottom)), ui::Scr::bottom);
	wids.push_back("bun", bunny, ui::Scr::top);
	generic_main_breaking_loop(wids);
}
