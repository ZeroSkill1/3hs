
#include <ui/smdhicon.hh>

#include "install.hh"
#include "panic.hh"
#include "i18n.hh"


void ui::next::SMDHIcon::setup(ctr::TitleSMDH *smdh, SMDHIconType type)
{
	unsigned int dim;
	load_smdh_icon(&this->img, *smdh, type, &dim);

	this->params.pos.h = this->params.pos.w = dim;
	this->params.center.x = this->params.center.y =
		this->params.depth = this->params.angle =
		this->params.pos.x = this->params.pos.y = 0;
}

void ui::next::SMDHIcon::setup(u64 tid, SMDHIconType type)
{
	ctr::TitleSMDH *smdh = ctr::smdh::get(tid);
	if(smdh == nullptr) panic(STRING(fail_load_smdh_icon));

	unsigned int dim;
	load_smdh_icon(&this->img, *smdh, type, &dim);
	delete smdh;

	this->params.pos.h = this->params.pos.w = dim;
	this->params.depth = this->z;

	this->params.center.x = this->params.center.y =
		this->params.angle = this->params.pos.x = this->params.pos.y = 0;
}

void ui::next::SMDHIcon::destroy()
{
	delete_smdh_icon(this->img);
}

void ui::next::SMDHIcon::set_x(float x)
{
	this->params.pos.x = this->x = ui::transform(this, x);
}

void ui::next::SMDHIcon::set_y(float y)
{
	this->params.pos.y = this->y = ui::transform(this, y);
}

void ui::next::SMDHIcon::set_z(float z)
{
	this->params.depth = this->z = z;
}

void ui::next::SMDHIcon::set_border(bool b)
{
	this->drawBorder = b;
}

float ui::next::SMDHIcon::width()
{
	return this->params.pos.w;
}

float ui::next::SMDHIcon::height()
{
	return this->params.pos.h;
}

bool ui::next::SMDHIcon::render(const ui::Keys& keys)
{
	((void) keys);
	if(this->drawBorder)
	{
		/* TODO: Set border color in theme */
		C2D_DrawRectSolid(this->x - 1.0f, this->y - 1.0f, 0.0f,
			this->params.pos.w + 2.0f, this->params.pos.h + 2.0f,
			C2D_Color32(0x00, 0x00, 0x00, 0xFF));
	}
	C2D_DrawImage(this->img, &this->params, nullptr);
	return true;
}

