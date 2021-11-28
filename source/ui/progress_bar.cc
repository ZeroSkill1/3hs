
#include <ui/progress_bar.hh>

#define FG_COLOR C2D_Color32(0x00, 0xD2, 0x03, 0xFF)
#define BG_COLOR C2D_Color32(0xFF, 0xFF, 0xFF, 0xFF)
#define TEXT_DIM 0.65f
#define X_OFFSET 10
#define Y_OFFSET 30
#define Y_LEN 30


std::string ui::up_to_mib_serialize(u64 n, u64 largest)
{
		if(largest < 1024) return std::to_string(n); /* < 1 KiB */
		if(largest < 1024 * 1024)  return ui::floating_prec<float>((float) n / 1024); /* < 1 MiB */
		else return ui::floating_prec<float>((float) n / 1024 / 1024);
}

std::string ui::up_to_mib_postfix(u64 n)
{
		if(n < 1024) return " B"; /* < 1 KiB */
		if(n < 1024 * 1024) return " KiB"; /* < 1 MiB */
		else return " MiB";
}

/* class ProgressBar */

void ui::ProgressBar::setup(u64 part, u64 total)
{
	this->outerw = ui::screen_width(this->screen) - (X_OFFSET * 2);
	this->buf = C2D_TextBufNew(100); /* probably big enough */
	this->update(part, total);
	this->x = X_OFFSET; /* set a good default */
}

void ui::ProgressBar::setup(u64 total)
{ this->setup(0, total); }

void ui::ProgressBar::setup()
{ this->setup(0, 0); }

void ui::ProgressBar::destroy()
{
	C2D_TextBufDelete(this->buf);
}

bool ui::ProgressBar::render(const ui::Keys& keys)
{
	((void) keys);
	C2D_DrawRectSolid(this->x, this->y, this->z, this->outerw, Y_LEN, BG_COLOR);

	// Overlay actual process
	if(this->w != 0)
		C2D_DrawRectSolid(X_OFFSET + 2, this->y + 2, this->z, this->w, Y_LEN - 4, FG_COLOR);

	if(this->activated)
	{
		C2D_DrawText(&this->a, C2D_WithColor, X_OFFSET, this->y - Y_LEN + 2,
			this->z, TEXT_DIM, TEXT_DIM, C2D_Color32(0xFF, 0xFF, 0xFF, 0xFF));
		C2D_DrawText(&this->bc, C2D_WithColor, this->bcx, this->y - Y_LEN + 2,
			this->z, TEXT_DIM, TEXT_DIM, C2D_Color32(0xFF, 0xFF, 0xFF, 0xFF));
	}

	return true;
}

void ui::ProgressBar::update_state()
{
	float perc = this->total == 0 ? 0.0f : ((float) this->part / this->total);
	this->w = (ui::screen_width(this->screen) - (X_OFFSET * 2) - 4) * perc;

	// (a)   (b/c)
	// 90%    9/10
	// [=========]

	// Parse strings
	std::string bc = this->serialize(this->part, this->total) + "/" + this->serialize(this->total, this->total) + this->postfix(this->total);
	std::string a = floating_prec<float>(perc * 100, 1) + "%";

	C2D_TextBufClear(this->buf);

	C2D_TextParse(&this->bc, this->buf, bc.c_str());
	C2D_TextParse(&this->a, this->buf, a.c_str());

	C2D_TextOptimize(&this->bc);
	C2D_TextOptimize(&this->a);

	// Pad to right
	C2D_TextGetDimensions(&this->bc, TEXT_DIM, TEXT_DIM, &this->bcx, nullptr);
	this->bcx = ui::screen_width(this->screen) - X_OFFSET - this->bcx;
}


void ui::ProgressBar::set_postfix(std::function<std::string(u64)> cb)
{ this->postfix = cb; }

void ui::ProgressBar::set_serialize(std::function<std::string(u64, u64)> cb)
{ this->serialize = cb; }

void ui::ProgressBar::activate()
{ this->activated = true; }

void ui::ProgressBar::update(u64 part, u64 total)
{ this->part = part; this->total = total; this->update_state(); }

void ui::ProgressBar::update(u64 part)
{ this->part = part; this->update_state(); }

float ui::ProgressBar::height()
{ return Y_LEN; }

float ui::ProgressBar::width()
{ return this->outerw; }

