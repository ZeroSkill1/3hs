
#ifndef inc_ui_bindings_hh
#define inc_ui_bindings_hh

#include <citro3d.h>
#include <citro2d.h>

#include <string>


namespace c2d
{
	typedef struct Dimensions
	{
		float height;
		float width;
	} Dimensions;

	template <typename T>
	class BaseWrap
	{
	public:
		T nhandle;

		T *handle()
		{
			return &this->nhandle;
		}
	};

	class SpriteSheet : public BaseWrap<C2D_SpriteSheet>
	{
	public:
		SpriteSheet(C2D_SpriteSheet sheet);
		SpriteSheet() { }

		static SpriteSheet from_file(std::string name);

		size_t size();
		void free();
	};

	class Sprite : public BaseWrap<C2D_Sprite>
	{
	public:
		Sprite(C2D_Sprite sprite);
		Sprite() { }

		static Sprite from_sheet(SpriteSheet *sheet, size_t index);

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
	};

	class TextBuf : public BaseWrap<C2D_TextBuf>
	{
	public:
		TextBuf(size_t size);
		TextBuf();

		void realloc(size_t siz);
		size_t size();
		void clear();
		void free();


	private:
		void alloc(size_t siz);


	};

	class Font : public BaseWrap<C2D_Font>
	{
	public:
		Font(std::string name);
		void free();
	};

	class Text : public BaseWrap<C2D_Text>
	{
	public:
		Text(TextBuf buf, Font font, std::string text);
		Text(TextBuf buf, std::string text);
		Text() { }

		void parse(TextBuf buf, Font font, std::string text);
		void parse(TextBuf buf, std::string text);
		void optimize();

		void draw(float x, float y, float z, u32 color, float scalex, float scaley, u32 flags = C2D_WithColor);
		void draw(float x, float y, float z, float scalex, float scaley, u32 flags);
		void draw(float x, float y, u32 color, float scalex, float scaley);
		void draw(float x, float y, float scalex, float scaley);

		Dimensions dimensions(float scalex, float scaley);
	};
}

#endif
