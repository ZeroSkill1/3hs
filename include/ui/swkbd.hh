
#ifndef inc_ui_swkbd_hh
#define inc_ui_swkbd_hh

#include <ui/base.hh>
#include <3ds.h>

#include <functional>
#include <string>


namespace ui
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

	/* Gets string input with ui::AppletSwkbd */
	std::string keyboard(std::function<void(ui::AppletSwkbd *)> configure,
		SwkbdButton *btn = nullptr, SwkbdResult *res = nullptr);
	/* Gets a number with ui::AppletSwkbd */
	uint64_t numpad(std::function<void(ui::AppletSwkbd *)> configure,
		size_t length = 10, SwkbdButton *btn = nullptr, SwkbdResult *res = nullptr);

/*	class Swkbd
	{

	};*/
}

#endif

