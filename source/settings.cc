
#include "settings.hh"

#include <ui/selector.hh>
#include <ui/button.hh>
#include <ui/text.hh>
#include <ui/core.hh>
#include <ui/list.hh>

#include <unistd.h>
#include <stdio.h>

#include <string>

#include "util.hh"

static bool g_loaded = false;
static Settings g_settings;


static lang::type get_lang()
{
	if(!g_loaded) ensure_settings();
	return g_settings.language;
}

bool settings_are_ready()
{ return g_loaded; }

Settings *get_settings()
{ return &g_settings; }

void save_settings()
{
	// Ensures dirs exist
	mkdir("/3ds", 0777);
	mkdir("/3ds/3hs", 0777);

	FILE *settings = fopen(SETTINGS_LOCATION, "w");
	if(settings == NULL) return; // this shouldn't happen, but we'll use an in-mem config

	fwrite(&g_settings, sizeof(Settings), 1, settings);
	fclose(settings);
}

static void write_default_settings()
{
	g_settings.language = i18n::default_lang();
	save_settings();
}

void ensure_settings()
{
	if(g_loaded) return;

	// We want the defaults
	if(!access(SETTINGS_LOCATION, F_OK) == 0)
		write_default_settings();
	else
	{
		FILE *settings = fopen(SETTINGS_LOCATION, "r");
		Settings nset; fread(&nset, sizeof(Settings), 1, settings);
		fclose(settings);

		if(memcmp(nset.magic, "3HSS", 4) == 0)
			g_settings = nset;
		else write_default_settings();
	}

	g_loaded = true;
}

enum SettingsId
{
	ID_LightMode, ID_Resumable,
	ID_FreeSpace, ID_Battery,
	ID_TimeFmt, ID_ProgLoc,
	ID_Language,
};

typedef struct SettingInfo
{
	std::string name;
	std::string desc;
	SettingsId ID;
} SettingInfo;

static std::vector<SettingInfo> g_settings_info =
{
	{ SURESTRING(light_mode)     , "Turn on light mode. This will change\nthe way most ui elements look."                           , ID_LightMode },
	{ SURESTRING(resume_dl)      , "Should we start where we\nleft off downloading the first time\nif we failed the first try?"     , ID_Resumable },
	{ SURESTRING(load_space)     , "Load the free space indicator.\nBootup time should be shorter\nif you disable this on large SDs", ID_FreeSpace },
	{ SURESTRING(show_battery)   , "Toggle visibility of battery in\ntop right corner"                                              , ID_Battery   },
	{ SURESTRING(time_format)    , "Your preferred time format.\nEither 24h or 12h."                                                , ID_TimeFmt   },
	{ SURESTRING(progbar_screen) , "The screen to draw progress bars on"                                                            , ID_ProgLoc   },
	{ SURESTRING(language)       , "The language 3hs is in.\nNote that to update all text you might\nneed to restart 3hs"           , ID_Language  },
};

static std::string serialize_id(SettingsId ID)
{
	switch(ID)
	{
	case ID_LightMode:
		return g_settings.isLightMode ? STRING(btrue) : STRING(bfalse);
	case ID_Resumable:
		return g_settings.resumeDownloads ? STRING(btrue) : STRING(bfalse);
	case ID_FreeSpace:
		return g_settings.loadFreeSpace ? STRING(btrue) : STRING(bfalse);
	case ID_Battery:
		return g_settings.showBattery ? STRING(btrue) : STRING(bfalse);
	case ID_TimeFmt:
		return g_settings.timeFormat == Timefmt::good
			? STRING(fmt_24h) : STRING(fmt_12h);
	case ID_ProgLoc:
		return g_settings.progloc == ProgressBarLocation::top
			? STRING(top) : STRING(bottom);
	case ID_Language:
		return i18n::langname(g_settings.language);
	}

	return STRING(unknown);
}

template <typename TEnum>
static TEnum get_enum(std::vector<std::string> keys, std::vector<TEnum> values, TEnum now)
{
	ui::end_frame();
	ui::Widgets wids;
	TEnum ret = now;

	wids.push_back("selector", new ui::Selector<TEnum>(keys, values, &ret), ui::Scr::bottom);
	wids.get<ui::Selector<TEnum>>("selector")->search_set_idx(now);
	generic_main_breaking_loop(wids);

	return ret;
}

static void update_settings_ID(SettingsId ID)
{
	switch(ID)
	{
	// Boolean
	case ID_LightMode:
		g_settings.isLightMode = !g_settings.isLightMode;
		break;
	case ID_Resumable:
		g_settings.resumeDownloads = !g_settings.resumeDownloads;
		break;
	case ID_FreeSpace:
		g_settings.loadFreeSpace = !g_settings.loadFreeSpace;
		break;
	case ID_Battery:
		g_settings.showBattery = !g_settings.showBattery;
		break;
	// Enums
	case ID_TimeFmt:
		g_settings.timeFormat = get_enum<Timefmt>(
			{ i18n::getstr(str::fmt_24h, get_lang()), i18n::getstr(str::fmt_12h, get_lang()) },
			{ Timefmt::good, Timefmt::bad },
			g_settings.timeFormat
		);
		break;
	case ID_ProgLoc:
		g_settings.progloc = get_enum<ProgressBarLocation>(
			{ STRING(top), STRING(bottom) },
			{ ProgressBarLocation::top, ProgressBarLocation::bottom },
			g_settings.progloc
		);
		break;
	case ID_Language:
		g_settings.language = get_enum<lang::type>(
			{ LANGNAME_ENGLISH, LANGNAME_DUTCH, LANGNAME_GERMAN, LANGNAME_SPANISH, LANGNAME_UWULANG },
			{ lang::english, lang::dutch, lang::german, lang::spanish, lang::uwulang },
			g_settings.language
		);
	}

	save_settings();
}

void show_settings()
{
	toggle_focus();
	ui::Widgets wids;
	ui::Text *value;

	wids.push_back("back", new ui::Button(STRING(back), 240, 210, 310, 230), ui::Scr::bottom);
	wids.get<ui::Button>("back")->set_on_click([](bool) -> ui::Results {
		return ui::Results::quit_loop;
	});

	ui::List<SettingInfo> *list = new ui::List<SettingInfo>(
		[](SettingInfo& entry) -> std::string { return entry.name; },
		[&value](ui::List<SettingInfo> *self, size_t i, u32) -> ui::Results {
			update_settings_ID(self->at(i).ID);
			value->replace_text(PSTRING(value_x, serialize_id(self->at(i).ID)));
			return ui::Results::end_early;
		}, g_settings_info
	);

	ui::WrapText *desc = new ui::WrapText(list->at(0).desc);
	wids.push_back(desc, ui::Scr::bottom);
	desc->set_basey(20); desc->set_pad(10);

	value = new ui::Text(ui::mk_left_WText(PSTRING(value_x, serialize_id(list->at(0).ID)),
		80, 20));
	wids.push_back(value, ui::Scr::bottom);

	list->set_on_change([&desc, &value](ui::List<SettingInfo> *self, size_t i) -> void {
		value->replace_text(PSTRING(value_x, serialize_id(self->at(i).ID)));
		desc->replace_text(self->at(i).desc);
	});

	wids.push_back(list);

	generic_main_breaking_loop(wids);
	toggle_focus();
}

