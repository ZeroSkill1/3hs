
#include "about.hh"

#include <ui/base.hh>

#include "ui/button.hh"
#include "ui/sprite.hh"
#include "ui/core.hh"

#include "build/logo.h"

#include "i18n.hh"
#include "util.hh"

// $ file gfx/img/logo.png
// gfx/img/logo.png: PNG image data, 48 x 48, 8-bit/color RGBA, non-interlaced
#define LOGO_X (48)
#define LOGO_Y (48)

#define add_cred(t,n) wids.push_back("cred" #n, new ui::Text(ui::mk_left_WText(t, GRID_AL_Y(n+2), GRID_AL_X(1))), ui::Scr::bottom);
#define fsize ui::constants::FSIZE

void show_about()
{
//	toggle_focus();

/*	ui::Widgets wids;
	wids.push_back("back", new ui::Button(STRING(back), 240, 210, 310, 230), ui::Scr::bottom);
	wids.get<ui::Button>("back")->set_on_click([](bool) -> ui::Results {
		return ui::Results::quit_loop;
	});

	wids.push_back("banner", new ui::WrapText(STRING(credits_thanks)));
	wids.get<ui::WrapText>("banner")->set_basey(70.0f);
	wids.get<ui::WrapText>("banner")->center();

	ui::StandaloneSprite *logo = new ui::StandaloneSprite(SHEET("logo"), logo_logo_idx);
	logo->get_sprite()->set_pos(
		SCREEN_WIDTH(ui::Scr::top) / 2 - LOGO_X / 2, SCREEN_HEIGHT() - LOGO_Y - 40); wids.push_back("logo", logo);

	wids.push_back("credits", new ui::Text(ui::mk_center_WText("Credits:", GRID_AL_Y(1), fsize, fsize, ui::Scr::bottom)), ui::Scr::bottom);
	wids.push_back("credits_long", new ui::WrapText(STRING(credits)), ui::Scr::bottom);
	wids.get<ui::WrapText>("credits_long")->set_basey(GRID_AL_Y(2));
	wids.get<ui::WrapText>("credits_long")->set_pad(GRID_AL_X(1));

	generic_main_breaking_loop(wids);*/


	ui::RenderQueue queue;

	// TODO: Button class
	ui::builder<ui::next::Button>(ui::Screen::bottom, STRING(back))
		.connect(ui::next::Button::click, []() -> bool { return false; })
		.size(70.0f, 20.0f)
		.x(240.0f)
		.y(210.0f)
		.add_to(queue);
	ui::builder<ui::next::Text>(ui::Screen::top, STRING(credits_thanks))
		.size(0.65f)
		.x(ui::layout::center_x)
		.y(70.0f)
		.add_to(queue);
	ui::builder<ui::next::Sprite>(ui::Screen::top, ui::SpriteStore::get_by_id(ui::sprite::logo))
		.x(ui::layout::center_x)
		.under(queue.back())
		.add_to(queue);
	ui::builder<ui::next::Text>(ui::Screen::bottom, "Credits:")
		.size(0.75f)
		.x(10.0f)
		.y(15.0f)
		.add_to(queue);
	ui::builder<ui::next::Text>(ui::Screen::bottom, STRING(credits))
		.size(0.65f)
		.x(15.0f)
		.under(queue.back())
		.add_to(queue);

	queue.render_finite_button(KEY_B);

//	toggle_focus();
}

