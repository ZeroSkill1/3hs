
#include "selectors.hh"
#include "ui/text.hh"

#include "install.hh"


long sel::cat(hs::Index& indx)
{
	ui::wid()->get<ui::Text>("curr_action_desc")->replace_text("Select a category:");

	ui::Widgets wids;
	long ret = 1;

	wids.push_back("cat_list", new ui::List<hs::Category>(
		[](hs::Category& other) -> std::string {
			return other.displayName;
		},
		[&](hs::Category& cat, size_t) -> ui::Results {
			bool rret = sel::subcat(indx, cat, ret);
			
			// We found a game we want to install; let main handle it
			if(ret > 0) return ui::Results::quit_no_end;

			// User wants to quit
			if(!rret) return ui::Results::quit_no_end;

			// This shouldn't happen (?)
			return ui::Results::go_on;
		}, indx.categories
	));

	ui::Keys keys;
	while(ui::framenext(keys))
	{
		if(!ui::framedraw(wids, keys))
			return ret;
	}

	// If we reached this we should always exit;
	// else we'd return somewhere else
	return sel::Results::exit;
}

bool sel::subcat(hs::Index& indx, hs::Category cat, long& id)
{
	ui::wid()->get<ui::Text>("curr_action_desc")->replace_text("Select a subcategory:");

	ui::Widgets wids;
	wids.push_back("subcat_list", new ui::List<hs::Subcategory>(
		[](hs::Subcategory& other) -> std::string {
			return other.displayName;
		},
		[&](hs::Subcategory& sub, size_t) -> ui::Results {
			bool ret = sel::game(cat.name, sub.name, id);
			ui::wid()->get<ui::Text>("curr_action_desc")->replace_text("Select a subcategory:");
			
			// We found an id we want to install
			if(id > 0) return ui::Results::quit_loop;

			// We want to go to the previous one (this one)
			// Reset id and keep going
			if(id == sel::Results::prev)
			{ id = 0; return ui::Results::end_early; }

			// User wants to quit
			if(!ret) return ui::Results::quit_no_end;

			// Nothing happened (?)
			return ui::Results::end_early;
		}, cat.subcategories
	));

	ui::Keys keys;
	while(ui::framenext(keys))
	{
		if(!ui::framedraw(wids, keys) || keys.kDown & KEY_B)
			return true;
	}

	id = sel::Results::cancel;
	return false;
}

bool sel::game(std::string cat, std::string subcat, long& id)
{
	ui::wid()->get<ui::Text>("curr_action_desc")->replace_text("Loading..."); quick_global_draw();
	std::vector<hs::Title> titles = hs::titles_in(cat, subcat);
	ui::wid()->get<ui::Text>("curr_action_desc")->replace_text("Select a title:");

	ui::Widgets wids;
	wids.push_back("titles_list", new ui::List<hs::Title>(
		[](hs::Title& other) -> std::string {
			return other.name;
		},
		[&](hs::Title& title, size_t) -> ui::Results {
			id = title.id;

			// After installing we want to go on in the game
			// Selector for that category
			return ui::Results::end_early;
		}, titles
	));

	ui::Keys keys;
	while(ui::framenext(keys))
	{
		if(!ui::framedraw(wids, keys))
			return true;
		if(keys.kDown & KEY_B)
		{
			id = sel::Results::prev;
			return false;
		}
	}

	id = sel::Results::cancel;
	return false;
}

void sel::game(hs::id_t id)
{
	hs::Title meta = hs::title_meta(id);

	/* Actually install */

	/* Balances ranges */
	size_t ranges[NUMTHREADS];
	for(size_t i = 0; i < NUMTHREADS; ++i)
	{
		size_t now = (meta.size / NUMTHREADS) * (i + 1);
		ranges[i] = now;
	}

	std::tuple<size_t, size_t> rrange[NUMTHREADS];
	rrange[0] = std::make_tuple(0, ranges[0]);

	for(size_t i = 1; i < NUMTHREADS - 1; ++i)
		rrange[i] = std::make_tuple(ranges[i - 1] + 1, ranges[i]);

	rrange[NUMTHREADS - 1] = std::make_tuple(ranges[NUMTHREADS - 2] + 1, meta.size);

	/* rrange:
	 * Size: 3347
	 * 0 -> 1673
	 * 1674-> 3347
	 * * * * * * */

	game::start_mutex();

	game::clean_mutex();

}
