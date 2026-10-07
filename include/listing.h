#ifndef LISTING_H
#define LISTING_H

#include <stddef.h>
#include <sys/stat.h>

#include "options.h"

typedef struct {
    char *label;
    char *full_name;
    struct stat info;
} ListingItem;

int run_target(const char *target, const Config *cfg);
int run_directory(const char *directory, const Config *cfg);

#endif

