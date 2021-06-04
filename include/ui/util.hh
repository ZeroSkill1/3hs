
#ifndef inc_ui_util_hh
#define inc_ui_util_hh

// This file contains several utility widgets
// that don't necessairly draw any visual
// elements

#include "ui/core.hh"

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
}


#endif

