
#ifndef inc_settings_hh
#define inc_settings_hh

#define SETTINGS_LOCATION "/3ds/3hs/settings"


typedef struct Settings
{
	char magic[4] = { '3', 'H', 'S', 'S' };
	bool isLightMode = false;
	bool resumeDownloads = true; // TODO: Implement this (resumable downloads)
	bool loadFreeSpace = true;
	bool showBattery = true;
	bool showNet = true;
} Settings;


Settings *get_settings();
void ensure_settings();
void save_settings();
void show_settings();

#endif

