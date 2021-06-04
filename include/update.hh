
#ifndef inc_update_hh
#define inc_update_hh

#define VERSION "v" FULL_VERSION

#define UP_BASE "http://download2.erista.me/3hs"
#define UP_VERSION (UP_BASE "/version")

#ifdef DEVICE_ID
# define UP_CIA(v) (std::string(UP_BASE) + "/app-" + v + "-" + #DEVICE_ID + ".cia")
#else
# define UP_CIA(v) (std::string(UP_BASE) + "/app-" + v + ".cia")
#endif

#include <net_common.hh>
#include <string>


std::string trim(const std::string& str, const std::string& whitespace = " \t\n");
std::string get_url_content(std::string from);
std::string get_latest_version_string();
bool update_app();

#endif
