#include "display.h"

#include <ctype.h>
#include <grp.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

static char
file_type(mode_t mode)
{
    if (S_ISREG(mode)) {
        return '-';
    }

    if (S_ISDIR(mode)) {
        return 'd';
    }

    if (S_ISLNK(mode)) {
        return 'l';
    }

    if (S_ISCHR(mode)) {
        return 'c';
    }

    if (S_ISBLK(mode)) {
        return 'b';
    }

    if (S_ISFIFO(mode)) {
        return 'p';
    }

    if (S_ISSOCK(mode)) {
        return 's';
    }

#ifdef S_ISWHT
    if (S_ISWHT(mode)) {
        return 'w';
    }
#endif

    return '-';
}

static void
format_mode(mode_t mode, char *buffer)
{
    buffer[0] = file_type(mode);

    buffer[1] = (mode & S_IRUSR) ? 'r' : '-';
    buffer[2] = (mode & S_IWUSR) ? 'w' : '-';

    if (mode & S_ISUID) {
        buffer[3] = (mode & S_IXUSR) ? 's' : 'S';
    } else {
        buffer[3] = (mode & S_IXUSR) ? 'x' : '-';
    }

    buffer[4] = (mode & S_IRGRP) ? 'r' : '-';
    buffer[5] = (mode & S_IWGRP) ? 'w' : '-';

    if (mode & S_ISGID) {
        buffer[6] = (mode & S_IXGRP) ? 's' : 'S';
    } else {
        buffer[6] = (mode & S_IXGRP) ? 'x' : '-';
    }

    buffer[7] = (mode & S_IROTH) ? 'r' : '-';
    buffer[8] = (mode & S_IWOTH) ? 'w' : '-';

    if (mode & S_ISVTX) {
        buffer[9] = (mode & S_IXOTH) ? 't' : 'T';
    } else {
        buffer[9] = (mode & S_IXOTH) ? 'x' : '-';
    }

    buffer[10] = '\0';
}

static void
print_owner(uid_t uid, const Options *options)
{
    struct passwd *pw;

    if (options->numeric_ids) {
        printf("%u", (unsigned int)uid);
        return;
    }

    pw = getpwuid(uid);

    if (pw != NULL) {
        printf("%s", pw->pw_name);
    } else {
        printf("%u", (unsigned int)uid);
    }
}

static void
print_group(gid_t gid, const Options *options)
{
    struct group *gr;

    if (options->numeric_ids) {
        printf("%u", (unsigned int)gid);
        return;
    }

    gr = getgrgid(gid);

    if (gr != NULL) {
        printf("%s", gr->gr_name);
    } else {
        printf("%u", (unsigned int)gid);
    }
}

static time_t
entry_time(const FileEntry *entry, const Options *options)
{
    if (options->use_ctime) {
        return entry->st.st_ctime;
    }

    if (options->use_atime) {
        return entry->st.st_atime;
    }

    return entry->st.st_mtime;
}

static void
print_time(time_t value)
{
    char buffer[64];
    struct tm *tm_info;

    tm_info = localtime(&value);

    if (tm_info == NULL) {
        printf("??? ?? ??:??");
        return;
    }

    if (strftime(buffer, sizeof(buffer), "%b %e %H:%M",
                 tm_info) == 0) {
        printf("??? ?? ??:??");
        return;
    }

    printf("%s", buffer);
}

static long long
block_size(void)
{
    const char *value;
    char *end;
    long long result;

    value = getenv("BLOCKSIZE");

    if (value == NULL || *value == '\0') {
        return 512;
    }

    result = strtoll(value, &end, 10);

    if (*end != '\0' || result <= 0) {
        return 512;
    }

    return result;
}

static long long
ceil_div(long long value, long long divisor)
{
    if (divisor <= 0) {
        return value;
    }

    return (value + divisor - 1) / divisor;
}

long long
entry_blocks(const FileEntry *entry, const Options *options)
{
    long long bytes;

    if (entry == NULL || options == NULL) {
        return 0;
    }

    bytes = (long long)entry->st.st_blocks * 512LL;

    if (options->human_readable) {
        return bytes;
    }

    if (options->kilobytes) {
        return ceil_div(bytes, 1024);
    }

    return ceil_div(bytes, block_size());
}

static void
print_size(long long bytes, const Options *options)
{
    static const char units[] = "BKMGTPE";
    double value;
    int unit = 0;

    if (!options->human_readable) {
        printf("%lld", bytes);
        return;
    }

    value = (double)bytes;

    while (value >= 1024.0 &&
           unit < (int)(sizeof(units) - 2)) {
        value /= 1024.0;
        ++unit;
    }

    if (unit == 0) {
        printf("%lldB", bytes);
    } else if (value >= 10.0) {
        printf("%.0f%c", value, units[unit]);
    } else {
        printf("%.1f%c", value, units[unit]);
    }
}

static void
print_name(const char *name, const Options *options)
{
    const unsigned char *p;

    if (name == NULL) {
        return;
    }

    if (options->raw ||
        (!options->quote && !options->raw && !isatty(STDOUT_FILENO))) {
        printf("%s", name);
        return;
    }

    p = (const unsigned char *)name;

    while (*p != '\0') {
        if (isprint(*p)) {
            putchar(*p);
        } else {
            putchar('?');
        }

        ++p;
    }
}

static char
classify_suffix(mode_t mode)
{
    if (S_ISDIR(mode)) {
        return '/';
    }

    if (S_ISLNK(mode)) {
        return '@';
    }

    if (S_ISFIFO(mode)) {
        return '|';
    }

    if (S_ISSOCK(mode)) {
        return '=';
    }

#ifdef S_ISWHT
    if (S_ISWHT(mode)) {
        return '%';
    }
#endif

    if (mode & (S_IXUSR | S_IXGRP | S_IXOTH)) {
        return '*';
    }

    return '\0';
}

static void
print_symlink_target(const FileEntry *entry)
{
    char buffer[4096];
    ssize_t length;

    if (!S_ISLNK(entry->st.st_mode)) {
        return;
    }

    length = readlink(entry->path, buffer, sizeof(buffer) - 1);

    if (length < 0) {
        return;
    }

    buffer[length] = '\0';

    printf(" -> %s", buffer);
}

static void
print_long(const FileEntry *entry, const Options *options)
{
    char mode[11];

    format_mode(entry->st.st_mode, mode);

    printf("%s %3lu ",
           mode,
           (unsigned long)entry->st.st_nlink);

    print_owner(entry->st.st_uid, options);
    printf(" ");

    print_group(entry->st.st_gid, options);
    printf(" ");

    if (S_ISCHR(entry->st.st_mode) ||
        S_ISBLK(entry->st.st_mode)) {
        printf("%8u,%3u ",
               (unsigned int)major(entry->st.st_rdev),
               (unsigned int)minor(entry->st.st_rdev));
    } else if (options->human_readable) {
        print_size((long long)entry->st.st_size, options);
        printf(" ");
    } else {
        printf("%8lld ", (long long)entry->st.st_size);
    }

    print_time(entry_time(entry, options));
    printf(" ");

    print_name(entry->name, options);

    if (options->classify) {
        char suffix = classify_suffix(entry->st.st_mode);

        if (suffix != '\0') {
            putchar(suffix);
        }
    }

    print_symlink_target(entry);

    putchar('\n');
}

void
print_entry(const FileEntry *entry, const Options *options)
{
    char suffix;

    if (entry == NULL || options == NULL) {
        return;
    }

    if (options->inode) {
        printf("%llu ",
               (unsigned long long)entry->st.st_ino);
    }

    if (options->blocks) {
        long long blocks = entry_blocks(entry, options);

        if (options->human_readable) {
            print_size(blocks, options);
        } else {
            printf("%lld", blocks);
        }

        printf(" ");
    }

    if (options->long_format || options->numeric_ids) {
        print_long(entry, options);
        return;
    }

    print_name(entry->name, options);

    if (options->classify) {
        suffix = classify_suffix(entry->st.st_mode);

        if (suffix != '\0') {
            putchar(suffix);
        }
    }

    putchar('\n');
}

void
print_total(long long total, const Options *options)
{
    if (options == NULL) {
        return;
    }

    printf("total ");

    if (options->human_readable) {
        print_size(total, options);
    } else {
        printf("%lld", total);
    }

    putchar('\n');
}
