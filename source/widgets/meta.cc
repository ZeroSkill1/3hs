
#include "widgets/meta.hh"

#define T_NAME "name          "
#define T_CAT  "category      "
#define T_TID  "title id      "
#define T_ID   "landing id    "
#define T_SIZE "size          "

#define T_LEN ((14*5)+1)
#define T_LONG this->cid

#define S_NAME  "name          "
#define S_CAT   "category      "
#define S_SIZE  "size          "
#define S_TITLE "total titles  "
#define S_DESC  "description   "

#define S_LEN ((14*5)+1)
#define S_LONG this->cdesc

#define C_NAME  "name          "
#define C_SIZE  "size          "
#define C_TITLE "total titles  "
#define C_DESC  "description   "

#define C_LEN ((14*4)+1)
#define C_LONG this->cdesc

#define twidth(n) (n.dimensions(tlen,tlen).width+moff)
#define tlen ui::constants::FSIZE
#define moff 9

#define sdraw(n,o) \
	ui::draw_at_absolute(moff, GRID_AL_Y(o), this->c##n); \
	this->s##n.draw(keys,target)

#define scroll_if_large(n,l) \
	if(this->s##n.length(tlen, tlen) + twidth(l) > SCREEN_WIDTH(ui::Scr::bottom)) \
		this->s##n.start_scroll()

#define cnr(n,l) \
	this->s##n.stop_scroll(); \
	scroll_if_large(n,l)

static hs::Index *g_index;
void ui::setup_meta(hs::Index *index)
{ g_index = index; }

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
	/* Name: x */ sdraw(name, 1);
	/* Category: x -> y */ sdraw(cat, 2);
	/* Title id: x */ sdraw(tid, 3);
	/* Landing id: x */ sdraw(id, 4);
	/* Size: x */ sdraw(size, 5);

	return ui::Results::go_on;
}

void ui::TitleMeta::update_title(hs::Title title)
{
	this->scat.replace_text((*g_index)[title.cat]->displayName + " -> " + (*(*g_index)[title.cat])[title.subcat]->displayName);
	this->ssize.replace_text(ui::human_readable_size_block(title.size));
	this->sid.replace_text(std::to_string(title.id));
	this->sname.replace_text(title.name);
	this->stid.replace_text(title.tid);

	cnr(name, T_LONG);
	cnr(cat, T_LONG);
}

void ui::TitleMeta::init_other()
{
	this->cbuf.realloc(T_LEN);

	this->cname.parse(this->cbuf, T_NAME);
	this->cname.optimize();

	this->ccat.parse(this->cbuf, T_CAT);
	this->ccat.optimize();

	this->ctid.parse(this->cbuf, T_TID);
	this->ctid.optimize();

	this->cid.parse(this->cbuf, T_ID);
	this->cid.optimize();

	this->csize.parse(this->cbuf, T_SIZE);
	this->csize.optimize();

	this->sname.move(twidth(T_LONG), GRID_AL_Y(1));
	this->scat.move(twidth(T_LONG), GRID_AL_Y(2));
	this->stid.move(twidth(T_LONG), GRID_AL_Y(3));
	this->sid.move(twidth(T_LONG), GRID_AL_Y(4));
	this->ssize.move(twidth(T_LONG), GRID_AL_Y(5));
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
	/* Name: x */ sdraw(name, 1);
	/* Category: x */ sdraw(cat, 2);
	/* Size: x */ sdraw(size, 3);
	/* Total titles: x */ sdraw(title, 4);
	/* Description: x */ sdraw(desc, 5);

	return ui::Results::go_on;
}

void ui::SubMeta::update_sub(hs::Subcategory sub)
{
	this->ssize.replace_text(ui::human_readable_size(sub.size));
	this->stitle.replace_text(std::to_string(sub.totalTitles));
	this->scat.replace_text((*g_index)[sub.cat]->displayName);
	this->sname.replace_text(sub.displayName);
	this->sdesc.replace_text(sub.desc);

	cnr(desc, S_LONG);
	cnr(cat, S_LONG);
}

void ui::SubMeta::init_other()
{
	this->cbuf.realloc(S_LEN);

	this->cname.parse(this->cbuf, S_NAME);
	this->cname.optimize();

	this->ccat.parse(this->cbuf, S_CAT);
	this->ccat.optimize();

	this->csize.parse(this->cbuf, S_SIZE);
	this->csize.optimize();

	this->ctitle.parse(this->cbuf, S_TITLE);
	this->ctitle.optimize();

	this->cdesc.parse(this->cbuf, S_DESC);
	this->cdesc.optimize();

	this->sname.move(twidth(S_LONG), GRID_AL_Y(1));
	this->scat.move(twidth(S_LONG), GRID_AL_Y(2));
	this->ssize.move(twidth(S_LONG), GRID_AL_Y(3));
	this->stitle.move(twidth(S_LONG), GRID_AL_Y(4));
	this->sdesc.move(twidth(S_LONG), GRID_AL_Y(5));
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
	/* Name: x */ sdraw(name, 1);
	/* Size: x */ sdraw(size, 2);
	/* Total titles: x */ sdraw(title, 3);
	/* Description: x */ sdraw(desc, 4);

	return ui::Results::go_on;
}

void ui::CatMeta::update_cat(hs::Category cat)
{
	this->ssize.replace_text(ui::human_readable_size(cat.size));
	this->stitle.replace_text(std::to_string(cat.totalTitles));
	this->sname.replace_text(cat.displayName);
	this->sdesc.replace_text(cat.desc);

	cnr(desc, C_LONG);
}

void ui::CatMeta::init_other()
{
	this->cbuf.realloc(C_LEN);

	this->cname.parse(this->cbuf, C_NAME);
	this->cname.optimize();

	this->csize.parse(this->cbuf, C_SIZE);
	this->csize.optimize();

	this->ctitle.parse(this->cbuf, C_TITLE);
	this->ctitle.optimize();

	this->cdesc.parse(this->cbuf, C_DESC);
	this->cdesc.optimize();

	this->sname.move(twidth(C_LONG), GRID_AL_Y(1));
	this->ssize.move(twidth(C_LONG), GRID_AL_Y(2));
	this->stitle.move(twidth(C_LONG), GRID_AL_Y(3));
	this->sdesc.move(twidth(C_LONG), GRID_AL_Y(4));
}
