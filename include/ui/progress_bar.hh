/*
 * Copyright (C) 2021-2022 MyPasswordIsWeak
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

#ifndef inc_ui_progress_bar_hh
#define inc_ui_progress_bar_hh

#include <ui/base.hh>

#include <functional>
#include <string>

#include "settings.hh"


namespace ui
{
	static inline ui::Screen progloc()
	{
		return get_settings()->progloc == ProgressBarLocation::bottom
			? ui::Screen::bottom : ui::Screen::top;
	}

	std::string up_to_mib_serialize(u64, u64);
	std::string up_to_mib_postfix(u64);

	class ProgressBar : public ui::BaseWidget
	{ UI_WIDGET("ProgressBar")
	public:
		void setup(u64 part, u64 total);
		void setup(u64 total);
		void setup();

		void destroy();

		bool render(const ui::Keys& keys) override;
		float height() override;
		float width() override;

		void update(u64 part, u64 total);
		void update(u64 part);

		void set_serialize(std::function<std::string(u64, u64)> cb);
		void set_postfix(std::function<std::string(u64)> cb);

		void activate();


	private:
		void update_state();

		bool activated = false;
		float bcx, w, outerw;
		u64 part, total;

		C2D_TextBuf buf;
		C2D_Text bc, a;

		std::function<std::string(u64, u64)> serialize = up_to_mib_serialize;
		std::function<std::string(u64)> postfix = up_to_mib_postfix;


	};
}

#endif

