
#ifndef inc_update_hh
#define inc_update_hh


#define UP_BASE "http://download2.erista.me/3hs"
#define UP_VERSION UP_BASE "/version"

#ifdef DEVICE_ID
# define UP_CIA_(v,id) (std::string(UP_BASE) + "/app-" + v + "-" + #id + ".cia")
# define UP_CIA(v,id) UP_CIA_(v,id)
# define DEVID DEVICE_ID
#else
# define UP_CIA_(v,id) (std::string(UP_BASE) + "/app-" + v + ".cia")
# define UP_CIA(v,id) UP_CIA_(v,id)
# define DEVID
#endif

#define VERSION_MAJOR 0
#define VERSION_MINOR 2
#define VERSION_PATCH 0

#define INT_TO_STR(i) INT_TO_STR_(i)
#define INT_TO_STR_(i) #i

#define MK_UA(MA,MI,PA) "hShop (3DS/CTR/KTR; ARMv6) 3hs/" MA "." MI "." PA
#define VERSION INT_TO_STR(VERSION_MAJOR) "." INT_TO_STR(VERSION_MINOR) "." INT_TO_STR(VERSION_PATCH)
#define VVERSION "v" VERSION
#define USER_AGENT MK_UA(INT_TO_STR(VERSION_MAJOR), INT_TO_STR(VERSION_MINOR), INT_TO_STR(VERSION_PATCH))

#include <string>


std::string trim(const std::string& str, const std::string& whitespace = " \t\n");
std::string get_url_content(std::string from);
std::string get_latest_version_string();
bool update_app();

#endif
