
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

#define T_S_LEN ((14*5)+1)
#define C_LEN ((14*4)+1)

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

static hs::Index *g_index;
void ui::setup_meta(hs::Index *index)
{ g_index = index; }

hs::Index *ui::get_index()
{ return g_index; }

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
	this->scat.replace_text((*g_index)[title.cat]->displayName + " -> " + (*(*g_index)[title.cat])[title.subcat]->displayName);
	this->ssize.replace_text(ui::human_readable_size_block(title.size));
	this->sid.replace_text(std::to_string(title.id));
	this->sname.replace_text(title.name);
	this->stid.replace_text(title.tid);

	cnr(name);
	cnr(cat);
}

void ui::TitleMeta::init_other()
{
	this->cbuf.realloc(T_S_LEN);

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
	this->scat.replace_text((*g_index)[sub.cat]->displayName);
	this->sname.replace_text(sub.displayName);
	this->sdesc.replace_text(sub.desc);

	cnr(desc);
	cnr(cat);
}

void ui::SubMeta::init_other()
{
	this->cbuf.realloc(T_S_LEN);

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
	this->cbuf.realloc(C_LEN);

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
