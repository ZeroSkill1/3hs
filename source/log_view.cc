
#include "log_view.hh"
#include "util.hh"
#include "i18n.hh"

#include <ui/loading.hh>
#include <ui/base.hh>
#include <3rd/log.hh>
#include <string>


// Gonna have to wait until hsite v3 releases
static void upload_logs()
{
	ui::notice("TODO");
}

/* always returns true */
static bool clear_logs()
{
	ui::loading([]() -> void {
		remove(LOGFILE);
		/* appender 0 is our file appender */
		plog::get()->getAppender<plog::RollingFileAppender<plog::TxtFormatter>>(0)
			->setFileName(LOGFILE); /* reload logfile */
	});
	return true;
}

void show_logs_menu()
{
	bool focus = set_focus(true);

	constexpr float w = ui::screen_width(ui::Screen::bottom) - 10.0f;
	constexpr float h = 20;

	ui::RenderQueue queue;

	ui::Button *buttons[2];

/*	ui::builder<ui::Button>(ui::Screen::bottom, STRING(upload_logs))
		.connect(ui::Button::click, []() -> bool {
			ui::RenderQueue::global()->render_and_then(upload_logs);
			return true;
		})
		.border()
		.size(w, h)
		.x(ui::layout::center_x)
		.y(20.0f)
		.add_to(&buttons[0], queue);*/

	ui::builder<ui::Button>(ui::Screen::bottom, STRING(clear_logs))
		.connect(ui::Button::click, clear_logs)
		.size(w, h)
		.x(ui::layout::center_x)
		.y(20.0f)
//		.under(queue.back(), 5.0f)
		.border()
		.add_to(&buttons[1], queue);

	size_t index = 0;
	auto call = [&index]() -> void {
		switch(index) {
		case 0:
			clear_logs();
			((void) upload_logs);
			break;
		case 1:
			clear_logs();
			break;
		}
	};

	ui::builder<ui::ButtonCallback>(ui::Screen::top, KEY_DOWN | KEY_UP | KEY_A)
		.connect(ui::ButtonCallback::kdown, [&index, &buttons, call](u32 k) -> bool {
			buttons[index]->set_border(false);
//			if((k & KEY_DOWN) && index < 1)
//				++(*(int*)&index); // hacky workaround
//			else if((k & KEY_UP) && index > 0)
//				--(*(int*)&index);
			/*else*/ if (k & KEY_A)
				ui::RenderQueue::global()->render_and_then(call);
			buttons[index]->set_border(true);
			return true;
		})
		.add_to(queue);

	queue.render_finite_button(KEY_B);

	set_focus(focus);
}

