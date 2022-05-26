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
			[str::credits_names] = "",
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
			[str::this_version] = "This is 3hs version %1", // %1 = version int "version desc"
			[str::retry_req] = "Request failed. Do you want to retry?",
			[str::search_zero_results] = "Search gave 0 results\nPress " UI_GLYPH_A " to go back",
			[str::credits] = "Credits",
			[str::extra_content] = "Add extra content to queue?",
			[str::check_extra] = "Search for extra content",
			[str::no_req] = "no requests made yet", /* context: hlink requests */
			[str::invalid_query] = "Invalid query\nPress " UI_GLYPH_A " to go back",
			[str::min_constraint] = "Current 3hs version %1 is lower than the required version %2",
			[str::proxy] = "Proxy",
			[str::none] = "(none)",
			[str::press_a_to_view] = "Click " UI_GLYPH_A " to view",
			[str::host] = "Host",
			[str::port] = "Port",
			[str::username] = "Username",
			[str::password] = "Password",
			[str::clear] = "Clear", /* context: clear the contents of a field */
			[str::progbar_screen_desc] = "The screen to draw progress bars on",
			[str::light_mode_desc] = "Turn on light mode. This will change the way most ui elements look",
			[str::resume_dl_desc] = "Should we start where we left off downloading the first time if we failed the first try?",
			[str::load_space_desc] = "Load the free space indicator",
			[str::show_battery_desc] = "Toggle visibility of battery in top right corner",
			[str::time_format_desc] = "Your preferred time format. Either 24h or 12h",
			[str::language_desc] = "The language 3hs is in. Note that to update all text you might need to restart 3hs",
			[str::lumalocalemode_desc] = "The mode LumaLocale autosetter uses. Automatic selects a language automatically. If you choose manual 3hs will prompt for a region after the installation of a content",
			[str::check_extra_desc] = "Search for extra content after an installation.",
			[str::proxy_desc] = "Configure a proxy. If you don't know what you're doing don't touch this",
			[str::install_all] = "Install all", /* context: install all items in queue */
			[str::install_no_base] = "DLC/Update doesn't have the base game installed. Install regardless?",
			[str::warn_no_base] = "Warn if a base game is not installed",
			[str::warn_no_base_desc] = "Give a warning if you're trying to install a DLC/update and the base game is not installed.",
			[str::replaying_errors] = "Replaying errors encountered while processing the queue.",
			[str::log] = "Logs",
			[str::upload_logs] = "Upload logs",
			[str::clear_logs] = "Clear logs",
			[str::found_missing] = "Found %1 missing title(s)\nCheck the queue to install them", // %1 = amount of titles found
			[str::found_0_missing] = "Didn't find any missing titles",
			[str::max_elogs] = "Maximum old log files",
			[str::max_elogs_desc] = "Amount of old log files to keep. Between 0-255 inclusive where 0 keeps no additional log files.",
			[str::elogs_hint] = "Number between 0-255 inclusive",
			[str::log_id] = "Use this ID to get support\n%1", // %1 = id
			[str::block] = "block",
			[str::blocks] = "blocks",
			[str::search_text] = "Search by text",
			[str::search_id] = "Lookup using hShop ID",
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
			[str::log] = "Logs",
			[str::upload_logs] = "Logs uploaden",
			[str::clear_logs] = "Logs verwijderen",
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
			[str::credits_names] = "",
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
				"Tipp: Drücke " UI_GLYPH_Y " um einen Inhalt zur\nWarteschlange hinzuzufügen",
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
			[str::search_id] = "Mithilfe einer hShop ID suchen"
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
			[str::install_no_base] = "La actualización/DLC no tiene el juego base installado. ¿Instalar de todas maneras?",
			[str::warn_no_base] = "Advertir si el juego base no está instalado",
			[str::warn_no_base_desc] = "Dar un aviso si estás intentanto instalar una actualización/DLC y el juego base no está instalado.",
			[str::replaying_errors] = "Reproduciendo errores ocurridos mientras se procesa la cola.",
			[str::log] = "Regustros",
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
			[str::search_id] = "Buscar usando una ID de hShop",
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

