
#ifndef inc_ui_loading_hh
#define inc_ui_loading_hh

#include <ui/sprite.hh>
#include <ui/core.hh>

#include <functional>


namespace ui
{
	/* draw `wids` while running `callback` */
	void loading(ui::Widgets& wids, std::function<void()> callback);
	/* run a loading animation (class Spinner) while running `callback` */
	void loading(std::function<void()> callback);

	class Spinner : public Widget
	{
	public:
		Spinner();

		ui::Results draw(ui::Keys&, ui::Scr) override;


	private:
		ui::StandaloneSprite sprite;


	};
}

#endif

