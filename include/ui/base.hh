
#ifndef inc_ui_base_hh
#define inc_ui_base_hh

// Defines constants, we copy them over in
// our enum later
#include "build/next.h"

#include <citro3d.h>
#include <citro2d.h>
#include <3ds.h>

#include <functional>
#include <vector>
#include <string>
#include <list>

#define UI_WIDGET(name) \
	public: \
		using ui::BaseWidget::BaseWidget; \
		const char *className = name; \
	private:

#define UI_REQUIRES_METHOD_VARIADIC(T, VarTs, method, Signature) \
	template<typename ... VarTs, typename = std::enable_if<detail::has_##method<T, Signature>::value>>
#define UI_REQUIRES_METHOD(T, method, Signature) \
	template<typename = std::enable_if<detail::has_##method<T, Signature>::value>>

// Button glyphs
#define UI_GLYPH_A "\uE000"
#define UI_GLYPH_B "\uE001"
#define UI_GLYPH_X "\uE002"
#define UI_GLYPH_Y "\uE003"
#define UI_GLYPH_L "\uE004"
#define UI_GLYPH_R "\uE005"
#define UI_GLYPH_DPAD "\uE006"


namespace ui
{
	class BaseWidget; /* forward declaration */

	namespace detail
	{
		// From: https://stackoverflow.com/questions/87372/check-if-a-class-has-a-member-function-of-a-given-signature
#define make_has_struct(method) \
			template<typename, typename T> \
			struct has_##method { \
				static_assert( \
					std::integral_constant<T, false>::value, \
					"Second template parameter needs to be of function type."); \
			}; \
			template<typename C, typename Ret, typename... Args> \
			struct has_##method<C, Ret(Args...)> { \
			private: \
					template<typename T> \
					static constexpr auto check(T*) \
					-> typename \
							std::is_same< \
									decltype( std::declval<T>().method( std::declval<Args>()... ) ), \
									Ret \
							>::type; \
					template<typename> \
					static constexpr std::false_type check(...); \
					typedef decltype(check<C>(0)) type; \
			public: \
					static constexpr bool value = type::value; \
			}

		make_has_struct(resize_children);
		make_has_struct(set_border);
		make_has_struct(autowrap);
		make_has_struct(finalize);
		make_has_struct(connect);
		make_has_struct(resize);
		make_has_struct(scroll);
#undef make_has_struct

		// This works somehow, unused (for now?)
		template <typename C>
		struct has_connect_enum {
		private:
			template <typename T>
			static constexpr auto check(T*) -> typename
				std::is_enum<typename T::connect_type>::type;
			template <typename T>
			static constexpr std::false_type check(...);
			typedef decltype(check<C>(0)) type;
		public:
			static constexpr bool value = type::value;
		};
	}

	enum class Screen
	{
		top, bottom
	};

	namespace dimensions
	{
		constexpr float height       = 240.0f; /* height of both screens */
		constexpr float width_bottom = 320.0f; /* width of the bottom screen */
		constexpr float width_top    = 400.0f; /* width of the top screen */
	}

	namespace layout
	{
		constexpr float center_x = -1.0f; /* center x position */
		constexpr float center_y = -2.0f; /* center y position */
		constexpr float left     =  3.0f; /* left of the screen */
		constexpr float right    = -4.0f; /* right of the screen */
		constexpr float top      =  3.0f; /* top of the screen */
		constexpr float bottom   = -6.0f; /* bottom of the screen */
		constexpr float base     = 50.0f; /* base heigth */
	}

	namespace layer
	{
		constexpr float middle = 0.5f;
		constexpr float top    = 1.0f;
		constexpr float bottom = 0.0f;
	}

	namespace tag
	{
		constexpr int action   = -1; /* action header */
		constexpr int more     = -2; /* more button */
		constexpr int settings = -3; /* settings button */
		constexpr int search   = -4; /* search button */
		constexpr int queue    = -5; /* queue button */
	};

	/* holds sprite ids used for ui::SpriteStore::get_by_id() */
	enum class sprite
	{
		battery_light = next_battery_light_idx,
#undef next_battery_light_idx
		battery_dark = next_battery_dark_idx,
#undef next_battery_dark
		bun = next_bun_idx,
#undef next_bun_idx
		logo = next_logo_idx,
#undef next_logo_idx
		more_dark = next_more_dark_idx,
#undef next_more_dark_idx
		more_light = next_more_light_idx,
#undef next_more_light_idx
		search_dark = next_search_dark_idx,
#undef next_search_dark_idx
		search_light = next_search_light_idx,
#undef next_search_light_idx
		settings_dark = next_settings_dark_idx,
#undef next_settings_dark_idx
		settings_light = next_settings_light_idx,
#undef next_settings_light_idx
		spinner = next_spinner_idx,
#undef next_spinner_idx
	};

#ifndef inc_ui_core_hh
	struct Keys
	{
		u32 kDown, kHeld, kUp;
		touchPosition touch;
	};
#endif

	/* gets the width of a screen */
	constexpr inline float screen_width(ui::Screen scr)
	{ return scr == ui::Screen::top ? ui::dimensions::width_top : ui::dimensions::width_bottom; }
	/* gets the height of the screen (exists for consistency) */
	constexpr inline float screen_height()
	{ return ui::dimensions::height; }

	/* used internally by set_x, set_y,
	 * returns the same coord unless it fits into
	 * ui::layer or ui::layout */
	float transform(BaseWidget *wid, float v);

	/* get the x position right from `from' for `newel' */
	float right(BaseWidget *from, BaseWidget *newel, float pad = 3.0f);
	/* get the x position left from `from' for `newel' */
	float left(BaseWidget *from, BaseWidget *newel, float pad = 3.0f);
	/* get the y position under `from' for `newel' */
	float under(BaseWidget *from, BaseWidget *newel, float pad = 3.0f);
	/* get the y position above `from' for `newel' */
	float above(BaseWidget *from, BaseWidget *newel, float pad = 3.0f);

	/* global initializiation */
	bool init();
	/* init with render targets */
	void init(C3D_RenderTarget *top, C3D_RenderTarget *bot);
	/* global deinitialization */
	void exit();

	/* base widget class */
	class BaseWidget
	{
	public:
		BaseWidget(ui::Screen scr)
			: screen(scr) { }

		/* not supposed to be overriden */
		virtual ~BaseWidget() = default;

		virtual void set_x(float x)
		{
			if(x == ui::layout::center_x) this->set_center_x();
			else this->x = ui::transform(this, x);
		}

		virtual void set_y(float y) { this->y = ui::transform(this, y); }
		virtual void set_z(float z) { this->z = z; }

		ui::Screen renders_on() { return this->screen; }

		virtual void set_center_x() { this->x = ui::transform(this, ui::layout::center_x); }

		virtual bool render(const ui::Keys& keys) = 0;
		virtual void destroy() { }

		virtual float height() = 0;
		virtual float width() = 0;

		virtual float get_x() { return this->x; }
		virtual float get_y() { return this->y; }
		virtual float get_z() { return this->z; }

		virtual void finalize() { }

		enum connect_type { };

		void set_hidden(bool b) { this->hidden = b; }
		bool is_hidden() { return this->hidden; }

		bool matches_tag(int t) { return this->tag == t; }
		void set_tag(int t) { this->tag = t; }


	protected:
		ui::Screen screen;
		float z = ui::layer::middle;
		bool hidden = false;
		float x = 0, y = 0;
		int tag = 0;


	};

	/* Renders a list of derivatives of ui::BaseWidget */
	class RenderQueue
	{
	public:
		~RenderQueue();

		/* push a new widget into the queue */
		void push(ui::BaseWidget *wid);
		/* render forever */
		void render_forever();
		/* render until render_frame() returns false */
		void render_finite();
		/* Renders until keys.kDown & kDownMask > 0 */
		void render_until_button(u32 kDownMask);
		/* Renders until keys.kDown & kDownMask > 0 or if render_frame() returns false */
		void render_finite_button(u32 kDownMask);
		/* returns false if this should be the last frame,
		 * else returns true */
		bool render_frame(const ui::Keys&);
		/* returns false if this should be the last frame,
		 * else returns true */
		bool render_frame();
		/* renders only the bottom widgets */
		bool render_bottom(const ui::Keys&);
		/* renders only the top widgets */
		bool render_top(const ui::Keys&);
		/* renders a frame on `screen' */
		bool render_screen(const ui::Keys&, ui::Screen screen);
		/* removes all widgets in the queue */
		void clear();
		/* runs the callback after the frame render is done. Runs only once
		 * NOTE: you can only have 1 callback every frame
		 * NOTE 2: only works on the global renderqueue */
		void render_and_then(std::function<bool()> cb);
		void render_and_then(std::function<void()> cb);
		/* Detaches a callback set by
		 * render_and_then */
		void detach_after();
		/* Signals the RenderQueue */
		void signal(u8 bits);
		/* Unsets a signal from the RenderQueue */
		void unsignal(u8 bits);
		/* find a widget by tag
		 * First searches top widgets, then bottom
		 * Returns nullptr if no matches were found */
		template <typename TWidget = ui::BaseWidget>
		TWidget *find_tag(int t)
		{
			for(ui::BaseWidget *w : this->top)
				if(w->matches_tag(t)) return (TWidget *) w;
			for(ui::BaseWidget *w : this->bot)
				if(w->matches_tag(t)) return (TWidget *) w;
			return nullptr;
		}
		/* gets the last pushed element
		 * returns nullptr if the queue is empty */
		template <typename TWidget = ui::BaseWidget>
		TWidget *back()
		{
			return (TWidget *) this->backPtr;
		}

		/* Gets the global RenderQueue */
		static ui::RenderQueue *global();
		/* Gets the pressed keys */
		static ui::Keys get_keys();

		enum signal { signal_cancel = 1 };


	private:
		std::function<bool()> *after_render_complete = nullptr;
		ui::BaseWidget *backPtr = nullptr;
		std::list<ui::BaseWidget *> top;
		std::list<ui::BaseWidget *> bot;
		u8 signalBit = 0;


	};

	/* builder for a ui::BaseWidget derivative */
	template <typename TWidget>
	class builder
	{
	public:
		template<typename ... Ts>
		builder(ui::Screen scr, Ts&& ... args)
		{
			this->el = new TWidget(scr);
			this->el->setup(args...);
		}

		~builder()
		{
			if(this->el != nullptr)
				delete this->el;
		}

		/* Sets the size of a widget. Not supported by all widgets */
		UI_REQUIRES_METHOD(TWidget, resize, void(float, float))
			ui::builder<TWidget>& size(float x, float y) { this->el->resize(x, y); return *this; }
		/* Sets the size of a widget. Not supported by all widgets */
		UI_REQUIRES_METHOD(TWidget, resize, void(float, float))
			ui::builder<TWidget>& size(float xy) { this->el->resize(xy, xy); return *this; }
		/* Sets the size of any potential widget children. Not supported by all widgets */
		UI_REQUIRES_METHOD(TWidget, resize_children, void())
			ui::builder<TWidget>& size_children(float x, float y) { this->el->resize_children(x, y); return *this; }
		/* Sets the size of any potential widget children. Not supported by all widgets */
		UI_REQUIRES_METHOD(TWidget, resize_children, void())
			ui::builder<TWidget>& size_children(float xy) { this->el->resize_children(xy, xy); return *this; }
		/* Autowraps the widget. Not supported by all widgets */
		UI_REQUIRES_METHOD(TWidget, autowrap, void())
			ui::builder<TWidget>& wrap() { this->el->autowrap(); return *this; }
		/* Sets a border around the widget. Not supported by all widgets */
		UI_REQUIRES_METHOD(TWidget, set_border, void(bool))
			ui::builder<TWidget>& border() { this->el->set_border(true); return *this; }
		/* Makes the widget scroll. Not supported by all widgets */
		UI_REQUIRES_METHOD(TWidget, scroll, void())
			ui::builder<TWidget>& scroll() { this->el->scroll(); return *this; }
		/* Connects a type and argument, effect depends on the widget.
		 * Not supported by all widgets */
		UI_REQUIRES_METHOD_VARIADIC(TWidget, Ts, connect, void(typename TWidget::connect_type, Ts...))
			ui::builder<TWidget>& connect(typename TWidget::connect_type type,
				Ts&& ... args) { this->el->connect(type, args...); return *this; }

		/* Do a manual configuration with a callback */
		ui::builder<TWidget>& configure(std::function<void(TWidget*)> conf) { conf(this->el); return *this; }
		/* Hide/unhide the widget */
		ui::builder<TWidget>& hide(bool hidden = true) { this->el->set_hidden(hidden); return *this; }
		/* Set the tag of the widget */
		ui::builder<TWidget>& tag(int t) { this->el->set_tag(t); return *this; }
		/* Set x position. note: you most likely want to call this last */
		ui::builder<TWidget>& x(float v) { this->el->set_x(v); return *this; }
		/* Set y position. note: you most likely want to call this last */
		ui::builder<TWidget>& y(float v) { this->el->set_y(v); return *this; }
		/* Set z position. note: you most likely want to call this last */
		ui::builder<TWidget>& z(float v) { this->el->set_z(v); return *this; }
		/* positions the widget under another widget */
		ui::builder<TWidget>& under(ui::BaseWidget *w, float pad = 3.0f) { this->el->set_y(ui::under(w, this->el, pad)); return *this; }
		/* positions the widget above another widget */
		ui::builder<TWidget>& above(ui::BaseWidget *w, float pad = 3.0f) { this->el->set_y(ui::above(w, this->el, pad)); return *this; }
		/* positions the widget right from another widget */
		ui::builder<TWidget>& right(ui::BaseWidget *w, float pad = 3.0f) { this->el->set_x(ui::right(w, this->el, pad)); return *this; }
		/* positions the widget left from another widget */
		ui::builder<TWidget>& left(ui::BaseWidget *w, float pad = 3.0f) { this->el->set_x(ui::left(w, this->el, pad)); return *this; }
		/* sets the y of the building widget to that of another one */
		ui::builder<TWidget>& align_y(ui::BaseWidget *w) { this->el->set_y(w->get_y()); return *this; }
		/* sets the x of the building widget to that of another one */
		ui::builder<TWidget>& align_x(ui::BaseWidget *w) { this->el->set_x(w->get_x()); return *this; }

		/* Add the built widget to a RenderQueue */
		void add_to(ui::RenderQueue& queue) { queue.push((ui::BaseWidget *) this->finalize()); }
		/* Add the built widget to a RenderQueue */
		void add_to(ui::RenderQueue *queue) { queue->push((ui::BaseWidget *) this->finalize()); }
		/* Add the built widget to a RenderQueue and your own pointer */
		void add_to(TWidget **ret, ui::RenderQueue& queue) { *ret = this->finalize(); queue.push((ui::BaseWidget *) *ret); }
		/* Add the built widget to a RenderQueue and your own pointer */
		void add_to(TWidget **ret, ui::RenderQueue *queue) { *ret = this->finalize(); queue->push((ui::BaseWidget *) *ret); }
		/* finalize the built widget and return a pointer to it */
		TWidget *finalize() { this->el->finalize(); TWidget *ret = this->el; this->el = nullptr; return ret; }


	private:
		TWidget *el = nullptr;


	};

	class SpriteStore
	{
	public:
		SpriteStore(const std::string& fname);
		SpriteStore() { }
		~SpriteStore();

		void open(const std::string& fname);
		size_t size();

		static C2D_Sprite get_by_id(ui::sprite id);


	private:
		C2D_SpriteSheet sheet = nullptr;


	};

	/* A wrapper for ui::BaseWidget with automatic:tm: management */
	template <typename TWidget>
	class ScopedWidget
	{
	public:
		~ScopedWidget()
		{
			if(this->wid != nullptr)
			{
				this->wid->destroy();
				delete this->wid;
			}
		}

		template <typename ... Ts>
		void setup(ui::Screen scr, Ts&& ... args)
		{
			this->wid = new TWidget(scr);
			this->wid->setup(args...);
		}

		/* for some widgets it's of great importance to call this when
		 * you're finished configuring, you should always do it just in case */
		void finalize()
		{
			this->wid->finalize();
		}

		// Shortcut
		bool render(const ui::Keys& keys) { return this->wid->render(keys); }
		TWidget *ptr() { return this->wid; }


	private:
		TWidget *wid = nullptr;


	};

	namespace next
	{
		class Text : public ui::BaseWidget
		{ UI_WIDGET("Text")
		public:
			void setup(const std::string& label);
			void destroy() override;

			bool render(const ui::Keys&) override;
			float height() override;
			float width() override;

			void resize(float x, float y);
			void autowrap();
			void scroll(); /* only supported with !center(), doesn't look amazing with multiline text but works */

			void set_center_x() override;

			/* sets the text of the widget to a new string */
			void set_text(const std::string& label);
			/* gets the current text of the widget */
			const std::string& get_text();


		private:
			void push_str(const std::string& str);
			void prepare_arrays();
			void reset_scroll();

			float xsiz = 0.65f, ysiz = 0.65f;
			std::vector<C2D_Text> lines;
			C2D_TextBuf buf = nullptr;
			bool doAutowrap = false;
			float lineHeigth = 0.0f;
			bool drawCenter = false;
			bool doScroll = false;
			std::string text;

			struct ScrollCtx {
				size_t timing;
				size_t offset;
				float height;
				float width;
				float rx;
			} sctx;


		};

		class Sprite : public ui::BaseWidget
		{ UI_WIDGET("Sprite")
		public:
			void setup(C2D_Sprite sprite);

			bool render(const ui::Keys&) override;
			float height() override;
			float width() override;

			void set_x(float x) override;
			void set_y(float y) override;
			void set_z(float z) override;

			void set_center(float x, float y);
			void rotate(float degs);


		private:
			C2D_Sprite sprite;


		};

		class Button : public ui::BaseWidget
		{ UI_WIDGET("Button")
		public:
			using click_cb_t = std::function<bool()>;

			void setup(const std::string& label);
			void setup(const C2D_Sprite& sprite);
			void destroy() override;
			void setup();

			bool render(const ui::Keys&) override;
			float height() override;
			float width() override;

			void set_border(bool b);

			void resize_children(float x, float y);
			void resize(float x, float y);
			void set_x(float x) override;
			void set_y(float x) override;
			void set_z(float z) override;

			/* autowrap for text size */
			void autowrap();

			enum connect_type { click, nobg };
			/* click */
			void connect(connect_type type, click_cb_t callback);
			/* nobg */
			void connect(connect_type type);

			float textwidth();


		private:
			click_cb_t on_click = []() -> bool { return true; };
			bool showBg = true, showBorder = false;
			float ox = 0.0f, oy = 0.0f;
			float w = 0.0f, h = 0.0f;
			ui::BaseWidget *widget;

			void readjust();


		};

		/* utility */

		class ButtonCallback : public ui::BaseWidget
		{ UI_WIDGET("ButtonCallback")
		public:
			void setup(u32 keys);

			bool render(const ui::Keys&) override;
			float height() override { return 0.0f; }
			float width() override { return 0.0f; }

			enum connect_type {
				none,
				kdown,
				kheld,
				kup,
			};
			/* activate */
			void connect(connect_type type, std::function<bool(u32)> cb);


		private:
			std::function<bool(u32)> cb = [](u32) -> bool { return true; };
			connect_type type = none;
			u32 keys;


		};
	}
}

#endif

