
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
		Sprite(ui::CSprite sprite);

		ui::Results draw(ui::Keys&, ui::Scr) override;
		void set_sprite(ui::CSprite sprite);


	protected:
		ui::CSprite sprite;


	};

	class StandaloneSprite : public Sprite
	{
	public:
		StandaloneSprite(std::string name, size_t index);
		~StandaloneSprite();

		ui::CSprite *get_sprite();


	private:
		ui::CSpriteSheet sheet;



	};
}

#endif
