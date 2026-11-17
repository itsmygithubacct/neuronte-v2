/* neuronte-cells: draw the characters a terminal is likely to show first
 * and keep them in cells files (include/neuronte_cells.h), one for each
 * terminal bundle in a directory of weights.
 *
 *   neuronte-cells --weights DIR --out DIR [--stamp FILE] [--conf FILE]
 *                  [--cells LIST] [--chars SET] [--styles LIST]
 *                  [--marks SET] [--marks-cells LIST]
 *                  [--check] [--verify] [--force] [--threads N] [--list]
 *                  [--verbose]
 *
 * The bundles are taken from the directory: the newest neuronte-split-vN,
 * every neuronte-term-* and every neuronte-kilix-*, and any other weight
 * file the settings name. A bundle with a colour net, or with no table of
 * the characters it is trained on, gets no cells file.
 *
 * What is drawn, for each bundle (NAME is the file's name without .nrt):
 *   cells    the cells, WxH,WxH,...
 *   chars    ascii, latin1 (ASCII and U+00A0 to U+00FF), all, none, or
 *            scalars and ranges in hexadecimal (2500-257F), with commas
 *            between; always only those the bundle is trained on
 *   styles   regular, bold, italic, bolditalic, with commas between
 * each read from --conf FILE (lines of key=value, the settings file of
 * neuronte-setup) as cells.NAME, cells_chars.NAME, cells_styles.NAME; else
 * from --cells, --chars, --styles; else from cells, cells_chars,
 * cells_styles in the file; else the default below. Samples per axis is 0,
 * the library's own choice, which is what the terminals pass.
 *
 * The status marks are a second, small set that the terminal bundles (the
 * split, term and kilix ones) get on top of that, in the regular style:
 *   marks        characters, written as chars is; none for no such set
 *   marks cells  the cells they are drawn at
 * read the same way, as cells_marks and cells_marks_cells, with or without
 * .NAME, or from --marks and --marks-cells. They are what Kilix draws in
 * one cell of a tab's title and of its bars and no font of Kilix's has:
 * the idle dot (U+2022, U+25CF), the eight moons (U+1F311 to U+1F318), and
 * the sun and the cloud (U+2600, U+2601), at Kilix's five cells. Each
 * bundle draws those of them it is trained on, so a bundle with no ASCII
 * still gets a cells file for its marks.
 *
 * Defaults. The cells are the ones the two terminals ask for. The
 * Neuronte-only terminal's cell is round(0.6 em) wide and twice as high:
 * 7x14, 8x16, 9x18, 10x20, 11x22 and 12x24 for 9 to 15 point at 96 dots
 * to the inch. Kilix's cell is its font's: 9x18 was measured, and 8x17,
 * 10x19, 10x20 and 12x24 are the others its tests use. A neuronte-kilix-* bundle is asked for those
 * five and for the two-cell boxes of each (16x34, 18x36, 20x38, 20x40,
 * 24x48), in the regular style only, and holds few characters: all of
 * them are drawn. Other bundles get ASCII and Latin-1 in four styles.
 *
 * --check   write nothing: exit 0 when every file is current, 1 when not.
 *           It compares sizes and times with the stamp and opens nothing.
 * --verify  with --check, also open every cells file against its weights
 *           (hashing them) and draw every entry again: exit 1 on any
 *           difference.
 * --force   make every file again.
 * A run when everything is current does nothing. Files are written beside
 * their final names and renamed; the stamp is written last.
 * MIT licence. See LICENSE. */
#define _POSIX_C_SOURCE 200809L
#include "neuronte.h"
#include "neuronte_cells.h"

#include <dirent.h>
#include <errno.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#define STAMP_HEAD "neuronte-cells 1"
#define TERMINAL_CELLS "7x14,8x16,8x17,9x18,10x19,10x20,11x22,12x24"
#define KILIX_CELLS "8x17,9x18,10x19,10x20,12x24,16x34,18x36,20x38,20x40,24x48"
#define ALL_STYLES "regular,bold,italic,bolditalic"
#define STATUS_MARKS "2022,25CF,2600,2601,1F311-1F318"
#define MARKS_CELLS "8x17,9x18,10x19,10x20,12x24"
#define MAX_BUNDLES 64
#define MAX_SETTINGS 256
#define MAX_BOXES 64
#define PATH_LEN 4096
#define LINE_LEN 2048

struct setting {
    char *key, *value;
};

struct bundle {
    char name[256]; /* without .nrt */
    const char *cells, *chars, *styles, *marks, *marks_cells;
    char line[LINE_LEN]; /* its stamp line up to the options */
    char rest[256];      /* and after them */
};

static struct setting settings[MAX_SETTINGS];
static int nsettings, verbose;
static const char *opt_cells, *opt_chars, *opt_styles, *opt_marks, *opt_marks_cells;

static void die(int code, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    fprintf(stderr, "neuronte-cells: ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    va_end(ap);
    exit(code);
}

/* snprintf that stops the command in place of cutting a name short. */
static void text(char *out, size_t size, const char *fmt, ...)
{
    va_list ap;
    int n;
    va_start(ap, fmt);
    n = vsnprintf(out, size, fmt, ap);
    va_end(ap);
    if (n < 0 || (size_t)n >= size)
        die(1, "a name or a setting is too long");
}

static char *copy(const char *s)
{
    char *c = malloc(strlen(s) + 1);
    if (!c)
        die(1, "out of memory");
    strcpy(c, s);
    return c;
}

/* The settings file: lines of key=value. Only the keys this command knows
 * are kept; the rest belong to neuronte-setup and to other steps. */
static void read_conf(const char *path)
{
    char line[LINE_LEN];
    FILE *f = fopen(path, "r");
    if (!f) {
        if (errno == ENOENT)
            return;
        die(2, "%s: %s", path, strerror(errno));
    }
    while (fgets(line, sizeof line, f)) {
        char *eq;
        line[strcspn(line, "\r\n")] = 0;
        if (!line[0] || line[0] == '#' || !(eq = strchr(line, '=')))
            continue;
        *eq = 0;
        if (strncmp(line, "cells", 5) || (line[5] && line[5] != '.' && line[5] != '_'))
            continue;
        if (nsettings == MAX_SETTINGS)
            die(2, "%s: too many cells settings", path);
        settings[nsettings].key = copy(line);
        settings[nsettings++].value = copy(eq + 1);
    }
    fclose(f);
}

static const char *setting(const char *key, const char *name)
{
    size_t len = strlen(key);
    int i;
    for (i = nsettings - 1; i >= 0; i--) {
        const char *k = settings[i].key;
        if (strncmp(k, key, len))
            continue;
        if (name ? k[len] == '.' && !strcmp(k + len + 1, name) : !k[len])
            return settings[i].value;
    }
    return NULL;
}

static const char *pick(const char *key, const char *name, const char *option, const char *builtin)
{
    const char *v = setting(key, name);
    if (!v)
        v = option;
    if (!v)
        v = setting(key, NULL);
    return v ? v : builtin;
}

static int by_name(const void *a, const void *b)
{
    return strcmp(a, b);
}

static int by_text(const void *a, const void *b)
{
    return strcmp(*(const char *const *)a, *(const char *const *)b);
}

/* The version number at the end of a name such as neuronte-split-v9. */
static long version_of(const char *name, size_t prefix)
{
    char *end;
    long v = strtol(name + prefix, &end, 10);
    return end != name + prefix && !*end ? v : -1;
}

/* The bundles, in the order of their names. */
static int find_bundles(const char *wdir, struct bundle *b)
{
    static char names[1024][256];
    char split[256] = "";
    struct dirent *e;
    long best = -1;
    int count = 0, n = 0, i;
    DIR *d = opendir(wdir);
    if (!d)
        die(1, "%s: %s", wdir, strerror(errno));
    while ((e = readdir(d)) != NULL) {
        size_t len = strlen(e->d_name);
        if (len < 5 || len > 255 || strcmp(e->d_name + len - 4, ".nrt") || e->d_name[0] == '.' ||
            strpbrk(e->d_name, " \t\n") || count == 1024)
            continue;
        memcpy(names[count], e->d_name, len - 4);
        names[count++][len - 4] = 0;
    }
    closedir(d);
    qsort(names, (size_t)count, sizeof names[0], by_name);
    for (i = 0; i < count; i++)
        if (!strncmp(names[i], "neuronte-split-v", 16) && version_of(names[i], 16) > best) {
            best = version_of(names[i], 16);
            text(split, sizeof split, "%s", names[i]);
        }
    for (i = 0; i < count && n < MAX_BUNDLES; i++) {
        int kilix = !strncmp(names[i], "neuronte-kilix-", 15);
        int terminal = !strcmp(names[i], split) || !strncmp(names[i], "neuronte-term-", 14) || kilix;
        if (!terminal && !setting("cells", names[i]) && !setting("cells_chars", names[i]))
            continue;
        memcpy(b[n].name, names[i], sizeof b[n].name);
        b[n].cells = pick("cells", names[i], opt_cells, kilix ? KILIX_CELLS : TERMINAL_CELLS);
        b[n].chars = pick("cells_chars", names[i], opt_chars, kilix ? "all" : "latin1");
        b[n].styles = pick("cells_styles", names[i], opt_styles, kilix ? "regular" : ALL_STYLES);
        b[n].marks = pick("cells_marks", names[i], opt_marks, terminal ? STATUS_MARKS : "none");
        b[n].marks_cells = pick("cells_marks_cells", names[i], opt_marks_cells, MARKS_CELLS);
        n++;
    }
    return n;
}

/* The next item of a comma-separated list, or 0 at its end. */
static int item(const char **list, char *out, size_t size)
{
    size_t len;
    while (**list == ',' || **list == ' ')
        ++*list;
    if (!**list)
        return 0;
    len = strcspn(*list, ", ");
    if (len >= size)
        die(2, "a setting's item is too long: %s", *list);
    memcpy(out, *list, len);
    out[len] = 0;
    *list += len;
    return 1;
}

static int parse_boxes(const char *list, int boxes[][2])
{
    char it[64], *end;
    int n = 0, i;
    if (!strcmp(list, "none"))
        return 0;
    while (item(&list, it, sizeof it)) {
        long w = strtol(it, &end, 10), h = *end == 'x' ? strtol(end + 1, &end, 10) : 0;
        if (*end || w < 1 || h < 1 || w > NEURONTE_MAX_CELL || h > NEURONTE_MAX_CELL)
            die(2, "not a cell (WxH, each from 1 to %d): %s", NEURONTE_MAX_CELL, it);
        for (i = 0; i < n; i++)
            if (boxes[i][0] == w && boxes[i][1] == h)
                break;
        if (i < n)
            continue;
        if (n == MAX_BOXES)
            die(2, "more than %d cells", MAX_BOXES);
        boxes[n][0] = (int)w;
        boxes[n++][1] = (int)h;
    }
    return n;
}

static unsigned parse_styles(const char *list)
{
    static const char *const names[4] = { "regular", "bold", "italic", "bolditalic" };
    char it[64];
    unsigned mask = 0;
    int i;
    while (item(&list, it, sizeof it)) {
        for (i = 0; i < 4; i++)
            if (!strcmp(it, names[i]))
                break;
        if (i == 4)
            die(2, "not a style (regular, bold, italic, bolditalic): %s", it);
        mask |= 1u << i;
    }
    return mask;
}

static size_t nstyles(const char *list)
{
    unsigned mask = parse_styles(list);
    return (mask & 1) + (mask >> 1 & 1) + (mask >> 2 & 1) + (mask >> 3 & 1);
}

/* The scalars of a set that the bundle is trained on, in rising order. */
static size_t parse_chars(const char *list, const neuronte *n, uint32_t **out)
{
    uint8_t *want = calloc(0x110000, 1);
    char it[64], *end;
    uint32_t s, *chars;
    size_t count = 0;
    if (!want)
        die(1, "out of memory");
    while (item(&list, it, sizeof it)) {
        unsigned long lo, hi;
        if (!strcmp(it, "none"))
            continue;
        if (!strcmp(it, "all")) {
            memset(want, 1, 0x110000);
            continue;
        }
        if (!strcmp(it, "ascii") || !strcmp(it, "latin1")) {
            memset(want + 0x20, 1, 0x7F - 0x20);
            if (it[0] == 'l')
                memset(want + 0xA0, 1, 0x100 - 0xA0);
            continue;
        }
        lo = hi = strtoul(it, &end, 16);
        if (end != it && *end == '-')
            hi = strtoul(end + 1, &end, 16);
        if (end == it || *end || lo > hi || hi > 0x10FFFF)
            die(2, "not a set of characters (ascii, latin1, all, none, 2500 or 2500-257F): %s", it);
        memset(want + lo, 1, hi - lo + 1);
    }
    for (s = 0; s <= 0x10FFFF; s++)
        if (want[s] && !(s >= 0xD800 && s <= 0xDFFF) && neuronte_untrained(n, s) == 0)
            count++;
        else
            want[s] = 0;
    chars = malloc((count ? count : 1) * sizeof *chars);
    if (!chars)
        die(1, "out of memory");
    for (s = 0, count = 0; s <= 0x10FFFF; s++)
        if (want[s])
            chars[count++] = s;
    free(want);
    *out = chars;
    return count;
}

struct work {
    const char *weights;
    const struct neuronte_cells_key *keys;
    float **cells;          /* drawn into, or */
    const neuronte_cells *c; /* compared with */
    size_t count;
    atomic_size_t next, wrong;
    atomic_int failed;
};

static void *draw(void *arg)
{
    struct work *w = arg;
    neuronte *n = NULL;
    float *scratch = NULL;
    size_t i, room = 0;
    if (neuronte_open_file(w->weights, &n) != NEURONTE_OK) {
        atomic_store(&w->failed, 1);
        return NULL;
    }
    while ((i = atomic_fetch_add(&w->next, 1)) < w->count && !atomic_load(&w->failed)) {
        const struct neuronte_cells_key *k = &w->keys[i];
        size_t area = (size_t)k->cell_w * k->cell_h;
        float *to = w->cells ? w->cells[i] : scratch;
        if (!w->cells && area > room) {
            free(scratch);
            to = scratch = malloc(area * sizeof *scratch);
            room = area;
            if (!scratch) {
                atomic_store(&w->failed, 1);
                break;
            }
        }
        if (neuronte_coverage_styled(n, k->scalar, k->cell_w, k->cell_h, k->samples, k->style, to) !=
            NEURONTE_OK) {
            atomic_store(&w->failed, 1);
            break;
        }
        if (w->c) {
            const float *kept = neuronte_cells_find(w->c, k->scalar, k->cell_w, k->cell_h, k->samples,
                                                    k->style, NEURONTE_CELLS_SNAP_FILE);
            if (!kept || memcmp(kept, to, area * sizeof *to))
                atomic_fetch_add(&w->wrong, 1);
        }
    }
    free(scratch);
    neuronte_close(n);
    return NULL;
}

static int run_threads(struct work *w, int threads)
{
    pthread_t id[64];
    int i, started = 0;
    if (threads > 64)
        threads = 64;
    for (i = 0; i < threads; i++)
        if (!pthread_create(&id[started], NULL, draw, w))
            started++;
    if (!started)
        draw(w);
    for (i = 0; i < started; i++)
        pthread_join(id[i], NULL);
    return atomic_load(&w->failed) ? -1 : 0;
}

static double now(void)
{
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (double)t.tv_sec + (double)t.tv_nsec / 1e9;
}

/* "SIZE SEC.NSEC" of a file, or "none". */
static void file_mark(const char *path, char *out, size_t size)
{
    struct stat st;
    if (stat(path, &st) || !S_ISREG(st.st_mode))
        text(out, size, "none");
    else
        text(out, size, "%lld %lld.%09ld", (long long)st.st_size, (long long)st.st_mtim.tv_sec,
                 (long)st.st_mtim.tv_nsec);
}

/* The keys a bundle's settings ask for. Returns their count; *why says
 * why there are none. */
static size_t plan(const struct bundle *b, const char *wpath, neuronte **handle,
                   struct neuronte_cells_key **keys, size_t *nchars, int *nboxes, const char **why)
{
    int boxes[MAX_BOXES][2], mboxes[MAX_BOXES][2], i;
    int nb = parse_boxes(b->cells, boxes), nmb = parse_boxes(b->marks_cells, mboxes);
    unsigned styles = parse_styles(b->styles), style;
    uint32_t *chars = NULL, *marks = NULL;
    size_t nc, nm, count = 0, j;
    neuronte *n = NULL;
    *keys = NULL;
    *handle = NULL;
    *nchars = 0;
    *nboxes = nb;
    if (neuronte_open_file(wpath, &n) != NEURONTE_OK)
        die(1, "%s: not a weight file this library opens", wpath);
    *handle = n;
    if (neuronte_untrained(n, 0x10FFFF) != 1) {
        *why = "no table of trained characters";
        return 0;
    }
    if (neuronte_layers(n) != 1) {
        *why = "a colour net";
        return 0;
    }
    nc = parse_chars(b->chars, n, &chars);
    nm = parse_chars(b->marks, n, &marks);
    if (!nb || !styles)
        nc = 0;
    if (!nmb)
        nm = 0;
    *nchars = nc;
    if (!nc && !nm) {
        free(chars);
        free(marks);
        *why = !nb ? "no cells asked for" : !styles ? "no styles asked for" :
               "trained on none of the characters asked for";
        return 0;
    }
    *keys = malloc((nc * (size_t)nb * 4 + nm * (size_t)nmb) * sizeof **keys);
    if (!*keys)
        die(1, "out of memory");
    for (i = 0; i < nb; i++)
        for (style = 0; style < 4; style++)
            for (j = 0; j < nc && (styles >> style & 1); j++) {
                struct neuronte_cells_key *k = &(*keys)[count++];
                k->scalar = chars[j];
                k->cell_w = (uint16_t)boxes[i][0];
                k->cell_h = (uint16_t)boxes[i][1];
                k->samples = 0;
                k->style = (uint8_t)style;
            }
    /* The status marks, regular, where the lines above did not draw them. */
    for (i = 0; i < nmb; i++) {
        int b2, in_boxes = 0;
        for (b2 = 0; b2 < nb; b2++)
            in_boxes |= boxes[b2][0] == mboxes[i][0] && boxes[b2][1] == mboxes[i][1];
        for (j = 0; j < nm; j++) {
            struct neuronte_cells_key *k;
            size_t c, drawn = 0;
            for (c = 0; c < nc && in_boxes && (styles & 1); c++)
                drawn |= chars[c] == marks[j];
            if (drawn)
                continue;
            k = &(*keys)[count++];
            k->scalar = marks[j];
            k->cell_w = (uint16_t)mboxes[i][0];
            k->cell_h = (uint16_t)mboxes[i][1];
            k->samples = 0;
            k->style = 0;
        }
    }
    free(chars);
    free(marks);
    return count;
}

/* Make one bundle's cells file. Fills b->rest. */
static void make(struct bundle *b, const char *wdir, const char *odir, int threads)
{
    char wpath[PATH_LEN], opath[PATH_LEN], mark[128];
    struct neuronte_cells_key *keys;
    const char *why = "";
    struct work w;
    neuronte *n;
    float *block, **cells;
    size_t count, nchars, i, floats = 0, at = 0;
    int nboxes, rc;
    double t0 = now();
    text(wpath, sizeof wpath, "%s/%s.nrt", wdir, b->name);
    text(opath, sizeof opath, "%s/%s.cells", odir, b->name);
    count = plan(b, wpath, &n, &keys, &nchars, &nboxes, &why);
    if (!count) {
        remove(opath);
        text(b->rest, sizeof b->rest, "entries 0 file none (%s)", why);
        if (verbose)
            fprintf(stderr, "neuronte-cells: %s: no cells file: %s\n", b->name, why);
        neuronte_close(n);
        return;
    }
    for (i = 0; i < count; i++)
        floats += (size_t)keys[i].cell_w * keys[i].cell_h;
    block = malloc(floats * sizeof *block);
    cells = malloc(count * sizeof *cells);
    if (!block || !cells)
        die(1, "out of memory");
    for (i = 0; i < count; i++) {
        cells[i] = block + at;
        at += (size_t)keys[i].cell_w * keys[i].cell_h;
    }
    memset(&w, 0, sizeof w);
    w.weights = wpath;
    w.keys = keys;
    w.cells = cells;
    w.count = count;
    if (run_threads(&w, threads))
        die(1, "%s: a character could not be drawn", b->name);
    rc = neuronte_cells_write(opath, wpath, n, NEURONTE_CELLS_SNAP_FILE, keys,
                              (const float *const *)cells, count);
    /* For every user to read, whatever the mask of the caller. */
    if (rc != NEURONTE_OK || chmod(opath, 0644))
        die(1, "%s: cannot be written (error %d)", opath, rc);
    file_mark(opath, mark, sizeof mark);
    text(b->rest, sizeof b->rest, "entries %zu file %s", count, mark);
    if (verbose)
        fprintf(stderr, "neuronte-cells: %s: %zu entries (%zu characters, %d cells, and %zu for status marks), "
                "%zu bytes, %.2f s\n", b->name, count, nchars, nboxes,
                count - nchars * (size_t)nboxes * nstyles(b->styles), 128 + 16 * count + 4 * floats, now() - t0);
    free(block);
    free(cells);
    free(keys);
    neuronte_close(n);
}

/* Open a cells file against its weights, hashing them, and draw every
 * entry it should hold again. 0 when all is as it should be. */
static int verify(const struct bundle *b, const char *wdir, const char *odir, int threads)
{
    char wpath[PATH_LEN], opath[PATH_LEN];
    struct neuronte_cells_key *keys;
    neuronte_cells *c = NULL;
    const char *why = "";
    struct work w;
    neuronte *n;
    size_t count, nchars;
    int nboxes, bad = 0, rc;
    text(wpath, sizeof wpath, "%s/%s.nrt", wdir, b->name);
    text(opath, sizeof opath, "%s/%s.cells", odir, b->name);
    count = plan(b, wpath, &n, &keys, &nchars, &nboxes, &why);
    neuronte_close(n);
    if (!count)
        return access(opath, F_OK) == 0;
    rc = neuronte_cells_open(opath, wpath, NEURONTE_CELLS_VERIFY, &c);
    if (rc != NEURONTE_OK || neuronte_cells_count(c) != count) {
        fprintf(stderr, "neuronte-cells: %s: %s\n", opath,
                rc == NEURONTE_OK ? "holds other entries than the settings ask for" :
                rc == NEURONTE_ERR_SHAPE ? "was made from other weights" : "is damaged or missing");
        bad = 1;
    } else {
        memset(&w, 0, sizeof w);
        w.weights = wpath;
        w.keys = keys;
        w.c = c;
        w.count = count;
        if (run_threads(&w, threads) || atomic_load(&w.wrong)) {
            fprintf(stderr, "neuronte-cells: %s: %zu of %zu entries are not what the net draws\n", opath,
                    (size_t)atomic_load(&w.wrong), count);
            bad = 1;
        }
    }
    neuronte_cells_close(c);
    free(keys);
    return bad;
}

int main(int argc, char **argv)
{
    static struct bundle bundles[MAX_BUNDLES];
    static char old[MAX_BUNDLES + 1][LINE_LEN + 256], own_stamp[PATH_LEN];
    const char *wdir = NULL, *odir = NULL, *stamp = NULL, *conf = NULL;
    char path[PATH_LEN], mark[128], line[LINE_LEN + 256];
    int check = 0, force = 0, deep = 0, list = 0, threads = 2, n, i, nold = 0, stale = 0, bad = 0;
    FILE *f;

    for (i = 1; i < argc; i++) {
        const char *a = argv[i], **value = NULL;
        if (!strcmp(a, "--weights")) value = &wdir;
        else if (!strcmp(a, "--out")) value = &odir;
        else if (!strcmp(a, "--stamp")) value = &stamp;
        else if (!strcmp(a, "--conf")) value = &conf;
        else if (!strcmp(a, "--cells")) value = &opt_cells;
        else if (!strcmp(a, "--chars")) value = &opt_chars;
        else if (!strcmp(a, "--styles")) value = &opt_styles;
        else if (!strcmp(a, "--marks")) value = &opt_marks;
        else if (!strcmp(a, "--marks-cells")) value = &opt_marks_cells;
        else if (!strcmp(a, "--threads") && i + 1 < argc) threads = atoi(argv[++i]);
        else if (!strcmp(a, "--check")) check = 1;
        else if (!strcmp(a, "--verify")) check = deep = 1;
        else if (!strcmp(a, "--force")) force = 1;
        else if (!strcmp(a, "--list")) list = 1;
        else if (!strcmp(a, "--verbose")) verbose = 1;
        else die(2, "unknown option %s (the top of tools/neuronte-cells.c lists them)", a);
        if (value) {
            if (i + 1 >= argc)
                die(2, "%s needs a value", a);
            *value = argv[++i];
        }
    }
    if (!wdir || !odir)
        die(2, "--weights DIR and --out DIR are needed");
    if (threads < 1)
        die(2, "--threads must be 1 or more");
    if (conf)
        read_conf(conf);
    n = find_bundles(wdir, bundles);

    /* Each bundle's stamp line up to its options: what its file is made from. */
    for (i = 0; i < n; i++) {
        int boxes[MAX_BOXES][2];
        struct bundle *b = &bundles[i];
        parse_boxes(b->cells, boxes); /* a mistake in a setting stops here, before anything is made */
        parse_boxes(b->marks_cells, boxes);
        parse_styles(b->styles);
        text(path, sizeof path, "%s/%s.nrt", wdir, b->name);
        file_mark(path, mark, sizeof mark);
        text(b->line, sizeof b->line, "bundle %s weight %s options cells=%s chars=%s styles=%s",
                 b->name, mark, b->cells, b->chars, b->styles);
        text(b->line + strlen(b->line), sizeof b->line - strlen(b->line), " marks=%s marks_cells=%s",
             b->marks, b->marks_cells);
        if (list)
            printf("%s cells=%s chars=%s styles=%s marks=%s marks_cells=%s\n", b->name, b->cells, b->chars,
                   b->styles, b->marks, b->marks_cells);
    }
    if (list)
        return 0;
    if (!stamp) {
        text(own_stamp, sizeof own_stamp, "%s/neuronte-cells.stamp", odir);
        stamp = own_stamp;
    }

    /* What the stamp says was made. A bundle is current when its line is
     * there with the same weights and options and its file is as recorded. */
    if ((f = fopen(stamp, "r")) != NULL) {
        if (fgets(line, sizeof line, f) && (line[strcspn(line, "\n")] = 0, !strcmp(line, STAMP_HEAD)))
            while (nold < MAX_BUNDLES && fgets(old[nold], sizeof old[0], f))
                old[nold][strcspn(old[nold], "\n")] = 0, nold++;
        fclose(f);
    }
    for (i = 0; i < n; i++) {
        struct bundle *b = &bundles[i];
        size_t len = strlen(b->line);
        int j, found = 0;
        for (j = 0; j < nold && !found; j++)
            if (!strncmp(old[j], b->line, len) && old[j][len] == ' ') {
                const char *rest = old[j] + len + 1, *file = strstr(rest, " file ");
                text(path, sizeof path, "%s/%s.cells", odir, b->name);
                file_mark(path, mark, sizeof mark);
                if (file && !strncmp(rest, "entries ", 8) &&
                    (!strncmp(file + 6, "none", 4) ? !strcmp(mark, "none") : !strcmp(file + 6, mark))) {
                    text(b->rest, sizeof b->rest, "%s", rest);
                    found = 1;
                }
            }
        if (!found || force)
            b->rest[0] = 0, stale++;
    }
    if (nold != n)
        stale++;
    if (check) {
        if (stale) {
            if (verbose)
                fprintf(stderr, "neuronte-cells: not current\n");
            return 1;
        }
        for (i = 0; deep && i < n; i++)
            bad |= verify(&bundles[i], wdir, odir, threads);
        return bad;
    }
    if (!stale)
        return 0;

    if (!mkdir(odir, 0755))
        chmod(odir, 0755);
    else if (errno != EEXIST)
        die(1, "%s: %s", odir, strerror(errno));
    for (i = 0; i < n; i++)
        if (!bundles[i].rest[0])
            make(&bundles[i], wdir, odir, threads);
    /* The directory is this command's: a cells file of a bundle that is no
     * longer wanted, and anything a stopped run left, is removed. */
    {
        char *gone[1024];
        struct dirent *e;
        int ngone = 0, j;
        DIR *d = opendir(odir);
        while (d && (e = readdir(d)) != NULL) {
            size_t len = strlen(e->d_name);
            int ours = 0;
            if (len > 4 && !strcmp(e->d_name + len - 4, ".tmp") && strstr(e->d_name, ".cells."))
                ours = 1;
            else if (len > 6 && !strcmp(e->d_name + len - 6, ".cells")) {
                ours = 1;
                for (j = 0; j < n; j++)
                    if (strlen(bundles[j].name) == len - 6 && !strncmp(bundles[j].name, e->d_name, len - 6) &&
                        strncmp(bundles[j].rest, "entries 0 ", 10))
                        ours = 0;
            }
            if (ours && ngone < 1024)
                gone[ngone++] = copy(e->d_name);
        }
        if (d)
            closedir(d);
        qsort(gone, (size_t)ngone, sizeof gone[0], by_text);
        for (j = 0; j < ngone; j++) {
            text(path, sizeof path, "%s/%s", odir, gone[j]);
            remove(path);
            free(gone[j]);
        }
    }
    text(path, sizeof path, "%s.%ld.tmp", stamp, (long)getpid());
    if (!(f = fopen(path, "w")))
        die(1, "%s: %s", path, strerror(errno));
    fprintf(f, "%s\n", STAMP_HEAD);
    for (i = 0; i < n; i++)
        fprintf(f, "%s %s\n", bundles[i].line, bundles[i].rest);
    if (fflush(f) || fsync(fileno(f)) || fclose(f) || rename(path, stamp))
        die(1, "%s: %s", stamp, strerror(errno));
    return 0;
}
