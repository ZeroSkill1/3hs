
#ifndef __ui_core_hh__
#define __ui_core_hh__

#include <citro3d.h>
#include <citro2d.h>
#include <3ds.h>

#include <functional>
#include <sstream>
#include <cstdarg>
#include <memory>
#include <vector>
#include <string>

#include "bindings.hh"

#define bad_text(t,x,y) { C2D_TextBuf __b; __b = C2D_TextBufNew(100); \
	C2D_Text __t; ui::parse_text(&__t, __b, t); C2D_TextOptimize(&__t); \
	ui::draw_at(x, y, __t); C2D_TextBufDelete(__b); }
#define quick_global_draw() { ui::Widgets wids; ui::Keys keys; \
	ui::framenext(keys); ui::framedraw(wids, keys); }
#define single_draw(wids) { ui::Keys keys; \
		ui::framenext(keys);  ui::framedraw(wids, keys); }
#define generic_main_loop(w) { ui::Keys k; \
	while(ui::framenext(k)) ui::framedraw(w,k); }
#define generic_main_breaking_loop(w) { ui::Keys k; \
	while(ui::framenext(k)) if(!ui::framedraw(w,k)) break; }
#define standalone_main_loop() { ui::Widgets w; \
	generic_main_loop(w); }
#define standalone_main_breaking_loop() { ui::Widgets w; \
	generic_main_breaking_loop(w); }

#define GRID_AL_X(x) ((x)*12)
#define GRID_AL_Y(y) ((y)*18)

#define GFX(n) "romfs:/gfx/" n
#define SHEET(n) GFX(n ".t3x")

#define SCREEN_WIDTH(s) (s == ui::Scr::top ? 400.0f : 320.0f)
#define SCREEN_HEIGHT() (240.0f)

namespace ui
{
	namespace constants
	{
		constexpr u32 COLOR_BOT = C2D_Color32(0x1C, 0x20, 0x21, 0xFF);
		constexpr u32 COLOR_TOP = C2D_Color32(0x1C, 0x20, 0x21, 0xFF);

		constexpr float FSIZE = 0.65f;
	}

	enum class Scr
	{ bottom, top };

	enum class Results
	{
		quit_loop,   // Quit the loop; just end it now
		go_on,       // Normal exit
		end_early,   // Stop render loop; don't end frame
		quit_no_end, // Quit the loop and don't end the frame
	};

	typedef struct Keys
	{
		touchPosition touch;
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
		Scr screen;

		virtual Results draw(Keys& keys, Scr target) = 0;
		virtual void pre_push() { }

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

		/* -1 = not found */
		int find_index_by_name(std::string name, Scr target);
		Widget *find_by_name(std::string name, Scr target);
		void delete_by_name(std::string name, Scr target);

		void for_each(std::string type, std::function<void(Widget *)> cb);

		template <typename T>
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

	void parse_text(c2d::Text *ret, c2d::TextBuf buf, std::string txt);
	void parse_text(C2D_Text *ret, C2D_TextBuf buf, std::string txt);

	void draw_at_absolute(float x, float y, c2d::Text& txt, u32 flags = 0, float sizeX = constants::FSIZE, float sizeY = constants::FSIZE);
	void draw_at(float x, float y, c2d::Text& txt, u32 flags = 0, float sizeX = constants::FSIZE, float sizeY = constants::FSIZE);

	void draw_at_absolute(float x, float y, C2D_Text& txt, u32 flags = 0, float sizeX = constants::FSIZE, float sizeY = constants::FSIZE);
	void draw_at(float x, float y, C2D_Text& txt, u32 flags = 0, float sizeX = constants::FSIZE, float sizeY = constants::FSIZE);

	ui::Results draw_widgets(std::vector<ui::Widget *> wids, ui::Keys& keys, ui::Scr target = ui::Scr::top);
	void switch_to(Scr target);
	void clear(Scr screen);
	void end_frame();


	C3D_RenderTarget *top();
	C3D_RenderTarget *bot();
	Widgets *wid();

	template <typename T>
	std::string floating_prec(T inte, int prec = 2)
	{
		std::ostringstream ss; ss.precision(prec);
		ss << std::fixed << inte; return ss.str();
	}

	template <typename T>
	std::string human_readable_size(T inte)
	{
		// Sorry for this mess.....
		if(inte < 1024) return std::to_string(inte) + " B"; /* < 1 KiB */
		if(inte < 1024 * 1024) /* < 1 MiB */
			return floating_prec<float>((float) inte / 1024) + " KiB";
		if(inte < 1024 * 1024 * 1024) /* < 1 GiB */
			return floating_prec<float>((float) inte / 1024 / 1024) + " MiB";
		if(inte < (long long) 1024 * 1024 * 1024 * 1024) /* < 1TiB */
			return floating_prec<float>((float) inte / 1024 / 1024 / 1024) + " GiB";
		return floating_prec<float>((float) inte / 1024 / 1024 / 1024 / 1024) + " TiB";
	}

	template <typename T>
	std::string human_readable_size_block(T inte)
	{
		std::string ret = human_readable_size<T>(inte);
		T blk = (double) inte / 1024 / 128;
		blk = blk == 0 ? 1 : blk;

		return ret + std::string(" (") + std::to_string(blk) + " blocks)";
	}
}

#endif

