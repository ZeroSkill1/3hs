
#include <ui/theme.hh>
#include <ui/base.hh>
#include <algorithm>

#include "panic.hh"
#include "log.hh"


static ui::ThemeManager manager;

#define fill_colors(s) fill_colors_((s).colors, (s).getters, (s).len)
static void fill_colors_(u32 *colors, const ui::slot_color_getter *getters, size_t len)
{
	for(size_t i = 0; i < len; ++i)
		colors[i] = getters[i]();
}

ui::ThemeManager::~ThemeManager()
{
	for(auto it : this->slots)
		free(it.second.colors);
}

ui::ThemeManager *ui::ThemeManager::global()
{ return &manager; }

ui::SlotManager ui::ThemeManager::get_slots(ui::BaseWidget *that, const char *id, size_t count, const slot_color_getter *getters)
{
	auto it = this->slots.find(id);
	if(it != this->slots.end())
	{
		if(that && that->supports_theme_hook())
			it->second.slaves.push_back(that);
		return ui::SlotManager(it->second.colors);
	}

	slot_data slot;
	slot.colors = (u32 *) malloc(sizeof(u32) * count);
	slot.getters = getters;
	if(that && that->supports_theme_hook())
		slot.slaves = { that };
	slot.len = count;
	fill_colors(slot);

	this->slots[id] = slot;
	this->slots[id].slaves.reserve(10);
	return ui::SlotManager(slot.colors);
}

void ui::ThemeManager::reget(const char *id)
{
	auto it = this->slots.find(id);
	if(it == this->slots.end())
		panic(std::string(id) + ": not found");
	fill_colors(it->second);
	for(ui::BaseWidget *w : it->second.slaves)
		w->update_theme_hook();
}

void ui::ThemeManager::reget()
{
	for(auto& it : this->slots)
	{
		fill_colors(it.second);
		for(size_t i = 0; i < it.second.slaves.size(); ++i)
			it.second.slaves[i]->update_theme_hook();
	}
}

void ui::ThemeManager::unregister(ui::BaseWidget *w)
{
	for(auto& it : this->slots)
	{
		for(size_t i = 0; i < it.second.slaves.size(); ++i)
		{
			if(it.second.slaves[i] == w)
			{
				it.second.slaves.erase(it.second.slaves.begin() + i);
				return;
			}
		}
	}
}

