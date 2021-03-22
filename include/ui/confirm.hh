
#ifndef __ui_confirm_hh__
#define __ui_confirm_hh__

#include "ui/core.hh"
#include "ui/text.hh"

#include <string>

#define CONFIRM_YES "Confirm"
#define CONFIRM_NO "Cancel"

#define CONFIRM_LEN (CONFIRM_YES_LEN+CONFIRM_NO_LEN)
#define CONFIRM_YES_LEN 6
#define CONFIRM_NO_LEN  7


namespace ui
{
	class Confirm : public ui::Widget
	{
	public:
		Confirm(std::string label, bool& res);

		void parse_text(C2D_Text *outTxt, C2D_TextBuf buf, std::string inTxt);
		ui::Results draw(ui::Keys&, ui::Scr) override;


	private:
		C2D_TextBuf buf;
		C2D_Text usr;
		C2D_Text yes;
		C2D_Text no;

		float ux;
		float yx;
		float nx;

		float yfx;
		float nfx;

		bool& returns;


	};
}


#endif
