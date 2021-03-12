
#include "ui/text.hh"


ui::Text::Text(ui::WText text)
	: Widget("text")
{
	this->buf = C2D_TextBufNew(TEXT_TXTBUFSIZ);
	this->add_text(text);
}

ui::Text::Text(std::vector<ui::WText> texts)
	: Widget("text")
{
	this->buf = C2D_TextBufNew(TEXT_TXTBUFSIZ);
	for(ui::WText& curr : texts)
		this->add_text(curr);
}

ui::Text::~Text()
{
	C2D_TextBufDelete(this->buf);
}


void ui::Text::add_text(WText text)
{
	_WText ret;
	ret.x = text.x;
	ret.y = text.y;
	ret.sizeX = text.sizeX;
	ret.sizeY = text.sizeY;

	ui::parse_text(&ret.text, this->buf, text.text);
	C2D_TextOptimize(&ret.text);
	this->texts.push_back(ret);
}

bool ui::Text::draw(ui::Keys& keys, ui::Scr)
{
	for(ui::_WText& curr : this->texts)
		ui::draw_at_absolute(curr.x, curr.y, curr.text, 0, curr.sizeX, curr.sizeY);
	return true;
}


ui::WText ui::mkWText(std::string text, float x, float y, float sizeX, float sizeY)
{
	return { text, x, y, sizeX, sizeY };
}

ui::WText ui::mk_center_WText(std::string text, float y, float sizeX, float sizeY)
{
	float charWidth = ui::char_size()->charWidth;
	float center_x = (200.0f - ((float)text.length() * ((sizeX * charWidth)) / 2.0f));

	return { text, center_x, y, sizeX, sizeY };
}