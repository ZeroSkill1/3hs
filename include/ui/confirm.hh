
#ifndef __ui_confirm_hh__
#define __ui_confirm_hh__

#include "ui/core.hh"
#include "ui/text.hh"

#include <string>


namespace ui
{
	class Confirm : public ui::Text
	{
	public:
		Confirm(std::string label, bool& res);

		bool draw(ui::Keys&, ui::Scr) override;


	private:
		bool& returns;


	};
}


#endif
