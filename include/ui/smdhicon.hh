
#ifndef inc_ui_smdhicon_hh
#define inc_ui_smdhicon_hh

#include <ui/base.hh>
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

	namespace next
	{
		class SMDHIcon : public ui::BaseWidget
		{ UI_WIDGET
		public:
			void setup(TitleSMDH *smdh, SMDHIconType type = SMDHIconType::large);
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
}

#endif

