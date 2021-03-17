
#include "./plog/Log.h"
#include "./plog/Initializers/RollingFileInitializer.h"

/*

    enum Severity
    {
        none = 0,
        fatal = 1,
        error = 2,
        warning = 3,
        info = 4,
        debug = 5,
        verbose = 6
    };


*/

#define lnone PLOG(plog::none)
#define lfatal PLOG(plog::fatal)
#define lerror PLOG(plog::error)
#define lwarning PLOG(plog::warning)
#define linfo PLOG(plog::info)
#define ldebug PLOG(plog::debug)
#define lverbose PLOG(plog::verbose)

#define llog linfo
