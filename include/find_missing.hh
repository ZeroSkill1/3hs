
#ifndef inc_find_missing_hh
#define inc_find_missing_hh

#include "hsapi.hh"


ssize_t show_find_missing(hsapi::htid tid);
bool tid_can_have_missing(hsapi::htid tid);
void show_find_missing_all();

#endif

