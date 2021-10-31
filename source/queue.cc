
#include "queue.hh"

#include <widgets/indicators.hh>

#include <ui/progress_bar.hh>
#include <ui/confirm.hh>
#include <ui/button.hh>
#include <ui/list.hh>
#include <ui/util.hh>
#include <ui/text.hh>
#include <ui/core.hh>

#include <widgets/meta.hh>

#include <vector>

#include "find_missing.hh"
#include "lumalocale.hh"
#include "install.hh"
#include "panic.hh"
#include "error.hh"
#include "i18n.hh"
#include "util.hh"

static std::vector<hsapi::FullTitle> g_queue;
std::vector<hsapi::FullTitle>& queue_get() { return g_queue; }

void queue_add(const hsapi::FullTitle& meta)
{
	g_queue.push_back(meta);
}

void queue_add(hsapi::hid id, bool disp)
{
	hsapi::FullTitle meta;
	Result res = disp ? hsapi::call(hsapi::title_meta, meta, std::move(id))
		: hsapi::scall(hsapi::title_meta, meta, std::move(id));
	if(R_FAILED(res)) return;
	queue_add(meta);
}

void queue_remove(size_t index)
{
	g_queue.erase(g_queue.begin() + index);
}

void queue_clear()
{
	g_queue.clear();
}

void queue_process(size_t index)
{
	if(g_queue.size() < index) return;
	toggle_focus();
	process_hs(g_queue[index]);
	toggle_focus();
	queue_remove(index);
}

void queue_process_all()
{
	toggle_focus();
	for(hsapi::FullTitle& meta : g_queue)
	{
		if(R_FAILED(process_hs(meta, false)))
			break;
	}
	luma::set_gamepatching();
	toggle_focus();
	queue_clear();
}

Result process_uri(const std::string& uri, bool reinstallable, hsapi::htid tid, FS_MediaType media)
{
	ui::Widgets wids;
	ui::ProgressBar *bar = new ui::ProgressBar(0, 1); // = 0%
	wids.push_back("prog_bar", bar, PROGBAR_LOCATION(ui::Scr::bottom));
	bar->set_mib_type();
	single_draw(wids);

	Result res = install_net_cia(makeurlwrap(uri), [&wids, bar](u64 now, u64 total) -> void {
		bar->update(now, total);
		bar->activate_text();
		single_draw(wids);
	}, reinstallable, tid, media);

	if(res == APPERR_NOREINSTALL)
	{
		bool userWantsReinstall = false;

		ui::Widgets rei;
		rei.push_back(new ui::Confirm(STRING(already_installed_reinstall),
			userWantsReinstall), ui::Scr::bottom);
		generic_main_breaking_loop(rei);

		if(userWantsReinstall)
			return process_uri(uri, true, tid, media);
	}

	if(R_FAILED(res))
	{
		error_container err = get_error(res);
		report_error(err, "User was installing from " + uri);
		handle_error(err);
	}

	ui::wid()->get<ui::FreeSpaceIndicator>("size_indicator")->update();
	return res;
}

Result process_hs(hsapi::hid id)
{
	hsapi::FullTitle meta;
	Result res = hsapi::call(hsapi::title_meta, meta, std::move(id));
	if(R_FAILED(res)) return 0; // user was already warned
	return process_hs(meta);
}

Result process_hs(const hsapi::FullTitle& meta, bool interactive, bool reinstall)
{
	ui::Widgets wids;
	ui::ProgressBar *bar = new ui::ProgressBar(0, 1); // = 0%
	wids.push_back("prog_bar", bar, PROGBAR_LOCATION(ui::Scr::bottom));
	bar->set_mib_type();
	single_draw(wids);

	Result res = install_hs_cia(meta, [&wids, bar](u64 now, u64 total) -> void {
		bar->update(now, total);
		bar->activate_text();
		single_draw(wids);
	}, reinstall);

	if(res == APPERR_NOREINSTALL)
	{
		bool userWantsReinstall = false;

		ui::Widgets rei;
		rei.push_back(new ui::Confirm(STRING(already_installed_reinstall),
			userWantsReinstall), ui::Scr::bottom);
		generic_main_breaking_loop(rei);

		if(userWantsReinstall)
			return process_hs(meta, interactive, true);
	}

	// Error!
	if(R_FAILED(res))
	{
		error_container err = get_error(res);
		report_error(err, "User was installing (" + tid_to_str(meta.tid) + ") (" + std::to_string(meta.id) + ")");
		handle_error(err);
	}

	// Prompt to ask for extra content
	else if(interactive && tid_can_have_missing(meta.tid) && get_settings()->askForExtraContent)
	{
		bool findMissing = true;

		ui::Widgets ext;
		ext.push_back(new ui::Confirm(STRING(extra_content), findMissing), ui::Scr::bottom);
		generic_main_breaking_loop(ext);

		if(findMissing)
			show_find_missing(meta.tid);
	}

	ui::wid()->get<ui::FreeSpaceIndicator>("size_indicator")->update();
	return res;
}

static void queue_is_empty(bool toggle = true)
{
	ui::Widgets wids;

	ui::WrapText *msg = new ui::WrapText(STRING(queue_empty));
	msg->center(); msg->set_basey((SCREEN_HEIGHT() / 2) - 30);
	wids.push_back(msg);

	wids.push_back(new ui::PressToContinue(KEY_A));
	generic_main_breaking_loop(wids);

	if(toggle) toggle_focus();
}

void show_queue()
{
	toggle_focus();

	// Queue is empty :craig:
	if(g_queue.size() == 0)
		return queue_is_empty();

	ui::Widgets wids;
	ui::List<hsapi::FullTitle> *list = new ui::List<hsapi::FullTitle>(
		[](hsapi::FullTitle& meta) -> std::string { return meta.name; },
		[](ui::List<hsapi::FullTitle> *self, size_t index, u32 keys) -> ui::Results {
			if(keys & KEY_X)
			{
				if(self->out_of_bounds(index))
					return ui::Results::go_on;
				queue_remove(index);
				self->update_text_reg();
				if(g_queue.size() != 0 && self->get_pointer() >= g_queue.size())
					self->set_pointer(g_queue.size() - 1);
			}
			return ui::Results::go_on;
		}, g_queue
	); wids.push_back("list", list);
	list->add_button(KEY_X);

	ui::TitleMeta *meta = new ui::TitleMeta();
	if(g_queue.size() > 0) meta->update_title(g_queue[0]);

	wids.push_back("meta", meta, ui::Scr::bottom);
	list->set_on_change([&](ui::List<hsapi::FullTitle> *self, size_t index) {
		if(self->out_of_bounds(index)) return;
		meta->update_title(self->at(index));
	});


	wids.push_back("back", new ui::Button(STRING(back), 240, 210, 310, 230), ui::Scr::bottom);
	wids.get<ui::Button>("back")->set_on_click([](bool) -> ui::Results {
		return ui::Results::quit_loop;
	});

	wids.push_back("install_all", new ui::Button("Install All", 10, 180, 100, 200), ui::Scr::bottom);
	wids.get<ui::Button>("install_all")->set_on_click([list](bool) -> ui::Results {
		ui::end_frame();
		queue_process_all();
		list->update_text_reg();
		queue_is_empty(false);
		return ui::Results::quit_no_end;
	});

	ui::Button *installSel = new ui::Button("Install Selected", 10, 210, 150, 230);
	wids.push_back("install_sel", installSel, ui::Scr::bottom);

	installSel->set_on_click([list](bool) -> ui::Results {
		if(list->out_of_bounds(list->get_pointer())) return ui::Results::go_on;
		ui::end_frame();
		queue_process(list->get_pointer());
		list->update_text_reg();
		if(g_queue.size() == 0)
		{
			queue_is_empty(false);
			return ui::Results::quit_no_end;
		}
		list->set_pointer(list->get_pointer() - 1);
		return ui::Results::end_early;
	}); installSel->toggle_click();

	wids.push_back(new ui::DoAfterFrames(20, [installSel]() -> ui::Results {
		installSel->toggle_click(); return ui::Results::go_on;
	}));

	generic_main_breaking_loop(wids);
	toggle_focus();
}

