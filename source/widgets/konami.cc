
#include "widgets/konami.hh"
#include "i18n.hh"

#include <3rd/log.hh>

#define KONCODE_SIZE (sizeof(konKeys) / sizeof(size_t))
static size_t konKeys[] = {
	KEY_UP, KEY_UP, KEY_DOWN, KEY_DOWN,
	KEY_LEFT, KEY_RIGHT, KEY_LEFT, KEY_RIGHT,
	KEY_B, KEY_A,
};


bool ui::KonamiListner::render(const ui::Keys& keys)
{
	if(this->currKey == KONCODE_SIZE)
	{
		ui::RenderQueue::global()->render_and_then([this]() -> void {
			this->show_bunny();
		});
		this->currKey = 0;
	}
	else if(keys.kDown & konKeys[this->currKey])
		++this->currKey;
	else if(keys.kDown != 0)
		this->currKey = 0;

	return true;
}

void ui::KonamiListner::show_bunny()
{
	linfo << "User found easter egg :blobaww:";
	ui::RenderQueue queue;

	ui::builder<ui::Sprite>(ui::Screen::top, ui::SpriteStore::get_by_id(ui::sprite::bun))
		.x(ui::layout::center_x).y(ui::layout::center_y)
		.add_to(queue);

	ui::builder<ui::Text>(ui::Screen::bottom, STRING(hs_bunny_found))
		.x(ui::layout::center_x).y(ui::layout::center_y)
		.wrap().add_to(queue);

	queue.render_finite_button(KEY_A | KEY_START);
}

