
#include "ui/bindings.hh"


c2d::Sprite::Sprite(C2D_Sprite sprite)
{
	this->nhandle = sprite;
}

c2d::Sprite c2d::Sprite::from_sheet(c2d::SpriteSheet *sheet, size_t index)
{
	C2D_Sprite sprite; C2D_SpriteFromSheet(&sprite, *sheet->handle(), index);
	return c2d::Sprite(sprite);
}


bool c2d::Sprite::draw()
{
	return C2D_DrawSprite(&this->nhandle);
}

void c2d::Sprite::rotate(float radians)
{
	C2D_SpriteRotate(&this->nhandle, radians);
}

void c2d::Sprite::rotate_degrees(float degrees)
{
	C2D_SpriteRotateDegrees(&this->nhandle, degrees);
}

void c2d::Sprite::move(float x, float y)
{
	C2D_SpriteMove(&this->nhandle, x, y);
}

void c2d::Sprite::scale(float x, float y)
{
	C2D_SpriteScale(&this->nhandle, x, y);
}

void c2d::Sprite::set_center(float x, float y)
{
	C2D_SpriteSetCenter(&this->nhandle, x, y);
}

void c2d::Sprite::set_center_raw(float x, float y)
{
	C2D_SpriteSetCenterRaw(&this->nhandle, x, y);
}

void c2d::Sprite::set_depth(float depth)
{
	C2D_SpriteSetDepth(&this->nhandle, depth);
}

void c2d::Sprite::set_pos(float x, float y)
{
	C2D_SpriteSetPos(&this->nhandle, x, y);
}

void c2d::Sprite::set_rotation(float radians)
{
	C2D_SpriteSetRotation(&this->nhandle, radians);
}

void c2d::Sprite::set_rotation_degrees(float degrees)
{
	C2D_SpriteSetRotationDegrees(&this->nhandle, degrees);
}


c2d::SpriteSheet::SpriteSheet(C2D_SpriteSheet sheet)
{
	this->nhandle = sheet;
}


c2d::SpriteSheet c2d::SpriteSheet::from_file(std::string name)
{
	return c2d::SpriteSheet(C2D_SpriteSheetLoad(name.c_str()));
}


size_t c2d::SpriteSheet::size()
{
	return C2D_SpriteSheetCount(this->nhandle);
}

void c2d::SpriteSheet::free()
{
	C2D_SpriteSheetFree(this->nhandle);
}


c2d::TextBuf::TextBuf(size_t siz)
{
	this->alloc(siz);
}

c2d::TextBuf::TextBuf()
{
	this->alloc(1);
}

void c2d::TextBuf::alloc(size_t siz)
{
	this->nhandle = C2D_TextBufNew(siz);
}

void c2d::TextBuf::realloc(size_t siz)
{
	this->nhandle = C2D_TextBufResize(this->nhandle, siz);
}

void c2d::TextBuf::free()
{
	C2D_TextBufDelete(this->nhandle);
}

size_t c2d::TextBuf::size()
{
	return C2D_TextBufGetNumGlyphs(this->nhandle);
}

void c2d::TextBuf::clear()
{
	C2D_TextBufClear(this->nhandle);
}

c2d::Text::Text(TextBuf buf, std::string text)
{
	this->parse(buf, text);
}

c2d::Text::Text(TextBuf buf, Font font, std::string text)
{
	this->parse(buf, font, text);
}


void c2d::Text::parse(TextBuf buf, Font font, std::string text)
{
	C2D_TextFontParse(&this->nhandle, *font.handle(), *buf.handle(), text.c_str());
}

void c2d::Text::parse(TextBuf buf, std::string text)
{
	C2D_TextParse(&this->nhandle, *buf.handle(), text.c_str());
}

void c2d::Text::optimize()
{
	C2D_TextOptimize(&this->nhandle);
}

c2d::Dimensions c2d::Text::dimensions(float scalex, float scaley)
{
	c2d::Dimensions ret;
	C2D_TextGetDimensions(&this->nhandle, scalex, scaley,
		&ret.width, &ret.height);
	return ret;
}

void c2d::Text::draw(float x, float y, float z, u32 color, float scalex, float scaley, u32 flags)
{
	C2D_DrawText(&this->nhandle, flags, x, y, z, scalex, scaley, color);
}

void c2d::Text::draw(float x, float y, float z, float scalex, float scaley, u32 flags)
{
	C2D_DrawText(&this->nhandle, flags, x, y , z, scalex, scaley);
}

void c2d::Text::draw(float x, float y, float scalex, float scaley)
{
	C2D_DrawText(&this->nhandle, 0, x, y , 0.0f, scalex, scaley);
}

void c2d::Text::draw(float x, float y, u32 color, float scalex, float scaley)
{
	C2D_DrawText(&this->nhandle, C2D_WithColor, x, y, 0.0f, scalex, scaley, color);
}


c2d::Font::Font(std::string name)
{
	this->nhandle = C2D_FontLoad(name.c_str());
}

void c2d::Font::free()
{
	C2D_FontFree(this->nhandle);
}
