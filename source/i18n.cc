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

#include <ui/base.hh> /* for UI_GLYPH_* */

#include "settings.hh"
#include "panic.hh"
#include "i18n.hh"

#include <3ds.h>

#define ENGLISH(sid) strtab[lang::english][str::sid]
#define STUB(id) [str::id] = ENGLISH(id)
#define RAW(lid, sid) strtab[lid][sid]

// [str::xxx] is a GCC extension
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
static const char *strtab[lang::_i_max][str::_i_max] =
{
	[lang::english] =
		{
			[str::banner]  = "The ultimate 3DS content preservation service", /* unused */
			[str::loading] = "Loading...",
			[str::luma_not_installed] = "An unsupported firmware was detected.",
			[str::install_luma] = "3hs cannot be used on this system.",
			[str::queue] = "Queue",
			[str::connect_wifi] = "Please connect to Wi-Fi and restart the app.", /* unused */
			[str::fail_init_networking] = "Failed to initialize networking",
			[str::fail_fetch_index] = "Failed to fetch index\n%1", /* unused */
			[str::credits_thanks] =
				"Thank you for using 3hs, a 3ds client for hShop.\n"
				"You can get this software for free at\n"
				"https://hshop.erista.me/",
			[str::credits_names] = "", /* unused */
			[str::press_to_install] =
				"Press " UI_GLYPH_A " to install this content.\n"
				"Press " UI_GLYPH_B " to go back.",
			[str::version] = "Version",
			[str::prodcode] = "Product Code",
			[str::size] = "Size",
			[str::name] = "Name",
			[str::tid] = "Title ID",
			[str::category] = "Category",
			[str::landing_id] = "hShop ID",
			[str::description] = "Description",
			[str::total_titles] = "Total Titles",
			[str::select_cat] = "Select a category",
			[str::select_subcat] = "Select a subcategory",
			[str::select_title] = "Select a title",
			[str::no_cats_index] = "no categories in index (?)", /* unused */
			[str::empty_subcat] = "Empty subcategory (?)", /* unused */
			[str::empty_cat] = "Empty category (?)", /* unused */
			[str::fmt_24h] = "24 hour", /* context: time format */
			[str::fmt_12h] = "12 hour", /* context: time format */
			[str::unknown] = "unknown", /* context: unknown setting */
			[str::btrue] = "true", /* context: setting is set to ON */
			[str::bfalse] = "false", /* context: setting is set to OFF */
			[str::top] = "top", /* context: top screen in progress bar location settings switcher */
			[str::bottom] = "bottom", /* context: bottom screen in progress bar location settings switcher */
			[str::light_mode] = "Light mode",
			[str::resume_dl] = "Resume downloads",
			[str::load_space] = "Show free space indicator",
			[str::show_battery] = "Show battery level indicator",
			[str::time_format] = "Time format",
			[str::progbar_screen] = "Progress bar position",
			[str::language] = "Language",
			[str::value_x] = "Value: %1", /* context: value of setting */
			[str::back] = "Back", /* context: back button */
			[str::invalid] = "invalid",
			[str::title_doesnt_exist] = "Title doesn't exist: %1", /* %1: title id */
			[str::fail_create_tex] = "Failed to create tex", /* unused */
			[str::fail_load_smdh_icon] = "load_smdh_icon(): invalid SMDHIconType", /* unused */
			[str::netcon_lost] = "Network connection lost (%1).\nRetrying in $t seconds...", /* %1: error code, $t: seconds remaining */
			[str::about_app] = "About",
			[str::help_manual] = "Help/Manual",
			[str::find_missing_content] = "Find missing content",
			[str::press_a_exit] = "Press " UI_GLYPH_A " to exit.", /* context: exit menu and sometimes application */
			[str::fatal_panic] = "A fatal panic has occurred.", /* context: can also be translated as fatal error */
			[str::failed_open_seeddb] = "Failed to open seeddb.bin.",
			[str::update_to] = "Do you want to update to %1?", /* %1: new version, i.e. 1.0.2 */
			[str::search_content] = "Search for content",
			[str::search_content_action] = "Search for content...",
			[str::results_query] = "Results for query \"%1\"", /* %1: search query */
			[str::result_code] = "Result code: %1", /* %1: result code */
			[str::level] = "Level: %1", /* %1: english level name (level code), context: used when displaying errors */
			[str::summary] = "Summary: %1", /* %1 english summary name (summary code), context: used when displaying errors */
			[str::module] = "Module: %1", /* %1 english module name (module code), context: used when displaying errors */
			[str::hs_bunny_found] = "Congrats! You found the hShop bunny!", /* context: easter egg, see also: gfx/img/bun.png */
			[str::already_installed_reinstall] = "Title already installed. Reinstall?",
			[str::queue_empty] =
				"Queue is empty\n"
				"Press " UI_GLYPH_A " to go back.\n"
				"Tip: Press " UI_GLYPH_Y " to add a title to the queue.",
			[str::cancel] = "Cancel",
			[str::confirm] = "Confirm",
			[str::invalid_proxy] = "Invalid proxy settings",
			[str::more_about_content] = "More about this content",
			[str::lumalocalemode] = "LumaLocale mode",
			[str::automatic] = "automatic",
			[str::manual] = "manual",
			[str::disabled] = "disabled",
			[str::patching_reboot] =
				"Luma3DS Game Patching is now enabled.\n"
				"The system must be restarted for this change to take effect.",
			[str::reboot_now] = "Restart now?",
			[str::this_version] = "This is 3hs version %1", /* %1 = version int "version desc" */
			[str::retry_req] = "Request failed. Retry?", /* context: API request */
			[str::search_zero_results] = "Your search returned no results.\nPress " UI_GLYPH_A " to go back.",
			[str::credits] = "Credits",
			[str::extra_content] = "Add extra content to queue?", /* context: i.e. DLC/Updates */
			[str::check_extra] = "Search for extra content", /* context: i.e. DLC/Updates */
			[str::no_req] = "No requests made yet", /* context: hlink requests */
			[str::invalid_query] = "Invalid query\nPress " UI_GLYPH_A " to go back.",
			[str::min_constraint] = "Current 3hs version %1 is lower than the required version %2",
			[str::proxy] = "Proxy",
			[str::none] = "(none)", /* context: used for no proxy set */
			[str::press_a_to_view] = "Click " UI_GLYPH_A " to view.", /* context: view proxy settings */
			[str::host] = "Host", /* context: proxy */
			[str::port] = "Port", /* context: proxy */
			[str::username] = "Username", /* context: proxy */
	 		[str::password] = "Password", /* context: proxy */
			[str::clear] = "Clear", /* context: clear the contents of a field */
			[str::progbar_screen_desc] = "Select the position of the progress bar.",
			[str::light_mode_desc] = "Enable/disable light mode. This will change the way most UI elements look.",
			[str::resume_dl_desc] = "Resume downloads after a network interruption.",
			[str::load_space_desc] = "Toggle the free space indicator.",
			[str::show_battery_desc] = "Toggle the battery level indicator.",
			[str::time_format_desc] = "Select the time format.",
			[str::language_desc] = "Set the language for 3hs. The application must be restarted for the selected language to take effect.",
			[str::lumalocalemode_desc] = "Set the mode of the LumaLocale autosetter. Automatic selects a language automatically. If this is set to manual, 3hs will prompt for a region after the installation of content.",
			[str::check_extra_desc] = "Search for extra content after installation.",
			[str::proxy_desc] = "Configure a proxy server for 3hs. This setting is for advanced users.",
			[str::install_all] = "Install all", /* context: install all items in queue */
			[str::install_no_base] = "The base game is not installed. Continue anyway?",
			[str::warn_no_base] = "Warn if a base game is not installed",
			[str::warn_no_base_desc] = "Shows a message before installing update or DLC content if the base game is not installed.",
			[str::replaying_errors] = "Replaying errors encountered while processing the queue.",
			[str::log] = "Logs", /* context: may also be translated as "manage logs" */
			[str::upload_logs] = "Upload logs",
			[str::clear_logs] = "Clear logs",
			[str::found_missing] = "Found %1 missing title(s)\nCheck the queue to install them.", // %1 = amount of titles found
			[str::found_0_missing] = "No missing titles were found.",
			[str::max_elogs] = "Maximum old log files",
			[str::max_elogs_desc] = "Set the number of log files to keep. Possible values are 0 to 255, where 0 keeps no additional log files.",
			[str::elogs_hint] = "Value between 0 and 255",
			[str::log_id] = "Use this ID to get support:\n%1", /* %1 = id */
			[str::block] = "block", /* context: nintendo blocks, use official nintendo translation if available */
			[str::blocks] = "blocks", /* context: see above, just in plural this time */
			[str::search_text] = "Search by text",
			[str::search_id] = "Search by hShop ID",
			[str::search_tid] = "Search by Title ID",
			[str::invalid_tid] = "Invalid Title ID",
			[str::theme_installer_tid_bad] = "Please browse the Themes category manually to look for themes",
			[str::enter_lgy_query] = "Enter a legacy search query",
			[str::no_other_params_tid] = "Cannot specify any other parameters when specifying a Title ID",
			[str::both_sd_and_sb] = "You must specify a sort method and a sort direction when sorting search results",
			[str::invalid_sb] = "Invalid sort method",
			[str::invalid_sd] = "Invalid sort direction",
			[str::invalid_includes] = "Invalid include filters",
			[str::invalid_excludes] = "Invalid exclude filters",
			[str::filter_overlap] = "Detected an overlap in the specified search filters",
			[str::lgy_search] = "Legacy Search",
		},

	[lang::dutch] =
		{
			[str::banner]  = "De ultieme 3DS verzameling.",
			[str::loading] = "Aan het laden ...",
			[str::luma_not_installed] = "Je gebruikt een (C)FW die niet ondersteund is",
			[str::install_luma] = "Je kan 3hs niet op deze 3DS gebruiken",
			[str::queue] = "Wachtrij",
			[str::connect_wifi] = "Verbind met WiFi en start de app opnieuw op",
			[str::fail_init_networking] = "Kon netwerkdiensten niet opstarten",
			[str::fail_fetch_index] = "Kon gegevens niet van de server halen\n%1",
			[str::credits_thanks] =
				"Bedankt voor het gebruiken van 3hs, een 3ds\n"
				"hShop applicatie\n"
				"Je kan deze software gratis verkrijgen op\n"
				"https://hshop.erista.me/",
			[str::credits_names] = "",
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
				"Tip: klik op " UI_GLYPH_Y " om een titel toe te voegen aan de wachtrij",
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
			[str::extra_content] = "Extras aan wachtrij toevoegen?",
			[str::check_extra] = "Zoek naar extras",
			[str::no_req] = "Er is nog niets gebeurd",
			[str::invalid_query] = "Ongeldige zoekopdracht\nKlik op " UI_GLYPH_A " om terug te gaan",
			[str::min_constraint] = "Huidige 3hs versie %1 is lager dan het minimale wat %2 is",
			[str::proxy] = "Proxy",
			[str::none] = "(geen)",
			[str::press_a_to_view] = "Klik op " UI_GLYPH_A " om de waarde te bekijken",
			[str::host] = "Adres",
			[str::port] = "Poort",
			[str::username] = "Gebruikersnaam",
			[str::password] = "Wachtwoord",
			[str::clear] = "Maak leeg",
			[str::progbar_screen_desc] = "Het scherm om de laadbalk op te laten verschijnen",
			[str::light_mode_desc] = "Zet lichte modus aan. Dit zal het uiterlijk van 3hs een lichte tint geven",
			[str::resume_dl_desc] = "Hervat downloads wanneer zij onderbroken worden",
			[str::load_space_desc] = "Laad de vrije opslag balk",
			[str::show_battery_desc] = "Laat de status van de batterij zien",
			[str::time_format_desc] = "Jouw favoriete tijd formaat. 24h of 12h. In Nederland wordt 24h het vaakst gebruikt",
			[str::language_desc] = "De taal waarin alles wordt weergegeven. Soms moet je 3hs herstarten voordat alles veranderd is",
			[str::lumalocalemode_desc] = "De wijze waarom LumaLocale bestanden worden gegenereerd. Automatisch zal alles automatisch regelen. Als je handmatig kiest zal je worden gevraagt naar de taal en regio",
			[str::check_extra_desc] = "Zoek naar extras na het installeren van een spel",
			[str::proxy_desc] = "Configureer een proxy. Als je niet weet wat dat is raak deze optie dan niet aan",
			[str::install_all] = "Installeer alles",
			[str::install_no_base] = "Het spel waarbij deze extra hoort is niet geïnstalleerd. Toch doorgaan?",
			[str::warn_no_base] = "Waarschuw bij nutteloze extras",
			[str::warn_no_base_desc] = "Geef een waarschuwing als je een extra probeert te installeren waarvan het bijhorende spel niet is geïnstalleerd",
			[str::replaying_errors] = "Fouten aan het herspelen die zijn voorgekomen.",
			[str::log] = "Beheer logboek",
			[str::upload_logs] = "Logboek uploaden",
			[str::clear_logs] = "Logboek verwijderen",
			[str::found_missing] = "%1 missende titel(s) gevonden\nGa naar de wachtrij om ze te installeren",
			[str::found_0_missing] = "Geen missende titles gevonden",
			[str::max_elogs] = "Maximum aantal extra log bestanden",
			[str::max_elogs_desc] = "Aantal oude log bestanden om te bewaren. Een nummer van 0 tot en met 255.",
			[str::elogs_hint] = "Nummer van 0 tot/met 255",
			[str::log_id] = "Gebruik deze ID om hulp te krijgen\n%1",
			[str::block] = "blok",
			[str::blocks] = "blokken",
			[str::search_text] = "Zoek op tekst",
			[str::search_id] = "Vind met hShop ID",
			STUB(search_tid),
			STUB(invalid_tid),
			STUB(theme_installer_tid_bad),
			STUB(enter_lgy_query),
			STUB(no_other_params_tid),
			STUB(both_sd_and_sb),
			STUB(invalid_sb),
			STUB(invalid_sd),
			STUB(invalid_includes),
			STUB(invalid_excludes),
			STUB(filter_overlap),
			STUB(lgy_search),
		},

	[lang::german] =
		{
			[str::banner]  = "Der ultimative 3DS-Inhalte-Dienst.",
			[str::loading] = "Laden ...",
			[str::luma_not_installed] = "Dieses Gerät verwendet eine nicht unterstützte Firmware",
			[str::install_luma] = "3hs kann auf diesem Gerät nicht verwendet werden",
			[str::queue] = "Warteschlange",
			[str::connect_wifi] = "Stell eine Verbindung zum Internet her und starte die App neu",
			[str::fail_init_networking] = "Netzwerkverbindung fehlgeschlagen",
			[str::fail_fetch_index] = "Index konnte nicht heruntergeladen werden\n%1",
			[str::credits_thanks] =
				"Danke, dass du 3hs nutzt.\n3hs ist ein 3DS-Client für hShop.\n"
				"Du kannst diese Software auf\nhttps://hshop.erista.me kostenlos herunterladen.\n",
			[str::credits_names] = "",
			[str::press_to_install] =
				"Drücke " UI_GLYPH_A " falls du diesen Inhalt zu installieren möchtest.\n"
				"Drücke " UI_GLYPH_B " falls nicht.",
			[str::version] = "Version",
			[str::prodcode] = "Produkt-Code",
			[str::size] = "Größe",
			[str::name] = "Name",
			[str::tid] = "Title-ID",
			[str::category] = "Kategorie",
			[str::landing_id] = "hShop-ID",
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
			[str::press_a_exit] = "Drücke " UI_GLYPH_A " zum Schließen",
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
				"Tipp: Drücke " UI_GLYPH_Y " um einen Inhalt zur Warteschlange hinzuzufügen",
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
					"Damit die Änderungen übernommen werden können, musst du deinen 3DS neustarten.",
			[str::reboot_now] = "Jetzt neustarten?",
			[str::this_version] = "Dies ist 3hs Version %1",
			[str::retry_req] = "Erneut versuchen?",
			[str::search_zero_results] = "Suche ergab keine Ergebnisse\nDrücke " UI_GLYPH_A " um zurückzukehren",
			[str::credits] = "Mitwirkende",
			[str::extra_content] = "Zusätzliche Inhalte zur Warteschlange hinzufügen?",
			[str::check_extra] = "Nach zusätzlichen Inhalten suchen",
			[str::no_req] = "keine anfragen gestellt", /* context: hlink requests */
			[str::invalid_query] = "Ungültige Anfrage\nDrücke " UI_GLYPH_A " um zurückzukehren",
			[str::min_constraint] = "Jetztige 3hs-Version ist %1 Versionen älter als die benötigte Version %2",
			[str::proxy] = "Proxy",
			[str::none] = "(keine)",
			[str::press_a_to_view] = "Drücke " UI_GLYPH_A " um anzusehen",
			[str::host] = "Adresse",
			[str::port] = "Port",
			[str::username] = "Benutzername",
			[str::password] = "Passwort",
			[str::clear] = "Löschen", /* context: clear the contents of a field */
			[str::progbar_screen_desc] = "Der Bildschirm auf dem die Fortschrittsbalken gezeichnet werden sollen",
			[str::light_mode_desc] = "Den Lichtmodus anschalten. Dies wird das Aussehen der meisten Elemente auf der Benutzeroberfläche ändern.",
			[str::resume_dl_desc] = "Sollen wir dort fortfahren, wo wir beim ersten Versuch fehlgeschlagen haben?",
			[str::load_space_desc] = "Die freier-Speicherplatz Anzeige anzeigen",
			[str::show_battery_desc] = "Batterieanzeige in der Ecke oben links anzeigen",
			[str::time_format_desc] = "Dein bevorzugtes Zeitformat, entweder 24-Stunden or 12-Stunden.",
			[str::language_desc] = "Die Sprache die 3hs verwenden soll. Um die Änderungen am Text vollstandig anzuwenden, musst du 3hs möglicherweise neu starten",
			[str::lumalocalemode_desc] = "Der Modus der LumaLocale autosetter verwdendet. Automatisch wird eine Sprache automatisch auswählen. Falls du Manuell auswählst, wird 3hs dich um die Region fragen nach der Installation eines Inhalts",
			[str::check_extra_desc] = "Nach zusätzlichen Inhalten suchen nach einer Installation",
			[str::proxy_desc] = "Konfiguriere eine Proxy. Nicht berühren, falls du nicht weißt, was du tust",
			[str::install_all] = "Alles installieren", /* context: install all items in queue */
			[str::install_no_base] = "Kein Grundspiel für dieses DLC/Update installiert. Trotzdem fortfahren?",
			[str::warn_no_base] = "Warnen, wenn ein Grundspiel nicht installiert ist",
			[str::warn_no_base_desc] = "Eine Warnung anzeigen, wenn du versuchst, DLC/Update Daten zu installieren, wenn das zusammenpassende Grundspiel nicht installiert ist.",
			[str::replaying_errors] = "Zeige Fehler die während der Verarbeitung der Warteschlange aufgetreten sind.",
			[str::log] = "Protokolle",
			[str::upload_logs] = "Protokolle hochladen",
			[str::clear_logs] = "Protokolle löschen",
			[str::found_missing] = "%1 fehlende(r) Inhalt(e) gefunden.\nDu kannst sie in der Warteschlange installieren.", // %1 = amount of titles found
			[str::found_0_missing] = "Keine fehlenden Inhalte gefunden",
			[str::max_elogs] = "Maximale Anzahl von alten Protokolldateien",
			[str::max_elogs_desc] = "Anzahl von alten Protokolldateien die behalten werden sollen, zwischen 0-255 inklusive, 0 = keine zusätzlichen Protokolldateien werden behalten.",
			[str::elogs_hint] = "Nummer zwischen 0-255 inklusive",
			[str::log_id] = "Benutze diese ID, um Hilfe zu bekommen\n%1",
			[str::block] = "Block",
			[str::blocks] = "Blöcke",
			[str::search_text] = "Mithilfe von Text suchen",
			[str::search_id] = "Mithilfe einer hShop ID suchen",
			STUB(search_tid),
			STUB(invalid_tid),
			STUB(theme_installer_tid_bad),
			STUB(enter_lgy_query),
			STUB(no_other_params_tid),
			STUB(both_sd_and_sb),
			STUB(invalid_sb),
			STUB(invalid_sd),
			STUB(invalid_includes),
			STUB(invalid_excludes),
			STUB(filter_overlap),
			STUB(lgy_search),
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
				"Puedes conseguir este software gratuito en\n"
				"https://hshop.erista.me/",
			[str::credits_names] = "",
			[str::press_to_install] =
				"Presiona " UI_GLYPH_A " si quieres instalar este título.\n"
				"Presiona " UI_GLYPH_B " si no quieres.",
			[str::version] = "Versión",
			[str::prodcode] = "Código de producto",
			[str::size] = "Tamaño",
			[str::name] = "Nombre",
			[str::tid] = "ID de título",
			[str::category] = "Categoría",
			[str::landing_id] = "ID hShop",
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
				"Consejo: presiona " UI_GLYPH_Y " para añadir un título a la cola",
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
			[str::extra_content] = "¿Añadir contenido extra a la cola?",
			[str::check_extra] = "Buscar contenido extra",
			[str::no_req] = "No hay solicitudes hechas todavía", /* context: hlink requests */
			[str::invalid_query] = "Consulta invalida\nPresiona " UI_GLYPH_A " para volver",
			[str::min_constraint] = "La versión de 3hs %1 es inferior a la versión requerida %2",
			[str::proxy] = "Proxy",
			[str::none] = "(ninguno)",
			[str::press_a_to_view] = "Pulsa " UI_GLYPH_A " para ver",
			[str::host] = "Host",
			[str::port] = "Puerto",
			[str::username] = "Usuario",
			[str::password] = "Contraseña",
			[str::clear] = "Despejar", /* context: clear the contents of a field */
			[str::progbar_screen_desc] = "La pantalla para dibujar barras de progreso",
			[str::light_mode_desc] = "Activar modo luz. Esto cambiará cómo se verán la mayoría de la interfaz",
			[str::resume_dl_desc] = "¿Debería resumir donde dejamos la descarga por primera vez si fallamos la anterior?",
			[str::load_space_desc] = "Cargar el indicador de espacio libre",
			[str::show_battery_desc] = "Alternar visibilidad de la batería en la esquina superior derecha",
			[str::time_format_desc] = "Formato de tiempo preferido. Entre 24 o 12 horas",
			[str::language_desc] = "El lenguaje que 3hs usa. Nótese que para actualizar todo el texto, tienes que reiniciar 3hs",
			[str::lumalocalemode_desc] = "El modo en la que el auto selector de LumaLocale funciona. Automático selecciona un lenjuage automáticamente. Si eliges manual, 3hs pedirá por una región después de la instalación de contenido",
			[str::check_extra_desc] = "Busca por contenido extra después de la instalación.",
			[str::proxy_desc] = "Configurar un proxy. Si no sabes lo que estás haciendo, no toques esto",
			[str::install_all] = "Instalar todo", /* context: install all items in queue */
			[str::install_no_base] = "La actualización/DLC no tiene el juego base instalado. ¿Instalar de todas maneras?",
			[str::warn_no_base] = "Advertir si el juego base no está instalado",
			[str::warn_no_base_desc] = "Dar un aviso si estás intentanto instalar una actualización/DLC y el juego base no está instalado.",
			[str::replaying_errors] = "Reproduciendo errores ocurridos mientras se procesa la cola.",
			[str::log] = "Registros",
			[str::upload_logs] = "Subir registros",
			[str::clear_logs] = "Despejar registros",
			[str::found_missing] = "Se encontró %1 Título faltante(s)\nRevisa la cola para instalarlos", // %1 = amount of titles found
			[str::found_0_missing] = "No se encontraron títulos faltantes",
			[str::max_elogs] = "Máxima capacidad de archivos de registro viejos",
			[str::max_elogs_desc] = "Capacidad de archivos de registro que se puede guardar. Debe ser un número que incluya de 0 a 255, al escoger 0, no se guardarán registros adicionales.",
			[str::elogs_hint] = "Número que incluya 0 a 255",
			[str::log_id] = "Usa este ID para conseguir soporte\n%1",
			[str::block] = "bloque",
			[str::blocks] = "bloques",
			[str::search_text] = "Busqueda por texto",
			[str::search_id] = "Buscar usando una ID hShop",
			STUB(search_tid),
			STUB(invalid_tid),
			STUB(theme_installer_tid_bad),
			STUB(enter_lgy_query),
			STUB(no_other_params_tid),
			STUB(both_sd_and_sb),
			STUB(invalid_sb),
			STUB(invalid_sd),
			STUB(invalid_includes),
			STUB(invalid_excludes),
			STUB(filter_overlap),
			STUB(lgy_search),
		},

	[lang::french] =
		{
			[str::banner]  = "L'ultime service de préservation de contenu 3DS",
			[str::loading] = "Chargement...",
			[str::luma_not_installed] = "Un firmware non supporté a été détecté.",
			[str::install_luma] = "3hs ne peut pas être utilisé sur ce système.",
			[str::queue] = "Liste d'attente",
			[str::connect_wifi] = "Connectez vous au WIFI et redémarrez l'app",
			[str::fail_init_networking] = "Impossible d'initialiser le réseau",
			[str::fail_fetch_index] = "Impossible de récupérer l'index\n%1",
			[str::credits_thanks] =
				"Merci d'utiliser 3hs, un client 3DS pour hshop.\n"
				"Vous pouvez avoir ce logiciel gratuitement sur\n"
				"https://hshop.erista.me/",
			[str::credits_names] = "",
			[str::press_to_install] =
				"Appuyez sur " UI_GLYPH_A " pour installer.\n"
				"Appuyez sur " UI_GLYPH_B " pour revenir.",
			[str::version] = "Version",
			[str::prodcode] = "Nom de Code",
			[str::size] = "Taille",
			[str::name] = "Nom",
			[str::tid] = "ID de titre",
			[str::category] = "Catégorie",
			[str::landing_id] = "ID hShop",
			[str::description] = "Description",
			[str::total_titles] = "Titres Total",
			[str::select_cat] = "Sélectionnez une catégorie",
			[str::select_subcat] = "Sélectionnez une sous-catégorie",
			[str::select_title] = "Sélectionnez un titre",
			[str::no_cats_index] = "aucune catégories dans l'index (?)",
			[str::empty_subcat] = "Sous-catégorie vide (?)",
			[str::empty_cat] = "Catégorie vide (?)",
			[str::fmt_24h] = "24 heures",
			[str::fmt_12h] = "12 heures",
			[str::unknown] = "inconnu",
			[str::btrue] = "vrai",
			[str::bfalse] = "faux",
			[str::top] = "haut",
			[str::bottom] = "bas",
			[str::light_mode] = "Thème clair",
			[str::resume_dl] = "Continuer les téléchargements",
			[str::load_space] = "Montrer l'indicateur d'espace libre",
			[str::show_battery] = "Montrer l'indicateur de batterie",
			[str::time_format] = "Format horaire",
			[str::progbar_screen] = "Position de la barre de progression",
			[str::language] = "Langue",
			[str::value_x] = "Valeur: %1",
			[str::back] = "Retour",
			[str::invalid] = "invalide",
			[str::title_doesnt_exist] = "Ce titre n'existe pas: %1",
			[str::fail_create_tex] = "Impossible de créer la texture",
			[str::fail_load_smdh_icon] = "load_smdh_icon(): invalid SMDHIconType",
			[str::netcon_lost] = "La connexion internet a été perdue (%1).\nNouvelle tentative dans $t seconds...",
			[str::about_app] = "A propos",
			[str::help_manual] = "Aide/Manuel",
			[str::find_missing_content] = "Trouver le contenu perdu",
			[str::press_a_exit] = "Appuyez sur " UI_GLYPH_A " pour quitter.",
			[str::fatal_panic] = "Une erreur fatale est survenue.",
			[str::failed_open_seeddb] = "Impossible d'ouvrir le seeddb.bin.",
			[str::update_to] = "Voulez vous mettre à jour en %1?",
			[str::search_content] = "Rechercher",
			[str::search_content_action] = "Recherche en cours...",
			[str::results_query] = "Résultats de la recherche \"%1\"",
			[str::result_code] = "Result code: %1",
			[str::level] = "Level: %1",
			[str::summary] = "Summary: %1",
			[str::module] = "Module: %1",
			[str::hs_bunny_found] = "Bravo! Tu as trouvé le lapin hShop!",
			[str::already_installed_reinstall] = "Le titre est déjà installé. Voulez-vous le réinstaller?",
			[str::queue_empty] =
				"La liste d'attente est vide.\n"
				"Appuyez sur " UI_GLYPH_A " pour revenir.\n"
				"Astuce: Appuyez sur " UI_GLYPH_Y " pour ajouter un titre à la liste d'attente.",
			[str::cancel] = "Annuler",
			[str::confirm] = "Confirmer",
			[str::invalid_proxy] = "Paramètre proxy invalide",
			[str::more_about_content] = "En savoir plus sur ce contenu",
			[str::lumalocalemode] = "LumaLocale mode",
			[str::automatic] = "automatique",
			[str::manual] = "manuel",
			[str::disabled] = "désactivé",
			[str::patching_reboot] =
				"Luma3DS Game Patching est maintenant activé.\n"
				"La console doit redémarrer pour que ce changement fasse effet.",
			[str::reboot_now] = "Redémarrer maintenant?",
			[str::this_version] = "Version de 3hs %1", // %1 = version int "version desc"
			[str::retry_req] = "Echec de la requête. Réessayer?",
			[str::search_zero_results] = "Aucun résultat pour votre recherche.\nAppuyez sur " UI_GLYPH_A " pour revenir.",
			[str::credits] = "Crédits",
			[str::extra_content] = "Ajouter le contenu additionnel à la liste d'attente?",
			[str::check_extra] = "Recherche de contenu additionnel",
			[str::no_req] = "Aucune requête pour le moment", /* context: hlink requests */
			[str::invalid_query] = "Recherche invalide\n appuyez sur " UI_GLYPH_A " pour revenir.",
			[str::min_constraint] = "La version actuelle de 3hs %1 est inférieure à celle requise %2",
			[str::proxy] = "Proxy",
			[str::none] = "(none)",
			[str::press_a_to_view] = "Appuyez sur " UI_GLYPH_A " pour afficher.",
			[str::host] = "Host",
			[str::port] = "Port",
			[str::username] = "Nom d'utilisateur",
			[str::password] = "Mot de passe",
			[str::clear] = "Supprimer", /* context: clear the contents of a field */
			[str::progbar_screen_desc] = "Choisissez la position de la barre de progression.",
			[str::light_mode_desc] = "Active/Désactive le thème clair.",
			[str::resume_dl_desc] = "Continuer les téléchargements après une interruption réseau.",
			[str::load_space_desc] = "Active/Désactive l'indicateur d'espace libre.",
			[str::show_battery_desc] = "Active/Désactive l'indicateur de batterie.",
			[str::time_format_desc] = "Choisissez le format de l'heure.",
			[str::language_desc] = "Change la langue de 3hs.\nL'application doit être redémarrée pour que la nouvelle langue soit affichée.",
			[str::lumalocalemode_desc] = "Choisit le mode de LumaLocale autosetter. \"Automatique\" séléctionne une langue automatiquement alors que \"Manuel\" affiche la région à choisir après l'installation.",
			[str::check_extra_desc] = "Cherche de contenu additionnel après l'installation.",
			[str::proxy_desc] = "Configure a proxy server for 3hs. This setting is for advanced users.",
			[str::install_all] = "Tout installer", /* context: install all items in queue */
			[str::install_no_base] = "Le jeu de base n'est pas installé. Continuer quand même?",
			[str::warn_no_base] = "Avertir si le jeu de base n'est pas installé",
			[str::warn_no_base_desc] = "Montre un message avant d'installer une mise à jour/DLC si le jeu de base n'est pas installé.",
			[str::replaying_errors] = "Remontre les erreurs encontrés dans la liste d'attente.",
			[str::log] = "Logs",
			[str::upload_logs] = "Envoyer les logs",
			[str::clear_logs] = "Supprimer les logs",
			[str::found_missing] = "%1 titre(s) manquant a été trouvé\nVérifiez la liste d'attente pour les installer.", // %1 = amount of titles found
			[str::found_0_missing] = "Aucun titre manquant n'a été trouvé.",
			[str::max_elogs] = "Nombre maximum de fichiers log",
			[str::max_elogs_desc] = "Choisissez le nombre de fichiers logs à conserver. Les valeurs possibles sont comprises entre 0 et 255, où 0 ne conserve pas de fichiers log additionnels.",
			[str::elogs_hint] = "Valeur entre 0 et 255",
			[str::log_id] = "Utilisez cet ID pour obtenir de l'aide:\n%1", // %1 = id
			[str::block] = "bloc",
			[str::blocks] = "blocs",
			[str::search_text] = "Recherche par texte",
			[str::search_id] = "Recherche par l'ID hshop",
			STUB(search_tid),
			STUB(invalid_tid),
			STUB(theme_installer_tid_bad),
			STUB(enter_lgy_query),
			STUB(no_other_params_tid),
			STUB(both_sd_and_sb),
			STUB(invalid_sb),
			STUB(invalid_sd),
			STUB(invalid_includes),
			STUB(invalid_excludes),
			STUB(filter_overlap),
			STUB(lgy_search),
		},

	[lang::french_canada] =
		{
			[str::banner]  = "Le service d'préservation ultime de la 3DS",
			[str::loading] = "Chargement...",
			[str::luma_not_installed] = "Version trop veille pour cette appli.",
			[str::install_luma] = "3hs n'peut pas être utilisé sur cette console.",
			[str::queue] = "Queue",
			[str::connect_wifi] = "Connecte-toi au WiFi et relance l'appli.",
			[str::fail_init_networking] = "Réseau non trouvé",
			[str::fail_fetch_index] = "Index non trouvée\n%1",
			[str::credits_thanks] =
				"Merci d'avoir utilisé 3hs.\n"
				"Vous pouvez télécharger cette appli sur\n"
				"https://hshop.erista.me/",
			[str::credits_names] = "",
			[str::press_to_install] =
				"Appuie " UI_GLYPH_A " pour installer.\n"
				"Appuie " UI_GLYPH_B " pour revenir.",
			[str::version] = "Version",
			[str::prodcode] = "Product Code",
			[str::size] = "Taille",
			[str::name] = "Nom",
			[str::tid] = "Title ID",
			[str::category] = "Catégorie",
			[str::landing_id] = "ID hShop",
			[str::description] = "Déscription",
			[str::total_titles] = "Titres en totales",
			[str::select_cat] = "Sélectionne une catégorie",
			[str::select_subcat] = "Sélectionne une sous-catégorie",
			[str::select_title] = "Sélectionne un titre",
			[str::no_cats_index] = "Aucune catégories dans index (?)",
			[str::empty_subcat] = "Sous-catégorie vide (?)",
			[str::empty_cat] = "Catégorie vide (?)",
			[str::fmt_24h] = "24 heures",
			[str::fmt_12h] = "12 heures",
			[str::unknown] = "Inconnu",
			[str::btrue] = "Vrai",
			[str::bfalse] = "Faux",
			[str::top] = "En haut",
			[str::bottom] = "En bas",
			[str::light_mode] = "Mode clair",
			[str::resume_dl] = "Reprendre les téléchargements",
			[str::load_space] = "Montre l'indicateur d'storage libre",
			[str::show_battery] = "Montre l'indicateur du niveau d'batterie ",
			[str::time_format] = "Format de temps",
			[str::progbar_screen] = "Position d'la barre d'progréssion",
			[str::language] = "Langage",
			[str::value_x] = "Valeur: %1",
			[str::back] = "Retour", /* context: back button */
			[str::invalid] = "Non valide",
			[str::title_doesnt_exist] = "Ce titre n'existe pas: %1",
			[str::fail_create_tex] = "Échoue de création de tex",
			[str::fail_load_smdh_icon] = "load_smdh_icon(): SMDHIconType invalide",
			[str::netcon_lost] = "Connection perdue (%1).\nOn recommence dans $t secondes...",
			[str::about_app] = "À propos",
			[str::help_manual] = "Aide/Manuel",
			[str::find_missing_content] = "Trouve le contenu perdu",
			[str::press_a_exit] = "Appuie " UI_GLYPH_A " pour quitter.", /* context: exit menu and sometimes application */
			[str::fatal_panic] = "Une erreur fatal s'est produit.", /* context: can also be translated as fatal error */
			[str::failed_open_seeddb] = "Arrive pas à ouvrir seeddb.bin.",
			[str::update_to] = "Veux-tu mettre à jour à %1?",
			[str::search_content] = "Cherche du contenu",
			[str::search_content_action] = "Recherche du contenu...",
			[str::results_query] = "Résultats de la requête \"%1\"", /* %1: search query */
			[str::result_code] = "Résultat du code: %1", /* %1: result code */
			[str::level] = "Niveau: %1", /* %1: (english) level name + level code */
			[str::summary] = "Résumé: %1", /* %1 (english) summary name + level code */
			[str::module] = "Module: %1", /* %1 (english) module name + module code */
			[str::hs_bunny_found] = "Bravo! T'as trouvé le lapin hShop!", /* context: easter egg, see also: gfx/img/bun.png */
			[str::already_installed_reinstall] = "Titre déjà installé. Réinstaller?",
			[str::queue_empty] =
				"La queue est vide\n"
				"Appuie " UI_GLYPH_A " pour partir en avant.\n"
				"Astuce: Appuie " UI_GLYPH_Y " pour ajouter une titre à la queue.",
			[str::cancel] = "Annuler",
			[str::confirm] = "Confirme",
			[str::invalid_proxy] = "Paramètres proxy invalides",
			[str::more_about_content] = "Plus à propos de ce contenu",
			[str::lumalocalemode] = "mode LumaLocale",
			[str::automatic] = "automatique",
			[str::manual] = "manuel",
			[str::disabled] = "désactivé",
			[str::patching_reboot] =
				"Luma3DS Game Patching est maintenant activé.\n"
				"La console doit-être redémarré pour que ce changement marche.",
			[str::reboot_now] = "Redémarre maintenant?",
			[str::this_version] = "La version de 3hs %1", /* %1 = version int "version desc" */
			[str::retry_req] = "Requête en échec. Réssaie?", /* context: API request */
			[str::search_zero_results] = "Votre recherche ne retrouve aucun résultat.\nPress " UI_GLYPH_A " to go back.",
			[str::credits] = "Crédits",
			[str::extra_content] = "Ajouter du contenu additionel à la queue?", /* context: i.e. DLC/Updates */
			[str::check_extra] = "Recherche du contenu additionel", /* context: i.e. DLC/Updates */
			[str::no_req] = "Aucune requête pour l'instant", /* context: hlink requests */
			[str::invalid_query] = "Requête invalide\nPress " UI_GLYPH_A " to go back.",
			[str::min_constraint] = "La Version actuelle de 3hs %1 est plus basse que %2",
			[str::proxy] = "Proxy",
			[str::none] = "(none)", /* context: used for no proxy set */
			[str::press_a_to_view] = "Appuie " UI_GLYPH_A " pour voir.", /* context: view proxy settings */
			[str::host] = "Hôte", /* context: proxy */
			[str::port] = "Port", /* context: proxy */
			[str::username] = "Utilisateur", /* context: proxy */
	 		[str::password] = "Mot de passe", /* context: proxy */
			[str::clear] = "Supprimer", /* context: clear the contents of a field */
			[str::progbar_screen_desc] = "Sélectionne la position d'la barre de progression.",
			[str::light_mode_desc] = "Active/Désactive le mode Clair. ça change le GUI de l'application.",
			[str::resume_dl_desc] = "Reprendre les téléchargement après l'interruption de réseau.",
			[str::load_space_desc] = "Active/Désactive l'indicateur d'storage libre.",
			[str::show_battery_desc] = "Active/Désactive l'indicateur du niveau d'batterie.",
			[str::time_format_desc] = "Choisis le temps de format.",
			[str::language_desc] = "Mets le langage pour 3hs. L'appli doit-être redémarré pour que le langage séléctionné prend effect.",
			[str::lumalocalemode_desc] = "Mets le mode du LumaLocale autosetter. Automatique sélectionne un langage automatiquement. Si elle est mis en manuel, 3hs va vous demander pour la région après l'installation du contenu.",
			[str::check_extra_desc] = "Recherche de contenu additionel après l'installation du contenu.",
			[str::proxy_desc] = "Configure un serveur proxy pour 3hs. Ce paramètre est pour les utilisateurs avancés.",
			[str::install_all] = "Installe tout", /* context: install all items in queue */
			[str::install_no_base] = "Le jeu n'est pas installé. Continuez malgré?",
			[str::warn_no_base] = "Avertir si le jeu de base n'est pas installé",
			[str::warn_no_base_desc] = "Montre un message avant d'installer une mise à jour/DLC si le jeu de base n'est pas installé.",
			[str::replaying_errors] = "Remontre les erreurs encontrés dans la requête.",
			[str::log] = "Logs", /* context: may also be translated as "manage logs" */
			[str::upload_logs] = "Envoye les logs",
			[str::clear_logs] = "Supprime les logs",
			[str::found_missing] = "%1 Titre(s) perdu(s) a été trouvé\nVérifiez la requête pour les installer.", // %1 = amount of titles found
			[str::found_0_missing] = "Aucun titre perdu n'a été trouvé.",
			[str::max_elogs] = "Nombre maximum de fichiers log",
			[str::max_elogs_desc] = "Choisissez le nombre de fichiers logs à conserver. Les valeurs possibles sont comprises entre 0 et 255, où 0 ne conserve pas de fichiers log additionnels.",
			[str::elogs_hint] = "Valuer entre 0 et 255",
			[str::log_id] = "Utilisez cet ID pour obtenir de l'aide:\n%1", /* %1 = id */
			[str::block] = "bloc", /* context: nintendo blocks, use official nintendo translation if available */
			[str::blocks] = "blocs", /* context: see above, just in plural this time */
			[str::search_text] = "Recherce par text",
			[str::search_id] = "Recherche par ID hShop",
			STUB(search_tid),
			STUB(invalid_tid),
			STUB(theme_installer_tid_bad),
			STUB(enter_lgy_query),
			STUB(no_other_params_tid),
			STUB(both_sd_and_sb),
			STUB(invalid_sb),
			STUB(invalid_sd),
			STUB(invalid_includes),
			STUB(invalid_excludes),
			STUB(filter_overlap),
			STUB(lgy_search),
		},

		[lang::romanian] =
		{
			[str::banner] = "Serviciul suprem de prezervare al conținutului pentru 3DS",
			[str::loading] = "Se încarcă...",
			[str::luma_not_installed] = "O versiune de sistem nesuportată a fost detetcată.",
			[str::install_luma] = "3hs nu poate fi folosit pe acest sistem.",
			[str::queue] = "Coadă",
			[str::connect_wifi] = "Vă rugăm să vă conectați la Wi-Fi şi să reporniți aplicația.",
			[str::fail_init_networking] = "Nu s-au putut inițializa serviciile de rețea",
			[str::fail_fetch_index] = "Nu s-a putut obține indexul\n%1",
			[str::credits_thanks] =
				"Mulțumim pentru că ați folosit 3hs, un client 3DS pentru hShop.\n"
				"Puteți obține această aplicație pe gratis la\n"
				"https://hshop.erista.me/",
			[str::credits_names] = "",
			[str::press_to_install] =
				"Apăsați " UI_GLYPH_A " pentru a instala acest conținut.\n"
				"Apăsați " UI_GLYPH_B " pentru a vă întoarce.",
			[str::version] = "Versiune",
			[str::prodcode] = "Cod de Produs",
			[str::size] = "Mărime",
			[str::name] = "Nume",
			[str::tid] = "ID de titlu",
			[str::category] = "Categorie",
			[str::landing_id] = "ID hShop",
			[str::description] = "Descriere",
			[str::total_titles] = "Totalul Titlurilor",
			[str::select_cat] = "Selectați o categorie",
			[str::select_subcat] = "Selectați o subcategorie",
			[str::select_title] = "Selectați un titlu",
			[str::no_cats_index] = "no categories in index (?)",
			[str::empty_subcat] = "Empty subcategory (?)",
			[str::empty_cat] = "Empty category (?)",
			[str::fmt_24h] = "24 de ore",
			[str::fmt_12h] = "12 ore",
			[str::unknown] = "necunoscut/ă",
			[str::btrue] = "adevărat",
			[str::bfalse] = "fals",
			[str::top] = "sus",
			[str::bottom] = "jos",
			[str::light_mode] = "Mod luminos",
			[str::resume_dl] = "Reluați descărcările",
			[str::load_space] = "Afişați indicator de spațiu liber",
			[str::show_battery] = "Afişați indicator de baterie",
			[str::time_format] = "Format de timp",
			[str::progbar_screen] = "Poziția bării de progres",
			[str::language] = "Limbă",
			[str::value_x] = "Valoare: %1",
			[str::back] = "Înapoi",
			[str::invalid] = "invalid",
			[str::title_doesnt_exist] = "Titlul nu există: %1",
			[str::fail_create_tex] = "Crearea de texturi a eşuat",
			[str::fail_load_smdh_icon] = "load_smdh_icon(): invalid SMDHIconType",
			[str::netcon_lost] = "Conexiunea la internet s-a pierdut (%1).\nSe va reîncerca în $t secunde...",
			[str::about_app] = "Despre",
			[str::help_manual] = "Ajutor/Manual",
			[str::find_missing_content] = "Găsiți conținut lipsă",
			[str::press_a_exit] = "Apăsați " UI_GLYPH_A " pentru a ieşi.",
			[str::fatal_panic] = "O panică fatală a avut loc.",
			[str::failed_open_seeddb] = "Deschiderea seeddb.bin a eşuat.",
			[str::update_to] = "Vreți să actualizați la %1?",
			[str::search_content] = "Căutați conținut",
			[str::search_content_action] = "Căutați conținut...",
			[str::results_query] = "Resultate ale căutării \"%1\"",
			[str::result_code] = "Cod rezultat: %1",
			[str::level] = "Nivel: %1",
			[str::summary] = "Sumar: %1",
			[str::module] = "Modul: %1",
			[str::hs_bunny_found] = "Felicitări! Ai găsit iepuraşul hShop!",
			[str::already_installed_reinstall] = "Titlu deja instalat. Reinstalați?",
			[str::queue_empty] =
				"Coada este goală\n"
				"Apăsați " UI_GLYPH_A " pentru a vă întoarce.\n"
				"Pont: Apăsați " UI_GLYPH_Y " pentru a adăuga un titlu la coadă.",
			[str::cancel] = "Anulați",
			[str::confirm] = "Confirmați",
			[str::invalid_proxy] = "Setări proxy invalide",
			[str::more_about_content] = "Mai multe despre acest conținut",
			[str::lumalocalemode] = "Modul LumaLocale",
			[str::automatic] = "automat",
			[str::manual] = "manual",
			[str::disabled] = "oprit",
			[str::patching_reboot] =
				"Luma3DS Game Patching este acum pornit.\n"
				"Sistemul trebuie repornit\npentru ca această schimbare să aibă efect.",
			[str::reboot_now] = "Reporniți acum?",
			[str::this_version] = "Acesta este 3hs versiunea %1", // %1 = version int "version desc"
			[str::retry_req] = "Cerere eşuată. Reîncercați?",
			[str::search_zero_results] = "Căutarea dvs. nu a dat rezultate.\nApăsați " UI_GLYPH_A " pentru a vă întoarce.",
			[str::credits] = "Mulțumiri",
			[str::extra_content] = "Adăugați conținut extra la coadă?",
			[str::check_extra] = "Căutați conținut extra",
			[str::no_req] = "Nici o cerere făcută încă", /* context: hlink requests */
			[str::invalid_query] = "Căutare invalidă\nApăsați " UI_GLYPH_A " pentru a vă întoarce.",
			[str::min_constraint] = " Versiunea curentă a 3hs %1 este mai mică decât versiunea necesară %2",
			[str::proxy] = "Proxy",
			[str::none] = "(nimic)",
			[str::press_a_to_view] = "Apăsați " UI_GLYPH_A " pentru a vizualiza.",
			[str::host] = "Gazdă",
			[str::port] = "Port",
			[str::username] = "Nume de Utilizator",
			[str::password] = "Parlă",
			[str::clear] = "Curățați", /* context: clear the contents of a field */
			[str::progbar_screen_desc] = "Selectați poziția bării de progres.",
			[str::light_mode_desc] = "Porniți/opriți modul luminos. Acesta va schimba felul în care arată majoritatea elementelor UI.",
			[str::resume_dl_desc] = "Reluați descărcările după o întrerupere de rețea.",
			[str::load_space_desc] = "Afişați indicatorul de progres.",
			[str::show_battery_desc] = "Afişați indicatorul de baterie.",
			[str::time_format_desc] = "Selectați formatul orar.",
			[str::language_desc] = "Setați limba a 3hs. Aplicația trebuie repornită pentru ca limba selectată să intre în efect.",
			[str::lumalocalemode_desc] = "Setați modul autosetatorului LumaLocale. Automat alege o limbă automat. Dacă este setat la manual, 3hs va solicita o regiune după instalarea conținutului.",
			[str::check_extra_desc] = "Căutați conținut extra după instalare.",
			[str::proxy_desc] = "Configurați un server proxy pentru 3hs. Această setare este pentru utilizatorii avansați.",
			[str::install_all] = "Instalați tot", /* context: install all items in queue */
			[str::install_no_base] = "Jocul de bază nu este instalat. Continuați totuşi?",
			[str::warn_no_base] = "Avertizează dacă un joc de bază nu a fost instalat",
			[str::warn_no_base_desc] = "Afişează un mesaj înainte de a instala un conținut de actualizare sau DLC dacă jocul de bază nu este instalat.",
			[str::replaying_errors] = "Se reiau erori apărute în timpul procesării cozii.",
			[str::log] = "Registre",
			[str::upload_logs] = "Încărcați registre",
			[str::clear_logs] = "ştergeți registre",
			[str::found_missing] = "S-au găsit %1 titluri care lipsesc\nVerificați coada pentru a le instala.", // %1 = amount of titles found
			[str::found_0_missing] = "Niciun titlu lipsă nu a fost găsit.",
			[str::max_elogs] = "Numărul maxim de registre vechi",
			[str::max_elogs_desc] = "Setați numărul de registre pe care să le țineți. Valorile posibile sunt de la 0 la 255, unde 0 nu reține niciun fişier de registru.",
			[str::elogs_hint] = "Valoare între 0 şi 255",
			[str::log_id] = "Folosiți acest ID pentru a obține suport:\n%1", // %1 = id
			[str::block] = "bloc",
			[str::blocks] = "blocuri",
			[str::search_text] = "Căutați după text",
			[str::search_id] = "Căutați după ID hShop",
			STUB(search_tid),
			STUB(invalid_tid),
			STUB(theme_installer_tid_bad),
			STUB(enter_lgy_query),
			STUB(no_other_params_tid),
			STUB(both_sd_and_sb),
			STUB(invalid_sb),
			STUB(invalid_sd),
			STUB(invalid_includes),
			STUB(invalid_excludes),
			STUB(filter_overlap),
			STUB(lgy_search),
		},

	[lang::italian] =
		{
			[str::banner] = "Il servizio definitivo per preservare i titoli 3DS",
			[str::loading] = "Caricamento...",
			[str::luma_not_installed] = "È stato trovato un firmware non supportato.",
			[str::install_luma] = "3hs non può essere utilizzato su questo sistema.",
			[str::queue] = "Coda",
			[str::connect_wifi] = "Per favore connettiti al Wi-Fi e riavvia l'app.",
			[str::fail_init_networking] = "Impossibile inizializzare la rete",
			[str::fail_fetch_index] = "Impossibile trovare l'indice\n%1",
			[str::credits_thanks] =
				"Grazie per aver scelto 3hs, app hShop per 3DS.\n"
				"Puoi prendere gratis questo software a\n"
				"https://hshop.erista.me/",
			[str::credits_names] = "", /* unused */
			[str::press_to_install] =
				"Premi " UI_GLYPH_A " per installare questo contenuto.\n"
				"Premi " UI_GLYPH_B " per annullare.",
			[str::version] = "Versione",
			[str::prodcode] = "Codice Prodotto",
			[str::size] = "Dimensioni",
			[str::name] = "Nome",
			[str::tid] = "ID Titolo",
			[str::category] = "Categoria",
			[str::landing_id] = "hShop ID",
			[str::description] = "Descrizione",
			[str::total_titles] = "Totale Titoli",
			[str::select_cat] = "Seleziona una categoria",
			[str::select_subcat] = "Seleziona una sottocategoria",
			[str::select_title] = "Seleziona un titolo",
			[str::no_cats_index] = "nessuna categoria all'indice (?)",
			[str::empty_subcat] = "Sottocategoria vuota (?)",
			[str::empty_cat] = "Categoria vuota (?)",
			[str::fmt_24h] = "24 ore",
			[str::fmt_12h] = "12 ore",
			[str::unknown] = "sconosciuto",  //"sconosciuta" may fit the context better.
			[str::btrue] = "sì",
			[str::bfalse] = "no",
			[str::top] = "superiore",
			[str::bottom] = "inferiore",
			[str::light_mode] = "Modalità chiara",
			[str::resume_dl] = "Riprendi i download",
			[str::load_space] = "Mostra l'indicatore di spazio libero",
			[str::show_battery] = "Mostra lo stato della batteria",
			[str::time_format] = "Formato dell'ora",
			[str::progbar_screen] = "Posizione barra di avanzamento",
			[str::language] = "Lingua",
			[str::value_x] = "Valore: %1",
			[str::back] = "Indietro",
			[str::invalid] = "non valido",
			[str::title_doesnt_exist] = "Il titolo non esiste: %1",
			[str::fail_create_tex] = "Impossibile creare tex",
			[str::fail_load_smdh_icon] = "load_smdh_icon(): SDMHIconType non valido",
			[str::netcon_lost] = "Connessione alla rete persa (%1).\nRiprovo fra $t secondi...",
			[str::about_app] = "Riguardo l'app",
			[str::help_manual] = "Aiuto/Manuale",
			[str::find_missing_content] = "Trova contenuti mancanti",
			[str::press_a_exit] = "Premi " UI_GLYPH_A " per uscire.",
			[str::fatal_panic] = "Si è verificato un fatale errore di panico.",
			[str::failed_open_seeddb] = "Impossibile aprire seeddb.bin.",
			[str::update_to] = "Vuoi aggiornare a %1?",
			[str::search_content] = "Cerca contenuto",
			[str::search_content_action] = "Cercando dei contenuti...",
			[str::results_query] = "Risultati per \"%1\"",
			[str::result_code] = "Codice risultante: %1",
			[str::level] = "Livello: %1",
			[str::summary] = "Riepilogo: %1",
			[str::module] = "Modulo: %1",
			[str::hs_bunny_found] = "Congratulazioni! Hai trovato il coniglio hShop!",
			[str::already_installed_reinstall] = "Titolo già installato. Reinstallare?",
			[str::queue_empty] =
				"La coda è vuota\n"
				"Premi " UI_GLYPH_A " per tornare indietro.\n"
				"Premi " UI_GLYPH_Y " per mettere in coda un titolo.",
			[str::cancel] = "Annulla",
			[str::confirm] = "Conferma",
			[str::invalid_proxy] = "Impostazioni del proxy non valide",
			[str::more_about_content] = "Di più su questo contenuto",
			[str::lumalocalemode] = "Modalità LumaLocale",
			[str::automatic] = "automatico",
			[str::manual] = "manuale",
			[str::disabled] = "disabilitato",
			[str::patching_reboot] =
				"Luma3DS Game Patching è ora attivo.\n"
				"Il sistema dev'essere riavviato perché i cambiamenti abbiano effetto.",
			[str::reboot_now] = "Riavviare adesso?",
			[str::this_version] = "Questa è la versione di 3hs %1",
			[str::retry_req] = "Richiesta fallita. Riprovare?",
			[str::search_zero_results] = "La tua ricerca non ha prodotto risultati.\nPremi" UI_GLYPH_A "per tornare indietro.",
			[str::credits] = "Crediti",
			[str::extra_content] = "Aggiungere un contenuto extra alla coda?",
			[str::check_extra] = "Cerca contenuti extra",
			[str::no_req] = "Nessuna richiesta",
			[str::invalid_query] = "Richiesta non valida\nPress" UI_GLYPH_A "per tornare indietro",
			[str::min_constraint] = "Questa versione di 3hs %1 è più vecchia di quella richiesta %2",
			[str::proxy] = "Proxy",
			[str::none] = "(nessuno)",
			[str::press_a_to_view] = "Premi " UI_GLYPH_A " per vedere.",
			[str::host] = "Host",
			[str::port] = "Porta",
			[str::username] = "Nome utente",
			[str::password] = "Password",
			[str::clear] = "Cancella",
			[str::progbar_screen_desc] = "Scegli la posizione della barra di avanzamento.",
			[str::light_mode_desc] = "Attiva/Disattiva la modalità chiara. Questo cambierà il colore degli elementi dell'interfaccia.",
			[str::resume_dl_desc] = "Riprendi i download dopo un'interruzione della connessione alla rete.",
			[str::load_space_desc] = "Scegli se visualizzare l'indicatore dello spazio libero.",
			[str::show_battery_desc] = "Scegli se visualizzare lo stato della batteria.",
			[str::time_format_desc] = "Seleziona il formato delle ore.",
			[str::language_desc] = "Imposta la linugua di 3hs. L'applicazione dev'essere riavviata perché il cambiamento abbia effetto.",
			[str::lumalocalemode_desc] = "Imposta la modalità del LumaLocale autosetter. 'Automatico' seleziona la lingua automaticamente. Se viene scelto 'manuale', 3hs chiederà la regione dopo l'installazione del contenuto.",
			[str::check_extra_desc] = "Cerca dei contenuti extra dopo l'installazione.",
			[str::proxy_desc] = "Configura un server proxy per 3hs. Questa impostazione è per gli utenti avanzati.",
			[str::install_all] = "Installa tutto",
			[str::install_no_base] = "Il gioco di base non è installato. Continuare ugualmente?",
			[str::warn_no_base] = "Avvisa se il gioco di base non è installato.",
			[str::warn_no_base_desc] = "Mostra un messaggio prima di installare un aggiornamento on un DLC se il gioco di base non è installato.",
			[str::replaying_errors] = "Errori di riproduzione rilevati durante l'elaborazione della coda.",
			[str::log] = "Gestisci i registri",
			[str::upload_logs] = "Registri di upload",
			[str::clear_logs] = "Cancella registri",
			[str::found_missing] = "Trovato/i %1 titolo/i mancanti\nControlla la coda per installarli.",
			[str::found_0_missing] = "Non sono stati trovati titoli mancanti.",
			[str::max_elogs] = "Massimi file di registro",
			[str::max_elogs_desc] = "Imposta il numero di file di registro da mantenere. Da 0 a 255, dove 0 non mantiene nessun registro in più.",
			[str::elogs_hint] = "Valore tra 0 e 255",
			[str::log_id] = "Usa questo ID per ottenere supporto:\n%1",
			[str::block] = "blocco",
			[str::blocks] = "blocchi",
			[str::search_text] = "Cerca tramite testo",
			[str::search_id] = "Cerca tramite ID hShop",
			STUB(search_tid),
			STUB(invalid_tid),
			STUB(theme_installer_tid_bad),
			STUB(enter_lgy_query),
			STUB(no_other_params_tid),
			STUB(both_sd_and_sb),
			STUB(invalid_sb),
			STUB(invalid_sd),
			STUB(invalid_includes),
			STUB(invalid_excludes),
			STUB(filter_overlap),
			STUB(lgy_search),
		},

	[lang::portuguese] =
		{
			[str::banner] = "O supremo serviço de preservação do conteúdo de 3DS", /* unused */
			[str::loading] = "Carregando...",
			[str::luma_not_installed] = "Uma versão de sistema não suportada foi detetada.",
			[str::install_luma] = "O 3hs não consegue ser usado neste sistema.",
			[str::queue] = "Fila",
			[str::connect_wifi] = "Porfavor conecte à wifi e reconecte.", /* unused */
			[str::fail_init_networking] = "Falha ao iniciar a rede",
			[str::fail_fetch_index] = "falha ao buscar índice\n%1", /* unused */
			[str::credits_thanks] =
				"Obrigado por usares a 3hs, um cliente de 3ds para a hShop.\n"
				"Podes ter este programa de graça pelo\n"
				"https://hshop.erista.me/",
			[str::credits_names] = "", /* unused */
			[str::press_to_install] =
				"Pressiona " UI_GLYPH_A " para instalar este conteúdo.\n"
				"Pressiona " UI_GLYPH_B " para voltar.",
			[str::version] = "Versão",
			[str::prodcode] = "Product Code",
			[str::size] = "Tamanho",
			[str::name] = "Nome",
			[str::tid] = "ID do Jogo",
			[str::category] = "Categoria",
			[str::landing_id] = "ID hShop",
			[str::description] = "Descrição",
			[str::total_titles] = "Total de Jogos",
			[str::select_cat] = "Seleciona a categoria",
			[str::select_subcat] = "Seleciona a sub-categoria",
			[str::select_title] = "Seleciona um jogo",
			[str::no_cats_index] = "sem categorias no indíce (?)", /* unused */
			[str::empty_subcat] = "Sub-categoria vazia (?)", /* inutilizada */
			[str::empty_cat] = "Categoria vazia (?)", /* inutilizada */
			[str::fmt_24h] = "24 horas", /* context: time format */
			[str::fmt_12h] = "12 horas", /* context: time format */
			[str::unknown] = "desconhecido", /* context: unknown setting */
			[str::btrue] = "ligado", /* context: setting is set to ON */
			[str::bfalse] = "desligado", /* context: setting is set to OFF */
			[str::top] = "cima", /* context: top screen */
			[str::bottom] = "baixo", /* context: bottom screen */
			[str::light_mode] = "Modo escuro",
			[str::resume_dl] = "Resumir downloads",
			[str::load_space] = "Mostrar indicadores de espaço livre",
			[str::show_battery] = "Mostrar indicador do nivel de bateria",
			[str::time_format] = "Formato do tempo",
			[str::progbar_screen] = "Posição da barra de progresso",
			[str::language] = "Linguagem",
			[str::value_x] = "Valor: %1", /* context: value of setting */
			[str::back] = "Voltar", /* context: back button */
			[str::invalid] = "invalido",
			[str::title_doesnt_exist] = "Jogo não existe: %1", /* %1: title id */
			[str::fail_create_tex] = "Falha ao criar texturas", /* unused */
			[str::fail_load_smdh_icon] = "load_smdh_icon(): invalid SMDHIconType", /* unused */
			[str::netcon_lost] = "Conexão com rede perdida (%1).\nTentando novamente em $t segundos...", /* %1: error code, $t: seconds remaining */
			[str::about_app] = "Sobre",
			[str::help_manual] = "Ajuda/Manual",
			[str::find_missing_content] = "Encontre conteúdo em falta",
			[str::press_a_exit] = "Pressione " UI_GLYPH_A " para sair.", /* context: exit menu and sometimes application */
			[str::fatal_panic] = "Um erro fatal ocorreu.", /* context: can also be translated as fatal error */
			[str::failed_open_seeddb] = "Falha ao abrir seeddb.bin.",
			[str::update_to] = "Queres atualizar para %1?", /* %1: new version, i.e. 1.0.2 */
			[str::search_content] = "Pesquise conteúdo",
			[str::search_content_action] = "Pesquise conteúdo...",
			[str::results_query] = "Resultados da pesquisa \"%1\"", /* %1: search query */
			[str::result_code] = "Codigo do resultado: %1", /* %1: result code */
			[str::level] = "Nivel: %1", /* %1: (english) level name + level code */
			[str::summary] = "Resumo: %1", /* %1 (english) summary name + level code */
			[str::module] = "Modulo: %1", /* %1 (english) module name + module code */
			[str::hs_bunny_found] = "Parabens! Encontraste o coelho do hShop!", /* context: easter egg, see also: gfx/img/bun.png */
			[str::already_installed_reinstall] = "Jogo já instalado. Reinstalar?",
			[str::queue_empty] =
				"A fila está vazia\n"
				"Pressione " UI_GLYPH_A " para voltar.\n"
				"Dica: Pressione " UI_GLYPH_Y " para adicionar um jogo a fila.",
			[str::cancel] = "Cancelar",
			[str::confirm] = "Confirmar",
			[str::invalid_proxy] = "Opções de Proxy invalidas",
			[str::more_about_content] = "Mais sobre este conteúdo",
			[str::lumalocalemode] = "Modo de LumaLocale",
			[str::automatic] = "automatico",
			[str::manual] = "manual",
			[str::disabled] = "desligado",
			[str::patching_reboot] =
				"Luma3DS Game Patching está ligado.\n"
				"A consola tem de ser reinicia para as mudanças tomarem efeito.",
			[str::reboot_now] = "Reiniciar agora?",
			[str::this_version] = "Está na versão 3hs %1", /* %1 = version int "version desc" */
			[str::retry_req] = "Pedido falhado. Tentar novamente?", /* context: API request */
			[str::search_zero_results] = "A pesquisa não retornou resultados.\nPressione " UI_GLYPH_A " para voltar.",
			[str::credits] = "Creditos",
			[str::extra_content] = "Adicionar conteúdo extra à fila?", /* context: i.e. DLC/Updates */
			[str::check_extra] = "Pesquisar por conteúdo extra?", /* context: i.e. DLC/Updates */
			[str::no_req] = "Nenhum pedido feito até agora", /* context: hlink requests */
			[str::invalid_query] = "Pesquisa invalida\nPressiona " UI_GLYPH_A " para voltar.",
			[str::min_constraint] = "Atual versão 3hs %1 é menor que a versão requerida %2",
			[str::proxy] = "Proxy",
			[str::none] = "(none)", /* context: used for no proxy set */
			[str::press_a_to_view] = "Pressione " UI_GLYPH_A " para ver.", /* context: view proxy settings */
			[str::host] = "Host", /* context: proxy */
			[str::port] = "Port", /* context: proxy */
			[str::username] = "Nome do usuário", /* context: proxy */
			[str::password] = "Password", /* context: proxy */
			[str::clear] = "Descartar", /* context: clear the contents of a field */
			[str::progbar_screen_desc] = "Selecione a posição da barra de progresso.",
			[str::light_mode_desc] = "Ligue/Desligue o modo escuro. Isto vai mudar como muito elementos de UI aparecem.",
			[str::resume_dl_desc] = "Resumir downloads depois de uma interrupção de rede.",
			[str::load_space_desc] = "Ligue o indicador de espaço livre.",
			[str::show_battery_desc] = "Ligue o indicador do nível de bateria.",
			[str::time_format_desc] = "Selecione o formato de tempo.",
			[str::language_desc] = "Define a linguagem para o 3hs. A app tem de ser reiniciada para a linguagem selecionada tomar efeito.",
			[str::lumalocalemode_desc] = "Define o modo do LumaLocale auto-defenidor. Automaticamente seleciona a linguagem automatica. Se isto for definido para manual, o 3hs vai pedir pela região depois da instalação do conteúdo.",
			[str::check_extra_desc] = "Pesquisar por conteúdo extra depois da instalação.",
			[str::proxy_desc] = "Configurar um server proxy para o 3hs. Esta definição é apenas para utilizadores avançados.",
			[str::install_all] = "Instalar tudo", /* context: install all items in queue */
			[str::install_no_base] = "O jogo base não está instalado. Continuar na mesma?",
			[str::warn_no_base] = "Avisar se o jogo base não está instalado",
			[str::warn_no_base_desc] = "Mostra uma mensagem antes de instalar updates ou DLCs se o jogo base não está instalado.",
			[str::replaying_errors] = "Erros encontrados ao processar a fila.",
			[str::log] = "Gerir registros", /* context: may also be translated as "manage logs" */
			[str::upload_logs] = "Enviar registros",
			[str::clear_logs] = "Descartar registros",
			[str::found_missing] = "Encontrado(s) %1 jogo(s) em falta\nVerifica a fila para os instalar.", // %1 = amount of titles found
			[str::found_0_missing] = "Nenhum jogo em falta encontrado.",
			[str::max_elogs] = "Maximo de ficheiros de registro antigos",
			[str::max_elogs_desc] = "Define o numero de ficheiros de registro para manter. Valores possiveis são de 0 a 255, 0 não mantém ficheiros de registro adicionais.",
			[str::elogs_hint] = "Valor entre 0 e 255",
			[str::log_id] = "Usa este ID para obter supporte:\n%1", /* %1 = id */
			[str::block] = "bloco", /* context: nintendo blocks, use official nintendo translation if available */
			[str::blocks] = "blocos", /* context: see above, just in plural this time */
			[str::search_text] = "Pesquisa por texto",
			[str::search_id] = "Pesquisa por ID hShop",
			STUB(search_tid),
			STUB(invalid_tid),
			STUB(theme_installer_tid_bad),
			STUB(enter_lgy_query),
			STUB(no_other_params_tid),
			STUB(both_sd_and_sb),
			STUB(invalid_sb),
			STUB(invalid_sd),
			STUB(invalid_includes),
			STUB(invalid_excludes),
			STUB(filter_overlap),
			STUB(lgy_search),
		},

	[lang::korean] =
		{
			/*if () has been used on a translated word it means I'm not happy with it but can't find an alternative*/
			[str::banner] = "The ultimate 3DS content preservation service", /* unused */
			[str::loading] = "로딩중...",
			[str::luma_not_installed] = "사용가능하지 않은 펌웨어 버전입니다.",
			[str::install_luma] = "이 시스템에 3hs를 사용할 수 없습니다.",
			[str::queue] = "대기열",
			[str::connect_wifi] = "Wi-Fi에 연결 후 앱을 다시 실행해주세요.", /* unused */
			[str::fail_init_networking] = "앱 실행이 실패했습니다",
			[str::fail_fetch_index] = "Failed to fetch index\n%1", /* unused */
			[str::credits_thanks] =
				"hShop 클라이언트 3hs를 사용해주셔서 감사합니다.\n"
				"https://hshop.erista.me/\n"
				"에서 이 소프트웨어를 무료로 다운로드하실수 있습니다.",
			[str::credits_names] = "", /* unused */
			[str::press_to_install] =
				"" UI_GLYPH_A " : 설치\n"
				"" UI_GLYPH_B " : 뒤로가기",
			[str::version] = "버전",
			[str::prodcode] = "제품 코드",
			[str::size] = "크기",
			[str::name] = "이름",
			[str::tid] = "소프트웨어 ID",
			[str::category] = "카테고리",
			[str::landing_id] = "hShop ID",
			[str::description] = "설명",
			[str::total_titles] = "총 소프트웨어",
			[str::select_cat] = "카테고리를 선택해주세요",
			[str::select_subcat] = "하위 카테고리를 선택해주세요",
			[str::select_title] = "소프트웨어를 선택해주세요",
			[str::no_cats_index] = "색인에 카테고리가 없음 (?)", /* unused */
			[str::empty_subcat] = "비어있는 하위 카테고리 (?)", /* unused */
			[str::empty_cat] = "비어있는 카테고리 (?)", /* unused */
			[str::fmt_24h] = "24시간 형식", /* context: time format */
			[str::fmt_12h] = "12시간 형식", /* context: time format */
			[str::unknown] = "unknown", /* context: unknown setting */
			[str::btrue] = "켜짐", /* context: setting is set to ON */
			[str::bfalse] = "꺼짐", /* context: setting is set to OFF */
			[str::top] = "위", /* context: top screen */
			[str::bottom] = "아래", /* context: bottom screen */
			[str::light_mode] = "밝은 테마",
			[str::resume_dl] = "다운로드 게속하기",
			[str::load_space] = "사용가능한 저장공간 포시",
			[str::show_battery] = "배터리 퍼센트 표시",
			[str::time_format] = "시간 형식",
			[str::progbar_screen] = "다운로드 (진행 표시 줄)",
			[str::language] = "언어",
			[str::value_x] = "값: %1", /* context: value of setting */
			[str::back] = "뒤로", /* context: back button */
			[str::invalid] = "유효하지 않음",
			[str::title_doesnt_exist] = "소프트웨어가 존재하지 않습니다: %1", /* %1: title id */
			[str::fail_create_tex] = "텍스처 생성 실패", /* unused */
			[str::fail_load_smdh_icon] = "load_smdh_icon(): invalid SMDHIconType", /* unused */
			[str::netcon_lost] = "네트워크 연결 실패 (%1).\n $t 초 후 재시도...", /* %1: error code, $t: seconds remaining */
			[str::about_app] = "정보",
			[str::help_manual] = "설명서",
			[str::find_missing_content] = "누락된 콘텐츠 찾기",
			[str::press_a_exit] = "" UI_GLYPH_A ": 나가기", /* context: exit menu and sometimes application */
			[str::fatal_panic] = "에러 발생", /* context: can also be translated as fatal error */
			[str::failed_open_seeddb] = "seeddb.bin 여는데 오류가 있었습니다",
			[str::update_to] = "버전 %1으로 업데이트하시겠습니까?", /* %1: new version, i.e. 1.0.2 */
			[str::search_content] = "콘텐츠 검색중",
			[str::search_content_action] = "콘텐츠 검색중...",
			[str::results_query] = "검색 결과 \"%1\"", /* %1: search query */
			[str::result_code] = "결과 코드: %1", /* %1: result code */
			[str::level] = "Level: %1", /* %1: (english) level name + level code */
			[str::summary] = "Summary: %1", /* %1 (english) summary name + level code */
			[str::module] = "Module: %1", /* %1 (english) module name + module code */
			[str::hs_bunny_found] = "hShop 토끼를 찾았다! ^^", /* context: easter egg, see also: gfx/img/bun.png */
			[str::already_installed_reinstall] = "소프트웨어가 이미 설치되어있습니다. 다시 설치하시겠습니다?",
			[str::queue_empty] =
				"대기열이 비어있습니다\n"
				"" UI_GLYPH_A ": 뒤로가기\n"
				"Tip: " UI_GLYPH_Y " 를 눌러 소프트웨어를 대기열에 추가",
			[str::cancel] = "취소",
			[str::confirm] = "확인",
			[str::invalid_proxy] = "유효하지 않은 프록시 설정",
			[str::more_about_content] = "콘텐츠 설명",
			[str::lumalocalemode] = "LumaLocale 모드",
			[str::automatic] = "자동",
			[str::manual] = "수동",
			[str::disabled] = "꺼짐",
			[str::patching_reboot] =
				"Luma3DS Game Patching이 활성화되었습니다.\n"
				"시스템을 재시작해야 적용이 됩니다.",
			[str::reboot_now] = "지금 재시작할까요?",
			[str::this_version] = "3hs 버전: %1", /* %1 = version int "version desc" */
			[str::retry_req] = "요청 실패. 재시도?", /* context: API request */
			[str::search_zero_results] = "일치하는 검색결과가 없습니다.\n" UI_GLYPH_A ": 뒤로가기.",
			[str::credits] = "제작진",
			[str::extra_content] = "DLC/업데이트를 대기열에 추가하시겠습니까?", /* context: i.e. DLC/Updates */
			[str::check_extra] = "DLC/업데이트 검색", /* context: i.e. DLC/Updates */
			[str::no_req] = "연결 요청 없음", /* context: hlink requests */
			[str::invalid_query] = "검색결과 없음\n" UI_GLYPH_A ": 뒤로가기",
			[str::min_constraint] = "3hs 버전 %2이 필요합니다. %1에서 업데이트해주세요.",
			[str::proxy] = "프록시",
			[str::none] = "(없음)", /* context: used for no proxy set */
			[str::press_a_to_view] = "" UI_GLYPH_A ": 보기", /* context: view proxy settings */
			[str::host] = "호스트", /* context: proxy */
			[str::port] = "포트", /* context: proxy */
			[str::username] = "유저네임", /* context: proxy */
			[str::password] = "암호", /* context: proxy */
			[str::clear] = "모두 지우기", /* context: clear the contents of a field */
			[str::progbar_screen_desc] = "다운로드 진행 표시 위치 선택",
			[str::light_mode_desc] = "밝은 테마를 켜기/끄기. 대부분의 UI의 색을 변경함",
			[str::resume_dl_desc] = "네트워크",
			[str::load_space_desc] = "사용가능한 저장공잔 표시 겨기/끄기",
			[str::show_battery_desc] = "배터리 퍼센트 표시 켜기/끄기",
			[str::time_format_desc] = "시간 형식을 선택",
			[str::language_desc] = "3hs의 언어를 설정합니다. 변경 후 앱을 재시작해야 적용됩니다.",
			[str::lumalocalemode_desc] = "LumaLocale autosetter 모드 설정. 자동은 자동으로 언어를 선택합니다. 수동으로 설정시, 3hs가 소프트웨어 설치 후 지역을 선택하도록 합니다.",
			[str::check_extra_desc] = "설치 후 DLC/업데이트 검색",
			[str::proxy_desc] = "3hs 프록시 서버 설정. 고급 유저용",
			[str::install_all] = "모두 설치", /* context: install all items in queue */
			[str::install_no_base] = "본게임이 설치되어있지 않습니다. 게속하시겠습니까?",
			[str::warn_no_base] = "본게임이 설치되어 있지 않을 경우 경고",
			[str::warn_no_base_desc] = "본게임이 설치되어 있지 않은 상태로 DLC/업데이트를 설치하려 할 경우 메세지 표시",
			[str::replaying_errors] = "대기열 프로세스 중 에러 보기",
			[str::log] = "로그 관리", /* context: may also be translated as "manage logs" */
			[str::upload_logs] = "로그 업로드하기",
			[str::clear_logs] = "로그 지우기",
			[str::found_missing] = "%1개의 DLC/업데이트 발견\n대기열을 확인해 설치하세요.", // %1 = amount of titles found
			[str::found_0_missing] = "0개의 DLC/업데이트 발견",
			[str::max_elogs] = "저장할 로그 파일량",
			[str::max_elogs_desc] = "저장할 로그 파일량 설정. 0~255까지 저장 가능하고, 0은 저장안함",
			[str::elogs_hint] = "0~255사이 값",
			[str::log_id] = "민원하실때 이 ID를 사용하세요:\n%1", /* %1 = id */
			[str::block] = "블록", /* context: nintendo blocks, use official nintendo translation if available */
			[str::blocks] = "블록", /* context: see above, just in plural this time */
			[str::search_text] = "이름으로 검색",
			[str::search_id] = "hShop ID로 검색",
			STUB(search_tid),
			STUB(invalid_tid),
			STUB(theme_installer_tid_bad),
			STUB(enter_lgy_query),
			STUB(no_other_params_tid),
			STUB(both_sd_and_sb),
			STUB(invalid_sb),
			STUB(invalid_sd),
			STUB(invalid_includes),
			STUB(invalid_excludes),
			STUB(filter_overlap),
			STUB(lgy_search),
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
	case lang::romanian: return LANGNAME_ROMANIAN;
	case lang::french: return LANGNAME_FRENCH;
	case lang::french_canada: return LANGNAME_FRENCH_CANADA;
	case lang::italian: return LANGNAME_ITALIAN;
	case lang::portuguese: return LANGNAME_PORTUGUESE;
	case lang::korean: return LANGNAME_KOREAN;
	}

	return "invalid";
}

// https://www.3dbrew.org/wiki/Country_Code_List
//  only took over those that we actually use
enum class CountryCode {
	canada  = 18,
	romania = 99,
};

lang::type i18n::default_lang()
{
	u8 syslang = 0;
	u8 countryinfo[4];
	if(R_FAILED(CFGU_GetSystemLanguage(&syslang)))
		syslang = CFG_LANGUAGE_EN;
	/* countryinfo */
	if(R_FAILED(CFGU_GetConfigInfoBlk2(4, 0x000B0000, countryinfo)))
		countryinfo[3] = 0;

	switch(countryinfo[3])
	{
	case (int) CountryCode::romania: return lang::romanian;
	}

	switch(syslang)
	{
	case CFG_LANGUAGE_DE: return lang::german;
	case CFG_LANGUAGE_ES: return lang::spanish;
	case CFG_LANGUAGE_NL: return lang::dutch;
	case CFG_LANGUAGE_IT: return lang::italian;
	case CFG_LANGUAGE_KO: return lang::korean;
	case CFG_LANGUAGE_PT: return lang::portuguese;
	case CFG_LANGUAGE_FR:
		return countryinfo[3] == (int) CountryCode::canada
			? lang::french_canada : lang::french;
	case CFG_LANGUAGE_JP: // fallthrough
	case CFG_LANGUAGE_EN: // fallthrough
	case CFG_LANGUAGE_ZH: // fallthrough
	case CFG_LANGUAGE_RU: // fallthrough
	case CFG_LANGUAGE_TW: // fallthrough
	default: return lang::english;
	}
}

