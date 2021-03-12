
#ifndef __ui_text_hh__
#define __ui_text_hh__

#include "ui/core.hh"


#define TEXT_TXTBUFSIZ 4096


namespace ui
{
	typedef struct 
	{
		std::string text;
		int x;
		int y;
	} WText;

	typedef struct _WText
	{
		C2D_Text text;
		int x;
		int y;
	} _WText;


	WText mkWText(std::string text, int x, int y);


	class Text : public Widget
	{
	public:
		Text(std::vector<WText> texts);
		Text(WText text);
		~Text();

		bool draw(ui::Keys&, ui::Scr) override;
		void add_text(WText text);


	private:
		std::vector<_WText> texts;
		C2D_TextBuf buf;

	};
}

#endif
