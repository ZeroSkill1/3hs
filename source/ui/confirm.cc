
#include "ui/confirm.hh"

#define widdraw(w) { ui::Results res; if((res = w.draw(keys,target)) != ui::Results::go_on) return res; }
#define END(r) { this->returns = r; return ui::Results::quit_loop; }
#define fs ui::constants::FSIZE


ui::Confirm::Confirm(std::string label, bool& res)
	: Widget("confirm"),
		yes(
			CONFIRM_YES, 75,
			SCREEN_HEIGHT() / 2 - 10, 150,
			SCREEN_HEIGHT() / 2 + 10
		),
		no(
			CONFIRM_NO, 175,
			SCREEN_HEIGHT() / 2 - 10, 250,
			SCREEN_HEIGHT() / 2 + 10
		),
		usr(ui::mk_center_WText(label, SCREEN_HEIGHT() / 2 - 40, constants::FSIZE, constants::FSIZE,
			ui::Scr::bottom)),
		returns(res)
{
	this->yes.set_on_click([&]() END(true));
	this->no.set_on_click([&]() END(false));
}

ui::Results ui::Confirm::draw(ui::Keys& keys, ui::Scr target)
{
	if(keys.kDown & KEY_A)
		END(true);
	if(keys.kDown & KEY_B)
		END(false);

	widdraw(this->usr);
	widdraw(this->yes);
	widdraw(this->no);
	return ui::Results::go_on;
}

