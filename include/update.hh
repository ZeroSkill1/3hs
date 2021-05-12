
#ifndef __update_hh__
#define __update_hh__

#define VERSION "v" FULL_VERSION

#define UP_BASE "http://192.168.2.12/3hs-update" /* TMP */
#define UP_CIA(v) (std::string(UP_BASE) + "/app-" + v + ".cia")
#define UP_VERSION (UP_BASE "/version")

#include <net_common.hh>
#include <string>


std::string trim(const std::string& str, const std::string& whitespace = " \t\n");
std::string get_url_content(std::string from);
std::string get_latest_version_string();
bool update_app();

#endif
