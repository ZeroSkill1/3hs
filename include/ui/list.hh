
#ifndef inc_ui_list_hh
#define inc_ui_list_hh

#include <ui/scrollingText.hh>
#include <ui/core.hh>
#include <functional>
#include <3rd/log.hh>

#include <ui/base.hh>
#include <panic.hh>

#define exec_onch() this->on_change(this, this->point)

#define LIST_ARR ("→")
// Unicode ......
#define LIST_ARR_SIZ 3


namespace ui
{
	namespace constants
	{
		constexpr int MAX_PER_SCREEN = 10;
		constexpr int SUPER_OFF = MAX_PER_SCREEN;
		constexpr int CURSOR_DELAY = 7;
		constexpr int CURSOR_INIT = -4;
		constexpr int TOP_ITEMS = 2;
	}

	/**
	 * typename T: The type of vector to use
	 * to_str: A callback to convert T to a string
	 * 	T&: The item to convert
	 * on_select: A callback that gets executed when A is pressed on a list item
	 * 	size_t: The selected index
	 */
	template<typename T>
	class List : public Widget
	{
	public:
		using list_onsel_cb = std::function<ui::Results(List<T> *, size_t, u32)>;
		using list_onch_cb = std::function<void(List<T> *, size_t)>;
		using list_sort_cb = std::function<void(std::vector<T>&)>;
		using list_tostr_cb = std::function<std::string(T&)>;


		List(list_tostr_cb to_str, list_onsel_cb on_select)
			: Widget("list"), items({ }), point(0)
		{ this->init(to_str, on_select); }

		List(list_tostr_cb to_str, list_onsel_cb on_select, std::vector<T>& items_)
			: Widget("list"), items(items_), point(0)
		{
			this->init(to_str, on_select);
			this->update_text_reg();
		}

		~List()
		{
			C2D_TextBufDelete(this->txtbuf);
		}

		void set_on_change(list_onch_cb cb)
		{
			this->on_change = cb;
		}

		size_t combined_len()
		{
			size_t ret = 0;
			for(T& val : this->items)
				ret += this->to_str(val).size();

			return ret + LIST_ARR_SIZ;
		}

		void set_pointer(size_t ni)
		{ this->point = ni; }

		const size_t& get_pointer()
		{ return this->point; }

		bool out_of_bounds(size_t index)
		{ return index >= this->items.size(); }

		void update_text_reg()
		{
			C2D_TextBufClear(this->txtbuf);
			this->txtbuf = C2D_TextBufResize(this->txtbuf, this->combined_len() + 1);
			this->text_update();
		}

		void text_update(list_sort_cb sort = [](std::vector<T>&) -> void { })
		{
			C2D_TextBufClear(this->txtbuf);
			this->create_text(&this->arrow, "→");
			this->txt.clear();

			std::vector<T> copy(this->items);
			sort(copy);

			for(T& val : copy)
			{
				std::string str = this->to_str(val);
				C2D_Text text; this->create_text(&text, str);
				this->txt.push_back(text);
			}
		}

		void create_text(C2D_Text *res, std::string txt)
		{
			ui::parse_text(res, this->txtbuf, txt);
			C2D_TextOptimize(res);
		}

		void append(T val)
		{
			this->items.push_back(val);
			std::string str = this->to_str(val);
			C2D_Text text; this->create_text(&text, str);
			this->txt.push_back(text);
		}

		T& at(size_t index)
		{ return (*this)[index]; }

		T& operator [] (size_t index)
		{ return this->items[index]; }

		ui::Results draw(Keys& keys, Scr) override
		{
			static int last = ui::constants::CURSOR_INIT;

			if((keys.kDown & KEY_DOWN) && this->point < this->items.size() - 1)
			{ ++this->point; last = ui::constants::CURSOR_INIT; exec_onch(); }
			else if((keys.kDown & KEY_UP) && this->point > 0)
			{ --this->point; last = ui::constants::CURSOR_INIT; exec_onch(); }
			else if((keys.kDown & KEY_LEFT) && this->point >= ui::constants::SUPER_OFF)
			{ this->point -= ui::constants::SUPER_OFF; last = ui::constants::CURSOR_INIT; exec_onch(); }
			else if((keys.kDown & KEY_RIGHT) && this->point < this->min(this->items.size(), ui::constants::SUPER_OFF))
			{ this->point += ui::constants::SUPER_OFF; last = ui::constants::CURSOR_INIT; exec_onch(); }

			else if((keys.kHeld & KEY_DOWN) && this->point < this->items.size() - 1 && last > ui::constants::CURSOR_DELAY)
			{ ++this->point; last = -1; exec_onch(); }
			else if((keys.kHeld & KEY_UP) && this->point > 0 && last > ui::constants::CURSOR_DELAY)
			{ --this->point; last = -1; exec_onch(); }
			else if((keys.kHeld & KEY_LEFT) && this->point >= ui::constants::SUPER_OFF && last > ui::constants::CURSOR_DELAY)
			{ this->point -= ui::constants::SUPER_OFF; last = -3; exec_onch(); }
			else if((keys.kHeld & KEY_RIGHT) && this->point < this->min(this->items.size(), ui::constants::SUPER_OFF) && last > ui::constants::CURSOR_DELAY)
			{ this->point += ui::constants::SUPER_OFF; last = -3; exec_onch(); }

			else if((keys.kDown | keys.kHeld) & KEY_RIGHT && last > ui::constants::CURSOR_DELAY)
			{ this->point = this->items.size() - 1; exec_onch(); }
			else if((keys.kDown | keys.kHeld) & KEY_LEFT && last > ui::constants::CURSOR_DELAY)
			{ this->point = 0; exec_onch(); }

			++last;


			for(size_t i = this->point > ui::constants::TOP_ITEMS
				? this->point - ui::constants::TOP_ITEMS
				: 0, j = 4, k = 0; i < this->items.size()
				&& k < constants::MAX_PER_SCREEN; ++i, ++j, ++k)
			{
				if(i == this->point) ui::draw_at(1, j, this->arrow, 0);
				ui::draw_at(3, j, this->txt[i], 0);
			}

			if(keys.kDown & this->buttonCombo)
				return this->on_select(this, this->point, keys.kDown);
			return ui::Results::go_on;
		}

		void add_button(u32 key)
		{ this->buttonCombo |= key; }


	private:
		std::vector<C2D_Text> txt;
		C2D_TextBuf txtbuf;

		C2D_Text arrow;

		std::vector<T>& items;
		size_t point;

		list_onch_cb on_change = [](List<T>*,size_t){};
		list_onsel_cb on_select;
		list_tostr_cb to_str;

		u32 buttonCombo = KEY_A;

		size_t min(size_t base, size_t other)
		{
			if(other > base) return 0;
			return base - other;
		}

		void init(list_tostr_cb to_str, list_onsel_cb on_select)
	 	{
			this->on_select = on_select;
			this->to_str = to_str;

			this->txtbuf = C2D_TextBufNew(this->combined_len() + 1);
			this->create_text(&this->arrow, "→");
		}
	};

	namespace next
	{
		template <typename T>
		class List : public ui::BaseWidget
		{ UI_WIDGET("List")
		public:
			using on_select_type = std::function<bool(List<T> *, size_t, u32)>;
			using on_change_type = std::function<void(List<T> *, size_t)>;
			using to_string_type = std::function<std::string(const T&)>;

			static constexpr size_t button_timeout_held = 7;
			static constexpr float scrollbar_width = 5.0f;
			static constexpr float selector_offset = 3.0f;
			static constexpr float text_spacing = 17.0f;
			static constexpr size_t button_timeout = 11;
			static constexpr float text_offset = 6.0f;
			static constexpr float text_size = 0.65;

			enum connect_type { select, change, to_string, buttons };

			void setup(std::vector<T> *items)
			{
				this->charsLeft = this->capacity = 1;
				this->buf = C2D_TextBufNew(1);
				this->items = items;

				this->buttonTimeout = 0;
				this->amountRows = 12;
				this->keys = KEY_A;
				this->view = 0;
				this->pos = 0;

				this->sx = ui::screen_width(this->screen) - scrollbar_width - 5.0f;
			}

			void destroy()
			{
				if(this->buf != nullptr)
					C2D_TextBufDelete(this->buf);
			}

			void finalize() override { this->update(); }

			bool render(const ui::Keys& keys) override
			{
				/* handle input */
				u32 effective = keys.kDown | keys.kHeld;

				if(this->buttonTimeout != 0) {
					--this->buttonTimeout;
					// please don't scream at me for using goto as a convenience
					goto builtin_controls_done;
				}

				if(effective & KEY_UP)
				{
					this->buttonTimeout = button_timeout;
					if(this->pos > 0)
					{
						if(this->pos == this->view)
							--this->view;
						--this->pos;
					}
					else
					{
						this->pos = this->lines.size() - 1;
						this->view = this->last_full_view();
					}
					this->on_change_(this, this->pos);
					this->update_scrolldata();
				}

				if(effective & KEY_DOWN)
				{
					this->buttonTimeout = button_timeout;
					size_t old = this->pos;
					if(this->pos < this->lines.size() - 1)
					{
						++this->pos;
						if(this->pos == this->view + this->amountRows - 1 && this->view < this->last_full_view())
							++this->view;
					}
					else
					{
						this->view = 0;
						this->pos = 0;
					}
					if(this->pos != old)
					{
						this->on_change_(this, this->pos);
						this->update_scrolldata();
					}
				}

				if(effective & KEY_LEFT)
				{
					this->buttonTimeout = button_timeout;
					this->view -= this->min<int>(this->amountRows, this->view);
					this->pos -= this->min<int>(this->amountRows, this->pos);
					this->on_change_(this, this->pos);
					this->update_scrolldata();
				}

				if(effective & KEY_RIGHT)
				{
					this->buttonTimeout = button_timeout;
					this->view = this->min<size_t>(this->view + this->amountRows, this->last_full_view());
					this->pos = this->min<size_t>(this->pos + this->amountRows, this->lines.size() - 1);
					this->on_change_(this, this->pos);
					this->update_scrolldata();
				}

				if(keys.kHeld & (KEY_UP | KEY_DOWN | KEY_LEFT | KEY_RIGHT))
					this->buttonTimeout = button_timeout_held; /* the timeout is a bit less if we want to hold */

builtin_controls_done:

				/* render the on-screen elements */
				size_t end = this->view + (this->lines.size() > this->amountRows
					? this->amountRows - 1 : this->lines.size());
				for(size_t i = this->view, j = 0; i < end; ++i, ++j)
				{
					// TODO: Theme support
					if(i == this->pos)
					{
						constexpr u32 white = C2D_Color32(0xFF, 0xFF, 0xFF, 0xFF);
						C2D_DrawLine(this->x, this->y + text_spacing * j + selector_offset,
							white, this->x, this->y + text_spacing * j + this->selh - selector_offset,
							white, 1.5f, this->z);
					}

					C2D_DrawText(&this->lines[i], C2D_WithColor, this->x + text_offset,
						this->y + text_spacing * j, this->z, text_size, text_size,
						C2D_Color32(0xFF, 0xFF, 0xFF, 0xFF));
				}

				/* render scrollbar */
				if(this->lines.size() > this->amountRows)
				{
					C2D_DrawRectSolid(this->sx - 1.0f, 0.0f, this->z, ui::screen_width(this->screen) - this->sx + 1.0f,
						ui::screen_height(), C2D_Color32(0x1C, 0x20, 0x21, 0xFF));
					C2D_DrawRectSolid(this->sx, this->sy, this->z, 5.0f, this->sh,
						C2D_Color32(0x3C, 0x3C, 0x3C, 0xFF));
				}

				if(keys.kDown & this->keys)
					return this->on_select_(this, this->pos, keys.kDown);
				return true;
			}

			float height() override
			{
				return this->lines.size() < this->amountRows
					? this->lines.size() * text_spacing
					: this->amountRows * text_spacing;
			}

			float width() override
			{
				return this->sx + scrollbar_width - this->x;
			}

			void connect(connect_type t, on_select_type cb)
			{
				if(t != select) panic("EINVAL");
				this->on_select_ = cb;
			}

			void connect(connect_type t, on_change_type cb)
			{
				if(t != change) panic("EINVAL");
				this->on_change_ = cb;
			}

			void connect(connect_type t, to_string_type cb)
			{
				if(t != to_string) panic("EINVAL");
				this->to_string_ = cb;
			}

			void connect(connect_type t, u32 k)
			{
				if(t != buttons) panic("EINVAL");
				this->keys |= k;
			}

			/* Rerenders all items in the list
			 * NOTE: if you simply want to update a certain item
			 *       consider using update(size_t)
			 **/
			void update()
			{
				this->clear();
				this->lines.reserve(this->items->size() - this->lines.capacity());
				for(T& i : *this->items)
					this->append_text(i);
				this->update_scrolldata();
			}

			/* Rerenders an item in the list
			 **/
			void update(size_t i)
			{
				std::string s = this->to_string_(i);
			}

			/* Amount of items ready to be rendered
			 **/
			size_t size() { return this->lines.size(); }

			/* Appends an item to items and appends it to the available texts to be rendered
			 **/
			void append(const T& val)
			{
				this->items->push_back(val);
				this->append_text(val);
			}

			/* Add a key that triggers select
			 **/
			void key(u32 k)
			{
				this->keys |= k;
			}

			/* Amount of items visable
			 **/
			size_t visable()
			{
				return this->lines.size() > this->amountRows
					? this->amountRows : this->lines.size();
			}

			/* Returns the element at i with range checking
			 **/
			T& at(size_t i)
			{
				return this->items->at(i);
			}

			/* Gets the current cursor position
			 **/
			size_t get_pos()
			{
				return this->pos;
			}

			/* Sets the current cursor position
			 **/
			void set_pos(size_t p)
			{
				if(p > 0 && p < this->lines.size())
					this->pos = p;
			}


		private:
			on_select_type on_select_ = [](List<T> *, size_t, u32) -> bool { return true; };
			on_change_type on_change_ = [](List<T> *, size_t) -> void { };
			to_string_type to_string_ = [](const T&) -> std::string { return ""; };

			size_t charsLeft;
			size_t capacity;
			C2D_TextBuf buf;

			float selw; /* width of the selected text */
			float selh; /* height of the selected text */
			float sh; /* scrollbar height */
			float sx; /* scrollbar x */
			float sy; /* scrollbar y */

			std::vector<C2D_Text> lines;
			std::vector<T> *items;

			u32 keys; /* keys that trigger select */

			int buttonTimeout; /* amount of frames that need to pass before the next button can be pressed */
			size_t amountRows; /* amount of rows to be drawn */
			size_t view; /* first viewable element */
			size_t pos; /* cursor position */

			template <typename TInt>
			TInt min(TInt a, TInt b)
			{
				return a > b ? b : a;
			}

			void append_text(const T& val)
			{
				std::string s = this->to_string_(val);
				this->alloc(s.size() + 1 /* +1 for NULL term */);
				this->lines.emplace_back();
				C2D_TextParse(&this->lines.back(), this->buf, s.c_str());
				C2D_TextOptimize(&this->lines.back());
			}

			void update_scrolldata()
			{
				/* selw, selh */
				C2D_TextGetDimensions(&this->lines[this->pos], text_size, text_size,
					&this->selw, &this->selh);
				/* sy */
				this->sy = ((float) this->view / (float) this->lines.size()) * this->height()
					+ this->y;
				/* sh */
				this->sh = this->min<float>(
						ui::screen_height() - this->sy - this->y,
						((float) this->visable() / (float) this->lines.size()) * this->height()
					);
			}

			void clear()
			{
				this->charsLeft = this->capacity;
				C2D_TextBufClear(this->buf);
				this->lines.clear();
			}

			void alloc(size_t n)
			{
				if(this->charsLeft < n)
				{
					this->capacity += n * 2;
					this->buf = C2D_TextBufResize(this->buf, this->capacity);
					this->charsLeft += n * 2;
				}

				this->charsLeft -= n;
			}

			size_t last_full_view()
			{
				return this->lines.size() > this->amountRows
					? this->lines.size() - this->amountRows + 1 : 0;
			}


		};
	}
}

#undef exec_onch
#endif

