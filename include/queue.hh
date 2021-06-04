
#ifndef inc_queue_hh
#define inc_queue_hh

#include <vector>
#include <3ds.h>

#include "hs.hh"

Result process_uri(const std::string& uri, bool reinstallable = false, const std::string& tid = "", FS_MediaType media = MEDIATYPE_SD);
Result process_hs(hs::FullTitle meta, bool reinstall = false);
Result process_hs(long int id);

void queue_add(hs::FullTitle meta);
void queue_process(size_t index);
void queue_remove(size_t index);
void queue_add(long int id);
void queue_process_all();
void queue_clear();
void show_queue();

#endif

