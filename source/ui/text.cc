
#include "ui/text.hh"

//if you want to make multiline text, make a ui::MultiLineText Widget or use a list because I don't want more than one piece of text in this widget

ui::Text::Text(WText txt) : Widget("text")
{
	this->buf = C2D_TextBufNew(TEXT_TXTBUFSIZ);

	_WText ret;
	ret.x = txt.x;
	ret.y = txt.y;
	ret.sizeX = txt.sizeX;
	ret.sizeY = txt.sizeY;

	this->parse_text(&ret.text, this->buf, txt.text);
	this->text = ret;
}

ui::Text::~Text()
{
	C2D_TextBufDelete(this->buf);
}

bool ui::Text::draw(ui::Keys& keys, ui::Scr)
{
	ui::draw_at_absolute(this->text.x, this->text.y, this->text.text, 0, this->text.sizeX, this->text.sizeY);
	return true;
}


ui::WText ui::mkWText(std::string text, float x, float y, float sizeX, float sizeY)
{
	return { text, x, y, sizeX, sizeY };
}

ui::WText ui::mk_center_WText(std::string text, float y, float sizeX, float sizeY)
{
	return { text, ui::get_center_x(text, sizeX), y, sizeX, sizeY };
}

void ui::Text::replace_text(std::string inTxt)
{
	C2D_TextBufClear(this->buf);
	
	ui::Text::parse_text(&this->text.text, this->buf, inTxt);
	this->text.x = ui::get_center_x(inTxt, this->text.sizeX);
}

void ui::Text::parse_text(C2D_Text *outTxt, C2D_TextBuf buf, std::string inTxt)
{
	ui::parse_text(outTxt, buf, inTxt);
	C2D_TextOptimize(outTxt);
}

float ui::get_center_x(std::string text, float sizeX)
{
	float charWidth = ui::char_size()->charWidth;
	return (200.0f - ((float)text.length() * ((sizeX * charWidth)) / 2.0f));
}