
#ifndef __ui_sprite_hh__
#define __ui_sprite_hh__

#include "ui/bindings.hh"
#include "ui/core.hh"

#include <string>


namespace ui
{
	class Sprite : public ui::Widget
	{
	public:
		Sprite() : ui::Widget("sprite") { }
		Sprite(c2d::Sprite sprite);

		ui::Results draw(ui::Keys&, ui::Scr) override;
		void set_sprite(c2d::Sprite sprite);


	protected:
		c2d::Sprite sprite;


	};

	class StandaloneSprite : public Sprite
	{
	public:
		StandaloneSprite(std::string name, size_t index);
		~StandaloneSprite();

		c2d::Sprite *get_sprite();


	private:
		c2d::SpriteSheet sheet;



	};
}

#endif
