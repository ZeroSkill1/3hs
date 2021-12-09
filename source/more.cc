
#include "more.hh"

#include "hlink/hlink_view.hh"

#include "find_missing.hh"
#include "log_view.hh"
#include "about.hh"
#include "i18n.hh"
#include "util.hh"

#include <ui/base.hh>

enum MoreInds {
	IND_ABOUT = 0,
	IND_FIND_MISSING,
	IND_LOG,
#ifndef RELEASE
	IND_HLINK,
#endif
	IND_MAX
};


void show_more()
{
	bool focus = set_focus(true);
	MoreInds index = IND_ABOUT;
	ui::Button *buttons[IND_MAX];

	constexpr float w = ui::screen_width(ui::Screen::bottom) - 10.0f;
	constexpr float h = 20;

	buttons[IND_ABOUT] = ui::builder<ui::Button>(ui::Screen::bottom, STRING(about_app))
		.connect(ui::Button::click, []() -> bool {
			ui::RenderQueue::global()->render_and_then(show_about);
			return true;
		})
		.size(w, h)
		.x(ui::layout::center_x)
		.y(20.0f)
		.border()
		.finalize();
	buttons[IND_FIND_MISSING] = ui::builder<ui::Button>(ui::Screen::bottom, STRING(find_missing_content))
		.connect(ui::Button::click, []() -> bool {
			ui::RenderQueue::global()->render_and_then(show_find_missing_all);
			return true;
		})
		.size(w, h)
		.x(ui::layout::center_x)
		.under(buttons[IND_ABOUT], 5.0f)
		.finalize();
	buttons[IND_LOG] = ui::builder<ui::Button>(ui::Screen::bottom, STRING(log))
		.connect(ui::Button::click, []() -> bool {
			ui::RenderQueue::global()->render_and_then(show_logs_menu);
			return true;
		})
		.size(w, h)
		.x(ui::layout::center_x)
		.under(buttons[IND_FIND_MISSING], 5.0f)
		.finalize();
#ifndef RELEASE
	buttons[IND_HLINK] = ui::builder<ui::Button>(ui::Screen::bottom, "hLink")
		.connect(ui::Button::click, []() -> bool {
			ui::RenderQueue::global()->render_and_then(show_hlink);
			return true;
		})
		.size(w, h)
		.x(ui::layout::center_x)
		.under(buttons[IND_LOG], 5.0f)
		.finalize();
#endif

	ui::RenderQueue queue;
	for(size_t i = 0; i < IND_MAX; ++i)
		queue.push(buttons[i]);

	auto call = [&index]() -> void {
		switch(index) {
			case IND_ABOUT:
				show_about();
				break;
			case IND_FIND_MISSING:
				show_find_missing_all();
				break;
			case IND_LOG:
				show_logs_menu();
				break;
#ifndef RELEASE
			case IND_HLINK:
				show_hlink();
				break;
#endif
			case IND_MAX:
				break;
		}
	};

	ui::builder<ui::ButtonCallback>(ui::Screen::top, KEY_DOWN | KEY_UP | KEY_A)
		.connect(ui::ButtonCallback::kdown, [&index, &buttons, call](u32 k) -> bool {
			buttons[index]->set_border(false);
			if((k & KEY_DOWN) && index < IND_MAX-1)
				++(*(int*)&index); // hacky workaround
			else if((k & KEY_UP) && index > 0)
				--(*(int*)&index);
			else if (k & KEY_A)
				ui::RenderQueue::global()->render_and_then(call);
			buttons[index]->set_border(true);
			return true;
		})
		.add_to(queue);

	queue.render_finite_button(KEY_B);
	set_focus(focus);
}

