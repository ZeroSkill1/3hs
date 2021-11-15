
#ifndef inc_settings_hh
#define inc_settings_hh

#include "i18n.hh"

#define SETTINGS_LOCATION "/3ds/3hs/settings"

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
	bool isLightMode = false;
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
} Settings;


bool settings_are_ready();
Settings *get_settings();
void ensure_settings();
void save_settings();
void show_settings();

#endif

