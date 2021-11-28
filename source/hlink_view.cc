
#include "hlink_view.hh"
#include "hlink.hh"
#include "panic.hh"
#include "util.hh"
#include "i18n.hh"

#include <ui/base.hh>
#include <3rd/log.hh>


static void addreq(ui::RenderQueue& queue, const std::string& reqstr)
{
	ui::builder<ui::Text>(ui::Screen::bottom, "Last request\n" + reqstr)
		.x(ui::layout::center_x)
		.y(ui::layout::base)
		.add_to(queue);
}

void show_hlink()
{
	bool focus = set_focus(true);

	std::string reqstr = STRING(no_req);

	hlink::create_server(
		[&reqstr](const std::string& from) -> bool {
			ui::RenderQueue queue;
			addreq(queue, reqstr);
			bool ret = false;

			ui::builder<ui::Text>(ui::Screen::top,
					"Do you want to accept a connection\n"
					"from " + from + "?\n"
					"Press " UI_GLYPH_A " to accept and " UI_GLYPH_B " to decline")
				.x(ui::layout::center_x)
				.y(ui::layout::base)
				.add_to(queue);

			ui::builder<ui::ButtonCallback>(ui::Screen::top, KEY_A)
				.connect(ui::ButtonCallback::kdown, [&ret](u32) -> bool { ret = true; return false; })
				.add_to(queue);

			ui::builder<ui::ButtonCallback>(ui::Screen::top, KEY_B)
				.connect(ui::ButtonCallback::kdown, [](u32) -> bool { return false; })
				.add_to(queue);

			queue.render_finite();
			return ret;
		},
		[&reqstr](const std::string& err) -> void {
			ui::RenderQueue queue;
			addreq(queue, reqstr);

			ui::builder<ui::Text>(ui::Screen::top, "Press " UI_GLYPH_A " to continue")
				.x(ui::layout::center_x)
				.y(ui::dimensions::height - 30.0f)
				.add_to(queue);
			ui::builder<ui::Text>(ui::Screen::top, "An error occured in the hLink server\n" + err)
				.x(ui::layout::center_x)
				.y(ui::layout::base)
				.add_to(queue);

			queue.render_finite_button(KEY_A);
		},
		[&reqstr](const std::string& ip) -> void {
			ui::RenderQueue queue;
			addreq(queue, reqstr);

			ui::builder<ui::Text>(ui::Screen::top, "Created the hLink server\nConnect to " + ip)
				.x(ui::layout::center_x)
				.y(ui::layout::base)
				.add_to(queue);

			queue.render_frame();
		},
		[]() -> bool {
			ui::Keys keys = ui::RenderQueue::get_keys();
			return !((keys.kDown | keys.kHeld) & (KEY_START | KEY_B));
		},
		[&reqstr](const std::string& str) -> void {
			ui::RenderQueue queue;
			reqstr = str;

			addreq(queue, reqstr);
			queue.render_frame();
		}
	);

	set_focus(focus);
}

