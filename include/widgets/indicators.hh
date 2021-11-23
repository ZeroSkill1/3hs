#ifndef inc_widgets_indicators_hh
#define inc_widgets_indicators_hh

#include <ui/base.hh>

#define NET_SPRITE_BUF_LEN 4
#define BAT_SPRITE_BUF_LEN 4


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

#if 0
	class FreeSpaceIndicator : public Widget
	{
	public:
		FreeSpaceIndicator();

		ui::Results draw(Keys&, Scr) override;
		void update();


	private:
		ui::Text sdmc;
		ui::Text nandt;
		ui::Text nandc;


	};

	class NetIndicator : public Widget
	{
	public:
		NetIndicator();

		ui::Results draw(Keys&, Scr) override;


	private:
		c2d::Sprite sprite[NET_SPRITE_BUF_LEN];
		c2d::SpriteSheet sheet;


	};

	class BatteryIndicator : public Widget
	{
	public:
		BatteryIndicator();
		~BatteryIndicator();

		ui::Results draw(Keys&, Scr) override;
		void update();


	private:
		void draw_lvl(u8 lvl);

		c2d::SpriteSheet sheet;
		c2d::Sprite light;
		c2d::Sprite dark;

		ui::Text percentage;
		u8 level = 0;


	};

	class TimeIndicator : public Widget
	{
	public:
		TimeIndicator();

		ui::Results draw(Keys&, Scr) override;
		static std::string time();

	public:
		ui::Text txt;


	};
#endif
}

#endif
