
#include "ui/bindings.hh"


ui::CSprite::CSprite(C2D_Sprite sprite)
{
	this->sprite = sprite;
}


ui::CSprite ui::CSprite::from_sheet(ui::CSpriteSheet *sheet, size_t index)
{
	C2D_Sprite sprite = sheet->_get_sprite(index);
	return ui::CSprite(sprite);
}



bool ui::CSprite::draw()
{
	if(!C2D_DrawSprite(&this->sprite))
		std::exit(1);
	return true;
}

void ui::CSprite::rotate(float radians)
{
	C2D_SpriteRotate(&this->sprite, radians);
}

void ui::CSprite::rotate_degrees(float degrees)
{
	C2D_SpriteRotateDegrees(&this->sprite, degrees);
}

void ui::CSprite::move(float x, float y)
{
	C2D_SpriteMove(&this->sprite, x, y);
}

void ui::CSprite::scale(float x, float y)
{
	C2D_SpriteScale(&this->sprite, x, y);
}

void ui::CSprite::set_center(float x, float y)
{
	C2D_SpriteSetCenter(&this->sprite, x, y);
}

void ui::CSprite::set_center_raw(float x, float y)
{
	C2D_SpriteSetCenterRaw(&this->sprite, x, y);
}

void ui::CSprite::set_depth(float depth)
{
	C2D_SpriteSetDepth(&this->sprite, depth);
}

void ui::CSprite::set_pos(float x, float y)
{
	C2D_SpriteSetPos(&this->sprite, x, y);
}

void ui::CSprite::set_rotation(float radians)
{
	C2D_SpriteSetRotation(&this->sprite, radians);
}

void ui::CSprite::set_rotation_degrees(float degrees)
{
	C2D_SpriteSetRotationDegrees(&this->sprite, degrees);
}


ui::CSpriteSheet::CSpriteSheet(C2D_SpriteSheet sheet)
{
	this->sheet = sheet;
}


ui::CSpriteSheet ui::CSpriteSheet::from_file(std::string name)
{
	return ui::CSpriteSheet(C2D_SpriteSheetLoad(name.c_str()));
}


C2D_Sprite ui::CSpriteSheet::_get_sprite(size_t index)
{
	C2D_Sprite ret;
	C2D_SpriteFromSheet(&ret, this->sheet, index);
	return ret;
}

size_t ui::CSpriteSheet::count()
{
	return C2D_SpriteSheetCount(this->sheet);
}

void ui::CSpriteSheet::free()
{
	C2D_SpriteSheetFree(this->sheet);
}
