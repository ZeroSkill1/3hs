
#include "about.hh"

#include <ui/base.hh>

#include "ui/button.hh"
#include "ui/sprite.hh"
#include "ui/core.hh"

#include "build/logo.h"

#include "update.hh" // VERSION
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
	ui::RenderQueue queue;

	// TODO: Button class
	ui::builder<ui::next::Button>(ui::Screen::bottom, STRING(back))
		.connect(ui::next::Button::click, []() -> bool { return false; })
		.size(70.0f, 20.0f)
		.x(240.0f)
		.y(210.0f)
		.add_to(queue);
	ui::builder<ui::next::Text>(ui::Screen::top, STRING(credits_thanks))
		.x(ui::layout::center_x)
		.y(50.0f)
		.add_to(queue);
	ui::builder<ui::next::Sprite>(ui::Screen::top, ui::SpriteStore::get_by_id(ui::sprite::logo))
		.x(ui::layout::center_x)
		.under(queue.back())
		.add_to(queue);
	ui::builder<ui::next::Text>(ui::Screen::top, PSTRING(this_version, VERSION))
		.x(ui::layout::center_x)
		.under(queue.back())
		.add_to(queue);
	ui::builder<ui::next::Text>(ui::Screen::bottom, STRING(credits))
		.size(0.75f)
		.x(10.0f)
		.y(15.0f)
		.add_to(queue);
	ui::builder<ui::next::Text>(ui::Screen::bottom, STRING(credits_names))
		.x(15.0f)
		.under(queue.back())
		.add_to(queue);

	bool focus = next::set_focus(true);
	queue.render_finite_button(KEY_START);
	next::set_focus(focus);
}

