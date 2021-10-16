
#include "widgets/meta.hh"
#include "i18n.hh"

// Sorry for this macro mess, even i find it hard to read now.
// D = Description, T = Title, C = Category, S = Subcategory

#define D_TTL_TITLE STRING(total_titles)
#define D_DESC      STRING(description)
#define D_LID       STRING(landing_id)
#define D_CAT       STRING(category)
#define D_TID       STRING(tid)
#define D_NAME      STRING(name)
#define D_SIZE      STRING(size)

#define BUF_LEN 1000

#define twidth(n) (n.dimensions(tlen,tlen).width+moff)
#define tlen ui::constants::FSIZE
#define moff 9

#define sdraw(n,o) \
	ui::draw_at_absolute(moff, GRID_AL_Y(o), this->c##n, 0, 0.45f, 0.45f); \
	this->s##n.draw(keys,target)

#define scroll_if_large(n) \
	this->s##n.scroll_if_overflow(ui::Scr::bottom)

#define cnr(n) \
	this->s##n.stop_scroll(); \
	scroll_if_large(n)


ui::TitleMeta::TitleMeta(hs::Title title)
	: Widget("tmd")
{
	this->update_title(title);
	this->init_other();
}

ui::TitleMeta::TitleMeta()
	: Widget("tmd")
{
	this->init_other();
}

ui::TitleMeta::~TitleMeta()
{
	this->cbuf.free();
}

ui::Results ui::TitleMeta::draw(ui::Keys& keys, ui::Scr target)
{
	/* Name: x */ sdraw(name, 2);
	/* Category: x -> y */ sdraw(cat, 4);
	/* Title id: x */ sdraw(tid, 6);
	/* Landing id: x */ sdraw(id, 8);
	/* Size: x */ sdraw(size, 10);

	return ui::Results::go_on;
}

void ui::TitleMeta::update_title(hs::Title title)
{
	this->scat.replace_text((*hs::get_index())[title.cat]->displayName + " -> " + (*(*hs::get_index())[title.cat])[title.subcat]->displayName);
	this->ssize.replace_text(ui::human_readable_size_block(title.size));
	this->sid.replace_text(std::to_string(title.id));
	this->sname.replace_text(title.name);
	this->stid.replace_text(title.tid);

	cnr(name);
	cnr(cat);
}

void ui::TitleMeta::init_other()
{
	this->cbuf.realloc(BUF_LEN);

	this->cname.parse(this->cbuf, D_NAME);
	this->cname.optimize();

	this->ccat.parse(this->cbuf, D_CAT);
	this->ccat.optimize();

	this->ctid.parse(this->cbuf, D_TID);
	this->ctid.optimize();

	this->cid.parse(this->cbuf, D_LID);
	this->cid.optimize();

	this->csize.parse(this->cbuf, D_SIZE);
	this->csize.optimize();

	this->sname.move(moff, GRID_AL_Y(1));
	this->scat.move(moff, GRID_AL_Y(3));
	this->stid.move(moff, GRID_AL_Y(5));
	this->sid.move(moff, GRID_AL_Y(7));
	this->ssize.move(moff, GRID_AL_Y(9));
}


ui::SubMeta::SubMeta(hs::Subcategory sub)
	: Widget("smd")
{
	this->update_sub(sub);
	this->init_other();
}

ui::SubMeta::SubMeta()
	: Widget("smd")
{
	this->init_other();
}

ui::SubMeta::~SubMeta()
{
	this->cbuf.free();
}

ui::Results ui::SubMeta::draw(ui::Keys& keys, ui::Scr target)
{
	/* Name: x */ sdraw(name, 2);
	/* Category: x */ sdraw(cat, 4);
	/* Size: x */ sdraw(size, 6);
	/* Total titles: x */ sdraw(title, 8);
	/* Description: x */ sdraw(desc, 10);

	return ui::Results::go_on;
}

void ui::SubMeta::update_sub(hs::Subcategory sub)
{
	this->ssize.replace_text(ui::human_readable_size(sub.size));
	this->stitle.replace_text(std::to_string(sub.totalTitles));
	this->scat.replace_text((*hs::get_index())[sub.cat]->displayName);
	this->sname.replace_text(sub.displayName);
	this->sdesc.replace_text(sub.desc);

	cnr(desc);
	cnr(cat);
}

void ui::SubMeta::init_other()
{
	this->cbuf.realloc(BUF_LEN);

	this->cname.parse(this->cbuf, D_NAME);
	this->cname.optimize();

	this->ccat.parse(this->cbuf, D_CAT);
	this->ccat.optimize();

	this->csize.parse(this->cbuf, D_SIZE);
	this->csize.optimize();

	this->ctitle.parse(this->cbuf, D_TTL_TITLE);
	this->ctitle.optimize();

	this->cdesc.parse(this->cbuf, D_DESC);
	this->cdesc.optimize();

	this->sname.move(moff, GRID_AL_Y(1));
	this->scat.move(moff, GRID_AL_Y(3));
	this->ssize.move(moff, GRID_AL_Y(5));
	this->stitle.move(moff, GRID_AL_Y(7));
	this->sdesc.move(moff, GRID_AL_Y(9));
}


ui::CatMeta::CatMeta(hs::Category cat)
	: Widget("cmd")
{
	this->update_cat(cat);
	this->init_other();
}

ui::CatMeta::CatMeta()
	: Widget("cmd")
{
	this->init_other();
}

ui::CatMeta::~CatMeta()
{
	this->cbuf.free();
}

ui::Results ui::CatMeta::draw(ui::Keys& keys, ui::Scr target)
{
	/* Name: x */ sdraw(name, 2);
	/* Size: x */ sdraw(size, 4);
	/* Total titles: x */ sdraw(title, 6);
	/* Description: x */ sdraw(desc, 8);

	return ui::Results::go_on;
}

void ui::CatMeta::update_cat(hs::Category cat)
{
	this->ssize.replace_text(ui::human_readable_size(cat.size));
	this->stitle.replace_text(std::to_string(cat.totalTitles));
	this->sname.replace_text(cat.displayName);
	this->sdesc.replace_text(cat.desc);

	cnr(desc);
}

void ui::CatMeta::init_other()
{
	this->cbuf.realloc(BUF_LEN);

	this->cname.parse(this->cbuf, D_NAME);
	this->cname.optimize();

	this->csize.parse(this->cbuf, D_SIZE);
	this->csize.optimize();

	this->ctitle.parse(this->cbuf, D_TTL_TITLE);
	this->ctitle.optimize();

	this->cdesc.parse(this->cbuf, D_DESC);
	this->cdesc.optimize();

	this->sname.move(moff, GRID_AL_Y(1));
	this->ssize.move(moff, GRID_AL_Y(3));
	this->stitle.move(moff, GRID_AL_Y(5));
	this->sdesc.move(moff, GRID_AL_Y(7));
}

/* next */

void ui::next::TitleMeta::setup(const hs::Title& meta)
{ this->set_title(meta); }

void ui::next::TitleMeta::set_title(const hs::Title& meta)
{
	this->queue.clear();

	ui::builder<ui::next::Text>(this->screen, meta.name)
		.x(this->get_x())
		.y(this->get_y())
		.scroll()
		.add_to(this->queue);
	ui::builder<ui::next::Text>(this->screen, STRING(name))
		.size(0.45f, 0.45f)
		.x(this->get_x())
		.under(this->queue.back(), -1.0f)
		.add_to(this->queue);

	ui::builder<ui::next::Text>(this->screen, (*hs::get_index())[meta.cat]->displayName + " -> " + (*(*hs::get_index())[meta.cat])[meta.subcat]->displayName)
		.x(this->get_x())
		.under(this->queue.back(), 1.0f)
		.scroll()
		.add_to(this->queue);
	ui::builder<ui::next::Text>(this->screen, STRING(category))
		.size(0.45f, 0.45f)
		.x(this->get_x())
		.under(this->queue.back(), -1.0f)
		.add_to(this->queue);

	ui::builder<ui::next::Text>(this->screen, meta.tid)
		.x(this->get_x())
		.under(this->queue.back(), 1.0f)
		.scroll()
		.add_to(this->queue);
	ui::builder<ui::next::Text>(this->screen, STRING(tid))
		.size(0.45f, 0.45f)
		.x(this->get_x())
		.under(this->queue.back(), -1.0f)
		.add_to(this->queue);

	ui::builder<ui::next::Text>(this->screen, std::to_string(meta.id))
		.x(this->get_x())
		.under(this->queue.back(), 1.0f)
		.scroll()
		.add_to(this->queue);
	ui::builder<ui::next::Text>(this->screen, STRING(landing_id))
		.size(0.45f, 0.45f)
		.x(this->get_x())
		.under(this->queue.back(), -1.0f)
		.add_to(this->queue);

	ui::builder<ui::next::Text>(this->screen, ui::human_readable_size_block(meta.size))
		.x(this->get_x())
		.under(this->queue.back(), 1.0f)
		.scroll()
		.add_to(this->queue);
	ui::builder<ui::next::Text>(this->screen, STRING(size))
		.size(0.45f, 0.45f)
		.x(this->get_x())
		.under(this->queue.back(), -1.0f)
		.add_to(this->queue);
}

bool ui::next::TitleMeta::render(const ui::Keys& keys)
{
	((void) keys); // unused
	return this->queue.render_screen(keys, this->screen);
}

float ui::next::TitleMeta::width()
{
	return 0.0f; // unsupported
}

float ui::next::TitleMeta::height()
{
	return this->queue.back()->get_y() + this->get_y();
}

float ui::next::TitleMeta::get_y()
{ return 10.0f; }

float ui::next::TitleMeta::get_x()
{ return 10.0f; }

