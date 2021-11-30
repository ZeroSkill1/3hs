
#include "widgets/indicators.hh"

#include <time.h>
#include <3ds.h>

#include "settings.hh"
#include "ctr.hh"

#define TAG_TWL 1
#define TAG_CTR 2
#define TAG_SD  3

#define TAG_PERC 4
#define TAG_FG   5

#define DIM_X 0.35f
#define DIM_Y 0.35f

void ui::FreeSpaceIndicator::setup()
{
	this->z = 1.0f; /* force on foreground */

	ui::builder<ui::Text>(this->screen)
		.size(DIM_X, DIM_Y)
		.y(ui::screen_height() - 10.0f)
		.tag(TAG_SD)
		.add_to(this->queue);

	ui::builder<ui::Text>(this->screen)
		.size(DIM_X, DIM_Y)
		.y(ui::screen_height() - 10.0f)
		.tag(TAG_TWL)
		.add_to(this->queue);

	ui::builder<ui::Text>(this->screen)
		.size(DIM_X, DIM_Y)
		.y(ui::screen_height() - 10.0f)
		.tag(TAG_CTR)
		.add_to(this->queue);

	this->update();
}

void ui::FreeSpaceIndicator::update()
{
	if(get_settings()->loadFreeSpace)
	{
		u64 nandt = 0, nandc = 0, sdmc = 0;

		ctr::get_free_space(DEST_TWLNand, &nandt);
		ctr::get_free_space(DEST_CTRNand, &nandc);
		ctr::get_free_space(DEST_Sdmc, &sdmc);

		ui::Text *sd = this->queue.find_tag<ui::Text>(TAG_SD);
		ui::Text *twl = this->queue.find_tag<ui::Text>(TAG_TWL);
		ui::Text *ctr = this->queue.find_tag<ui::Text>(TAG_CTR);

		sd->set_text("SD: " + human_readable_size<u64>(sdmc) + " | ");
		sd->set_x(ui::layout::left);

		twl->set_text("TWLNand: " + human_readable_size<u64>(nandt) + " | ");
		twl->set_x(ui::right(sd, twl));

		ctr->set_text("CTRNand: " + human_readable_size<u64>(nandc));
		ctr->set_x(ui::right(twl, ctr));
	}
}

bool ui::FreeSpaceIndicator::render(const ui::Keys& keys)
{
	return get_settings()->loadFreeSpace
		? this->queue.render_screen(keys, this->screen)
		: true;
}

/* TimeIndicator */

void ui::TimeIndicator::setup()
{
	this->text.setup(this->screen);
	this->text->resize(0.4f, 0.4f);
	this->text->set_y(3.0f);
	this->text->set_x(5.0f);

	this->lastCheck = 0;
	this->update();
}

bool ui::TimeIndicator::render(const ui::Keys& keys)
{
	this->update();
	this->text->render(keys);
	return true;
}

void ui::TimeIndicator::update()
{
	time_t now = ::time(nullptr);
	/* accuracy of time() is 1 sec, and our
	 * clock is as well; if now != lastCheck
	 * the diff is 1 sec */
	if(now > this->lastCheck)
	{
		this->text->set_text(ui::TimeIndicator::time(now));
		this->lastCheck = now;
	}
}

std::string ui::TimeIndicator::time(time_t now)
{
	struct tm *tm;
	if((tm = localtime(&now)) == nullptr)
		return "00:00:00";

	// 24h aka good
	if(get_settings()->timeFormat == Timefmt::good)
	{
		constexpr int size = 3 /* hh: */ + 3 /* mm: */ + 2 /* ss */ + 1 /* NULL term */;
		char str[size];

		snprintf(str, size, "%02d:%02d:%02d", tm->tm_hour, tm->tm_min, tm->tm_sec);
		return std::string(str, size);
	}

	// 12h aka american aka bad
	else
	{
		constexpr int size = 3 /* hh: */ + 3 /* mm: */ + 2 /* ss */ + 3 /* " PM"/" AM" */ + 1 /* NULL term */;
		char str[size];

		// Why do i have to write branching code
		// for this shit hour format
		// 24h is so neat but NO we americans:tm: must
		// use an anoying version because ???
		// i get its easier to read for humans
		// (not) but its so annoying to deal with
		// times/dates are annoying in general
		// but fuck this

		// Now we need to use PM
		if(tm->tm_hour > 12 && tm->tm_hour != 24)
		{
			// % 12 is a neat little trick to make it not error
			snprintf(str, size, "%02d:%02d:%02d PM", tm->tm_hour % 12, tm->tm_min, tm->tm_sec);
		}

		// 12:00 (24h) should be 12 PM and not 12 or 0 AM
		else if(tm->tm_hour == 12)
		{
			snprintf(str, size, "12:%02d:%02d PM", tm->tm_min, tm->tm_sec);
		}

		// 00:00 (24h) becomes 12 AM (???)
		else if(tm->tm_hour == 0)
		{
			snprintf(str, size, "12:%02d:%02d AM", tm->tm_min, tm->tm_sec);
		}

		// Now we use AM
		else
		{
			snprintf(str, size, "%02d:%02d:%02d AM", tm->tm_hour, tm->tm_min, tm->tm_sec);
		}

		return std::string(str, size);
	}
}

/* BatteryIndicator */

void ui::BatteryIndicator::setup()
{
	ui::builder<ui::Text>(this->screen)
		.size(0.5f)
		.y(5.0f)
		.tag(TAG_PERC)
		.add_to(this->queue);
	ui::builder<ui::Sprite>(this->screen, ui::SpriteStore::get_by_id(ui::sprite::battery_dark))
		.x(ui::screen_width(ui::Screen::top) - 37.0f)
		.y(5.0f)
		.tag(TAG_FG)
		.add_to(this->queue);
}

void ui::BatteryIndicator::update()
{
	u8 nlvl = 0;

	if(R_FAILED(MCUHWC_GetBatteryLevel(&nlvl)) || this->level == nlvl)
		return;

	this->level = nlvl;
	ui::Text *perc = this->queue.find_tag<ui::Text>(TAG_PERC);
	ui::Sprite *fg = this->queue.find_tag<ui::Sprite>(TAG_FG);

	perc->set_text(std::to_string(this->level) + "%");
	perc->set_x(ui::left(fg, perc));
}

static u8 lvl2batlvl(u8 lvl)
{
	u8 ret = lvl / 25 + 1;
	return ret > 4 ? 4 : ret;
}

bool ui::BatteryIndicator::render(const ui::Keys& keys)
{
#ifdef RELEASE
	if(get_settings()->showBattery)
	{
		this->update();

		float width = lvl2batlvl(this->level) * 5.0f;
		C2D_DrawRectSolid(ui::screen_width(ui::Screen::top) - 13.0f - width, 7.0f, 0.0f,
			width, 12.0f, this->level == 1 ? C2D_Color32f(0xDA, 0x00, 0x00, 0xFF)
			: C2D_Color32f(0x00, 0xA2, 0x00, 0xFF));
		this->queue.render_top(keys);
	}
#else
	/* mcuhwc is not supported in citra */
	((void) lvl2batlvl);
	((void) keys);
#endif

	return true;
}

void ui::NetIndicator::setup()
{
	this->sprite.setup(ui::Screen::top, ui::SpriteStore::get_by_id(ui::sprite::net_discon));
	this->sprite->set_x(ui::screen_width(ui::Screen::top) - 27.0f);
	this->sprite->set_y(ui::screen_height() - 11.0f);

	this->status = -1;
	this->update();
}

bool ui::NetIndicator::render(const ui::Keys& keys)
{
	this->update();
	return this->sprite->render(keys);
}

void ui::NetIndicator::update()
{
	u32 acuStat = 0;
	s8 rstat;

	if(R_SUCCEEDED(ACU_GetWifiStatus(&acuStat)) && acuStat > 0)
		rstat = osGetWifiStrength();
	else rstat = -1;

	if(rstat == this->status) return;
	this->status = rstat;

	switch(this->status)
	{
	case -1: /* disconnected */
		this->sprite->set_sprite(ui::SpriteStore::get_by_id(ui::sprite::net_discon));
		break;
	case 0: /* terrible */
		this->sprite->set_sprite(ui::SpriteStore::get_by_id(ui::sprite::net_0));
		break;
	case 1: /* bad */
		this->sprite->set_sprite(ui::SpriteStore::get_by_id(ui::sprite::net_1));
		break;
	case 2: /* decent */
		this->sprite->set_sprite(ui::SpriteStore::get_by_id(ui::sprite::net_2));
		break;
	case 3: /* good */
		this->sprite->set_sprite(ui::SpriteStore::get_by_id(ui::sprite::net_3));
		break;
	default:
		panic("EINVAL");
	}
}

