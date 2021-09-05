
#ifndef inc_i18n_hh
#define inc_i18n_hh

#include <stdint.h>

#include <type_traits>
#include <vector>
#include <string>

#define LANGNAME_ENGLISH "English"
#define LANGNAME_DUTCH "Nederlands"
#define LANGNAME_GERMAN "Deutsch"

namespace str
{
	enum _enum
	{
		banner  = 0,
		loading = 1,

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

		_i_max,
	};

	using type = unsigned short int;
}


namespace i18n
{
	/* you don't have to cache the return of this function */
	const char *getstr(str::type id);

	const char *langname(lang::type id);
	lang::type default_lang();

	// from here on out it's template shit

	namespace detail
	{
		template <typename T,
				// T == std::string
				typename std::enable_if<std::is_same<T, std::string>::value, int>::type = 0
			>
		std::string stringify(const T& arg)
		{ return arg; }

		template <typename T,
				// Does T have a std::to_string() overload?
				typename = decltype(std::to_string(std::declval<T>()))
			>
		std::string stringify(const T& arg)
		{ return std::to_string(arg); }

		// fallback for for example `const char *`
		template <typename T>
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

