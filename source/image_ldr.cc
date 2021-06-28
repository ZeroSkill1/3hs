
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

void load_smdh_icon(C2D_Image *ret, const TitleSMDH& smdh, SMDHIconType type)
{
	unsigned int dim; // x = y
	u8 *data;

	switch(type)
	{
	case SMDHIconType::large:
		data = (u8 *) smdh.iconLarge;
		dim = 150;
		break;
	case SMDHIconType::small:
		data = (u8 *) smdh.iconSmall;
		dim = 24;
		break;

	default:
		panic("load_smdh_icon(): Invalid SMDHIconType");
		return;
	}

	unsigned int dim2 = next_pow2(dim);

	// subtex
	Tex3DS_SubTexture *subtex = new Tex3DS_SubTexture;
	subtex->right = dim / next_pow2(dim);
	subtex->bottom = 1.0f - subtex->right;
	subtex->width = subtex->height = dim;
	subtex->left = 0.0f;
	subtex->top = 1.0f;

	// tex
	C3D_Tex *tex = new C3D_Tex;
	init_tex(tex, dim2, dim2, GPU_RGB565);

	u16 *dst = (u16 *) tex->data + (dim2 - dim) * dim2;
	u16 *src = (u16 *) data;

	for(size_t i = 0; i < dim / 8; ++i)
	{
		memcpy(dst, src, dim * 8 * sizeof(u16));
		dst += dim2 * 8;
		src += dim  * 8;
	}

	C3D_TexFlush(tex);

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

