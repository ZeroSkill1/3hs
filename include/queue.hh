
#ifndef inc_queue_hh
#define inc_queue_hh

#include <vector>
#include <3ds.h>

#include "hsapi.hh"


std::vector<hsapi::FullTitle>& queue_get();

void queue_add(hsapi::hid id, bool disp = true);
void queue_add(const hsapi::FullTitle& meta);
void queue_process(size_t index);
void queue_remove(size_t index);
void queue_add(hsapi::hid id);
void queue_process_all();
void queue_clear();
void show_queue();

#endif

