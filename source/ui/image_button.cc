
#include "ui/image_button.hh"

#ifdef USE_SETTINGS_H
# include "settings.hh"
# define BTN_CLR (get_settings()->isLightMode ? ui::constants::COLOR_BTN_LI : ui::constants::COLOR_BTN)
# define BTN_IMG (get_settings()->isLightMode ? this->light : this->dark)

#else
# define BTN_CLR ui::constants::COLOR_BTN
#endif


ui::ImageButton::ImageButton(std::string sheet_name, int light_idx, int dark_idx, float x1, float y1, float x2, float y2)
	: Widget("image_button"), x1(x1), x2(x2), y1(y1), y2(y2)
{
	this->init(c2d::SpriteSheet::from_file(sheet_name), light_idx, dark_idx, x1, y1, x2, y2);
}

ui::ImageButton::ImageButton(c2d::SpriteSheet sheet, int light_idx, int dark_idx, float x1, float y1, float x2, float y2)
	: Widget("image_button"), x1(x1), x2(x2), y1(y1), y2(y2)
{
	this->init(sheet, light_idx, dark_idx, x1, y1, x2, y2);	
}

void ui::ImageButton::init(c2d::SpriteSheet sheet, int light_idx, int dark_idx, float x1, float y1, float x2, float y2)
{
	this->sheet = sheet;
	this->dark = c2d::Sprite::from_sheet((&(this->sheet)), dark_idx);
	this->light = c2d::Sprite::from_sheet((&(this->sheet)), light_idx);
	this->dark.set_pos(x1, y1);
	this->light.set_pos(x1, y1);
}

void ui::ImageButton::set_on_click(button_on_click cb)
{
	this->on_click = cb;
}

ui::Results ui::ImageButton::draw(ui::Keys& keys, ui::Scr)
{
	C2D_DrawRectSolid(this->x1, this->y1, 0, this->x2 - this->x1, this->y2 - this->y1, BTN_CLR);
	BTN_IMG.draw();

	// "Works but cursed"

	if(this->clickable && keys.touch.px >= this->x1 && keys.touch.px <= this->x2 &&
			keys.touch.py >= this->y1 && keys.touch.py <= this->y2)
		return this->on_click();

	return ui::Results::go_on;
}

void ui::ImageButton::toggle_click()
{
	this->clickable = this->clickable ? false : true;
}

ui::ImageButton::~ImageButton()
{
	this->sheet.free();
}