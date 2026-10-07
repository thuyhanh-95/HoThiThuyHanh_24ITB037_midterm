#define _POSIX_C_SOURCE 200809L

#include "display.h"

#include <ctype.h>
#include <grp.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#if defined(__linux__)
#include <sys/sysmacros.h>
#endif

static char type_char(mode_t mode)
{
    if (S_ISREG(mode)) return '-';
    if (S_ISDIR(mode)) return 'd';
    if (S_ISLNK(mode)) return 'l';
    if (S_ISCHR(mode)) return 'c';
    if (S_ISBLK(mode)) return 'b';
    if (S_ISFIFO(mode)) return 'p';
#ifdef S_ISSOCK
    if (S_ISSOCK(mode)) return 's';
#endif
#ifdef S_ISWHT
    if (S_ISWHT(mode)) return 'w';
#endif
    return '-';
}

static void make_mode(mode_t mode, char out[11])
{
    out[0] = type_char(mode);
    out[1] = (mode & S_IRUSR) ? 'r' : '-';
    out[2] = (mode & S_IWUSR) ? 'w' : '-';
    out[3] = (mode & S_IXUSR) ? 'x' : '-';
    if (mode & S_ISUID) out[3] = (mode & S_IXUSR) ? 's' : 'S';
    out[4] = (mode & S_IRGRP) ? 'r' : '-';
    out[5] = (mode & S_IWGRP) ? 'w' : '-';
    out[6] = (mode & S_IXGRP) ? 'x' : '-';
    if (mode & S_ISGID) out[6] = (mode & S_IXGRP) ? 's' : 'S';
    out[7] = (mode & S_IROTH) ? 'r' : '-';
    out[8] = (mode & S_IWOTH) ? 'w' : '-';
    out[9] = (mode & S_IXOTH) ? 'x' : '-';
#ifdef S_ISVTX
    if (mode & S_ISVTX) out[9] = (mode & S_IXOTH) ? 't' : 'T';
#endif
    out[10] = '\0';
}

static void show_user(uid_t uid, const Config *cfg)
{
    struct passwd *pw;

    if ((cfg->flags & OPT_NUMERIC) != 0u) {
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

static void show_group(gid_t gid, const Config *cfg)
{
    struct group *gr;

    if ((cfg->flags & OPT_NUMERIC) != 0u) {
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

static time_t item_time(const ListingItem *item, const Config *cfg)
{
    if ((cfg->flags & OPT_CTIME) != 0u) {
        return item->info.st_ctime;
    }
    if ((cfg->flags & OPT_ATIME) != 0u) {
        return item->info.st_atime;
    }
    return item->info.st_mtime;
}

static void show_time(time_t stamp)
{
    char text[64];
    struct tm *local = localtime(&stamp);

    if (local == NULL ||
        strftime(text, sizeof(text), "%b %e %H:%M", local) == 0) {
        printf("??? ?? ??:??");
        return;
    }

    printf("%s", text);
}

static long long get_block_size(void)
{
    const char *env = getenv("BLOCKSIZE");
    char *end;
    long long value;

    if (env == NULL || *env == '\0') {
        return 512;
    }

    value = strtoll(env, &end, 10);
    if (*end != '\0' || value <= 0) {
        return 512;
    }

    return value;
}

static long long round_up(long long value, long long unit)
{
    if (unit <= 0) {
        return value;
    }
    return (value + unit - 1) / unit;
}

long long blocks_for(const ListingItem *item, const Config *cfg)
{
    long long bytes;

    if (item == NULL || cfg == NULL) {
        return 0;
    }

    bytes = (long long)item->info.st_blocks * 512LL;

    if ((cfg->flags & OPT_HUMAN) != 0u) {
        return bytes;
    }

    if ((cfg->flags & OPT_KILOBYTES) != 0u) {
        return round_up(bytes, 1024);
    }

    return round_up(bytes, get_block_size());
}

static void show_size(long long bytes, const Config *cfg)
{
    static const char units[] = "BKMGTPE";
    double value = (double)bytes;
    int unit = 0;

    if ((cfg->flags & OPT_HUMAN) == 0u) {
        printf("%lld", bytes);
        return;
    }

    while (value >= 1024.0 && unit < 6) {
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

static void show_name(const char *name, const Config *cfg)
{
    const unsigned char *ptr;

    if (name == NULL) {
        return;
    }

    if ((cfg->flags & OPT_RAW) != 0u ||
        ((cfg->flags & (OPT_QUOTE | OPT_RAW)) == 0u &&
         !isatty(STDOUT_FILENO))) {
        printf("%s", name);
        return;
    }

    ptr = (const unsigned char *)name;
    while (*ptr != '\0') {
        putchar(isprint(*ptr) ? *ptr : '?');
        ++ptr;
    }
}

static char suffix_for(mode_t mode)
{
    if (S_ISDIR(mode)) return '/';
    if (S_ISLNK(mode)) return '@';
    if (S_ISFIFO(mode)) return '|';
#ifdef S_ISSOCK
    if (S_ISSOCK(mode)) return '=';
#endif
#ifdef S_ISWHT
    if (S_ISWHT(mode)) return '%';
#endif
    if (mode & (S_IXUSR | S_IXGRP | S_IXOTH)) return '*';
    return '\0';
}

static void show_link_target(const ListingItem *item)
{
    char target[4096];
    ssize_t size;

    if (!S_ISLNK(item->info.st_mode)) {
        return;
    }

    size = readlink(item->full_name, target, sizeof(target) - 1);
    if (size < 0) {
        return;
    }

    target[size] = '\0';
    printf(" -> %s", target);
}

static void render_long(const ListingItem *item, const Config *cfg)
{
    char mode[11];
    char suffix;

    make_mode(item->info.st_mode, mode);

    printf("%s %3lu ", mode, (unsigned long)item->info.st_nlink);
    show_user(item->info.st_uid, cfg);
    putchar(' ');
    show_group(item->info.st_gid, cfg);
    putchar(' ');

    if (S_ISCHR(item->info.st_mode) || S_ISBLK(item->info.st_mode)) {
#ifdef major
        printf("%8u,%3u ",
               (unsigned int)major(item->info.st_rdev),
               (unsigned int)minor(item->info.st_rdev));
#else
        printf("%8u,%3u ", 0u, 0u);
#endif
    } else if ((cfg->flags & OPT_HUMAN) != 0u) {
        show_size((long long)item->info.st_size, cfg);
        putchar(' ');
    } else {
        printf("%8lld ", (long long)item->info.st_size);
    }

    show_time(item_time(item, cfg));
    putchar(' ');
    show_name(item->label, cfg);

    if ((cfg->flags & OPT_CLASSIFY) != 0u) {
        suffix = suffix_for(item->info.st_mode);
        if (suffix != '\0') {
            putchar(suffix);
        }
    }

    show_link_target(item);
    putchar('\n');
}

void render_item(const ListingItem *item, const Config *cfg)
{
    char suffix;

    if (item == NULL || cfg == NULL) {
        return;
    }

    if ((cfg->flags & OPT_INODE) != 0u) {
        printf("%llu ", (unsigned long long)item->info.st_ino);
    }

    if ((cfg->flags & OPT_BLOCKS) != 0u) {
        long long value = blocks_for(item, cfg);
        if ((cfg->flags & OPT_HUMAN) != 0u) {
            show_size(value, cfg);
        } else {
            printf("%lld", value);
        }
        putchar(' ');
    }

    if ((cfg->flags & (OPT_LONG | OPT_NUMERIC)) != 0u) {
        render_long(item, cfg);
        return;
    }

    show_name(item->label, cfg);

    if ((cfg->flags & OPT_CLASSIFY) != 0u) {
        suffix = suffix_for(item->info.st_mode);
        if (suffix != '\0') {
            putchar(suffix);
        }
    }

    putchar('\n');
}

void render_total(long long total, const Config *cfg)
{
    if (cfg == NULL) {
        return;
    }

    printf("total ");
    if ((cfg->flags & OPT_HUMAN) != 0u) {
        show_size(total, cfg);
    } else {
        printf("%lld", total);
    }
    putchar('\n');
}

