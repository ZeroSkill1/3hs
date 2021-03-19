
#include "ui/confirm.hh"
#include <3rd/log.hh>

ui::Confirm::Confirm(std::string label, bool& res)
	: Widget("confirm"), returns(res)
{
	this->buf = C2D_TextBufNew(CONFIRM_LEN + label.size());
	this->parse_text(&this->yes, this->buf, CONFIRM_YES);
	this->parse_text(&this->no, this->buf, CONFIRM_NO);
	this->parse_text(&this->usr, this->buf, label);

	this->yx = ui::get_center_x(&this->yes, constants::FSIZE, constants::FSIZE, ui::Scr::bottom) - 40;
	this->nx = ui::get_center_x(&this->no, constants::FSIZE, constants::FSIZE, ui::Scr::bottom) + 40;
	this->ux = ui::get_center_x(&this->usr, constants::FSIZE, constants::FSIZE, ui::Scr::bottom);

	this->yfx = ui::get_center_x(&this->yes, constants::FSIZE, constants::FSIZE, ui::Scr::bottom) - 40 + ui::text_width(&this->yes);
	this->nfx = ui::get_center_x(&this->no, constants::FSIZE, constants::FSIZE, ui::Scr::bottom) + 40 + ui::text_width(&this->no);
}

static bool close_enough(u16 cmp, u16 cmpto, u16 max)
{
	return (cmp > cmpto && cmp < max);
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

	if(keys.touch.px >= this->yx && keys.touch.px <= this->yfx && close_enough(keys.touch.py, SCREEN_HEIGHT() / 2, 15))
	{
		this->returns = true;
		return false;
	}

	if(keys.touch.px >= this->nx && keys.touch.px <= this->nfx && close_enough(keys.touch.py, SCREEN_HEIGHT() / 2, 15))
	{
		this->returns = false;
		return false;
	}

	ui::draw_at_absolute(this->ux, (SCREEN_HEIGHT() / 2) - 20, this->usr);
	ui::draw_at_absolute(this->yx, (SCREEN_HEIGHT() / 2), this->yes);
	ui::draw_at_absolute(this->nx, (SCREEN_HEIGHT() / 2), this->no);

	return true;
}

void ui::Confirm::parse_text(C2D_Text *outTxt, C2D_TextBuf buf, std::string inTxt)
{
	ui::parse_text(outTxt, buf, inTxt);
	C2D_TextOptimize(outTxt);
}
