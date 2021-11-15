
#include "extmeta.hh"

#include <ui/base.hh>

#include <iostream>

#include <ui/scrollingText.hh>
#include <widgets/meta.hh>
#include <ui/core.hh>

#include "thread.hh"
#include "panic.hh"
#include "i18n.hh"
#include "util.hh"
#include "ctr.hh"


bool show_extmeta_lazy(const hsapi::Title& base)
{
	ui::next::Text *prodcode;
	ui::next::Text *version;
	ui::RenderQueue queue;

	bool ret = true;

	ui::builder<ui::next::Text>(ui::Screen::top, STRING(press_to_install))
		.x(ui::layout::center_x)
		.y(170.0f)
		.add_to(queue);

	/***
	 * name (wrapped)
	 * category -> subcategory
	 *
	 * "Press a to install, b to not"
	 * =======================
	 * version     prod
	 * tid         
	 * size
	 * landing id
	 ***/

	/* name */
	ui::builder<ui::next::Text>(ui::Screen::top, base.name)
		.x(9.0f)
		.y(25.0f)
		// if this overflows to the point where it overlaps with
		// STRING(press_to_install) we're pretty fucked, but i
		// don't think we have such titles
		.wrap()
		.add_to(queue);
	ui::builder<ui::next::Text>(ui::Screen::top, STRING(name))
		.size(0.45f)
		.x(9.0f)
		.under(queue.back(), -1.0f)
		.add_to(queue);

	/* category -> subcategory */
	// on the bottom screen there are cases where this overflows,
	// but i don't think that can happen on the top screen sinc it's a bit bigger
	ui::builder<ui::next::Text>(ui::Screen::top, hsapi::get_index()->find(base.cat)->disp + " -> " + hsapi::get_index()->find(base.cat)->find(base.subcat)->disp)
		.x(9.0f)
		.under(queue.back(), 1.0f)
		.add_to(queue);
	ui::builder<ui::next::Text>(ui::Screen::top, STRING(category))
		.size(0.45f)
		.x(9.0f)
		.under(queue.back(), -1.0f)
		.add_to(queue);

	/* version */
	ui::builder<ui::next::Text>(ui::Screen::bottom, "1.0.0 (1024)" /*STRING(loading)*/) 
		.x(9.0f)
		.y(20.0f)
		.add_to(&version, queue);
	ui::builder<ui::next::Text>(ui::Screen::bottom, STRING(version))
		.size(0.45f)
		.x(9.0f)
		.under(queue.back(), -1.0f)
		.add_to(queue);

	/* product code */
	ui::builder<ui::next::Text>(ui::Screen::bottom, "CTR-P-ABCD" /*STRING(loading)*/)
		.x(175.0f)
		.align_y(version)
		.add_to(&prodcode, queue);
	ui::builder<ui::next::Text>(ui::Screen::bottom, STRING(prodcode))
		.size(0.45f)
		.x(175.0f)
		.under(queue.back(), -1.0f)
		.add_to(queue);

	/* size */
	ui::builder<ui::next::Text>(ui::Screen::bottom, ui::human_readable_size_block<hsapi::hsize>(base.size))
		.x(9.0f)
		.under(queue.back(), 1.0f)
		.add_to(queue);
	ui::builder<ui::next::Text>(ui::Screen::bottom, STRING(size))
		.size(0.45f)
		.x(9.0f)
		.under(queue.back(), -1.0f)
		.add_to(queue);

	/* title id */
	ui::builder<ui::next::Text>(ui::Screen::bottom, ctr::tid_to_str(base.tid))
		.x(9.0f)
		.under(queue.back(), 1.0f)
		.add_to(queue);
	ui::builder<ui::next::Text>(ui::Screen::bottom, STRING(tid))
		.size(0.45f)
		.x(9.0f)
		.under(queue.back(), -1.0f)
		.add_to(queue);

	/* landing id */
	ui::builder<ui::next::Text>(ui::Screen::bottom, std::to_string(base.id))
		.x(9.0f)
		.under(queue.back(), 1.0f)
		.add_to(queue);
	ui::builder<ui::next::Text>(ui::Screen::bottom, STRING(landing_id))
		.size(0.45f)
		.x(9.0f)
		.under(queue.back(), -1.0f)
		.add_to(queue);

	ui::builder<ui::next::ButtonCallback>(ui::Screen::top, KEY_B)
		.connect(ui::next::ButtonCallback::kdown, [&ret](u32) -> bool { return ret = false; })
		.add_to(queue);

	// launch fetch additional data thread,
	ctr::thread<> th([base, version, prodcode]() -> void {
		hsapi::FullTitle full;
		if(R_FAILED(hsapi::title_meta(full, base.id)))
			return;
		version->set_text("hello");
		prodcode->set_text("hello");
	});
#if 0
		/* gotta wait for the frame to finish before modifying the widgets */
	//	ui::RenderQueue::global()->render_and_then([full, version, prodcode]() -> void {
//			version->set_text(hsapi::parse_vstring(full.version) + " (" + std::to_string(full.version) + ")");
//			prodcode->set_text(full.prod);
			std::cerr << (long long) version;
			std::cerr << full.prod << "\n";
//		});
	});
#endif
		ui::RenderQueue::global()->render_and_then([version]() -> void {
			version->set_text("hello");
		});

//		.x(9.0f)
//		.y(20.0f)
	version->set_text("fucks");
	queue.render_finite_button(KEY_Y);

	/* At this point we're done rendering and
	 * waiting for the *fetching* of the full data
	 * and *setting* of the renderqueue callback */
	th.join();
	/* We clear the renderqueue callback here
	 * because else it may operate on an invalid
	 * renderqueue in the next frame */
	ui::RenderQueue::global()->detach_after();

	return ret;
}

bool show_extmeta(const hsapi::FullTitle& title)
{
	std::string odesc = next::set_desc(STRING(more_about_content));
	bool oret = true;

	ui::RenderQueue queue;

	ui::builder<ui::next::Text>(ui::Screen::top, STRING(press_to_install))
		.x(ui::layout::center_x)
		.y(170.0f)
		.add_to(queue);

	ui::builder<ui::next::Text>(ui::Screen::top, hsapi::parse_vstring(title.version) + " (" + std::to_string(title.version) + ")")
		.x(9.0f)
		.y(40.0f)
		.add_to(queue);
	ui::builder<ui::next::Text>(ui::Screen::top, STRING(version))
		.size(0.45f)
		.x(9.0f)
		.under(queue.back(), -1.0f)
		.add_to(queue);
	ui::builder<ui::next::Text>(ui::Screen::top, title.prod)
		.x(9.0f)
		.under(queue.back(), 1.0f)
		.add_to(queue);
	ui::builder<ui::next::Text>(ui::Screen::top, STRING(prodcode))
		.size(0.45f)
		.x(9.0f)
		.under(queue.back(), -1.0f)
		.add_to(queue);


	ui::builder<ui::next::TitleMeta>(ui::Screen::bottom, title)
		.add_to(queue);

	ui::builder<ui::next::ButtonCallback>(ui::Screen::top, KEY_B)
		.connect(ui::next::ButtonCallback::kdown, [&oret](u32) -> bool { return oret = false; })
		.add_to(queue);

	queue.render_finite_button(KEY_A);
	next::set_desc(odesc);
	return oret;
}

