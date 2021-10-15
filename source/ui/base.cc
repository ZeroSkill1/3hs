
#include <ui/base.hh>

/* internal constants */
#define SPRITESHEET_PATH "romfs:/gfx/next.t3x"

/* global variables */
static C3D_RenderTarget *g_top;
static C3D_RenderTarget *g_bot;

static ui::RenderQueue g_renderqueue;
static ui::SpriteStore g_spritestore;


/* helpers */

float ui::transform(ui::BaseWidget *wid, float v)
{
	if(v >= 0.0f) return v;
	switch((int) v)
	{
	case (int) ui::layout::bottom:
		return ui::dimensions::height - wid->height() - 3.0f; /* padding of 3.0f */
	case (int) ui::layout::right:
		return ui::screen_width(wid->renders_on()) - wid->width() - 3.0f; /* padding of 3.0f */
	case (int) ui::layout::center_x:
		return (ui::screen_width(wid->renders_on()) / 2) - (wid->width() / 2.0f);
	case (int) ui::layout::center_y:
		return (ui::dimensions::height / 2) - (wid->height() / 2.0f);
	}

	return 0.0f;
}

float ui::under(ui::BaseWidget *from, ui::BaseWidget *newel, float pad)
{
	((void) newel); /* newel is here for consistency */
	return from->get_y() + from->height() + pad;
}

float ui::above(ui::BaseWidget *from, ui::BaseWidget *newel, float pad)
{
	return from->get_y() - newel->height() - pad;
}

float ui::right(ui::BaseWidget *from, ui::BaseWidget *newel, float pad)
{
	((void) newel); /* newel is here for consistency */
	return from->get_x() + from->width() + pad;
}

float ui::left(ui::BaseWidget *from, ui::BaseWidget *newel, float pad)
{
	return from->get_x() - newel->width() - pad;
}

void ui::exit()
{
	C2D_Fini();
	C3D_Fini();
	gfxExit();
}

void ui::init(C3D_RenderTarget *top, C3D_RenderTarget *bot)
{
	g_spritestore.open(SPRITESHEET_PATH);

	g_top = top;
	g_bot = bot;
}

bool ui::init()
{
	g_spritestore.open(SPRITESHEET_PATH);

	gfxInitDefault();
	if(!C3D_Init(C3D_DEFAULT_CMDBUF_SIZE)) return false;
	if(!C2D_Init(C2D_DEFAULT_MAX_OBJECTS)) return false;
	C2D_Prepare();

	g_bot = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);
	g_top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);

	return true;
}

/* class RenderQueue */

ui::RenderQueue::~RenderQueue()
{
	for(ui::BaseWidget *wid : this->bot)
	{ wid->destroy(); delete wid; }
	for(ui::BaseWidget *wid : this->top)
	{ wid->destroy(); delete wid; }

	this->top.clear();
	this->bot.clear();
}

void ui::RenderQueue::push(ui::BaseWidget *wid)
{
	if(wid->renders_on() == ui::Screen::bottom)
		this->bot.push_back(wid);
	if(wid->renders_on() == ui::Screen::top)
		this->top.push_back(wid);
	this->backPtr = wid;
}

ui::BaseWidget *ui::RenderQueue::back()
{
	return this->backPtr;
}

ui::Keys ui::RenderQueue::get_keys()
{
	hidScanInput();
	ui::Keys keys = {
		hidKeysDown(), hidKeysHeld(), hidKeysUp(),
		{ 0, 0 }
	};
	hidTouchRead(&keys.touch);
	return keys;
}

bool ui::RenderQueue::render_frame(const ui::Keys& keys)
{
	C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
	C2D_TargetClear(g_top, C2D_Color32(0x1C, 0x20, 0x21, 0xFF)); /* TODO: Theme intergration */
	C2D_TargetClear(g_bot, C2D_Color32(0x1C, 0x20, 0x21, 0xFF));
	bool ret = true;

	C2D_SceneBegin(g_top);
	for(ui::BaseWidget *wid : this->top)
		if(!wid->is_hidden()) ret &= wid->render(keys);
	ret &= g_renderqueue.render_top(keys);

	C2D_SceneBegin(g_bot);
	for(ui::BaseWidget *wid : this->bot)
		if(!wid->is_hidden()) ret &= wid->render(keys);
	ret &= g_renderqueue.render_bottom(keys);

	C3D_FrameEnd(0);
	return ret;
}

bool ui::RenderQueue::render_frame()
{
	ui::Keys keys = ui::RenderQueue::get_keys();
	return this->render_frame(keys);
}

bool ui::RenderQueue::render_bottom(const ui::Keys& keys)
{
	bool ret = true;
	for(ui::BaseWidget *wid : this->bot)
		if(!wid->is_hidden()) ret &= wid->render(keys);
	return ret;
}

bool ui::RenderQueue::render_top(const ui::Keys& keys)
{
	bool ret = true;
	for(ui::BaseWidget *wid : this->top)
		if(!wid->is_hidden()) ret &= wid->render(keys);
	return ret;
}

void ui::RenderQueue::render_forever()
{
	while(true) this->render_frame();
}

void ui::RenderQueue::render_until_button(u32 kDownMask)
{
	ui::Keys keys = ui::RenderQueue::get_keys();
	while((keys.kDown & kDownMask) == 0)
		this->render_frame(keys), keys = ui::RenderQueue::get_keys();
}

void ui::RenderQueue::render_finite()
{
	while(this->render_frame())
		;
}

void ui::RenderQueue::render_finite_button(u32 kDownMask)
{
	ui::Keys keys = ui::RenderQueue::get_keys();
	while((keys.kDown & kDownMask) == 0 && this->render_frame(keys))
		keys = ui::RenderQueue::get_keys();
}

ui::RenderQueue *ui::RenderQueue::global()
{ return &g_renderqueue; }

/* class SpriteStore */

ui::SpriteStore::SpriteStore(const std::string& fname)
{
	this->open(fname);
}

ui::SpriteStore::~SpriteStore()
{
	if(this->sheet != nullptr)
		C2D_SpriteSheetFree(this->sheet);
}

void ui::SpriteStore::open(const std::string& fname)
{
	this->sheet = C2D_SpriteSheetLoad(fname.c_str());
}

size_t ui::SpriteStore::size()
{
	return C2D_SpriteSheetCount(this->sheet);
}

C2D_Sprite ui::SpriteStore::get_by_id(ui::sprite id)
{
	C2D_Sprite ret; C2D_SpriteFromSheet(&ret, g_spritestore.sheet, (size_t) id);
	return ret;
}


/* core widget class Text */

void ui::next::Text::setup(const std::string& label)
{
	this->text = label;
	this->prepare_arrays();
//	this->buf = C2D_TextBufNew(label.size() + 1);
//	C2D_TextParse(&this->label, this->buf, label.c_str());
//	C2D_TextOptimize(&this->label);
}

void ui::next::Text::prepare_arrays()
{
	if(this->buf == nullptr) this->buf = C2D_TextBufNew(this->text.size() + 1);
	else
	{
		// Reset
		C2D_TextBufClear(this->buf);
		this->buf = C2D_TextBufResize(this->buf, this->text.size());
		this->lines.clear();
	}

	int width = ui::screen_width(this->screen);
	int lines = (this->text.size() / (width - this->x)) + 1;
	this->lines.reserve(lines);

	int curWidth = this->x;
	std::string cur;

	for(size_t i = 0; i < this->text.size(); ++i)
	{
		if((this->doAutowrap && width - curWidth < 0) || this->text[i] == '\n')
		{
			curWidth = this->x;
			this->push_str(cur);
			cur.clear();

			if(this->text[i] == '\n')
				continue;
		}

		cur.push_back(this->text[i]);
		if(this->doAutowrap)
		{
			charWidthInfo_s *ch = C2D_FontGetCharWidthInfo(NULL, C2D_FontGlyphIndexFromCodePoint(NULL, this->text[i]));
			if(ch != NULL) curWidth += ch->glyphWidth;
		}
	}

	if(cur.size() > 0) this->push_str(cur);
	if(this->lines.size() > 0)
	{
		C2D_TextGetDimensions(&this->lines.front(), this->xsiz, this->ysiz, nullptr,
			&this->lineHeigth);
	}
	else this->lineHeigth = 0;
}

void ui::next::Text::push_str(const std::string& str)
{
	this->lines.emplace_back();
	C2D_TextParse(&this->lines.back(), this->buf, str.c_str());
	C2D_TextOptimize(&this->lines.back());
}

void ui::next::Text::destroy()
{
	C2D_TextBufDelete(this->buf);
}

static float get_center_x(C2D_Text *txt, float xsiz, float ysiz, ui::Screen screen)
{
	float width; C2D_TextGetDimensions(txt, xsiz, ysiz, &width, nullptr);
	return ((ui::screen_width(screen) / 2) - (width / 2.0f));
}

bool ui::next::Text::render(const ui::Keys& keys)
{
	((void) keys);

	/* TODO: Respect theme colors */

	for(size_t i = 0; i < this->lines.size(); ++i)
	{
		if(this->drawCenter)
		{
			float center = get_center_x(&this->lines[i], this->xsiz, this->ysiz,
				this->screen);

			C2D_DrawText(&this->lines[i], C2D_WithColor, center,
				this->y + (this->lineHeigth * i), this->z, this->xsiz, this->ysiz, C2D_Color32(0xFF, 0xFF, 0xFF, 0xFF));
		}
		else
		{
			C2D_DrawText(&this->lines[i], C2D_WithColor, this->x,
				this->y + (this->lineHeigth * i), this->z, this->xsiz, this->ysiz, C2D_Color32(0xFF, 0xFF, 0xFF, 0xFF));
		}
	}

	return true;
}

void ui::next::Text::autowrap()
{
	this->doAutowrap = true;
	this->prepare_arrays();
}

void ui::next::Text::set_center_x()
{
	this->drawCenter = true;
}

float ui::next::Text::height()
{
	return this->lines.size() * this->lineHeigth;
}

float ui::next::Text::width()
{
	float ret = 0.0f;
	for(C2D_Text& line : this->lines)
	{
		float width;
		C2D_TextGetDimensions(&line, this->xsiz, this->ysiz, &width, nullptr);
		if(width > ret) ret = width;
	}
	return ret;
}

void ui::next::Text::resize(float x, float y)
{
	this->xsiz = x;
	this->ysiz = y;
}

/* core widget class Sprite */

void ui::next::Sprite::setup(C2D_Sprite sprite)
{
	// Compatibility with other methods
	C2D_SpriteSetCenter(&sprite, 0.0f, 0.0f);
	this->sprite = sprite;
}

float ui::next::Sprite::height()
{
	return this->sprite.image.subtex->height;
}

float ui::next::Sprite::width()
{
	return this->sprite.image.subtex->width;
}

bool ui::next::Sprite::render(const ui::Keys& keys)
{
	((void) keys); // unused
	C2D_DrawSprite(&this->sprite);
	return true;
}

void ui::next::Sprite::set_x(float x)
{
	this->x = ui::transform(this, x);
	C2D_SpriteSetPos(&this->sprite, x, this->y);
}

void ui::next::Sprite::set_y(float y)
{
	this->y = ui::transform(this, y);
	C2D_SpriteSetPos(&this->sprite, this->x, this->y);
}

void ui::next::Sprite::set_z(float z)
{
	this->z = z;
	C2D_SpriteSetDepth(&this->sprite, z);
}

/* core widget class Button */

void ui::next::Button::setup(const std::string& label)
{
	this->label.setup(this->screen, label);
	this->readjust();
}

void ui::next::Button::setup()
{
	this->label.setup(this->screen, "");
}

float ui::next::Button::height()
{ return this->h; }

float ui::next::Button::width()
{ return this->w; }

void ui::next::Button::resize(float x, float y)
{
	this->w = x;
	this->h = y;
	this->readjust();
}

void ui::next::Button::resize_children(float x, float y)
{
	this->label.ptr()->resize(x, y);
	this->readjust();
}

void ui::next::Button::set_x(float x)
{
	this->x = x;
	this->readjust();
}

void ui::next::Button::set_y(float y)
{
	this->y = y;
	this->readjust();
}

void ui::next::Button::set_z(float z)
{
	this->z = z;
	this->label.ptr()->set_z(z);
}

void ui::next::Button::readjust()
{
	// Getting better results with the -1.0f somehow
	this->label.ptr()->set_y(((this->h / 2) - (this->label.ptr()->height() / 2)) + this->y);
	this->label.ptr()->set_x(((this->w / 2) - (this->label.ptr()->width() / 2)) + this->x);

	this->ox = this->x + x;
	this->oy = this->y + y;
}

bool ui::next::Button::render(const ui::Keys& keys)
{
	/* TODO: Listen to theme */
	C2D_DrawRectSolid(this->x, this->y, this->z, this->w, this->h, C2D_Color32(0x32, 0x35, 0x36, 0xFF));
	this->label.render(keys);

	if(keys.touch.px >= this->x && keys.touch.px <= this->ox &&
			keys.touch.py >= this->y && keys.touch.py <= this->oy)
		return this->on_click();

	return true;
}

void ui::next::Button::autowrap()
{
	this->w = this->label.ptr()->width() + 10.0f;
//	this->h = this->label.ptr()->height() + 2.0f; // We don't want to wrap the heigth usually
	this->readjust();
}

void ui::next::Button::connect(connect_type type, std::function<bool()> callback)
{
	switch(type)
	{
	case ui::next::Button::click:
		this->on_click = callback;
		break;
	default:
		//panic("Button::connect(): invalid state");
		break;
	}
}

/* core widget class ButtonCallback */

void ui::next::ButtonCallback::setup(u32 keys)
{ this->keys = keys; }

bool ui::next::ButtonCallback::render(const ui::Keys& keys)
{
	switch(this->type)
	{
	case ui::next::ButtonCallback::kdown:
		if(keys.kDown & this->keys)
			return this->cb(keys.kDown);
		break;
	case ui::next::ButtonCallback::kheld:
		if(keys.kHeld & this->keys)
			return this->cb(keys.kHeld);
		break;
	case ui::next::ButtonCallback::kup:
		if(keys.kUp & this->keys)
			return this->cb(keys.kUp);
		break;
	case ui::next::ButtonCallback::none:
		break;
	}
	return true;
}

void ui::next::ButtonCallback::connect(ui::next::ButtonCallback::connect_type type, std::function<bool(u32)> cb)
{
	this->type = type;
	this->cb = cb;
}

