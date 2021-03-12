
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

	ui::parse_text(&ret.text, this->buf, text.text);
	C2D_TextOptimize(&ret.text);
	this->texts.push_back(ret);
}

bool ui::Text::draw(ui::Keys& keys, ui::Scr)
{
	for(ui::_WText& curr : this->texts)
		ui::draw_at(curr.x, curr.y, curr.text);
	return true;
}


ui::WText ui::mkWText(std::string text, int y, int x)
{
	return { text, x, y };
}
