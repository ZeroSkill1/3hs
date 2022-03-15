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

#ifndef inc_settings_hh
#define inc_settings_hh

#include "i18n.hh"

#define SETTINGS_LOCATION "/3ds/3hs/settings"
#define DICOLOR(light, dark) (get_settings()->isLightMode ? light : dark)

// 24h is superior but we'll allow 12h
// because we care about customizability
enum class Timefmt
{
	good = 24,
	bad = 12
};

enum class ProgressBarLocation
{
	top, bottom
};

enum class LumaLocaleMode
{
	disabled, automatic, manual,
};

typedef struct Settings
{
	char magic[4] = { '3', 'H', 'S', 'S' };
	bool isLightMode = true;
	bool resumeDownloads = true;
	bool loadFreeSpace = true;
	bool showBattery = true;
	bool showNet = true;
	Timefmt timeFormat = Timefmt::good;
	bool unused0 = true; /* used to be: firstRun */
	ProgressBarLocation progloc = ProgressBarLocation::bottom;
	lang::type language = lang::english;
	LumaLocaleMode lumalocalemode = LumaLocaleMode::automatic;
	bool askForExtraContent = true;
	bool warnNoBase = true;
} Settings;


bool settings_are_ready();
Settings *get_settings();
void ensure_settings();
void save_settings();
void show_settings();
void log_settings();

#endif

