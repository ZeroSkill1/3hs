
#include "ui/confirm.hh"


ui::Confirm::Confirm(std::string label, bool& res)
	: Text(mk_center_WText(label, SCREEN_HEIGHT() / 2 - 1)),
		returns(res)
{
	this->name("confirm");
}

bool ui::Confirm::draw(ui::Keys& keys, ui::Scr)
{
	if(keys.kDown & KEY_A)
	{
		this->returns = true;
		return false;
	}

	if(keys.kDown & KEY_B)
	{
		this->returns = false;
		return false;
	}

	ui::draw_at_absolute(this->text.x, this->text.y, this->text.text, 0,
		this->text.sizeX, this->text.sizeY);
	return true;
}
