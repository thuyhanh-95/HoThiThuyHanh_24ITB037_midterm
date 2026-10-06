#ifndef DISPLAY_H
#define DISPLAY_H

#include "listing.h"
#include "options.h"

void print_entry(const FileEntry *entry, const Options *options);

long long entry_blocks(const FileEntry *entry,
                       const Options *options);

void print_total(long long total,
                 const Options *options);
#endif
