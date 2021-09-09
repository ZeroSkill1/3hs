
#ifndef inc_ui_meta_hh
#define inc_ui_meta_hh

#include "ui/scrollingText.hh"
#include "ui/core.hh"

#include "ui/bindings.hh"
#include "hs.hh"


namespace ui
{
	class TitleMeta : public ui::Widget
	{
	public:
		TitleMeta(hs::Title title);
		TitleMeta();
		~TitleMeta();

		ui::Results draw(ui::Keys&, ui::Scr) override;
		void update_title(hs::Title title);


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
		SubMeta(hs::Subcategory sub);
		SubMeta();
		~SubMeta();

		ui::Results draw(ui::Keys&, ui::Scr) override;
		void update_sub(hs::Subcategory sub);


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
		CatMeta(hs::Category cat);
		CatMeta();
		~CatMeta();

		ui::Results draw(ui::Keys&, ui::Scr) override;
		void update_cat(hs::Category cat);


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
}

#endif

