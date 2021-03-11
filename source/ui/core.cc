
#include "ui/core.hh"

static C3D_RenderTarget *g_top;
static C3D_RenderTarget *g_bot;

C3D_RenderTarget *ui::bot()
{ return g_bot; }

C3D_RenderTarget *ui::top()
{ return g_top; }


bool ui::framenext(ui::Keys& keys)
{
	hidScanInput();
	keys.kDown = hidKeysDown();
	keys.kHeld = hidKeysHeld();
	keys.kUp = hidKeysUp();

	if(keys.kDown & KEY_START)
		return false;
	return aptMainLoop();
}

void ui::framedraw(ui::Widgets& wids, ui::Keys& keys)
{
	C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
	ui::clear(ui::Scr::top);
	C2D_SceneBegin(g_top);

	for(ui::Widget *wid : wids.widgets)
	{
		wid->draw(keys);
	}

	C3D_FrameEnd(0);
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
	return true;
}

void ui::global_deinit()
{
	C2D_Fini();
	C3D_Fini();
	gfxExit();
}

void ui::Widgets::clear()
{
	for(ui::Widget *wid : this->widgets)
		delete wid;
	this->widgets.clear();
}

ui::Widgets::~Widgets()
{
	this->clear();
}

void ui::Widgets::push_back(ui::Widget& widget)
{
	this->widgets.push_back(&widget);
}

void ui::draw_at(int x, int y, C2D_Text& txt, u32 flags)
{
	// 9 = about the distance between 2 texts
	C2D_DrawText(&txt, flags, x * 9, y * 9, 0.0f, 0.5f, 0.5f);
}
