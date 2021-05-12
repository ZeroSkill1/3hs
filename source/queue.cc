
#include "queue.hh"

#include <widgets/indicators.hh>

#include <ui/press_to_continue.hh>
#include <ui/do_after_frames.hh>
#include <ui/progress_bar.hh>
#include <ui/button.hh>
#include <ui/list.hh>
#include <ui/text.hh>
#include <ui/core.hh>

#include <widgets/meta.hh>

#include <vector>

#include "install.hh"
#include "error.hh"

static std::vector<hs::FullTitle> g_queue;
std::vector<hs::FullTitle> *queue()
{ return &g_queue; }


void queue_add(hs::FullTitle meta)
{
	g_queue.push_back(meta);
}

void queue_add(long int id)
{
	queue_add(hs::title_meta(id));
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
	process_hs(g_queue[index]);
	queue_remove(index);
}

void queue_process_all()
{
	for(hs::FullTitle& meta : g_queue)
	{
		if(!NET_OK(process_hs(meta)))
			break;
	}
	queue_clear();
}

Result process_hs(long int id)
{ return process_hs(hs::title_meta(id)); }

Result process_hs(hs::FullTitle meta)
{
	ui::Widgets wids;
	ui::ProgressBar *bar = new ui::ProgressBar(0, 1); // = 0%
	wids.push_back("prog_bar", bar, ui::Scr::bottom);
	bar->set_mib_type();
	single_draw(wids);

	Result res = install_hs_cia(&meta, [&wids, bar](u64 now, u64 total) -> void {
		bar->update(now, total);
		bar->activate_text();
		single_draw(wids);
	});
// Error!
	if(!NET_OK(res))
	{
		ui::Widgets errs;
		errs.push_back(new ui::PressToContinue(KEY_A));
		error_container err = get_error(res);
		report_error(err, "User was installing (" + meta.tid + ") (" + std::to_string(meta.id) + ")");

		constexpr float base = 70.0f;
		ui::Text *text = new ui::Text(ui::mk_center_WText("Press " GLYPH_A " to continue", SCREEN_HEIGHT() - 30.0f));
		float height = ui::text_height(&text->gtext().ctext) - 3.0f;
		errs.push_back(text);

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

		generic_main_breaking_loop(errs);
	}

	ui::wid()->get<ui::FreeSpaceIndicator>("size_indicator")->update();
	return res;
}

static void queue_is_empty(bool toggle = true)
{
	ui::Widgets wids;

	ui::WrapText *msg = new ui::WrapText("Queue is empty\nPress " GLYPH_A " to go back\nTip: press " GLYPH_Y " to add a title to the queue");
	msg->center(); msg->set_basey((SCREEN_HEIGHT() / 2) - 30);
	wids.push_back(msg);

	wids.push_back(new ui::PressToContinue(KEY_A));
	generic_main_breaking_loop(wids);

	if(toggle)
	{
		ui::wid()->for_each("button", [](ui::Widget *widget) -> void { ((ui::Button *) widget)->toggle(); });
		ui::wid()->get<ui::Text>("curr_action_desc")->toggle();
	}
}

void show_queue()
{
	ui::wid()->for_each("button", [](ui::Widget *widget) -> void { ((ui::Button *) widget)->toggle(); });
	ui::wid()->get<ui::Text>("curr_action_desc")->toggle();

	// Queue is empty :craig:
	if(g_queue.size() == 0)
		return queue_is_empty();

	ui::Widgets wids;
	ui::List<hs::FullTitle> *list = new ui::List<hs::FullTitle>(
		[](hs::FullTitle& meta) -> std::string { return meta.name; },
		[](ui::List<hs::FullTitle> *self, size_t index, u32 keys) -> ui::Results {
			if(keys & KEY_X)
			{
				if(self->out_of_bounds(index)) return ui::Results::go_on;
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
	list->set_on_change([&](ui::List<hs::FullTitle> *self, size_t index) {
		if(self->out_of_bounds(index)) return;
		meta->update_title(self->at(index));
	});


	wids.push_back("back", new ui::Button("Back", 240, 210, 310, 230), ui::Scr::bottom);
	wids.get<ui::Button>("back")->set_on_click([]() -> ui::Results {
		return ui::Results::quit_loop;
	});

	wids.push_back("install_all", new ui::Button("Install All", 10, 180, 100, 200), ui::Scr::bottom);
	wids.get<ui::Button>("install_all")->set_on_click([list]() -> ui::Results {
		queue_process_all(); list->update_text_reg();
		queue_is_empty(false);
		return ui::Results::quit_loop;
	});

	ui::Button *installSel = new ui::Button("Install Selected", 10, 210, 150, 230);
	wids.push_back("install_sel", installSel, ui::Scr::bottom);

	installSel->set_on_click([list]() -> ui::Results {
		if(list->out_of_bounds(list->get_pointer())) return ui::Results::go_on;
		queue_process(list->get_pointer()); list->update_text_reg();
		if(g_queue.size() == 0)
		{
			queue_is_empty(false);
			return ui::Results::quit_loop;
		}
		return ui::Results::go_on;
	}); installSel->toggle_click();
	wids.push_back(new ui::DoAfterFrames(20, [installSel]() -> ui::Results {
		installSel->toggle_click(); return ui::Results::go_on;
	}));


	generic_main_breaking_loop(wids);


	ui::wid()->for_each("button", [](ui::Widget *widget) -> void { ((ui::Button *) widget)->toggle(); });
	ui::wid()->get<ui::Text>("curr_action_desc")->toggle();
}

