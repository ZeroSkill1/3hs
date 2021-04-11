
#ifndef __ui_button_hh__
#define __ui_button_hh__

#include "ui/bindings.hh"
#include "ui/core.hh"

#include <functional>

#define button_on_click std::function<ui::Results()>


namespace ui
{
	class Button : public Widget
	{
	public:
		Button(std::string label, u32 color, float x1, float y1, float x2, float y2);

		void set_on_click(button_on_click cb);
		void change_label(std::string label);

		ui::Results draw(ui::Keys&, ui::Scr) override;


	private:
		c2d::TextBuf buf;
		c2d::Text label;

		button_on_click on_click = [](){return ui::Results::go_on;};

		float x1, x2;
		float y1, y2;
		u32 color;

	};
}

#endif

