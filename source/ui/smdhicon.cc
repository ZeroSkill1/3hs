
#include "ui/smdhicon.hh"

#include "install.hh"
#include "panic.hh"
#include "i18n.hh"


ui::SMDHIcon::SMDHIcon(TitleSMDH *smdh, SMDHIconType type)
	: Widget("smdh_icon")
{
	unsigned int dim;
	load_smdh_icon(&this->img, *smdh, type, &dim);

	this->params.pos.h = this->params.pos.w = dim;
	this->params.center.x = this->params.center.y =
		this->params.depth = this->params.angle =
		this->params.pos.x = this->params.pos.y = 0;
}

ui::SMDHIcon::SMDHIcon(u64 tid, SMDHIconType type)
	: Widget("smdh_icon")
{
	TitleSMDH *smdh = smdh_get(tid);
	if(smdh == nullptr) panic(STRING(fail_load_smdh_icon));

	unsigned int dim;
	load_smdh_icon(&this->img, *smdh, type, &dim);
	delete smdh;

	this->params.pos.h = this->params.pos.w = dim;
	this->params.center.x = this->params.center.y =
		this->params.depth = this->params.angle =
		this->params.pos.x = this->params.pos.y = 0;
}

ui::SMDHIcon::~SMDHIcon()
{
	delete_smdh_icon(this->img);
}

void ui::SMDHIcon::resize(float dim)
{ this->params.pos.w = this->params.pos.h = dim; }

void ui::SMDHIcon::center(float x, float y)
{
	this->params.center.x = x;
	this->params.center.y = y;
}

void ui::SMDHIcon::resize(float w, float h)
{
	this->params.pos.w = w;
	this->params.pos.h = h;
}

void ui::SMDHIcon::move(float x, float y)
{
	this->params.pos.x = x;
	this->params.pos.y = y;
}

ui::Results ui::SMDHIcon::draw(ui::Keys&, ui::Scr)
{
	C2D_DrawImage(this->img, &this->params, nullptr);
	return ui::Results::go_on;
}

