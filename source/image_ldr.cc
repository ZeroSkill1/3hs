
// To load C2D_Image's

#include "image_ldr.hh"

#include <citro3d.h>
#include <citro2d.h>
#include <3ds.h>

#include "panic.hh"


#define U16_MIN_64(i) ((u16) (next_pow2(i) < 64 ? 64 : next_pow2(i)))

static u32 next_pow2(u32 i)
{
	i--;
	i |= i >> 1;
	i |= i >> 2;
	i |= i >> 4;
	i |= i >> 8;
	i |= i >> 16;
	i++;

	return i;
}

static void init_tex(C3D_Tex *tex, u32 width, u32 height, GPU_TEXCOLOR format)
{
	if(!C3D_TexInit(tex, width, height, format)) panic("Failed to create tex");
	C3D_TexSetFilter(tex, GPU_NEAREST, GPU_NEAREST);
}

void load_tiled_image(C2D_Image *ret, void *data, u32 dataSize, u32 width, u32 height, GPU_TEXCOLOR format)
{
	u32 pwidth = U16_MIN_64(width);
	u32 pheight = U16_MIN_64(height);

	ret->tex = (C3D_Tex *) linearAlloc(sizeof(C3D_Tex));
	init_tex(ret->tex, pwidth, pheight, format);

	// We need to convert
	if(width != pwidth || height != pheight)
	{
		u32 pixelSize = dataSize / width / height;

		memset(ret->tex->data, 0x0, ret->tex->size);
		for(u32 y = 0; y < height; y += 8)
		{
			u32 dstPos = y * pwidth * pixelSize;
			u32 srcPos = y * width * pixelSize;

			memcpy(&((u8 *) ret->tex->data)[dstPos], &((u8 *) data)[srcPos], width * 8 * pixelSize);
		}
	}

	// Its good already
	else
	{
		memcpy(ret->tex->data, data, ret->tex->size);
	}

	C3D_TexFlush(ret->tex);

	// Load subtex

	Tex3DS_SubTexture *subtex = (Tex3DS_SubTexture *) linearAlloc(sizeof(Tex3DS_SubTexture));
	subtex->width = (u16) width;
	subtex->height = (u16) height;
	subtex->left = 0.0f;
	subtex->top = 1.0f;
	subtex->right = (width / (float) pwidth);
	subtex->bottom = 1.0f - (height / (float) pheight);

	ret->subtex = subtex;
}

void fabricated_image_free(C2D_Image image)
{
	linearFree((void *) image.subtex);
	C3D_TexDelete(image.tex);
}

void load_smdh_icon(C2D_Image *ret, const TitleSMDH& smdh, SMDHIconType type)
{
	unsigned int dim; // x = y
	u8 *src;

	switch(type)
	{
	case SMDHIconType::large:
		src = (u8 *) smdh.iconLarge;
		dim = 48;
		break;
	case SMDHIconType::small:
		src = (u8 *) smdh.iconSmall;
		dim = 24;
		break;

	default:
		panic("load_smdh_icon(): Invalid SMDHIconType");
		return;
	}

	// subtex
	Tex3DS_SubTexture *subtex = new Tex3DS_SubTexture;
	subtex->right = subtex->top = dim / next_pow2(dim);
	subtex->width = subtex->height = dim;
	subtex->bottom = subtex->left = 0.0f;

	// tex
	C3D_Tex *tex = new C3D_Tex;
	C3D_TexInit(tex, dim, dim, GPU_RGB565);

//	u16 *dst = ((u16 *) tex->data) + (next_pow2(dim) - dim) * next_pow2(dim);
	for(size_t i = 0; i < dim; i += 8)
	{
		u32 dstPos = i * next_pow2(dim) * sizeof(u16);
		u32 srcPos = i * dim * sizeof(u16);

		memcpy((u16 *) ret->tex->data + dstPos, src + srcPos, dim * 8 * sizeof(u16));
//		memcpy(dst, src, dim * 8 * sizeof(u16));
	}

	// fill in data
	ret->subtex = subtex;
	ret->tex = tex;
}

void delete_smdh_icon(C2D_Image icon)
{
	C3D_TexDelete(icon.tex);
	delete icon.subtex;
	delete icon.tex;
}

