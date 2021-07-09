
#include "ui/util.hh"


// press_to_continue

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

// press_button_callback

ui::PressButtonCallback::PressButtonCallback(std::function<ui::Results()> cb_, u32 keys_, bool triggerOnce_)
	: Widget("press_button_callback"), cb(cb_), triggerOnce(triggerOnce_), keys(keys_) { }

ui::Results ui::PressButtonCallback::draw(ui::Keys& keys, ui::Scr)
{
	if(this->triggerOnce && this->triggered)
		return ui::Results::go_on;
	if(keys.kDown & this->keys)
	{
		this->triggered = true;
		return this->cb();
	}
	return ui::Results::go_on;
}

// do_after_frames

ui::DoAfterFrames::DoAfterFrames(size_t frames, std::function<ui::Results()> cb)
	: Widget("do_after_frames"), on_done(cb), tFrames(frames)
{ }

ui::Results ui::DoAfterFrames::draw(ui::Keys&, ui::Scr)
{
	if(this->dFrames > this->tFrames) return ui::Results::go_on;
	ui::Results ret = ui::Results::go_on;
	if(this->dFrames == this->tFrames) ret = this->on_done();
	++this->dFrames; return ret;
}

