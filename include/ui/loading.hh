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
			void setup(Result res, size_t nsecs, bool *shouldStop = nullptr);
			bool render(ui::Keys& keys) override;
			float height() override { return 0.0f; }
			float width() override { return 0.0f; }


		private:
			void update_text(time_t now);

			ui::ScopedWidget<ui::Text> text;
			time_t startTime;
			time_t lastCheck;
			bool *shouldStop;
			std::string res;
			size_t nsecs;


		};
	}

	class ProgressBar;
	class LoadingBar
	{
	public:
		LoadingBar(size_t max);

		void reset_to(size_t nmax);
		void update(size_t n = 1);

	private:
		ui::ProgressBar *bar;
		ui::RenderQueue rq;
		size_t cur, max;
	};

	/* run a loading animation (class Spinner) while running `callback`. callback is ran on the same thread as the caller of the function.*/
	void loading(std::function<void()> callback);
 	/**
	 * use `$t' as a placeholder for the seconds left until the end of the timeout
	 * returns true if the user cancelled the timeout (if allowed)
	 */
	bool timeoutscreen(Result res, size_t nsecs, bool allowCancel = true);


	class Spinner : public ui::BaseWidget
	{ UI_WIDGET("Spinner")
	public:
		void setup();

		bool render(ui::Keys&) override;
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

