
#include "extmeta.hh"

#include <ui/scrollingText.hh>
#include <widgets/meta.hh>
#include <ui/core.hh>
#include <ui/util.hh>
#include <ui/text.hh>

#include "panic.hh"
#include "i18n.hh"


bool show_extmeta(hs::FullTitle title)
{
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
	confirm->set_basey(190.0f);
	confirm->center();

	wids.push_back(confirm);

	wids.push_back(new ui::Text(ui::mkWText(STRING(version), 9.0f, GRID_AL_Y(5), 0.45f, 0.45f)));
	wids.push_back(new ui::Text(ui::mkWText(hs::parse_vstring(title.version) + " (" + std::to_string(title.version) + ")", 9.0f, GRID_AL_Y(4))));
	wids.push_back(new ui::Text(ui::mkWText(STRING(prodcode), 9.0f, GRID_AL_Y(7), 0.45f, 0.45f)));
	wids.push_back(new ui::Text(ui::mkWText(title.prod, 9.0f, GRID_AL_Y(6))));

	// TODO: parse html for title.desc?

	generic_main_breaking_loop(wids);
	return ret;
}

