/* This file is part of 3hs
 * Copyright (C) 2021-2022 hShop developer team
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

#include "search.hh"

#include <widgets/meta.hh>

#include <ui/menuselect.hh>
#include <ui/loading.hh>
#include <ui/swkbd.hh>
#include <ui/base.hh>

#include "lumalocale.hh"
#include "installgui.hh"
#include "extmeta.hh"
#include "queue.hh"
#include "hsapi.hh"
#include "next.hh"
#include "util.hh"
#include "i18n.hh"

static bool is_tid(std::string& str)
{
	for(size_t i = 0; i < str.size(); ++i)
  		if(!((str[i] >= '0' && str[i] <= '9') || (str[i] >= 'a' && str[i] <= 'z') || (str[i] >= 'A' && str[i] <= 'Z')))
    		return false;

	return str.find("0004") == 0;
}

static void error(const std::string& msg)
{
	ui::RenderQueue queue;

	ui::builder<ui::Text>(ui::Screen::top, msg)
		.x(ui::layout::center_x)
		.y(ui::layout::center_y)
		.add_to(queue);

	queue.render_finite_button(KEY_A);
}

static void show_searchbar_search()
{
	SwkbdResult res;
	SwkbdButton btn;

	std::string query = ui::keyboard([](ui::AppletSwkbd *swkbd) -> void {
		swkbd->hint(STRING(search_content_action));
		swkbd->valid(SWKBD_NOTEMPTY_NOTBLANK);
	}, &btn, &res);

	if(btn != SWKBD_BUTTON_CONFIRM)
		return;

	if(query.size() < 2 || res == SWKBD_INVALID_INPUT || res == SWKBD_OUTOFMEM || res == SWKBD_BANNED_INPUT)
	{
		error(STRING(invalid_query));
		return;
	}

	set_desc(PSTRING(results_query, query));
	set_focus(true);

	std::vector<hsapi::Title> titles;
	Result rres = hsapi::call<std::vector<hsapi::Title>&, const std::string&>(hsapi::search, titles, query);
	if(R_FAILED(rres)) return;

	if(titles.size() == 0)
	{
		error(STRING(search_zero_results));
		return;
	}

	next::maybe_sel_gam(titles);
	return;
}

static void show_id_search()
{
	SwkbdResult res;
	SwkbdButton btn;

	uint64_t id = ui::numpad([](ui::AppletSwkbd *swkbd) -> void {
		swkbd->hint(STRING(search_id));
	}, &btn, &res, 16);

	if(btn != SWKBD_BUTTON_CONFIRM)
		return;

	if(res == SWKBD_INVALID_INPUT || res == SWKBD_OUTOFMEM || res == SWKBD_BANNED_INPUT)
		return;

	hsapi::FullTitle title;
	Result rres = hsapi::call<hsapi::FullTitle&, hsapi::hid>(hsapi::title_meta, title, id);
	if(R_FAILED(rres)) return;

	if(show_extmeta(title))
		install::gui::hs_cia(title);
}

static void show_tid_search()
{
	SwkbdResult res;
	SwkbdButton btn;

	std::string title_id = ui::keyboard([](ui::AppletSwkbd *swkbd) -> void {
		swkbd->hint(STRING(search_tid));
	}, &btn, &res, 16);

	if(btn != SWKBD_BUTTON_CONFIRM)
		return;

	if(res == SWKBD_INVALID_INPUT || res == SWKBD_OUTOFMEM || res == SWKBD_BANNED_INPUT)
		return;

	if (!is_tid(title_id))
	{
		error(STRING(invalid_tid));
		return;
	}

	if (title_id == "0004000001111100")
	{
		error(STRING(theme_installer_tid_bad));
		return;
	}

	std::vector<hsapi::Title> titles;
	Result rres = hsapi::call<std::vector<hsapi::Title>&, const std::string&>(hsapi::get_by_title_id, titles, title_id);
	if(R_FAILED(rres))
		return;

	if(titles.size() == 0)
	{
		error(STRING(search_zero_results));
		return;
	}

	next::maybe_sel_gam(titles);
	return;
}

void show_search()
{
	std::string desc = set_desc(STRING(search_content));
	bool focus = set_focus(true);

	ui::RenderQueue queue;
	ui::builder<ui::MenuSelect>(ui::Screen::bottom)
		.connect(ui::MenuSelect::add, STRING(search_text), show_searchbar_search)
		.connect(ui::MenuSelect::add, STRING(search_id), show_id_search)
		.connect(ui::MenuSelect::add, STRING(search_tid), show_tid_search)
		.add_to(queue);

	queue.render_finite_button(KEY_B);

	set_focus(focus);
	set_desc(desc);
}

