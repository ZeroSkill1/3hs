
#ifndef inc_ui_loading_hh
#define inc_ui_loading_hh

#include <ui/base.hh>

#include <functional>


namespace ui
{
	namespace detail
	{
		class TimeoutScreenHelper : public ui::BaseWidget
		{ UI_WIDGET("TimeoutScreenHelper")
		public:
			void setup(const std::string& fmt, size_t nsecs, bool *shouldStop = nullptr);
			bool render(const ui::Keys& keys) override;
			float height() override { return 0.0f; }
			float width() override { return 0.0f; }


		private:
			ui::ScopedWidget<ui::Text> text;
			void update_text(time_t now);
			time_t startTime;
			time_t lastCheck;
			bool *shouldStop;
			std::string fmt;
			size_t nsecs;


		};
	}

	/* draw `queue` while running `callback` */
	void loading(ui::RenderQueue& queue, std::function<void()> callback);
	/* run a loading animation (class Spinner) while running `callback` */
	void loading(std::function<void()> callback);
 	/**
	 * use `$t' as a placeholder for the seconds left until the end of the timeout
	 * returns true if the user cancelled the timeout (if allowed)
	 */
	bool timeoutscreen(const std::string& fmt, size_t nsecs, bool allowCancel = true);


	class Spinner : public ui::BaseWidget
	{ UI_WIDGET("Spinner")
	public:
		void setup();

		bool render(const ui::Keys&) override;
		float height() override;
		float width() override;

		void set_x(float x) override;
		void set_y(float y) override;
		void set_z(float z) override;


	private:
		ui::ScopedWidget<ui::Sprite> sprite;


	};
}

#endif

