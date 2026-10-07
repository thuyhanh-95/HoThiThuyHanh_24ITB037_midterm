#ifndef DISPLAY_H
#define DISPLAY_H

#include "listing.h"
#include "options.h"

long long blocks_for(const ListingItem *item, const Config *cfg);
void render_item(const ListingItem *item, const Config *cfg);
void render_total(long long total, const Config *cfg);

#endif

