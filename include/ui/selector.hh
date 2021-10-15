
#ifndef inc_ui_selector_hh
#define inc_ui_selector_hh

#include <ui/base.hh>

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
		constexpr float SEL_FONTSIZ = 0.65f;
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
			C2D_DrawRectSolid(btnx1, btny1, Z_OFF, SEL_LABEL_WIDTH, SEL_LABEL_HEIGHT, SEL_CLR);

			// Draw triangles for next/prev...
			C2D_DrawTriangle(btnx1 - SEL_TRI_WIDTH, (SEL_LABEL_HEIGHT / 2) + btny1, SEL_CLR, btnx1 - SEL_TRI_PAD, btny1, SEL_CLR,
				btnx1 - SEL_TRI_PAD, btny1 + SEL_LABEL_HEIGHT, SEL_CLR, Z_OFF); // prev

			C2D_DrawTriangle(btnx1 + SEL_LABEL_WIDTH + SEL_TRI_WIDTH, (SEL_LABEL_HEIGHT / 2) + btny1, SEL_CLR, btnx1 + SEL_LABEL_WIDTH + SEL_TRI_PAD, btny1, SEL_CLR,
				btnx1 + SEL_LABEL_WIDTH + SEL_TRI_PAD, btny1 + SEL_LABEL_HEIGHT, SEL_CLR, Z_OFF); // next

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

	namespace next
	{
		template <typename TEnum>
		class Selector : public ui::BaseWidget
		{ UI_WIDGET
		public:
			void setup(const std::vector<std::string>& labels, const std::vector<TEnum>& values, TEnum *res = nullptr)
			{
				this->values = &values;
				this->res = res;

				this->buf = C2D_TextBufNew(this->accumul_size(labels));

				for(const std::string& label : labels)
				{
					C2D_Text text;
					C2D_TextParse(&text, this->buf, label.c_str());
					C2D_TextOptimize(&text);
					this->labels.push_back(text);
				}

				using namespace constants;
				this->x = (ui::screen_width(this->screen) - SEL_LABEL_WIDTH) / 2;
				this->y = (ui::dimensions::height - SEL_LABEL_HEIGHT) / 2;
				this->assign_txty();
			}

			void set_x(float) override { } /* stub */
			void set_y(float) override { } /* stub */

			void destroy() override
			{
				C2D_TextBufClear(this->buf);
			}

			bool render(const ui::Keys& keys) override
			{
				/* TODO: Theme support */
				/* TODO: Make triangles clickable */
				using namespace constants;

				// The following maths is very long and cursed
				// So it is recommended to do something else

				// Draw box for label..
				C2D_DrawRectSolid(this->x, this->y, this->z, SEL_LABEL_WIDTH, SEL_LABEL_HEIGHT, SEL_CLR);

				// Draw triangles for next/prev...
				C2D_DrawTriangle(this->x - SEL_TRI_WIDTH, (SEL_LABEL_HEIGHT / 2) + this->y, SEL_CLR, this->x - SEL_TRI_PAD, this->y, SEL_CLR,
					this->x - SEL_TRI_PAD, this->y + SEL_LABEL_HEIGHT, SEL_CLR, this->z); // prev

				C2D_DrawTriangle(this->x + SEL_LABEL_WIDTH + SEL_TRI_WIDTH, (SEL_LABEL_HEIGHT / 2) + this->y, SEL_CLR, this->x + SEL_LABEL_WIDTH + SEL_TRI_PAD, this->y, SEL_CLR,
					this->x + SEL_LABEL_WIDTH + SEL_TRI_PAD, this->y + SEL_LABEL_HEIGHT, SEL_CLR, this->z); // next

				// Draw label...
				C2D_DrawText(&this->labels[this->idx], C2D_WithColor, this->tx, this->ty, this->z, SEL_FONTSIZ, SEL_FONTSIZ, C2D_Color32(0xFF, 0xFF, 0xFF, 0xFF));

				// Take input...
				if(keys.kDown & KEY_A)
				{
					if(this->res != nullptr)
						*this->res = (*this->values)[this->idx];
					return false;
				}

				if(keys.kDown & KEY_LEFT)
					this->wrap_minus();

				if(keys.kDown & KEY_RIGHT)
					this->wrap_plus();

				if(keys.kDown & KEY_B)
					return false;

				return true;
			}

			void search_set_idx(TEnum value)
			{
				for(size_t i = 0; i < this->values->size(); ++i)
				{
					if((*this->values)[i] == value)
					{
						this->idx = i;
						break;
					}
				}
			}

			float width()
			{
				using namespace constants;
				return (SEL_TRI_WIDTH * 2) + (SEL_TRI_PAD * 2) + SEL_LABEL_WIDTH;
			}

			float height()
			{
				using namespace constants;
				return SEL_LABEL_HEIGHT;
			}


		private:
			const std::vector<TEnum> *values;
			std::vector<C2D_Text> labels;
			TEnum *res = nullptr;
			C2D_TextBuf buf;
			size_t idx = 0;

			float tx, ty;

			void assign_txty()
			{
				using namespace constants;
				float h, w;
				C2D_TextGetDimensions(&this->labels[this->idx], SEL_FONTSIZ, SEL_FONTSIZ, &w, &h);
				this->tx = (SEL_LABEL_WIDTH / 2) - (w / 2) + this->x;
				this->ty = (SEL_LABEL_HEIGHT / 2) - (h / 2) + this->y;
			}

			size_t accumul_size(const std::vector<std::string>& labels)
			{
				size_t ret = 0;
				for(const std::string& label : labels)
				{ ret += label.size() + 1; }
				return ret;
			}

			void wrap_minus()
			{
				if(this->idx > 0) --this->idx;
				else this->idx = this->labels.size() - 1;
				this->assign_txty();
			}

			void wrap_plus()
			{
				if(this->idx < this->labels.size() - 1) ++this->idx;
				else this->idx = 0;
				this->assign_txty();
			}


		};
	}
}

#undef SEL_CLR
#endif

