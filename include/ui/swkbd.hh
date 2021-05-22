
#ifndef inc_ui_swkbd_hh
#define inc_ui_swkbd_hh

#include "ui/core.hh"

#include <functional>


namespace ui
{
	class AppletSwkbd : public Widget
	{
	public:
		AppletSwkbd(std::string *ret, int maxLen = 200, SwkbdType type = SWKBD_TYPE_NORMAL,
			int numBtns = 2);

		void validinput(SwkbdValidInput valid, u32 filterFlags = 0, u32 maxDigits = 0);
		void passmode(SwkbdPasswordMode mode);
		void init_text(std::string txt);
		void hint(std::string txt);

		SwkbdResult get_result();
		SwkbdButton get_button();

		// This will draw exactly once
		ui::Results draw(ui::Keys&, ui::Scr) override;

		static std::string read(std::function<void(AppletSwkbd *)> setup = empty_setup,
			SwkbdResult *result = nullptr, SwkbdButton *button = nullptr,
			int maxlen = 200, SwkbdType type = SWKBD_TYPE_NORMAL, int numBtns = 2);
		static void empty_setup(AppletSwkbd *) { };


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

