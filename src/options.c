#include "options.h"

#include <stdio.h>
#include <unistd.h>

void
options_init(Options *options)
{
    if (options == NULL) {
        return;
    }

    options->all = 0;
    options->almost_all = 0;
    options->use_ctime = 0;
    options->directory = 0;
    options->classify = 0;
    options->no_sort = 0;
    options->human_readable = 0;
    options->inode = 0;
    options->kilobytes = 0;
    options->long_format = 0;
    options->numeric_ids = 0;
    options->quote = 0;
    options->recursive = 0;
    options->reverse = 0;
    options->sort_size = 0;
    options->blocks = 0;
    options->sort_time = 0;
    options->use_atime = 0;
    options->raw = 0;
}

int
options_parse(Options *options, int argc, char *argv[])
{
    int opt;

    if (options == NULL) {
        return -1;
    }

    while ((opt = getopt(argc, argv, "AacdFfhiklnqRrSstuw")) != -1) {
        switch (opt) {
        case 'A':
            options->almost_all = 1;
            break;

        case 'a':
            options->all = 1;
            break;

        case 'c':
            options->use_ctime = 1;
            options->use_atime = 0;
            break;

        case 'd':
            options->directory = 1;
	    options->recursive = 0;
            break;

        case 'F':
            options->classify = 1;
            break;

        case 'f':
            options->no_sort = 1;
            break;

        case 'h':
            options->human_readable = 1;
            options->kilobytes = 0;
            break;

        case 'i':
            options->inode = 1;
            break;

        case 'k':
            options->kilobytes = 1;
            options->human_readable = 0;
            break;

        case 'l':
            options->long_format = 1;
	    options->numeric_ids = 0;
            break;

        case 'n':
            options->long_format = 1;
            options->numeric_ids = 1;
            break;

        case 'q':
            options->quote = 1;
            options->raw = 0;
            break;

        case 'R':
            options->recursive = 1;
            options->directory = 0;
            break;

        case 'r':
            options->reverse = 1;
            break;

        case 'S':
            options->sort_size = 1;
            break;

        case 's':
            options->blocks = 1;
            break;

        case 't':
            options->sort_time = 1;
            break;

        case 'u':
            options->use_atime = 1;
            options->use_ctime = 0;
            break;

        case 'w':
            options->raw = 1;
            options->quote = 0;
            break;

        default:
            fprintf(stderr,
                    "usage: myls [-AacdFfhiklnqRrSstuw] [file ...]\n");
            return -1;
        }
    }

    return optind;
}
