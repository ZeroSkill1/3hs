
#include "more.hh"

#include "find_missing.hh"
#include "hlink_view.hh"
#include "about.hh"
#include "i18n.hh"
#include "help.hh"
#include "util.hh"

#include <ui/image_button.hh>
#include <ui/button.hh>
#include <ui/base.hh>

enum MoreInds {
	IND_ABOUT = 0,
	IND_FIND_MISSING,
	IND_HELP,
	IND_HLINK,
	IND_MAX
};

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

	size_t index = 0;

	ui::Button *buttons[IND_MAX];

	// Create buttons
	ui::Button *about = new ui::Button(STRING(about_app), Y1, X1(IND_ABOUT), Y2, X2(IND_ABOUT));
	ui::Button *find_missing = new ui::Button(STRING(find_missing_content), Y1, X1(IND_FIND_MISSING),
		Y2, X2(IND_FIND_MISSING));
	ui::Button *help = new ui::Button(STRING(help_manual), Y1, X1(IND_HELP), Y2, X2(IND_HELP));
	ui::Button *hlink = new ui::Button("hLink", Y1, X1(IND_HLINK), Y2, X2(IND_HLINK));

	// Setup buttons
	about->highlight();

	about->set_on_click([&index, &buttons](bool inFrame) -> ui::Results {
		if(inFrame) ui::end_frame();
		set_hi(&index, buttons, IND_ABOUT); show_about();
		return ui::Results::end_early;
	});

	find_missing->set_on_click([&index, &buttons](bool inFrame) -> ui::Results {
		if(inFrame) ui::end_frame();
		set_hi(&index, buttons, IND_FIND_MISSING);
		return ui::Results::end_early;
	});

	help->set_on_click([&index, &buttons](bool inFrame) -> ui::Results {
		if(inFrame) ui::end_frame();
		set_hi(&index, buttons, IND_HELP); show_help();
		return ui::Results::end_early;
	});

	find_missing->set_on_click([&index, &buttons](bool inFrame) -> ui::Results {
		if(inFrame) ui::end_frame();
		set_hi(&index, buttons, IND_HELP); show_find_missing();
		return ui::Results::end_early;
	});

	hlink->set_on_click([&index, &buttons](bool inFrame) -> ui::Results {
		if(inFrame) ui::end_frame();
		set_hi(&index, buttons, IND_HELP); show_hlink();
		return ui::Results::end_early;
	});

	// Setup indices
	buttons[IND_ABOUT]        = about;
	buttons[IND_FIND_MISSING] = find_missing;
	buttons[IND_HELP]         = help;
	buttons[IND_HLINK]        = hlink;

	ui::Widgets wids;
	wids.push_back(about, ui::Scr::bottom);
	wids.push_back(find_missing, ui::Scr::bottom);
	wids.push_back(help, ui::Scr::bottom);
	wids.push_back(hlink, ui::Scr::bottom);

	ui::Keys keys;
	while(ui::framenext(keys))
	{
		if(!ui::framedraw(wids, keys)) break;
		if(keys.kDown & KEY_DOWN && index < IND_MAX - 1)
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

