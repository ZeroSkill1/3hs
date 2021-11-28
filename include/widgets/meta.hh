
#ifndef inc_ui_meta_hh
#define inc_ui_meta_hh

#include <ui/base.hh>
#include "hsapi.hh"


namespace ui
{
	class CatMeta : public ui::BaseWidget
	{ UI_WIDGET("CatMeta")
	public:
		void setup(const hsapi::Category& cat);

		void set_cat(const hsapi::Category& cat);

		float get_x() override;
		float get_y() override;

		bool render(const ui::Keys& keys) override;
		float height() override;
		float width() override;


	private:
		ui::RenderQueue queue;


	};

	class SubMeta : public ui::BaseWidget
	{ UI_WIDGET("SubMeta")
	public:
		void setup(const hsapi::Subcategory& sub);

		void set_sub(const hsapi::Subcategory& sub);

		float get_x() override;
		float get_y() override;

		bool render(const ui::Keys& keys) override;
		float height() override;
		float width() override;


	private:
		ui::RenderQueue queue;


	};

	class TitleMeta : public ui::BaseWidget
	{ UI_WIDGET("TitleMeta")
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

#endif

