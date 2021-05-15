
#include "search.hh"

#include <widgets/meta.hh>

#include <ui/press_to_continue.hh>
#include <ui/button.hh>
#include <ui/swkbd.hh>
#include <ui/text.hh>
#include <ui/list.hh>

#include "queue.hh"
#include "util.hh"
#include "hs.hh"


static void search_is_empty(std::string prev)
{
	ui::Widgets wids;

	ui::WrapText *msg = new ui::WrapText("Search gave 0 results\nPress " GLYPH_A " to go back");
	msg->center(); msg->set_basey((SCREEN_HEIGHT() / 2) - 30);
	wids.push_back(msg);

	wids.push_back(new ui::PressToContinue(KEY_A));
	generic_main_breaking_loop(wids);

	toggle_focus(prev);
}


void show_search()
{
	std::string prev = toggle_focus("Search for content");
	quick_global_draw();

	std::string query;

	ui::AppletSwkbd *keyboard = new ui::AppletSwkbd(&query);
	keyboard->validinput(SWKBD_NOTEMPTY_NOTBLANK);
	keyboard->hint("Search for content...");

	ui::Widgets search;
	search.push_back(keyboard);
	single_draw(search);

	SwkbdResult result = keyboard->get_result();
	if(keyboard->get_button() != SWKBD_BUTTON_CONFIRM || query.size() < 2 || (
			result == SWKBD_INVALID_INPUT || result ==  SWKBD_OUTOFMEM ||
			result == SWKBD_BANNED_INPUT
		)) return search_is_empty(prev);

	ui::Widgets wids;
	long int id = 0;

	ui::wid()->get<ui::Text>("curr_action_desc")->replace_text("Loading ...");
	quick_global_draw(); std::vector<hs::Title> titles = hs::search(query);
	ui::wid()->get<ui::Text>("curr_action_desc")->replace_text(
		"Results for query \"" + query + "\"");

	if(titles.size() == 0) return search_is_empty(prev);

	ui::List<hs::Title> *list = new ui::List<hs::Title>(
		[](hs::Title& title) -> std::string { return title.name; },
		[&](ui::List<hs::Title> *self, size_t index, u32 keys) -> ui::Results {
			if(keys & KEY_B) return ui::Results::quit_loop;
			if(keys & KEY_A)
			{
				id = self->at(index).id;
				return ui::Results::quit_no_end;
			}
			else if(keys & KEY_Y)
				queue_add(self->at(index).id);

			return ui::Results::go_on;
		}, titles
	); wids.push_back("list", list);
	list->add_button(KEY_Y | KEY_B);

	ui::TitleMeta *meta = new ui::TitleMeta();
	if(titles.size() > 0) meta->update_title(titles[0]);

	wids.push_back("meta", meta, ui::Scr::bottom);
	list->set_on_change([&](ui::List<hs::Title> *self, size_t index) {
		meta->update_title(self->at(index));
	});


	generic_main_breaking_loop(wids);
	toggle_focus(prev);
}

