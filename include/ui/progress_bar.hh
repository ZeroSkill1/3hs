
#ifndef inc_ui_progress_bar_hh
#define inc_ui_progress_bar_hh

#include <ui/base.hh>

#include <functional>
#include <string>

#include "settings.hh"


namespace ui
{
	static inline ui::Screen progloc()
	{
		return get_settings()->progloc == ProgressBarLocation::bottom
			? ui::Screen::bottom : ui::Screen::top;
	}

	std::string up_to_mib_serialize(u64, u64);
	std::string up_to_mib_postfix(u64);

	class ProgressBar : public ui::BaseWidget
	{ UI_WIDGET("ProgressBar")
	public:
		void setup(u64 part, u64 total);
		void setup(u64 total);
		void setup();

		void destroy();

		bool render(const ui::Keys& keys) override;
		float height() override;
		float width() override;

		void update(u64 part, u64 total);
		void update(u64 part);

		void set_serialize(std::function<std::string(u64, u64)> cb);
		void set_postfix(std::function<std::string(u64)> cb);

		void activate();


	private:
		void update_state();

		bool activated = false;
		float bcx, w, outerw;
		u64 part, total;

		C2D_TextBuf buf;
		C2D_Text bc, a;

		std::function<std::string(u64, u64)> serialize = up_to_mib_serialize;
		std::function<std::string(u64)> postfix = up_to_mib_postfix;


	};
}

#endif

