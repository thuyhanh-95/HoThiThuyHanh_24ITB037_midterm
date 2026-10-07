#define _POSIX_C_SOURCE 200809L

#include "display.h"
#include "listing.h"
#include "options.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

typedef struct {
    char *path;
    int directory;
} Target;

static int target_before(const Target *a, const Target *b)
{
    if (a->directory != b->directory) {
        return a->directory < b->directory;
    }
    return strcmp(a->path, b->path) < 0;
}

static void order_targets(Target *targets,
                          size_t count,
                          const Config *cfg)
{
    size_t i;

    if (targets == NULL || cfg == NULL || count < 2 ||
        (cfg->flags & OPT_NO_SORT) != 0u) {
        return;
    }

    for (i = 1; i < count; ++i) {
        Target picked = targets[i];
        size_t j = i;

        while (j > 0 && target_before(&picked, &targets[j - 1])) {
            targets[j] = targets[j - 1];
            --j;
        }
        targets[j] = picked;
    }
}

int main(int argc, char **argv)
{
    Config cfg;
    int first;
    int count;
    Target *targets;
    int i;
    int status = 0;

    config_init(&cfg);
    first = config_read(&cfg, argc, argv);

    if (first < 0) {
        return 1;
    }

    if (first == argc) {
        return run_target(".", &cfg);
    }

    count = argc - first;
    targets = calloc((size_t)count, sizeof(*targets));
    if (targets == NULL) {
        fprintf(stderr, "myls: memory allocation failed\n");
        return 1;
    }

    for (i = 0; i < count; ++i) {
        struct stat st;

        targets[i].path = argv[first + i];
        targets[i].directory = 0;
        if (lstat(targets[i].path, &st) == 0) {
            targets[i].directory = S_ISDIR(st.st_mode);
        }
    }

    order_targets(targets, (size_t)count, &cfg);

    for (i = 0; i < count; ++i) {
        if (targets[i].directory &&
            (cfg.flags & OPT_DIR_ONLY) == 0u) {
            if (count > 1) {
                if (i != 0) {
                    putchar('\n');
                }
                printf("%s:\n", targets[i].path);
            }

            if (run_directory(targets[i].path, &cfg) != 0) {
                status = 1;
            }
        } else {
            ListingItem item;
            struct stat st;

            if (lstat(targets[i].path, &st) == -1) {
                fprintf(stderr, "myls: %s: %s\n",
                        targets[i].path, strerror(errno));
                status = 1;
                continue;
            }

            item.label = targets[i].path;
            item.full_name = targets[i].path;
            item.info = st;
            render_item(&item, &cfg);
        }
    }

    free(targets);
    return status;
}

