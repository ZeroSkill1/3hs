
#include "ui/scrollingText.hh"


ui::ScrollingText::ScrollingText(float x, float y, std::string text)
	: Widget("scrolling_text"), rtext(text), x(x), y(y)
{
	this->buf = C2D_TextBufNew(text.size() + 1);
	this->replace_text(text);
}

ui::ScrollingText::ScrollingText()
	: Widget("scrolling_text")
{
	this->buf = C2D_TextBufNew(1);
}

ui::ScrollingText::~ScrollingText()
{
	C2D_TextBufDelete(this->buf);
}

size_t ui::ScrollingText::length(float sizex, float sizey)
{
	float ret = 0; C2D_TextGetDimensions(&this->text,
		sizex, sizey, &ret, NULL); return ret;
}

void ui::ScrollingText::move(float x, float y)
{
	this->x = x;
	this->y = y;
}

ui::Results ui::ScrollingText::draw(ui::Keys&, ui::Scr)
{
	if(this->scrolling)
	{
		// reset
		if(this->index > this->rtext.size())
		{
			this->impl_replace_text(this->rtext);
			this->offset = -60;
			this->index = 0;
		}

		else if(this->offset > 0 && this->offset % 15 == 0)
		{
			this->impl_replace_text(this->rtext.substr(this->index++));
		}

		++this->offset;
	}

	ui::draw_at_absolute(this->x, this->y, this->text);
	return ui::Results::go_on;
}

void ui::ScrollingText::replace_text(std::string str)
{
	this->rtext = str;
	this->impl_replace_text(str);
}

void ui::ScrollingText::impl_replace_text(std::string str)
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

void ui::ScrollingText::reset()
{
	this->offset = -60;
	this->index = 0;
}

void ui::ScrollingText::stop_scroll()
{
	this->scrolling = false;
	this->reset();
}
