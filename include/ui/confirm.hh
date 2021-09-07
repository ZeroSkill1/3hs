
#ifndef inc_ui_confirm_hh
#define inc_ui_confirm_hh

#include "ui/button.hh"
#include "ui/core.hh"
#include "ui/text.hh"

#include <string>


namespace ui
{
	class Confirm : public ui::Widget
	{
	public:
		Confirm(std::string label, bool& res);

		ui::Results draw(ui::Keys&, ui::Scr) override;


	private:
		ui::Button yes;
		ui::Button no;
		ui::Text usr;

		bool& returns;


	};
}

#endif

