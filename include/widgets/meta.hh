
#ifndef inc_ui_meta_hh
#define inc_ui_meta_hh

#include "ui/scrollingText.hh"
#include "ui/core.hh"

#include "ui/bindings.hh"
#include "hsapi.hh"

#include <ui/base.hh>


namespace ui
{
	class TitleMeta : public ui::Widget
	{
	public:
		TitleMeta(const hsapi::Title& title);
		TitleMeta();
		~TitleMeta();

		ui::Results draw(ui::Keys&, ui::Scr) override;
		void update_title(const hsapi::Title& title);


	private:
		void init_other();

		c2d::TextBuf cbuf;
		c2d::Text cname;
		c2d::Text csize;
		c2d::Text ccat;
		c2d::Text ctid;
		c2d::Text cid;

		ui::ScrollingText sname;
		ui::ScrollingText ssize;
		ui::ScrollingText scat;
		ui::ScrollingText stid;
		ui::ScrollingText sid;


	};

	class SubMeta : public ui::Widget
	{
	public:
		SubMeta(const hsapi::Subcategory& sub);
		SubMeta();
		~SubMeta();

		ui::Results draw(ui::Keys&, ui::Scr) override;
		void update_sub(const hsapi::Subcategory& sub);


	private:
		void init_other();

		c2d::TextBuf cbuf;
		c2d::Text ctitle;
		c2d::Text cname;
		c2d::Text csize;
		c2d::Text cdesc;
		c2d::Text ccat;

		ui::ScrollingText stitle;
		ui::ScrollingText sname;
		ui::ScrollingText ssize;
		ui::ScrollingText sdesc;
		ui::ScrollingText scat;


	};

	class CatMeta : public ui::Widget
	{
	public:
		CatMeta(const hsapi::Category& cat);
		CatMeta();
		~CatMeta();

		ui::Results draw(ui::Keys&, ui::Scr) override;
		void update_cat(const hsapi::Category& cat);


	private:
		void init_other();

		c2d::TextBuf cbuf;
		c2d::Text ctitle;
		c2d::Text cname;
		c2d::Text csize;
		c2d::Text cdesc;

		ui::ScrollingText stitle;
		ui::ScrollingText sname;
		ui::ScrollingText ssize;
		ui::ScrollingText sdesc;


	};

	namespace next
	{
		class TitleMeta : public ui::BaseWidget
		{ UI_WIDGET
		public:
			void setup(const hsapi::Title& meta);

			void set_title(const hsapi::Title& meta);

			float get_x() override;
			float get_y() override;

			bool render(const ui::Keys& keys) override;
			float height() override;
			float width() override;


		private:
			ui::RenderQueue queue;


		};
	}
}

#endif

