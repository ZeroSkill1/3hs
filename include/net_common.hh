
#ifndef inc_net_common_hh
#define inc_net_common_hh

#define VERSION_MAJOR 0
#define VERSION_MINOR 1
#define VERSION_PATCH 6

#define INT_TO_STR(i) INT_TO_STR_(i)
#define INT_TO_STR_(i) #i

#define MK_UA(MA,MI,PA) "hShop (3DS/CTR/KTR; ARM64) 3hs/" MA "." MI "." PA
#define MK_CURL_PARAM(u) "User-Agent: " u

#define FULL_VERSION INT_TO_STR(VERSION_MAJOR) "." INT_TO_STR(VERSION_MINOR) "." INT_TO_STR(VERSION_PATCH)

#define USER_AGENT MK_UA(INT_TO_STR(VERSION_MAJOR), INT_TO_STR(VERSION_MINOR), INT_TO_STR(VERSION_PATCH))
#define CURL_UA MK_CURL_PARAM(USER_AGENT)

#endif

