#include "sorting.h"

#include <string.h>
#include <time.h>

static time_t selected_time(const ListingItem *item, const Config *cfg)
{
    if ((cfg->flags & OPT_CTIME) != 0u) {
        return item->info.st_ctime;
    }
    if ((cfg->flags & OPT_ATIME) != 0u) {
        return item->info.st_atime;
    }
    return item->info.st_mtime;
}

static int compare_items(const ListingItem *left,
                         const ListingItem *right,
                         const Config *cfg)
{
    int result;

    if ((cfg->flags & OPT_SIZE_SORT) != 0u) {
        if (left->info.st_size < right->info.st_size) {
            result = 1;
        } else if (left->info.st_size > right->info.st_size) {
            result = -1;
        } else {
            result = strcmp(left->label, right->label);
        }
    } else if ((cfg->flags & OPT_TIME_SORT) != 0u) {
        time_t lt = selected_time(left, cfg);
        time_t rt = selected_time(right, cfg);

        if (lt < rt) {
            result = 1;
        } else if (lt > rt) {
            result = -1;
        } else {
            result = strcmp(left->label, right->label);
        }
    } else {
        result = strcmp(left->label, right->label);
    }

    if ((cfg->flags & OPT_REVERSE) != 0u) {
        result = -result;
    }

    return result;
}

void reorder_items(ListingItem *items, size_t length, const Config *cfg)
{
    size_t i;

    if (items == NULL || cfg == NULL || length < 2) {
        return;
    }

    if ((cfg->flags & OPT_NO_SORT) != 0u) {
        return;
    }

    for (i = 1; i < length; ++i) {
        ListingItem saved = items[i];
        size_t j = i;

        while (j > 0 &&
               compare_items(&items[j - 1], &saved, cfg) > 0) {
            items[j] = items[j - 1];
            --j;
        }

        items[j] = saved;
    }
}

