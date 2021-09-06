
#include "more.hh"

#include "find_missing.hh"
#include "hlink_view.hh"
#include "about.hh"
#include "i18n.hh"
#include "help.hh"
#include "util.hh"

#include <ui/image_button.hh>
#include <ui/button.hh>

#define ABOUT_I 0
#define FIND_MISSING_I 1
#define HELP_I 2
#define HLINK_I 3

#define Y1 (10)
#define Y2 (SCREEN_WIDTH(ui::Scr::bottom) - 10)
#define X1(i) (30 + (i * 30))
#define X2(i) (50 + (i * 30))


static void enter_exit()
{
	ui::wid()->get<ui::ImageButton>("more")->toggle_click();
}

void set_hi(size_t *index, ui::Button *buttons[], int ni)
{
	buttons[*index]->highlight(false);
	buttons[ni]->highlight();
	*index = ni;
}

void show_more()
{
	enter_exit();

#ifdef V02
	constexpr size_t maxIndex = 4;
#else
	constexpr size_t maxIndex = 3;
#endif
	size_t index = 0;

	ui::Button *buttons[maxIndex];

	// Create buttons
	ui::Button *about = new ui::Button(STRING(about_app), Y1, X1(ABOUT_I), Y2, X2(ABOUT_I));
	ui::Button *find_missing = new ui::Button(STRING(find_missing_content), Y1, X1(FIND_MISSING_I),
		Y2, X2(FIND_MISSING_I));
	ui::Button *help = new ui::Button(STRING(help_manual), Y1, X1(HELP_I), Y2, X2(HELP_I));
#ifdef V02
	ui::Button *hlink = new ui::Button("hLink", Y1, X1(HLINK_I), Y2, X2(HLINK_I));
#endif

	// Setup buttons
	about->highlight();

	about->set_on_click([&index, &buttons](bool inFrame) -> ui::Results {
		if(inFrame) ui::end_frame();
		set_hi(&index, buttons, ABOUT_I); show_about();
		return ui::Results::end_early;
	});

	find_missing->set_on_click([&index, &buttons](bool inFrame) -> ui::Results {
		if(inFrame) ui::end_frame();
		set_hi(&index, buttons, FIND_MISSING_I);
		return ui::Results::end_early;
	});

	help->set_on_click([&index, &buttons](bool inFrame) -> ui::Results {
		if(inFrame) ui::end_frame();
		set_hi(&index, buttons, HELP_I); show_help();
		return ui::Results::end_early;
	});

	find_missing->set_on_click([&index, &buttons](bool inFrame) -> ui::Results {
		if(inFrame) ui::end_frame();
		set_hi(&index, buttons, HELP_I); show_find_missing();
		return ui::Results::end_early;
	});

#ifdef V02
	hlink->set_on_click([&index, &buttons](bool inFrame) -> ui::Results {
		if(inFrame) ui::end_frame();
		set_hi(&index, buttons, HELP_I); show_hlink();
		return ui::Results::end_early;
	});
#endif

	// Setup indices
	buttons[ABOUT_I]        = about;
	buttons[FIND_MISSING_I] = find_missing;
	buttons[HELP_I]         = help;
#ifdef V02
	buttons[HLINK_I]        = hlink;
#endif

	ui::Widgets wids;
	wids.push_back(about, ui::Scr::bottom);
	wids.push_back(find_missing, ui::Scr::bottom);
	wids.push_back(help, ui::Scr::bottom);
#ifdef V02
	wids.push_back(hlink, ui::Scr::bottom);
#endif

	ui::Keys keys;
	while(ui::framenext(keys))
	{
		if(!ui::framedraw(wids, keys)) break;
		if(keys.kDown & KEY_DOWN && index < maxIndex - 1)
		{
			buttons[index]->highlight(false);
			buttons[index + 1]->highlight();
			++index;
		}

		else if(keys.kDown & KEY_UP && index > 0)
		{
			buttons[index]->highlight(false);
			buttons[index - 1]->highlight();
			--index;
		}

		else if(keys.kDown & KEY_A)
		{
			buttons[index]->click(false);
		}

		else if(keys.kDown & KEY_B)
		{
			break;
		}
	}

	enter_exit();
}

