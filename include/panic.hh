
#ifndef inc_panic_hh
#define inc_panic_hh

#include <string>

#include <string.h>
#include <3ds.h>

#include "error.hh"

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#define panic(...) panic_impl(std::string(__FILENAME__) + ":" + std::string(__func__) + ":" + std::to_string(__LINE__) __VA_OPT__(,) __VA_ARGS__)
#define panic_if_err_3ds(result) do { Result res = (result); if(R_FAILED(res)) panic(res); } while(0)

#ifndef NDEBUG
# define panic_assert(cond) if(!(cond)) panic("Assertion failed:\n"#cond)
#else
# define panic_assert(_) (void)
#endif

void handle_error(error_container err);

[[noreturn]] void panic_impl(std::string caller, std::string msg);
[[noreturn]] void panic_impl(std::string caller, Result res);
[[noreturn]] void panic_impl(std::string caller);

Result init_services(bool& isLuma);
void exit_services();

#endif

