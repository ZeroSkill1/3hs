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

#include "settings.hh"

#include <widgets/indicators.hh>
#include <ui/theme.hh>

#include <ui/selector.hh>
#include <ui/swkbd.hh>
#include <ui/list.hh>

#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>

#include <string>

#include "proxy.hh"
#include "util.hh"
#include "log.hh"


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
	cfguInit(); /* ensure cfg is loaded */
	g_settings.language = i18n::default_lang();
	cfguExit(); /* remove cfg reference */
	save_settings();
}

void ensure_settings()
{
	if(g_loaded) return;

	// We want the defaults
	if(access(SETTINGS_LOCATION, F_OK) != 0)
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
	ID_Language, ID_Localemode,
	ID_Extra, ID_Proxy, ID_WarnNoBase,
	ID_MaxELogs,
};

typedef struct SettingInfo
{
	std::string name;
	std::string desc;
	SettingsId ID;
} SettingInfo;

static const char *localemode2str(LumaLocaleMode mode)
{
	switch(mode)
	{
	case LumaLocaleMode::manual: return STRING(manual);
	case LumaLocaleMode::automatic: return STRING(automatic);
	case LumaLocaleMode::disabled: return STRING(disabled);
	}

	return STRING(unknown);
}

static const char *localemode2str_en(LumaLocaleMode mode)
{
	switch(mode)
	{
	case LumaLocaleMode::manual: return "manual";
	case LumaLocaleMode::automatic: return "automatic";
	case LumaLocaleMode::disabled: return "disabled";
	}
	return STRING(unknown);
}

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
	case ID_WarnNoBase:
		return g_settings.warnNoBase ? STRING(btrue) : STRING(bfalse);
	case ID_TimeFmt:
		return g_settings.timeFormat == Timefmt::good
			? STRING(fmt_24h) : STRING(fmt_12h);
	case ID_ProgLoc:
		return g_settings.progloc == ProgressBarLocation::top
			? STRING(top) : STRING(bottom);
	case ID_Language:
		return i18n::langname(g_settings.language);
	case ID_Localemode:
		return localemode2str(g_settings.lumalocalemode);
	case ID_Extra:
		return g_settings.checkForExtraContent ? STRING(btrue) : STRING(bfalse);
	case ID_Proxy:
		return proxy::is_set() ? STRING(press_a_to_view) : STRING(none);
	case ID_MaxELogs:
		return std::to_string(g_settings.maxExtraLogs);
	}

	return STRING(unknown);
}

template <typename TEnum>
static void read_set_enum(const std::vector<std::string>& keys,
	const std::vector<TEnum>& values, TEnum& val)
{
	ui::RenderQueue queue;

	ui::builder<ui::Selector<TEnum>>(ui::Screen::bottom, keys, values, &val)
		.add_to(queue);

	queue.render_finite();
}

static void clean_proxy_content(std::string& s)
{
	if(s.find("https://") == 0)
		s.replace(0, 8, "");
	if(s.find("http://") == 0)
		s.replace(0, 7, "");
	std::string::size_type pos;
	while((pos = s.find(":")) != std::string::npos)
		s.replace(pos, 1, "");
	while((pos = s.find("\n")) != std::string::npos)
		s.replace(pos, 1, "");
}

static void show_update_proxy()
{
	ui::RenderQueue queue;

	ui::Button *password;
	ui::Button *username;
	ui::Button *host;
	ui::Button *port;

	constexpr float w = ui::screen_width(ui::Screen::bottom) - 10.0f;
	constexpr float h = 20;

#define UPDATE_LABELS() do { \
		port->set_label(proxy::proxy().port == 0 ? "" : std::to_string(proxy::proxy().port)); \
		password->set_label(proxy::proxy().password); \
		username->set_label(proxy::proxy().username); \
		host->set_label(proxy::proxy().host); \
	} while(0)

#define BASIC_CALLBACK(name) \
		[&name]() -> bool { \
			ui::RenderQueue::global()->render_and_then([&name]() -> void { \
				SwkbdButton btn; \
				std::string val = ui::keyboard([](ui::AppletSwkbd *swkbd) -> void { \
					swkbd->hint(STRING(name)); \
				}, &btn); \
			\
				if(btn == SWKBD_BUTTON_CONFIRM) \
				{ \
					clean_proxy_content(val); \
					proxy::proxy().name = val; \
					name->set_label(val); \
				} \
			}); \
			return true; \
		}

	/* host */
	ui::builder<ui::Text>(ui::Screen::bottom, STRING(host))
		.x(ui::layout::left)
		.y(10.0f)
		.add_to(queue);
	ui::builder<ui::Button>(ui::Screen::bottom)
		.connect(ui::Button::click, BASIC_CALLBACK(host))
		.size(w, h)
		.x(ui::layout::center_x)
		.under(queue.back())
		.add_to(&host, queue);
	/* port */
	ui::builder<ui::Text>(ui::Screen::bottom, STRING(port))
		.x(ui::layout::left)
		.under(queue.back())
		.add_to(queue);
	ui::builder<ui::Button>(ui::Screen::bottom)
		.connect(ui::Button::click, [&port]() -> bool {
			ui::RenderQueue::global()->render_and_then([&port]() -> void {
				SwkbdButton btn;
				uint64_t val = ui::numpad([](ui::AppletSwkbd *swkbd) -> void {
					swkbd->hint(STRING(port));
				}, 5 /* max is 65535: 5 digits */, &btn);

				if(btn == SWKBD_BUTTON_CONFIRM)
				{
					val &= 0xFFFF; /* limit to a max of 0xFFFF */
					proxy::proxy().port = val;
					port->set_label(std::to_string(val));
				}
			});
			return true;
		})
		.size(w, h)
		.x(ui::layout::center_x)
		.under(queue.back())
		.add_to(&port, queue);
	/* username */
	ui::builder<ui::Text>(ui::Screen::bottom, STRING(username))
		.x(ui::layout::left)
		.under(queue.back())
		.add_to(queue);
	ui::builder<ui::Button>(ui::Screen::bottom)
		.connect(ui::Button::click, BASIC_CALLBACK(username))
		.size(w, h)
		.x(ui::layout::center_x)
		.under(queue.back())
		.add_to(&username, queue);
	/* password */
	ui::builder<ui::Text>(ui::Screen::bottom, STRING(password))
		.x(ui::layout::left)
		.under(queue.back())
		.add_to(queue);
	ui::builder<ui::Button>(ui::Screen::bottom)
		.connect(ui::Button::click, BASIC_CALLBACK(password))
		.size(w, h)
		.x(ui::layout::center_x)
		.under(queue.back())
		.add_to(&password, queue);

	ui::builder<ui::Button>(ui::Screen::bottom, STRING(clear))
		.connect(ui::Button::click, [host, port, username, password]() -> bool {
			proxy::clear();
			UPDATE_LABELS();
			return true;
		})
		.connect(ui::Button::nobg)
		.x(10.0f)
		.y(210.0f)
		.wrap()
		.add_to(queue);

	UPDATE_LABELS();
	queue.render_finite_button(KEY_B | KEY_A);
#undef BASIC_CALLBACK
#undef UPDATE_LABELS
}

static void show_elogs()
{
	uint64_t val;
	do {
		SwkbdButton btn;
		val = ui::numpad([](ui::AppletSwkbd *swkbd) -> void {
			swkbd->hint(STRING(elogs_hint));
		}, 3 /* max is 255: 3 digits */, &btn);

		if(btn != SWKBD_BUTTON_CONFIRM)
			return;
		if(val <= 0xFF)
			break;
	} while(true);
	g_settings.maxExtraLogs = val & 0xFF;
}

static void update_settings_ID(SettingsId ID)
{
	switch(ID)
	{
	// Boolean
	case ID_LightMode:
		g_settings.isLightMode = !g_settings.isLightMode;
		ui::ThemeManager::global()->reget();
		break;
	case ID_Resumable:
		g_settings.resumeDownloads = !g_settings.resumeDownloads;
		break;
	case ID_WarnNoBase:
		g_settings.warnNoBase = !g_settings.warnNoBase;
		break;
	case ID_FreeSpace:
		g_settings.loadFreeSpace = !g_settings.loadFreeSpace;
		// If we switched it from off to on and we've never drawed the widget before
		// It wouldn't draw the widget until another update
		ui::RenderQueue::global()->find_tag<ui::FreeSpaceIndicator>(ui::tag::free_indicator)->update();
		break;
	case ID_Battery:
		g_settings.showBattery = !g_settings.showBattery;
		break;
	case ID_Extra:
		g_settings.checkForExtraContent = !g_settings.checkForExtraContent;
		break;
	// Enums
	case ID_TimeFmt:
		read_set_enum<Timefmt>(
			{ i18n::getstr(str::fmt_24h, get_lang()), i18n::getstr(str::fmt_12h, get_lang()) },
			{ Timefmt::good, Timefmt::bad },
			g_settings.timeFormat
		);
		break;
	case ID_ProgLoc:
		read_set_enum<ProgressBarLocation>(
			{ STRING(top), STRING(bottom) },
			{ ProgressBarLocation::top, ProgressBarLocation::bottom },
			g_settings.progloc
		);
		break;
	case ID_Language:
		read_set_enum<lang::type>(
			{ LANGNAME_ENGLISH, LANGNAME_DUTCH, LANGNAME_GERMAN, LANGNAME_SPANISH, LANGNAME_FRENCH, LANGNAME_ROMANIAN },
			{ lang::english, lang::dutch, lang::german, lang::spanish, lang::french, lang::romanian },
			g_settings.language
		);
		break;
	case ID_Localemode:
		read_set_enum<LumaLocaleMode>(
			{ STRING(automatic), STRING(manual), STRING(disabled) },
			{ LumaLocaleMode::automatic, LumaLocaleMode::manual, LumaLocaleMode::disabled },
			g_settings.lumalocalemode
		);
		break;
	// Other
	case ID_Proxy:
		show_update_proxy();
		break;
	case ID_MaxELogs:
		show_elogs();
		break;
	}
}

void log_settings()
{
#define BOOL(n) g_settings.n ? "true" : "false"
	ilog("settings dump: "
		"isLightMode: %s, "
		"resumeDownloads: %s, "
		"loadFreeSpace: %s, "
		"showBattery: %s, "
		"showNet: %s, "
		"timeFormat: %i, "
		"progloc: %s, "
		"language: %s, "
		"lumalocalemode: %s, "
		"checkForExtraContent: %s, "
		"warnNoBase: %s, "
		"maxExtraLogs: %u",
			BOOL(isLightMode), BOOL(resumeDownloads), BOOL(loadFreeSpace),
			BOOL(showBattery), BOOL(showNet), (int) g_settings.timeFormat,
			g_settings.progloc == ProgressBarLocation::bottom ? "bottom" : "top",
			i18n::langname(g_settings.language), localemode2str_en(g_settings.lumalocalemode),
			BOOL(checkForExtraContent), BOOL(warnNoBase), g_settings.maxExtraLogs);
#undef BOOL
}

void show_settings()
{
	std::vector<SettingInfo> settingsInfo =
	{
		{ STRING(light_mode)     , STRING(light_mode_desc)     , ID_LightMode  },
		{ STRING(resume_dl)      , STRING(resume_dl_desc)      , ID_Resumable  },
		{ STRING(load_space)     , STRING(load_space_desc)     , ID_FreeSpace  },
		{ STRING(show_battery)   , STRING(show_battery_desc)   , ID_Battery    },
		{ STRING(time_format)    , STRING(time_format_desc)    , ID_TimeFmt    },
		{ STRING(progbar_screen) , STRING(progbar_screen_desc) , ID_ProgLoc    },
		{ STRING(language)       , STRING(language_desc)       , ID_Language   },
		{ STRING(lumalocalemode) , STRING(lumalocalemode)      , ID_Localemode },
		{ STRING(check_extra)    , STRING(check_extra_desc)    , ID_Extra      },
		{ STRING(proxy)          , STRING(proxy_desc)          , ID_Proxy      },
		{ STRING(warn_no_base)   , STRING(warn_no_base_desc)   , ID_WarnNoBase },
		{ STRING(max_elogs)      , STRING(max_elogs_desc)      , ID_MaxELogs   },
	};

	panic_assert(settingsInfo.size() > 0, "empty settings meta table");

	using list_t = ui::List<SettingInfo>;

	bool focus = set_focus(true);

	ui::RenderQueue queue;
	ui::Text *value;
	ui::Text *desc;
	list_t *list;

	ui::builder<ui::Text>(ui::Screen::bottom, settingsInfo[0].desc)
		.x(10.0f)
		.y(20.0f)
		.wrap()
		.add_to(&desc, queue);
	ui::builder<ui::Text>(ui::Screen::bottom, PSTRING(value_x, serialize_id(settingsInfo[0].ID)))
		.x(20.0f)
		.under(desc, 5.0f)
		.wrap()
		.add_to(&value, queue);
	ui::builder<list_t>(ui::Screen::top, &settingsInfo)
		.connect(list_t::to_string, [](const SettingInfo& entry) -> std::string { return entry.name; })
		.connect(list_t::select, [value](list_t *self, size_t i, u32 kDown) -> bool {
			((void) kDown);
			ui::RenderQueue::global()->render_and_then([self, i, value]() -> void {
				update_settings_ID(self->at(i).ID);
				value->set_text(PSTRING(value_x, serialize_id(self->at(i).ID)));
			});
			return true;
		})
		.connect(list_t::change, [value, desc](list_t *self, size_t i) -> void {
			value->set_text(PSTRING(value_x, serialize_id(self->at(i).ID)));
			desc->set_text(self->at(i).desc);
			value->set_y(ui::under(desc, value, 5.0f));
		})
		.x(5.0f).y(25.0f)
		.add_to(&list, queue);

	queue.render_finite_button(KEY_B);
	set_focus(focus);

	log_delete_invalid();
	save_settings();
	proxy::write();
}

