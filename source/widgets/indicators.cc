
#include "widgets/indicators.hh"
#include "build/net_icons.h"

#include <3ds.h>

#ifdef USE_SETTINGS_H
# include "settings.hh"
# define BG_CLR (get_settings()->isLightMode ? ui::constants::COLOR_TOP_LI : ui::constants::COLOR_TOP)
#else
# define BG_CLR ui::constants::COLOR_TOP
#endif

#define BG_HEIGHT 10

// FREE SPACE

Result get_free_space(Destination media, u64 *size)
{
	FS_ArchiveResource resource = { 0, 0, 0, 0 };
	Result res = 0;

	switch(media)
	{
	case DEST_TWLNand: res = FSUSER_GetArchiveResource(&resource, SYSTEM_MEDIATYPE_TWL_NAND); break;
	case DEST_CTRNand: res = FSUSER_GetArchiveResource(&resource, SYSTEM_MEDIATYPE_CTR_NAND); break;
	case DEST_Sdmc: res = FSUSER_GetArchiveResource(&resource, SYSTEM_MEDIATYPE_SD); break;
	}

	if(!R_FAILED(res)) *size = resource.clusterSize * resource.freeClusters;
	return res;
}


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

		get_free_space(DEST_TWLNand, &nandt);
		get_free_space(DEST_CTRNand, &nandc);
		get_free_space(DEST_Sdmc, &sdmc);

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
		C2D_DrawRectSolid(0, SCREEN_HEIGHT() - BG_HEIGHT, 0, SCREEN_WIDTH(screen), BG_HEIGHT, BG_CLR);
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
	: Widget("battery_indicator"), percentage(ui::mk_right_WText(
		"0%", 1.0f, 40.0f /* TODO: Make this the length of the battery icon + some extra */,
		0.5f, 0.5f
	))
{
	this->update();
}

void ui::BatteryIndicator::update()
{
	static u8 lvl = 0;
	u8 nlvl = 0;

	if(R_FAILED(MCUHWC_GetBatteryLevel(&nlvl)) || lvl == nlvl)
		return;

	lvl = nlvl;
	this->percentage.replace_text(std::to_string(lvl) + "%");
}

ui::Results ui::BatteryIndicator::draw(ui::Keys& keys, ui::Scr target)
{
#ifdef USE_SETTINGS_H
	if(get_settings()->showBattery)
#endif
	{
		this->update();
		this->percentage.draw(keys, target);
		// TODO: Draw parts with this->level
	}

	return ui::Results::go_on;
}

