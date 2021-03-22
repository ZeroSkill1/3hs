
#ifndef __ui_bindings_hh__
#define __ui_bindings_hh__

#include <citro3d.h>
#include <citro2d.h>

#include <string>


namespace ui
{
	class CSpriteSheet
	{
	public:
		CSpriteSheet(C2D_SpriteSheet sheet);
		CSpriteSheet() { }

		static CSpriteSheet from_file(std::string name);

		size_t count();
		void free();

		C2D_Sprite _get_sprite(size_t index);

		C2D_SpriteSheet sheet;


	};

	class CSprite
	{
	public:
		CSprite(C2D_Sprite sprite);
		CSprite() { }

		static CSprite from_sheet(CSpriteSheet *sheet, size_t index);

		void set_rotation_degrees(float degrees);
		void set_center_raw(float x, float y);
		void rotate_degrees(float degrees);
		void set_center(float x, float y);
		void set_rotation(float radians);
		void set_pos(float x, float y);
		void scale(float x, float y);
		void move(float x, float y);
		void set_depth(float depth);
		void rotate(float radians);
		bool draw();

		C2D_Sprite sprite;


	};
}

#endif
