
#include "selectors.hh"


int sel::cat(hs::Index& indx)
{
	ui::Widgets wids;
	int ret = 1;

	ui::List<std::string> *listWid = new ui::List<std::string>(
		[](std::string& other) { return other; },
		[&](size_t index) -> bool {
			return sel::subcat(indx, (*listWid)[index], ret);
		}
	);

	for(hs::Category& cat : indx.categories)
		listWid->append(cat.displayName);
	wids.push_back("cat_list", listWid);

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

bool sel::subcat(hs::Index& indx, std::string catstr, int& id)
{
	hs::Category *cat = indx[catstr];
	// This shouldn't happen ....
	if(cat == nullptr)
	{
		id = sel::Results::cat_nullptr;
		return false;
	}

	ui::Widgets wids;

	ui::List<std::string> *listWid = new ui::List<std::string>(
		[](std::string& other) { return other; },
		[&](size_t index) -> bool {
			return sel::game(catstr, (*listWid)[index], id);
		}
	);

	for(hs::Subcategory& sub : cat->subcategories)
		listWid->append(sub.displayName);
	wids.push_back("subcat_list", listWid);

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
