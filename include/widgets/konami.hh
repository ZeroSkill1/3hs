
#ifndef inc_ui_konami_hh
#define inc_ui_konami_hh

#include <ui/base.hh>


namespace ui
{
	class KonamiListner : public ui::BaseWidget
	{ UI_WIDGET("KonamiListner")
	public:
		bool render(const ui::Keys& keys) override;
		float height() override { return 0.0f; }
		float width() override { return 0.0f; }
		void show_bunny();


	private:
		size_t currKey = 0;


	};
}

#endif
