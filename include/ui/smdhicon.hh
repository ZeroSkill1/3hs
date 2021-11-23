
#ifndef inc_ui_smdhicon_hh
#define inc_ui_smdhicon_hh

#include <ui/base.hh>

#include "image_ldr.hh"


namespace ui
{
	class SMDHIcon : public ui::BaseWidget
	{ UI_WIDGET("SMDHIcon")
	public:
		void setup(ctr::TitleSMDH *smdh, SMDHIconType type = SMDHIconType::large);
		void setup(u64 tid, SMDHIconType type = SMDHIconType::large);
		void destroy() override;

		bool render(const ui::Keys& keys) override;
		float height() override;
		float width() override;

		void set_x(float x) override;
		void set_y(float y) override;
		void set_z(float z) override;

		void set_border(bool b);


	private:
		bool drawBorder = false;
		C2D_DrawParams params;
		C2D_Image img;


	};
}

#endif

