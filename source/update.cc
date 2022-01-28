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

#include "update.hh"

#include <3rd/log.hh>

#include <ui/confirm.hh>
#include <ui/base.hh>

#include "installgui.hh"
#include "install.hh"
#include "hsapi.hh"
#include "queue.hh"
#include "i18n.hh"


bool update_app()
{
	if(envIsHomebrew())
	{
		linfo << "Used 3dsx version. Not checking for updates";
		return false;
	}

	std::string nver;
	if(R_FAILED(hsapi::get_latest_version_string(nver)))
		return false; // We will then error at index

	linfo << "Fetched new version " << nver << ", current version is " << VERSION;
	if(nver == VERSION)
		return false;

	bool update;
	ui::RenderQueue queue;

	ui::builder<ui::Confirm>(ui::Screen::bottom, PSTRING(update_to, nver), update)
		.y(80.0f).add_to(queue);

	queue.render_finite();

	if(!update)
	{
		linfo << "User declined update";
		return false;
	}

	u64 tid = 0x0; Result res = 0;
	if(R_FAILED(res = APT_GetProgramID(&tid)))
	{
		lerror << "APT_GetProgramID(...): " << std::hex << res;
		return false;
	}

	install::gui::net_cia(hsapi::update_location(nver), tid, false, true);
	return true;
}

