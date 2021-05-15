
#include "about.hh"

#include "ui/button.hh"
#include "ui/sprite.hh"
#include "ui/core.hh"
#include "ui/text.hh"

#include "build/logo.h"

// $ file gfx/img/logo.png
// gfx/img/logo.png: PNG image data, 48 x 48, 8-bit/color RGBA, non-interlaced
#define LOGO_X (48)
#define LOGO_Y (48)

#define add_cred(t,n) wids.push_back("cred" #n, new ui::Text(ui::mk_left_WText(t, GRID_AL_Y(n+2), GRID_AL_X(1))), ui::Scr::bottom);
#define fsize ui::constants::FSIZE


void show_about()
{
	/* Destroy */
	ui::wid()->for_each("button", [](ui::Widget *widget) -> void { ((ui::Button *) widget)->toggle(); });
	ui::wid()->get<ui::Text>("curr_action_desc")->toggle();

	ui::Widgets wids;
	wids.push_back("back", new ui::Button("Back", 240, 210, 310, 230), ui::Scr::bottom);
	wids.get<ui::Button>("back")->set_on_click([]() -> ui::Results {
		return ui::Results::quit_loop;
	});

	wids.push_back("banner", new ui::Text(ui::mk_center_WText("Thank you for using 3hs, a 3ds client for hShop", SCREEN_HEIGHT() / 2 - 10)));

	ui::StandaloneSprite *logo = new ui::StandaloneSprite(SHEET("logo"), logo_logo_idx);
	logo->get_sprite()->set_pos(
		SCREEN_WIDTH(ui::Scr::top) / 2 - LOGO_X / 2, SCREEN_HEIGHT() - LOGO_Y - 40); wids.push_back("logo", logo);

	wids.push_back("credits", new ui::Text(ui::mk_center_WText("Credits:", GRID_AL_Y(1), fsize, fsize, ui::Scr::bottom)), ui::Scr::bottom);
	add_cred("MyPasswordIsWeak: Programming", 1);
	add_cred("TimmSkiller: Programming", 2);
	add_cred("CremeChocolate: Art", 3);
	add_cred("Laswell: Design", 4);

	generic_main_breaking_loop(wids);


	/* Restore */
	ui::wid()->for_each("button", [](ui::Widget *widget) -> void { ((ui::Button *) widget)->toggle(); });
	ui::wid()->get<ui::Text>("curr_action_desc")->toggle();
}

