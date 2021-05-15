
#ifndef __ui_swkbd_hh__
#define __ui_swkbd_hh__

#include "ui/core.hh"


namespace ui
{
	class AppletSwkbd : public Widget
	{
	public:
		AppletSwkbd(std::string *ret, int maxLen = 200, SwkbdType type = SWKBD_TYPE_NORMAL,
			int numBtns = 3);

		void validinput(SwkbdValidInput valid, u32 filterFlags = 0, u32 maxDigits = 0);
		void passmode(SwkbdPasswordMode mode);
		void init_text(std::string txt);
		void hint(std::string txt);

		SwkbdResult get_result();
		SwkbdButton get_button();

		// This will draw exactly once
		ui::Results draw(ui::Keys&, ui::Scr) override;


	private:
		std::string *sret = nullptr;
		SwkbdButton button;
		SwkbdState state;
		size_t len;


	};

/*	class Swkbd
	{

	};*/
}

#endif

