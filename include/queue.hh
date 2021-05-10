
#ifndef __queue_hh__
#define __queue_hh__

#include <vector>

#include "hs.hh"

// Returns FALSE if the user cancelled, else returns TRUE
bool process_hs(hs::FullTitle meta);
bool process_hs(long int id);

void queue_add(hs::FullTitle meta);
void queue_process(size_t index);
void queue_remove(size_t index);
void queue_add(long int id);
void queue_process_all();
void queue_clear();
void show_queue();

#endif

