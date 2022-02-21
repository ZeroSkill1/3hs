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

#include "find_missing.hh"
#include "install.hh"
#include "hsapi.hh"
#include "queue.hh"
#include "panic.hh"
#include "ctr.hh"

#include <ui/loading.hh>

#include <algorithm>


template <typename T>
static void vecappend(std::vector<T>& a, const std::vector<T>& b)
{ a.insert(a.end(), b.begin(), b.end()); }

bool tid_can_have_missing(hsapi::htid tid)
{
	u16 category = (tid >> 32) & 0xFFFF;
	return category == 0x0000 /* normal */ || category == 0x8000 /* DSiWare/TWL */;
}

ssize_t show_find_missing(hsapi::htid tid)
{
	ssize_t ret = -1;
	ui::loading([tid, &ret]() -> void {
		std::vector<hsapi::htid> installed;
		panic_if_err_3ds(ctr::list_titles_on(MEDIATYPE_SD, installed));
		ctr::list_titles_on(MEDIATYPE_GAME_CARD, installed); // it might error if there is no cart inserted so we don't want to panic if it fails

		std::vector<hsapi::htid> doCheckOn;
		if(tid == 0) doCheckOn = installed;
		else doCheckOn.push_back(tid);

		std::vector<hsapi::htid> installedGames;
		std::copy_if(doCheckOn.begin(), doCheckOn.end(), std::back_inserter(installedGames), tid_can_have_missing);

		hsapi::BatchRelated related;
		if(R_FAILED(hsapi::batch_related(related, installedGames)))
			return;

		std::vector<hsapi::FullTitle> potentialInstalls;

		for(size_t i = 0; i < installedGames.size(); ++i)
		{
			vecappend(potentialInstalls, related[installedGames[i]].updates);
			vecappend(potentialInstalls, related[installedGames[i]].dlc);
		}

		std::vector<hsapi::FullTitle> newInstalls;
		std::copy_if(potentialInstalls.begin(), potentialInstalls.end(), std::back_inserter(newInstalls), [installed](const hsapi::FullTitle& title) -> bool {
			if(std::find(queue_get().begin(), queue_get().end(), title) == queue_get().end())
				return false;
			/* not installed */
			if(std::find(installed.begin(), installed.end(), title.tid) != installed.end()) return true;
			AM_TitleEntry te;
			if(R_FAILED(ctr::get_title_entry(*inst, te)))
				return false;
			return title.version > te.version;
		});

		for(const hsapi::Title& title : newInstalls)
			queue_add(title.id, false);
		ret = newInstalls.size();
	});
	return ret;
}

void show_find_missing_all()
{
	ssize_t done = show_find_missing(0x0);
	if(done == -1) return;
	if(done == 0) ui::notice(STRING(found_0_missing));
	else ui::notice(PSTRING(found_missing, done));
}

