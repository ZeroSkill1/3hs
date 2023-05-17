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

#ifndef inc_ui_theme_hh
#define inc_ui_theme_hh

#include <unordered_map>
#include <3ds/types.h>
#include <functional>
#include <citro2d.h>
#include <stddef.h>
#include <string>
#include <vector>

#define UI_SLOTS_PROTO_EXTERN(name) \
	extern ui::slot_color_getter *name##__do_not_touch;
#define UI_SLOTS_PROTO(name, count) \
	ui::SlotManager slots = ui::ThemeManager::global()->get_slots(this, id /* id is defined by UI_WIDGET */, count, name##__do_not_touch);
#define UI_SLOTS__3(line, name, ...) \
	static ui::slot_color_getter _slot_data__do_not_touch##line[] = { __VA_ARGS__ }; \
	ui::slot_color_getter *name##__do_not_touch = _slot_data__do_not_touch##line;
#define UI_SLOTS__2(line, name, ...) \
	UI_SLOTS__3(line, name, __VA_ARGS__)
#define UI_SLOTS(name, ...) \
	UI_SLOTS__2(__LINE__, name, __VA_ARGS__)
#define UI_CTHEME_GETTER(symbol, id) \
	static u32 symbol() { return *ui::Theme::global()->get_color(id); }
#define UI_STATIC_SLOTS__3(line, objn, id, ...) \
	static ui::slot_color_getter _static_slot_data__do_not_touch##line[] = { __VA_ARGS__ }; \
	static const ui::StaticSlot objn = { _static_slot_data__do_not_touch##line, id, sizeof(_static_slot_data__do_not_touch##line) / sizeof(ui::slot_color_getter) };
#define UI_STATIC_SLOTS__2(line, objn, id, ...) \
	UI_STATIC_SLOTS__3(line, objn, id, __VA_ARGS__)
#define UI_STATIC_SLOTS(objn, id, ...) \
	UI_STATIC_SLOTS__2(__LINE__, objn, id, __VA_ARGS__)


namespace ui
{
	typedef u32 (*slot_color_getter)();
	class BaseWidget;

	typedef struct ThemeDescriptorImage {
		C2D_Image actual_image;
		bool isOwn;
	} ThemeDescriptorImage;
	typedef struct ThemeDescriptorColor {
		u32 color;
	} ThemeDescriptorColor;

	namespace theme
	{
		enum color_enum {
			background_color,
			text_color,
			button_background_color,
			button_border_color,
			battery_green_color,
			battery_red_color,
			toggle_green_color,
			toggle_red_color,
			toggle_slider_color,
			progress_bar_foreground_color,
			progress_bar_background_color,
			scrollbar_color,
			led_green_color,
			led_red_color,
			smdh_icon_border_color,
			checkbox_border_color,
			checkbox_check_color,
			graph_line_color,
			warning_color,
			x_color,
			battery_charging_color,
			cmax,
			/* don't forget to update max_color when adding a color! */
		};
		enum image_enum {
			more_image,
			battery_image,
			search_image,
			settings_image,
			spinner_image,
			random_image,
			background_top_image,
			background_bottom_image,
			battery_charging_image,
			imax,
		};
	}

	class Theme
	{
	public:
		Theme() { this->clear(); }

		u32 *get_color(u32 descriptor_id)       { return &this->color_descriptors[descriptor_id].color; }
		C2D_Image *get_image(u32 descriptor_id) { return &this->image_descriptors[descriptor_id].actual_image; }

		void cleanup() { this->cleanup_images(); }
		/* clear all references & delete color data but don't free */
		void clear();

		static Theme *global();

		std::string author;
		std::string name;
		std::string id; /* usually file path */

		enum flags_field {
			load_data = 1,
			load_meta = 2,
		};

		bool open(const char *filename, ui::Theme *base, u8 flags = ui::Theme::load_data | ui::Theme::load_meta);
		bool open(const u8 *data, u32 size, const std::string& id, ui::Theme *base, u8 flags = ui::Theme::load_data | ui::Theme::load_meta);
		/* creates a reference to images, copy of colors */
		void replace_without_meta(ui::Theme& other);
		void replace_with(ui::Theme& other);

		bool operator == (const Theme& other)
		{ return this->id == other.id; }

	private:
		ThemeDescriptorColor color_descriptors[theme::cmax];
		ThemeDescriptorImage image_descriptors[theme::imax];
		bool parse(std::function<bool(u8 *, u32)> read_data, size_t size, u8 flags);
		void cleanup_images();

	};

	class SlotManager
	{
	public:
		SlotManager(const u32 *colors)
			: colors(colors) { }
		SlotManager()
			: colors(nullptr) { }
		/* no bounds checking! */
		constexpr u32 get(size_t i) { return this->colors[i]; }
		constexpr u32 operator [] (size_t i) { return this->get(i); }
		constexpr bool is_initialized() { return this->colors != nullptr; }
	private:
		const u32 *colors;
	};

	struct StaticSlot
	{
		const slot_color_getter *getters;
		const char *id;
		size_t count;
	};

	class ThemeManager
	{
	public:
		ui::SlotManager get_slots(BaseWidget *that, const char *id, size_t count, const slot_color_getter *getters);
		void reget(const char *id);
		void reget();

		/* XXX: Is there really not better way to go around this than a pointer vector?
		 *      I feel there must be, but i just don't know how. For now, this will do. */
		void unregister(ui::BaseWidget *w);

		static ui::ThemeManager *global();

		~ThemeManager();


	public:
		struct slot_data {
			const slot_color_getter *getters; /* of size len */
			std::vector<BaseWidget *> slaves;
			u32 *colors; /* of size len */
			size_t len;
		};
		std::unordered_map<std::string, slot_data> slots;


	};
}

#endif

