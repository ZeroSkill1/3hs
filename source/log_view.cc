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
#include "hsapi.hh"
#include "util.hh"
#include "i18n.hh"
#include "log.hh"

#include <ui/menuselect.hh>
#include <ui/loading.hh>
#include <ui/base.hh>
#include <string>


static void upload_logs()
{
	FILE *f = fopen(log_filename(), "r");
	if(!f) return;
	fseek(f, 0, SEEK_END);
	u32 size = ftell(f);
	fseek(f, 0, SEEK_SET);
	char *data = (char *) malloc(size);
	if(fread(data, size, 1, f) != 1)
		return;
	std::string id;
	Result res = hsapi::call(hsapi::upload_log, (const char *) data, std::move(size), id);
	free(data);
	if(R_FAILED(res))
	{
		error_container err = get_error(res);
		handle_error(err);
		return;
	}
	ui::notice(PSTRING(log_id, id));
}

static void clear_logs()
{
	ui::loading([]() -> void {
		log_del();
	});
}

void show_logs_menu()
{
	bool focus = set_focus(true);

	ui::RenderQueue queue;
	ui::builder<ui::MenuSelect>(ui::Screen::bottom)
		.connect(ui::MenuSelect::add, STRING(upload_logs), upload_logs)
		.connect(ui::MenuSelect::add, STRING(clear_logs), clear_logs)
		.add_to(queue);

	queue.render_finite_button(KEY_B);

	set_focus(focus);
}

