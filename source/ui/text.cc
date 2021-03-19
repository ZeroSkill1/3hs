
#include "ui/text.hh"

// If you want to make multiline text, make a ui::MultiLineText Widget or use
// A list because I don't want more than one piece of text in this widget


ui::Text::Text(WText txt)
	: Widget("text")
{
	this->buf = C2D_TextBufNew(txt.text.size() + 1);
	this->parse_text(&txt.ctext, this->buf, txt.text);
	txt.calc(&txt);
	this->text = txt;
}

ui::Text::~Text()
{
	C2D_TextBufDelete(this->buf);
}

bool ui::Text::draw(ui::Keys&, ui::Scr)
{
	ui::draw_at_absolute(this->text.x, this->text.y, this->text.ctext, 0,
		this->text.sizeX, this->text.sizeY);
	return true;
}


ui::WText ui::mkWText(std::string text, float x, float y, float sizeX, float sizeY, std::function<void(WText *)> callback)
{
	return { callback, text, sizeX, sizeY, x, y };
}
#include <3rd/log.hh>
ui::WText ui::mk_center_WText(std::string text, float y, float sizeX, float sizeY, ui::Scr screen)
{
	return ui::mkWText(text, 0, y, sizeX, sizeY, [screen](WText *self) -> void {
		self->x = ui::get_center_x(&self->ctext, self->sizeX, self->sizeY, screen);
	});
}

ui::WText ui::mk_right_WText(std::string text, float y, float pad, float sizeX, float sizeY, ui::Scr screen)
{
	return ui::mkWText(text, 0, y, sizeX, sizeY, [screen, pad](WText *self) -> void {
		self->x = SCREEN_WIDTH(screen) - pad - ui::text_width(&self->ctext,
			self->sizeX, self->sizeY);
	});
}

void ui::Text::replace_text(std::string inTxt)
{
	C2D_TextBufClear(this->buf);
	this->buf = C2D_TextBufResize(this->buf, inTxt.size() + 1);

	this->text.text = inTxt;
	this->parse_text(&this->text.ctext, this->buf, this->text.text);
	this->text.calc(&this->text);
}

void ui::Text::parse_text(C2D_Text *outTxt, C2D_TextBuf buf, std::string inTxt)
{
	ui::parse_text(outTxt, buf, inTxt);
	C2D_TextOptimize(outTxt);
}

float ui::get_center_x(C2D_Text *txt, float sizeX, float sizeY, ui::Scr screen)
{
	float width = ui::text_width(txt, sizeX, sizeY);
	return ((SCREEN_WIDTH(screen) / 2) - (width / 2.0f));
}

float ui::text_width(C2D_Text *txt, float sizeX, float sizeY)
{
	float width;
	C2D_TextGetDimensions(txt, sizeX, sizeY, &width, NULL);
	return width;
}
