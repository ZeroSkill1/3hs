
#ifndef __ui_konami_hh__
#define __ui_konami_hh__

#include "ui/bindings.hh"
#include "ui/sprite.hh"
#include "ui/core.hh"


namespace ui
{


	class Konami : public ui::Widget
	{
	public:
		Konami();

		ui::Results draw(ui::Keys&, ui::Scr) override;
		void show_bunny();


	private:
		size_t currKey;


	};
}

#endif
