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

#ifndef inc_proxy_h
#define inc_proxy_h

#include <string>
#include <3ds.h>


namespace proxy
{
	typedef struct Params
	{
		std::string host;
		u16 port = 0;

		std::string username;
		std::string password;
	} Params;

	Result apply(httpcContext *context);
	void init();

	bool validate(const Params& p);
	Params& proxy();
	bool is_set();
	void write();
	void clear();
}


#endif

