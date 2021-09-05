
#include "settings.hh"
#include "panic.hh"
#include "i18n.hh"

#include <3ds.h>

#define ENGLISH(sid) strtab[lang::english][sid]
#define RAW(lid, sid) strtab[lid][sid]

#pragma GCC diagnostic ignored "-Wpedantic"
static const char *strtab[lang::_i_max][str::_i_max] =
{
	[lang::english] =
		{
			[str::banner]  = "The ultimate 3DS content preservation service.",
			[str::loading] = "Loading ...", 
		},

	[lang::dutch] =
		{
			[str::banner]  = "De ultieme 3DS verzameling.",
			[str::loading] = "Aan het laden ...",
		},

	[lang::german] =
		{
			[str::banner]  = "Der ultimative 3DS Inhalte-Dienst",
			[str::loading] = "Laden ...",
		},
};
#pragma GCC diagnostic pop


const char *i18n::getstr(str::type id)
{
	return RAW(get_settings()->language, id);
}

const char *i18n::langname(lang::type id)
{
	switch(id)
	{
	case lang::english: return LANGNAME_ENGLISH;
	case lang::dutch: return LANGNAME_DUTCH;
	case lang::german: return LANGNAME_GERMAN;
	}

	return "invalid";
}

lang::type i18n::default_lang()
{
	u8 syslang = 0;
	panic_if_err_3ds(CFGU_GetSystemLanguage(&syslang));

	switch(syslang)
	{
	case CFG_LANGUAGE_DE: return lang::german;
	case CFG_LANGUAGE_NL: return lang::dutch;

	case CFG_LANGUAGE_JP: // fallthrough
	case CFG_LANGUAGE_EN: // fallthrough
	case CFG_LANGUAGE_FR: // fallthrough
	case CFG_LANGUAGE_IT: // fallthrough
	case CFG_LANGUAGE_ES: // fallthrough
	case CFG_LANGUAGE_ZH: // fallthrough
	case CFG_LANGUAGE_KO: // fallthrough
	case CFG_LANGUAGE_PT: // fallthrough
	case CFG_LANGUAGE_RU: // fallthrough
	case CFG_LANGUAGE_TW: // fallthrough
	default: return lang::english;
	}
}

