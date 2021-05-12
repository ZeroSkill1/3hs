
#ifndef __ui_scrolling_text_hh__
#define __ui_scrolling_text_hh__

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

		size_t length(float sizex, float sizey);
		void move(float x, float y);
		void start_scroll();
		void stop_scroll();
		void reset();


	private:
		void impl_replace_text(std::string str);

		std::string rtext;
		C2D_TextBuf buf;
		C2D_Text text;

		bool scrolling = false;
		size_t index = 0;
		int offset = -60;
		size_t max;
		float x;
		float y;


	};
}


#endif
