
#include <3ds.h>

#include <ui/press_to_continue.hh>
#include <ui/scrollingText.hh>
#include <ui/progress_bar.hh>
#include <ui/confirm.hh>
#include <ui/button.hh>
#include <ui/text.hh>
#include <ui/core.hh>
#include <ui/list.hh>

#include <widgets/konami.hh>
#include <widgets/meta.hh>

#include <net_common.hh>
#include <3rd/log.hh>
#include <hs.hh>

#include "install.hh"
#include "update.hh"
#include "error.hh"
#include "about.hh"
#include "seed.hh"
#include "next.hh"

#ifdef __RELEASE__
# define LOG_LEVEL plog::info
#else
# define LOG_LEVEL plog::verbose
#endif

void init_services()
{
	romfsInit();
	aptInit();
	fsInit();
	amInit();
}

void exit_services()
{
	romfsExit();
	aptExit();
	fsExit();
	amExit();
}

void ensure_logs_dir()
{
	mkdir("/3ds", 0777);
	mkdir("/3ds/3hs", 0777);
}

int main(int argc, char* argv[])
{
	init_services();
	ensure_logs_dir();
	init_seeddb();

	plog::init(LOG_LEVEL, "/3ds/3hs/3hs.log");
	linfo << "App version: " FULL_VERSION;

	if(!ui::global_init())
	{
		lfatal << "ui::global_init() failed, this should **never** happen";
		ui::global_deinit();
		exit_services();
		return 1;
	}

	ui::wid()->push_back("version", new ui::Text(ui::mk_right_WText(VERSION, 3.0f, 5.0f, ui::constants::FSIZE, ui::constants::FSIZE, ui::Scr::bottom)), ui::Scr::bottom);
	ui::wid()->push_back("header_desc", new ui::Text(ui::mk_center_WText("The ultimate 3DS content preservation service.", 30.0f)), ui::Scr::top);
	ui::wid()->push_back("curr_action_desc", new ui::Text(ui::mk_center_WText("Loading ...", 45.0f)), ui::Scr::top);
	ui::wid()->push_back("header", new ui::Text(ui::mk_center_WText("hShop", 0.0f, 1.0f, 1.0f)), ui::Scr::top);
	ui::wid()->push_back("konami", new ui::Konami(), ui::Scr::top);

	ui::wid()->push_back("about", new ui::Button("About", C2D_Color32(0x32, 0x35, 0x36, 0xFF), 10, 210, 80, 230), ui::Scr::bottom);
	ui::wid()->get<ui::Button>("about")->set_on_click([]() -> ui::Results {
		show_about(); return ui::Results::end_early;
	});

	quick_global_draw();



	if(osGetWifiStrength() == 0)
	{
		lwarning << "No wifi found, waiting for wifi";

		ui::wid()->get<ui::Text>("curr_action_desc")->replace_text("Please connect to wifi and restart the app");
		ui::Keys keys; ui::Widgets dummy;
		// 0 = NO wifi at all
		while(ui::framenext(keys) && osGetWifiStrength() == 0)
			ui::framedraw(dummy, keys);
	}

#ifdef __RELEASE__
	// Check if luma is installed
	// 1. Citra is used; not compatible
	// 2. Other cfw used; not supported
	Handle lumaCheck;
	if(R_FAILED(svcConnectToPort(&lumaCheck, "hb:ldr")))
	{
		lfatal << "Luma3DS is not installed, user is using an unsupported CFW or running in Citra";
		ui::wid()->get<ui::Text>("curr_action_desc")->replace_text("Luma3DS is not installed on this system");
		ui::wid()->push_back("msg1", new ui::Text(ui::mk_center_WText("Please install Luma3DS on a real 3DS", 78.0f)), ui::Scr::top);
		standalone_main_breaking_loop();
		ui::global_deinit();
		hs::global_deinit();
		exit_services();
		return 3;
	}
	svcCloseHandle(lumaCheck);
#endif

	if(!hs::global_init())
	{
		lfatal << "hs::global_init() failed";
		ui::wid()->get<ui::Text>("curr_action_desc")
			->replace_text("Failed to init networking");
		standalone_main_loop();
		ui::global_deinit();
		hs::global_deinit();
		exit_services();
		return 2;
	}

#ifdef __RELEASE__
	// If we updated ...
	llog << "Checking for updates";
	if(update_app())
	{
		llog << "Updated from " VERSION;
		hs::global_deinit();
		ui::global_deinit();
		exit_services();
		return 0;
	}
#endif

	hs::Index indx;
	llog << "Fetching index";
	indx = hs::Index::get();

	if(index_failed(indx))
	{
		lfatal << "Failed to fetch index, dns fucked? Server down? " << index_error(indx);
		ui::wid()->get<ui::Text>("curr_action_desc")
			->replace_text(std::string("Couldn't load index: ") + index_error(indx));
		standalone_main_loop();
		ui::global_deinit();
		hs::global_deinit();
		exit_services();
		return 3;
	}

	ui::setup_meta(&indx);


	// Old logic was cursed, made it a bit better :blobaww:
	while(aptMainLoop())
	{
cat:
		std::string cat = next::sel_cat(indx);
		// User wants to exit app
		if(cat == next_cat_exit) break;
		llog << "NEXT(c): " << cat;

sub:
		std::string sub = next::sel_sub(indx, cat);
		if(sub == next_sub_back) goto cat;
		if(sub == next_sub_exit) break;
		llog << "NEXT(s): " << sub;

 gam:
		long int id = next::sel_gam(cat, sub);
		if(id == next_gam_back) goto sub;
		if(id == next_gam_exit) break;
		llog << "NEXT(g): " << id;

		{
			/* INSTALL */
			ui::Widgets wids;
			ui::ProgressBar *bar = new ui::ProgressBar(0, 1); // = 0%
			wids.push_back("prog_bar", bar, ui::Scr::bottom);
			bar->set_mib_type();
			single_draw(wids);

			hs::FullTitle meta = hs::title_meta(id);
			Result res = install_hs_cia(&meta, [&wids, bar](u64 now, u64 total) -> void {
				bar->update(now, total);
				bar->activate_text();
				single_draw(wids);
			});

			// Error!
			if(res != 0)
			{
				ui::Widgets errs;
				errs.push_back(new ui::PressToContinue(KEY_A));
				error_container err = get_error(res);
				report_error(err, "User was installing (" + meta.tid + ") (" + std::to_string(id) + ")");

				constexpr float base = 70.0f;
				ui::Text *text = new ui::Text(ui::mk_center_WText("Press A to continue", SCREEN_HEIGHT() - 20.0f));
				float height = ui::text_height(&text->gtext().ctext) - 3.0f;
				errs.push_back(text);

				if(err.type == ErrType_curl)
				{
					ui::wid()->get<ui::Text>("curr_action_desc")->replace_text("CURL Error");
					errs.push_back(new ui::Text(ui::mk_center_WText(format_err(err.sDesc, err.iDesc),
						base + height)));
				}

				else if(err.type == ErrType_3ds)
				{
					ui::wid()->get<ui::Text>("curr_action_desc")->replace_text("3DS System Error");
					errs.push_back(new ui::Text(ui::mk_center_WText(format_err(err.sDesc, err.iDesc),
						base + height)));
					errs.push_back(new ui::Text(ui::mk_center_WText("Result Code: 0x" + pad8code(err.full),
						base + (height * 2))));
					errs.push_back(new ui::Text(ui::mk_center_WText("Level: " + format_err(err.sLvl, err.iLvl),
						base + (height * 3))));
					errs.push_back(new ui::Text(ui::mk_center_WText("Summary: " + format_err(err.sSum, err.iSum),
						base + (height * 4))));
					errs.push_back(new ui::Text(ui::mk_center_WText("Module: " + format_err(err.sMod, err.iMod),
						base + (height * 5))));
				}

				generic_main_breaking_loop(errs);
			}

			goto gam;
		}
	}


	llog << "Sayonara, app deinit";
	hs::global_deinit();
	ui::global_deinit();
	exit_services();
	return 0;
}
