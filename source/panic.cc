
#include "panic.hh"

#include <stdlib.h>

#include "ui/core.hh"
#include "ui/text.hh"
#include "ui/util.hh"

#include "hs.hh"


Result init_services()
{
	Result res;

#ifdef RELEASE // Not implmented in citra
	if(R_FAILED(res = mcuHwcInit())) return res;
#endif

	if(R_FAILED(res = romfsInit())) return res;
	if(R_FAILED(res = aptInit())) return res;
	if(R_FAILED(res = fsInit())) return res;
	if(R_FAILED(res = amInit())) return res;
	if(R_FAILED(res = psInit())) return res;

	return res;
}

void exit_services()
{
#ifdef RELEASE
	mcuHwcExit();
#endif

	romfsExit();
	aptExit();
	fsExit();
	amExit();
	psExit();
}

static void push_a_widget(ui::Widgets& wids, float *height = nullptr)
{
	ui::Text *text = new ui::Text(ui::mk_center_WText("Press " GLYPH_A " to continue", SCREEN_HEIGHT() - 30.0f));
	if(height != nullptr) *height = ui::text_height(&text->gtext().ctext) - 3.0f;
	wids.push_back(text);
}

static void push_error_widget(const error_container& err, ui::Widgets& errs, float base = 70.0f)
{
	float height; push_a_widget(errs, &height);

	if(err.type == ErrType_curl)
	{
		errs.push_back(new ui::Text(ui::mk_center_WText(format_err(err.sDesc, err.iDesc),
			base + height)));
	}

	else if(err.type == ErrType_3ds)
	{
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
}

void handle_error(error_container err)
{
	ui::Widgets errs;
	errs.push_back(new ui::PressToContinue(KEY_A));
	push_error_widget(err, errs);

	generic_main_breaking_loop(errs);
}


static void panic_core(std::string caller, ui::Widgets& wids)
{
	wids.push_back(new ui::PressToContinue(KEY_A));
	ui::Text *desc = ui::wid()->get<ui::Text>("curr_action_desc");
	if(desc == nullptr)
		wids.push_back(new ui::Text(ui::mk_center_WText("Fatal Panic Occurred @ " + caller, 45.0f)));
	else desc->replace_text("Fatal Panic Occurred @ " + caller);

	generic_main_breaking_loop(wids);

	// Deinit
	exit_services();
	hs::global_deinit();
	ui::global_deinit();

	// Exit
	exit(1);
}

void panic_impl(std::string caller, std::string msg)
{
	ui::Widgets wids;

	ui::WrapText *text = new ui::WrapText(msg);
	text->set_basey(70.0f);
	text->center();

	wids.push_back(text);
	push_a_widget(wids);

	panic_core(caller, wids);
}

void panic_impl(std::string caller, Result res)
{
	ui::Widgets wids;

	error_container err = get_error(res);
	push_error_widget(err, wids);

	panic_core(caller, wids);
}

void panic_impl(std::string caller)
{
	ui::Widgets wids;

	panic_core(caller, wids);
}

