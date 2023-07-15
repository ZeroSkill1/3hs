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

#include "queue.hh"

#include <widgets/indicators.hh>

#include <ui/confirm.hh>
#include <ui/list.hh>

#include <widgets/meta.hh>

#include <algorithm>
#include <vector>

#include "lumalocale.hh"
#include "installgui.hh"
#include "install.hh"
#include "panic.hh"
#include "error.hh"
#include "i18n.hh"
#include "util.hh"
#include "ctr.hh"
#include "log.hh"

static std::vector<hsapi::Title> g_queue;
std::vector<hsapi::Title>& queue_get() { return g_queue; }

void queue_add(const hsapi::Title& meta)
{
	if(std::find_if(g_queue.begin(), g_queue.end(), [&meta](const hsapi::Title& title) -> bool { return title.id == meta.id; }) != g_queue.end())
		return;
	g_queue.push_back(meta);
}

void queue_add(hsapi::hid id, bool disp)
{
	if(std::find_if(g_queue.begin(), g_queue.end(), [id](const hsapi::Title& title) -> bool { return title.id == id; }) != g_queue.end())
		return;
	hsapi::Title meta;
	Result res = disp ? hsapi::call(hsapi::title_meta, meta, std::move(id))
		: hsapi::scall(hsapi::title_meta, meta, std::move(id));
	if(R_FAILED(res)) return;
	queue_add(meta);
}

void queue_remove(size_t index)
{
	g_queue.erase(g_queue.begin() + index);
}

void queue_clear()
{
	g_queue.clear();
}

void queue_process(size_t index)
{
	if(R_SUCCEEDED(install::gui::hs_cia(g_queue[index])))
		queue_remove(index);
}

void queue_process_all()
{
	Result res;
	if(R_FAILED(res = ctr::increase_sleep_lock_ref()))
		elog("failed to acquire sleep mode lock: %08lX", res);
	size_t i;

	struct errvec {
		Result res;
		hsapi::Title *meta;
		bool operator == (const hsapi::Title& other)
		{ return other.id == this->meta->id; }
	};
	std::vector<errvec> errs;
	enum PostProcFlag {
		NONE       = 0,
		WARN_THEME = 1,
		WARN_FILE  = 2,
		SET_PATCH  = 4,
	}; int procflag = NONE;
	for(i = 0; i < g_queue.size(); ++i)
	{
		ilog("Processing title with id=%llu", g_queue[i].id);
		res = install::gui::hs_cia(g_queue[i], false, false, PSTRING(installing_game_x_of_y,
			hsapi::title_name(g_queue[i]), i + 1, g_queue.size()));
		ilog("Finished processing, res=%016lX", res);
		if(R_FAILED(res))
		{
			errvec ev;
			ev.res = res; ev.meta = &g_queue[i];
			errs.push_back(ev);
			if(res == APPERR_CANCELLED)
				break; /* finished */
		}
		else
		{
			if(luma::set_locale(g_queue[i].tid, false))
				procflag |= SET_PATCH;
			if(hsapi::category(g_queue[i].cat).name == THEMES_CATEGORY)
				procflag |= WARN_THEME;
			else if(g_queue[i].flags & hsapi::TitleFlag::installer)
				procflag |= WARN_FILE;
		}
	}

	if(procflag & WARN_THEME) ui::notice(STRING(theme_installed));
	if(procflag & WARN_FILE) ui::notice(STRING(file_installed));

	if(errs.size() != 0)
	{
		ui::LED::ClearResetFlags();
		install::gui::ErrorLED();
		ctr::decrease_sleep_lock_ref();

		ui::notice(STRING(replaying_errors));
		for(errvec& ev : errs)
		{
			error_container err = get_error(ev.res);
			handle_error(err, &ev.meta->name);
		}
	}
	else
	{
		ui::LED::ClearResetFlags();
		install::gui::SuccessLED();
		ctr::decrease_sleep_lock_ref();
	}

	if(procflag & SET_PATCH) luma::maybe_set_gamepatching();

	/* i is the amount of installs processed */
	for(size_t j = 0, k = 0; k < i; ++k)
	{
		/* if g_queue[j] not in errs, advance the queue iterator */
		if(std::find(errs.begin(), errs.end(), g_queue[j]) != errs.end())
			++j;
		/* else if succeeded, remove it from the queue and don't advance queue iterator since the next element is at j */
		else queue_remove(j);
	}
}

static void queue_is_empty()
{
	ui::RenderQueue queue;

	ui::builder<ui::Text>(ui::Screen::top, STRING(queue_empty))
		.x(ui::layout::center_x)
		.y(ui::layout::center_y)
		.wrap()
		.add_to(queue);

	queue.render_finite_button(KEY_A | KEY_B);
}

void show_queue()
{
	using list_t = ui::List<hsapi::Title>;
	bool focus = set_focus(true);

	// Queue is empty :craig:
	if(g_queue.size() == 0)
	{
		queue_is_empty();
		set_focus(focus);
		return;
	}

	ui::RenderQueue queue;

	ui::TitleMeta *meta;

	ui::builder<ui::TitleMeta>(ui::Screen::bottom, g_queue[0])
		.add_to(&meta, queue);

	ui::builder<list_t>(ui::Screen::top, &g_queue)
		.to_string([](const hsapi::Title& meta) -> std::string { return meta.name; })
		.when_select([meta](list_t *self, size_t i, u32 kDown) -> bool {
			/* why is the cast necessairy? */
			((void) i);
			ui::RenderQueue::global()->render_and_then((std::function<bool()>) [self, meta, kDown]() -> bool {
				size_t i = self->get_pos(); /* for some reason the i param corrupted (?) */
				if(kDown & KEY_X)
					queue_remove(i);
				else if(kDown & KEY_A)
					queue_process(i);

				if(g_queue.size() == 0)
					return false; /* we're done */
				/* if we removed the last item */
				if(i >= g_queue.size())
					--i;

				meta->set_title(self->at(i));
				self->set_pos(i);
				self->update();

				return true;
			});

			return true;
		})
		.when_change([meta](list_t *self, size_t i) -> void {
			meta->set_title(self->at(i));
		})
		.buttons(KEY_X)
		.x(5.0f).y(25.0f)
		.add_to(queue);

	ui::builder<ui::Button>(ui::Screen::bottom, STRING(install_all))
		.when_clicked([]() -> bool {
			ui::RenderQueue::global()->render_and_then(queue_process_all);
			/* the queue will always be empty after this */
			return false;
		})
		.wrap()
		.x(ui::layout::right)
		.y(210.0f)
		.add_to(queue);

	queue.render_finite_button(KEY_B);
	set_focus(focus);
}

