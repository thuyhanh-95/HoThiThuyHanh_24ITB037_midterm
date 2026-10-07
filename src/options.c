#define _POSIX_C_SOURCE 200809L

#include "options.h"

#include <stdio.h>
#include <unistd.h>

static void set_flag(Config *cfg, unsigned int bit)
{
    cfg->flags |= bit;
}

static void clear_flag(Config *cfg, unsigned int bit)
{
    cfg->flags &= ~bit;
}

void config_init(Config *cfg)
{
    if (cfg != NULL) {
        cfg->flags = 0u;
    }
}

int config_read(Config *cfg, int argc, char **argv)
{
    int ch;

    if (cfg == NULL) {
        return -1;
    }

    while ((ch = getopt(argc, argv, "AacdFfhiklnqRrSstuw")) != -1) {
        switch (ch) {
        case 'A':
            set_flag(cfg, OPT_ALMOST);
            break;
        case 'a':
            set_flag(cfg, OPT_ALL);
            break;
        case 'c':
            set_flag(cfg, OPT_CTIME);
            clear_flag(cfg, OPT_ATIME);
            break;
        case 'd':
            set_flag(cfg, OPT_DIR_ONLY);
            clear_flag(cfg, OPT_RECURSIVE);
            break;
        case 'F':
            set_flag(cfg, OPT_CLASSIFY);
            break;
        case 'f':
            set_flag(cfg, OPT_NO_SORT);
            break;
        case 'h':
            set_flag(cfg, OPT_HUMAN);
            clear_flag(cfg, OPT_KILOBYTES);
            break;
        case 'i':
            set_flag(cfg, OPT_INODE);
            break;
        case 'k':
            set_flag(cfg, OPT_KILOBYTES);
            clear_flag(cfg, OPT_HUMAN);
            break;
        case 'l':
            set_flag(cfg, OPT_LONG);
            clear_flag(cfg, OPT_NUMERIC);
            break;
        case 'n':
            set_flag(cfg, OPT_LONG);
            set_flag(cfg, OPT_NUMERIC);
            break;
        case 'q':
            set_flag(cfg, OPT_QUOTE);
            clear_flag(cfg, OPT_RAW);
            break;
        case 'R':
            set_flag(cfg, OPT_RECURSIVE);
            clear_flag(cfg, OPT_DIR_ONLY);
            break;
        case 'r':
            set_flag(cfg, OPT_REVERSE);
            break;
        case 'S':
            set_flag(cfg, OPT_SIZE_SORT);
            break;
        case 's':
            set_flag(cfg, OPT_BLOCKS);
            break;
        case 't':
            set_flag(cfg, OPT_TIME_SORT);
            break;
        case 'u':
            set_flag(cfg, OPT_ATIME);
            clear_flag(cfg, OPT_CTIME);
            break;
        case 'w':
            set_flag(cfg, OPT_RAW);
            clear_flag(cfg, OPT_QUOTE);
            break;
        default:
            fprintf(stderr,
                    "usage: myls [-AacdFfhiklnqRrSstuw] [file ...]\n");
            return -1;
        }
    }

    return optind;
}

