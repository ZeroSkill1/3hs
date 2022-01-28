/*
 * Copyright (C) 2021-2022 MyPasswordIsWeak
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <https://www.gnu.org/licenses/>.
 */

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
//			buttons[index]->set_border(false);
//			if((k & KEY_DOWN) && index < 1)
//				++index;
//			else if((k & KEY_UP) && index > 0)
//				--index;
			/*else*/ if (k & KEY_A)
				ui::RenderQueue::global()->render_and_then(call);
//			buttons[index]->set_border(true);
			return true;
		})
		.add_to(queue);

	queue.render_finite_button(KEY_B);

	set_focus(focus);
}

