
#ifndef __ui_text_hh__
#define __ui_text_hh__

#include "ui/core.hh"

#include <functional>


namespace ui
{
	typedef struct WText
	{
		std::function<void(WText *)> calc;
		std::string text;
		float sizeX;
		float sizeY;
		float x;
		float y;

		C2D_Text ctext;
	} WText;
	static void __empty(WText*){}

	WText mkWText(std::string text, float x, float y, float sizeX = 0.50f, float sizeY = 0.50f, std::function<void(WText *)> callback = __empty);
	WText mk_right_WText(std::string text, float y, float pad = 1.0f, float sizeX = 0.50f, float sizeY = 0.50f, ui::Scr scr = ui::Scr::top);
	WText mk_center_WText(std::string text, float y, float sizeX = 0.50f, float sizeY = 0.50f, ui::Scr scr = ui::Scr::top);
	float get_center_x(std::string text, float sizeX, ui::Scr scr = ui::Scr::top);

	class Text : public Widget
	{
	public:
		Text(WText text);
		~Text();

		bool draw(ui::Keys&, ui::Scr) override;
		void replace_text(std::string text);


	protected:
		C2D_TextBuf buf;
		WText text;

		void parse_text(C2D_Text *outTxt, C2D_TextBuf buf, std::string inTxt);


	};
}

#endif
