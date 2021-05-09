
#include "ui/press_to_continue.hh"


ui::PressToContinue::PressToContinue(u32 field)
	: Widget("press_to_continue"), buttons(field)
{ }

ui::PressToContinue::PressToContinue()
	: Widget("press_to_continue") { }


void ui::PressToContinue::press_any()
{ this->pressAny = true; }

void ui::PressToContinue::add(u32 button)
{ this->buttons |= button; }

void ui::PressToContinue::remove(u32 button)
{ this->buttons &= ~(button); }


ui::Results ui::PressToContinue::draw(ui::Keys& keys, ui::Scr)
{
	switch(this->mode)
	{
	case PressToContinue::kDown:
		return keys.kDown & this->buttons ? ui::Results::quit_loop : ui::Results::go_on;
	case PressToContinue::kHeld:
		return keys.kHeld & this->buttons ? ui::Results::quit_loop : ui::Results::go_on;
	case PressToContinue::kUp:
		return keys.kUp & this->buttons ? ui::Results::quit_loop : ui::Results::go_on;
	default: return ui::Results::go_on;
	}
}

