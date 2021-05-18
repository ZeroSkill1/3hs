
#include "ui/button.hh"

#ifdef USE_SETTINGS_H
# include "settings.hh"
# define BTN_CLR (get_settings()->isLightMode ? ui::constants::COLOR_BTN_LI : ui::constants::COLOR_BTN)
# define HI_CLR (get_settings()->isLightMode ? ui::constants::COLOR_HI_LI : ui::constants::COLOR_HI)
#else
# define BTN_CLR ui::constants::COLOR_BTN
# define HI_CLR ui::consants::COLOR_HI
#endif


ui::Button::Button(std::string label, float x1, float y1, float x2, float y2)
	: Widget("button"), x1(x1), x2(x2), y1(y1), y2(y2)
{
	this->change_label(label);
}

void ui::Button::change_label(std::string label)
{
	this->buf.realloc(label.size() + 1);
	this->label.parse(this->buf, label);
	this->label.optimize();
}

void ui::Button::set_on_click(button_on_click cb)
{
	this->on_click = cb;
}

ui::Results ui::Button::draw(ui::Keys& keys, ui::Scr)
{

	if(this->shouldHighlight)
	{
		C2D_DrawRectSolid(this->x1 - 1, this->y1 - 1, 0, this->x2 - this->x1 + 2, this->y2 - this->y1 + 2, HI_CLR);
	}

	C2D_DrawRectSolid(this->x1, this->y1, 0, this->x2 - this->x1, this->y2 - this->y1, BTN_CLR);
	// "Works but cursed"
	ui::draw_at_absolute((((this->x2 - this->x1) / 2) - (this->label.dimensions(
		ui::constants::FSIZE, ui::constants::FSIZE).width / 2)) + this->x1,
		 ((this->y2 - this->y1) / 2) + (this->y1 - (this->label.dimensions(
		ui::constants::FSIZE, ui::constants::FSIZE).height / 2)), this->label);

	if(this->clickable && keys.touch.px >= this->x1 && keys.touch.px <= this->x2 &&
			keys.touch.py >= this->y1 && keys.touch.py <= this->y2)
		return this->click();

	return ui::Results::go_on;
}

ui::Results ui::Button::click(bool inFrame)
{
	return this->on_click(inFrame);
}

void ui::Button::toggle_click()
{
	this->clickable = this->clickable ? false : true;
}

void ui::Button::highlight(bool value)
{
	this->shouldHighlight = value;
}

