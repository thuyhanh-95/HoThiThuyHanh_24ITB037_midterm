#ifndef SORTING_H
#define SORTING_H

#include <stddef.h>

#include "listing.h"
#include "options.h"

void reorder_items(ListingItem *items, size_t length, const Config *cfg);

#endif

