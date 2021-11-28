#ifndef inc_widgets_indicators_hh
#define inc_widgets_indicators_hh

#include <ui/base.hh>


namespace ui
{
	class FreeSpaceIndicator : public ui::BaseWidget
	{ UI_WIDGET("FreeSpaceIndicator")
	public:
		void setup();
		bool render(const ui::Keys& keys) override;
		float height() override { return 0.0f; }
		float width() override { return 0.0f; }
		void update();


	private:
		ui::RenderQueue queue;


	};

	class TimeIndicator : public ui::BaseWidget
	{ UI_WIDGET("TimeIndicator")
	public:
		void setup();
		bool render(const ui::Keys& keys) override;
		float height() override { return 0.0f; }
		float width() override { return 0.0f; }
		void update();

		static std::string time(time_t stamp);


	public:
		ui::ScopedWidget<ui::Text> text;
		time_t lastCheck;


	};

	class BatteryIndicator : public ui::BaseWidget
	{ UI_WIDGET("BatteryIndicator")
	public:
		void setup();
		bool render(const ui::Keys& keys) override;
		float height() override { return 0.0f; }
		float width() override { return 0.0f; }
		void update();


	private:
		ui::RenderQueue queue;
		u8 level = 0;


	};

	class NetIndicator : public ui::BaseWidget
	{ UI_WIDGET("NetIndicator")
	public:
		void setup();
		bool render(const ui::Keys& keys) override;
		float height() override { return 0.0f; }
		float width() override { return 0.0f; }
		void update();


	private:
		ui::ScopedWidget<ui::Sprite> sprite;
		s8 status;


	};
}

#endif
