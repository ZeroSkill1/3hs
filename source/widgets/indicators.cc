#if 0
#include "widgets/indicators.hh"
#include "build/net_icons.h"
#include "build/battery.h"

#include <time.h>
#include <3ds.h>

#include "ctr.hh"

#ifdef USE_SETTINGS_H
# include "settings.hh"
# define BG_CLR (get_settings()->isLightMode ? ui::constants::COLOR_TOP_LI : ui::constants::COLOR_TOP)
#else
# define BG_CLR ui::constants::COLOR_TOP
#endif

#define BG_HEIGHT 10

// FREE SPACE

ui::FreeSpaceIndicator::FreeSpaceIndicator()
	: Widget("free_space_indicator"), sdmc(ui::mk_left_WText(
		"", SCREEN_HEIGHT() - 10, 4.0f, 0.4f, 0.35f
	)), nandt(ui::mk_center_WText(
		"", SCREEN_HEIGHT() - 10, 0.4f, 0.35f
	)), nandc(ui::mk_right_WText(
		"", SCREEN_HEIGHT() - 10, 4.0f, 0.4f, 0.35f
	))
{
	this->force_foreground();
	this->update();
}

void ui::FreeSpaceIndicator::update()
{
#ifdef USE_SETTINGS_H
	if(get_settings()->loadFreeSpace)
#endif
	{
		u64 nandt = 0, nandc = 0, sdmc = 0;

		ctr::get_free_space(DEST_TWLNand, &nandt);
		ctr::get_free_space(DEST_CTRNand, &nandc);
		ctr::get_free_space(DEST_Sdmc, &sdmc);

		this->nandt.replace_text("TWLNand: " + human_readable_size<u64>(nandt));
		this->nandc.replace_text("CTRNand: " + human_readable_size<u64>(nandc));
		this->sdmc.replace_text("SD: " + human_readable_size<u64>(sdmc));
	}
}

ui::Results ui::FreeSpaceIndicator::draw(Keys& keys, Scr screen)
{
#ifdef USE_SETTINGS_H
	if(get_settings()->loadFreeSpace)
#endif
	{
		C2D_DrawRectSolid(0, SCREEN_HEIGHT() - BG_HEIGHT, Z_OFF, SCREEN_WIDTH(screen), BG_HEIGHT, BG_CLR);
		this->nandt.draw(keys, screen);
		this->nandc.draw(keys, screen);
		this->sdmc.draw(keys, screen);
	}

	return ui::Results::go_on;
}

// NET

ui::NetIndicator::NetIndicator()
	: Widget("net_indicator")
{
	this->sheet = c2d::SpriteSheet::from_file(SHEET("net_icons"));

/*	this->sprites[0] = c2d::Sprite::from_sheet(&this->sheet, net_icons_net0_idx);
	this->sprites[1] = c2d::Sprite::from_sheet(&this->sheet, net_icons_net1_idx);
	this->sprites[2] = c2d::Sprite::from_sheet(&this->sheet, net_icons_net2_idx);
	this->sprites[3] = c2d::Sprite::from_sheet(&this->sheet, net_icons_net3_idx);

	// Configure each sprite...
	for(size_t i = 0; i < NET_SPRITE_BUF_LEN; ++i)
	{
		this->sprites[i].move(10, 10);
	}*/
}

ui::Results ui::NetIndicator::draw(ui::Keys&, ui::Scr)
{
#ifdef USE_CONFIG_H
	if(get_settings()->showNet)
#endif
	{
//		this->sprite[osGetWifiStrength()].draw();
	}

	return ui::Results::go_on;
}

// BATTERY

ui::BatteryIndicator::BatteryIndicator()
	: Widget("battery_indicator"), sheet(c2d::SpriteSheet::from_file(SHEET("battery"))),
		percentage(ui::mk_right_WText("0%", 5.0f, 40.0f, 0.5f, 0.5f))
{
	this->update();

	this->light = c2d::Sprite::from_sheet(&this->sheet, battery_battery_light_idx);
	this->dark = c2d::Sprite::from_sheet(&this->sheet, battery_battery_dark_idx);

	this->light.move(SCREEN_WIDTH(ui::Scr::top) - 33.0f, 5.0f);
	this->dark.move(SCREEN_WIDTH(ui::Scr::top) - 33.0f, 5.0f);
}

ui::BatteryIndicator::~BatteryIndicator()
{
	this->sheet.free();
}

void ui::BatteryIndicator::update()
{
	u8 nlvl = 0;

	if(R_FAILED(MCUHWC_GetBatteryLevel(&nlvl)) || this->level == nlvl)
		return;

	this->level = nlvl;
	this->percentage.replace_text(std::to_string(level) + "%");
}

static u8 lvl2batlvl(u8 lvl)
{
	u8 ret = lvl / 25 + 1;
	return ret > 4 ? 4 : ret;
}

ui::Results ui::BatteryIndicator::draw(ui::Keys& keys, ui::Scr target)
{
#ifdef USE_SETTINGS_H
	if(get_settings()->showBattery)
#endif
	{
		this->update();
		this->percentage.draw(keys, target);

		this->draw_lvl(lvl2batlvl(this->level));
	}

	return ui::Results::go_on;
}

void ui::BatteryIndicator::draw_lvl(u8 lvl)
{
#ifdef USE_SETTINGS_H
# define container (get_settings()->isLightMode ? this->light : this->dark)
# define color_green (get_settings()->isLightMode ? C2D_Color32f(0x00, 0xFF, 0x00, 0xFF) \
                                                  : C2D_Color32f(0x00, 0xA2, 0x00, 0xFF))
# define color_red (get_settings()->isLightMode ? C2D_Color32f(0xFF, 0x00, 0x00, 0xFF) \
                                                : C2D_Color32f(0xDA, 0x00, 0x00, 0xFF))
#else
# define container this->dark
# define color_green C2D_Color32f(0x00, 0xFF, 0x00, 0xFF)
# define color_red C2D_Color32f(0xFF, 0x00, 0x00, 0xFF)
#endif

	float width = lvl * 5.0f;
	C2D_DrawRectSolid(SCREEN_WIDTH(ui::Scr::top) - 9.0f - width, 7.0f, 0.0f, width, 12.0f,
		lvl == 1 ? color_red : color_green);

	container.draw();

#undef color_green
#undef color_red
#undef container
}

// TIME

ui::TimeIndicator::TimeIndicator()
	: Widget("time_indicator"), txt(ui::mk_left_WText("00:00:00", 3.0f, 5.0f, 0.4f, 0.4f))
{

}

ui::Results ui::TimeIndicator::draw(ui::Keys& keys, ui::Scr target)
{
	this->txt.replace_text(ui::TimeIndicator::time());
	this->txt.draw(keys, target);
	return ui::Results::go_on;
}

std::string ui::TimeIndicator::time()
{
	time_t now = ::time(nullptr);
	struct tm *tm;
	if((tm = localtime(&now)) == nullptr)
		return "00:00:00";

#ifdef USE_SETTINGS_H
	// 24h aka good
	if(get_settings()->timeFormat == Timefmt::good)
#endif
	{
		constexpr int size = 3 /* hh: */ + 3 /* mm: */ + 2 /* ss */ + 1 /* NULL term */;
		char str[size];

		snprintf(str, size, "%02d:%02d:%02d", tm->tm_hour, tm->tm_min, tm->tm_sec);
		return std::string(str, size);
	}

#ifdef USE_SETTINGS_H
	// 12h aka american aka bad
	else
	{
		constexpr int size = 3 /* hh: */ + 3 /* mm: */ + 2 /* ss */ + 3 /* " PM"/" AM" */ + 1 /* NULL term */;
		char str[size];

		// Why do i have to write branching code
		// for this shit hour format
		// 24h is so neat but NO we americans:tm: must
		// use an anoying version because ???
		// i get its easier to read for humans
		// (not) but its so annoying to deal with
		// times/dates are annoying in general
		// but fuck this

		// Now we need to use PM
		if(tm->tm_hour > 12 && tm->tm_hour != 24)
		{
			// % 12 is a neat little trick to make it not error
			snprintf(str, size, "%02d:%02d:%02d PM", tm->tm_hour % 12, tm->tm_min, tm->tm_sec);
		}

		// 12:00 (24h) should be 12 PM and not 12 or 0 AM
		else if(tm->tm_hour == 12)
		{
			snprintf(str, size, "12:%02d:%02d PM", tm->tm_min, tm->tm_sec);
		}

		// 00:00 (24h) becomes 12 AM (???)
		else if(tm->tm_hour == 0)
		{
			snprintf(str, size, "12:%02d:%02d AM", tm->tm_min, tm->tm_sec);
		}

		// Now we use AM
		else
		{
			snprintf(str, size, "%02d:%02d:%02d AM", tm->tm_hour, tm->tm_min, tm->tm_sec);
		}

		return std::string(str, size);
	}
#endif

}
#endif
