
#ifndef inc_ui_confirm_hh
#define inc_ui_confirm_hh

#include "ui/button.hh"
#include "ui/core.hh"
#include "ui/text.hh"

#include <string>

#define CONFIRM_YES "Confirm"
#define CONFIRM_NO "Cancel"

#define CONFIRM_LEN (CONFIRM_YES_LEN+CONFIRM_NO_LEN)
#define CONFIRM_YES_LEN 6
#define CONFIRM_NO_LEN  7


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

