
#ifndef inc_queue_hh
#define inc_queue_hh

#include <vector>
#include <3ds.h>

#include "hsapi.hh"


Result process_uri(const std::string& uri, bool reinstallable = false, hsapi::htid tid = 0x0, FS_MediaType media = MEDIATYPE_SD);
Result process_hs(const hsapi::FullTitle& meta, bool reinstall = false);
Result process_hs(hsapi::hid id);

void queue_add(const hsapi::FullTitle& meta);
void queue_process(size_t index);
void queue_remove(size_t index);
void queue_add(hsapi::hid id);
void queue_process_all();
void queue_clear();
void show_queue();

#endif

