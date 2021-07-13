
#include "hlink_view.hh"
#include "hlink.hh"
#include "util.hh"

#include <ui/confirm.hh>
#include <ui/core.hh>
#include <ui/text.hh>
#include <ui/util.hh>


void show_hlink()
{
#ifndef V02
	return;
#endif

	toggle_focus();

	hlink::create_server(
		[](const std::string& from) -> bool {
			ui::Widgets wids;
			bool ret = false;

			ui::WrapText *txt = new ui::WrapText(
				"Do you want to accept a connection\n"
				"from " + from + "?\n"
				"Press " GLYPH_A " to accept and " GLYPH_B " to decline"
				);
			txt->set_basey(70.0f);
			txt->center();

			wids.push_back(new ui::PressButtonCallback([&ret]() -> ui::Results {
				ret = true; return ui::Results::quit_loop;
			}, KEY_A));

			wids.push_back(new ui::PressButtonCallback([&ret]() -> ui::Results {
				ret = false; return ui::Results::quit_loop;
			}, KEY_B));

			wids.push_back(txt);

			generic_main_breaking_loop(wids);

			return ret;
		},
		[](const std::string& err) -> void {
			ui::Widgets wids;

			wids.push_back(new ui::PressToContinue(KEY_A));
			wids.push_back(new ui::Text(ui::mk_center_WText(
				"Press " GLYPH_A " to continue", SCREEN_HEIGHT() - 30.0f)));

			ui::WrapText *maintxt = new ui::WrapText("An error occurred creating the hLink server\n" + err);
			maintxt->set_basey(70.0f);
			maintxt->center();

			wids.push_back(maintxt);

			generic_main_breaking_loop(wids);
		},
		[](const std::string& ip) -> void {
			ui::Widgets wids;

			ui::WrapText *txt = new ui::WrapText("Created the hLink server\nConnect to " + ip);
			txt->set_basey(70.0f);
			txt->center();

			wids.push_back(txt);

			single_draw(wids);
		},
		[]() -> bool {
			ui::Keys keys = ui::get_keys();
			return !((keys.kDown | keys.kHeld) & (KEY_START | KEY_B));
		}
	);

	toggle_focus();
}

