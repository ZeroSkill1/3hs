
#include "selectors.hh"
#include "ui/text.hh"


int sel::cat(hs::Index& indx)
{
	ui::wid()->get<ui::Text>("curr_action_desc")->replace_text("Select a category:");

	ui::Widgets wids;
	int ret = 1;

	wids.push_back("cat_list", new ui::List<hs::Category>(
		[](hs::Category& other) -> std::string {
			return other.displayName;
		},
		[&](hs::Category& cat, size_t) -> bool {
			return sel::subcat(indx, cat, ret);
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

bool sel::subcat(hs::Index& indx, hs::Category cat, int& id)
{
	ui::wid()->get<ui::Text>("curr_action_desc")->replace_text("Select a subcategory:");

	ui::Widgets wids;
	wids.push_back("subcat_list", new ui::List<hs::Subcategory>(
		[](hs::Subcategory& other) -> std::string {
			return other.displayName;
		},
		[&](hs::Subcategory& sub, size_t) -> bool {
			bool ret = sel::game(cat.name, sub.name, id);
			ui::wid()->get<ui::Text>("curr_action_desc")->replace_text("Select a subcategory:");
			if(id == sel::Results::prev)
			{ id = 0; return true; }
			return ret;
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

bool sel::game(std::string cat, std::string subcat, int& id)
{
	ui::wid()->get<ui::Text>("curr_action_desc")->replace_text("Loading..."); quick_global_draw();
	std::vector<hs::Title> titles = hs::titles_in(cat, subcat);
	ui::wid()->get<ui::Text>("curr_action_desc")->replace_text("Select a title:");

	ui::Widgets wids;
	wids.push_back("titles_list", new ui::List<hs::Title>(
		[](hs::Title& other) -> std::string {
			return other.name;
		},
		[](hs::Title& title, size_t) -> bool {
			return true;
		}, titles, 10000
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
