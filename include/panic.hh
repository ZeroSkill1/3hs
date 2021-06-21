
#ifndef inc_panic_hh
#define inc_panic_hh

#include <string>

#include <3ds.h>

#include "error.hh"

#define panic_if_err_3ds(result) { Result res = (result); if(R_FAILED(res)) panic(res); }
#define panic_assert(cond, ...) if(!(cond)) panic(__VA_ARGS__)

void handle_error(error_container err);

void panic(std::string msg);
void panic(Result res);
void panic();

Result init_services();
void exit_services();

#endif

