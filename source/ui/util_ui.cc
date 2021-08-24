
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

// timeout_screen_helper

ui::TimeoutScreenHelper::TimeoutScreenHelper(const std::string& fmt, size_t nsecs, bool *shouldStop)
	: Widget("timeout_screen_helper"), fmt(fmt), startTime(time(NULL)), lastCheck(this->startTime), shouldStop(shouldStop), nsecs(nsecs)
{
	this->text.screen = ui::Scr::top;
	this->text.set_basey(80.0f);
	this->text.center();

	this->update_text(this->startTime);
}

void ui::TimeoutScreenHelper::update_text(time_t now)
{
	std::string ntxt;
	ntxt.reserve(this->fmt.size());

	for(size_t i = 0; i < this->fmt.size(); ++i)
	{
		switch(this->fmt[i])
		{
		case '%':
			++i;
			if(this->fmt[i] == 't')
				ntxt += std::to_string(this->nsecs - (now - this->startTime));
			else ntxt.push_back(this->fmt[i]);
			break;

		default:
			ntxt.push_back(this->fmt[i]);
			break;

		}
	}

	this->text.replace_text(ntxt);
}

ui::Results ui::TimeoutScreenHelper::draw(ui::Keys& keys, ui::Scr screen)
{
	if(keys.kDown & KEY_START && this->shouldStop != nullptr)
	{
		*this->shouldStop = true;
		return ui::Results::quit_loop;
	}

	time_t now = time(NULL);
	if(this->lastCheck != now)
	{
		this->update_text(now);
		this->lastCheck = now;
	}

	if(now - this->startTime >= this->nsecs)
		return ui::Results::quit_loop;

	return this->text.draw(keys, screen);
}

// timeoutscreen()

bool ui::timeoutscreen(const std::string& fmt, size_t nsecs, bool allowCancel)
{
	ui::Widgets wids;
	bool ret = false;

	wids.push_back(new ui::TimeoutScreenHelper(fmt, nsecs, allowCancel ? &ret : nullptr));

	generic_main_breaking_startreplacing_loop(wids);
	return ret;
}


