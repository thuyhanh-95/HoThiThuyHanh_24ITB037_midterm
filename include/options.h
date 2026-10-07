#ifndef OPTIONS_H
#define OPTIONS_H

#include <stddef.h>

enum {
    OPT_ALL          = 1u << 0,
    OPT_ALMOST       = 1u << 1,
    OPT_CTIME        = 1u << 2,
    OPT_DIR_ONLY     = 1u << 3,
    OPT_CLASSIFY     = 1u << 4,
    OPT_NO_SORT      = 1u << 5,
    OPT_HUMAN        = 1u << 6,
    OPT_INODE        = 1u << 7,
    OPT_KILOBYTES    = 1u << 8,
    OPT_LONG         = 1u << 9,
    OPT_NUMERIC      = 1u << 10,
    OPT_QUOTE        = 1u << 11,
    OPT_RECURSIVE    = 1u << 12,
    OPT_REVERSE      = 1u << 13,
    OPT_SIZE_SORT    = 1u << 14,
    OPT_BLOCKS       = 1u << 15,
    OPT_TIME_SORT    = 1u << 16,
    OPT_ATIME        = 1u << 17,
    OPT_RAW          = 1u << 18
};

typedef struct {
    unsigned int flags;
} Config;

void config_init(Config *cfg);
int config_read(Config *cfg, int argc, char **argv);

#endif

