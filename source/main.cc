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

#include <3ds.h>

#include <ui/list.hh>
#include <ui/base.hh>

#include <widgets/indicators.hh>
#include <widgets/konami.hh>
#include <widgets/meta.hh>

#include <3rd/plog/Appenders/ColorConsoleAppender.h>
#include <3rd/log.hh>

#include "lumalocale.hh"
#include "installgui.hh"
#include "settings.hh"
#include "log_view.hh"
#include "extmeta.hh"
#include "update.hh"
#include "search.hh"
#include "queue.hh"
#include "panic.hh"
#include "about.hh"
#include "proxy.hh"
#include "hsapi.hh"
#include "more.hh"
#include "next.hh"
#include "i18n.hh"
#include "seed.hh"
#include "util.hh"

#ifdef RELEASE
# define LOG_LEVEL plog::info
#else
# define LOG_LEVEL plog::verbose
#endif

#include <ui/base.hh>

#ifndef RELEASE
static plog::ColorConsoleAppender<plog::TxtFormatter> colorAppender(plog::streamStdErr);
#endif

#ifndef RELEASE
class FrameCounter : public ui::BaseWidget
{ UI_WIDGET("FrameCounter")
public:
	float width() override { return this->t->width(); }
	float height() override { return this->t->height(); }
	void set_x(float x) override { this->x = x; this->t->set_x(x); }
	void set_y(float y) override { this->y = y; this->t->set_y(y); }
	void resize(float x, float y) { this->t->resize(x, y); }

	void setup()
	{
		this->t.setup(this->screen, "0 fps");
	}

	bool render(const ui::Keys& k) override
	{
		time_t now = time(NULL);
		if(now != this->frames[this->i].time)
			this->switch_frame(now);
		++this->frames[this->i].frames;
		this->t->render(k);
		return true;
	}

	int fps()
	{
		return this->frames[this->i == 0 ? 1 : 0].frames;
	}

private:
	struct {
		time_t time;
		int frames;
	} frames[2] = {
		{ 0, 60 },
		{ 0, 60 },
	};

	ui::ScopedWidget<ui::Text> t;
	size_t i = 0;

	void set_label(int fps)
	{
		this->t->set_text(std::to_string(fps) + " fps");
		this->t->set_x(this->x);
	}

	void switch_frame(time_t d)
	{
		this->set_label(this->frames[this->i].frames);
		this->i = this->i == 0 ? 1 : 0;
		this->frames[this->i].time = d;
		this->frames[this->i].frames = 0;
	}

};
#endif

static void deinit_all()
{
	hsapi::global_deinit();
	ui::exit();
	exit_services();
}

int main(int argc, char* argv[])
{
	((void) argc);
	((void) argv);

	plog::init(LOG_LEVEL, LOGFILE);
	linfo << "version=" VVERSION;

#ifndef RELEASE
	// Colored logs appender for gdb
	consoleDebugInit(debugDevice_SVC);
	plog::get()->addAppender(&colorAppender);
#endif


	bool isLuma = false;
	panic_if_err_3ds(init_services(isLuma));
	if(!ui::init())
	{
		lfatal << "ui::init() failed, this should **never** happen";
		exit_services();
		exit(0);
	}

#ifdef RELEASE
	// Check if luma is installed
	// 1. Citra is used; not compatible
	// 2. Other cfw used; not supported
	if(!isLuma)
	{
		lfatal << "Luma3DS is not installed, user is using an unsupported CFW or running in Citra";
		ui::RenderQueue queue;

		ui::builder<ui::Text>(ui::Screen::top, STRING(luma_not_installed))
			.x(ui::layout::center_x).y(45.0f)
			.add_to(queue);
		ui::builder<ui::Text>(ui::Screen::top, STRING(install_luma))
			.x(ui::layout::center_x).under(queue.back())
			.add_to(queue);

		queue.render_finite_button(KEY_START | KEY_B);
		ui::exit();
		exit_services();
		exit(0);
	}
#endif

	init_seeddb();
	ensure_settings();
	proxy::init();

	osSetSpeedupEnable(true); // speedup for n3dses

	/* new ui setup */
	ui::builder<ui::Text>(ui::Screen::top, "") /* text is not immediately set */
		.x(ui::layout::center_x)
		.y(4.0f)
		.tag(ui::tag::action)
		.add_to(ui::RenderQueue::global());

	/* buttons */
	ui::builder<ui::Button>(ui::Screen::bottom, ui::SpriteStore::get_by_id(ui::sprite::settings_dark))
		.connect(ui::Button::click, []() -> bool {
			ui::RenderQueue::global()->render_and_then(show_settings);
			return true;
		})
		.connect(ui::Button::nobg)
		.wrap()
		.x(5.0f)
		.y(210.0f)
		.tag(ui::tag::settings)
		.add_to(ui::RenderQueue::global());

	ui::builder<ui::Button>(ui::Screen::bottom, ui::SpriteStore::get_by_id(ui::sprite::more_dark))
		.connect(ui::Button::click, []() -> bool {
			ui::RenderQueue::global()->render_and_then(show_more);
			return true;
		})
		.connect(ui::Button::nobg)
		.wrap()
		.right(ui::RenderQueue::global()->back())
		.y(210.0f)
		.tag(ui::tag::more)
		.add_to(ui::RenderQueue::global());

	ui::builder<ui::Button>(ui::Screen::bottom, ui::SpriteStore::get_by_id(ui::sprite::search_dark))
		.connect(ui::Button::click, []() -> bool {
			ui::RenderQueue::global()->render_and_then(show_search);
			return true;
		})
		.connect(ui::Button::nobg)
		.wrap()
		.right(ui::RenderQueue::global()->back())
		.y(210.0f)
		.tag(ui::tag::search)
		.add_to(ui::RenderQueue::global());

	ui::builder<ui::Button>(ui::Screen::bottom, STRING(queue))
		.connect(ui::Button::click, []() -> bool {
			ui::RenderQueue::global()->render_and_then(show_queue);
			return true;
		})
		.connect(ui::Button::nobg)
		.wrap()
		.right(ui::RenderQueue::global()->back())
		.y(210.0f)
		.tag(ui::tag::queue)
		.add_to(ui::RenderQueue::global());

	ui::builder<ui::KonamiListner>(ui::Screen::top)
		.tag(ui::tag::konami)
		.add_to(ui::RenderQueue::global());

	ui::builder<ui::FreeSpaceIndicator>(ui::Screen::top)
		.tag(ui::tag::free_indicator)
		.add_to(ui::RenderQueue::global());

	ui::builder<ui::TimeIndicator>(ui::Screen::top)
		.add_to(ui::RenderQueue::global());

	ui::builder<ui::BatteryIndicator>(ui::Screen::top)
		.add_to(ui::RenderQueue::global());

#ifndef RELEASE
	ui::builder<FrameCounter>(ui::Screen::top)
		.size(0.4f)
		.x(ui::layout::right)
		.y(25.0f)
		.add_to(ui::RenderQueue::global());
#endif

	ui::builder<ui::NetIndicator>(ui::Screen::top)
		.add_to(ui::RenderQueue::global());


	// DRM Check
#ifdef DEVICE_ID
	u32 devid = 0;
	PS_GetDeviceId(&devid);
	// DRM Check failed
	if(devid != DEVICE_ID)
	{
		lerror << "Piracyception";
		(* (int *) nullptr) = 0xdeadbeef;
	}
#endif
	// end DRM Check

	if(!hsapi::global_init())
	{
		lfatal << "hsapi::global_init() failed";
		panic(STRING(fail_init_networking));
		exit(0);
	}

	atexit(deinit_all);

#ifdef RELEASE
	// If we updated ...
	llog << "Checking for updates";
	if(update_app())
	{
		llog << "Updated from " VERSION;
		exit(0);
	}
#endif

	while(R_FAILED(hsapi::call(hsapi::fetch_index)))
		show_more();

	lverbose << "Done fetching index.";

	size_t catptr = 0, subptr = 0, gamptr = 0;
	const std::string *associatedcat = nullptr;
	const std::string *associatedsub = nullptr;
	std::vector<hsapi::Title> titles;

	// Old logic was cursed, made it a bit better :blobaww:
	while(aptMainLoop())
	{
cat:
		const std::string *cat = next::sel_cat(&catptr);
		// User wants to exit app
		if(cat == next_cat_exit) break;
		llog << "NEXT(c): " << *cat;

sub:
		if(associatedcat != cat) subptr = 0;
		associatedcat = cat;

		const std::string *sub = next::sel_sub(*cat, &subptr);
		if(sub == next_sub_back) goto cat;
		if(sub == next_sub_exit) break;
		llog << "NEXT(s): " << *sub;

		if(associatedsub != sub)
		{
			titles.clear();
			if(R_FAILED(hsapi::call(hsapi::titles_in, titles, *cat, *sub)))
				goto sub;
		}

gam:
		if(associatedsub != sub) gamptr = 0;
		associatedsub = sub;

		hsapi::hid id = next::sel_gam(titles, &gamptr);
		if(id == next_gam_back) goto sub;
		if(id == next_gam_exit) break;

		llog << "NEXT(g): " << id;

		hsapi::FullTitle meta;
		if(show_extmeta_lazy(titles, id, &meta))
			install::gui::hs_cia(meta);

		goto gam;
	}

	llog << "Goodbye, app deinit";
	exit(0);
}

