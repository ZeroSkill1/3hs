
#ifndef __ui_image_button_hh__
#define __ui_image_button_hh__

#include "ui/bindings.hh"
#include "ui/sprite.hh"
#include "ui/core.hh"

#include <functional>

#define button_on_click std::function<ui::Results()>


namespace ui
{
	class ImageButton : public Widget
	{
	public:
		ImageButton(std::string sheet_name, int light_idx, int dark_idx, float x1, float y1, float x2, float y2);
		ImageButton(c2d::SpriteSheet sheet, int light_idx, int dark_idx, float x1, float y1, float x2, float y2);
		void init(c2d::SpriteSheet sheet, int light_idx, int dark_idx, float x1, float y1, float x2, float y2);

		void set_on_click(button_on_click cb);
		void toggle_click();

		ui::Results draw(ui::Keys&, ui::Scr) override;

		~ImageButton();


	private:
		c2d::SpriteSheet sheet;
		c2d::Sprite dark;
		c2d::Sprite light;

		button_on_click on_click = [](){ return ui::Results::go_on; };

		bool clickable = true;

		float x1, x2;
		float y1, y2;
	};
}

#endif

