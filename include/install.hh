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

#ifndef inc_game_hh
#define inc_game_hh

#include <functional>
#include <string>
#include <3ds.h>

#include "hsapi.hh"

#define CIA_HANDLE_INVALID UINT32_MAX

typedef std::function<void(u64 /* done */, u64 /* total */)> prog_func;
typedef std::function<Result(std::string&)> get_url_func;
static void default_prog_func(u64, u64) { }

static inline get_url_func makeurlwrap(const std::string& url)
{
	return [url](std::string& ret) -> Result {
		ret = url;
		return 0;
	};
}

namespace install
{
	void global_abort();

	Result net_cia(get_url_func get_url, u64 tid, prog_func prog = default_prog_func,
		bool reinstallable = false);
	Result hs_cia(const hsapi::Title& meta, prog_func prog = default_prog_func,
		bool reinstallable = false);

	inline bool is_in_progress()
	{
		extern Handle active_cia_handle;
		return active_cia_handle != CIA_HANDLE_INVALID;
	}
}

#endif

