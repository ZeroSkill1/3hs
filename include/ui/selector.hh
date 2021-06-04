
#ifndef inc_ui_selector_hh
#define inc_ui_selector_hh

#include "ui/bindings.hh"
#include "ui/core.hh"

#include <string>
#include <vector>

#ifdef USE_SETTINGS_H
# include "settings.hh"
# define SEL_CLR (get_settings()->isLightMode ? ui::constants::COLOR_BTN_LI : ui::constants::COLOR_BTN)
#else
# define SEL_CLR ui::constants::COLOR_BTN
#endif


namespace ui
{
	namespace constants
	{
		constexpr float SEL_LABEL_HEIGHT = 30.0f;
		constexpr float SEL_LABEL_WIDTH = 200.0f;
		constexpr float SEL_TRI_WIDTH = 30.0f;
		constexpr float SEL_TRI_PAD = 5.0f;
	}

	template <typename TEnum>
	class Selector : public Widget
	{
	public:
		Selector(std::vector<std::string> labels, std::vector<TEnum> values_, TEnum *res_ = nullptr)
			: Widget("selector"), values(values_), res(res_), buf(this->accumul_size(labels))
		{
			for(std::string& label : labels)
				this->labels.emplace_back(this->buf, label);
			for(c2d::Text& txt : this->labels)
				txt.optimize();
		}

		~Selector() { this->buf.free(); }

		ui::Results draw(ui::Keys& keys, ui::Scr target) override
		{
			using namespace constants;

			// The following maths is very long and cursed
			// So it is recommended to do something else

			float btnx1 = (SCREEN_WIDTH(target) - SEL_LABEL_WIDTH) / 2;
			float btny1 = (SCREEN_HEIGHT() - SEL_LABEL_HEIGHT) / 2;

			// Draw box for label..
			C2D_DrawRectSolid(btnx1, btny1, 0, SEL_LABEL_WIDTH, SEL_LABEL_HEIGHT, SEL_CLR);

			// Draw triangles for next/prev...
			C2D_DrawTriangle(btnx1 - SEL_TRI_WIDTH, (SEL_LABEL_HEIGHT / 2) + btny1, SEL_CLR, btnx1 - SEL_TRI_PAD, btny1, SEL_CLR,
				btnx1 - SEL_TRI_PAD, btny1 + SEL_LABEL_HEIGHT, SEL_CLR, 1); // prev

			C2D_DrawTriangle(btnx1 + SEL_LABEL_WIDTH + SEL_TRI_WIDTH, (SEL_LABEL_HEIGHT / 2) + btny1, SEL_CLR, btnx1 + SEL_LABEL_WIDTH + SEL_TRI_PAD, btny1, SEL_CLR,
				btnx1 + SEL_LABEL_WIDTH + SEL_TRI_PAD, btny1 + SEL_LABEL_HEIGHT, SEL_CLR, 1); // next

			// Draw label...
			ui::draw_at_absolute((SEL_LABEL_WIDTH / 2) - (this->labels[this->idx].dimensions(FSIZE, FSIZE).width / 2) + btnx1,
				(SEL_LABEL_HEIGHT / 2) + (btny1 - this->labels[this->idx].dimensions(FSIZE, FSIZE).height / 2), this->labels[this->idx]);

			// Take input...
			if(keys.kDown & KEY_A)
			{
				if(this->res != nullptr)
					*this->res = this->values[this->idx];
				return ui::Results::quit_loop;
			}

			if(keys.kDown & KEY_LEFT)
				this->wrap_minus();

			if(keys.kDown & KEY_RIGHT)
				this->wrap_plus();

			if(keys.kDown & KEY_B)
				return ui::Results::quit_loop;

			return ui::Results::go_on;
		}

		void search_set_idx(TEnum value)
		{
			for(size_t i = 0; i < this->values.size(); ++i)
			{
				if(this->values[i] == value)
				{
					this->idx = i;
					break;
				}
			}
		}


	private:
		std::vector<c2d::Text> labels;
		std::vector<TEnum> values;
		TEnum *res = nullptr;
		c2d::TextBuf buf;
		size_t idx = 0;

		size_t accumul_size(std::vector<std::string>& labels)
		{
			size_t ret = 0;
			for(std::string& label : labels)
			{ ret += label.size() + 1; }
			return ret;
		}

		void wrap_minus()
		{
			if(this->idx > 0) --this->idx;
			else this->idx = this->labels.size() - 1;
		}

		void wrap_plus()
		{
			if(this->idx < this->labels.size() - 1) ++this->idx;
			else this->idx = 0;
		}


	};
}

#undef SEL_CLR
#endif

