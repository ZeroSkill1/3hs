
#include "ui/confirm.hh"
#include "i18n.hh"

#define widdraw(w) { ui::Results res; if((res = w.draw(keys,target)) != ui::Results::go_on) return res; }
#define END(r) { this->returns = r; return ui::Results::quit_loop; }


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

/* next */

#undef END
#define END(r) { *this->ret = (r); return false; }


void ui::next::Confirm::setup(const std::string& label, bool& ret)
{
	this->ret = &ret;

	ui::builder<ui::next::Button>(this->screen, STRING(confirm))
		.connect(ui::next::Button::click, [this]() END(true))
		.tag(1)
		.add_to(this->queue);
	ui::builder<ui::next::Button>(this->screen, STRING(cancel))
		.connect(ui::next::Button::click, [this]() END(false))
		.tag(0)
		.add_to(this->queue);
	ui::builder<ui::next::Text>(this->screen, label)
		.x(ui::layout::center_x)
		.y(this->y)
		.tag(2)
		.add_to(this->queue);

	this->adjust();
}

void ui::next::Confirm::adjust()
{
	ui::next::Button *yes = this->queue.find_tag<ui::next::Button>(1);
	ui::next::Button *no = this->queue.find_tag<ui::next::Button>(0);
	ui::next::Text *label = this->queue.find_tag<ui::next::Text>(2);

	label->set_y(this->y);

	float yl = yes->textwidth();
	float nl = no->textwidth();

	const float border = 6.0f;

	float largest = yl > nl ? yl : nl;
	float middle = (ui::screen_width(this->screen) / 2) - (largest / 2);

	yes->set_x(middle - largest / 2 - border);
	yes->set_y(ui::under(label, yes));
	yes->resize(largest + border, 20.0f);

	no->set_x(middle + largest / 2 + border);
	no->set_y(ui::under(label, no));
	no->resize(largest + border, 20.0f);
}

void ui::next::Confirm::set_y(float y)
{
	this->y = y;
	this->adjust();
}

bool ui::next::Confirm::render(const ui::Keys& keys)
{
	if(keys.kDown & (KEY_B | KEY_A))
		END(keys.kDown & KEY_A);

	return this->queue.render_screen(keys, this->screen);
}

float ui::next::Confirm::height()
{
	return 20.0f + this->queue.find_tag(2)->height();
}

float ui::next::Confirm::width()
{
	return (this->queue.back()->width() * 2) + 6.0f;
}

