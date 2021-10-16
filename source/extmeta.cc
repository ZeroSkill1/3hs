
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


bool show_extmeta(hs::FullTitle title)
{
//#if 0
	std::string odesc = next::set_desc(STRING(more_about_content));
	bool oret = true;

	ui::RenderQueue queue;

	ui::builder<ui::next::Text>(ui::Screen::top, hs::parse_vstring(title.version) + " (" + std::to_string(title.version) + ")")
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
//#endif

	std::string desc = swap_desc(STRING(more_about_content));

	ui::Widgets wids;
	bool ret = false;

	wids.push_back(new ui::PressButtonCallback([&ret]() -> ui::Results {
		ret = true; return ui::Results::quit_loop;
	}, KEY_A));

	wids.push_back(new ui::PressButtonCallback([&ret]() -> ui::Results {
		ret = false; return ui::Results::quit_loop;
	}, KEY_B));

	wids.push_back(new ui::TitleMeta(title), ui::Scr::bottom);

	ui::WrapText *confirm = new ui::WrapText(STRING(press_to_install));
	confirm->set_basey(170.0f);
	confirm->center();

	wids.push_back(confirm);

	wids.push_back(new ui::Text(ui::mkWText(STRING(version), 9.0f, GRID_AL_Y(5), 0.45f, 0.45f)));
	wids.push_back(new ui::Text(ui::mkWText(hs::parse_vstring(title.version) + " (" + std::to_string(title.version) + ")", 9.0f, GRID_AL_Y(4))));
	wids.push_back(new ui::Text(ui::mkWText(STRING(prodcode), 9.0f, GRID_AL_Y(7), 0.45f, 0.45f)));
	wids.push_back(new ui::Text(ui::mkWText(title.prod, 9.0f, GRID_AL_Y(6))));

	// TODO: parse html for title.desc?

	generic_main_breaking_loop(wids);

	swap_desc(desc);
	return ret;
}

