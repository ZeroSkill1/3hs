
#include "ui/sprite.hh"


ui::Sprite::Sprite(ui::CSprite sprite)
	: Widget("sprite"), sprite(sprite)
{

}

ui::Results ui::Sprite::draw(ui::Keys&, ui::Scr)
{
	this->sprite.draw();
	return ui::Results::go_on;
}

void ui::Sprite::set_sprite(ui::CSprite sprite)
{
	this->sprite = sprite;
}



ui::StandaloneSprite::StandaloneSprite(std::string name, size_t index)
	: Sprite()
{
	this->sheet = ui::CSpriteSheet::from_file(name);
	this->set_sprite(ui::CSprite::from_sheet(&this->sheet, index));
}

ui::StandaloneSprite::~StandaloneSprite()
{
	this->sheet.free();
}

ui::CSprite *ui::StandaloneSprite::get_sprite()
{
	return &this->sprite;
}


