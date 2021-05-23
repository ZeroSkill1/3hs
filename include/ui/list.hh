
#ifndef inc_ui_list_hh
#define inc_ui_list_hh

#include <ui/scrollingText.hh>
#include <ui/core.hh>
#include <functional>
#include <3rd/log.hh>

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

			lverbose << "Created textbuf of size: " << ret + LIST_ARR_SIZ;
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

		void text_update()
		{
			C2D_TextBufClear(this->txtbuf);
			this->create_text(&this->arrow, "→");
			this->txt.clear();

			for(T& val : this->items)
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

		ui::Results draw(Keys& keys, Scr target) override
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
}

#undef exec_onch
#endif

