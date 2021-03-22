
#include "ui/core.hh"

#define DO_WIDS_DRAW(wids) { \
	ui::Results res = ui::draw_widgets(wids, keys); switch(res) { \
	case ui::Results::quit_loop   : ret = false; goto exit; \
	case ui::Results::end_early   : return true; \
	case ui::Results::quit_no_end : return false; \
	case ui::Results::go_on       : break; }}


static C3D_RenderTarget *g_top;
static C3D_RenderTarget *g_bot;
static ui::Widgets g_widgets;
static C2D_Font g_font;


C3D_RenderTarget *ui::bot()
{ return g_bot; }

C3D_RenderTarget *ui::top()
{ return g_top; }

ui::Widgets *ui::wid()
{ return &g_widgets; }


bool ui::framenext(ui::Keys& keys)
{
	hidScanInput();
	keys.kDown = hidKeysDown();
	keys.kHeld = hidKeysHeld();
	keys.kUp = hidKeysUp();

	hidTouchRead(&keys.touch);

	if(keys.kDown & KEY_START)
		return false;
	return aptMainLoop();
}

ui::Results ui::draw_widgets(std::vector<ui::Widget *> wids, ui::Keys& keys)
{
	for(ui::Widget *wid : wids)
	{
		if(!wid->enabled) continue;
		ui::Results ret = wid->draw(keys, ui::Scr::top);
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
	DO_WIDS_DRAW(g_widgets.top);
	DO_WIDS_DRAW(wids.top);

	C2D_SceneBegin(g_bot);
	DO_WIDS_DRAW(g_widgets.bot);
	DO_WIDS_DRAW(wids.bot);


exit:
	C3D_FrameEnd(0);
	return ret;
}

void ui::clear(ui::Scr screen)
{
	switch(screen)
	{
	case ui::Scr::top:
		C2D_TargetClear(g_top, ui::constants::COLOR_TOP);
		break;

	case ui::Scr::bottom:
		C2D_TargetClear(g_bot, ui::constants::COLOR_BOT);
		break;
	}
}

bool ui::global_init()
{
	gfxInitDefault();
	if(!C3D_Init(C3D_DEFAULT_CMDBUF_SIZE)) return false;
	if(!C2D_Init(C2D_DEFAULT_MAX_OBJECTS)) return false;
	C2D_Prepare();

	g_bot = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);
	g_top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
//	if((g_font = C2D_FontLoad(ui::constants::FONT)) == NULL)
//		return false;

	return true;
}

void ui::global_deinit()
{
//	if(g_font != NULL)
//		C2D_FontFree(g_font);
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
	}
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
	switch(target)
	{
	case ui::Scr::bottom:
		widget->screen = ui::Scr::bottom;
		this->bot.push_back(widget);
		break;

	case ui::Scr::top:
		widget->screen = ui::Scr::top;
		this->top.push_back(widget);
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

void ui::draw_at(float x, float y, C2D_Text& txt, u32 flags, float sizeX, float sizeY)
{
	// Sorry for the magic numbers :kek:
	C2D_DrawText(&txt, C2D_WithColor | flags, x * 12, y * 18, 0.0f, sizeX, sizeY, 0xFFFFFFFF);
}

void ui::draw_at_absolute(float x, float y, C2D_Text& txt, u32 flags, float sizeX, float sizeY)
{
	C2D_DrawText(&txt, C2D_WithColor | flags, x, y, 0.0f, sizeX, sizeY, 0xFFFFFFFF);
}

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
	}
}

void ui::parse_text(C2D_Text *ret, C2D_TextBuf buf, std::string txt)
{
	C2D_TextFontParse(ret, g_font, buf, txt.c_str());
}
