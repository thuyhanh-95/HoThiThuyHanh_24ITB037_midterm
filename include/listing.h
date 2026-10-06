#ifndef LISTING_H
#define LISTING_H

#include <stddef.h>
#include <sys/stat.h>

#include "options.h"

typedef struct {
    char *name;
    char *path;
    struct stat st;
} FileEntry;

int list_path(const char *path, const Options *options);

int list_directory(const char *path, const Options *options);

#endif
