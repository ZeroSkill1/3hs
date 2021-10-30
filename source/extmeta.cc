
#include "extmeta.hh"

#include <ui/base.hh>

#include <ui/scrollingText.hh>
#include <widgets/meta.hh>
#include <ui/core.hh>
#include <ui/util.hh>
#include <ui/text.hh>

#include "panic.hh"
#include "i18n.hh"
#include "util.hh"


bool show_extmeta(const hsapi::FullTitle& title)
{
	std::string odesc = next::set_desc(STRING(more_about_content));
	bool oret = true;

	ui::RenderQueue queue;

	ui::builder<ui::next::Text>(ui::Screen::top, hsapi::parse_vstring(title.version) + " (" + std::to_string(title.version) + ")")
		.x(9.0f)
		.y(40.0f)
		.add_to(queue);
	ui::builder<ui::next::Text>(ui::Screen::top, STRING(version))
		.size(0.45f)
		.x(9.0f)
		.under(queue.back(), -1.0f)
		.add_to(queue);
	ui::builder<ui::next::Text>(ui::Screen::top, title.prod)
		.x(9.0f)
		.under(queue.back(), 1.0f)
		.add_to(queue);
	ui::builder<ui::next::Text>(ui::Screen::top, STRING(prodcode))
		.size(0.45f)
		.x(9.0f)
		.under(queue.back(), -1.0f)
		.add_to(queue);


	ui::builder<ui::next::TitleMeta>(ui::Screen::bottom, title)
		.add_to(queue);

	ui::builder<ui::next::ButtonCallback>(ui::Screen::top, KEY_B)
		.connect(ui::next::ButtonCallback::kdown, [&oret](u32) -> bool { return oret = false; })
		.add_to(queue);

	queue.render_finite_button(KEY_A);
	next::set_desc(odesc);
	return oret;
}

