
#include "about.hh"

#include <ui/base.hh>

#include "update.hh" // VERSION
#include "i18n.hh"
#include "util.hh"


void show_about()
{
	ui::RenderQueue queue;

	ui::builder<ui::Text>(ui::Screen::top, STRING(credits_thanks))
		.x(ui::layout::center_x)
		.y(ui::layout::base)
		.add_to(queue);
	ui::builder<ui::Sprite>(ui::Screen::top, ui::SpriteStore::get_by_id(ui::sprite::logo))
		.x(ui::layout::center_x)
		.under(queue.back())
		.add_to(queue);
	ui::builder<ui::Text>(ui::Screen::top, PSTRING(this_version, VERSION))
		.x(ui::layout::center_x)
		.under(queue.back())
		.add_to(queue);
	ui::builder<ui::Text>(ui::Screen::bottom, STRING(credits))
		.size(0.75f)
		.x(10.0f)
		.y(15.0f)
		.add_to(queue);
	ui::builder<ui::Text>(ui::Screen::bottom, STRING(credits_names))
		.x(15.0f)
		.under(queue.back())
		.add_to(queue);

	bool focus = set_focus(true);
	queue.render_finite_button(KEY_B | KEY_A | KEY_START);
	set_focus(focus);
}

