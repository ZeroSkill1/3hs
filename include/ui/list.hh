
#ifndef __ui_list_hh__
#define __ui_list_hh__

#include <ui/core.hh>

#include <functional>


#define LIST_TXTBUFSIZ 4096


namespace ui
{
	namespace constants
	{
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
		List(std::function<std::string(T&)> to_str, std::function<bool(T&, size_t)> on_select)
			: Widget("list"), point(0)
		{ this->init(to_str, on_select); }
	
		List(std::function<std::string(T&)> to_str, std::function<bool(T&, size_t)> on_select, std::vector<T> items)
			: Widget("list"), point(0)
		{
			this->init(to_str, on_select);
			this->items = items;
			this->text_update();
		}

		~List()
		{
			C2D_TextBufDelete(this->txtbuf);
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

		T& operator [] (size_t index)
		{ return this->items[index]; }

		bool draw(Keys& keys, Scr target) override
		{
			static int last = ui::constants::CURSOR_INIT;
			if((keys.kDown & KEY_DOWN) && this->point < this->items.size() - 1)
			{ ++point; last = ui::constants::CURSOR_INIT; }
			if((keys.kDown & KEY_UP) && this->point > 0)
			{ --point; last = ui::constants::CURSOR_INIT; }
			if((keys.kHeld & KEY_UP) && this->point > 0 && last > ui::constants::CURSOR_DELAY)
			{ --point; last = -1; }
			if((keys.kHeld & KEY_DOWN) && this->point < this->items.size() - 1 && last > ui::constants::CURSOR_DELAY)
			{ ++point; last = -1; }
			++last;


			for(size_t i = this->point > ui::constants::TOP_ITEMS
				? this->point - ui::constants::TOP_ITEMS
				: 0, j = 5; i < this->items.size(); ++i, ++j)
			{
				if(i == this->point) ui::draw_at(1, j, this->arrow, 0, 0.45f, 0.45f);
				ui::draw_at(3, j, this->txt[i], 0, 0.45f, 0.45f);
			}

			if(keys.kDown & KEY_A)
				return this->on_select(this->items[this->point], this->point);
			return true;
		}


	private:
		std::vector<C2D_Text> txt;
		C2D_TextBuf txtbuf;

		C2D_Text arrow;

		std::vector<T> items;
		size_t point;

		std::function<bool(T&, size_t)> on_select;
		std::function<std::string(T&)> to_str;
		

		void init(std::function<std::string(T&)> to_str, std::function<bool(T&, size_t)> on_select)
	 	{
			this->on_select = on_select;
			this->to_str = to_str;

			this->txtbuf = C2D_TextBufNew(LIST_TXTBUFSIZ);
			this->create_text(&this->arrow, "→");
		}
	};
}


#endif
