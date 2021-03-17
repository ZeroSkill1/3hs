
#include "ui/confirm.hh"
#include <3rd/log.hh>

ui::Confirm::Confirm(std::string label, bool& res)
	: Widget("confirm"), returns(res)
{
	this->buf = C2D_TextBufNew(CONFIRM_LEN + label.size());
	this->parse_text(&this->yes, this->buf, CONFIRM_YES);
	this->parse_text(&this->no, this->buf, CONFIRM_NO);
	this->parse_text(&this->usr, this->buf, label);

	this->yx = ui::get_center_x(CONFIRM_YES, 0.5f, ui::Scr::bottom) - 40;
	this->nx = ui::get_center_x(CONFIRM_NO, 0.5f, ui::Scr::bottom) + 40;
	this->ux = ui::get_center_x(label, 0.5f, ui::Scr::bottom);

	float chl = ui::char_size()->charWidth;
	this->yfx = ui::get_center_x(CONFIRM_YES, 0.5f, ui::Scr::bottom) - 40 + (chl * CONFIRM_YES_LEN);
	this->nfx = ui::get_center_x(CONFIRM_NO, 0.5f, ui::Scr::bottom) + 40 + (chl * CONFIRM_NO_LEN);
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
