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

#ifndef inc_update_hh
#define inc_update_hh


#define UP_BASE "http://download2.erista.me/3hs"
#define UP_VERSION UP_BASE "/version"

#ifdef DEVICE_ID
# define UP_CIA_(v,id) (std::string(UP_BASE) + "/app-" + v + "-" + #id + ".cia")
# define UP_CIA(v,id) UP_CIA_(v,id)
# define DEVID DEVICE_ID
#else
# define UP_CIA_(v,id) (std::string(UP_BASE) + "/app-" + v + ".cia")
# define UP_CIA(v,id) UP_CIA_(v,id)
# define DEVID
#endif

#define VERSION_MAJOR 1
#define VERSION_MINOR 1
#define VERSION_PATCH 0
#define VERSION_DESC "aeraria"

#define INT_TO_STR(i) INT_TO_STR_(i)
#define INT_TO_STR_(i) #i

#define MK_UA(MA,MI,PA) "hShop (3DS/CTR/KTR; ARMv6) 3hs/" MA "." MI "." PA
#define VERSION INT_TO_STR(VERSION_MAJOR) "." INT_TO_STR(VERSION_MINOR) "." INT_TO_STR(VERSION_PATCH)
#define VVERSION "v" VERSION
#define USER_AGENT MK_UA(INT_TO_STR(VERSION_MAJOR), INT_TO_STR(VERSION_MINOR), INT_TO_STR(VERSION_PATCH))

#include <string>


std::string trim(const std::string& str, const std::string& whitespace = " \t\n");
std::string get_url_content(std::string from);
std::string get_latest_version_string();
bool update_app();

#endif
