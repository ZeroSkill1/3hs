
#ifndef inc_image_ldr_hh
#define inc_image_ldr_hh

#include <citro2d.h>
#include <citro3d.h>

#include "titles.hh"

#define SMDH_ICON_FORMAT GPU_RGB565

enum class SMDHIconType
{ large, small };

void load_smdh_icon(C2D_Image *ret, const TitleSMDH& smdh, SMDHIconType type,
	unsigned int *chosenDimensions = nullptr);
void delete_smdh_icon(C2D_Image icon);

#endif

