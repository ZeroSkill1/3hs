
#include "util.hh"

#include <widgets/konami.hh>

#include <ui/image_button.hh>
#include <ui/button.hh>
#include <ui/text.hh>

#include <ui/base.hh>


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


std::string swap_desc(std::string text)
{
	ui::Text *action = ui::wid()->get<ui::Text>("curr_action_desc");
	std::string ret = action->value();
	action->replace_text(text);
	return ret;
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

bool next::set_focus(bool focus)
{
	bool ret = ui::RenderQueue::global()->find_tag(ui::tag::action)->is_hidden();
	ui::RenderQueue::global()->find_tag(ui::tag::settings)->set_hidden(focus);
	ui::RenderQueue::global()->find_tag(ui::tag::action)->set_hidden(focus);
	ui::RenderQueue::global()->find_tag(ui::tag::more)->set_hidden(focus);
	return ret;
}

std::string next::set_desc(const std::string& nlabel)
{
	ui::next::Text *action = ui::RenderQueue::global()->find_tag<ui::next::Text>(ui::tag::action);
	std::string old = action->get_text();
	action->set_text(nlabel);
	return old;
}

