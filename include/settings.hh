
#ifndef inc_settings_hh
#define inc_settings_hh

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

typedef struct Settings
{
	char magic[4] = { '3', 'H', 'S', 'S' };
	bool isLightMode = false;
	bool resumeDownloads = true;
	bool loadFreeSpace = true;
	bool showBattery = true;
	bool showNet = true;
	Timefmt timeFormat = Timefmt::good;
	bool firstRun = true;
	ProgressBarLocation progloc = ProgressBarLocation::bottom;
} Settings;


Settings *get_settings();
void ensure_settings();
void save_settings();
void show_settings();

#endif

