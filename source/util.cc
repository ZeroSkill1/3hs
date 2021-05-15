
#include "util.hh"

#include <widgets/konami.hh>

#include <ui/image_button.hh>
#include <ui/button.hh>
#include <ui/text.hh>


static void toggle_btn_konami()
{
	ui::wid()->for_each("image_button", [](ui::Widget *widget) -> void {
		((ui::ImageButton *) widget)->toggle();
	});

	ui::wid()->for_each("button", [](ui::Widget *widget) -> void {
		((ui::Button *) widget)->toggle();
	});

	ui::wid()->get<ui::Konami>("konami")->toggle();
}


std::string toggle_focus(std::string text)
{
	toggle_btn_konami();
	ui::Text *action = ui::wid()->get<ui::Text>("curr_action_desc");
	std::string ret = action->value();
	action->replace_text(text);
	return ret;
}

void toggle_focus()
{
	ui::wid()->get<ui::Text>("curr_action_desc")->toggle();
	toggle_btn_konami();
}

