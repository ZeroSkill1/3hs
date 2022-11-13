/* This file is part of 3hs
 * Copyright (C) 2021-2022 hShop developer team
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef inc_ui_smdhicon_hh
#define inc_ui_smdhicon_hh

#include <ui/base.hh>

#include "image_ldr.hh"


namespace ui
{
	UI_SLOTS_PROTO_EXTERN(SMDHIcon_color)
	class SMDHIcon : public ui::BaseWidget
	{ UI_WIDGET("SMDHIcon")
	public:
		void setup(ctr::TitleSMDH *smdh, SMDHIconType type = SMDHIconType::large);
		void setup(u64 tid, SMDHIconType type = SMDHIconType::large);
		void destroy() override;

		bool render(ui::Keys& keys) override;
		float height() override;
		float width() override;

		void set_x(float x) override;
		void set_y(float y) override;
		void set_z(float z) override;

		void set_border(bool b);


	private:
		UI_SLOTS_PROTO(SMDHIcon_color, 1)
		bool drawBorder = false;
		C2D_DrawParams params;
		C2D_Image img;


	};
}

#endif

