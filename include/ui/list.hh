
#ifndef __ui_list_hh__
#define __ui_list_hh__

#include <ui/core.hh>

#include <functional>
#include <cstdio>


#define TXTBUFSIZ 4096


namespace ui
{
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
		List(std::function<std::string(T&)> to_str, std::function<bool(size_t)> on_select)
			: Widget("list"), point(0)
		{
			this->on_select = on_select;
			this->to_str = to_str;

			this->txtbuf = C2D_TextBufNew(TXTBUFSIZ);
			this->create_text(&this->arrow, ">");
		}

		~List()
		{
			C2D_TextBufDelete(this->txtbuf);
		}

		void text_update()
		{
			C2D_TextBufClear(this->txtbuf);
			this->txt.clear();

			for(const T& val : this->items)
			{
				std::string str = this->to_str(val);
				C2D_Text text; this->create_text(&text, str);
				this->txt.push_back(text);
			}
		}

		void create_text(C2D_Text *res, std::string txt)
		{
			C2D_TextParse(res, this->txtbuf, txt.c_str());
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
			if((keys.kDown & KEY_DOWN) && this->point < this->items.size() - 1)
				++point;
			if((keys.kDown & KEY_UP) && this->point > 0)
				--point;

			for(size_t i = 0; i < this->items.size(); ++i)
			{
				if(i == this->point) ui::draw_at(0, i, this->arrow);
				ui::draw_at(2, i, this->txt[i]);
			}

			if(keys.kDown & KEY_A)
				return this->on_select(this->point);
			return true;
		}


	private:
		std::vector<C2D_Text> txt;
		C2D_TextBuf txtbuf;

		C2D_Text arrow;

		std::vector<T> items;
		size_t point;

		std::function<std::string(T&)> to_str;
		std::function<bool(size_t)> on_select;


	};
}


#endif
