
#include <ui/menuselect.hh>
#include "panic.hh"


void ui::MenuSelect::setup()
{
	this->w = ui::screen_width(this->screen) - 10.0f;
	this->h = 20.0f;
}

void ui::MenuSelect::connect(ui::MenuSelect::connect_type t, const std::string& s,
	callback_t c)
{
	if(t != ui::MenuSelect::add) panic("EINVAL");
	u32 myi = this->funcs.size();
	this->funcs.push_back(c);
	ui::Button *b = ui::builder<ui::Button>(this->screen, s)
		.connect(ui::Button::click, [this, myi]() -> bool {
			this->btns[this->i]->set_border(false);
			this->btns[myi]->set_border(true);
			this->i = myi;
			this->call_current();
			return true;
		})
		.size(this->w, this->h)
		.x(ui::layout::center_x)
		.finalize();
	if(myi == 0)
		b->set_y(20.0f);
	else
		b->set_y(ui::under(this->q.back(), b, 5.0f));
	this->btns.push_back(b);
	this->q.push(b);
}

float ui::MenuSelect::height()
{
	size_t s = this->funcs.size();
	if(s == 0) return 0.0f;
	return s * this->h + (s - 1) * 5.0f;
}

float ui::MenuSelect::width()
{
	return this->w;
}

bool ui::MenuSelect::render(const ui::Keys& k)
{
	panic_assert(this->funcs.size() != 0, "Empty menuselect");
	this->btns[this->i]->set_border(false);

	if((k.kDown & KEY_UP) && this->i > 0) --this->i;
	if((k.kDown & KEY_DOWN) && this->i < this->btns.size() - 1) ++this->i;
	if(k.kDown & KEY_A) this->call_current();

	this->btns[this->i]->set_border(true);
	this->q.render_screen(k, this->screen);
	return true;
}

void ui::MenuSelect::call_current()
{
	ui::RenderQueue::global()->render_and_then((std::function<bool()>) [this]() -> bool {
		this->funcs[this->i]();
		return true;
	});
}

