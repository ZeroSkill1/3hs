
#ifndef inc_ui_confirm_hh
#define inc_ui_confirm_hh

#include <ui/base.hh>

#include <string>


namespace ui
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

#endif

