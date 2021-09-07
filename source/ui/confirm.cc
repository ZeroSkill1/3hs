
#include "ui/confirm.hh"
#include "i18n.hh"

#define widdraw(w) { ui::Results res; if((res = w.draw(keys,target)) != ui::Results::go_on) return res; }
#define END(r) { this->returns = r; return ui::Results::quit_loop; }

#include "panic.hh"
ui::Confirm::Confirm(std::string label, bool& res)
	: Widget("confirm"),
		yes(
			STRING(confirm), 0, 0, 0, 0 // configured later
		),
		no(
			STRING(cancel), 0, 0, 0, 0 // configured later
		),
		usr(ui::mk_center_WText(label, SCREEN_HEIGHT() / 2 - 40,
			constants::FSIZE, constants::FSIZE, ui::Scr::bottom)),
		returns(res)
{
	this->force_screen(ui::Scr::bottom);

	this->yes.set_on_click([&](bool) END(true));
	this->no.set_on_click([&](bool) END(false));

	// Complicated configure for translations :yeehad:

	float yl = this->yes.textlen();
	float nl = this->no.textlen();

	const float scrcenter = SCREEN_WIDTH(ui::Scr::bottom) / 2;
	const float h1 = SCREEN_HEIGHT() / 2 - 10;
	const float h2 = SCREEN_HEIGHT() / 2 + 10;
	const float border = 3.0f;

	float largest = (yl > nl ? yl : nl) + border;

	this->yes.move(
		scrcenter - largest - border, h1,
		scrcenter - border, h2
	);

	this->no.move(
		scrcenter + largest + border, h2,
		scrcenter + border, h1
	);
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

