
#ifndef inc_ui_confirm_hh
#define inc_ui_confirm_hh

#include "ui/button.hh"
#include "ui/core.hh"
#include "ui/text.hh"

#include <ui/base.hh>

#include <string>


namespace ui
{
	class Confirm : public ui::Widget
	{
	public:
		Confirm(std::string label, bool& res);

		ui::Results draw(ui::Keys&, ui::Scr) override;


	private:
		ui::Button yes;
		ui::Button no;
		ui::Text usr;

		bool& returns;


	};

	namespace next
	{
		class Confirm : public ui::BaseWidget
		{ UI_WIDGET("Confirm")
		public:
			void setup(const std::string& label, bool& ret);

			bool render(const ui::Keys&) override;
			float height() override;
			float width() override;

			void set_y(float y) override;


		private:
			ui::RenderQueue queue;
			bool *ret;

			void adjust();


		};
	}
}

#endif

