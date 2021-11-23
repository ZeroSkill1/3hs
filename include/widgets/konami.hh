
#ifndef inc_ui_konami_hh
#define inc_ui_konami_hh

#if 0
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
		size_t currKey = 0;


	};
}
#endif

#endif
