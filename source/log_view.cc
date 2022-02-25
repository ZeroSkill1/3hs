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

#include <ui/menuselect.hh>
#include <ui/loading.hh>
#include <ui/base.hh>
#include <3rd/log.hh>
#include <string>


// Gonna have to wait until hsite v3 releases
static void upload_logs()
{
	ui::notice("TODO");
}

static void clear_logs()
{
	ui::loading([]() -> void {
		remove(LOGFILE);
		/* appender 0 is our file appender */
		plog::get()->getAppender<plog::RollingFileAppender<plog::TxtFormatter>>(0)
			->setFileName(LOGFILE); /* reload logfile */
	});
}

void show_logs_menu()
{
	bool focus = set_focus(true);

	ui::RenderQueue queue;
	ui::builder<ui::MenuSelect>(ui::Screen::bottom)
#ifndef RELEASE
		.connect(ui::MenuSelect::add, STRING(upload_logs), upload_logs)
#endif
		.connect(ui::MenuSelect::add, STRING(clear_logs), clear_logs)
		.add_to(queue);

	queue.render_finite_button(KEY_B);

	set_focus(focus);
}

