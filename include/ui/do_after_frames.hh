
#ifndef __ui_do_after_frames_hh__
#define __ui_do_after_frames_hh__

#include "ui/core.hh"

#include <functional>


namespace ui
{
	class DoAfterFrames : public Widget
	{
	public:
		DoAfterFrames(size_t frames, std::function<ui::Results()> cb)
			: Widget("do_after_frames"), on_done(cb), tFrames(frames)
		{ }

		ui::Results draw(ui::Keys&, ui::Scr) override
		{
			if(this->dFrames > this->tFrames) return ui::Results::go_on;
			ui::Results ret = ui::Results::go_on;
			if(this->dFrames == this->tFrames) ret = this->on_done();
			++this->dFrames; return ret;
		}


	private:
		std::function<ui::Results()> on_done;
		size_t tFrames = 0;
		size_t dFrames = 0;


	};
}

#endif

