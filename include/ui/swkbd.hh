
#ifndef inc_ui_swkbd_hh
#define inc_ui_swkbd_hh

#include "ui/button.hh"
#include "ui/text.hh"
#include "ui/core.hh"

#include <ui/base.hh>

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

	namespace next
	{
		class AppletSwkbd : public ui::BaseWidget
		{ UI_WIDGET("AppletSwkbd")
		public:
			void setup(std::string *ret, int maxLen = 200, SwkbdType type = SWKBD_TYPE_NORMAL,
				int numBtns = 2);

			bool render(const ui::Keys&) override;
			float height() override;
			float width() override;

			enum connect_type { button, result };

			void connect(connect_type, SwkbdButton *b); /* button */
			void connect(connect_type, SwkbdResult *r); /* result */

			/* set the hint text, the text displayed transparantly
			 * if you don't have any text entered */
			void hint(const std::string& h);
			/* Sets the password mode, see enum SwkbdPasswordMode for possible values */
			void passmode(SwkbdPasswordMode mode);
			/* Sets the initial search text */
			void init_text(const std::string& t);
			/* Sets the valid input mode,
			 * filterflags is a bitfield of SWKBD_FILTER_* */
			void valid(SwkbdValidInput mode, u32 filterFlags = 0, u32 maxDigits = 0);


		private:
			SwkbdButton *buttonPtr = nullptr;
			SwkbdResult *resPtr = nullptr;
			SwkbdState state;
			std::string *ret;
			size_t len;


		};
	}

	/* Gets string input with ui::next::AppletSwkbd */
	std::string keyboard(std::function<void(ui::next::AppletSwkbd *)> configure,
		SwkbdButton *btn = nullptr, SwkbdResult *res = nullptr);
	/* Gets a number with ui::next::AppletSwkbd */
	uint64_t numpad(std::function<void(ui::next::AppletSwkbd *)> configure,
		size_t length = 10, SwkbdButton *btn = nullptr, SwkbdResult *res = nullptr);

/*	class Swkbd
	{

	};*/
}

#endif

