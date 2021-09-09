
#ifndef inc_ui_scrolling_text_hh
#define inc_ui_scrolling_text_hh

#include <ui/core.hh>


namespace ui
{
	class ScrollingText : public Widget
	{
	public:
		ScrollingText(float x, float y, std::string str);
		ScrollingText();
		~ScrollingText();

		ui::Results draw(ui::Keys&, ui::Scr) override;
		void replace_text(std::string str);

		void resize(float sizex, float sizey);
		void move(float x, float y);
		void start_scroll();
		void stop_scroll();
		size_t length();
		void reset();

		void scroll_if_overflow(ui::Scr screen);


	private:
		void impl_replace_text(std::string str);
		void update_h();

		std::string rtext;
		C2D_TextBuf buf;
		C2D_Text text;

		float sizey = ui::constants::FSIZE;
		float sizex = ui::constants::FSIZE;

		bool scrolling = false;
		size_t timing = 1; // one because 0%x=0
		size_t offset = 0;
		size_t strlen = 0;
		float texth = 0;
		float ogx;
		float x;
		float y;


	};
}


#endif
