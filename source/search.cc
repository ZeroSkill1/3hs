
#include "search.hh"

#include <widgets/meta.hh>

#include <ui/loading.hh>
#include <ui/button.hh>
#include <ui/swkbd.hh>
#include <ui/util.hh>
#include <ui/text.hh>
#include <ui/list.hh>

#include "lumalocale.hh"
#include "extmeta.hh"
#include "queue.hh"
#include "hsapi.hh"
#include "util.hh"
#include "i18n.hh"


static void search_is_empty(std::string prev)
{
	ui::Widgets wids;

	ui::WrapText *msg = new ui::WrapText(STRING(search_zero_results));
	msg->center(); msg->set_basey((SCREEN_HEIGHT() / 2) - 30);
	wids.push_back(msg);

	wids.push_back(new ui::PressToContinue(KEY_A));
	generic_main_breaking_loop(wids);

	swap_desc(prev);
}


static void show_searchbar_search()
{
	std::string prev = toggle_focus(STRING(search_content));
	quick_global_draw();

	SwkbdResult res; SwkbdButton btn;
	std::string query = ui::AppletSwkbd::read([](ui::AppletSwkbd *keyboard) -> void {
		keyboard->validinput(SWKBD_NOTEMPTY_NOTBLANK);
		keyboard->hint(STRING(search_content_action));
	}, &res, &btn);

	toggle_focus();

	if(btn != SWKBD_BUTTON_CONFIRM || query.size() < 2 || (
			res == SWKBD_INVALID_INPUT || res ==  SWKBD_OUTOFMEM ||
			res == SWKBD_BANNED_INPUT
		)) return search_is_empty(prev);

	ui::Widgets wids;

	ui::wid()->get<ui::Text>("curr_action_desc")->toggle();

	std::vector<hsapi::Title> titles;
	Result rres = hsapi::call<std::vector<hsapi::Title>&, const std::string&>(hsapi::search, titles, query);
	if(R_FAILED(rres))
		return;

	ui::wid()->get<ui::Text>("curr_action_desc")->replace_text(
		PSTRING(results_query, query));

	if(titles.size() == 0) return search_is_empty(prev);

	ui::List<hsapi::Title> *list = new ui::List<hsapi::Title>(
		[](hsapi::Title& title) -> std::string { return title.name; },
		[&](ui::List<hsapi::Title> *self, size_t index, u32 keys) -> ui::Results {
			if(keys & KEY_B) return ui::Results::quit_loop;
			if(keys & KEY_A)
			{
				ui::end_frame();

				hsapi::FullTitle meta;
				if(R_FAILED(hsapi::call(hsapi::title_meta, meta, std::move((hsapi::hid) self->at(index).id))))
					return ui::Results::go_on;

				if(show_extmeta(meta))
				{
					toggle_focus();
					process_hs(meta);
					luma::set_gamepatching();
					toggle_focus();
				}

				return ui::Results::end_early;
			}
			else if(keys & KEY_Y)
			{
				ui::end_frame();
				queue_add(self->at(index).id, true);
				return ui::Results::quit_no_end;
			}

			return ui::Results::go_on;
		}, titles
	); wids.push_back("list", list);
	list->add_button(KEY_Y | KEY_B);

	ui::TitleMeta *meta = new ui::TitleMeta();
	if(titles.size() > 0) meta->update_title(titles[0]);

	wids.push_back("meta", meta, ui::Scr::bottom);
	list->set_on_change([&](ui::List<hsapi::Title> *self, size_t index) {
		meta->update_title(self->at(index));
	});


	generic_main_breaking_loop(wids);
	swap_desc(prev);
}

void show_search()
{
	show_searchbar_search();
}

