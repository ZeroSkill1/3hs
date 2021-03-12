
#include "selectors.hh"


int sel::cat(hs::Index& indx)
{
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
	ui::Widgets wids;
	wids.push_back("subcat_list", new ui::List<hs::Subcategory>(
		[](hs::Subcategory& other) -> std::string {
			return other.displayName;
		},
		[&](hs::Subcategory& sub, size_t) -> bool {
			return sel::game(cat.name, sub.name, id);
		}, cat.subcategories
	));

	ui::Keys keys;
	while(ui::framenext(keys))
	{
		if(!ui::framedraw(wids, keys))
			return true;
	}

	id = sel::Results::cancel;
	return false;
}

bool sel::game(std::string cat, std::string subcat, int& id)
{
	/* Sub */ return true;
}
