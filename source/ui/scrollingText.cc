
#include "ui/scrollingText.hh"


ui::ScrollingText::ScrollingText(float x, float y, std::string text)
	: Widget("scrolling_text"), rtext(text), max(text.size()), x(x), y(y)
{
	this->buf = C2D_TextBufNew(text.size() + 1);
	this->replace_text(text);
}

ui::ScrollingText::~ScrollingText()
{
	C2D_TextBufDelete(this->buf);
}


ui::Results ui::ScrollingText::draw(ui::Keys&, ui::Scr)
{
	if(this->scrolling)
	{
		// reset
		if(this->index > this->rtext.size())
		{
			this->offset = 0;
			this->index = 0;
		}

		if(this->offset % 15 == 0)
		{
			this->replace_text(this->rtext.substr(this->index++));
			this->toff = 0;
		}

		++this->offset;
//		this->toff += this->toff > ui::char_size()->charWidth ?
//			0 : 1;
	}

	ui::draw_at_absolute(this->x - this->toff, this->y, this->text);
	return ui::Results::go_on;
}

void ui::ScrollingText::replace_text(std::string str)
{
	C2D_TextBufClear(this->buf);
	this->buf = C2D_TextBufResize(this->buf, str.size() + 1);
	ui::parse_text(&this->text, this->buf, str);
	C2D_TextOptimize(&this->text);
}

void ui::ScrollingText::start_scroll()
{
	this->scrolling = true;
}

void ui::ScrollingText::stop_scroll()
{
	this->scrolling = false;
	this->offset = 0;
}
