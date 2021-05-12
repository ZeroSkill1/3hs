
#include "widgets/indicators.hh"

#ifdef USE_SETTINGS_H
# include "settings.hh"
# define BG_CLR (get_settings()->isLightMode ? ui::constants::COLOR_TOP_LI : ui::constants::COLOR_TOP)
#else
# define BG_CLR ui::constants::COLOR_TOP
#endif

#define BG_HEIGHT 10


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

