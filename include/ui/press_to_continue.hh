
#ifndef inc_ui_press_to_continue_hh
#define inc_ui_press_to_continue_hh

#include "ui/core.hh"


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
}

#endif

