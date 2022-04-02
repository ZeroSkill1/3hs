/*
 * Copyright (C) 2021-2022 MyPasswordIsWeak
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

#ifndef inc_i18n_hh
#define inc_i18n_hh

#include <stdint.h>

#include <type_traits>
#include <vector>
#include <string>

#define LANGNAME_ENGLISH "English"
#define LANGNAME_DUTCH "Nederlands"
#define LANGNAME_GERMAN "Deutsch"
#define LANGNAME_SPANISH "Español"

// ParamSTRING
#define PSTRING(x, ...) i18n::interpolate(str::x, __VA_ARGS__)
#define SURESTRING(x) i18n::getsurestr(str::x)
#define STRING(x) i18n::getstr(str::x)

namespace str
{
	enum _enum
	{
		banner  = 0,
		loading = 1,
		luma_not_installed = 2,
		install_luma = 3,
		queue = 4,
		connect_wifi = 5,
		fail_init_networking = 6,
		fail_fetch_index = 7, // %1: error message (in english, leave out in other langs?)
		credits_thanks = 8,
		credits_names = 9,
		press_to_install = 10,
		version = 11,
		prodcode = 12,
		size = 13,
		name = 14,
		tid = 15,
		category = 16,
		landing_id = 17,
		description = 18,
		total_titles = 19,
		select_cat = 20,
		select_subcat = 21,
		select_title = 22,
		no_cats_index = 23,
		empty_subcat = 24,
		empty_cat = 25, // unused
		fmt_24h = 26,
		fmt_12h = 27,
		unknown = 28,
		btrue = 29,
		bfalse = 30,
		top = 31,
		bottom = 32,
		light_mode = 33,
		resume_dl = 34,
		load_space = 35,
		show_battery = 36,
		time_format = 37,
		progbar_screen = 38,
		language = 39,
		value_x = 40, // %1 = x
		back = 41,
		invalid = 42,
		title_doesnt_exist = 43, // %1 = tid
		fail_create_tex = 44,
		fail_load_smdh_icon = 45, // unused
		netcon_lost = 46, // %t = seconds left
		about_app = 47,
		help_manual = 48,
		find_missing_content = 49,
		press_a_exit = 50,
		fatal_panic = 51,
		failed_open_seeddb = 52,
		update_to = 53, // %1 = new version
		search_content = 54,
		search_content_action = 55, // this one has a ... after it
		results_query = 56, // %1 = query
		result_code = 57, // %1 = result code (with 0x)
		level = 58, // %1 = level
		summary = 59, // %1 = summary
		module = 60, // %1 = module
		hs_bunny_found = 61,
		already_installed_reinstall = 62,
		queue_empty = 63,
		cancel = 64,
		confirm = 65,
		invalid_proxy = 66,
		more_about_content = 67,
		lumalocalemode = 68,
		automatic = 69,
		manual = 70,
		disabled = 71,
		patching_reboot = 72,
		reboot_now = 73,
		this_version = 74,
		retry_req = 75,
		search_zero_results = 76,
		credits = 77,
		extra_content = 78,
		check_extra = 79,
		no_req = 80,
		invalid_query = 81,
		min_constraint = 82, // %1 = current version, %2 = minimum version
		proxy = 83,
		none = 84,
		press_a_to_view = 85,
		host = 86,
		port = 87,
		username = 88,
		password = 89,
		clear = 90,
		progbar_screen_desc = 91,
		light_mode_desc = 92,
		resume_dl_desc = 93,
		load_space_desc = 94,
		show_battery_desc = 95,
		time_format_desc = 96,
		language_desc = 97,
		lumalocalemode_desc = 98,
		check_extra_desc = 99,
		proxy_desc = 100,
		install_all = 101,
		install_no_base = 102,
		warn_no_base = 103,
		warn_no_base_desc = 104,
		replaying_errors = 105,
		log = 106,
		upload_logs = 107,
		clear_logs = 108,
		found_missing = 109,
		found_0_missing = 110,
		max_elogs = 111,
		max_elogs_desc = 112,
		elogs_hint = 113,

		_i_max,
	};

	using type = unsigned short int;
}

namespace lang
{
	enum _enum
	{
		english = 0,
		dutch   = 1,
		german  = 2,
		spanish = 3,

		_i_max,
	};

	using type = unsigned short int;
}


namespace i18n
{
	/* you don't have to cache the return of this function */
	const char *getstr(str::type sid, str::type lid);
	const char *getstr(str::type id);

	/* ensure the config exists; more expensive than getstr() */
	const char *getsurestr(str::type id);

	const char *langname(lang::type id);
	lang::type default_lang();

	// from here on out it's template shit

	namespace detail
	{
		template <typename T,
				// Does T have a std::to_string() overload?
				typename = decltype(std::to_string(std::declval<T>()))
			>
		std::string stringify(const T& arg)
		{ return std::to_string(arg); }

		// fallback for `const char *` (technically all arrays) and `std::string`
		template <typename T,
			typename std::enable_if<std::is_array<T>::value
				|| std::is_same<T, std::string>::value, bool>::type * = nullptr
			>
		std::string stringify(const T& arg)
		{ return std::string(arg); }

		template <typename T>
		std::string adv_getstr(str::type id, std::vector<std::string>& substs, const T& head)
		{
			substs.push_back(stringify(head));
			const char *rawstr = getstr(id);
			std::string ret;

			do
			{
				if(*rawstr == '%')
				{
					++rawstr;
					if(*rawstr == '%') // literal "%"
						ret.push_back('%');
					// lets just assume our input/output is always correct...
					else ret += substs[(*rawstr) - '0' - 1];
				}

				else ret.push_back(*rawstr);
			} while(*rawstr++);

			return ret;
		}

		template <typename THead, typename ... TTail>
		std::string adv_getstr(str::type id, std::vector<std::string>& substs,
			const THead& head, const TTail& ... tail)
		{
			substs.push_back(stringify(head));
			return adv_getstr(id, substs, tail...);
		}
	}

	/* you should cache the return of this function */
	template <typename T>
	std::string interpolate(str::type id, const T& head)
	{
		std::vector<std::string> substs;
		return detail::adv_getstr<T>(id, substs, head);
	}

	/* you should cache the return of this function */
	template <typename THead, typename ... TTail>
	std::string interpolate(str::type id, const THead& head, const TTail& ... tail)
	{
		std::vector<std::string> substs = { detail::stringify(head) };
		return detail::adv_getstr(id, substs, tail...);
	}
}

#endif

