
#include "next.hh"

#include <widgets/meta.hh>
#include <ui/core.hh>
#include <ui/list.hh>
#include <ui/text.hh>


std::string next::sel_cat(hs::Index& index)
{
	// Update curr action
	ui::wid()->get<ui::Text>("curr_action_desc")->replace_text(
		"Select a category");

	std::string ret;

	ui::Widgets widgets;
	widgets.push_back("list", new ui::List<hs::Category>(
		[](hs::Category& cat) -> std::string { return cat.displayName; },
		[&ret](ui::List<hs::Category> *self, size_t index) -> ui::Results {
			ret = self->at(index).name; return ui::Results::quit_loop;
		}, index.categories
	));

	ui::CatMeta *meta = new ui::CatMeta();
	if(index.categories.size() > 0) meta->update_cat(index.categories[0]);
	else lerror << "assert(index.categories.size() > 0) failed?? Is the server having a stroke?";

	widgets.push_back("meta", meta, ui::Scr::bottom);
	widgets.get<ui::List<hs::Category>>("list")->set_on_change([&](ui::List<hs::Category> *self, size_t index) {
		meta->update_cat(self->at(index));
	});

	ui::Keys keys;
	while(ui::framenext(keys))
	{
		// We're done and we got a selection
		if(!ui::framedraw(widgets, keys))
			return ret;
	}

	// User pressed [START]
	return next_cat_exit;
}

std::string next::sel_sub(hs::Index& index, std::string cat)
{
	// Update curr action
	ui::wid()->get<ui::Text>("curr_action_desc")->replace_text(
		"Select a subcategory");

	hs::Category *rcat = index[cat];
	std::string ret;

	llog << "assert(" << rcat->name << " == " << cat << ")";

	ui::Widgets widgets;
	widgets.push_back("list", new ui::List<hs::Subcategory>(
		[](hs::Subcategory& cat) -> std::string { return cat.displayName; },
		[&](ui::List<hs::Subcategory> *self, size_t index) {
			ret = self->at(index).name; return ui::Results::quit_loop;
		}, rcat->subcategories
	));

	ui::SubMeta *meta = new ui::SubMeta();
	if(rcat->subcategories.size() > 0) meta->update_sub(rcat->subcategories[0]);
	else lerror << "assert(rcat->subcategories.size() > 0) failed?? cat=" << cat;

	widgets.push_back("meta", meta, ui::Scr::bottom);
	widgets.get<ui::List<hs::Subcategory>>("list")->set_on_change([&](ui::List<hs::Subcategory> *self, size_t index) {
		meta->update_sub(self->at(index));
	});

	ui::Keys keys;
	while(ui::framenext(keys))
	{
		if(keys.kDown & KEY_B)
			return next_sub_back;

		// We're done and we got a selection
		if(!ui::framedraw(widgets, keys))
			return ret;
	}

	// User pressed [START]
	return next_sub_exit;
}

long int next::sel_gam(std::string cat, std::string sub)
{
	ui::wid()->get<ui::Text>("curr_action_desc")->replace_text(
		"Loading ..."); quick_global_draw();
	std::vector<hs::Title> titles = hs::titles_in(cat, sub);
	ui::wid()->get<ui::Text>("curr_action_desc")->replace_text(
		"Select a title");

	long int ret;

	ui::Widgets widgets;
	widgets.push_back("list", new ui::List<hs::Title>(
		[](hs::Title& title) -> std::string { return title.name; },
		[&](ui::List<hs::Title> *self, size_t index) -> ui::Results {
			ret = self->at(index).id; return ui::Results::quit_no_end;
		}, titles
	));

	ui::TitleMeta *meta = new ui::TitleMeta();
	if(titles.size() > 0) meta->update_title(titles[0]);
	else lerror << "assert(titles.size() > 0) failed?? cat=" << cat << ",sub=" << sub;

	widgets.push_back("meta", meta, ui::Scr::bottom);
	widgets.get<ui::List<hs::Title>>("list")->set_on_change([&](ui::List<hs::Title> *self, size_t index) {
		meta->update_title(self->at(index));
	});

	ui::Keys keys;
	while(ui::framenext(keys))
	{
		if(keys.kDown & KEY_B)
			return next_gam_back;

		// We're done and we got a selection
		if(!ui::framedraw(widgets, keys))
			return ret;
	}

	// User pressed [START]
	return next_gam_exit;
}
