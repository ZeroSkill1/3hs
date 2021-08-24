
#ifndef inc_ui_util_hh
#define inc_ui_util_hh

// This file contains several utility widgets
// that don't necessairly draw any visual
// elements

#include "ui/core.hh"
#include "ui/text.hh"

#include <functional>


namespace ui
{
	class PressToContinue : public Widget
	{
	public:
		PressToContinue(u32 field);
		PressToContinue();

		void remove(u32 button);
		void add(u32 button);
		void press_any();

		ui::Results draw(ui::Keys&, ui::Scr) override;

		enum modes { kHeld, kDown, kUp };


	private:
		bool pressAny = false;
		modes mode = kDown;
		u32 buttons;


	};

	class PressButtonCallback : public Widget
	{
	public:
		PressButtonCallback(std::function<ui::Results()> cb, u32 keys, bool triggerOnce = true);

		ui::Results draw(ui::Keys&, ui::Scr) override;


	private:
		std::function<ui::Results()> cb;
		bool triggered = false;
		bool triggerOnce;
		u32 keys;


	};


	class DoAfterFrames : public Widget
	{
	public:
		DoAfterFrames(size_t frames, std::function<ui::Results()> cb);

		ui::Results draw(ui::Keys&, ui::Scr) override;


	private:
		std::function<ui::Results()> on_done;
		size_t tFrames = 0;
		size_t dFrames = 0;


	};

	class TimeoutScreenHelper : public Widget
	{
	public:
		TimeoutScreenHelper(const std::string& fmt, size_t nsecs, bool *shouldStop = nullptr);

		ui::Results draw(ui::Keys&, ui::Scr) override;


	private:
		void update_text(time_t now);

		const std::string& fmt;
		ui::WrapText text;
		time_t startTime;
		time_t lastCheck;
		bool *shouldStop;
		size_t nsecs;


	};

	/**
	 * use `%t' as a placeholder for the seconds left until the end of the timeout
	 * returns true if the user cancelled the timeout (if allowed)
	 */
	bool timeoutscreen(const std::string& fmt, size_t nsecs, bool allowCancel = true);
}


#endif

