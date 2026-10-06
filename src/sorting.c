#include <string.h>
#include <time.h>
#include "sorting.h"
#include <stdlib.h>
static int
compare_name(const void *a, const void *b)
{
    const FileEntry *ea = a;
    const FileEntry *eb = b;

    return strcmp(ea->name, eb->name);
}

static int
compare_size(const void *a, const void *b)
{
    const FileEntry *ea = a;
    const FileEntry *eb = b;

    if (ea->st.st_size < eb->st.st_size) {
        return 1;
    }

    if (ea->st.st_size > eb->st.st_size) {
        return -1;
    }

    return strcmp(ea->name, eb->name);
}

static int time_mode = 0;

static int
compare_time(const void *a, const void *b)
{
    const FileEntry *ea = a;
    const FileEntry *eb = b;
    time_t ta;
    time_t tb;

    if (time_mode == 1) {
        ta = ea->st.st_atime;
        tb = eb->st.st_atime;
    } else if (time_mode == 2) {
        ta = ea->st.st_ctime;
        tb = eb->st.st_ctime;
    } else {
        ta = ea->st.st_mtime;
        tb = eb->st.st_mtime;
    }

    if (ta < tb) {
        return 1;
    }

    if (ta > tb) {
        return -1;
    }

    return strcmp(ea->name, eb->name);
}
void
sort_entries(FileEntry *entries, size_t count,
              const Options *options)
{
    if (entries == NULL || options == NULL || count < 2) {
        return;
    }

    if (options->no_sort) {
        return;
    }

    if (options->sort_size) {
        qsort(entries, count, sizeof(FileEntry), compare_size);
    } else if (options->sort_time) {
	if (options->use_ctime) {
        	time_mode = 2;
    	} else if (options->use_atime) {
        	time_mode = 1;
    	} else {
        	time_mode = 0;
    	}

        qsort(entries, count, sizeof(FileEntry), compare_time);
    } else {
        qsort(entries, count, sizeof(FileEntry), compare_name);
    }

    if (options->reverse) {
        size_t left = 0;
        size_t right = count - 1;

        while (left < right) {
            FileEntry temp = entries[left];
            entries[left] = entries[right];
            entries[right] = temp;

            ++left;
            --right;
        }
    }
}
