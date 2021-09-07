
#include "settings.hh"
#include "ui/core.hh"
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
			[str::luma_not_installed] = "Luma3DS is not installed on this system",
			[str::install_luma] = "Please install Luma3DS on a real 3DS",
			[str::queue] = "Queue",
			[str::connect_wifi] = "Please connect to WiFi and restart the app",
			[str::fail_init_networking] = "Failed to initialize networking",
			[str::fail_fetch_index] = "Failed to fetch index\n%1",
			[str::credits_thanks] =
				"Thank you for using 3hs, a 3ds client for hShop\n"
				"You can get this software for free at\n"
				"https://hshop.erista.me/",
			[str::credits] =
				"MyPasswordIsWeak: Programming,\n"
				"      Translations\n"
				"TimmSkiller: Programming,\n"
				"      Translations\n"
				"Laswell: Design, Translations\n"
				"CremeChocolate: Art",
			[str::press_to_install] =
				"Press " GLYPH_A " if you want to install this title.\n"
				"Press " GLYPH_B " if you don't want to.",
			[str::version] = "Version",
			[str::prodcode] = "Product code",
			[str::size] = "Size",
			[str::name] = "Name",
			[str::tid] = "Title ID",
			[str::category] = "Category",
			[str::landing_id] = "Landing ID",
			[str::description] = "Description",
			[str::total_titles] = "Total Titles",
			[str::select_cat] = "Select a category",
			[str::select_subcat] = "Select a subcategory",
			[str::select_title] = "Select a title",
			[str::no_cats_index] = "no categories in index (?)",
			[str::empty_subcat] = "Empty subcategory (?)",
			[str::empty_cat] = "Empty category (?)",
			[str::fmt_24h] = "24 hour",
			[str::fmt_12h] = "12 hour",
			[str::unknown] = "unknown",
			[str::btrue] = "true",
			[str::bfalse] = "false",
			[str::top] = "top",
			[str::bottom] = "bottom",
			[str::light_mode] = "Light mode",
			[str::resume_dl] = "Resume downloads",
			[str::load_space] = "Load Free Space indicator",
			[str::show_battery] = "Show Battery",
			[str::time_format] = "Time format",
			[str::progbar_screen] = "Progress bar screen",
			[str::language] = "Language",
			[str::value_x] = "Value: %1",
			[str::back] = "Back",
			[str::invalid] = "invalid",
			[str::title_doesnt_exist] = "Title doesn't exist: %1",
			[str::fail_create_tex] = "Failed to create tex",
			[str::fail_load_smdh_icon] = "load_smdh_icon(): invalid SMDHIconType",
			[str::netcon_lost] = "Network connection lost.\nRetrying in %t seconds...",
			[str::about_app] = "About",
			[str::help_manual] = "Help/Manual",
			[str::find_missing_content] = "Find missing content",
			[str::press_a_exit] = "Press " GLYPH_A " to exit",
			[str::fatal_panic] = "Fatal Panic Occurred",
			[str::failed_open_seeddb] = "Failed to open seeddb.bin",
			[str::update_to] = "Do you want to update to %1?",
			[str::search_content] = "Search for content",
			[str::search_content_action] = "Search for content...",
			[str::results_query] = "Results for query \"%1\"",
			[str::result_code] = "Result code: %1",
			[str::level] = "Level: %1",
			[str::summary] = "Summary: %1",
			[str::module] = "Module: %1",
			[str::hs_bunny_found] = "Congrats! You found the hShop bunny",
			[str::already_installed_reinstall] = "Title already installed. Reinstall?",
			[str::queue_empty] =
				"Queue is empty\n"
				"Press " GLYPH_A " to go back\n"
				"Tip: press " GLYPH_Y " to add a title to the queue",
		},

	[lang::dutch] =
		{
			[str::banner]  = "De ultieme 3DS verzameling.",
			[str::loading] = "Aan het laden ...",
			[str::luma_not_installed] = "Luma3DS is niet geïnstalleerd op dit systeem",
			[str::install_luma] = "Installeer Luma3DS op een echte 3DS",
			[str::queue] = "Wachtrij",
			[str::connect_wifi] = "Verbind met WiFi en start de app opnieuw op",
			[str::fail_init_networking] = "Kon netwerkdiensten niet opstarten",
			[str::fail_fetch_index] = "Kon gegevens niet van de server halen\n%1",
			[str::credits_thanks] =
				"Bedankt voor het gebruiken van 3hs, een 3ds\n"
				"hShop applicatie\n"
				"Je kan deze software gratis verkrijgen op\n"
				"https://hshop.erista.me/",
			[str::credits] =
				"MyPasswordIsWeak: Programmeren,\n"
				"      Vertalingen\n"
				"TimmSkiller: Programmeren,\n"
				"      Vertalingen\n"
				"Laswell: Design, Vertalingen\n"
				"CremeChocolate: Kunst",
			[str::press_to_install] =
				"Klik op " GLYPH_A " als je deze titel wilt installeren.\n"
				"Klik op " GLYPH_B " als je dat niet wilt.",
			[str::version] = "Versie",
			[str::prodcode] = "Productie code",
			[str::size] = "Grootte",
			[str::name] = "Naam",
			[str::tid] = "Titel ID",
			[str::category] = "Categorie",
			[str::landing_id] = "Lanceer ID", // TODO: Make it less awkward?
			[str::description] = "Beschrijving",
			[str::total_titles] = "Aantal titels",
			[str::select_cat] = "Selecteer een categorie",
			[str::select_subcat] = "Selecteer een subcategorie",
			[str::select_title] = "Selecteer een titel",
			[str::no_cats_index] = "Geen categorieën van server gekregen (?)",
			[str::empty_subcat] = "Lege subcategorie (?)",
			[str::empty_cat] = "Lege categorie (?)",
			[str::fmt_24h] = "24 uur",
			[str::fmt_12h] = "12 uur",
			[str::unknown] = "onbekend",
			[str::btrue] = "waar",
			[str::bfalse] = "niet waar",
			[str::top] = "boven",
			[str::bottom] = "onder",
			[str::light_mode] = "Licht thema",
			[str::resume_dl] = "Downloads voortzetten",
			[str::load_space] = "Laad bruikbare opslag indicator",
			[str::show_battery] = "Laad batterij indicator",
			[str::time_format] = "Tijdsformaat",
			[str::progbar_screen] = "Laadbalk scherm",
			[str::language] = "Taal",
			[str::value_x] = "Waarde: %1",
			[str::back] = "Terug",
			[str::invalid] = "ongeldig",
			[str::title_doesnt_exist] = "Titel bestaat niet: %1",
			[str::fail_create_tex] = "Kon geen tex maken",
			[str::fail_load_smdh_icon] = "load_smdh_icon(): ongeldig SMDHIconType",
			[str::netcon_lost] = "Network verbinding verloren.\nWe proberen het in %t seconden weer...",
			[str::about_app] = "Informatie over applicatie",
			[str::help_manual] = "Hulp/Gebruikshandleiding",
			[str::find_missing_content] = "Vind missende titels",
			[str::press_a_exit] = "Klik op " GLYPH_A " om af te sluiten",
			[str::fatal_panic] = "Fatale fout",
			[str::failed_open_seeddb] = "Kon seeddb.bin niet openen",
			[str::update_to] = "Wil je updaten naar versie %1?",
			[str::search_content] = "Zoek naar titels",
			[str::search_content_action] = "Zoek naar titels...", // this one has a ... after it
			[str::results_query] = "Resultaten voor zoekopdracht \"%1\"",
			[str::result_code] = "Resultaatcode: %1",
			[str::level] = "Niveau: %1",
			[str::summary] = "Samenvatting: %1",
			[str::module] = "Module: %1",
			[str::hs_bunny_found] = "Je hebt het hShop konijn gevonden!",
			[str::already_installed_reinstall] = "Titel is al geïnstalleerd. Herinstalleeren?",
			[str::queue_empty] =
				"De wachtrij is leeg\n"
				"Klik op " GLYPH_A " om terug te gaan\n"
				"Tip: klik op " GLYPH_Y " om een titel toe te voegen\n"
				"aan de wachtrij",
		},

	[lang::german] =
		{
			[str::banner]  = "Der ultimative 3DS-Inhalte-Dienst.",
			[str::loading] = "Laden ...",
			[str::luma_not_installed] = "Luma3DS wurde auf dieser Konsole nicht gefunden.",
			[str::install_luma] = "Bitte installiere Luma3DS auf einem echten 3DS.",
			[str::queue] = "Warteschlange",
			[str::connect_wifi] = "Stell eine Verbindung zum Internet her und starte die App neu",
			[str::fail_init_networking] = "Netzwerkverbindung fehlgeschlagen",
			[str::fail_fetch_index] = "Index konnte nicht heruntergeladen werden\n%1",
			[str::credits_thanks] =
				"Danke, dass du 3hs nutzt.\n3hs ist ein 3DS-Client für hShop.\n"
				"Du kannst diese Software auf\nhttps://hshop.erista.me kostenlos herunterladen.\n",
			[str::credits] =
				"MyPasswordIsWeak: Programmierung,\n"
				"      Übersetzungen\n"
				"TimmSkiller: Programmierung,\n"
				"      Übersetzungen\n"
				"Laswell: Design, Übersetzungen\n"
				"CremeChocolate: Kunstwerke",
			[str::press_to_install] =
				"Drücke " GLYPH_A " falls du diesen Inhalt zu installieren\nmöchtest.\n"
				"Drücke " GLYPH_B " falls nicht.",
			[str::version] = "Version",
			[str::prodcode] = "Produkt-Code",
			[str::size] = "Größe",
			[str::name] = "Name",
			[str::tid] = "Title-ID",
			[str::category] = "Kategorie",
			[str::landing_id] = "Inhalt-ID",
			[str::description] = "Beschreibung",
			[str::total_titles] = "Gesamte Anzahl der Inhalte",
			[str::select_cat] = "Wähle eine Kategorie aus",
			[str::select_subcat] = "Wähle eine Unterkategorie aus",
			[str::select_title] = "Wähle einen Inhalt",
			[str::no_cats_index] = "Keine Kategorien im Index (?)",
			[str::empty_subcat] = "Leere Unterkategorie (?)",
			[str::empty_cat] = "Leere Kategorie (?)",
			[str::fmt_24h] = "24-Stunden-Zeit",
			[str::fmt_12h] = "12-Stunden-Zeit",
			[str::unknown] = "unbekannt",
			[str::btrue] = "Ja",
			[str::bfalse] = "Nein",
			[str::top] = "Oben",
			[str::bottom] = "Unten",
			[str::light_mode] = "Helles Design",
			[str::resume_dl] = "Downloads fortsetzen",
			[str::load_space] = "Freier-Speicherplatz-Indikator anzeigen",
			[str::show_battery] = "Batterie anzeigen",
			[str::time_format] = "Zeitformat",
			[str::progbar_screen] = "Ladebalken-Bildschirm",
			[str::language] = "Sprache",
			[str::value_x] = "Wert: %1",
			[str::back] = "Zurück",
			[str::invalid] = "ungültig",
			[str::title_doesnt_exist] = "Inhalt existiert nicht: %1",
			[str::fail_create_tex] = "Konnte tex nicht erstellen",
			[str::fail_load_smdh_icon] = "load_smdh_icon(): ungültiger SMDHIconType",
			[str::netcon_lost] = "Netzwerkverbindung verloren.\nNeuer versuch in %t Sekunden...",
			[str::about_app] = "Über",
			[str::help_manual] = "Hilfe/Anleitung",
			[str::find_missing_content] = "Suche nach fehlenden Inhalten",
			[str::press_a_exit] = "Drücke " GLYPH_A " um die App zu schließen",
			[str::fatal_panic] = "Schwerwiegender Fehler aufgetreten",
			[str::failed_open_seeddb] = "Konnte seeddb.bin nicht öffnen",
			[str::update_to] = "Möchtest du auf Version %1 updaten?",
			[str::search_content] = "Suche nach Inhalten",
			[str::search_content_action] = "Suche nach Inhalten...",
			[str::results_query] = "Ergebnisse für \"%1\"",
			[str::result_code] = "Fehler-Code: %1",
			[str::level] = "Level: %1",
			[str::summary] = "Kurzbeschreibung: %1",
			[str::module] = "Modul: %1",
			[str::hs_bunny_found] = "Glückwunsch! Du hast has hShop-Häschen gefunden",
			[str::already_installed_reinstall] = "Inhalt ist bereits installiert. Neuinstallieren?",
			[str::queue_empty] =
				"Warteschlange ist leer\n"
				"Drücke " GLYPH_A " um zurückzukehren\n"
				"Tipp: Drücke " GLYPH_Y " um einen Inhalt zur Warteschlange\nhinzuzufügen",
		},

	[lang::spanish] =
		{
			[str::banner]  = "El servicio de preservación de 3DS definitivo.",
			[str::loading] = "Loading ...",
			[str::luma_not_installed] = "Luma3DS no está instalado en este sistema",
			[str::install_luma] = "Por favor installe Luma3DS en una 3DS real",
			[str::queue] = "Cola",
			[str::connect_wifi] = "Por favor conéctese al Wi-Fi",
			[str::fail_init_networking] = "No se pudo inicializar redes",
			[str::fail_fetch_index] = "No se pudo recuperar el índice\n%1",
			[str::credits_thanks] =
				"Gracias por usar 3hs, un cliente de 3ds\npara hShop\n"
				"Puedes conseguir este software gratuito en at\n"
				"https://hshop.erista.me/",
			[str::credits] =
				"MyPasswordIsWeak: Programación,\n"
				"      Traducción\n"
				"TimmSkiller: Programación,\n"
				"      Traducción\n"
				"Laswell: Diseño, Traducción\n"
				"CremeChocolate: Arte",
			[str::press_to_install] =
				"Presiona " GLYPH_A " si quieres instalar este título.\n"
				"Presiona " GLYPH_B " si no quieres.",
			[str::version] = "Versión",
			[str::prodcode] = "Código de producto",
			[str::size] = "Tamaño",
			[str::name] = "Nombre",
			[str::tid] = "ID de título",
			[str::category] = "Categoría",
			[str::landing_id] = "Landing ID",
			[str::description] = "Descripción",
			[str::total_titles] = "Total de Titulos",
			[str::select_cat] = "Selecciona una categoría",
			[str::select_subcat] = "Selecciona una subcategoría",
			[str::select_title] = "Selecciona un título",
			[str::no_cats_index] = "No hay categorías en el índice (?)",
			[str::empty_subcat] = "Subcategoría vacía (?)",
			[str::empty_cat] = "Categoría Vacía (?)",
			[str::fmt_24h] = "24 horas",
			[str::fmt_12h] = "12 horas",
			[str::unknown] = "Desconocido",
			[str::btrue] = "Verdadero",
			[str::bfalse] = "Falso",
			[str::top] = "Superior",
			[str::bottom] = "Inferior",
			[str::light_mode] = "Modo luz",
			[str::resume_dl] = "Resumir descargas",
			[str::load_space] = "Cargar indicador de espacio libre",
			[str::show_battery] = "Mostrar Batería",
			[str::time_format] = "Formato de tiempo",
			[str::progbar_screen] = "Pantalla de barra de progreso",
			[str::language] = "Lenguaje",
			[str::value_x] = "Valor: %1",
			[str::back] = "Volver",
			[str::invalid] = "Inválido",
			[str::title_doesnt_exist] = "El título no existe: %1",
			[str::fail_create_tex] = "No se pudo crear tex",
			[str::fail_load_smdh_icon] = "load_smdh_icon(): SMDHIconType inválido",
			[str::netcon_lost] = "Se perdió la conexión con la red.\nReintentando in %t seconds...",
			[str::about_app] = "Acerca de",
			[str::help_manual] = "Ayuda/Manual",
			[str::find_missing_content] = "Buscar contenido ausente",
			[str::press_a_exit] = "Presiona " GLYPH_A " para salir",
			[str::fatal_panic] = "Ocurrió un error fatal de pánico",
			[str::failed_open_seeddb] = "No se pudo abrir seeddb.bin",
			[str::update_to] = "¿Quieres actualizar a %1?",
			[str::search_content] = "Buscar contenido",
			[str::search_content_action] = "Buscando por contenido...",
			[str::results_query] = "Resultados de \"%1\"",
			[str::result_code] = "Código de resultado: %1",
			[str::level] = "Nivel: %1",
			[str::summary] = "Sumario: %1",
			[str::module] = "Módulo: %1",
			[str::hs_bunny_found] = "¡Felicidades! ¡Encontraste a la conejita de hShop!",
			[str::already_installed_reinstall] = "Título ya instalado. ¿Reinstalar?",
			[str::queue_empty] =
				"La cola está vacia\n"
				"Presiona " GLYPH_A " para volver\n"
				"Consejo: presiona " GLYPH_Y " para añadir un título\na la cola",
		},

	[lang::uwulang] =
		{
			[str::banner]  = "The ultimate 3DS content preservatiOwOn service.",
			[str::loading] = "LOwOading ...",
			[str::luma_not_installed] = "LUwUma3DS is nOwOt installed OwOn this system",
			[str::install_luma] = "Please install Luma3DS OwOn a real 3DS",
			[str::queue] = "QUwU",
			[str::connect_wifi] = "Pleasew cOwOnnect tOwO WiFi and restart the app",
			[str::fail_init_networking] = "Failed tow initialize netOwOrking",
			[str::fail_fetch_index] = "Failed to fetch index\n%1",
			[str::credits_thanks] =
				"Thank youWu foWor UwUsing 3hs,\na 3ds client fOwOr hShOwOp\n"
				"You can get this sOwOftware fOwOr free at\n"
				"https://hshop.erista.me/",
			[str::credits] =
				"MyPasswordIsWeak: PrOwOgramming,\n"
				"      TranslatiOwOns\n"
				"TimmSkiller: PrOwOgramming,\n"
				"      TranslatiOwOns\n"
				"Laswell: Design, TranslatiOwOns\n"
				"CremeChocolate: Art",
			[str::press_to_install] =
				"Press " GLYPH_A " if youwu want to install this title.\n"
				"Press " GLYPH_B " if youwu don't want to.",
			[str::version] = "VersiOwOn",
			[str::prodcode] = "Product cOwOde",
			[str::size] = "Size",
			[str::name] = "Name",
			[str::tid] = "Title ID",
			[str::category] = "CategOwOry",
			[str::landing_id] = "Landing ID",
			[str::description] = "DescriptiOwOn",
			[str::total_titles] = "Total Titles",
			[str::select_cat] = "Select a categOwOry",
			[str::select_subcat] = "Select a subcategOwOry",
			[str::select_title] = "Select a title",
			[str::no_cats_index] = "no categOwOries in index (?)",
			[str::empty_subcat] = "Empty subcategOwOry (?)",
			[str::empty_cat] = "Empty categOwOry (?)",
			[str::fmt_24h] = "24 hoUwUr",
			[str::fmt_12h] = "12 hoUwUr",
			[str::unknown] = "UwUnknown",
			[str::btrue] = "true",
			[str::bfalse] = "false",
			[str::top] = "tOwOp",
			[str::bottom] = "bOwOttom",
			[str::light_mode] = "Light mOwOde",
			[str::resume_dl] = "ResUwUme downlOwOads",
			[str::load_space] = "LOwOad Free Space indicatOwOr",
			[str::show_battery] = "ShOwO Battery",
			[str::time_format] = "Time fOwOrmat",
			[str::progbar_screen] = "PrOwOgress bar screen",
			[str::language] = "LangUwUage",
			[str::value_x] = "ValUwU: %1",
			[str::back] = "Back",
			[str::invalid] = "invalid",
			[str::title_doesnt_exist] = "Title dOwOesn't exist: %1",
			[str::fail_create_tex] = "Failed to create tex",
			[str::fail_load_smdh_icon] = "load_smdh_icon(): invalid SMDHIconType",
			[str::netcon_lost] = "NetwOwOrk connectiOwOn lost.\nRetrying in %t seconds...",
			[str::about_app] = "AbOwOut",
			[str::help_manual] = "Help/Manual",
			[str::find_missing_content] = "Find missing cOwOntent",
			[str::press_a_exit] = "Press " GLYPH_A " tOwO exit",
			[str::fatal_panic] = "Fatal Panic OwOccurred",
			[str::failed_open_seeddb] = "Failed to OwOpen seeddb.bin",
			[str::update_to] = "Do yoUwU want tOwO UwUpdate to %1?",
			[str::search_content] = "Search fOwOr content",
			[str::search_content_action] = "Search fOwOr content...",
			[str::results_query] = "ResUwUlts fOwOr qUwUery \"%1\"",
			[str::result_code] = "ResUwUlt code: %1",
			[str::level] = "Level: %1",
			[str::summary] = "SUwUmmary: %1",
			[str::module] = "MOwOdule: %1",
			[str::hs_bunny_found] = "COwOngrats! You found the hShop bunny",
			[str::already_installed_reinstall] = "Title already installed. Reinstall?",
			[str::queue_empty] =
				"QUwU is empty\n"
				"Press " GLYPH_A " to gOwO back\n"
				"Tip: press " GLYPH_Y " to add a title to the qUwU",
		},
};
#pragma GCC diagnostic pop


const char *i18n::getstr(str::type id)
{
	return RAW(get_settings()->language, id);
}

const char *i18n::getstr(str::type sid, lang::type lid)
{
	return RAW(lid, sid);
}

const char *i18n::getsurestr(str::type sid)
{
	if(!settings_are_ready())
		ensure_settings();
	return RAW(get_settings()->language, sid);
}

const char *i18n::langname(lang::type id)
{
	switch(id)
	{
	case lang::english: return LANGNAME_ENGLISH;
	case lang::dutch: return LANGNAME_DUTCH;
	case lang::german: return LANGNAME_GERMAN;
	case lang::spanish: return LANGNAME_SPANISH;
	case lang::uwulang: return LANGNAME_UWULANG;
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
	case CFG_LANGUAGE_ES: return lang::spanish;
	case CFG_LANGUAGE_NL: return lang::dutch;

	case CFG_LANGUAGE_JP: // fallthrough
	case CFG_LANGUAGE_EN: // fallthrough
	case CFG_LANGUAGE_FR: // fallthrough
	case CFG_LANGUAGE_IT: // fallthrough
	case CFG_LANGUAGE_ZH: // fallthrough
	case CFG_LANGUAGE_KO: // fallthrough
	case CFG_LANGUAGE_PT: // fallthrough
	case CFG_LANGUAGE_RU: // fallthrough
	case CFG_LANGUAGE_TW: // fallthrough
	default: return lang::english;
	}
}

