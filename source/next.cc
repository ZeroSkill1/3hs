
#include "next.hh"

#include <widgets/meta.hh>
#include <ui/list.hh>
#include <ui/base.hh>

#include "hsapi.hh"

#include "queue.hh"
#include "panic.hh"
#include "i18n.hh"
#include "util.hh"


const std::string *next::sel_cat(size_t *cursor)
{
	panic_assert(hsapi::get_index()->categories.size() > *cursor, "invalid cursor position");
	using list_t = ui::List<hsapi::Category>;

	std::string desc = set_desc(STRING(select_cat));
	bool focus = set_focus(false);
	const std::string *ret = nullptr;

	ui::RenderQueue queue;

	ui::CatMeta *meta;
	list_t *list;

	ui::builder<ui::CatMeta>(ui::Screen::bottom, hsapi::get_index()->categories[*cursor])
		.add_to(&meta, queue);

	ui::builder<list_t>(ui::Screen::top, &hsapi::get_index()->categories)
		.connect(list_t::to_string, [](const hsapi::Category& cat) -> std::string { return cat.disp; })
		.connect(list_t::select, [&ret](list_t *self, size_t i, u32 kDown) -> bool {
			ret = &self->at(i).name;
			if(kDown & KEY_START)
				ret = next_cat_exit;
			return false;
		})
		.connect(list_t::change, [meta](list_t *self, size_t i) -> void {
			meta->set_cat(self->at(i));
		})
		.connect(list_t::buttons, KEY_START)
		.x(5.0f).y(25.0f)
		.add_to(&list, queue);

	if(cursor != nullptr) list->set_pos(*cursor);
	queue.render_finite();
	if(cursor != nullptr) *cursor = list->get_pos();

	set_focus(focus);
	set_desc(desc);
	return ret;
}

const std::string *next::sel_sub(const std::string& cat, size_t *cursor)
{
	using list_t = ui::List<hsapi::Subcategory>;

	std::string desc = set_desc(STRING(select_cat));
	bool focus = set_focus(false);
	const std::string *ret = nullptr;

	hsapi::Category *rcat = hsapi::get_index()->find(cat);
	ui::RenderQueue queue;

	panic_assert(rcat, "couldn't find category");

	ui::SubMeta *meta;
	list_t *list;

	panic_assert(rcat->subcategories.size() > *cursor, "invalid cursor position");

	ui::builder<ui::SubMeta>(ui::Screen::bottom, rcat->subcategories[*cursor])
		.add_to(&meta, queue);

	ui::builder<list_t>(ui::Screen::top, &rcat->subcategories)
		.connect(list_t::to_string, [](const hsapi::Subcategory& scat) -> std::string { return scat.disp; })
		.connect(list_t::select, [&ret](list_t *self, size_t i, u32 kDown) -> bool {
			ret = &self->at(i).name;
			if(kDown & KEY_B) ret = next_sub_back;
			if(kDown & KEY_START) ret = next_sub_exit;
			return false;
		})
		.connect(list_t::change, [meta](list_t *self, size_t i) -> void {
			meta->set_sub(self->at(i));
		})
		.connect(list_t::buttons, KEY_B | KEY_START)
		.x(5.0f).y(25.0f)
		.add_to(&list, queue);

	if(cursor != nullptr) list->set_pos(*cursor);
	queue.render_finite();
	if(cursor != nullptr) *cursor = list->get_pos();

	set_focus(focus);
	set_desc(desc);
	return ret;
}

hsapi::hid next::sel_gam(std::vector<hsapi::Title>& titles, size_t *cursor)
{
	panic_assert(titles.size() > *cursor, "invalid cursor position");
	using list_t = ui::List<hsapi::Title>;

	std::string desc = set_desc(STRING(select_title));
	bool focus = set_focus(false);
	hsapi::hid ret = next_gam_back;

	ui::RenderQueue queue;

	ui::TitleMeta *meta;
	list_t *list;

	ui::builder<ui::TitleMeta>(ui::Screen::bottom, titles[*cursor])
		.add_to(&meta, queue);

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
		.connect(list_t::change, [meta](list_t *self, size_t i) -> void {
			meta->set_title(self->at(i));
		})
		.connect(list_t::buttons, KEY_B | KEY_Y | KEY_START)
		.x(5.0f).y(25.0f)
		.add_to(&list, queue);

	if(cursor != nullptr) list->set_pos(*cursor);
	queue.render_finite();
	if(cursor != nullptr) *cursor = list->get_pos();

	set_focus(focus);
	set_desc(desc);
	return ret;
}

