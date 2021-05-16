
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

std::string ui::Text::value()
{ return this->text.text; }

ui::Results ui::Text::draw(ui::Keys&, ui::Scr)
{
	ui::draw_at_absolute(this->text.x, this->text.y, this->text.ctext, 0,
		this->text.sizeX, this->text.sizeY);
	return ui::Results::go_on;
}

ui::WText& ui::Text::gtext()
{ return this->text; }

ui::WText ui::mkWText(std::string text, float x, float y, float sizeX, float sizeY, std::function<void(WText *)> callback)
{
	return { callback, text, sizeX, sizeY, x, y };
}

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

ui::WText ui::mk_left_WText(std::string text, float y, float pad, float sizeX, float sizeY)
{
	return ui::mkWText(text, pad, y, sizeX, sizeY, [](WText *) -> void { });
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

float ui::get_center_x_txt(std::string txt, float sizeX, float sizeY, ui::Scr screen)
{
	C2D_TextBuf buf = C2D_TextBufNew(txt.size() + 1);
	C2D_Text text; ui::parse_text(&text, buf, txt);

	float width = ui::text_width(&text, sizeX, sizeY);
	C2D_TextBufDelete(buf);

	return ((SCREEN_WIDTH(screen) / 2) - (width / 2.0f));
}

float ui::text_width(C2D_Text *txt, float sizeX, float sizeY)
{
	float width; C2D_TextGetDimensions(txt, sizeX, sizeY, &width, NULL);
	return width;
}

float ui::text_height(C2D_Text *txt, float sizeX, float sizeY)
{
	float height; C2D_TextGetDimensions(txt, sizeX, sizeY, NULL, &height);
	return height;
}

// WrapText

ui::WrapText::WrapText(std::string text_)
	: Widget("wrap_text"), text(text_) { }

ui::WrapText::WrapText()
	: Widget("wrap_text") { }

ui::WrapText::~WrapText()
{
	C2D_TextBufDelete(this->buf);
}

void ui::WrapText::pre_push()
{
	if(this->buf == NULL) this->buf = C2D_TextBufNew(this->text.size() + 1);


	else
	{
		C2D_TextBufClear(this->buf);
		this->buf = C2D_TextBufResize(this->buf, this->text.size());
		this->lines.clear();
	}

	int width = SCREEN_WIDTH(this->screen);
	int lines = (this->text.size() / (width - this->pad)) + 1;
	this->lines.clear(); this->lines.reserve(lines * sizeof(C2D_Text));

	std::string cur;
//	float curWidth = this->baseY;

	for(size_t i = 0; i < this->text.size(); ++i)
	{
		// TODO: Fix actual wrap
		if(/*(int) curWidth % (int) (width - this->pad) == 0) || */this->text[i] == '\n')
		{
//			curWidth = this->baseY;
			this->push_str(cur);
			cur.clear();
			if(this->text[i] == '\n')
				continue;
		}

		cur.push_back(this->text[i]);
//		charWidthInfo_s *ch = C2D_FontGetCharWidthInfo(NULL, this->text[i]);
//		if(ch != NULL) curWidth += ch->charWidth;
	}
	if(cur.size() > 0) this->push_str(cur);
}

void ui::WrapText::push_str(std::string str)
{
	C2D_Text txt;

	ui::parse_text(&txt, this->buf, str);
	C2D_TextOptimize(&txt);

	this->lines.push_back(txt);
}

ui::Results ui::WrapText::draw(ui::Keys&, ui::Scr)
{
	size_t lhei = 0;
	if(this->lines.size() > 0)
		lhei = ui::text_height(&this->lines[0]);

	for(size_t i = 0; i < this->lines.size(); ++i)
	{
		if(this->drawCenter)
		{
			ui::draw_at_absolute(ui::get_center_x(&this->lines[i], ui::constants::FSIZE,
				ui::constants::FSIZE, this->screen), this->baseY + (lhei * i), this->lines[i]);
		}
		else
			ui::draw_at_absolute(this->pad, this->baseY + (lhei * i), this->lines[i]);
	}

	return ui::Results::go_on;
}

void ui::WrapText::replace_text(std::string txt)
{
	this->text = txt;
	this->pre_push();
}

void ui::WrapText::set_pad(float pad)
{
	if(pad < SCREEN_WIDTH(this->screen))
		this->pad = pad;
}

void ui::WrapText::set_basey(float baseY)
{
	if(baseY > 0) this->baseY = baseY;
}

void ui::WrapText::center()
{
	this->drawCenter = true;
	this->pad = 0;
	this->pre_push();
}

