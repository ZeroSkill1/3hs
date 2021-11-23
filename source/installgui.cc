
#include "installgui.hh"

#include <ui/progress_bar.hh>
#include <ui/base.hh>

#include "find_missing.hh"
#include "lumalocale.hh"
#include "settings.hh"
#include "panic.hh"
#include "util.hh"
#include "ctr.hh"

static void make_queue(ui::RenderQueue& queue, ui::ProgressBar **bar)
{
	ui::builder<ui::ProgressBar>(ui::progloc())
		.y(ui::layout::center_y)
		.add_to(bar, queue);
	queue.render_frame();
}

static bool ask_reinstall(bool interactive)
{
	if(!interactive) return false;

	ui::RenderQueue queue;

	bool ret = false;
	ui::builder<ui::Confirm>(ui::Screen::bottom, STRING(already_installed_reinstall), ret)
		.y(ui::layout::center_y)
		.add_to(queue);

	queue.render_finite();
	return ret;
}

static void finalize_install(u64 tid, bool interactive)
{
	// Prompt to ask for extra content
	if(interactive && tid_can_have_missing(tid) && get_settings()->askForExtraContent)
	{
		bool findMissing = true;

		ui::RenderQueue queue;
		ui::builder<ui::Confirm>(ui::Screen::bottom, STRING(extra_content), findMissing)
			.y(ui::layout::center_y)
			.add_to(queue);
		queue.render_finite();

		if(findMissing)
			show_find_missing(tid);
	}

	/* only set locale if we're interactive or if it's set to go automatically */
	if(interactive || get_settings()->lumalocalemode == LumaLocaleMode::automatic)
		luma::set_locale(tid);
	/* this can prompt for a reboot so it should only be done
	 * interactively */
	if(interactive) luma::maybe_set_gamepatching();
}

Result install::gui::net_cia(const std::string& url, u64 tid, bool interactive)
{
	bool focus = set_focus(true);
	ui::ProgressBar *bar;
	ui::RenderQueue queue;

	make_queue(queue, &bar);

	bool shouldReinstall = false;
	Result res = 0;

start_install:
	res = install::net_cia(makeurlwrap(url), tid, [&queue, &bar](u64 now, u64 total) -> void {
			bar->update(now, total);
			bar->activate();
			queue.render_frame();
		}, shouldReinstall);

	if(res == APPERR_NOREINSTALL)
	{
		if((shouldReinstall = ask_reinstall(interactive)))
			goto start_install;
	}

	if(R_FAILED(res))
	{
		error_container err = get_error(res);
		report_error(err, "User was installing from " + url);
		if(interactive) handle_error(err);
	}

	else finalize_install(tid, interactive);

	set_focus(focus);
	return res;
}

Result install::gui::hs_cia(const hsapi::FullTitle& meta, bool interactive)
{
	bool focus = set_focus(true);
	ui::ProgressBar *bar;
	ui::RenderQueue queue;

	make_queue(queue, &bar);

	bool shouldReinstall = false;
	Result res = 0;

start_install:
	res = install::hs_cia(meta, [&queue, &bar](u64 now, u64 total) -> void {
			bar->update(now, total);
			bar->activate();
			queue.render_frame();
		}, shouldReinstall);

	if(res == APPERR_NOREINSTALL)
	{
		if((shouldReinstall = ask_reinstall(interactive)))
			goto start_install;
	}

	if(R_FAILED(res))
	{
		error_container err = get_error(res);
		report_error(err, "User was installing (" + ctr::tid_to_str(meta.tid) + ") (" + std::to_string(meta.id) + ")");
		if(interactive) handle_error(err);
	}

	else finalize_install(meta.tid, interactive);

	set_focus(focus);
	return res;
}

