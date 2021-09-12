
#include "lumalocale.hh"
#include "settings.hh"
#include "install.hh"
#include "titles.hh"

#include <ui/smdhicon.hh>
#include <ui/selector.hh>
#include <ui/confirm.hh>
#include <ui/text.hh>
#include <ui/core.hh>

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
	ui::Widgets wids;

	ui::SMDHIcon *icon = new ui::SMDHIcon(smdh, SMDHIconType::large);
	icon->move(SCREEN_WIDTH(ui::Scr::top) / 2 - 30, SCREEN_HEIGHT() / 2 - 64);

	TitleSMDHTitle *title = smdh_get_native_title(smdh);
	ui::WrapText *label = new ui::WrapText(
		smdh_u16conv(title->descShort, 0x40) + "\n" +
		smdh_u16conv(title->descLong, 0x80)
	);

	label->set_basey(SCREEN_HEIGHT() / 2 + 10);
	label->autowrap();
	label->center();

	u8 lang = 0;
	// EN, JP, FR, DE, IT, ES, ZH, KO, NL, PT, RU, TW
	static const char *langlut[] = { "EN", "JP", "FR", "DE", "IT", "ES", "ZH", "KO", "NL", "PT", "RU", "TW" };
	ui::Selector<u8> *selector = new ui::Selector<u8>(
		{ langlut[0], langlut[1], langlut[2], langlut[3], langlut[4], langlut[5], langlut[6], langlut[7], langlut[8], langlut[9], langlut[10], langlut[11] },
		{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 },
		&lang
	);

	wids.push_back(selector, ui::Scr::bottom);
	wids.push_back(label);
	wids.push_back(icon);

	generic_main_breaking_loop(wids);
	return langlut[lang];
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
		ui::Widgets wids;

		ui::WrapText *rebootprompt = new ui::WrapText(STRING(patching_reboot));
		rebootprompt->set_basey(70.0f);
		rebootprompt->center();

		bool shouldReboot = true;
		ui::Confirm *prompt = new ui::Confirm(STRING(reboot_now), shouldReboot);

		wids.push_back(rebootprompt);
		wids.push_back(prompt);

		generic_main_breaking_loop(wids);

		if(shouldReboot) NS_RebootSystem();
	}
}

void luma::set_locale(u64 tid)
{
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

