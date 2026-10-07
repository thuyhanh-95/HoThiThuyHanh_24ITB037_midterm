#define _POSIX_C_SOURCE 200809L

#include "listing.h"

#include "display.h"
#include "sorting.h"

#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

static int hidden_name(const char *name, const Config *cfg)
{
    if (name == NULL || cfg == NULL) {
        return 1;
    }

    if ((cfg->flags & OPT_ALL) != 0u) {
        return 0;
    }

    if ((cfg->flags & OPT_ALMOST) != 0u) {
        return strcmp(name, ".") == 0 || strcmp(name, "..") == 0;
    }

    return name[0] == '.';
}

static char *copy_text(const char *src)
{
    size_t length;
    char *copy;

    if (src == NULL) {
        return NULL;
    }

    length = strlen(src) + 1;
    copy = malloc(length);
    if (copy != NULL) {
        memcpy(copy, src, length);
    }
    return copy;
}

static char *make_path(const char *base, const char *name)
{
    size_t length;
    char *full;

    if (base == NULL || name == NULL) {
        return NULL;
    }

    length = strlen(base) + strlen(name) + 2;
    full = malloc(length);
    if (full != NULL) {
        snprintf(full, length, "%s/%s", base, name);
    }
    return full;
}

static void release_items(ListingItem *items, size_t count)
{
    size_t i;

    if (items == NULL) {
        return;
    }

    for (i = 0; i < count; ++i) {
        free(items[i].label);
        free(items[i].full_name);
    }

    free(items);
}

static int append_item(ListingItem **items,
                       size_t *used,
                       size_t *capacity,
                       const char *directory,
                       const char *name)
{
    ListingItem *grown;
    char *label;
    char *full;

    if (*used == *capacity) {
        size_t next = (*capacity == 0) ? 16 : (*capacity * 2);
        grown = realloc(*items, next * sizeof(**items));
        if (grown == NULL) {
            return -1;
        }
        *items = grown;
        *capacity = next;
    }

    label = copy_text(name);
    full = make_path(directory, name);

    if (label == NULL || full == NULL) {
        free(label);
        free(full);
        return -1;
    }

    if (lstat(full, &(*items)[*used].info) == -1) {
        fprintf(stderr, "myls: %s: %s\n", full, strerror(errno));
        free(label);
        free(full);
        return 1;
    }

    (*items)[*used].label = label;
    (*items)[*used].full_name = full;
    ++(*used);

    return 0;
}

static long long total_blocks(ListingItem *items,
                              size_t count,
                              const Config *cfg)
{
    size_t i;
    long long total = 0;

    for (i = 0; i < count; ++i) {
        total += blocks_for(&items[i], cfg);
    }
    return total;
}

static int is_directory(const ListingItem *item)
{
    return item != NULL && S_ISDIR(item->info.st_mode);
}

static int scan_directory(const char *directory,
                          const Config *cfg,
                          int show_header)
{
    DIR *dir;
    struct dirent *de;
    ListingItem *items = NULL;
    size_t used = 0;
    size_t capacity = 0;
    size_t i;
    int status = 0;

    if (directory == NULL || cfg == NULL) {
        return 1;
    }

    dir = opendir(directory);
    if (dir == NULL) {
        fprintf(stderr, "myls: %s: %s\n",
                directory, strerror(errno));
        return 1;
    }

    while ((de = readdir(dir)) != NULL) {
        int result;

        if (hidden_name(de->d_name, cfg)) {
            continue;
        }

        result = append_item(&items, &used, &capacity,
                             directory, de->d_name);

        if (result < 0) {
            fprintf(stderr, "myls: memory allocation failed\n");
            release_items(items, used);
            closedir(dir);
            return 1;
        }
        if (result > 0) {
            status = 1;
        }
    }

    if (closedir(dir) == -1) {
        fprintf(stderr, "myls: %s: %s\n",
                directory, strerror(errno));
        status = 1;
    }

    reorder_items(items, used, cfg);

    if (show_header) {
        printf("%s:\n", directory);
    }

    if (((cfg->flags & OPT_LONG) != 0u ||
         (cfg->flags & OPT_BLOCKS) != 0u) &&
        isatty(STDOUT_FILENO)) {
        render_total(total_blocks(items, used, cfg), cfg);
    }

    for (i = 0; i < used; ++i) {
        render_item(&items[i], cfg);
    }

    if ((cfg->flags & OPT_RECURSIVE) != 0u) {
        for (i = 0; i < used; ++i) {
            char *child;

            if (!is_directory(&items[i])) {
                continue;
            }
            if (strcmp(items[i].label, ".") == 0 ||
                strcmp(items[i].label, "..") == 0) {
                continue;
            }

            child = make_path(directory, items[i].label);
            if (child == NULL) {
                fprintf(stderr, "myls: memory allocation failed\n");
                status = 1;
                continue;
            }

            putchar('\n');
            if (scan_directory(child, cfg, 1) != 0) {
                status = 1;
            }
            free(child);
        }
    }

    release_items(items, used);
    return status;
}

int run_directory(const char *directory, const Config *cfg)
{
    return scan_directory(directory, cfg, 0);
}

int run_target(const char *target, const Config *cfg)
{
    struct stat info;
    ListingItem item;

    if (target == NULL || cfg == NULL) {
        return 1;
    }

    if (lstat(target, &info) == -1) {
        fprintf(stderr, "myls: %s: %s\n",
                target, strerror(errno));
        return 1;
    }

    if ((cfg->flags & OPT_DIR_ONLY) != 0u ||
        !S_ISDIR(info.st_mode)) {
        item.label = (char *)target;
        item.full_name = (char *)target;
        item.info = info;
        render_item(&item, cfg);
        return 0;
    }

    return run_directory(target, cfg);
}

