
#ifndef inc_ui_smdhicon_hh
#define inc_ui_smdhicon_hh

#include <ui/core.hh>

#include "image_ldr.hh"
#include "titles.hh"


namespace ui
{
	class SMDHIcon : public Widget
	{
	public:
		SMDHIcon(TitleSMDH *smdh, SMDHIconType type = SMDHIconType::large);
		SMDHIcon(u64 tid, SMDHIconType type = SMDHIconType::large);
		~SMDHIcon();

		void resize(float w, float h);
		void resize(float dim);

		void center(float x, float y);
		void move(float x, float y);

		ui::Results draw(ui::Keys&, ui::Scr) override;


	private:
		C2D_DrawParams params;
		C2D_Image img;


	};
}

#endif

