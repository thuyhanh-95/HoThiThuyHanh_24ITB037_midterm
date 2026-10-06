#include "listing.h"
#include "sorting.h"
#include "display.h"
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

static int
should_skip_entry(const char *name, const Options *options)
{
    if (name == NULL || options == NULL) {
        return 1;
    }

    if (options->all) {
        return 0;
    }

    if (options->almost_all) {
        return strcmp(name, ".") == 0 ||
               strcmp(name, "..") == 0;
    }

    return name[0] == '.';
}

static char *
duplicate_string(const char *source)
{
    size_t length;
    char *result;

    if (source == NULL) {
        return NULL;
    }

    length = strlen(source) + 1;

    result = malloc(length);

    if (result == NULL) {
        return NULL;
    }

    memcpy(result, source, length);

    return result;
}

static char *
join_path(const char *directory, const char *name)
{
    size_t length;
    char *result;

    length = strlen(directory) + strlen(name) + 2;

    result = malloc(length);

    if (result == NULL) {
        return NULL;
    }

    snprintf(result, length, "%s/%s", directory, name);

    return result;
}

static void
free_entries(FileEntry *entries, size_t count)
{
    size_t i;

    if (entries == NULL) {
        return;
    }

    for (i = 0; i < count; ++i) {
        free(entries[i].name);
	free(entries[i].path);
    }

    free(entries);
}

static long long
directory_total(FileEntry *entries,
                size_t count,
                const Options *options)
{
    size_t i;
    long long total = 0;

    for (i = 0; i < count; ++i) {
        total += entry_blocks(&entries[i], options);
    }

    return total;
}

static int
is_directory_entry(const FileEntry *entry)
{
    return entry != NULL && S_ISDIR(entry->st.st_mode);
}

static int
list_directory_internal(const char *path,
                        const Options *options,
                        int print_header)
{
    DIR *dir;
    struct dirent *entry;
    FileEntry *entries;
    size_t count = 0;
    size_t capacity = 16;
    int status = 0;
    size_t i;

    if (path == NULL || options == NULL) {
        return 1;
    }

    dir = opendir(path);

    if (dir == NULL) {
        fprintf(stderr,
                "myls: %s: %s\n",
                path,
                strerror(errno));
        return 1;
    }

    entries = malloc(capacity * sizeof(*entries));

    if (entries == NULL) {
        fprintf(stderr, "myls: memory allocation failed\n");
        closedir(dir);
        return 1;
    }

    while ((entry = readdir(dir)) != NULL) {
        char *name_copy;
        char *full_path;

        if (should_skip_entry(entry->d_name, options)) {
            continue;
        }

        if (count == capacity) {
            FileEntry *new_entries;

            capacity *= 2;

            new_entries = realloc(
                entries,
                capacity * sizeof(*entries)
            );

            if (new_entries == NULL) {
                fprintf(stderr,
                        "myls: memory allocation failed\n");

                free_entries(entries, count);
                closedir(dir);
                return 1;
            }

            entries = new_entries;
        }

        name_copy = duplicate_string(entry->d_name);

        if (name_copy == NULL) {
            fprintf(stderr,
                    "myls: memory allocation failed\n");

            free_entries(entries, count);
            closedir(dir);
            return 1;
        }

        full_path = join_path(path, entry->d_name);

        if (full_path == NULL) {
            fprintf(stderr,
                    "myls: memory allocation failed\n");

            free(name_copy);
            free_entries(entries, count);
            closedir(dir);
            return 1;
        }

        if (lstat(full_path, &entries[count].st) == -1) {
            fprintf(stderr,
                    "myls: %s: %s\n",
                    full_path,
                    strerror(errno));

            free(full_path);
            free(name_copy);

            status = 1;
            continue;
        }

        entries[count].name = name_copy;
	entries[count].path = full_path;
	++count;

    }

    if (closedir(dir) == -1) {
        fprintf(stderr,
                "myls: %s: %s\n",
                path,
                strerror(errno));

        status = 1;
    }

    sort_entries(entries, count, options);

    if (print_header) {
        printf("%s:\n", path);
    }

    if (options->long_format && isatty(STDOUT_FILENO)) {
        print_total(directory_total(entries, count, options),
                    options);
    } else if (options->blocks && isatty(STDOUT_FILENO)) {
        print_total(directory_total(entries, count, options),
                    options);
    }

    for (i = 0; i < count; ++i) {
        print_entry(&entries[i], options);
    }

    if (options->recursive) {
        for (i = 0; i < count; ++i) {
            char *subdir;

            if (!is_directory_entry(&entries[i])) {
                continue;
            }

            if (strcmp(entries[i].name, ".") == 0 ||
                strcmp(entries[i].name, "..") == 0) {
                continue;
            }

            subdir = join_path(path, entries[i].name);

            if (subdir == NULL) {
                status = 1;
                continue;
            }

            printf("\n");

            if (list_directory_internal(subdir,
                                        options,
                                        1) != 0) {
                status = 1;
            }

            free(subdir);
        }
    }

    free_entries(entries, count);

    return status;
}

int
list_directory(const char *path, const Options *options)
{
    return list_directory_internal(path, options, 0);
}

int
list_path(const char *path, const Options *options)
{
    struct stat st;
    FileEntry entry;

    if (path == NULL || options == NULL) {
        return 1;
    }

    if (lstat(path, &st) == -1) {
        fprintf(stderr,
                "myls: %s: %s\n",
                path,
                strerror(errno));
        return 1;
    }

    if (options->directory ||
        !S_ISDIR(st.st_mode)) {

        entry.name = (char *)path;
        entry.path = (char *)path;
	entry.st = st;
	
        print_entry(&entry, options);
	
        return 0;
    }

    return list_directory(path, options);
}
