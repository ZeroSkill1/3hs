
#include "settings.hh"
#include <ui/image_button.hh>
#include <ui/button.hh>
#include <ui/text.hh>
#include <ui/core.hh>
#include <ui/list.hh>

#include <unistd.h>
#include <stdio.h>

#include <string>

static Settings g_settings;


Settings *get_settings()
{ return &g_settings; }

void save_settings()
{
	FILE *settings = fopen(SETTINGS_LOCATION, "w");
	fwrite(&g_settings, sizeof(Settings), 1, settings);
	fclose(settings);
}

void ensure_settings()
{
	// We want the defaults
	if(!access(SETTINGS_LOCATION, F_OK) == 0)
		save_settings();
	else
	{
		FILE *settings = fopen(SETTINGS_LOCATION, "r");
		Settings nset; fread(&nset, sizeof(Settings), 1, settings);
		fclose(settings);

		if(memcmp(nset._magic, "3HSS", 4) == 0)
			g_settings = nset;
		else save_settings();
	}
}

enum SettingsId
{
	ID_LightMode, ID_Resumable,
	ID_FreeSpace, ID_Battery,
};

typedef struct SettingInfo
{
	std::string name;
	std::string desc;
	SettingsId ID;
} SettingInfo;

static std::vector<SettingInfo> g_settings_info =
{
	{ "Light Mode"               , "Turn on light mode. This will change\nthe way most ui elements look."                           , ID_LightMode },
	{ "Resume Downloads"         , "Should we start where we\nleft off downloading the first time\nif we failed the first try?"     , ID_Resumable },
	{ "Load Free Space indicator", "Load the free space indicator.\nBootup time should be shorter\nif you disable this on large SDs", ID_FreeSpace },
	{ "Show Battery"             , "Toggle visability of battery in\ntop right corner"                                              , ID_Battery   },
};

static std::string serialize_id(SettingsId ID)
{
	switch(ID)
	{
	case ID_LightMode:
		return g_settings.isLightMode ? "true" : "false";
	case ID_Resumable:
		return g_settings.resumeDownloads ? "true" : "false";
	case ID_FreeSpace:
		return g_settings.loadFreeSpace ? "true" : "false";
	case ID_Battery:
		return g_settings.showBattery ? "true" : "false";
	default:
		return "Unknown";
	}
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
	}
	save_settings();
}

void show_settings()
{
	ui::wid()->for_each("button", [](ui::Widget *widget) -> void { ((ui::Button *) widget)->toggle(); });
	ui::wid()->for_each("image_button", [](ui::Widget *widget) -> void { ((ui::ImageButton *) widget)->toggle(); });
	ui::wid()->get<ui::Text>("curr_action_desc")->toggle();

	ui::Widgets wids;
	ui::Text *value;

	wids.push_back("back", new ui::Button("Back", 240, 210, 310, 230), ui::Scr::bottom);
	wids.get<ui::Button>("back")->set_on_click([]() -> ui::Results {
		return ui::Results::quit_loop;
	});

	ui::List<SettingInfo> *list = new ui::List<SettingInfo>(
		[](SettingInfo& entry) -> std::string { return entry.name; },
		[&value](ui::List<SettingInfo> *self, size_t i, u32) -> ui::Results {
			update_settings_ID(self->at(i).ID);
			value->replace_text("Value: " + serialize_id(self->at(i).ID));
			return ui::Results::go_on;
		}, g_settings_info
	);

	ui::WrapText *desc = new ui::WrapText(list->at(0).desc);
	wids.push_back(desc, ui::Scr::bottom);
	desc->set_basey(20); desc->set_pad(10);

	value = new ui::Text(ui::mk_left_WText("Value: " + serialize_id(list->at(0).ID), 80, 20));
	wids.push_back(value, ui::Scr::bottom);

	list->set_on_change([&desc, &value](ui::List<SettingInfo> *self, size_t i) -> void {
		value->replace_text("Value: " + serialize_id(self->at(i).ID));
		desc->replace_text(self->at(i).desc);
	});

	wids.push_back(list);

	generic_main_breaking_loop(wids);

	ui::wid()->for_each("button", [](ui::Widget *widget) -> void { ((ui::Button *) widget)->toggle(); });
	ui::wid()->for_each("image_button", [](ui::Widget *widget) -> void { ((ui::ImageButton *) widget)->toggle(); });
	ui::wid()->get<ui::Text>("curr_action_desc")->toggle();
}

