
#ifndef __ui_progress_bar_hh__
#define __ui_progress_bar_hh__

#include <ui/bindings.hh>
#include <ui/core.hh>

#include <functional>
#include <string>


namespace ui
{
	class ProgressBar : public ui::Widget
	{
	public:
		ProgressBar(u64 part, u64 total);
		ProgressBar(u64 total);
		ProgressBar();

		ui::Results draw(ui::Keys&, ui::Scr) override;

		void update(u64 part, u64 total);
		void update(u64 part);

		void set_serialize(std::function<std::string(u64, u64)> cb);
		void set_postfix(std::function<std::string(u64)> cb);

		void activate_text();
		void set_mib_type();


	private:
		c2d::TextBuf buf = c2d::TextBuf(100);
		u64 part = 0, total = 0;
		bool activated = false;

		std::function<std::string(u64, u64)> serialize = [](u64 n, u64) { return std::to_string(n); };
		std::function<std::string(u64)> postfix = [](u64) { return ""; };


	};

	std::string up_to_mib_serialize(u64, u64);
	std::string up_to_mib_postfix(u64);
	void up_to_mib(ProgressBar& bar);
}

#endif

