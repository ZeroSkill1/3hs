
#ifndef inc_ui_button_hh_
#define inc_ui_button_hh_

#include "ui/bindings.hh"
#include "ui/core.hh"

#include <functional>


namespace ui
{
	typedef std::function<ui::Results(bool)> button_on_click;

	class Button : public Widget
	{
	public:
		Button(std::string label, float x1, float y1, float x2, float y2);

		void set_on_click(button_on_click cb);
		void change_label(std::string label);

		ui::Results draw(ui::Keys&, ui::Scr) override;
		ui::Results click(bool inFrame = true);

		void highlight(bool value = true);
		void toggle_click();


	private:
		c2d::TextBuf buf;
		c2d::Text label;

		button_on_click on_click = [](bool){ return ui::Results::go_on; };

		bool shouldHighlight = false;
		bool clickable = true;


		float x1, x2;
		float y1, y2;


	};
}

#endif

