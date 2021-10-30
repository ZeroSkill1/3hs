
#include "build/spinner.h"
#include "ui/loading.hh"
#include "thread.hh"
#include "util.hh"
#include "i18n.hh"


void ui::loading(ui::Widgets& wids, std::function<void()> callback)
{
	std::string old = swap_desc(STRING(loading));

	thread<> th(callback);

	ui::Keys keys;
	while(!th.finished() && ui::framenext_nobreak(keys))
		ui::framedraw(wids, keys);

	swap_desc(old);
}

void ui::loading(std::function<void()> callback)
{
	ui::Widgets wids;
	wids.push_back(new ui::Spinner());
	ui::loading(wids, callback);
}


ui::Spinner::Spinner() : Widget("spinner"),
	sprite(SHEET("spinner"), spinner_spinner_idx)
{
	this->sprite.get_sprite()->set_pos(SCREEN_WIDTH(ui::Scr::top) / 2,
		SCREEN_HEIGHT() / 2);
	this->sprite.get_sprite()->set_center(.5f, .5f);
}

ui::Results ui::Spinner::draw(ui::Keys& keys, ui::Scr screen)
{
	this->sprite.get_sprite()->rotate_degrees(1.0f);
	this->sprite.draw(keys, screen);
	return ui::Results::go_on;
}


void ui::next::loading(ui::RenderQueue& queue, std::function<void()> callback)
{
	std::string desc = ::next::set_desc(STRING(loading));
	bool focus = ::next::set_focus(true);

	thread<> th(callback);

	ui::Keys keys = ui::RenderQueue::get_keys();
	while(!th.finished() && queue.render_frame(keys))
		keys = ui::RenderQueue::get_keys();

	::next::set_focus(focus);
	::next::set_desc(desc);
}

void ui::next::loading(std::function<void()> callback)
{
	RenderQueue queue;

	ui::builder<ui::next::Spinner>(ui::Screen::top)
		.x(ui::layout::center_x)
		.y(ui::layout::center_y)
		.add_to(queue);

	ui::next::loading(queue, callback);
}

/* class Spinner */

void ui::next::Spinner::setup()
{
	this->sprite.setup(this->screen, ui::SpriteStore::get_by_id(ui::sprite::spinner));
	this->sprite.ptr()->set_center(0.5f, 0.5f);
}

bool ui::next::Spinner::render(const ui::Keys& keys)
{
	this->sprite.ptr()->rotate(1.0f);
	return this->sprite.render(keys);
}

float ui::next::Spinner::width()
{ return this->sprite.ptr()->width(); }

float ui::next::Spinner::height()
{ return this->sprite.ptr()->height(); }

void ui::next::Spinner::set_x(float x)
{
	if((int) x == ui::layout::center_x)
	{
		// We need to do some extra maths here because we have a (0.5, 0.5) center
		x = ui::screen_width(this->screen) / 2;
	} else x = ui::transform(this, x);

	this->sprite.ptr()->set_x(x);
	this->x = x;
}

void ui::next::Spinner::set_y(float y)
{
	if((int) y == ui::layout::center_y)
	{
		// We need to do some extra maths here because we have a (0.5, 0.5) center
		y = ui::screen_height() / 2;
	} else y = ui::transform(this, y);

	this->sprite.ptr()->set_y(y);
	this->y = y;
}

void ui::next::Spinner::set_z(float z)
{ this->sprite.ptr()->set_z(z); }

