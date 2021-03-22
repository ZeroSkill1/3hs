
#ifndef __ui_scrolling_text_hh__
#define __ui_scrolling_text_hh__

#include <ui/core.hh>


#define STEXT_BUFSIZ 128


namespace ui
{
	class ScrollingText : public Widget
	{
	public:
		ScrollingText(float x, float y, std::string str);
		~ScrollingText();

		ui::Results draw(ui::Keys&, ui::Scr) override;
		void replace_text(std::string str);

		void start_scroll();
		void stop_scroll();


	private:
		std::string rtext;
		C2D_TextBuf buf;
		C2D_Text text;

		bool scrolling = false;
		size_t offset = 0;
		size_t index = 0;
		size_t toff = 0;
		size_t max;
		float x;
		float y;


	};
}


#endif
