
#include <ui/base.hh> /* for UI_GLYPH_* */

#include "settings.hh"
#include "panic.hh"
#include "i18n.hh"

#include <3ds.h>

#define ENGLISH(sid) strtab[lang::english][str::sid]
#define STUB(id) [str::id] = ENGLISH(id)
#define RAW(lid, sid) strtab[lid][sid]

// [str::xxx] is a GCC extension
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
			[str::credits_names] =
				"MyPasswordIsWeak: Programming,\n"
				"      Translations\n"
				"TimmSkiller: Programming,\n"
				"      Translations\n"
				"Laswell: Design, Translations\n"
				"CremeChocolate: Art",
			[str::press_to_install] =
				"Press " UI_GLYPH_A " if you want to install this title.\n"
				"Press " UI_GLYPH_B " if you don't want to.",
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
			[str::netcon_lost] = "Network connection lost (%1).\nRetrying in $t seconds...",
			[str::about_app] = "About",
			[str::help_manual] = "Help/Manual",
			[str::find_missing_content] = "Find missing content",
			[str::press_a_exit] = "Press " UI_GLYPH_A " to exit",
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
				"Press " UI_GLYPH_A " to go back\n"
				"Tip: press " UI_GLYPH_Y " to add a title to the queue",
			[str::cancel] = "Cancel",
			[str::confirm] = "Confirm",
			[str::invalid_proxy] = "Invalid proxy settings",
			[str::more_about_content] = "More about this content",
			[str::lumalocalemode] = "LumaLocale mode",
			[str::automatic] = "automatic",
			[str::manual] = "manual",
			[str::disabled] = "disabled",
			[str::patching_reboot] =
				"Luma3DS Game Patching was enabled.\n"
				"For the change to take effect, you have to\nrestart your 3DS.",
			[str::reboot_now] = "Restart now?",
			[str::this_version] = "This is 3hs version %1",
			[str::retry_req] = "Request failed. Do you want to retry?",
			[str::search_zero_results] = "Search gave 0 results\nPress " UI_GLYPH_A " to go back",
			[str::credits] = "Credits",
			[str::extra_content] = "Add extra content to queue?",
			[str::ask_extra] = "Ask for extra content",
			[str::no_req] = "no requests made yet",
			[str::invalid_query] = "Invalid query\nPress " UI_GLYPH_A " to go back",
			[str::min_constraint] = "Current 3hs version %1 is lower than the required version %2",
			[str::proxy] = "Proxy",
			[str::none] = "(none)",
			[str::press_a_to_view] = "Click " UI_GLYPH_A " to view",
			[str::host] = "Host",
			[str::port] = "Port",
			[str::username] = "Username",
			[str::password] = "Password",
			[str::clear] = "Clear",
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
			[str::credits_names] =
				"MyPasswordIsWeak: Programmeren,\n"
				"      Vertalingen\n"
				"TimmSkiller: Programmeren,\n"
				"      Vertalingen\n"
				"Laswell: Design, Vertalingen\n"
				"CremeChocolate: Kunst",
			[str::press_to_install] =
				"Klik op " UI_GLYPH_A " als je deze titel wilt installeren.\n"
				"Klik op " UI_GLYPH_B " als je dat niet wilt.",
			[str::version] = "Versie",
			[str::prodcode] = "Productie code",
			[str::size] = "Grootte",
			[str::name] = "Naam",
			[str::tid] = "Titel ID",
			[str::category] = "Categorie",
			[str::landing_id] = "hShop ID",
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
			[str::netcon_lost] = "Netwerk verbinding verloren (%1).\nWe proberen het in $t seconden weer...",
			[str::about_app] = "Informatie over applicatie",
			[str::help_manual] = "Hulp/Gebruikshandleiding",
			[str::find_missing_content] = "Vind missende titels",
			[str::press_a_exit] = "Klik op " UI_GLYPH_A " om af te sluiten",
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
				"Klik op " UI_GLYPH_A " om terug te gaan\n"
				"Tip: klik op " UI_GLYPH_Y " om een titel toe te voegen\n"
				"aan de wachtrij",
			[str::cancel] = "Weiger",
			[str::confirm] = "Accepteer",
			[str::invalid_proxy] = "Ongeldige proxy instellingen",
			[str::more_about_content] = "Meer over deze titel",
			[str::lumalocalemode] = "LumaLocale modus",
			[str::automatic] = "automatisch",
			[str::manual] = "handmatig",
			[str::disabled] = "uitgeschakeled",
			[str::patching_reboot] =
				"Luma3DS Game Patching is ingeschakeld\n"
				"Om de verandering te zien, moet je je\n3DS herstarten",
			[str::reboot_now] = "Nu herstarten?",
			[str::this_version] = "Dit is 3hs versie %1",
			[str::retry_req] ="Opnieuw proberen gegevens te halen?",
			[str::search_zero_results] =  "Zoekopdracht gaf 0 resultaten\nKlik op " UI_GLYPH_A " om terug te gaan",
			[str::credits] = "Met dank aan",
			STUB(extra_content),
			STUB(ask_extra),
			STUB(no_req),
			[str::invalid_query] = "Ongeldige zoekopdracht\nKlik op " UI_GLYPH_A " om terug te gaan",
			STUB(min_constraint),
			[str::proxy] = "Proxy",
			[str::none] = "(geen)",
			[str::press_a_to_view] = "Klik op " UI_GLYPH_A " om de waarde te bekijken",
			STUB(host),
			STUB(port),
			STUB(username),
			STUB(password),
			STUB(clear),
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
			[str::credits_names] =
				"MyPasswordIsWeak: Programmierung,\n"
				"      Übersetzungen\n"
				"TimmSkiller: Programmierung,\n"
				"      Übersetzungen\n"
				"Laswell: Design, Übersetzungen\n"
				"CremeChocolate: Kunstwerke",
			[str::press_to_install] =
				"Drücke " UI_GLYPH_A " falls du diesen Inhalt zu installieren\nmöchtest.\n"
				"Drücke " UI_GLYPH_B " falls nicht.",
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
			[str::netcon_lost] = "Netzwerkverbindung verloren (%1).\nNeuer versuch in $t Sekunden...",
			[str::about_app] = "Über",
			[str::help_manual] = "Hilfe/Anleitung",
			[str::find_missing_content] = "Suche nach fehlenden Inhalten",
			[str::press_a_exit] = "Drücke " UI_GLYPH_A " um die App zu schließen",
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
				"Drücke " UI_GLYPH_A " um zurückzukehren\n"
				"Tipp: Drücke " UI_GLYPH_Y " um einen Inhalt zur Warteschlange\nhinzuzufügen",
			[str::cancel] = "Abbrechen",
			[str::confirm] = "Bestätigen",
			[str::invalid_proxy] = "Ungültige Proxy-Einstellungen",
			[str::more_about_content] = "Mehr über diesen Inhalt",
			[str::lumalocalemode] = "LumaLocale-Modus",
			[str::automatic] = "Automatisch",
			[str::manual] = "Manuell",
			[str::disabled] = "Deaktiviert",
			[str::patching_reboot] =
					"Luma3DS-Game-Patching wurde aktiviert.\n"
					"Damit die Änderungen übernommen werden\nkönnen, musst du deinen 3DS neustarten.",
			[str::reboot_now] = "Jetzt neustarten?",
			[str::this_version] = "Dies ist 3hs Version %1",
			[str::retry_req] = "Erneut versuchen?",
			[str::search_zero_results] = "Suche ergab keine Ergebnisse\nDrücke " UI_GLYPH_A " um zurückzukehren",
			[str::credits] = "Mitwirkende",
			STUB(extra_content),
			STUB(ask_extra),
			STUB(no_req),
			STUB(invalid_query),
			STUB(min_constraint),
			[str::proxy] = "Proxy",
			STUB(none),
			STUB(press_a_to_view),
			STUB(host),
			STUB(port),
			STUB(username),
			STUB(password),
			STUB(clear),
		},

	[lang::spanish] =
		{
			[str::banner]  = "El servicio de preservación de 3DS definitivo.",
			[str::loading] = "Cargando ...",
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
			[str::credits_names] =
				"MyPasswordIsWeak: Programación,\n"
				"      Traducción\n"
				"TimmSkiller: Programación,\n"
				"      Traducción\n"
				"Laswell: Diseño, Traducción\n"
				"CremeChocolate: Arte",
			[str::press_to_install] =
				"Presiona " UI_GLYPH_A " si quieres instalar este título.\n"
				"Presiona " UI_GLYPH_B " si no quieres.",
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
			[str::netcon_lost] = "Se perdió la conexión con la red (%1).\nReintentando in $t seconds...",
			[str::about_app] = "Acerca de",
			[str::help_manual] = "Ayuda/Manual",
			[str::find_missing_content] = "Buscar contenido ausente",
			[str::press_a_exit] = "Presiona " UI_GLYPH_A " para salir",
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
				"Presiona " UI_GLYPH_A " para volver\n"
				"Consejo: presiona " UI_GLYPH_Y " para añadir un título\na la cola",
			[str::cancel] = "Cancelar",
			[str::confirm] = "Confirmar",
			[str::invalid_proxy] = "Configuración de proxy inválida",
			[str::more_about_content] = "Más sobre este contenido",
			[str::lumalocalemode] = "modo LumaLocale",
			[str::automatic] = "automático",
			[str::manual] = "manual",
			[str::disabled] = "desactivado",
			[str::patching_reboot] =
					"El Parcheo de Juegos de Luma3DS ha sido activado.\n"
					"Para que el cambio tenga efecto, tienes que \nreinicia tu 3DS.",
			[str::reboot_now] = "¿Reiniciar ahora?",
			[str::this_version] = "Esto es 3hs, versión %1",
			[str::retry_req] = "¿Quieres reintentar?",
			[str::search_zero_results] = "La búsqueda no dió resultados\nPresiona " UI_GLYPH_A " para regresar",
			[str::credits] = "Créditos",
			STUB(extra_content),
			STUB(ask_extra),
			STUB(no_req),
			STUB(invalid_query),
			STUB(min_constraint),
			[str::proxy] = "Proxy",
			STUB(none),
			STUB(press_a_to_view),
			STUB(host),
			STUB(port),
			STUB(username),
			STUB(password),
			STUB(clear),
		}
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
	}

	return "invalid";
}

lang::type i18n::default_lang()
{
	u8 syslang = 0;
	if(R_FAILED(CFGU_GetSystemLanguage(&syslang)))
		return lang::english;

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

