
#include "ui/core.hh"

#ifdef USE_SETTINGS_H
	#include "settings.hh"
#endif


#define DO_WIDS_DRAW(wids, keys, target, for_fg) { \
	switch(ui::draw_widgets(wids, keys, target, for_fg)) { \
	case ui::Results::quit_loop   : ret = false; goto exit; \
	case ui::Results::end_early   : return true; \
	case ui::Results::quit_no_end : return false; \
	case ui::Results::go_on       : break; }}


static C3D_RenderTarget *g_top;
static C3D_RenderTarget *g_bot;
static ui::Widgets g_widgets;


C3D_RenderTarget *ui::bot()
{ return g_bot; }

C3D_RenderTarget *ui::top()
{ return g_top; }

ui::Widgets *ui::wid()
{ return &g_widgets; }


void ui::end_frame()
{ C3D_FrameEnd(0); }

ui::Keys ui::get_keys()
{
	ui::Keys ret;
	hidScanInput();
	ret.kDown = hidKeysDown();
	ret.kHeld = hidKeysHeld();
	ret.kUp = hidKeysUp();

	hidTouchRead(&ret.touch);
	return ret;
}

bool ui::framenext(ui::Keys& keys)
{
	keys = ui::get_keys();
	if(keys.kDown & KEY_START)
		return false;
	return aptMainLoop();
}

ui::Results ui::draw_widgets(std::vector<ui::Widget *> wids, ui::Keys& keys, ui::Scr target, bool fg)
{
	for(ui::Widget *wid : wids)
	{
		if(!wid->enabled) continue;
		else if(wid->forceFg && !fg) continue;
		else if(!wid->forceFg && fg) continue;
		ui::Results ret = wid->draw(keys, target);
		if(ret != ui::Results::go_on)
			return ret;
	}
	return ui::Results::go_on;
}

bool ui::framedraw(ui::Widgets& wids, ui::Keys& keys)
{
	C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
	ui::clear(ui::Scr::bottom);
	ui::clear(ui::Scr::top);
	bool ret = true;


	C2D_SceneBegin(g_top);
	DO_WIDS_DRAW(g_widgets.top, keys, ui::Scr::top, false);
	DO_WIDS_DRAW(wids.top, keys, ui::Scr::top, false);
	DO_WIDS_DRAW(g_widgets.top, keys, ui::Scr::top, true); // Second iter

	C2D_SceneBegin(g_bot);
	DO_WIDS_DRAW(g_widgets.bot, keys, ui::Scr::bottom, false);
	DO_WIDS_DRAW(wids.bot, keys, ui::Scr::bottom, false);
	DO_WIDS_DRAW(g_widgets.bot, keys, ui::Scr::bottom, true); // Second iter


exit:
	C3D_FrameEnd(0);
	return ret;
}

void ui::clear(ui::Scr screen)
{
#ifdef USE_SETTINGS_H
	#define CTOP (get_settings()->isLightMode ? ui::constants::COLOR_TOP_LI : ui::constants::COLOR_TOP)
	#define CBOT (get_settings()->isLightMode ? ui::constants::COLOR_BOT_LI : ui::constants::COLOR_BOT)
#else
	#define CTOP ui::constants::COLOR_TOP
	#define CBOT ui::constants::COLOR_BOT
#endif

	switch(screen)
	{
	case ui::Scr::top:
		C2D_TargetClear(g_top, CTOP);
		break;

	case ui::Scr::bottom:
		C2D_TargetClear(g_bot, CBOT);
		break;

	case ui::Scr::_none:
		break;
	}

#undef CTOP
#undef CBOT
}

bool ui::global_init()
{
	gfxInitDefault();
	if(!C3D_Init(C3D_DEFAULT_CMDBUF_SIZE)) return false;
	if(!C2D_Init(C2D_DEFAULT_MAX_OBJECTS)) return false;
	C2D_Prepare();

	g_bot = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);
	g_top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);

	return true;
}

void ui::global_deinit()
{
	C2D_Fini();
	C3D_Fini();
	gfxExit();
}

void ui::Widgets::clear(ui::Scr target)
{
	switch(target)
	{
	case ui::Scr::bottom:
		for(ui::Widget *wid : this->bot)
			delete wid;
		this->bot.clear();
		break;

	case ui::Scr::top:
		for(ui::Widget *wid : this->top)
			delete wid;
		this->top.clear();
		break;

	case ui::Scr::_none:
		break;
	}
}

static void for_each_widget(std::vector<ui::Widget *> *wids, std::string type, std::function<void(ui::Widget *)> cb)
{
	for(ui::Widget *wid : *wids)
	{
		if(wid->iden == type)
			cb(wid);
	}
}

void ui::Widgets::for_each(std::string type, std::function<void(ui::Widget *)> cb)
{
	for_each_widget(&this->top, type, cb);
	for_each_widget(&this->bot, type, cb);
}

void ui::Widgets::clear()
{
	this->clear(ui::Scr::bottom);
	this->clear(ui::Scr::top);
}

ui::Widgets::~Widgets()
{
	this->clear();
}

void ui::Widgets::push_back(ui::Widget *widget, Scr target)
{
	if(widget->forceScr != ui::Scr::_none)
		target = widget->forceScr;

	switch(target)
	{
	case ui::Scr::bottom:
		widget->screen = ui::Scr::bottom;
		widget->pre_push();
		this->bot.push_back(widget);
		break;

	case ui::Scr::top:
		widget->screen = ui::Scr::top;
		widget->pre_push();
		this->top.push_back(widget);
		break;

	case ui::Scr::_none:
		delete widget;
		break;
	}
}

void ui::Widgets::push_back(std::string name, ui::Widget *widget, ui::Scr target)
{
	widget->name(name);
	this->push_back(widget, target);
}

void ui::Widgets::delete_by_name(std::string name, ui::Scr target)
{
	int index = this->find_index_by_name(name, target);
	if(index == -1) return;
	std::vector<ui::Widget *> *vec = &(target == ui::Scr::bottom ? this->bot : this->top);
	vec->erase(vec->begin() + index);
}

int ui::Widgets::find_index_by_name(std::string name, ui::Scr target)
{
	std::vector<ui::Widget *> *vec;
	switch(target)
	{
	case ui::Scr::bottom:
		vec = &this->bot;
		break;

	case ui::Scr::top:
		vec = &this->top;
		break;

	case ui::Scr::_none:
		return -1;
	}

	for(size_t i = 0; i < vec->size(); ++i)
	{
		if((*vec)[i]->formal == name)
			return i;
	}
	return -1;
}

ui::Widget *ui::Widgets::find_by_name(std::string name, ui::Scr target)
{
	int index = this->find_index_by_name(name, target);
	if(index == -1) return nullptr;
	return target == ui::Scr::bottom
		? this->bot[index] : this->top[index];
}


#ifdef USE_SETTINGS_H
# define TXT_CLR (get_settings()->isLightMode ? ui::constants::COLOR_TXT_LI : ui::constants::COLOR_TXT)
#else
# define TXT_CLR ui::constants::COLOR_TXT
#endif

void ui::draw_at(float x, float y, C2D_Text& txt, u32 flags, float sizeX, float sizeY, float z)
{
	// Sorry for the magic numbers :kek:
	C2D_DrawText(&txt, C2D_WithColor | flags, GRID_AL_X(x), GRID_AL_Y(y), z, sizeX, sizeY, TXT_CLR);
}

void ui::draw_at(float x, float y, c2d::Text& txt, u32 flags, float sizeX, float sizeY, float z)
{
	txt.draw(GRID_AL_X(x), GRID_AL_Y(y), z, TXT_CLR, sizeX, sizeY, flags | C2D_WithColor);
}

void ui::draw_at_absolute(float x, float y, C2D_Text& txt, u32 flags, float sizeX, float sizeY, float z)
{
	C2D_DrawText(&txt, C2D_WithColor | flags, x, y, z, sizeX, sizeY, TXT_CLR);
}

void ui::draw_at_absolute(float x, float y, c2d::Text& txt, u32 flags, float sizeX, float sizeY, float z)
{
	txt.draw(x, y, z, TXT_CLR, sizeX, sizeY, flags | C2D_WithColor);
}

#undef TXT_CLR


void ui::switch_to(ui::Scr target)
{
	switch(target)
	{
	case ui::Scr::bottom:
		C2D_SceneTarget(g_bot);
		break;

	case ui::Scr::top:
		C2D_SceneTarget(g_top);
		break;

	case ui::Scr::_none:
		break;
	}
}

void ui::parse_text(C2D_Text *ret, C2D_TextBuf buf, std::string txt)
{
	C2D_TextParse(ret, buf, txt.c_str());
}

void ui::parse_text(c2d::Text *ret, c2d::TextBuf buf, std::string txt)
{
	ret->parse(buf, txt);
}
