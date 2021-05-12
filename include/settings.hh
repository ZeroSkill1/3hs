
#ifndef __settings_hh__
#define __settings_hh__

#define SETTINGS_LOCATION "/3ds/3hs/settings"

typedef struct Settings
{
	char _magic[4] = { '3', 'H', 'S', 'S' };
	bool isLightMode = false;
	bool resumeDownloads = true; // TODO: Implement this (resumable downloads)
	bool loadFreeSpace = true;
} Settings;


Settings *get_settings();
void ensure_settings();
void save_settings();
void show_settings();

#endif

