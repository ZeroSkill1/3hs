
#include "ui/scrollingText.hh"

#ifdef USE_SETTINGS_H
#include "settings.hh"
#define OVERLAY_COLOR(scr) (scr == ui::Scr::top \
	? get_settings()->isLightMode ? ui::constants::COLOR_TOP_LI : ui::constants::COLOR_TOP /* top scr */ \
	: get_settings()->isLightMode ? ui::constants::COLOR_BOT_LI : ui::constants::COLOR_BOT /* bot scr */ )
#else
#define OVERLAY_COLOR(scr) (scr == ui::Scr::top ? ui::constants::COLOR_TOP : ui::constants::COLOR_BOT)
#endif

#define CONTINUE_AFTER_X_FRAMES 20
#define MOVE_EVERY_X_FRAMES 3
#define FADE_OUT_MOVES 0

static float diff(float a, float b)
{ return a > b ? a - b : b - a; }

ui::ScrollingText::ScrollingText(float x, float y, std::string text)
	: Widget("scrolling_text"), rtext(text), ogx(x), x(x), y(y)
{
	this->buf = C2D_TextBufNew(text.size() + 1);
	this->replace_text(text);
	this->update_h();
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

size_t ui::ScrollingText::length()
{
	float ret = 0; C2D_TextGetDimensions(&this->text,
		this->sizex, this->sizey, &ret, NULL); return ret;
}

void ui::ScrollingText::move(float x, float y)
{
	this->ogx = this->x = x;
	this->y = y;
}

ui::Results ui::ScrollingText::draw(ui::Keys&, ui::Scr)
{
	if(this->scrolling)
	{
		if(this->offset > strlen)
		{
			this->reset();
		}

		else if(this->offset == 0)
		{
			if(this->timing > CONTINUE_AFTER_X_FRAMES)
				++this->offset;
			++this->timing;
		}

		else if(this->timing % MOVE_EVERY_X_FRAMES == 0)
		{
			this->timing = 1;
			++this->offset;
			--this->x;
		}

		else
		{
			++this->timing;
		}
	}

	ui::draw_at_absolute(this->x, this->y, this->text, 0, ui::constants::FSIZE, ui::constants::FSIZE, 0.0f);
	C2D_DrawRectSolid(0, this->y, Z_OFF_OVERLAY, this->ogx, diff(this->y, this->texth),
		OVERLAY_COLOR(this->screen));

	return ui::Results::go_on;
}

void ui::ScrollingText::update_h()
{
	C2D_TextGetDimensions(&this->text, this->sizex, this->sizey,
		nullptr, &this->texth);
}

void ui::ScrollingText::replace_text(std::string str)
{
	this->rtext = str;
	this->impl_replace_text(str);
	this->strlen = this->length() + FADE_OUT_MOVES;
}

void ui::ScrollingText::impl_replace_text(std::string str)
{
	C2D_TextBufClear(this->buf);
	this->buf = C2D_TextBufResize(this->buf, str.size() + 1);

	ui::parse_text(&this->text, this->buf, str);
	C2D_TextOptimize(&this->text);
}

void ui::ScrollingText::resize(float sizex, float sizey)
{
	this->sizex = sizex;
	this->sizey = sizey;
	this->update_h();
}

void ui::ScrollingText::start_scroll()
{
	this->scrolling = true;
}

void ui::ScrollingText::reset()
{
	this->timing = 1;
	this->offset = 0;
	this->x = ogx;
}

void ui::ScrollingText::stop_scroll()
{
	this->scrolling = false;
	this->reset();
}

void ui::ScrollingText::scroll_if_overflow(ui::Scr screen)
{
	if(this->length() + this->x > SCREEN_WIDTH(screen))
		this->start_scroll();
}

