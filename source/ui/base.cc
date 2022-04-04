/*
 * Copyright (C) 2021-2022 MyPasswordIsWeak
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include <ui/base.hh>
#include <panic.hh>

#include "settings.hh"
#include "i18n.hh"
#include "log.hh"

/* internal constants */
#define SPRITESHEET_PATH "romfs:/gfx/next.t3x"

/* global variables */
static C3D_RenderTarget *g_top;
static C3D_RenderTarget *g_bot;

static ui::RenderQueue g_renderqueue;
static ui::SpriteStore g_spritestore;

/* used for debugging purposes */
static bool g_inRender = false;

static ui::SlotManager slotmgr { nullptr };


/* helpers */

static inline float center_pos(float max, float width)
{
	return max / 2.0f - width / 2.0f;
}

float ui::transform(ui::BaseWidget *wid, float v)
{
	if(v >= 0.0f) return v;
	switch((int) v)
	{
	case (int) ui::layout::bottom:
		return ui::dimensions::height - wid->height() - 10.0f; /* padding of 10.0f */
	case (int) ui::layout::right:
		return ui::screen_width(wid->renders_on()) - wid->width() - 10.0f; /* padding of 10.0f */
	case (int) ui::layout::center_x:
		return center_pos(ui::screen_width(wid->renders_on()), wid->width());
	case (int) ui::layout::center_y:
		return center_pos(ui::dimensions::height, wid->height());
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

void ui::set_double_center(BaseWidget *first, BaseWidget *second, float pad)
{
	float w1 = first->width();
	float w2 = second->width();
	float total = w1 + w2 + pad;

	float start = center_pos(ui::screen_width(first->renders_on()), total);
	first->set_x(start);
	second->set_x(start + w1 + pad);
}

float ui::center_align_y(BaseWidget *from, BaseWidget *newel)
{
	return center_pos(from->height(), newel->height()) + from->get_y();
}

void ui::exit()
{
	C2D_Fini();
	C3D_Fini();
	gfxExit();
}

void ui::notice(const std::string& msg)
{
	ui::RenderQueue queue;

	ui::builder<ui::Text>(ui::Screen::top, STRING(press_a_exit))
		.x(ui::layout::center_x)
		.y(ui::layout::bottom)
		.add_to(queue);

	ui::builder<ui::Text>(ui::Screen::top, msg)
		.x(ui::layout::center_x)
		.y(70.0f)
		.wrap()
		.add_to(queue);

	queue.render_finite_button(KEY_A | KEY_START);
}

u32 ui::color_button() { return DICOLOR(UI_COLOR(DE,DE,DE,FF), UI_COLOR(32,35,36,FF)); }
u32 ui::color_text() { return DICOLOR(UI_COLOR(00,00,00,FF), UI_COLOR(FF,FF,FF,FF)); }
u32 ui::color_bg() { return DICOLOR(UI_COLOR(FF,FF,FF,FF), UI_COLOR(1C,20,21,FF)); }

static ui::slot_color_getter slotmgr_getters[] = {
	ui::color_bg
};

void ui::init(C3D_RenderTarget *top, C3D_RenderTarget *bot)
{
	g_spritestore.open(SPRITESHEET_PATH);

	g_top = top;
	g_bot = bot;

	slotmgr = ThemeManager::global()->get_slots(nullptr, "__global_slot_manager", 1, slotmgr_getters);
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

	slotmgr = ThemeManager::global()->get_slots(nullptr, "__global_slot_manager", 1, slotmgr_getters);
	return true;
}

/* class RenderQueue */

ui::RenderQueue::~RenderQueue()
{
	this->clear();
}

void ui::RenderQueue::clear()
{
	for(ui::BaseWidget *wid : this->bot)
	{ wid->destroy(); delete wid; }
	for(ui::BaseWidget *wid : this->top)
	{ wid->destroy(); delete wid; }

	this->signalBit = 0;

	this->backPtr = nullptr;
	this->top.clear();
	this->bot.clear();

	this->detach_after();
}

void ui::RenderQueue::push(ui::BaseWidget *wid)
{
	if(wid->renders_on() == ui::Screen::bottom)
		this->bot.push_back(wid);
	if(wid->renders_on() == ui::Screen::top)
		this->top.push_back(wid);
	this->backPtr = wid;
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
	if(this->signalBit & ui::RenderQueue::signal_cancel)
		return false;

	if(!aptMainLoop())
		::exit(0); /* finish */
	if(g_inRender)
	{
		C3D_FrameEnd(0); /* we first need to finish because panic() will render */
		g_inRender = false;
		panic("illegal double render");
	}

	C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
	g_inRender = true;

	C2D_TargetClear(g_top, slotmgr.get(0));
	C2D_TargetClear(g_bot, slotmgr.get(0));
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
	g_inRender = false;

	if(g_renderqueue.after_render_complete != nullptr)
	{
		std::function<bool()> *func = g_renderqueue.after_render_complete;
		g_renderqueue.after_render_complete = nullptr;
		ret &= (*func)();
		delete func;
	}

	return ret;
}

void ui::RenderQueue::render_and_then(std::function<bool()> cb)
{
	panic_assert(this->after_render_complete == nullptr, "illegal double after_render_complete");
	this->after_render_complete = new std::function<bool()>(cb);
}

void ui::RenderQueue::render_and_then(std::function<void()> cb)
{ this->render_and_then(std::function<bool()>([cb]() { cb(); return true; })); }

void ui::RenderQueue::detach_after()
{
	if(this->after_render_complete != nullptr)
	{
		delete this->after_render_complete;
		this->after_render_complete = nullptr;
	}
}

void ui::RenderQueue::signal(u8 bits)
{ this->signalBit |= bits; }

void ui::RenderQueue::unsignal(u8 bits)
{ this->signalBit &= ~bits; }

bool ui::RenderQueue::render_frame()
{
	ui::Keys keys = ui::RenderQueue::get_keys();
	return this->render_frame(keys);
}

bool ui::RenderQueue::render_screen(const ui::Keys& keys, ui::Screen screen)
{
	return screen == ui::Screen::top ? this->render_top(keys) : this->render_bottom(keys);
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
	panic_assert(this->sheet != nullptr, "failed to open spritesheet");
}

size_t ui::SpriteStore::size()
{
	return C2D_SpriteSheetCount(this->sheet);
}

C2D_Sprite ui::SpriteStore::get_by_id(ui::sprite id)
{
	C2D_Sprite ret;
	C2D_SpriteFromSheet(&ret, g_spritestore.sheet, (size_t) id);
	return ret;
}


/* core widget class Text */

UI_SLOTS(ui::Text_color, ui::color_bg, ui::color_text)

void ui::Text::setup(const std::string& label)
{ this->set_text(label); }

void ui::Text::setup()
{ this->set_text(""); }

void ui::Text::prepare_arrays()
{
	if(this->buf == nullptr) this->buf = C2D_TextBufNew(this->text.size() + 11);
	else
	{
		// Reset
		C2D_TextBufClear(this->buf);
		this->buf = C2D_TextBufResize(this->buf, this->text.size() + 11);
		this->lines.clear();
	}

	int width = ui::screen_width(this->screen);
	int lines = (this->text.size() / (width - this->x)) + 1;
	this->lines.reserve(lines);

	int curWidth = this->x;
	char codepoint[4] = { 0x00, 0x00, 0x00, 0x00 };
	u8 expect = 0;
	u8 cpc = 0;
	std::string cur;

	for(size_t i = 0; i < this->text.size(); ++i)
	{
		if((this->doAutowrap && width - curWidth < 0) || this->text[i] == '\n')
		{
			curWidth = this->x;
			if(!isspace(this->text[i]))
				cur.push_back('-');
			this->push_str(cur);
			cur.clear();

			/* makes it so that you don't get random indents if
			 * the next character is a space, or in the case of
			 * a newline doesn't create a new paragraph */
			if(isspace(this->text[i]))
				continue;
		}

		u8 code = this->text[i];
		if(cpc == 0)
		{
			codepoint[cpc++] = code;
			if(code < 0x80)
				; /* single-byte; immediately process */
			/* doesn't check for actual validity but this works well enough */
			else if(code < 0xE0) { expect = 2; continue; }
			else if(code < 0xF0) { expect = 3; continue; }
			else if(code < 0xF5) { expect = 4; continue; }
		}
		else if(cpc != expect)
		{
			codepoint[cpc++] = code;
			if(cpc != expect)
				continue;
		}

		cur.append(codepoint, cpc);

		if(this->doAutowrap)
		{
			charWidthInfo_s *ch = C2D_FontGetCharWidthInfo(NULL, C2D_FontGlyphIndexFromCodePoint(NULL, * (u32 *) codepoint));
			if(ch != NULL) curWidth += ch->glyphWidth;
			else dlog("Codepoint not found: %s (0x%08lX)", std::string(codepoint, cpc).c_str(), * (u32 *) codepoint);
		}

		memset(codepoint, 0x00, 4);
		expect = 0;
		cpc = 0;
	}

	if(expect != cpc)
		elog("Incomplete utf-8 data passed. Not appending final codepoint.");

	if(cur.size() > 0) this->push_str(cur);
	if(this->lines.size() > 0)
	{
		C2D_TextGetDimensions(&this->lines.front(), this->xsiz, this->ysiz, nullptr,
			&this->lineHeigth);
	}
	else this->lineHeigth = 0;
}

void ui::Text::push_str(const std::string& str)
{
	this->lines.emplace_back();
	C2D_TextParse(&this->lines.back(), this->buf, str.c_str());
	C2D_TextOptimize(&this->lines.back());
}

void ui::Text::destroy()
{
	C2D_TextBufDelete(this->buf);
}

static float get_center_x(C2D_Text *txt, float xsiz, float ysiz, ui::Screen screen)
{
	float width; C2D_TextGetDimensions(txt, xsiz, ysiz, &width, nullptr);
	return ((ui::screen_width(screen) / 2) - (width / 2.0f));
}

bool ui::Text::render(const ui::Keys& keys)
{
	((void) keys);

	if(this->doScroll)
	{
		if(this->sctx.offset > this->sctx.width)
			this->reset_scroll();
		else if(this->sctx.offset == 0)
		{
			if(this->sctx.timing > 20)
				++this->sctx.offset;
			++this->sctx.timing;
		}
		else if(this->sctx.timing % 3 == 0)
		{
			this->sctx.timing = 1;
			++this->sctx.offset;
			--this->sctx.rx;
		}
		else ++this->sctx.timing;

		C2D_DrawRectSolid(0, this->y, 0.1f, this->x, this->sctx.height, this->slots.get(0));
	}

	for(size_t i = 0; i < this->lines.size(); ++i)
	{
		if(this->drawCenter)
		{
			float center = get_center_x(&this->lines[i], this->xsiz, this->ysiz,
				this->screen);

			C2D_DrawText(&this->lines[i], C2D_WithColor, center,
				this->y + (this->lineHeigth * i), this->z, this->xsiz, this->ysiz, this->slots.get(1));
		}
		else if(this->doScroll)
		{

			C2D_DrawText(&this->lines[i], C2D_WithColor, this->sctx.rx,
				this->y + (this->lineHeigth * i), this->z, this->xsiz, this->ysiz, this->slots.get(1));
		}
		else
		{
			C2D_DrawText(&this->lines[i], C2D_WithColor, this->x,
				this->y + (this->lineHeigth * i), this->z, this->xsiz, this->ysiz, this->slots.get(1));
		}
	}

	return true;
}

void ui::Text::reset_scroll()
{
	this->sctx.height = this->height();
	this->sctx.width = this->width();
	this->sctx.rx = this->x;
	this->sctx.timing = 1;
	this->sctx.offset = 0;
}

void ui::Text::autowrap()
{
	this->doAutowrap = true;
	this->prepare_arrays();
}

void ui::Text::scroll()
{
	this->reset_scroll();
	// Only scroll if required
	if(this->sctx.width + this->x > ui::screen_width(this->screen))
	{
		this->doScroll = true;
		this->z = 0.0f;
	}
}

void ui::Text::set_center_x()
{
	this->drawCenter = true;
}

float ui::Text::height()
{
	return this->lines.size() * this->lineHeigth;
}

float ui::Text::width()
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

void ui::Text::resize(float x, float y)
{
	this->xsiz = x;
	this->ysiz = y;
}

const std::string& ui::Text::get_text()
{ return this->text; }

void ui::Text::set_text(const std::string& label)
{
	this->text = label;
	this->prepare_arrays();
}

/* core widget class Sprite */

static u32 sprite_stub() { return 0; }
static ui::slot_color_getter sprite_stub_a[] = { sprite_stub };
void ui::Sprite::setup(const C2D_Sprite& light, const C2D_Sprite& dark)
{
	const C2D_Sprite *first = &dark, *second = &light;
	if(get_settings()->isLightMode) { first = &light; second = &dark; }
	this->sprite = *first;
	this->second = *second;
	this->flags |= ui::Sprite::flag_darklight;
	ui::ThemeManager::global()->get_slots(this, "Sprite", 1, sprite_stub_a);
}

void ui::Sprite::setup(const C2D_Sprite& sprite)
{
	// Compatibility with other methods
	this->set_center(0.0f, 0.0f);
	this->sprite = sprite;
}

void ui::Sprite::destroy()
{
	if(this->flags & ui::Sprite::flag_darklight)
	{
		ui::ThemeManager::global()->unregister(this);
	}
}

bool ui::Sprite::supports_theme_hook() { return true; }
void ui::Sprite::update_theme_hook()
{ /* only called if this->flags & ui::Sprite::flag_darklight */
	C2D_Sprite first = this->sprite;
	this->sprite = this->second;
	this->second = first;
	this->sprite.params = first.params;
}

void ui::Sprite::set_sprite(const C2D_Sprite& sprite)
{
	C2D_DrawParams params = this->sprite.params;
	this->sprite = sprite;
	/* copy config */
	this->sprite.params = params;
}

float ui::Sprite::height()
{
	return this->sprite.image.subtex->height;
}

float ui::Sprite::width()
{
	return this->sprite.image.subtex->width;
}

bool ui::Sprite::render(const ui::Keys& keys)
{
	((void) keys); // unused
	C2D_DrawSprite(&this->sprite);
	return true;
}

void ui::Sprite::set_x(float x)
{
	this->x = ui::transform(this, x);
	C2D_SpriteSetPos(&this->sprite, x, this->y);
}

void ui::Sprite::set_y(float y)
{
	this->y = ui::transform(this, y);
	C2D_SpriteSetPos(&this->sprite, this->x, this->y);
}

void ui::Sprite::set_z(float z)
{
	this->z = z;
	C2D_SpriteSetDepth(&this->sprite, z);
}

void ui::Sprite::rotate(float degs)
{
	C2D_SpriteRotateDegrees(&this->sprite, degs);
}

void ui::Sprite::set_center(float x, float y)
{
	C2D_SpriteSetCenter(&this->sprite, x, y);
}

/* core widget class Button */

static u32 button_border_color() { return DICOLOR(UI_COLOR(00,00,00,FF), UI_COLOR(FF,FF,FF,FF)); }
UI_SLOTS(ui::Button_colors, button_border_color, ui::color_button)

void ui::Button::setup(const C2D_Sprite& light, const C2D_Sprite& dark)
{
	ui::Sprite *label = new ui::Sprite(this->screen);
	label->setup(light, dark);
	label->set_z(1.0f);
	label->finalize();

	this->widget = label;
	this->readjust();
}

void ui::Button::setup(const std::string& text)
{
	this->widget = nullptr;
	this->set_label(text);
}

void ui::Button::setup(const C2D_Sprite& sprite)
{
	ui::Sprite *label = new ui::Sprite(this->screen);
	label->setup(sprite);
	label->set_z(1.0f);
	label->finalize();

	this->widget = label;
	this->readjust();
}

void ui::Button::setup()
{ this->setup(""); }

void ui::Button::set_label(const std::string& v)
{
	if(this->widget != nullptr)
	{
		this->widget->destroy();
		delete this->widget;
	}

	ui::Text *label = new ui::Text(this->screen);
	label->setup(v);
	label->finalize();

	this->widget = label;
	this->readjust();
}

void ui::Button::destroy()
{
	this->widget->destroy();
	delete this->widget;
}

float ui::Button::height()
{ return this->h; }

float ui::Button::width()
{ return this->w; }

void ui::Button::resize(float x, float y)
{
	this->w = x;
	this->h = y;
	this->readjust();
}

void ui::Button::resize_children(float x, float y)
{
	// Super unsafe... works though
	((ui::Text *) this->widget)->resize(x, y);
	this->readjust();
}

void ui::Button::set_x(float x)
{
	this->x = ui::transform(this, x);
	this->readjust();
}

void ui::Button::set_y(float y)
{
	this->y = ui::transform(this, y);
	this->readjust();
}

void ui::Button::set_z(float z)
{
	this->z = z;
	this->widget->set_z(z);
}

void ui::Button::set_border(bool b)
{
	this->showBorder = b;
}

void ui::Button::readjust()
{
	/* center the subwidget in the container */
	this->widget->set_y(((this->h / 2) - (this->widget->height() / 2)) + this->y);
	this->widget->set_x(((this->w / 2) - (this->widget->width() / 2)) + this->x);

	this->ox = this->x + this->w;
	this->oy = this->y + this->h;
}

bool ui::Button::render(const ui::Keys& keys)
{
	if(this->showBorder) C2D_DrawRectSolid(this->x - 1, this->y - 1, 0.0f, this->w + 2, this->h + 2, this->slots.get(0));
	if(this->showBg) C2D_DrawRectSolid(this->x, this->y, 0.1f, this->w, this->h, this->slots.get(1));
	this->widget->render(keys);

	if(keys.touch.px >= this->x && keys.touch.px <= this->ox &&
			keys.touch.py >= this->y && keys.touch.py <= this->oy)
		return this->on_click();

	return true;
}

void ui::Button::autowrap()
{
	this->w = this->widget->width() + (this->showBg ? 0.0f : 10.0f);
	this->h = this->widget->height() + (this->showBg ? 0.0f : 2.0f);
	this->readjust();
}

void ui::Button::connect(connect_type type)
{
	switch(type)
	{
	case ui::Button::nobg:
		this->showBg = false;
		break;
	default:
		panic("EINVAL");
		break;
	}
}

void ui::Button::connect(connect_type type, std::function<bool()> callback)
{
	switch(type)
	{
	case ui::Button::click:
		this->on_click = callback;
		break;
	default:
		panic("EINVAL");
		break;
	}
}

float ui::Button::textwidth()
{
	return this->widget->width();
}

/* core widget class ButtonCallback */

void ui::ButtonCallback::setup(u32 keys)
{ this->keys = keys; }

bool ui::ButtonCallback::render(const ui::Keys& keys)
{
	switch(this->type)
	{
	case ui::ButtonCallback::kdown:
		if(keys.kDown & this->keys)
			return this->cb(keys.kDown);
		break;
	case ui::ButtonCallback::kheld:
		if(keys.kHeld & this->keys)
			return this->cb(keys.kHeld);
		break;
	case ui::ButtonCallback::kup:
		if(keys.kUp & this->keys)
			return this->cb(keys.kUp);
		break;
	case ui::ButtonCallback::none:
		break;
	}
	return true;
}

void ui::ButtonCallback::connect(ui::ButtonCallback::connect_type type, std::function<bool(u32)> cb)
{
	this->type = type;
	this->cb = cb;
}

