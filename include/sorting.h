#ifndef SORTING_H
#define SORTING_H

#include <stddef.h>

#include "listing.h"
#include "options.h"

void sort_entries(FileEntry *entries, size_t count,
                  const Options *options);

#endif
