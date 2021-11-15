
#include "next.hh"

#include <widgets/meta.hh>
#include <ui/core.hh>
#include <ui/list.hh>
#include <ui/text.hh>

#include <ui/base.hh>

#include "hsapi.hh"

#include "queue.hh"
#include "panic.hh"
#include "i18n.hh"
#include "util.hh"


const std::string *next::sel_cat(size_t *cursor)
{
	using list_t = ui::next::List<hsapi::Category>;

	std::string desc = next::set_desc(STRING(select_cat));
	bool focus = next::set_focus(false);
	const std::string *ret = nullptr;

	ui::RenderQueue queue;

	list_t *list;
	ui::builder<list_t>(ui::Screen::top, &hsapi::get_index()->categories)
		.connect(list_t::to_string, [](const hsapi::Category& cat) -> std::string { return cat.disp; })
		.connect(list_t::select, [&ret](list_t *self, size_t i, u32 kDown) -> bool {
			ret = &self->at(i).name;
			if(kDown & KEY_START)
				ret = next_cat_exit;
			return false;
		})
		.connect(list_t::change, [](list_t *self, size_t i) -> void {
			self->at(i).name;
		})
		.connect(list_t::buttons, KEY_START)
		.x(5.0f).y(25.0f)
		.add_to(&list, queue);

	if(cursor != nullptr) list->set_pos(*cursor);
	queue.render_finite();
	if(cursor != nullptr) *cursor = list->get_pos();

	next::set_focus(focus);
	next::set_desc(desc);
	return ret;

#if 0
	// Update curr action
	ui::wid()->get<ui::Text>("curr_action_desc")->replace_text(STRING(select_cat));


	ui::Widgets widgets;
	widgets.push_back("list", new ui::List<hsapi::Category>(
		[](hsapi::Category& cat) -> std::string { return cat.disp; },
		[&ret](ui::List<hsapi::Category> *self, size_t index, u32) -> ui::Results {
			ret = &self->at(index).name; return ui::Results::quit_loop;
		}, hsapi::get_index()->categories
	));

	ui::CatMeta *meta = new ui::CatMeta();
	if(hsapi::get_index()->categories.size() > 0) meta->update_cat(hsapi::get_index()->categories[0]);
	else panic(STRING(no_cats_index));

	widgets.push_back("meta", meta, ui::Scr::bottom);
	widgets.get<ui::List<hsapi::Category>>("list")->set_on_change([&](ui::List<hsapi::Category> *self, size_t index) {
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
#endif
}

const std::string *next::sel_sub(const std::string& cat, size_t *cursor)
{
	using list_t = ui::next::List<hsapi::Subcategory>;

	std::string desc = next::set_desc(STRING(select_cat));
	bool focus = next::set_focus(false);
	const std::string *ret = nullptr;

	hsapi::Category *rcat = hsapi::get_index()->find(cat);
	ui::RenderQueue queue;

	list_t *list;
	ui::builder<list_t>(ui::Screen::top, &rcat->subcategories)
		.connect(list_t::to_string, [](const hsapi::Subcategory& scat) -> std::string { return scat.disp; })
		.connect(list_t::select, [&ret](list_t *self, size_t i, u32 kDown) -> bool {
			ret = &self->at(i).name;
			if(kDown & KEY_B) ret = next_sub_back;
			if(kDown & KEY_START) ret = next_sub_exit;
			return false;
		})
		.connect(list_t::change, [](list_t *self, size_t i) -> void {
			self->at(i).name;
		})
		.connect(list_t::buttons, KEY_B | KEY_START)
		.x(5.0f).y(25.0f)
		.add_to(&list, queue);



	if(cursor != nullptr) list->set_pos(*cursor);
	queue.render_finite();
	if(cursor != nullptr) *cursor = list->get_pos();

	next::set_focus(focus);
	next::set_desc(desc);
	return ret;


#if 0
	// Update curr action
	ui::wid()->get<ui::Text>("curr_action_desc")->replace_text(STRING(select_subcat));

	hsapi::Category *rcat = hsapi::get_index()->find(cat);
	std::string *ret;

	llog << "assert(" << rcat->name << " == " << cat << ")";

	ui::Widgets widgets;
	widgets.push_back("list", new ui::List<hsapi::Subcategory>(
		[](hsapi::Subcategory& cat) -> std::string { return cat.disp; },
		[&](ui::List<hsapi::Subcategory> *self, size_t index, u32) {
			ret = &self->at(index).name; return ui::Results::quit_loop;
		}, rcat->subcategories
	));

	ui::SubMeta *meta = new ui::SubMeta();
	if(rcat->subcategories.size() > 0) meta->update_sub(rcat->subcategories[0]);
	else panic(STRING(empty_subcat));


	widgets.push_back("meta", meta, ui::Scr::bottom);
	widgets.get<ui::List<hsapi::Subcategory>>("list")->set_on_change([&](ui::List<hsapi::Subcategory> *self, size_t index) {
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
#endif
}

hsapi::hid next::sel_gam(std::vector<hsapi::Title>& titles, size_t *cursor)
{
	using list_t = ui::next::List<hsapi::Title>;

	std::string desc = next::set_desc(STRING(select_title));
	bool focus = next::set_focus(false);
	hsapi::hid ret = next_gam_back;

	ui::RenderQueue queue;

	list_t *list;
	ui::builder<list_t>(ui::Screen::top, &titles)
		.connect(list_t::to_string, [](const hsapi::Title& title) -> std::string { return title.name; })
		.connect(list_t::select, [&ret](list_t *self, size_t i, u32 kDown) -> bool {
			ret = self->at(i).id;
			if(kDown & KEY_B) ret = next_gam_back;
			if(kDown & KEY_START) ret = next_gam_exit;
			if(kDown & KEY_Y)
			{
				ui::RenderQueue::global()->render_and_then([ret]() -> void {
					/* the true is there to stop ambiguidity (how?) */
					queue_add(ret, true);
				});
				return true;
			}
			return false;
		})
		.connect(list_t::change, [](list_t *self, size_t i) -> void {
			self->at(i).id;
		})
		.connect(list_t::buttons, KEY_B | KEY_Y | KEY_START)
		.x(5.0f).y(25.0f)
		.add_to(&list, queue);

	if(cursor != nullptr) list->set_pos(*cursor);
	queue.render_finite();
	if(cursor != nullptr) *cursor = list->get_pos();

	next::set_focus(focus);
	next::set_desc(desc);
	return ret;
#if 0
	ui::wid()->get<ui::Text>("curr_action_desc")->replace_text(STRING(select_title));

	hsapi::hid ret;

	ui::Widgets widgets;
	ui::List<hsapi::Title> *list = new ui::List<hsapi::Title>(
		[](hsapi::Title& title) -> std::string { return title.name; },
		[&](ui::List<hsapi::Title> *self, size_t index, u32 keys) -> ui::Results {
			if(keys & KEY_A)
			{
				ret = self->at(index).id;
				return ui::Results::quit_no_end;
			}
			else if(keys & KEY_Y)
			{
				// it does some drawing :yeehad:
				ui::end_frame();
				queue_add(self->at(index).id, true);
				return ui::Results::end_early;
			}

			return ui::Results::go_on;
		}, titles
	); widgets.push_back("list", list);
	list->add_button(KEY_Y);

	ui::TitleMeta *meta = new ui::TitleMeta();
	if(titles.size() > 0) meta->update_title(titles[0]);
	else { delete meta; return next_gam_back; }

	widgets.push_back("meta", meta, ui::Scr::bottom);
	widgets.get<ui::List<hsapi::Title>>("list")->set_on_change([&](ui::List<hsapi::Title> *self, size_t index) {
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
#endif
}

