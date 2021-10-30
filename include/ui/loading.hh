
#ifndef inc_ui_loading_hh
#define inc_ui_loading_hh

#include <ui/sprite.hh>
#include <ui/core.hh>
#include <ui/base.hh>

#include <functional>


namespace ui
{
	/* draw `wids` while running `callback` */
	void loading(ui::Widgets& wids, std::function<void()> callback);
	/* run a loading animation (class Spinner) while running `callback` */
	void loading(std::function<void()> callback);

	class Spinner : public Widget
	{
	public:
		Spinner();

		ui::Results draw(ui::Keys&, ui::Scr) override;


	private:
		ui::StandaloneSprite sprite;


	};

	namespace next
	{
		/* draw `wids` while running `callback` */
		void loading(ui::RenderQueue& queue, std::function<void()> callback);
		/* run a loading animation (class Spinner) while running `callback` */
		void loading(std::function<void()> callback);

		class Spinner : public ui::BaseWidget
		{ UI_WIDGET("Spinner")
		public:
			void setup() override;

			bool render(const ui::Keys&) override;
			float height() override;
			float width() override;

			void set_x(float x) override;
			void set_y(float y) override;
			void set_z(float z) override;


		private:
			ui::ScopedWidget<ui::next::Sprite> sprite;


		};
	}
}

#endif

