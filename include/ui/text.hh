
#ifndef __ui_text_hh__
#define __ui_text_hh__

#include "ui/core.hh"


#define TEXT_TXTBUFSIZ 4096


namespace ui
{
	typedef struct 
	{
		std::string text;
		float x;
		float y;
		float sizeX;
		float sizeY;
	} WText;

	typedef struct _WText
	{
		C2D_Text text;
		float x;
		float y;
		float sizeX;
		float sizeY;
	} _WText;


	WText mkWText(std::string text, float x, float y, float sizeX = 0.50f, float sizeY = 0.50f);
	WText mk_center_WText(std::string text, float y, float sizeX = 0.50f, float sizeY = 0.50f);
	float get_center_x(std::string text, float sizeX);

	class Text : public Widget
	{
	public:
		Text(WText text);
		~Text();

		bool draw(ui::Keys&, ui::Scr) override;
		void replace_text(std::string text);

	private:
		_WText text;
		C2D_TextBuf buf;

		void parse_text(C2D_Text *outTxt, C2D_TextBuf buf, std::string inTxt);
	};
}

#endif
