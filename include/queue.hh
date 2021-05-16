
#ifndef inc_queue_hh
#define inc_queue_hh

#include <3ds/types.h>
#include <vector>

#include "hs.hh"

Result process_hs(hs::FullTitle meta);
Result process_hs(long int id);

void queue_add(hs::FullTitle meta);
void queue_process(size_t index);
void queue_remove(size_t index);
void queue_add(long int id);
void queue_process_all();
void queue_clear();
void show_queue();

#endif

