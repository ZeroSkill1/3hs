
#ifndef __widgets_indicators_hh__
#define __widgets_indicators_hh__

#include <ui/core.hh>
#include <ui/text.hh>

#define NET_SPRITE_BUF_LEN 4


enum Destination
{
	DEST_CTRNand, DEST_TWLNand,
	DEST_Sdmc,
};


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

		ui::Results draw(Keys&, Scr) override;
		void update();


	private:
		ui::Text percentage;
		u8 level;


	};
}

#endif

