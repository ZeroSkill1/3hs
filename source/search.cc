
#include "search.hh"

#include <widgets/meta.hh>

#include <ui/loading.hh>
#include <ui/swkbd.hh>
#include <ui/base.hh>

#include "lumalocale.hh"
#include "extmeta.hh"
#include "queue.hh"
#include "hsapi.hh"
#include "next.hh"
#include "util.hh"
#include "i18n.hh"


static void error(const std::string& msg)
{
	ui::RenderQueue queue;

	ui::builder<ui::next::Text>(ui::Screen::top, msg)
		.x(ui::layout::center_x)
		.y(ui::layout::center_y)
		.add_to(queue);
	
	queue.render_finite_button(KEY_A);
}

static void show_searchbar_search()
{
	std::string desc = next::set_desc(STRING(search_content));
	bool focus = next::set_focus(true);

	ui::RenderQueue::global()->render_frame();

	SwkbdResult res;
	SwkbdButton btn;

	std::string query = ui::keyboard([](ui::next::AppletSwkbd *swkbd) -> void {
		swkbd->hint(STRING(search_content_action));
		swkbd->valid(SWKBD_NOTEMPTY_NOTBLANK);
	}, &btn, &res);

	if(btn != SWKBD_BUTTON_CONFIRM)
	{
		next::set_focus(focus);
		next::set_desc(desc);
		return;
	}

	if(query.size() < 2 || res == SWKBD_INVALID_INPUT || res == SWKBD_OUTOFMEM || res == SWKBD_BANNED_INPUT)
	{
		error(STRING(invalid_query));
		next::set_focus(focus);
		next::set_desc(desc);
		return;
	}

	next::set_desc(PSTRING(results_query, query));
	next::set_focus(true);

	std::vector<hsapi::Title> titles;
	Result rres = hsapi::call<std::vector<hsapi::Title>&, const std::string&>(hsapi::search, titles, query);
	if(R_FAILED(rres))
	{
		next::set_focus(focus);
		next::set_desc(desc);
		return;
	}

	if(titles.size() == 0)
	{
		error(STRING(search_zero_results));
		next::set_focus(focus);
		next::set_desc(desc);
		return;
	}

	size_t cur = 0;
	do {
		hsapi::hid id = next::sel_gam(titles, &cur);
		if(id == next_gam_exit || id == next_gam_back)
			break;

		hsapi::FullTitle meta;
		if(show_extmeta_lazy(titles, id, &meta))
		{
			toggle_focus();
			process_hs(meta);
			luma::set_gamepatching();
			toggle_focus();
		}
	} while(true);

	next::set_focus(focus);
	next::set_desc(desc);
}

void show_search()
{
	show_searchbar_search();
}

