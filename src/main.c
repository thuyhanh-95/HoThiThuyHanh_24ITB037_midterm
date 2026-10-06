#include "listing.h"
#include "options.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

typedef struct {
    char *path;
    int is_directory;
} Operand;

static int
compare_operands(const void *a, const void *b)
{
    const Operand *oa = a;
    const Operand *ob = b;

    if (oa->is_directory != ob->is_directory) {
        return oa->is_directory - ob->is_directory;
    }

    return strcmp(oa->path, ob->path);
}

static void
reverse_operands(Operand *operands, size_t count)
{
    size_t left = 0;
    size_t right;

    if (count == 0) {
        return;
    }

    right = count - 1;

    while (left < right) {
        Operand temp = operands[left];

        operands[left] = operands[right];
        operands[right] = temp;

        ++left;
        --right;
    }
}

int
main(int argc, char *argv[])
{
    Options options;
    int first_operand;
    int exit_status = 0;
    int operand_count;
    Operand *operands;
    int i;

    options_init(&options);

    first_operand = options_parse(&options, argc, argv);

    if (first_operand < 0) {
        return 2;
    }

    if (first_operand == argc) {
        return list_directory(".", &options);
    }

    operand_count = argc - first_operand;

    operands = calloc((size_t)operand_count,
                      sizeof(*operands));

    if (operands == NULL) {
        fprintf(stderr,
                "myls: memory allocation failed\n");
        return 1;
    }

    for (i = 0; i < operand_count; ++i) {
        struct stat st;

        operands[i].path = argv[first_operand + i];

        if (lstat(operands[i].path, &st) == -1) {
            operands[i].is_directory = 0;
            continue;
        }

        operands[i].is_directory =
            !options.directory && S_ISDIR(st.st_mode);
    }

    if (!options.no_sort) {
        qsort(operands,
              (size_t)operand_count,
              sizeof(*operands),
              compare_operands);

        if (options.reverse) {
            reverse_operands(operands,
                             (size_t)operand_count);
        }
    }

    for (i = 0; i < operand_count; ++i) {
        int status;

        if (operand_count > 1 &&
            operands[i].is_directory) {
            if (i > 0) {
                printf("\n");
            }

            printf("%s:\n", operands[i].path);

            status = list_directory(operands[i].path,
                                    &options);
        } else {
            status = list_path(operands[i].path,
                               &options);
        }

        if (status != 0) {
            exit_status = 1;
        }
    }

    free(operands);

    return exit_status;
}
