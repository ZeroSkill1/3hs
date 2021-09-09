
#ifndef inc_widgets_indicators_hh
#define inc_widgets_indicators_hh

#include <ui/bindings.hh>
#include <ui/core.hh>
#include <ui/text.hh>

#include "install.hh"

#define NET_SPRITE_BUF_LEN 4
#define BAT_SPRITE_BUF_LEN 4


Result get_free_space(Destination media, u64 *size);

namespace ui
{
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
}

#endif

