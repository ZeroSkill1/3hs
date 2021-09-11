
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

