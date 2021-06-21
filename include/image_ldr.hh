
#ifndef inc_image_ldr_hh
#define inc_image_ldr_hh

#include <citro2d.h>
#include <citro3d.h>

#include "titles.hh"

#define SMDH_ICON_FORMAT GPU_RGB565

enum class SMDHIconType
{ large, small };

void load_tiled_image(C2D_Image *ret, void *data, u32 dataSize, u32 width, u32 height, GPU_TEXCOLOR format = GPU_RGBA8);
void load_smdh_icon(C2D_Image *ret, const TitleSMDH& smdh, SMDHIconType type);
void delete_smdh_icon(C2D_Image icon);
void fabricated_image_free(C2D_Image);

#endif

