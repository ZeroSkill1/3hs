/*
 * Copyright (C) 2021-2022 MyPasswordIsWeak
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include "ui/loading.hh"
#include "thread.hh"
#include "util.hh"
#include "i18n.hh"


void ui::loading(ui::RenderQueue& queue, std::function<void()> callback)
{
	std::string desc = ::set_desc(STRING(loading));
	bool focus = ::set_focus(true);

	ctr::thread<> th(callback);

	ui::Keys keys = ui::RenderQueue::get_keys();
	while(!th.finished() && queue.render_frame(keys))
		keys = ui::RenderQueue::get_keys();

	::set_focus(focus);
	::set_desc(desc);
}

void ui::loading(std::function<void()> callback)
{
	RenderQueue queue;

	ui::builder<ui::Spinner>(ui::Screen::top)
		.x(ui::layout::center_x)
		.y(ui::layout::center_y)
		.add_to(queue);

	ui::loading(queue, callback);
}

/* class Spinner */

void ui::Spinner::setup()
{
	this->sprite.setup(this->screen, ui::SpriteStore::get_by_id(ui::sprite::spinner));
	this->sprite.ptr()->set_center(0.5f, 0.5f);
}

bool ui::Spinner::render(const ui::Keys& keys)
{
	this->sprite.ptr()->rotate(1.0f);
	return this->sprite->render(keys);
}

float ui::Spinner::width()
{ return this->sprite.ptr()->width(); }

float ui::Spinner::height()
{ return this->sprite.ptr()->height(); }

void ui::Spinner::set_x(float x)
{
	if((int) x == ui::layout::center_x)
	{
		// We need to do some extra maths here because we have a (0.5, 0.5) center
		x = ui::screen_width(this->screen) / 2;
	} else x = ui::transform(this, x);

	this->sprite.ptr()->set_x(x);
	this->x = x;
}

void ui::Spinner::set_y(float y)
{
	if((int) y == ui::layout::center_y)
	{
		// We need to do some extra maths here because we have a (0.5, 0.5) center
		y = ui::screen_height() / 2;
	} else y = ui::transform(this, y);

	this->sprite.ptr()->set_y(y);
	this->y = y;
}

void ui::Spinner::set_z(float z)
{ this->sprite.ptr()->set_z(z); }

void ui::detail::TimeoutScreenHelper::setup(const std::string& fmt, size_t nsecs, bool *shouldStop)
{
	this->fmt = fmt;
	this->startTime = this->lastCheck = time(NULL);
	this->shouldStop = shouldStop;
	this->nsecs = nsecs;

	this->text.setup(ui::Screen::top, "");
	this->text.ptr()->set_x(ui::layout::center_x);
	this->text.ptr()->set_y(80.0f);

	this->update_text(this->startTime);
}

void ui::detail::TimeoutScreenHelper::update_text(time_t now)
{
	std::string ntxt;
	ntxt.reserve(this->fmt.size());

	for(size_t i = 0; i < this->fmt.size(); ++i)
	{
		switch(this->fmt[i])
		{
		case '$':
			++i;
			if(this->fmt[i] == 't')
				ntxt += std::to_string(this->nsecs - (now - this->startTime));
			else ntxt.push_back(this->fmt[i]);
			break;

		default:
			ntxt.push_back(this->fmt[i]);
			break;

		}
	}

	this->text.ptr()->set_text(ntxt);
}

bool ui::detail::TimeoutScreenHelper::render(const ui::Keys& keys)
{
	if(keys.kDown & KEY_START && this->shouldStop != nullptr)
	{
		*this->shouldStop = true;
		return false;
	}

	time_t now = time(NULL);
	if(this->lastCheck != now)
	{
		this->update_text(now);
		this->lastCheck = now;
	}

	if(now - this->startTime >= this->nsecs)
		return false;

	return this->text->render(keys);
}

// timeoutscreen()

bool ui::timeoutscreen(const std::string& fmt, size_t nsecs, bool allowCancel)
{
	ui::RenderQueue queue;
	bool ret = false;

	ui::builder<ui::detail::TimeoutScreenHelper>(ui::Screen::top, fmt, nsecs, allowCancel ? &ret : nullptr)
		.add_to(queue);

	queue.render_finite();
	return ret;
}

