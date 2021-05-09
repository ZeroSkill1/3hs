
#include "ui/progress_bar.hh"
#include "ui/button.hh"
#include "ui/text.hh"

#define X_OFFSET 10
#define Y_OFFSET 30
#define Y_LEN 30

#define BG_COLOR C2D_Color32(0xFF, 0xFF, 0xFF, 0xFF) // #FFFFFF
#define FG_COLOR C2D_Color32(0x00, 0xD2, 0x03, 0xFF) // #00D203


ui::ProgressBar::ProgressBar(u64 part_, u64 total_)
	: Widget("progress_bar"), part(part_), total(total_)
{ this->setup(); }

ui::ProgressBar::ProgressBar(u64 total_)
	: Widget("progress_bar"), total(total_)
{ this->setup(); }

ui::ProgressBar::ProgressBar()
	: Widget("progress_bar")
{ this->setup(); }

ui::ProgressBar::~ProgressBar()
{ ui::wid()->for_each("button", [](ui::Widget *wid) -> void { ((ui::Button *) wid)->toggle_click(); }); }


void ui::ProgressBar::update(u64 part, u64 total)
{ this->part = part; this->total = total; }

void ui::ProgressBar::update(u64 part)
{ this->part = part; }


ui::Results ui::ProgressBar::draw(ui::Keys&, ui::Scr target)
{
	C2D_DrawRectSolid(X_OFFSET, (SCREEN_HEIGHT() / 2) - Y_LEN + Y_OFFSET, 0, SCREEN_WIDTH(target) - (X_OFFSET * 2), Y_LEN, BG_COLOR);

	if(this->total != 0)
	{
		// Overlay actual process
		float perc = ((float) this->part / this->total);
		int width = (SCREEN_WIDTH(target) - (X_OFFSET * 2) - 4) * perc;
		C2D_DrawRectSolid(X_OFFSET + 2, ((SCREEN_HEIGHT() / 2) - Y_LEN + Y_OFFSET) + 2, 0, width, Y_LEN - 4, FG_COLOR);

		if(this->activated)
		{
			// [=========]
			// 90%    9/10
			// (z)   (x/y)

			// Parse into actual string
			this->buf.clear();
			c2d::Text xy(this->buf, this->serialize(this->part, this->total) + "/" + this->serialize(this->total, this->total) + this->postfix(this->total));
			c2d::Text z(this->buf, floating_prec<float>(perc * 100, 1) + "%");

			// Pad to right
			float xyx = SCREEN_WIDTH(target) - X_OFFSET - ui::text_width(xy.handle());

			ui::draw_at_absolute(xyx, (SCREEN_HEIGHT() / 2) - Y_OFFSET + 6, xy);
			ui::draw_at_absolute(X_OFFSET, (SCREEN_HEIGHT() / 2) - Y_OFFSET + 6, z);
		}
	}

	return ui::Results::go_on;
}

void ui::ProgressBar::setup()
{
	ui::wid()->for_each("button", [](ui::Widget *wid) -> void { ((ui::Button *) wid)->toggle_click(); });
}

void ui::ProgressBar::set_postfix(std::function<std::string(u64)> cb)
{ this->postfix = cb; }

void ui::ProgressBar::set_serialize(std::function<std::string(u64, u64)> cb)
{ this->serialize = cb; }

void ui::ProgressBar::set_mib_type()
{ ui::up_to_mib(*this); }

void ui::ProgressBar::activate_text()
{ this->activated = true; }


void ui::up_to_mib(ProgressBar& bar)
{
	bar.set_serialize(ui::up_to_mib_serialize);
	bar.set_postfix(ui::up_to_mib_postfix);
}

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

