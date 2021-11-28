
// To load C2D_Image's

#include "image_ldr.hh"
#include "panic.hh"
#include "i18n.hh"

#include <citro3d.h>
#include <citro2d.h>
#include <3ds.h>

#define SMDH_ICON_FORMAT GPU_RGB565


static u32 next_pow2(u32 i)
{
	--i;
	i |= i >> 1;
	i |= i >> 2;
	i |= i >> 4;
	i |= i >> 8;
	i |= i >> 16;
	++i;

	return i;
}

void load_smdh_icon(C2D_Image *ret, const ctr::TitleSMDH& smdh, SMDHIconType type,
	unsigned int *chosenDimensions)
{
	unsigned int dim; // x = y
	u16 *src;

	switch(type)
	{
	case SMDHIconType::large:
		src = (u16 *) smdh.iconLarge;
		dim = 48;
		break;
	case SMDHIconType::small:
		src = (u16 *) smdh.iconSmall;
		dim = 24;
		break;

	default:
		panic(STRING(fail_load_smdh_icon));
		return;
	}

	unsigned int dim2 = next_pow2(dim);
	if(chosenDimensions != nullptr)
		*chosenDimensions = dim2;

	// subtex
	Tex3DS_SubTexture *subtex = new Tex3DS_SubTexture;
	subtex->width = subtex->height = dim;
	subtex->right = (float) dim / dim2;
	subtex->bottom = subtex->left = 0;
	subtex->top = (float) dim / dim2;

	// tex
	C3D_Tex *tex = new C3D_Tex;
	if(!C3D_TexInit(tex, dim2, dim2, GPU_RGB565))
		panic(STRING(fail_create_tex));

	u16 *dst = (u16 *) tex->data + (dim2 - dim) * dim2;

	for(size_t i = 0; i < dim; i += 8)
	{
		memcpy(dst, src, dim * 8 * sizeof(u16));
		dst += dim2 * 8;
		src += dim * 8;
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

