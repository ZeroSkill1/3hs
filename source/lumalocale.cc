
#include "lumalocale.hh"
#include "settings.hh"
#include "install.hh"
#include "titles.hh"
#include "util.hh"

#include <ui/smdhicon.hh>
#include <ui/selector.hh>
#include <ui/confirm.hh>
#include <ui/base.hh>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


bool has_region(TitleSMDH *smdh, Region region)
{
	// This is a mess
	return
		(smdh->region & (u32) RegionLockout::JPN && region == Region::JPN) ||
		(smdh->region & (u32) RegionLockout::USA && region == Region::USA) ||
		(smdh->region & (u32) RegionLockout::EUR && region == Region::EUR) ||
		(smdh->region & (u32) RegionLockout::AUS && region == Region::EUR) ||
		(smdh->region & (u32) RegionLockout::CHN && region == Region::CHN) ||
		(smdh->region & (u32) RegionLockout::KOR && region == Region::KOR) ||
		(smdh->region & (u32) RegionLockout::TWN && region == Region::TWN);
}

static const char *get_auto_lang_str(TitleSMDH *smdh)
{
	if(smdh->region & (u32) RegionLockout::JPN) return "JP";
	if(smdh->region & (u32) RegionLockout::USA) return "EN";
	if(smdh->region & (u32) RegionLockout::EUR) return "EN";
	if(smdh->region & (u32) RegionLockout::AUS) return "EN";
	if(smdh->region & (u32) RegionLockout::CHN) return "ZH";
	if(smdh->region & (u32) RegionLockout::KOR) return "KR";
	if(smdh->region & (u32) RegionLockout::TWN) return "TW";
	// Fail
	return nullptr;
}

static const char *get_manual_lang_str(TitleSMDH *smdh)
{
	TitleSMDHTitle *title = smdh_get_native_title(smdh);
	bool focus = next::set_focus(true);
	ui::RenderQueue queue;

	u8 lang = 0;
	// EN, JP, FR, DE, IT, ES, ZH, KO, NL, PT, RU, TW
	static const std::vector<std::string> langlut = { "EN", "JP", "FR", "DE", "IT", "ES", "ZH", "KO", "NL", "PT", "RU", "TW" };
	static const std::vector<u8> enumVals = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };

	ui::builder<ui::next::SMDHIcon>(ui::Screen::top, smdh, SMDHIconType::large)
		.x(ui::dimensions::width_top / 2 - 30)
		.y(ui::dimensions::height / 2 - 64)
		.border()
		.add_to(queue);
	ui::builder<ui::next::Text>(ui::Screen::top,
			smdh_u16conv(title->descShort, 0x40) + "\n" +
			smdh_u16conv(title->descLong, 0x80))
		.x(ui::layout::center_x)
		.under(queue.back())
		.wrap()
		.add_to(queue);
	ui::builder<ui::next::Selector<u8>>(ui::Screen::bottom, langlut, enumVals, &lang)
		.add_to(queue);

	queue.render_finite_button(KEY_B);

	next::set_focus(focus);
	return langlut[lang].c_str();
}

static const char *get_region_str(TitleSMDH *smdh)
{
	if(smdh->region & (u32) RegionLockout::JPN) return "JPN";
	if(smdh->region & (u32) RegionLockout::USA) return "USA";
	if(smdh->region & (u32) RegionLockout::EUR) return "EUR";
	if(smdh->region & (u32) RegionLockout::AUS) return "EUR";
	if(smdh->region & (u32) RegionLockout::CHN) return "CHN";
	if(smdh->region & (u32) RegionLockout::KOR) return "KOR";
	if(smdh->region & (u32) RegionLockout::TWN) return "TWN";
	// Fail
	return nullptr;
}

static std::string ensure_path(u64 tid)
{
	// path: /luma/titles/{tid}/locale.txt
	std::string path;
#define ITER(x) do { path += (x); mkdir(path.c_str(), 0777); } while(0)
	ITER("/luma");
	ITER("/titles/");
	ITER(tid_to_str(tid));
#undef ITER
	return path + "/locale.txt";
}

static void write_file(u64 tid, const char *region, const char *lang)
{
	std::string path = ensure_path(tid);
	// locale.txt already exists
	if(access(path.c_str(), F_OK) == 0)
		return;

	std::string contents = std::string(region) + " " + std::string(lang);

	FILE *file = fopen(path.c_str(), "w");
	if(file == nullptr) return;

	fwrite(contents.c_str(), 1, contents.size(), file);
	fclose(file);
}

static bool enable_gamepatching_buf(u8 *buf)
{
	// Luma3DS config format
	// byte 0-3 : "CONF"
	// byte 4-5 : version_major
	// byte 6-7 : version_minor
	// byte ... : config

	// version_major != 2
	if(* (u16 *) &buf[4] != 2)
		return true;

	bool ret = buf[8] & 0x8;
	buf[8] |= 0x8;
	return ret;
}

/* return sif gamepatching was set before */
static bool enable_gamepatching()
{
	FILE *config = fopen("/luma/config.bin", "r");
	if(config == nullptr) return true;

	u8 buf[32];
	if(fread(buf, 1, 32, config) != 32)
	{ fclose(config); return true; }

	fclose(config);
	bool isSet = enable_gamepatching_buf(buf);
	if(isSet) return true;

	// We need to update settings
	config = fopen("/luma/config.bin", "w");
	if(config == nullptr) return true;

	if(fwrite(buf, 1, 32, config) != 32)
	{ fclose(config); return true; }

	fclose(config);
	return false;
}

void luma::set_gamepatching()
{
	if(get_settings()->lumalocalemode == LumaLocaleMode::disabled)
		return;

	// We should prompt for reboot...
	if(!enable_gamepatching())
	{
		ui::RenderQueue queue;
		bool reboot;

		ui::builder<ui::next::Text>(ui::Screen::top, STRING(patching_reboot))
			.x(ui::layout::center_x)
			.y(ui::layout::base)
			.add_to(queue);

		ui::builder<ui::next::Confirm>(ui::Screen::bottom, STRING(reboot_now), reboot)
			.y(80.0f).add_to(queue);

		queue.render_finite();
		if(reboot) NS_RebootSystem();
	}
}

void luma::set_locale(u64 tid)
{
	get_manual_lang_str(smdh_get(tid));

	// we don't want to set a locale
	if(get_settings()->lumalocalemode == LumaLocaleMode::disabled)
		return;

	TitleSMDH *smdh = smdh_get(tid);
	Region region = Region::WORLD;
	const char *langstr = nullptr;
	const char *regstr = nullptr;

	if(smdh == nullptr) return;

	// We don't need to do anything
	if(smdh->region == (u32) RegionLockout::WORLD)
		goto del_smdh;

	u8 sysregion;
	if(R_FAILED(CFGU_SecureInfoGetRegion(&sysregion)))
		goto del_smdh;

	// Convert to Region
	switch(sysregion)
	{
		case CFG_REGION_AUS: case CFG_REGION_EUR: region = Region::EUR; break;
		case CFG_REGION_CHN: region = Region::CHN; break;
		case CFG_REGION_JPN: region = Region::JPN; break;
		case CFG_REGION_KOR: region = Region::KOR; break;
		case CFG_REGION_TWN: region = Region::TWN; break;
		case CFG_REGION_USA: region = Region::USA; break;
		default: goto del_smdh; // invalid region
	}

	// If we have our own region we don't need to do anything
	if(has_region(smdh, region)) goto del_smdh;
	regstr = get_region_str(smdh);

	if(get_settings()->lumalocalemode == LumaLocaleMode::automatic)
		langstr = get_auto_lang_str(smdh);
	else if(get_settings()->lumalocalemode == LumaLocaleMode::manual)
		langstr = get_manual_lang_str(smdh);

	write_file(tid, regstr, langstr);

del_smdh:
	delete smdh;

}

