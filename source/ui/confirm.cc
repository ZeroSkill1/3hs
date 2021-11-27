
#include "ui/confirm.hh"
#include "i18n.hh"

#define END(r) { *this->ret = (r); return false; }


void ui::Confirm::setup(const std::string& label, bool& ret)
{
	this->ret = &ret;

	ui::builder<ui::Button>(this->screen, STRING(confirm))
		.connect(ui::Button::click, [this]() END(true))
		.tag(1)
		.add_to(this->queue);
	ui::builder<ui::Button>(this->screen, STRING(cancel))
		.connect(ui::Button::click, [this]() END(false))
		.tag(0)
		.add_to(this->queue);
	ui::builder<ui::Text>(this->screen, label)
		.x(ui::layout::center_x)
		.y(this->y)
		.tag(2)
		.wrap()
		.add_to(this->queue);

	this->adjust();
}

void ui::Confirm::adjust()
{
	ui::Button *yes = this->queue.find_tag<ui::Button>(1);
	ui::Button *no = this->queue.find_tag<ui::Button>(0);
	ui::Text *label = this->queue.find_tag<ui::Text>(2);

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

void ui::Confirm::set_y(float y)
{
	this->y = ui::transform(this, y);
	this->adjust();
}

bool ui::Confirm::render(const ui::Keys& keys)
{
	if(keys.kDown & (KEY_B | KEY_A))
		END(keys.kDown & KEY_A);

	return this->queue.render_screen(keys, this->screen);
}

float ui::Confirm::height()
{
	return 20.0f + this->queue.find_tag(2)->height();
}

float ui::Confirm::width()
{
	return (this->queue.back()->width() * 2) + 6.0f;
}

