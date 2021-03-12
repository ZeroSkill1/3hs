
#ifndef __ui_core_hh__
#define __ui_core_hh__

#include <citro3d.h>
#include <citro2d.h>
#include <3ds.h>

#include <iostream>
#include <cstdarg>
#include <memory>
#include <vector>
#include <string>

#define bad_text(t,x,y) { C2D_TextBuf __b; __b = C2D_TextBufNew(100); \
	C2D_Text __t; ui::parse_text(&__t, __b, t); C2D_TextOptimize(&__t); \
	ui::draw_at(x, y, __t); C2D_TextBufDelete(__b); }
#define quick_global_draw() { ui::Widgets wids; ui::Keys keys; \
	ui::framenext(keys); ui::framedraw(wids, keys); }
#define generic_main_loop(w) { ui::Keys k; \
	while(ui::framenext(k)) ui::framedraw(w,k); }
#define standalone_main_loop() { ui::Widgets w; \
	generic_main_loop(w); }

#define GFX(n) ("romfs:/gfx/" n)
#define SCREEN_WIDTH 400


namespace ui
{
	namespace constants
	{
		constexpr u32 COLOR_BOT = C2D_Color32(0x1C, 0x20, 0x21, 0xFF);
		constexpr u32 COLOR_TOP = C2D_Color32(0x1C, 0x20, 0x21, 0xFF);

		constexpr char FONT[] = GFX("JetBrainsMono.bcfnt");
	}

	enum class Scr
	{ bottom, top };

	typedef struct Keys
	{
		u32 kDown;
		u32 kHeld;
		u32 kUp;
	} Keys;


	class Widget
	{
	public:
		virtual ~Widget() { }
		Widget(std::string id)
		{ this->iden = id; }

		std::string formal;
		std::string iden;

		virtual bool draw(Keys& keys, Scr target) = 0;

		virtual void init() { };

		void name(std::string name)
		{ this->formal = name; }

		bool enabled = true;
		void toggle()
		{
			this->enabled = this->enabled ? false : true;
		}
	};


	class Widgets
	{
	public:
		void push_back(std::string name, Widget *val, Scr target = Scr::top);
		void push_back(Widget *val, Scr target = Scr::top);

		void clear(Scr target);
		void clear();

		~Widgets();

		std::vector<Widget *> top;
		std::vector<Widget *> bot;

		Widget *find_by_name(std::string name, Scr target);

		template <typename T = Widget>
		T *get(std::string name)
		{
			T *ret;
			if((ret = (T *) this->find_by_name(name, Scr::top)) != nullptr)
				return ret;
			return (T *) this->find_by_name(name, Scr::bottom);
		}
	};


	void global_deinit();
	bool global_init();

	bool framedraw(Widgets& wids, Keys& keys);
	bool framenext(Keys& keys);

	void parse_text(C2D_Text *ret, C2D_TextBuf buf, std::string txt);
	void draw_at(float x, float y, C2D_Text& txt, u32 flags = 0, float sizeX = 0.50f, float sizeY = 0.50f);
	void draw_at_absolute(float x, float y, C2D_Text& txt, u32 flags = 0, float sizeX = 0.50f, float sizeY = 0.50f);
	void switch_to(Scr target);
	void clear(Scr screen);


	C3D_RenderTarget *top();
	C3D_RenderTarget *bot();
	charWidthInfo_s *char_size();
	Widgets *wid();
}

#endif
