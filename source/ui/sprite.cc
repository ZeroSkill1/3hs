
#include "ui/sprite.hh"


ui::Sprite::Sprite(c2d::Sprite sprite)
	: Widget("sprite"), sprite(sprite)
{

}

ui::Results ui::Sprite::draw(ui::Keys&, ui::Scr)
{
	this->sprite.draw();
	return ui::Results::go_on;
}

void ui::Sprite::set_sprite(c2d::Sprite sprite)
{
	this->sprite = sprite;
}



ui::StandaloneSprite::StandaloneSprite(std::string name, size_t index)
	: Sprite()
{
	this->sheet = c2d::SpriteSheet::from_file(name);
	this->set_sprite(c2d::Sprite::from_sheet(&this->sheet, index));
}

ui::StandaloneSprite::~StandaloneSprite()
{
	this->sheet.free();
}

c2d::Sprite *ui::StandaloneSprite::get_sprite()
{
	return &this->sprite;
}


