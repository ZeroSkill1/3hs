
#include "queue.hh"

#include <widgets/indicators.hh>

#include <ui/confirm.hh>
#include <ui/list.hh>

#include <widgets/meta.hh>

#include <vector>

#include "lumalocale.hh"
#include "installgui.hh"
#include "install.hh"
#include "panic.hh"
#include "error.hh"
#include "i18n.hh"
#include "util.hh"
#include "ctr.hh"

static std::vector<hsapi::FullTitle> g_queue;
std::vector<hsapi::FullTitle>& queue_get() { return g_queue; }

void queue_add(const hsapi::FullTitle& meta)
{
	g_queue.push_back(meta);
}

void queue_add(hsapi::hid id, bool disp)
{
	hsapi::FullTitle meta;
	Result res = disp ? hsapi::call(hsapi::title_meta, meta, std::move(id))
		: hsapi::scall(hsapi::title_meta, meta, std::move(id));
	if(R_FAILED(res)) return;
	queue_add(meta);
}

void queue_remove(size_t index)
{
	g_queue.erase(g_queue.begin() + index);
}

void queue_clear()
{
	g_queue.clear();
}

void queue_process(size_t index)
{
	if(R_SUCCEEDED(install::gui::hs_cia(g_queue[index])))
		queue_remove(index);
}

void queue_process_all()
{
	std::vector<Result> errs;
	for(hsapi::FullTitle& meta : g_queue)
	{
		Result res = install::gui::hs_cia(meta, false);
		if(R_FAILED(res)) errs.push_back(res);
	}

	/* only enable gamepatching if one or more
	 * titles succeeded to install */
	if(errs.size() != g_queue.size())
		luma::maybe_set_gamepatching();

	// TODO: Displays errs

	queue_clear();
}

static void queue_is_empty()
{
	ui::RenderQueue queue;

	ui::builder<ui::Text>(ui::Screen::top, STRING(queue_empty))
		.x(ui::layout::center_x)
		.y(ui::layout::center_y)
		.add_to(queue);

	queue.render_finite_button(KEY_A | KEY_B);
}

void show_queue()
{
	using list_t = ui::List<hsapi::FullTitle>;
	bool focus = set_focus(true);

	// Queue is empty :craig:
	if(g_queue.size() == 0)
	{
		queue_is_empty();
		set_focus(focus);
		return;
	}

	ui::RenderQueue queue;

	ui::TitleMeta *meta;

	ui::builder<ui::TitleMeta>(ui::Screen::bottom, g_queue[0])
		.add_to(&meta, queue);

	ui::builder<list_t>(ui::Screen::top, &g_queue)
		.connect(list_t::to_string, [](const hsapi::FullTitle& meta) -> std::string { return meta.name; })
		.connect(list_t::select, [meta](list_t *self, size_t i, u32 kDown) -> bool {
			/* why is the cast necessairy? */
			((void) i);
			ui::RenderQueue::global()->render_and_then((std::function<bool()>) [self, meta, kDown]() -> bool {
				size_t i = self->get_pos(); /* for some reason the i param corrupted (?) */
				if(kDown & KEY_X)
					queue_remove(i);
				// TODO: fix segv when clicking [A] & error
				else if(kDown & KEY_A)
					queue_process(i);

				if(g_queue.size() == 0)
					return false; /* we're done */
				/* if we removed the last item */
				if(i == g_queue.size())
					--i;

				i = i > 0 ? i - 1 : 0;
				meta->set_title(self->at(i));
				self->set_pos(i);
				self->update();

				return true;
			});

			return true;
		})
		.connect(list_t::change, [meta](list_t *self, size_t i) -> void {
			meta->set_title(self->at(i));
		})
		.connect(list_t::buttons, KEY_X)
		.x(5.0f).y(25.0f)
		.add_to(queue);

	ui::builder<ui::Button>(ui::Screen::bottom, STRING(install_all))
		.connect(ui::Button::click, []() -> bool {
			queue_process_all();
			/* the queue will always be empty after this */
			return false;
		})
		.wrap()
		.x(ui::layout::right)
		.y(210.0f)
		.add_to(queue);

	queue.render_finite_button(KEY_B);
	set_focus(focus);
}

