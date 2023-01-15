/* This file is part of 3hs
 * Copyright (C) 2021-2022 hShop developer team
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

#include <ui/progress_bar.hh>
#include "settings.hh"

#define OUTER_W(screen) (ui::screen_width(screen) - (X_OFFSET * 2))
#define GRAPH_STEP(x) ((ui::screen_width(screen) - ((x) * 2)) / (float) (ui::SpeedBuffer::max_size() - 2))
#define LATENCY_BAR_HEIGHT 50.0f
#define TEXT_DIM 0.65f
#define X_OFFSET 10
#define Y_OFFSET 30
#define Y_LEN 30


UI_CTHEME_GETTER(color_fg, ui::theme::progress_bar_foreground_color)
UI_CTHEME_GETTER(color_bg, ui::theme::progress_bar_background_color)
UI_CTHEME_GETTER(color_text, ui::theme::text_color)
UI_SLOTS(ui::ProgressBar_color, color_text, color_fg, color_bg)

UI_CTHEME_GETTER(color_graph, ui::theme::graph_line_color)
//static u32 color_graph() { return UI_COLOR(00,00,00,FF); }
UI_SLOTS(ui::LatencyGraph_color, color_graph)

std::string ui::up_to_mib_serialize(u64 n, u64 largest)
{
	if(largest < 1024) return std::to_string(n); /* < 1 KiB */
	if(largest < 1024 * 1024)  return ui::floating_prec<float>((float) n / 1024); /* < 1 MiB */
	else return ui::floating_prec<float>((float) n / 1024 / 1024);
}

std::string ui::up_to_mib_postfix(u64 n)
{
	if(n < 1024) return " B"; /* < 1 KiB */
	if(n < 1024 * 1024) return " KiB"; /* < 1 MiB */
	else return " MiB";
}

/* class ProgressBar */

void ui::ProgressBar::setup(u64 part, u64 total)
{
	this->outerw = OUTER_W(this->screen);
	this->buf = C2D_TextBufNew(100); /* probably big enough */
	this->update(part, total);
	this->x = X_OFFSET; /* set a good default */
}

void ui::ProgressBar::setup(u64 total)
{ this->setup(0, total); }

void ui::ProgressBar::setup()
{ this->setup(0, 0); }

void ui::ProgressBar::destroy()
{
	C2D_TextBufDelete(this->buf);
}

bool ui::ProgressBar::render(ui::Keys& keys)
{
	(void) keys;
	/* background rect */
	C2D_DrawRectSolid(this->x, this->y, this->z, this->outerw, Y_LEN, this->slots.get(2));

	// Overlay actual process
	if(this->w != 0)
		C2D_DrawRectSolid(X_OFFSET + 2, this->y + 2, this->z, this->w, Y_LEN - 4, this->slots.get(1));

	if(this->flags & ui::ProgressBar::FLAG_ACTIVE)
	{
		C2D_DrawText(&this->a, C2D_WithColor, X_OFFSET, this->y - Y_LEN + 2,
			this->z, TEXT_DIM, TEXT_DIM, this->slots.get(0));
		C2D_DrawText(&this->bc, C2D_WithColor, this->bcx, this->y - Y_LEN + 2,
			this->z, TEXT_DIM, TEXT_DIM, this->slots.get(0));

		if(this->flags & ui::ProgressBar::FLAG_SHOW_SPEED)
		{
			C2D_DrawText(&this->d, C2D_WithColor, X_OFFSET, this->y + Y_LEN + 2,
				this->z, TEXT_DIM, TEXT_DIM, this->slots.get(0));
			C2D_DrawText(&this->e, C2D_WithColor, this->ex, this->y + Y_LEN + 2,
				this->z, TEXT_DIM, TEXT_DIM, this->slots.get(0));
		}
	}

	return true;
}

static std::string format_duration(time_t secs)
{
	struct tm *tm = gmtime(&secs);
	char ret[10];
	if(tm->tm_hour != 0)
		sprintf(ret, "%02i:%02i:%02i", tm->tm_hour, tm->tm_min, tm->tm_sec);
	else
		sprintf(ret, "%02i:%02i", tm->tm_min, tm->tm_sec);
	return ret;
}

void ui::ProgressBar::update_state()
{
	float perc = this->total == 0 ? 0.0f : ((float) this->part / this->total);
	this->w = (ui::screen_width(this->screen) - (X_OFFSET * 2) - 4) * perc;

	// (a)        (b/c)
	// 90%         9/10
	// [==============]
	// 1MiB/s  1:00 ETA
	// (d)          (e)

	// Parse strings
	std::string bc = this->serialize(this->part, this->total) + "/" + this->serialize(this->total, this->total) + this->postfix(this->total);
	std::string a = floating_prec<float>(perc * 100, 1) + "%";

	C2D_TextBufClear(this->buf);

	ui::parse_text(&this->bc, this->buf, bc.c_str());
	ui::parse_text(&this->a, this->buf, a.c_str());

	C2D_TextOptimize(&this->bc);
	C2D_TextOptimize(&this->a);

	if(this->flags & ui::ProgressBar::FLAG_SHOW_SPEED)
	{
		/* when ~1 second isn't accurate enough */
		u64 now = osGetTime();
		u64 diff = now - this->prevpoll;

		/* take the sample */
		const float actual_bytes_s = (((float) this->part - (float) this->prevpart) / (diff / 1000.0f));
		this->speedDiffs.push(actual_bytes_s);
		/* and average it against the last 30 */
		const float bytes_s = this->speedDiffs.avg();
		const char *format;
		float speed_i;
		/* we can use MiB/s */
		if(bytes_s >= (1024.0f * 1024.0f))
		{
			speed_i = bytes_s / (1024.0f * 1024.0f);
			format = "MiB/s";
		}
		/* if we have less than 1MiB/s speed we fall back to KiB/s */
		else
		{
			speed_i = bytes_s / 1024.0f;
			format = "KiB/s";
		}
		this->prevpart = this->part;
		this->prevpoll = now;

		std::string speed = floating_prec<float>(speed_i) + std::string(format);

		/* if we have no speed yet, we cannot know the ETA, so we just omit it */
		std::string eta = bytes_s ? "ETA " + format_duration((this->total - this->part) / bytes_s) : "";

		ui::parse_text(&this->d, this->buf, speed.c_str());
		ui::parse_text(&this->e, this->buf, eta.c_str());
		C2D_TextOptimize(&this->d);
		C2D_TextOptimize(&this->e);

		C2D_TextGetDimensions(&this->e, TEXT_DIM, TEXT_DIM, &this->ex, nullptr);
		this->ex = ui::screen_width(this->screen) - X_OFFSET - this->ex;
	}

	// Pad to right
	C2D_TextGetDimensions(&this->bc, TEXT_DIM, TEXT_DIM, &this->bcx, nullptr);
	this->bcx = ui::screen_width(this->screen) - X_OFFSET - this->bcx;
}

void ui::ProgressBar::set_postfix(std::function<std::string(u64)> cb)
{ this->postfix = cb; this->flags &= ~ui::ProgressBar::FLAG_SHOW_SPEED; }

void ui::ProgressBar::set_serialize(std::function<std::string(u64, u64)> cb)
{ this->serialize = cb; this->flags &= ~ui::ProgressBar::FLAG_SHOW_SPEED; }

void ui::ProgressBar::activate()
{ this->flags |= ui::ProgressBar::FLAG_ACTIVE; }

void ui::ProgressBar::update(u64 part, u64 total)
{ this->part = part; this->total = total; this->update_state(); }

void ui::ProgressBar::update(u64 part)
{ this->part = part; this->update_state(); }

float ui::ProgressBar::height()
{ return Y_LEN; }

float ui::ProgressBar::width()
{ return this->outerw; }

/* LatencyGraph */

void ui::LatencyGraph::setup(const ui::SpeedBuffer& buffer)
{
	this->buffer = &buffer;
	this->w = OUTER_W(this->screen);
	this->step = GRAPH_STEP(0);
}

void ui::LatencyGraph::set_x(float x)
{
	this->x = x;
	this->step = GRAPH_STEP(x);
}

bool ui::LatencyGraph::render(ui::Keys& keys)
{
	(void) keys;
	if(!this->buffer->size())
		return true; /* no data */

	size_t i = this->buffer->start(), end = this->buffer->end();
	float max, min;
	this->buffer->maxmin(max, min);
	float mult = LATENCY_BAR_HEIGHT / (max - min), avg = this->buffer->avg();
	float x0 = this->x, y0 = this->y - (this->buffer->at(i) - avg) * mult, x1, y1;
	for(i = this->buffer->next(i); i != end; i = this->buffer->next(i))
	{
		x1 = x0 + this->step;
		y1 = this->y - (this->buffer->at(i) - avg) * mult;
		C2D_DrawLine(x0, y0, this->slots[0], x1, y1, this->slots[0], 2.0f, this->z);
		x0 = x1;
		y0 = y1;
	}

	return true;
}

float ui::LatencyGraph::width()
{
	return this->w;
}

float ui::LatencyGraph::height()
{
	return LATENCY_BAR_HEIGHT;
}

