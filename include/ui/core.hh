
#ifndef inc_ui_core_hh
#define inc_ui_core_hh

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


#define quick_global_draw()          \
  do {                               \
    ui::Widgets wids; ui::Keys keys; \
    ui::framenext(keys);             \
    ui::framedraw(wids, keys);       \
  } while(0)

#define single_draw(wids)      \
  do {                         \
    ui::Keys keys;             \
    ui::framenext(keys);       \
    ui::framedraw(wids, keys); \
  } while(0)

#define generic_main_loop(w) \
  do {                       \
    ui::Keys k;              \
    while(ui::framenext(k))  \
      ui::framedraw(w,k);    \
  } while(0)

#define generic_main_breaking_loop(w) \
  do {                                \
    ui::Keys k;                       \
    while(ui::framenext(k))           \
      if(!ui::framedraw(w,k)) break;  \
  } while(0)

#define generic_main_breaking_startreplacing_loop(w) \
  do {                                               \
    ui::Keys k;                                      \
    while(ui::framenext_nobreak(k))                  \
      if(!ui::framedraw(w,k)) break;                 \
  } while(0)

#define standalone_main_loop() \
  do {                         \
    ui::Widgets w;             \
    generic_main_loop(w);      \
  } while(0)

#define standalone_main_breaking_loop() \
  do {                                  \
    ui::Widgets w;                      \
    generic_main_breaking_loop(w);      \
  } while(0)

// Button glyphs
#define GLYPH_A "\uE000"
#define GLYPH_B "\uE001"
#define GLYPH_X "\uE002"
#define GLYPH_Y "\uE003"
#define GLYPH_L "\uE004"
#define GLYPH_R "\uE005"
#define GLYPH_DPAD "\uE006"

#define GRID_AL_X(x) ((x)*12)
#define GRID_AL_Y(y) ((y)*18)

#define GFX(n) "romfs:/gfx/" n
#define SHEET(n) GFX(n ".t3x")

#define SCREEN_WIDTH(s) (s == ui::Scr::top ? 400.0f : 320.0f)
#define SCREEN_HEIGHT() (240.0f)

// don't use something else without a good reason
#define Z_OFF (1.0f)
#define Z_OFF_TEXT (0.0f)
#define Z_OFF_OVERLAY (0.1f)


namespace ui
{
	namespace constants
	{
		constexpr u32 COLOR_HI = C2D_Color32(0xFF, 0xFF, 0xFF, 0xFF);
		constexpr u32 COLOR_BOT = C2D_Color32(0x1C, 0x20, 0x21, 0xFF);
		constexpr u32 COLOR_TOP = C2D_Color32(0x1C, 0x20, 0x21, 0xFF);
		constexpr u32 COLOR_BTN = C2D_Color32(0x32, 0x35, 0x36, 0xFF);
		constexpr u32 COLOR_TXT = C2D_Color32(0xFF, 0xFF, 0xFF, 0xFF);
		constexpr u32 COLOR_BAR_BG = C2D_Color32(0xFF, 0xFF, 0xFF, 0xFF);
		constexpr u32 COLOR_BAR_FG = C2D_Color32(0x00, 0xD2, 0x03, 0xFF);

#ifdef USE_SETTINGS_H
		constexpr u32 COLOR_HI_LI = C2D_Color32(0x00, 0x00, 0x00, 0xFF);
		constexpr u32 COLOR_BOT_LI = C2D_Color32(0xEE, 0xEE, 0xEE, 0xFF);
		constexpr u32 COLOR_TOP_LI = C2D_Color32(0xEE, 0xEE, 0xEE, 0xFF);
		constexpr u32 COLOR_BTN_LI = C2D_Color32(0xDE, 0xDE, 0xDE, 0xFF);
		constexpr u32 COLOR_TXT_LI = C2D_Color32(0x00, 0x00, 0x00, 0xFF);
		constexpr u32 COLOR_BAR_BG_LI = C2D_Color32(0xDE, 0xDE, 0xDE, 0xFF);
		constexpr u32 COLOR_BAR_FG_LI = C2D_Color32(0xD0, 0xD0, 0xD0, 0xFF);
#endif

		constexpr float FSIZE = 0.65f;
	}

	enum class Scr
	{
		bottom,      // bottom screen
		top,         // top screen
		_none,       // internal
	};

	enum class Results
	{
		quit_loop,   // Quit the loop; just end it now
		go_on,       // Normal exit
		end_early,   // Stop render loop; don't end frame
		quit_no_end, // Quit the loop and don't end the frame
	};

#ifndef inc_ui_base_hh
	typedef struct Keys
	{
		u32 kDown, kHeld, kUp;
		touchPosition touch;
	} Keys;
#endif

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

		Scr forceScr = Scr::_none;
		bool forceFg = false;
		bool enabled = true;

		void force_screen(Scr scr)
		{ this->forceScr = scr; }

		void force_foreground()
		{ this->forceFg = true; }

		void toggle()
		{ this->enabled = this->enabled ? false : true; }

		void visibility(bool visable)
		{ this->enabled = visable; }
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
	bool framenext_nobreak(Keys& keys);
	bool framenext(Keys& keys);

	void parse_text(c2d::Text *ret, c2d::TextBuf buf, std::string txt);
	void parse_text(C2D_Text *ret, C2D_TextBuf buf, std::string txt);

	void draw_at_absolute(float x, float y, c2d::Text& txt, u32 flags = 0, float sizeX = constants::FSIZE, float sizeY = constants::FSIZE, float z = Z_OFF);
	void draw_at(float x, float y, c2d::Text& txt, u32 flags = 0, float sizeX = constants::FSIZE, float sizeY = constants::FSIZE, float z = Z_OFF);

	void draw_at_absolute(float x, float y, C2D_Text& txt, u32 flags = 0, float sizeX = constants::FSIZE, float sizeY = constants::FSIZE, float z = Z_OFF);
	void draw_at(float x, float y, C2D_Text& txt, u32 flags = 0, float sizeX = constants::FSIZE, float sizeY = constants::FSIZE, float z = Z_OFF);

	ui::Results draw_widgets(std::vector<ui::Widget *> wids, ui::Keys& keys, ui::Scr target = ui::Scr::top, bool fg = false);
	void switch_to(Scr target);
	void clear(Scr screen);
	ui::Keys get_keys();
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

	template <typename TInt>
	std::string human_readable_size(TInt i)
	{
		// Sorry for this mess.....
		if(i < 1024) return std::to_string(i) + " B"; /* < 1 KiB */
		if(i < 1024 * 1024) /* < 1 MiB */
			return floating_prec<float>((float) i / 1024) + " KiB";
		if(i < 1024 * 1024 * 1024) /* < 1 GiB */
			return floating_prec<float>((float) i / 1024 / 1024) + " MiB";
		if(i < (long long) 1024 * 1024 * 1024 * 1024) /* < 1TiB */
			return floating_prec<float>((float) i / 1024 / 1024 / 1024) + " GiB";
		return floating_prec<float>((float) i / 1024 / 1024 / 1024 / 1024) + " TiB";
	}

	template <typename TInt>
	std::string human_readable_size_block(TInt i)
	{
		std::string ret = human_readable_size<TInt>(i);
		TInt blk = (double) i / 1024 / 128;
		blk = blk == 0 ? 1 : blk;

		return ret + std::string(" (") + std::to_string(blk) + (blk == 1 ? " block)" : " blocks)");
	}
}

#endif

