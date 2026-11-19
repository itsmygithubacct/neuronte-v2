/* neuronte-fontfile: write font files that ordinary programs can load,
 * made only from what the Neuronte nets draw.
 *
 *   neuronte-fontfile [--weights DIR] [--out DIR] [options]
 *
 * A program that takes its text from fontconfig and FreeType cannot run a
 * net. This command runs the nets once and writes what they drew as
 * TrueType files: an outline of every character, traced from a net's
 * coverage of a large cell, and embedded bitmap strikes that hold the
 * nets' own pixels at small sizes. Nothing is read from any font; the
 * tables are written from the OpenType specification.
 *
 *   NeuronteSans-Regular.ttf, -Bold, -Italic, -BoldItalic   the desktop faces
 *   NeuronteSansMetric-Regular.ttf, -Bold, -Italic, -BoldItalic
 *                                the desktop faces at Helvetica's advances
 *   NeuronteMono-Regular.ttf                                the terminal bundles
 *   neuronte-fontfile.stamp      the SHA-256, size and time of every weight
 *                                file used and of every file written
 *
 * Run again, it does nothing when the stamp matches the weight files and
 * the written files (exit 0). A file whose size and modification time are
 * the stamp's is taken as unchanged; one whose time differs is hashed, and
 * --verify hashes them all. --check only says whether a run would write
 * (exit 0 current, 1 not). Files are written beside their final names and
 * renamed into place, the stamp last. After writing it runs fc-cache on
 * the directory unless --no-fc-cache is given; --fc-cache-nowait starts it
 * and does not wait for the two seconds it sleeps.
 *
 * docs/neuronte-fontfile.md says what the files hold and what they lack.
 * MIT licence. See LICENSE.
 */
#define _POSIX_C_SOURCE 200809L
#include "neuronte.h"

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <spawn.h>
#include <stdarg.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <threads.h>
#include <time.h>
#include <unistd.h>

extern char **environ;

#define GENERATOR 1         /* changes whenever the same weights would give other bytes */
#define UPM 1000            /* font units to the em */
#define MAX_UI 32           /* the desktop faces draw black and white to this em */
#define MAX_TERMS 64
#define MAX_STRIKES 40
#define MAX_PATH_LEN 4096

static int verbose;

static void die(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    fputs("neuronte-fontfile: ", stderr);
    vfprintf(stderr, fmt, ap);
    fputc('\n', stderr);
    va_end(ap);
    exit(2);
}

static void *xmalloc(size_t n)
{
    void *p = malloc(n ? n : 1);
    if (!p)
        die("out of memory");
    return p;
}

static void *xcalloc(size_t n, size_t m)
{
    void *p = calloc(n ? n : 1, m ? m : 1);
    if (!p)
        die("out of memory");
    return p;
}

static void *xrealloc(void *p, size_t n)
{
    p = realloc(p, n ? n : 1);
    if (!p)
        die("out of memory");
    return p;
}

static double now(void)
{
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (double)t.tv_sec + (double)t.tv_nsec * 1e-9;
}

/* ---- SHA-256 (FIPS 180-4) ---- */

typedef struct {
    uint32_t h[8];
    uint64_t len;
    unsigned char block[64];
    size_t fill;
} sha256;

static const uint32_t K256[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};

static uint32_t rotr(uint32_t v, int n)
{
    return v >> n | v << (32 - n);
}

static void sha_block(sha256 *s, const unsigned char *p)
{
    uint32_t w[64], a, b, c, d, e, f, g, h;
    int i;
    for (i = 0; i < 16; i++)
        w[i] = (uint32_t)p[4 * i] << 24 | (uint32_t)p[4 * i + 1] << 16 | (uint32_t)p[4 * i + 2] << 8 |
               p[4 * i + 3];
    for (; i < 64; i++) {
        uint32_t s0 = rotr(w[i - 15], 7) ^ rotr(w[i - 15], 18) ^ w[i - 15] >> 3;
        uint32_t s1 = rotr(w[i - 2], 17) ^ rotr(w[i - 2], 19) ^ w[i - 2] >> 10;
        w[i] = w[i - 16] + s0 + w[i - 7] + s1;
    }
    a = s->h[0], b = s->h[1], c = s->h[2], d = s->h[3];
    e = s->h[4], f = s->h[5], g = s->h[6], h = s->h[7];
    for (i = 0; i < 64; i++) {
        uint32_t t1 = h + (rotr(e, 6) ^ rotr(e, 11) ^ rotr(e, 25)) + ((e & f) ^ (~e & g)) + K256[i] + w[i];
        uint32_t t2 = (rotr(a, 2) ^ rotr(a, 13) ^ rotr(a, 22)) + ((a & b) ^ (a & c) ^ (b & c));
        h = g, g = f, f = e, e = d + t1, d = c, c = b, b = a, a = t1 + t2;
    }
    s->h[0] += a, s->h[1] += b, s->h[2] += c, s->h[3] += d;
    s->h[4] += e, s->h[5] += f, s->h[6] += g, s->h[7] += h;
}

static void sha_init(sha256 *s)
{
    static const uint32_t iv[8] = {0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
                                   0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19};
    memcpy(s->h, iv, sizeof iv);
    s->len = 0;
    s->fill = 0;
}

static void sha_add(sha256 *s, const void *data, size_t n)
{
    const unsigned char *p = data;
    s->len += n;
    while (n) {
        size_t k = 64 - s->fill;
        if (k > n)
            k = n;
        memcpy(s->block + s->fill, p, k);
        s->fill += k, p += k, n -= k;
        if (s->fill == 64) {
            sha_block(s, s->block);
            s->fill = 0;
        }
    }
}

static void sha_hex(sha256 *s, char out[65])
{
    uint64_t bits = s->len * 8;
    unsigned char pad[72] = {0x80}, tail[8];
    size_t k = (s->fill < 56 ? 56 : 120) - s->fill;
    int i;
    for (i = 0; i < 8; i++)
        tail[i] = (unsigned char)(bits >> (56 - 8 * i));
    sha_add(s, pad, k);
    sha_add(s, tail, 8);
    for (i = 0; i < 8; i++)
        sprintf(out + 8 * i, "%08x", (unsigned)s->h[i]);
}

static int file_sha(const char *path, char out[65])
{
    static unsigned char chunk[1 << 16];
    sha256 s;
    size_t n;
    FILE *f = fopen(path, "rb");
    if (!f)
        return -1;
    sha_init(&s);
    while ((n = fread(chunk, 1, sizeof chunk, f)) > 0)
        sha_add(&s, chunk, n);
    if (ferror(f)) {
        fclose(f);
        return -1;
    }
    fclose(f);
    sha_hex(&s, out);
    return 0;
}

/* ---- a growing buffer of big-endian bytes ---- */

typedef struct {
    unsigned char *p;
    size_t n, cap;
} buf;

static void need(buf *b, size_t k)
{
    if (b->n + k > b->cap) {
        size_t c = b->cap ? b->cap : 256;
        while (c < b->n + k)
            c *= 2;
        b->p = xrealloc(b->p, c);
        b->cap = c;
    }
}

static void p8(buf *b, unsigned v)
{
    need(b, 1);
    b->p[b->n++] = (unsigned char)v;
}

static void p16(buf *b, unsigned v)
{
    p8(b, v >> 8);
    p8(b, v);
}

static void p32(buf *b, uint32_t v)
{
    p16(b, v >> 16);
    p16(b, v & 0xFFFF);
}

static void pbytes(buf *b, const void *data, size_t n)
{
    need(b, n);
    if (n)
        memcpy(b->p + b->n, data, n);
    b->n += n;
}

static void pzero(buf *b, size_t n)
{
    need(b, n);
    memset(b->p + b->n, 0, n);
    b->n += n;
}

static void pad4(buf *b)
{
    while (b->n & 3)
        p8(b, 0);
}

static void set32(buf *b, size_t at, uint32_t v)
{
    b->p[at] = (unsigned char)(v >> 24);
    b->p[at + 1] = (unsigned char)(v >> 16);
    b->p[at + 2] = (unsigned char)(v >> 8);
    b->p[at + 3] = (unsigned char)v;
}

static void pprintf(buf *b, const char *fmt, ...)
{
    char line[MAX_PATH_LEN + 256];
    va_list ap;
    int n;
    va_start(ap, fmt);
    n = vsnprintf(line, sizeof line, fmt, ap);
    va_end(ap);
    if (n < 0 || (size_t)n >= sizeof line)
        die("a line is too long");
    pbytes(b, line, (size_t)n);
}

/* ---- tracing: the half-coverage line of a cell, as closed polygons ---- */

typedef struct {
    double x, y;
} dpt;

typedef struct {
    dpt *p;
    int n;
} loop;

typedef struct {
    loop *l;
    int n;
} loops;

static void loops_free(loops *ls)
{
    int i;
    for (i = 0; i < ls->n; i++)
        free(ls->l[i].p);
    free(ls->l);
    ls->l = NULL;
    ls->n = 0;
}

static double seg_dist2(dpt p, dpt a, dpt b)
{
    double dx = b.x - a.x, dy = b.y - a.y, len = dx * dx + dy * dy, t;
    if (len <= 0.0)
        return (p.x - a.x) * (p.x - a.x) + (p.y - a.y) * (p.y - a.y);
    t = ((p.x - a.x) * dx + (p.y - a.y) * dy) / len;
    t = t < 0.0 ? 0.0 : t > 1.0 ? 1.0 : t;
    dx = a.x + t * dx - p.x;
    dy = a.y + t * dy - p.y;
    return dx * dx + dy * dy;
}

/* Mark the points of p[first..last], indices taken round the loop, that a
 * polygon within tol of the traced one has to keep (Douglas and Peucker). */
static void keep_run(const dpt *p, int n, int first, int last, double tol2, char *keep, int *stack)
{
    int top = 0;
    stack[top++] = first;
    stack[top++] = last;
    while (top) {
        int b = stack[--top], a = stack[--top], i, worst = -1;
        double far = tol2;
        for (i = a + 1; i < b; i++) {
            double d = seg_dist2(p[i % n], p[a % n], p[b % n]);
            if (d > far) {
                far = d;
                worst = i;
            }
        }
        if (worst >= 0) {
            keep[worst % n] = 1;
            stack[top++] = a;
            stack[top++] = worst;
            stack[top++] = worst;
            stack[top++] = b;
        }
    }
}

static void simplify(loop *l, double tol)
{
    int n = l->n, i, a = 0, b = 0, m = 0;
    char *keep;
    int *stack;
    double far = -1.0;
    if (n < 4)
        return;
    keep = xcalloc((size_t)n, 1);
    stack = xmalloc((size_t)(4 * n + 8) * sizeof *stack);
    for (i = 1; i < n; i++)
        if (l->p[i].x < l->p[a].x || (l->p[i].x == l->p[a].x && l->p[i].y < l->p[a].y))
            a = i;
    for (i = 0; i < n; i++) {
        double dx = l->p[i].x - l->p[a].x, dy = l->p[i].y - l->p[a].y, d = dx * dx + dy * dy;
        if (d > far) {
            far = d;
            b = i;
        }
    }
    if (b < a) {
        int t = a;
        a = b;
        b = t;
    }
    keep[a] = keep[b] = 1;
    keep_run(l->p, n, a, b, tol * tol, keep, stack);
    keep_run(l->p, n, b, a + n, tol * tol, keep, stack);
    for (i = 0; i < n; i++)
        if (keep[i])
            l->p[m++] = l->p[i];
    l->n = m;
    free(keep);
    free(stack);
}

/* Trace cov, w by h coverages in 0..1 sampled at pixel centres, along the
 * line where coverage is one half. A sample of a distance net is a ramp one
 * pixel wide across the ink edge, so the half line between two samples,
 * found by linear interpolation, is the edge the net answered. Outside the
 * cell there is no ink. Loops come out with ink on the left of travel as
 * seen on the screen, x in 0..w and y in 0..h, y down. */
static void trace(const float *cov, int w, int h, double tol, loops *out)
{
    int pw = w + 2, ph = h + 2, i, j, e, count = pw * ph * 2;
    float *p = xmalloc((size_t)pw * ph * sizeof *p);
    int *next = xmalloc((size_t)count * sizeof *next);
    char *seen = xcalloc((size_t)count, 1);

    for (i = 0; i < pw * ph; i++)
        p[i] = -0.5f;
    for (j = 0; j < h; j++)
        for (i = 0; i < w; i++)
            p[(j + 1) * pw + i + 1] = cov[(size_t)j * w + i] - 0.5f;
    for (i = 0; i < count; i++)
        next[i] = -1;
#define EH(I, J) (2 * ((J) * pw + (I)))
#define EV(I, J) (2 * ((J) * pw + (I)) + 1)
    for (j = 0; j + 1 < ph; j++)
        for (i = 0; i + 1 < pw; i++) {
            float tl = p[j * pw + i], tr = p[j * pw + i + 1];
            float bl = p[(j + 1) * pw + i], br = p[(j + 1) * pw + i + 1];
            int T = EH(i, j), B = EH(i, j + 1), L = EV(i, j), R = EV(i + 1, j);
            int k = (tl > 0) << 3 | (tr > 0) << 2 | (br > 0) << 1 | (bl > 0);
            int mid = (double)tl + tr + bl + br > 0.0;
            switch (k) {
            case 1: next[B] = L; break;
            case 2: next[R] = B; break;
            case 3: next[R] = L; break;
            case 4: next[T] = R; break;
            case 5:
                if (mid)
                    next[T] = L, next[B] = R;
                else
                    next[B] = L, next[T] = R;
                break;
            case 6: next[T] = B; break;
            case 7: next[T] = L; break;
            case 8: next[L] = T; break;
            case 9: next[B] = T; break;
            case 10:
                if (mid)
                    next[R] = T, next[L] = B;
                else
                    next[L] = T, next[R] = B;
                break;
            case 11: next[R] = T; break;
            case 12: next[L] = R; break;
            case 13: next[B] = R; break;
            case 14: next[L] = B; break;
            default: break;
            }
        }
#undef EH
#undef EV
    out->l = NULL;
    out->n = 0;
    for (e = 0; e < count; e++) {
        loop l;
        int cur = e, len = 0, cap = 64;
        if (next[e] < 0 || seen[e])
            continue;
        l.p = xmalloc((size_t)cap * sizeof *l.p);
        do {
            int cell = cur / 2, ci = cell % pw, cj = cell / pw;
            float a = p[cell], b = cur & 1 ? p[cell + pw] : p[cell + 1];
            double t = (double)a / ((double)a - (double)b);
            if (len == cap) {
                cap *= 2;
                l.p = xrealloc(l.p, (size_t)cap * sizeof *l.p);
            }
            if (cur & 1) {
                l.p[len].x = ci - 0.5;
                l.p[len].y = cj - 0.5 + t;
            } else {
                l.p[len].x = ci - 0.5 + t;
                l.p[len].y = cj - 0.5;
            }
            len++;
            seen[cur] = 1;
            cur = next[cur];
        } while (cur >= 0 && cur != e);
        l.n = len;
        simplify(&l, tol);
        out->l = xrealloc(out->l, (size_t)(out->n + 1) * sizeof *out->l);
        out->l[out->n++] = l;
    }
    free(p);
    free(next);
    free(seen);
}

/* ---- a face being built ---- */

typedef struct {
    int16_t x, y;
} ipt;

typedef struct {
    uint8_t w, h, adv, present;
    int8_t bx, by; /* the left column from the pen, and the top row above the baseline */
    uint8_t *data;
    size_t len;
} sbit;

typedef struct {
    int ppem, depth; /* depth 1: one bit a pixel; 8: one byte */
    sbit *g;         /* one per glyph */
} strike;

typedef struct {
    uint32_t scalar;
    const neuronte *src; /* the net that draws its outline */
    int advance, lsb;    /* font units */
    int dx;              /* the advance table's shift of the drawing, font units */
    loops traced;        /* cell pixels, until the baseline is known */
    int ncont, npts;
    int *end;
    ipt *pts;
    int xmin, ymin, xmax, ymax;
} glyph;

typedef struct {
    const char *file, *family, *style, *psname;
    int bold, italic, mono;
    int nglyphs;
    glyph *g; /* glyph 0 is .notdef, the rest in scalar order */
    int nstrikes;
    strike s[MAX_STRIKES];
    int ascender, descender; /* font units, descender negative */
    int xheight, capheight;
    int cell_w;        /* trace cell, pixels */
    double unit;       /* font units per trace pixel */
    double base;       /* the baseline's row in the trace cell */
    char source[65];   /* a digest of the weight files it was made from */
} face;

static int find_glyph(const face *f, uint32_t scalar)
{
    int lo = 1, hi = f->nglyphs;
    while (lo < hi) {
        int mid = lo + (hi - lo) / 2;
        if (f->g[mid].scalar < scalar)
            lo = mid + 1;
        else
            hi = mid;
    }
    return lo < f->nglyphs && f->g[lo].scalar == scalar ? lo : -1;
}

/* ---- jobs, run on a few threads ---- */

typedef struct {
    void (*run)(void *arg, int index);
    void *arg;
    int count;
    atomic_int next;
} jobs;

static int worker(void *arg)
{
    jobs *j = arg;
    for (;;) {
        int i = atomic_fetch_add(&j->next, 1);
        if (i >= j->count)
            return 0;
        j->run(j->arg, i);
    }
}

static void run_jobs(void (*run)(void *, int), void *arg, int count, int threads)
{
    jobs j;
    thrd_t t[16];
    int i, started = 0;
    j.run = run;
    j.arg = arg;
    j.count = count;
    atomic_init(&j.next, 0);
    if (threads > 16)
        threads = 16;
    for (i = 1; i < threads && i < count; i++)
        if (thrd_create(&t[started], worker, &j) == thrd_success)
            started++;
    worker(&j);
    for (i = 0; i < started; i++)
        thrd_join(t[i], NULL);
}

/* ---- outlines ---- */

typedef struct {
    face *f;
    double tol;
    float **kept;         /* a desktop face: each glyph's coverage of the trace cell, drawn once */
    int stroke;           /* and how far the library's bold strikes it to the right, in pixels */
    const neuronte *mark; /* draws glyph 0, with the untrained mark on */
    uint32_t mark_scalar;
    int mark_w;
} trace_arg;

static void trace_one(void *varg, int i)
{
    trace_arg *a = varg;
    face *f = a->f;
    glyph *g = &f->g[i];
    int w = i ? f->cell_w : a->mark_w, h = 2 * w, rc, x, y, k;
    float *cov = xmalloc((size_t)w * h * sizeof *cov);
    if (i == 0)
        rc = a->mark ? neuronte_coverage(a->mark, a->mark_scalar, w, h, 1, cov) : -1;
    else if (a->kept) {
        /* The library's bold of a cell: each pixel is the greatest coverage
         * from itself to stroke pixels to its left (neuronte_coverage_styled). */
        const float *base = a->kept[i];
        for (y = 0; y < h; y++)
            for (x = 0; x < w; x++) {
                float v = base[(size_t)y * w + x];
                for (k = 1; k <= a->stroke && k <= x; k++)
                    if (base[(size_t)y * w + x - k] > v)
                        v = base[(size_t)y * w + x - k];
                cov[(size_t)y * w + x] = v;
            }
        rc = NEURONTE_OK;
    } else
        rc = neuronte_coverage(g->src, g->scalar, w, h, 1, cov);
    if (rc == NEURONTE_OK)
        trace(cov, w, h, a->tol, &g->traced);
    else if (i)
        die("U+%04X: the net did not draw (%d)", (unsigned)g->scalar, rc);
    free(cov);
}

static double loops_bottom(const loops *ls)
{
    double y = -1.0;
    int i, k;
    for (i = 0; i < ls->n; i++)
        for (k = 0; k < ls->l[i].n; k++)
            if (ls->l[i].p[k].y > y)
                y = ls->l[i].p[k].y;
    return y;
}

static double loops_top(const loops *ls)
{
    double y = 1e9;
    int i, k;
    for (i = 0; i < ls->n; i++)
        for (k = 0; k < ls->l[i].n; k++)
            if (ls->l[i].p[k].y < y)
                y = ls->l[i].p[k].y;
    return y;
}

/* Cell pixels to font units. x is from the pen, y up from the baseline.
 * shear is the library's italic for a cell w wide and h high: a row is
 * moved right by (0.675 - y / h) * w / 2. */
static void to_units(glyph *g, double unit, double base, int shear, int w, int h)
{
    int i, k;
    size_t total = 0;
    for (i = 0; i < g->traced.n; i++)
        total += (size_t)g->traced.l[i].n;
    g->pts = xmalloc((total + 1) * sizeof *g->pts);
    g->end = xmalloc(((size_t)g->traced.n + 1) * sizeof *g->end);
    g->npts = g->ncont = 0;
    for (i = 0; i < g->traced.n; i++) {
        const loop *l = &g->traced.l[i];
        int first = g->npts;
        /* Backwards: a TrueType outline keeps its ink on the right. */
        for (k = l->n - 1; k >= 0; k--) {
            double x = l->p[k].x, y = l->p[k].y;
            ipt q;
            if (shear)
                x += (0.675 - y / h) * w / 2.0;
            q.x = (int16_t)(floor(x * unit + 0.5) + g->dx);
            q.y = (int16_t)floor((base - y) * unit + 0.5);
            if (g->npts > first && g->pts[g->npts - 1].x == q.x && g->pts[g->npts - 1].y == q.y)
                continue;
            g->pts[g->npts++] = q;
        }
        while (g->npts - first > 1 && g->pts[first].x == g->pts[g->npts - 1].x &&
               g->pts[first].y == g->pts[g->npts - 1].y)
            g->npts--;
        if (g->npts - first < 3) {
            g->npts = first; /* too small to enclose a font unit */
            continue;
        }
        g->end[g->ncont++] = g->npts - 1;
    }
    g->xmin = g->ymin = g->xmax = g->ymax = 0;
    for (k = 0; k < g->npts; k++) {
        if (!k || g->pts[k].x < g->xmin)
            g->xmin = g->pts[k].x;
        if (!k || g->pts[k].x > g->xmax)
            g->xmax = g->pts[k].x;
        if (!k || g->pts[k].y < g->ymin)
            g->ymin = g->pts[k].y;
        if (!k || g->pts[k].y > g->ymax)
            g->ymax = g->pts[k].y;
    }
    g->lsb = g->xmin;
    loops_free(&g->traced);
}

/* ---- the sfnt tables ---- */

static void pname(buf *records, buf *strings, int platform, int name_id, const char *text)
{
    size_t i, len = strlen(text);
    p16(records, platform);
    p16(records, platform == 3 ? 1 : 0);
    p16(records, platform == 3 ? 0x409 : 0);
    p16(records, name_id);
    p16(records, (unsigned)(platform == 3 ? 2 * len : len));
    p16(records, (unsigned)strings->n);
    for (i = 0; i < len; i++) {
        if (platform == 3)
            p8(strings, 0);
        p8(strings, (unsigned char)text[i]);
    }
}

static void table_name(const face *f, buf *b)
{
    buf rec = {0}, str = {0};
    char full[128], unique[160], version[64], about[256];
    const char *text[14] = {0};
    static const int ids[] = {1, 2, 3, 4, 5, 6, 10, 13};
    int platform, i, count = 0;

    if (!strcmp(f->style, "Regular"))
        snprintf(full, sizeof full, "%s", f->family);
    else
        snprintf(full, sizeof full, "%s %s", f->family, f->style);
    snprintf(version, sizeof version, "Version 1.%03d", GENERATOR);
    snprintf(unique, sizeof unique, "%s;%.16s", f->psname, f->source);
    snprintf(about, sizeof about,
             "Written by neuronte-fontfile from what the Neuronte nets draw. Weights %.16s.", f->source);
    text[1] = f->family;
    text[2] = f->style;
    text[3] = unique;
    text[4] = full;
    text[5] = version;
    text[6] = f->psname;
    text[10] = about;
    text[13] = "MIT licence";
    for (platform = 1; platform <= 3; platform += 2)
        for (i = 0; i < (int)(sizeof ids / sizeof ids[0]); i++) {
            pname(&rec, &str, platform, ids[i], text[ids[i]]);
            count++;
        }
    p16(b, 0);
    p16(b, (unsigned)count);
    p16(b, (unsigned)(6 + rec.n));
    pbytes(b, rec.p, rec.n);
    pbytes(b, str.p, str.n);
    free(rec.p);
    free(str.p);
}

/* Runs of scalars whose glyphs are consecutive too. Glyphs are in scalar
 * order, so a run is a run of consecutive scalars. */
typedef struct {
    uint32_t first, last;
    int glyph;
} run;

static int cmap_runs(const face *f, uint32_t lo, uint32_t hi, run **out)
{
    run *r = xmalloc((size_t)f->nglyphs * sizeof *r);
    int i, n = 0;
    for (i = 1; i < f->nglyphs; i++) {
        uint32_t s = f->g[i].scalar;
        if (s < lo || s > hi)
            continue;
        if (n && r[n - 1].last + 1 == s)
            r[n - 1].last = s;
        else {
            r[n].first = r[n].last = s;
            r[n].glyph = i;
            n++;
        }
    }
    *out = r;
    return n;
}

static void table_cmap(const face *f, buf *b)
{
    run *bmp, *all;
    int nb = cmap_runs(f, 0, 0xFFFE, &bmp), na = cmap_runs(f, 0, 0x10FFFF, &all);
    int astral = f->nglyphs > 1 && f->g[f->nglyphs - 1].scalar > 0xFFFF, i;
    int seg = nb + 1, search = 1, selector = 0;
    size_t f4 = (size_t)16 + 8 * (size_t)seg, at4, at12;

    p16(b, 0);
    p16(b, astral ? 4 : 2);
    at4 = (size_t)4 + 8 * (astral ? 4 : 2);
    at12 = at4 + f4;
    p16(b, 0), p16(b, 3), p32(b, (uint32_t)at4);
    if (astral)
        p16(b, 0), p16(b, 4), p32(b, (uint32_t)at12);
    p16(b, 3), p16(b, 1), p32(b, (uint32_t)at4);
    if (astral)
        p16(b, 3), p16(b, 10), p32(b, (uint32_t)at12);

    while (search * 2 <= seg) {
        search *= 2;
        selector++;
    }
    p16(b, 4), p16(b, (unsigned)f4), p16(b, 0);
    p16(b, (unsigned)(2 * seg));
    p16(b, (unsigned)(2 * search));
    p16(b, (unsigned)selector);
    p16(b, (unsigned)(2 * seg - 2 * search));
    for (i = 0; i < nb; i++)
        p16(b, bmp[i].last);
    p16(b, 0xFFFF);
    p16(b, 0);
    for (i = 0; i < nb; i++)
        p16(b, bmp[i].first);
    p16(b, 0xFFFF);
    for (i = 0; i < nb; i++)
        p16(b, (unsigned)(bmp[i].glyph - (int)bmp[i].first) & 0xFFFF);
    p16(b, 1);
    for (i = 0; i < seg; i++)
        p16(b, 0);
    if (astral) {
        p16(b, 12), p16(b, 0);
        p32(b, (uint32_t)(16 + 12 * (size_t)na));
        p32(b, 0);
        p32(b, (uint32_t)na);
        for (i = 0; i < na; i++)
            p32(b, all[i].first), p32(b, all[i].last), p32(b, (uint32_t)all[i].glyph);
    }
    free(bmp);
    free(all);
}

static void table_glyf(const face *f, buf *glyf, buf *loca)
{
    int i, k;
    for (i = 0; i < f->nglyphs; i++) {
        const glyph *g = &f->g[i];
        int px = 0, py = 0;
        p32(loca, (uint32_t)glyf->n);
        if (!g->ncont)
            continue;
        p16(glyf, (unsigned)g->ncont);
        p16(glyf, (unsigned)g->xmin & 0xFFFF);
        p16(glyf, (unsigned)g->ymin & 0xFFFF);
        p16(glyf, (unsigned)g->xmax & 0xFFFF);
        p16(glyf, (unsigned)g->ymax & 0xFFFF);
        for (k = 0; k < g->ncont; k++)
            p16(glyf, (unsigned)g->end[k]);
        p16(glyf, 0);
        /* Every point is on the curve: the outline is the traced polygon. */
        for (k = 0; k < g->npts; k++) {
            int dx = g->pts[k].x - px, dy = g->pts[k].y - py;
            unsigned flag = 0x01;
            if (dx == 0)
                flag |= 0x10;
            else if (dx > -256 && dx < 256)
                flag |= 0x02 | (dx > 0 ? 0x10 : 0);
            if (dy == 0)
                flag |= 0x20;
            else if (dy > -256 && dy < 256)
                flag |= 0x04 | (dy > 0 ? 0x20 : 0);
            p8(glyf, flag);
            px = g->pts[k].x, py = g->pts[k].y;
        }
        for (px = 0, k = 0; k < g->npts; px = g->pts[k].x, k++) {
            int dx = g->pts[k].x - px;
            if (dx == 0)
                continue;
            if (dx > -256 && dx < 256)
                p8(glyf, (unsigned)abs(dx));
            else
                p16(glyf, (unsigned)dx & 0xFFFF);
        }
        for (py = 0, k = 0; k < g->npts; py = g->pts[k].y, k++) {
            int dy = g->pts[k].y - py;
            if (dy == 0)
                continue;
            if (dy > -256 && dy < 256)
                p8(glyf, (unsigned)abs(dy));
            else
                p16(glyf, (unsigned)dy & 0xFFFF);
        }
        pad4(glyf);
    }
    p32(loca, (uint32_t)glyf->n);
}

static int clamp_i8(int v)
{
    return v < -128 ? -128 : v > 127 ? 127 : v;
}

/* EBLC and EBDT: one strike a size, index format 1 (an offset a glyph)
 * over each run of glyphs that have a bitmap, image format 1 (small
 * metrics, then rows that each start on a byte). */
static void table_bitmaps(const face *f, buf *eblc, buf *ebdt)
{
    int s, i;
    size_t sizes_at, arrays_at;
    p16(ebdt, 2), p16(ebdt, 0);
    p16(eblc, 2), p16(eblc, 0);
    p32(eblc, (uint32_t)f->nstrikes);
    sizes_at = eblc->n;
    pzero(eblc, (size_t)48 * (size_t)f->nstrikes);
    for (s = 0; s < f->nstrikes; s++) {
        const strike *st = &f->s[s];
        buf heads = {0}, subs = {0};
        int nsub = 0, first = -1, last = -1, asc = 0, desc = 0, wmax = 0;
        size_t at = sizes_at + (size_t)48 * (size_t)s;
        for (i = 0; i < f->nglyphs;) {
            int j = i, k;
            size_t start;
            if (!st->g[i].present) {
                i++;
                continue;
            }
            while (j + 1 < f->nglyphs && st->g[j + 1].present)
                j++;
            if (first < 0)
                first = i;
            last = j;
            p16(&heads, (unsigned)i), p16(&heads, (unsigned)j);
            p32(&heads, (uint32_t)subs.n); /* less the array of heads, added below */
            p16(&subs, 1), p16(&subs, 1);
            p32(&subs, (uint32_t)ebdt->n);
            start = ebdt->n;
            for (k = i; k <= j; k++) {
                const sbit *g = &st->g[k];
                p32(&subs, (uint32_t)(ebdt->n - start));
                p8(ebdt, g->h), p8(ebdt, g->w);
                p8(ebdt, (unsigned)g->bx & 0xFF), p8(ebdt, (unsigned)g->by & 0xFF);
                p8(ebdt, g->adv);
                pbytes(ebdt, g->data, g->len);
                if (g->by > asc)
                    asc = g->by;
                if (g->h - g->by > desc)
                    desc = g->h - g->by;
                if (g->adv > wmax)
                    wmax = g->adv;
            }
            p32(&subs, (uint32_t)(ebdt->n - start));
            nsub++;
            i = j + 1;
        }
        for (i = 0; i < nsub; i++) {
            size_t off = (size_t)8 * (size_t)i + 4;
            uint32_t v = (uint32_t)heads.p[off] << 24 | (uint32_t)heads.p[off + 1] << 16 |
                         (uint32_t)heads.p[off + 2] << 8 | heads.p[off + 3];
            set32(&heads, off, v + (uint32_t)heads.n);
        }
        arrays_at = eblc->n;
        pbytes(eblc, heads.p, heads.n);
        pbytes(eblc, subs.p, subs.n);
        set32(eblc, at, (uint32_t)arrays_at);
        set32(eblc, at + 4, (uint32_t)(heads.n + subs.n));
        set32(eblc, at + 8, (uint32_t)nsub);
        for (i = 0; i < 2; i++) { /* the line, across and down */
            unsigned char *m = eblc->p + at + 16 + 12 * i;
            m[0] = (unsigned char)clamp_i8(asc);
            m[1] = (unsigned char)clamp_i8(-desc);
            m[2] = (unsigned char)wmax;
            m[3] = 1; /* an upright caret */
            m[8] = (unsigned char)clamp_i8(asc);
            m[9] = (unsigned char)clamp_i8(-desc);
        }
        eblc->p[at + 40] = (unsigned char)(first >> 8), eblc->p[at + 41] = (unsigned char)first;
        eblc->p[at + 42] = (unsigned char)(last >> 8), eblc->p[at + 43] = (unsigned char)last;
        eblc->p[at + 44] = eblc->p[at + 45] = (unsigned char)st->ppem;
        eblc->p[at + 46] = (unsigned char)st->depth;
        eblc->p[at + 47] = 1; /* horizontal metrics */
        free(heads.p);
        free(subs.p);
    }
}

/* The Unicode range bits of OS/2 that this face's characters fall in. */
static void unicode_ranges(const face *f, uint32_t bits[4])
{
    static const struct {
        uint32_t lo, hi;
        int bit;
    } blocks[] = {{0x0020, 0x007E, 0},   {0x00A0, 0x00FF, 1},   {0x0100, 0x017F, 2},   {0x0180, 0x024F, 3},
                  {0x0250, 0x02AF, 4},   {0x02B0, 0x02FF, 5},   {0x0300, 0x036F, 6},   {0x0370, 0x03FF, 7},
                  {0x0400, 0x04FF, 9},   {0x0530, 0x058F, 10},  {0x0590, 0x05FF, 11},  {0x0E80, 0x0EFF, 25},
                  {0x10A0, 0x10FF, 26},  {0x1E00, 0x1EFF, 29},  {0x1F00, 0x1FFF, 30},  {0x2000, 0x206F, 31},
                  {0x2070, 0x209F, 32},  {0x20A0, 0x20CF, 33},  {0x2100, 0x214F, 35},  {0x2150, 0x218F, 36},
                  {0x2190, 0x21FF, 37},  {0x2200, 0x22FF, 38},  {0x2300, 0x23FF, 39},  {0x2400, 0x243F, 40},
                  {0x2440, 0x245F, 41},  {0x2460, 0x24FF, 42},  {0x2500, 0x257F, 43},  {0x2580, 0x259F, 44},
                  {0x25A0, 0x25FF, 45},  {0x2600, 0x26FF, 46},  {0x2700, 0x27BF, 47},  {0x2800, 0x28FF, 82},
                  {0xE000, 0xF8FF, 60},  {0x10000, 0x10FFFF, 57}};
    int i;
    size_t k;
    memset(bits, 0, 4 * sizeof bits[0]);
    for (i = 1; i < f->nglyphs; i++)
        for (k = 0; k < sizeof blocks / sizeof blocks[0]; k++)
            if (f->g[i].scalar >= blocks[k].lo && f->g[i].scalar <= blocks[k].hi)
                bits[blocks[k].bit / 32] |= 1u << (blocks[k].bit % 32);
}

static uint32_t checksum(const unsigned char *p, size_t n)
{
    uint32_t sum = 0;
    size_t i;
    for (i = 0; i < n; i++)
        sum += (uint32_t)p[i] << (24 - 8 * (i & 3));
    return sum;
}

/* The whole file. outlines 0 leaves glyf and loca out (bitmaps only). */
static void build_sfnt(const face *f, int outlines, buf *out)
{
    enum { CMAP, EBDT, EBLC, OS2, GASP, GLYF, HEAD, HHEA, HMTX, LOCA, MAXP, NAME, POST, NTAB };
    static const char *tags[NTAB] = {"cmap", "EBDT", "EBLC", "OS/2", "gasp", "glyf", "head",
                                     "hhea", "hmtx", "loca", "maxp", "name", "post"};
    /* The directory is sorted by tag: capitals come first. */
    static const int order[NTAB] = {EBDT, EBLC, OS2, CMAP, GASP, GLYF, HEAD, HHEA, HMTX, LOCA, MAXP, NAME, POST};
    buf t[NTAB];
    int i, k, n = 0, present[NTAB], search = 1, selector = 0;
    int xmin = 0, ymin = 0, xmax = 0, ymax = 0, maxpts = 0, maxcont = 0, advmax = 0;
    int minlsb = 0, minrsb = 0, extent = 0, seen = 0, line = (UPM + 7) / 14;
    long advsum = 0;
    uint32_t ranges[4], last_char = 0, first_char = 0xFFFF;
    size_t at, head_at = 0, length[NTAB];

    memset(t, 0, sizeof t);
    for (i = 0; i < f->nglyphs; i++) {
        const glyph *g = &f->g[i];
        if (g->advance > advmax)
            advmax = g->advance;
        advsum += g->advance;
        if (i) {
            if (g->scalar < first_char)
                first_char = g->scalar;
            last_char = g->scalar > 0xFFFF ? 0xFFFF : g->scalar;
        }
        if (!g->ncont)
            continue;
        if (!seen || g->xmin < xmin)
            xmin = g->xmin;
        if (!seen || g->ymin < ymin)
            ymin = g->ymin;
        if (!seen || g->xmax > xmax)
            xmax = g->xmax;
        if (!seen || g->ymax > ymax)
            ymax = g->ymax;
        if (!seen || g->lsb < minlsb)
            minlsb = g->lsb;
        if (!seen || g->advance - g->xmax < minrsb)
            minrsb = g->advance - g->xmax;
        if (g->xmax > extent)
            extent = g->xmax;
        if (g->npts > maxpts)
            maxpts = g->npts;
        if (g->ncont > maxcont)
            maxcont = g->ncont;
        seen = 1;
    }

    /* head */
    p32(&t[HEAD], 0x00010000);
    p32(&t[HEAD], 0x00010000 + (uint32_t)GENERATOR * 0x10000 / 1000);
    p32(&t[HEAD], 0);
    p32(&t[HEAD], 0x5F0F3CF5);
    p16(&t[HEAD], 0x000B); /* baseline at 0, left side at 0, whole pixel sizes */
    p16(&t[HEAD], UPM);
    /* No clock: the same weights give the same bytes. 2026-01-01, in
     * seconds from 1904. */
    for (i = 0; i < 2; i++)
        p32(&t[HEAD], 0), p32(&t[HEAD], 3850070400u);
    p16(&t[HEAD], (unsigned)xmin & 0xFFFF), p16(&t[HEAD], (unsigned)ymin & 0xFFFF);
    p16(&t[HEAD], (unsigned)xmax & 0xFFFF), p16(&t[HEAD], (unsigned)ymax & 0xFFFF);
    p16(&t[HEAD], (unsigned)((f->bold ? 1 : 0) | (f->italic ? 2 : 0)));
    p16(&t[HEAD], 6);
    p16(&t[HEAD], 2);
    p16(&t[HEAD], 1); /* loca holds 32-bit offsets */
    p16(&t[HEAD], 0);

    /* hhea */
    p32(&t[HHEA], 0x00010000);
    p16(&t[HHEA], (unsigned)f->ascender & 0xFFFF);
    p16(&t[HHEA], (unsigned)f->descender & 0xFFFF);
    p16(&t[HHEA], 0);
    p16(&t[HHEA], (unsigned)advmax);
    p16(&t[HHEA], (unsigned)minlsb & 0xFFFF);
    p16(&t[HHEA], (unsigned)minrsb & 0xFFFF);
    p16(&t[HHEA], (unsigned)extent & 0xFFFF);
    p16(&t[HHEA], f->italic ? 4 : 1); /* the caret leans as the italic does, 1 across in 4 up */
    p16(&t[HHEA], f->italic ? 1 : 0);
    pzero(&t[HHEA], 10);
    p16(&t[HHEA], 0);
    p16(&t[HHEA], (unsigned)f->nglyphs);

    /* maxp */
    p32(&t[MAXP], outlines ? 0x00010000 : 0x00005000);
    p16(&t[MAXP], (unsigned)f->nglyphs);
    if (outlines) {
        p16(&t[MAXP], (unsigned)maxpts);
        p16(&t[MAXP], (unsigned)maxcont);
        p16(&t[MAXP], 0), p16(&t[MAXP], 0);
        p16(&t[MAXP], 1);
        pzero(&t[MAXP], 16);
    }

    /* hmtx */
    for (i = 0; i < f->nglyphs; i++)
        p16(&t[HMTX], (unsigned)f->g[i].advance), p16(&t[HMTX], (unsigned)f->g[i].lsb & 0xFFFF);

    /* OS/2, version 4 */
    unicode_ranges(f, ranges);
    p16(&t[OS2], 4);
    p16(&t[OS2], (unsigned)(advsum / f->nglyphs));
    p16(&t[OS2], f->bold ? 700 : 400);
    p16(&t[OS2], 5);
    p16(&t[OS2], 0); /* may be embedded and installed */
    /* Sub- and superscripts: 65% of the em, 14% below and 48% above. */
    p16(&t[OS2], UPM * 65 / 100), p16(&t[OS2], UPM * 65 / 100), p16(&t[OS2], 0), p16(&t[OS2], UPM * 14 / 100);
    p16(&t[OS2], UPM * 65 / 100), p16(&t[OS2], UPM * 65 / 100), p16(&t[OS2], 0), p16(&t[OS2], UPM * 48 / 100);
    p16(&t[OS2], (unsigned)line);
    p16(&t[OS2], (unsigned)((f->xheight + line) / 2));
    p16(&t[OS2], 0);
    for (i = 0; i < 10; i++)
        p8(&t[OS2], i == 3 && f->mono ? 9 : 0); /* PANOSE: only "monospaced" is stated */
    for (i = 0; i < 4; i++)
        p32(&t[OS2], ranges[i]);
    pbytes(&t[OS2], "NRNT", 4);
    p16(&t[OS2], (unsigned)((f->italic ? 0x01 : 0) | (f->bold ? 0x20 : 0) | (!f->italic && !f->bold ? 0x40 : 0)));
    p16(&t[OS2], first_char);
    p16(&t[OS2], last_char);
    p16(&t[OS2], (unsigned)f->ascender & 0xFFFF);
    p16(&t[OS2], (unsigned)f->descender & 0xFFFF);
    p16(&t[OS2], 0);
    p16(&t[OS2], (unsigned)f->ascender);
    p16(&t[OS2], (unsigned)-f->descender);
    p32(&t[OS2], 1); /* code page 1252 */
    p32(&t[OS2], 0);
    p16(&t[OS2], (unsigned)f->xheight);
    p16(&t[OS2], (unsigned)f->capheight);
    p16(&t[OS2], 0);
    p16(&t[OS2], 32);
    p16(&t[OS2], 0);

    /* post, version 3: no glyph names */
    p32(&t[POST], 0x00030000);
    p32(&t[POST], f->italic ? (uint32_t)-919781 : 0); /* -atan(1/4) in degrees, 16.16 */
    p16(&t[POST], (unsigned)-line & 0xFFFF);
    p16(&t[POST], (unsigned)line);
    p32(&t[POST], f->mono ? 1 : 0);
    pzero(&t[POST], 16);

    /* gasp: grey at every size, no grid fitting; there are no instructions */
    p16(&t[GASP], 1), p16(&t[GASP], 1), p16(&t[GASP], 0xFFFF), p16(&t[GASP], 0x000A);

    table_name(f, &t[NAME]);
    table_cmap(f, &t[CMAP]);
    if (outlines)
        table_glyf(f, &t[GLYF], &t[LOCA]);
    if (f->nstrikes)
        table_bitmaps(f, &t[EBLC], &t[EBDT]);

    for (i = 0; i < NTAB; i++) {
        present[i] = t[i].n > 0;
        n += present[i];
    }
    while (search * 2 <= n) {
        search *= 2;
        selector++;
    }
    p32(out, 0x00010000);
    p16(out, (unsigned)n);
    p16(out, (unsigned)(16 * search));
    p16(out, (unsigned)selector);
    p16(out, (unsigned)(16 * n - 16 * search));
    at = (size_t)12 + 16 * (size_t)n;
    for (k = 0; k < NTAB; k++) {
        i = order[k];
        if (!present[i])
            continue;
        pbytes(out, tags[i], 4);
        length[i] = t[i].n;
        pad4(&t[i]);
        p32(out, checksum(t[i].p, t[i].n));
        p32(out, (uint32_t)at);
        p32(out, (uint32_t)length[i]);
        if (i == HEAD)
            head_at = at;
        at += t[i].n;
    }
    for (k = 0; k < NTAB; k++)
        if (present[order[k]])
            pbytes(out, t[order[k]].p, t[order[k]].n);
    set32(out, head_at + 8, 0xB1B0AFBAu - checksum(out->p, out->n));
    for (i = 0; i < NTAB; i++)
        free(t[i].p);
}

/* ---- the weight files ---- */

typedef struct {
    char dir[MAX_PATH_LEN];
    char names[512][128];
    int count;
} listing;

static int cmp_name(const void *a, const void *b)
{
    return strcmp(a, b);
}

static void list_dir(listing *l, const char *dir)
{
    DIR *d = opendir(dir);
    struct dirent *e;
    if (!d)
        die("%s: %s", dir, strerror(errno));
    snprintf(l->dir, sizeof l->dir, "%s", dir);
    l->count = 0;
    while ((e = readdir(d)) != NULL) {
        size_t len = strlen(e->d_name);
        if (len < 5 || len >= sizeof l->names[0] || strcmp(e->d_name + len - 4, ".nrt"))
            continue;
        /* The stamp is lines of words: a name with a space could not be
         * recorded, and no shipped file has one. */
        if (strpbrk(e->d_name, " \t\r\n")) {
            fprintf(stderr, "neuronte-fontfile: %s/%s: not used, its name has a space\n", dir, e->d_name);
            continue;
        }
        if (l->count < 512)
            memcpy(l->names[l->count++], e->d_name, len + 1);
    }
    closedir(d);
    qsort(l->names, (size_t)l->count, sizeof l->names[0], cmp_name);
}

/* The file PREFIX N .nrt with the greatest N, or NULL. */
static const char *newest(const listing *l, const char *prefix)
{
    size_t plen = strlen(prefix);
    const char *best = NULL;
    long best_v = -1;
    int i;
    for (i = 0; i < l->count; i++) {
        char *end;
        long v;
        if (strncmp(l->names[i], prefix, plen))
            continue;
        v = strtol(l->names[i] + plen, &end, 10);
        if (end == l->names[i] + plen || strcmp(end, ".nrt") || v <= best_v)
            continue;
        best_v = v;
        best = l->names[i];
    }
    return best;
}

typedef struct {
    long long size, sec;
    long nsec;
} filetime;

static int file_time(const char *path, filetime *t)
{
    struct stat st;
    if (stat(path, &st))
        return -1;
    t->size = (long long)st.st_size;
    t->sec = (long long)st.st_mtim.tv_sec;
    t->nsec = (long)st.st_mtim.tv_nsec;
    return 0;
}

typedef struct {
    char name[128];
    char sha[65];
    filetime t;
    neuronte *n;
} weight;

typedef struct {
    weight w[160];
    int count;
    int names_only; /* list what would be used; read and open nothing */
} weights;

static neuronte *use(weights *ws, const listing *l, const char *name)
{
    char path[MAX_PATH_LEN + 160];
    weight *w;
    int i, rc;
    if (!name)
        return NULL;
    for (i = 0; i < ws->count; i++)
        if (!strcmp(ws->w[i].name, name))
            return ws->w[i].n;
    if (ws->count == 160)
        die("too many weight files");
    w = &ws->w[ws->count];
    snprintf(path, sizeof path, "%s/%s", l->dir, name);
    snprintf(w->name, sizeof w->name, "%s", name);
    if (ws->names_only) {
        w->n = NULL;
        ws->count++;
        return NULL;
    }
    /* The time before the bytes: a file changed while it is read is then
     * found changed by the next run. */
    if (file_time(path, &w->t) || file_sha(path, w->sha))
        die("%s: %s", path, strerror(errno));
    rc = neuronte_open_file(path, &w->n);
    if (rc != NEURONTE_OK)
        die("%s: not a weight file this library opens (%d)", path, rc);
    ws->count++;
    return w->n;
}

typedef struct {
    neuronte *ui[MAX_UI + 1], *ui_italic[MAX_UI + 1];
    neuronte *bold_ui[MAX_UI + 1], *bold_italic[MAX_UI + 1]; /* drawn bold faces (Metric) */
    neuronte *scaled, *slant, *large;
    neuronte *term[MAX_TERMS];
    int terms;
    int metric; /* Neuronte Sans Metric: advances in thousandths, shifts, drawn bold */
} sources;

/* The weight files the wanted faces are made from. The Sans files take
 * only their mark for a missing character from the terminal bundle. */
/* 0 for a bundle whose drawings are made for another box than one cell
 * (two cells, or a glyph in two halves). Such a file says so with a card,
 * NAME.symbols.txt, beside it, where a bundle of one-cell characters has
 * NAME.list.txt or none. A glyph of a font file is one cell, so it is
 * left out. */
static int one_cell(const listing *l, const char *name)
{
    char card[MAX_PATH_LEN + 160];
    snprintf(card, sizeof card, "%s/%.*s.symbols.txt", l->dir, (int)(strlen(name) - 4), name);
    return access(card, F_OK) != 0;
}

static void open_sources(sources *src, weights *ws, const listing *l, int sans, int mono)
{
    char prefix[64];
    const char *name;
    int em, i;
    memset(src, 0, sizeof *src);
    for (em = 1; sans && em <= 16; em++) {
        snprintf(prefix, sizeof prefix, "neuronte-ui%d-v", em);
        src->ui[em] = use(ws, l, newest(l, prefix));
        snprintf(prefix, sizeof prefix, "neuronte-ui%d-italic-v", em);
        src->ui_italic[em] = use(ws, l, newest(l, prefix));
    }
    if (sans) {
        name = newest(l, "neuronte-ui-scaled-latin1-v");
        src->scaled = use(ws, l, name ? name : newest(l, "neuronte-ui-scaled-v"));
        name = newest(l, "neuronte-ui-scaled-latin1-italic-advances-v");
        src->slant = use(ws, l, name ? name : newest(l, "neuronte-ui-scaled-italic-advances-v"));
        name = newest(l, "neuronte-ui-large-latin1-v");
        src->large = use(ws, l, name ? name : newest(l, "neuronte-ui-large-v"));
    }
    /* The newest split bundle, then every term bundle by name, then every
     * kilix bundle by name: text first, a terminal's own symbols after it.
     * A character goes to the first trained on it. */
    if ((name = newest(l, "neuronte-split-v")) != NULL)
        src->term[src->terms++] = use(ws, l, name);
    for (i = 0; mono && i < l->count && src->terms < MAX_TERMS; i++)
        if (!strncmp(l->names[i], "neuronte-term-", 14) && one_cell(l, l->names[i]))
            src->term[src->terms++] = use(ws, l, l->names[i]);
    for (i = 0; mono && i < l->count && src->terms < MAX_TERMS; i++)
        if (!strncmp(l->names[i], "neuronte-kilix-", 15) && one_cell(l, l->names[i]))
            src->term[src->terms++] = use(ws, l, l->names[i]);
}

/* The files of Neuronte Sans Metric: the pixel faces of 1 to 16 px and
 * their italics, the drawn bold faces and their italics where there are
 * some (9 to 16 px), and the scaled (17 to 32 px) and large faces. Their
 * italic above 16 px is the upright face sheared, at its own advances.
 * The mark comes from the first terminal bundle, as for Neuronte Sans. */
static void open_metric(sources *m, weights *ws, const listing *l, const sources *src)
{
    char prefix[64];
    int em;
    memset(m, 0, sizeof *m);
    m->metric = 1;
    for (em = 1; em <= 16; em++) {
        snprintf(prefix, sizeof prefix, "neuronte-metric-sans%d-v", em);
        m->ui[em] = use(ws, l, newest(l, prefix));
        snprintf(prefix, sizeof prefix, "neuronte-metric-sans%d-italic-v", em);
        m->ui_italic[em] = use(ws, l, newest(l, prefix));
        snprintf(prefix, sizeof prefix, "neuronte-metric-sans%d-bold-v", em);
        m->bold_ui[em] = use(ws, l, newest(l, prefix));
        snprintf(prefix, sizeof prefix, "neuronte-metric-sans%d-bold-italic-v", em);
        m->bold_italic[em] = use(ws, l, newest(l, prefix));
    }
    m->scaled = use(ws, l, newest(l, "neuronte-metric-sans-scaled-v"));
    m->large = use(ws, l, newest(l, "neuronte-metric-sans-large-v"));
    if (src->terms) {
        m->term[0] = src->term[0];
        m->terms = 1;
    }
}

/* Is n one of the files a desktop family's own faces are made from? The
 * first terminal bundle is, for the mark. */
static int own_file(const sources *src, const neuronte *n)
{
    int em;
    if (!n)
        return 0;
    for (em = 1; em <= MAX_UI; em++)
        if (n == src->ui[em] || n == src->ui_italic[em] || n == src->bold_ui[em] || n == src->bold_italic[em])
            return 1;
    return n == src->scaled || n == src->slant || n == src->large || (src->terms && n == src->term[0]);
}

/* 1 when the handle has a table of trained characters and s is in it. A
 * file without a table answers 0 for every scalar, so ask about a value
 * no net is trained on first. */
static int has_table(const neuronte *n)
{
    return neuronte_untrained(n, 0x10FFFF) == 1;
}

static int trained(const neuronte *n, uint32_t s)
{
    return n && s <= 0x10FFFF && !(s >= 0xD800 && s <= 0xDFFF) && neuronte_untrained(n, s) == 0;
}

/* A desktop face's characters are the ones its advance table lists. A face
 * that is one net has no table of trained characters; one that is a bundle
 * has, and then both must name the character. */
static int in_face(const neuronte *n, uint32_t s)
{
    return trained(n, s) && neuronte_advances(n) > 0 && neuronte_advance_scalar(n, s) == s;
}

/* ---- strikes ---- */

/* What the desktop nets drew, kept so that the four Sans files share it:
 * each is the same letters, struck twice for bold and leant for italic. */
typedef struct {
    int nglyphs;
    uint32_t *scalar;
    float **trace;          /* [glyph]: the outline net's coverage of the trace cell */
    float **up[MAX_UI + 1]; /* [em][glyph]: the upright face of that em, em by 2 em, or NULL */
    float **it[MAX_UI + 1]; /* the same from the italic nets, 16 px and below */
    float **bup[MAX_UI + 1]; /* the drawn bold faces, where there are some */
    float **bit[MAX_UI + 1]; /* and their italics */
} sans_kept;

typedef struct {
    face *f;
    strike *st;
    const sources *src;
    const sans_kept *kept;
} strike_arg;

/* Cut the ink out of a cell and keep it as a glyph of the strike. cov is
 * the coverage; depth 1 keeps a pixel more than half covered, as the
 * library's binary letters do, and depth 8 keeps the coverage as a byte,
 * rounded as the terminal rounds it. */
static void keep_cell(sbit *g, const float *cov, int w, int h, int depth, int base, int advance)
{
    int x, y, x0 = w, y0 = h, x1 = -1, y1 = -1, bw, bh, row;
    for (y = 0; y < h; y++)
        for (x = 0; x < w; x++) {
            float v = cov[(size_t)y * w + x];
            int on = depth == 1 ? v > 0.5f : v * 255.0f + 0.5f >= 1.0f;
            if (!on)
                continue;
            if (x < x0)
                x0 = x;
            if (x > x1)
                x1 = x;
            if (y < y0)
                y0 = y;
            if (y > y1)
                y1 = y;
        }
    g->present = 1;
    g->adv = (uint8_t)(advance > 255 ? 255 : advance);
    if (x1 < 0) {
        /* No ink: a space. One clear pixel on the baseline, because a
         * bitmap of no size is read as no bitmap, and then the pen moves
         * by the outline's advance and not by this size's own. */
        g->w = g->h = 1;
        g->bx = 0;
        g->by = 1;
        g->len = 1;
        g->data = xcalloc(1, 1);
        return;
    }
    bw = x1 - x0 + 1;
    bh = y1 - y0 + 1;
    if (bw > 255 || bh > 255 || x0 > 127 || base - y0 > 127 || base - y0 < -128)
        die("a strike glyph does not fit small metrics");
    g->w = (uint8_t)bw;
    g->h = (uint8_t)bh;
    g->bx = (int8_t)x0;
    g->by = (int8_t)(base - y0);
    row = depth == 1 ? (bw + 7) / 8 : bw;
    g->len = (size_t)row * bh;
    g->data = xcalloc(g->len, 1);
    for (y = 0; y < bh; y++)
        for (x = 0; x < bw; x++) {
            float v = cov[(size_t)(y0 + y) * w + x0 + x];
            if (depth == 1) {
                if (v > 0.5f)
                    g->data[(size_t)y * row + x / 8] |= (uint8_t)(0x80 >> (x & 7));
            } else
                g->data[(size_t)y * row + x] = v <= 0.0f ? 0 : v >= 1.0f ? 255 : (uint8_t)(v * 255.0f + 0.5f);
        }
}

/* The baseline of a face at one size: the row under the lowest ink of H.
 * Without an H, the design cell's own baseline. */
static int cell_base(const neuronte *n, int w, int h, int samples)
{
    float *cov = xmalloc((size_t)w * h * sizeof *cov);
    int x, y, base = -1;
    if (trained(n, 'H') && neuronte_coverage(n, 'H', w, h, samples, cov) == NEURONTE_OK)
        for (y = h - 1; y >= 0 && base < 0; y--)
            for (x = 0; x < w; x++)
                if (cov[(size_t)y * w + x] > 0.5f) {
                    base = y + 1;
                    break;
                }
    free(cov);
    return base >= 0 ? base : (27 * h + 20) / 40;
}

/* The untrained mark as the terminal shows it, in the terminal's cell for
 * this size, standing on the cell's baseline. */
static void mark_strike(strike *st, const sources *src, int cell_w)
{
    int w = cell_w, h = 2 * w;
    float *cov;
    if (!src->terms || !has_table(src->term[0]))
        return;
    cov = xmalloc((size_t)w * h * sizeof *cov);
    /* The handle has the mark on (set in main), and U+10FFFF is trained by none. */
    if (neuronte_coverage(src->term[0], 0x10FFFF, w, h, 0, cov) == NEURONTE_OK)
        keep_cell(&st->g[0], cov, w, h, st->depth, cell_base(src->term[0], w, h, 0), w);
    free(cov);
}

static int mono_cell(int ppem)
{
    int w = (6 * ppem + 5) / 10; /* the terminal's rule: 0.6 em, to the nearest pixel */
    return w < 2 ? 2 : w;
}

/* The library's italic moves a row right by (0.675 - (row + 0.5) / h) * w / 2
 * pixels, to the nearest pixel, a tie away from zero. */
static int italic_shift(int row, int w, int h)
{
    int64_t num = (27LL * h - 40LL * row - 20) * w, den = 80LL * h;
    return num >= 0 ? (int)((num + den / 2) / den) : -(int)((-num + den / 2) / den);
}

static float at(const float *row, int w, int x)
{
    return x >= 0 && x < w ? row[x] : 0.0f;
}

static void sans_strike(void *varg, int index)
{
    strike_arg *a = (strike_arg *)varg + index;
    face *f = a->f;
    strike *st = a->st;
    const sources *src = a->src;
    const sans_kept *kept = a->kept;
    int em = st->ppem, w = em, h = 2 * em, i, base = -1, x, y;
    int sheared = em > 16 && f->italic;
    /* A drawn bold face is drawn as it is (bold 0), at its own advances;
     * where there is none, bold is the letter struck twice. */
    const neuronte *drawn = em <= 16 && f->bold ? (f->italic ? src->bold_italic[em] : src->bold_ui[em]) : NULL;
    int struck = f->bold && !drawn;
    const neuronte *adv_n = drawn ? drawn
                            : em <= 16 ? (f->italic ? src->ui_italic[em] : src->ui[em])
                                       : (f->italic && src->slant ? src->slant : src->scaled);
    float **cells = drawn ? (f->italic ? kept->bit[em] : kept->bup[em])
                    : em <= 16 && f->italic ? kept->it[em] : kept->up[em];
    /* One column more than the cell: the second strike of a bold letter may
     * land there. */
    float *cell = xmalloc((size_t)(w + 1) * h * sizeof *cell);
    const float *cap;

    /* The baseline is the row under the lowest ink of the upright H. */
    i = find_glyph(f, 'H');
    cap = i > 0 ? kept->up[em][i] : NULL;
    for (y = h - 1; cap && y >= 0 && base < 0; y--)
        for (x = 0; x < w; x++)
            if (cap[(size_t)y * w + x] > 0.5f) {
                base = y + 1;
                break;
            }
    if (base < 0)
        base = (27 * h + 20) / 40;
    for (i = 1; i < f->nglyphs; i++) {
        uint32_t s = f->g[i].scalar;
        const float *cov = cells[i];
        if (!cov || neuronte_advance_scalar(adv_n, s) != s)
            continue;
        for (y = 0; y < h; y++) {
            const float *row = cov + (size_t)y * w;
            int shift = sheared ? italic_shift(y, w, h) : 0;
            for (x = 0; x <= w; x++) {
                float v = at(row, w, x - shift);
                /* Bold: the letter struck twice, the second time one pixel
                 * to the right, each pixel the greater of the two. */
                if (struck && at(row, w, x - shift - 1) > v)
                    v = at(row, w, x - shift - 1);
                /* The sheared italic is the library's styled cell, which
                 * keeps its ink inside the cell. */
                cell[(size_t)y * (w + 1) + x] = sheared && x == w ? 0.0f : v;
            }
        }
        keep_cell(&st->g[i], cell, w + 1, h, 1, base, neuronte_advance(adv_n, s, struck, em));
        /* A version 8 table moves the drawing right of the pen, as the
         * library's draw calls do. */
        if (src->metric) {
            int px = 0;
            neuronte_advance_shift(adv_n, s, struck, em, NULL, &px);
            if (st->g[i].bx + px > 127 || st->g[i].bx + px < -128)
                die("a shifted strike glyph does not fit small metrics");
            st->g[i].bx = (int8_t)(st->g[i].bx + px);
        }
    }
    mark_strike(st, src, mono_cell(em));
    free(cell);
}

typedef struct {
    face *f;
    strike *st;
    int glyph;
} mono_job;

static void mono_strike(void *varg, int index)
{
    mono_job *j = (mono_job *)varg + index;
    face *f = j->f;
    strike *st = j->st;
    int w = mono_cell(st->ppem), h = 2 * w, i = j->glyph;
    const neuronte *n = f->g[i].src;
    float *cov = xmalloc((size_t)w * h * sizeof *cov);
    /* Samples 0, style none: the call the terminal makes for a cell. */
    if (neuronte_coverage(n, i ? f->g[i].scalar : 0x10FFFF, w, h, 0, cov) == NEURONTE_OK)
        keep_cell(&st->g[i], cov, w, h, 8, (int)floor(f->base / (2.0 * f->cell_w) * h + 0.5), w);
    free(cov);
}

/* ---- the faces ---- */

typedef struct {
    int threads, trace_px, strikes, outlines, cover;
    double tol;
    const char *mono_sizes, *mono_chars;
} options;

/* The digest a file carries in its version string: of the weight files its
 * own face is made from, so that a file is the same bytes whether it was
 * made alone or with the others. The Sans files use every desktop net and
 * the first terminal bundle (for the mark); Mono uses the bundles. */
static void face_digest(face *f, const weights *ws, const sources *src)
{
    sha256 s;
    int i, k;
    sha_init(&s);
    for (i = 0; i < ws->count; i++) {
        int bundle = -1;
        for (k = 0; k < src->terms; k++)
            if (src->term[k] == ws->w[i].n)
                bundle = k;
        /* A desktop family's digest is of its own files only, so a file of
         * the other family beside them changes no byte of it. */
        if (f->mono ? bundle >= 0 : own_file(src, ws->w[i].n))
            sha_add(&s, ws->w[i].sha, 64);
    }
    sha_hex(&s, f->source);
}

static void finish_outlines(face *f, const sources *src, const options *opt, float **kept, int stroke, int shear)
{
    trace_arg ta;
    int i, w = f->cell_w, h = 2 * w, hi = 0, lo = 0;
    double mark_base;

    ta.f = f;
    ta.kept = kept;
    ta.stroke = stroke;
    ta.tol = opt->tol;
    ta.mark = src->terms && has_table(src->term[0]) ? src->term[0] : NULL;
    ta.mark_scalar = 0x10FFFF;
    ta.mark_w = opt->trace_px;
    run_jobs(trace_one, &ta, f->nglyphs, opt->threads);

    /* The baseline is under the lowest ink of H, as the net draws it. */
    i = find_glyph(f, 'H');
    f->base = i > 0 && f->g[i].traced.n ? loops_bottom(&f->g[i].traced) : 0.675 * h;
    f->capheight = i > 0 && f->g[i].traced.n ? (int)floor((f->base - loops_top(&f->g[i].traced)) * f->unit + 0.5) : 0;
    i = find_glyph(f, 'x');
    f->xheight = i > 0 && f->g[i].traced.n ? (int)floor((f->base - loops_top(&f->g[i].traced)) * f->unit + 0.5) : 0;

    /* Glyph 0: the mark in a terminal cell, 0.6 em wide, on that cell's baseline. */
    if (f->mono)
        mark_base = f->base * ta.mark_w / w;
    else {
        mark_base = 0.675 * 2 * ta.mark_w;
        if (ta.mark && trained(ta.mark, 'H')) {
            float *cov = xmalloc((size_t)ta.mark_w * 2 * ta.mark_w * sizeof *cov);
            loops ls;
            if (neuronte_coverage(ta.mark, 'H', ta.mark_w, 2 * ta.mark_w, 1, cov) == NEURONTE_OK) {
                trace(cov, ta.mark_w, 2 * ta.mark_w, opt->tol, &ls);
                if (ls.n)
                    mark_base = loops_bottom(&ls);
                loops_free(&ls);
            }
            free(cov);
        }
    }
    to_units(&f->g[0], 0.6 * UPM / ta.mark_w, mark_base, 0, ta.mark_w, 2 * ta.mark_w);
    f->g[0].advance = UPM * 6 / 10;
    for (i = 1; i < f->nglyphs; i++)
        to_units(&f->g[i], f->unit, f->base, shear, w, h);

    if (f->mono) {
        /* The line is the cell: nothing above its top or below its bottom. */
        f->ascender = (int)floor(f->base * f->unit + 0.5);
        f->descender = f->ascender - (int)floor(h * f->unit + 0.5);
    } else {
        /* The highest and the lowest ink of any character. */
        for (i = 0; i < f->nglyphs; i++) {
            if (f->g[i].ncont && f->g[i].ymax > hi)
                hi = f->g[i].ymax;
            if (f->g[i].ncont && f->g[i].ymin < lo)
                lo = f->g[i].ymin;
        }
        f->ascender = hi;
        f->descender = lo;
    }
}

typedef struct {
    const neuronte *n;
    uint32_t scalar;
    int w, samples;
    float **slot;
} keep_job;

static void keep_one(void *varg, int index)
{
    keep_job *j = (keep_job *)varg + index;
    float *cov = xmalloc((size_t)j->w * 2 * j->w * sizeof *cov);
    if (neuronte_coverage(j->n, j->scalar, j->w, 2 * j->w, j->samples, cov) != NEURONTE_OK)
        die("U+%04X: the net did not draw", (unsigned)j->scalar);
    *j->slot = cov;
}

static const neuronte *sans_outline(const sources *src)
{
    return src->large ? src->large : src->scaled;
}

/* Run each desktop net once over its characters. */
static void keep_sans(sans_kept *kept, const sources *src, const options *opt, int italics)
{
    const neuronte *outline = sans_outline(src);
    keep_job *jobs_list;
    uint32_t s;
    int n = 1, i, em, njobs = 0;

    if (!outline || !neuronte_advances(outline))
        die("the weights hold no scalable desktop face (neuronte-ui-large-*.nrt or neuronte-ui-scaled-*.nrt)");
    kept->scalar = xcalloc(4097, sizeof *kept->scalar);
    for (s = 0; s <= 0x10FFFF && n < 4097; s++)
        if (in_face(outline, s))
            kept->scalar[n++] = s; /* in rising order; glyph 0 is the mark */
    kept->nglyphs = n;
    jobs_list = xmalloc((size_t)n * (4 * MAX_UI + 2) * sizeof *jobs_list);
    kept->trace = xcalloc((size_t)n, sizeof *kept->trace);
    for (i = 1; i < n; i++) {
        keep_job j = {outline, kept->scalar[i], opt->trace_px, 1, &kept->trace[i]};
        jobs_list[njobs++] = j;
    }
    for (em = 1; em <= MAX_UI; em++) {
        const neuronte *up = em <= 16 ? src->ui[em] : src->scaled;
        const neuronte *it = em <= 16 ? src->ui_italic[em] : NULL;
        int samples = em < 24 ? 2 : 1; /* what the draw calls take when left to choose */
        kept->up[em] = xcalloc((size_t)n, sizeof *kept->up[em]);
        kept->it[em] = xcalloc((size_t)n, sizeof *kept->it[em]);
        kept->bup[em] = xcalloc((size_t)n, sizeof *kept->bup[em]);
        kept->bit[em] = xcalloc((size_t)n, sizeof *kept->bit[em]);
        if (!opt->strikes || !up || !neuronte_advances(up))
            continue;
        for (i = 1; i < n; i++) {
            const neuronte *bu = em <= 16 ? src->bold_ui[em] : NULL, *bi = em <= 16 ? src->bold_italic[em] : NULL;
            if (in_face(up, kept->scalar[i])) {
                keep_job j = {up, kept->scalar[i], em, samples, &kept->up[em][i]};
                jobs_list[njobs++] = j;
            }
            if (italics && it && in_face(it, kept->scalar[i])) {
                keep_job j = {it, kept->scalar[i], em, samples, &kept->it[em][i]};
                jobs_list[njobs++] = j;
            }
            if (bu && in_face(bu, kept->scalar[i])) {
                keep_job j = {bu, kept->scalar[i], em, samples, &kept->bup[em][i]};
                jobs_list[njobs++] = j;
            }
            if (italics && bi && in_face(bi, kept->scalar[i])) {
                keep_job j = {bi, kept->scalar[i], em, samples, &kept->bit[em][i]};
                jobs_list[njobs++] = j;
            }
        }
    }
    run_jobs(keep_one, jobs_list, njobs, opt->threads);
    free(jobs_list);
}

static void build_sans(face *f, const sans_kept *kept, const sources *src, const weights *ws,
                       const options *opt)
{
    const neuronte *outline = sans_outline(src);
    int i, em, nargs = 0;
    strike_arg args[MAX_UI];

    f->nglyphs = kept->nglyphs;
    f->g = xcalloc((size_t)f->nglyphs, sizeof *f->g);
    f->cell_w = opt->trace_px;
    f->unit = (double)UPM / f->cell_w;
    for (i = 1; i < f->nglyphs; i++) {
        glyph *g = &f->g[i];
        uint32_t s = kept->scalar[i];
        const neuronte *adv = f->italic && src->slant && neuronte_advance_scalar(src->slant, s) == s ? src->slant
                                                                                                    : outline;
        g->scalar = s;
        g->src = outline;
        if (src->metric) {
            /* Thousandths of an em, the weight's own column (Helvetica-Bold's
             * widths for bold), and the table's shift of the drawing in the
             * left side bearing. The pen never moves by the shift. */
            int shift = 0;
            g->advance = neuronte_advance_milli(adv, s, f->bold) * UPM / 1000;
            neuronte_advance_shift(adv, s, f->bold, 1, &shift, NULL);
            g->dx = shift * UPM / 1000;
        } else
            /* The scalable bold is the library's: ink a 32nd of an em wider,
             * and the pen that much further. */
            g->advance = neuronte_advance_milli(adv, s, 0) * UPM / 1000 + (f->bold ? UPM / 32 : 0);
    }
    face_digest(f, ws, src);
    i = (f->cell_w + 16) / 32;
    finish_outlines(f, src, opt, kept->trace, f->bold ? (i ? i : 1) : 0, f->italic);

    if (!opt->strikes)
        return;
    for (em = 1; em <= MAX_UI; em++) {
        strike *st;
        if (!src->ui[em] && em <= 16)
            continue;
        if (em > 16 && (!src->scaled || !neuronte_advances(src->scaled) || (f->italic && !src->slant && !src->metric)))
            continue;
        if (em <= 16 && (!neuronte_advances(src->ui[em]) ||
                         (f->italic && (!src->ui_italic[em] || !neuronte_advances(src->ui_italic[em])))))
            continue;
        st = &f->s[f->nstrikes++];
        st->ppem = em;
        st->depth = 1;
        st->g = xcalloc((size_t)f->nglyphs, sizeof *st->g);
        args[nargs].f = f;
        args[nargs].st = st;
        args[nargs].src = src;
        args[nargs].kept = kept;
        nargs++;
    }
    run_jobs(sans_strike, args, nargs, opt->threads);
}

static int in_list(const char *list, long v)
{
    const char *p = list;
    while (p && *p) {
        char *end;
        long lo = strtol(p, &end, 0), hi = lo;
        if (end == p)
            break;
        if (*end == '-')
            hi = strtol(end + 1, &end, 0);
        if (v >= lo && v <= hi)
            return 1;
        p = *end == ',' ? end + 1 : end;
        if (*end && *end != ',')
            break;
    }
    return 0;
}

static void build_mono(face *f, const sources *src, const weights *ws, const options *opt)
{
    uint32_t s;
    int n = 0, cap = 1024, i, k, ppem, njobs = 0;
    struct {
        uint32_t scalar;
        const neuronte *src;
    } *found = xmalloc((size_t)cap * sizeof *found);
    mono_job *jobs_list;

    if (!src->terms)
        die("the weights hold no terminal bundle (neuronte-split-vN.nrt)");
    for (k = 0; k < src->terms; k++)
        if (!has_table(src->term[k]))
            die("terminal bundle %d has no table of trained characters", k);
    for (s = 0; s <= 0x10FFFF; s++) {
        if (s >= 0xD800 && s <= 0xDFFF)
            continue;
        for (k = 0; k < src->terms; k++)
            if (neuronte_untrained(src->term[k], s) == 0)
                break;
        if (k == src->terms)
            continue;
        if (n == cap) {
            cap *= 2;
            found = xrealloc(found, (size_t)cap * sizeof *found);
        }
        found[n].scalar = s;
        found[n].src = src->term[k];
        n++;
    }
    if (n + 1 > 65535)
        die("more characters than one font file holds");
    f->nglyphs = n + 1;
    f->g = xcalloc((size_t)f->nglyphs, sizeof *f->g);
    f->cell_w = opt->trace_px;
    f->unit = 0.6 * UPM / f->cell_w; /* the cell is 0.6 em wide and 1.2 em high */
    f->g[0].src = src->term[0];
    for (i = 0; i < n; i++) {
        f->g[i + 1].scalar = found[i].scalar;
        f->g[i + 1].src = found[i].src;
        f->g[i + 1].advance = UPM * 6 / 10;
    }
    free(found);
    face_digest(f, ws, src);
    finish_outlines(f, src, opt, NULL, 0, 0);

    if (!opt->strikes)
        return;
    jobs_list = xmalloc((size_t)f->nglyphs * MAX_STRIKES * sizeof *jobs_list);
    for (ppem = 4; ppem <= 64 && f->nstrikes < MAX_STRIKES; ppem++) {
        strike *st;
        if (!in_list(opt->mono_sizes, ppem))
            continue;
        st = &f->s[f->nstrikes++];
        st->ppem = ppem;
        st->depth = 8;
        st->g = xcalloc((size_t)f->nglyphs, sizeof *st->g);
        for (i = 0; i < f->nglyphs; i++)
            if (i == 0 || in_list(opt->mono_chars, (long)f->g[i].scalar)) {
                jobs_list[njobs].f = f;
                jobs_list[njobs].st = st;
                jobs_list[njobs].glyph = i;
                njobs++;
            }
    }
    run_jobs(mono_strike, jobs_list, njobs, opt->threads);
    free(jobs_list);
}

/* GTK 4, left to itself, measures a glyph by its outline at the size of
 * the em and cuts the glyph's picture to that box, also when the picture
 * comes from a strike. A small size's own drawing does not sit where the
 * scaled outline does, so letters lost columns there. Give each outline two
 * more contours of one point each, at the corners of a box that also holds
 * every strike's bitmap from 6 px up with a pixel to spare. A contour of
 * one point encloses nothing and draws nothing; it only widens the box. */
static void cover_strikes(face *f)
{
    int i, s;
    for (i = 0; i < f->nglyphs; i++) {
        glyph *g = &f->g[i];
        int x0 = g->xmin, x1 = g->xmax, y0 = g->ymin, y1 = g->ymax;
        if (!g->ncont)
            continue;
        for (s = 0; s < f->nstrikes; s++) {
            const strike *st = &f->s[s];
            const sbit *b = &st->g[i];
            double px = (double)UPM / st->ppem;
            if (st->ppem < 6 || !b->present)
                continue;
            if (floor((b->bx - 1) * px) < x0)
                x0 = (int)floor((b->bx - 1) * px);
            if (ceil((b->bx + b->w + 1) * px) > x1)
                x1 = (int)ceil((b->bx + b->w + 1) * px);
            if (ceil((b->by + 1) * px) > y1)
                y1 = (int)ceil((b->by + 1) * px);
            if (floor((b->by - b->h - 1) * px) < y0)
                y0 = (int)floor((b->by - b->h - 1) * px);
        }
        if (x0 == g->xmin && x1 == g->xmax && y0 == g->ymin && y1 == g->ymax)
            continue;
        g->pts = xrealloc(g->pts, (size_t)(g->npts + 2) * sizeof *g->pts);
        g->end = xrealloc(g->end, (size_t)(g->ncont + 2) * sizeof *g->end);
        g->pts[g->npts].x = (int16_t)x0, g->pts[g->npts].y = (int16_t)y0;
        g->end[g->ncont++] = g->npts++;
        g->pts[g->npts].x = (int16_t)x1, g->pts[g->npts].y = (int16_t)y1;
        g->end[g->ncont++] = g->npts++;
        g->xmin = x0, g->xmax = x1, g->ymin = y0, g->ymax = y1;
        g->lsb = g->xmin;
    }
}

/* ---- files ---- */

static int write_whole(const char *dir, const char *name, const unsigned char *data, size_t len)
{
    char tmp[MAX_PATH_LEN + 200], path[MAX_PATH_LEN + 200];
    int fd;
    size_t done = 0;
    snprintf(path, sizeof path, "%s/%s", dir, name);
    snprintf(tmp, sizeof tmp, "%s/.%s.%ld.tmp", dir, name, (long)getpid());
    fd = open(tmp, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0)
        return -1;
    while (done < len) {
        ssize_t k = write(fd, data + done, len - done);
        if (k < 0) {
            if (errno == EINTR)
                continue;
            close(fd);
            unlink(tmp);
            return -1;
        }
        done += (size_t)k;
    }
    if (fchmod(fd, 0644) || fsync(fd) || close(fd) || rename(tmp, path)) {
        unlink(tmp);
        return -1;
    }
    return 0;
}

static void sync_dir(const char *dir)
{
    int fd = open(dir, O_RDONLY | O_DIRECTORY);
    if (fd >= 0) {
        fsync(fd);
        close(fd);
    }
}

static int mkdirs(const char *dir)
{
    char path[MAX_PATH_LEN];
    size_t i, len = strlen(dir);
    if (len >= sizeof path)
        return -1;
    memcpy(path, dir, len + 1);
    for (i = 1; i <= len; i++)
        if (path[i] == '/' || path[i] == 0) {
            char c = path[i];
            path[i] = 0;
            /* Made for every user to read, whatever the caller's umask. */
            if (mkdir(path, 0755) == 0)
                chmod(path, 0755);
            else if (errno != EEXIST)
                return -1;
            path[i] = c;
        }
    return 0;
}

/* A run that was killed leaves the file it was writing, under its
 * temporary name. Fontconfig does not read names that start with a dot,
 * but they should not pile up. Runs are kept apart by neuronte-setup. */
static void sweep(const char *dir)
{
    DIR *d = opendir(dir);
    struct dirent *e;
    if (!d)
        return;
    while ((e = readdir(d)) != NULL) {
        size_t len = strlen(e->d_name);
        char path[2 * MAX_PATH_LEN];
        if (len < 14 || strncmp(e->d_name, ".Neuronte", 9) || strcmp(e->d_name + len - 4, ".tmp"))
            continue;
        if ((size_t)snprintf(path, sizeof path, "%s/%s", dir, e->d_name) < sizeof path)
            unlink(path);
    }
    closedir(d);
}

static unsigned char *read_whole(const char *path, size_t *len)
{
    FILE *f = fopen(path, "rb");
    buf b = {0};
    size_t n;
    unsigned char chunk[4096];
    if (!f)
        return NULL;
    while ((n = fread(chunk, 1, sizeof chunk, f)) > 0)
        pbytes(&b, chunk, n);
    fclose(f);
    p8(&b, 0);
    *len = b.n - 1;
    return b.p;
}

/* fc-cache writes the directory's cache and then sleeps two seconds, so
 * that later changes carry a later time. With wait 0 it is started and
 * left to finish by itself: a program that looks before the cache is
 * written reads the directory itself, which is correct and a little slower. */
static void fc_cache(const char *dir, int wait)
{
    char *argv[] = {"fc-cache", (char *)dir, NULL};
    pid_t pid;
    int status;
    if (posix_spawnp(&pid, "fc-cache", NULL, NULL, argv, environ) != 0) {
        fprintf(stderr, "neuronte-fontfile: fc-cache was not run: fontconfig will scan %s itself\n", dir);
        return;
    }
    while (wait && waitpid(pid, &status, 0) < 0 && errno == EINTR)
        ;
}

/* DIR and NAME of a path, for a file written beside its final name. */
static void split_path(const char *path, char dir[MAX_PATH_LEN], const char **name)
{
    const char *slash = strrchr(path, '/');
    if (!slash) {
        memcpy(dir, ".", 2);
        *name = path;
    } else {
        size_t n = slash == path ? 1 : (size_t)(slash - path);
        memcpy(dir, path, n);
        dir[n] = 0;
        *name = slash + 1;
    }
}

static void stamp_line(buf *b, const char *kind, const char *sha, const filetime *t, const char *name)
{
    pprintf(b, "%s %s %lld %lld.%09ld %s\n", kind, sha, t->size, t->sec, t->nsec, name);
}

/* Is everything the stamp names as it was? head is what the stamp must
 * start with (this generator and its options), plan the weight files a run
 * would use now. A file is read only when its time is not the stamp's, or
 * always with verify. fresh receives the stamp with the times of now, and
 * *touched how many times differed over the same bytes. */
static int is_current(const char *stamp_path, const buf *head, const weights *plan, const char *wdir,
                      const char *odir, int verify, buf *fresh, int *touched, int *outputs)
{
    size_t len;
    unsigned char *old = read_whole(stamp_path, &len);
    const char *p;
    int wi = 0, ok = 1;
    *touched = *outputs = 0;
    if (!old)
        return 0;
    if (len < head->n || memcmp(old, head->p, head->n)) {
        free(old);
        return 0;
    }
    pbytes(fresh, head->p, head->n);
    p = (const char *)old + head->n;
    while (ok && *p) {
        char kind[8], sha[65], name[128], path[2 * MAX_PATH_LEN], got[65];
        filetime was, is;
        int used = 0, weight_line;
        if (sscanf(p, "%7s %64s %lld %lld.%ld %127s\n%n", kind, sha, &was.size, &was.sec, &was.nsec, name, &used) != 6 ||
            !used || strchr(name, '/')) {
            ok = 0;
            break;
        }
        p += used;
        weight_line = !strcmp(kind, "weight");
        if (weight_line) {
            /* The same files in the same order: a new bundle in the
             * directory, or a newer version of one, is a change. */
            if (*outputs || wi >= plan->count || strcmp(plan->w[wi].name, name))
                ok = 0;
            wi++;
        } else if (!strcmp(kind, "output"))
            ++*outputs;
        else
            ok = 0;
        if (!ok)
            break;
        snprintf(path, sizeof path, "%s/%s", weight_line ? wdir : odir, name);
        if (file_time(path, &is) || is.size != was.size) {
            ok = 0;
            break;
        }
        if (verify || is.sec != was.sec || is.nsec != was.nsec) {
            if (file_sha(path, got) || strcmp(got, sha))
                ok = 0;
            else if (is.sec != was.sec || is.nsec != was.nsec)
                ++*touched;
        }
        stamp_line(fresh, kind, sha, &is, name);
    }
    free(old);
    return ok && wi == plan->count && *outputs > 0;
}

/* Is KEY a whole word of the comma list? "bold" is not found in "bolditalic". */
static int listed(const char *list, const char *key)
{
    size_t n = strlen(key);
    const char *at = strstr(list, key);
    while (at && !((at == list || at[-1] == ',') && (at[n] == 0 || at[n] == ',')))
        at = strstr(at + 1, key);
    return at != NULL;
}

static void usage(void)
{
    fputs("usage: neuronte-fontfile [--weights DIR] [--out DIR] [--stamp FILE]\n"
          "           [--check] [--verify] [--force] [--threads N] [--faces LIST]\n"
          "           [--no-strikes] [--no-outlines] [--mono-sizes LIST]\n"
          "           [--mono-chars LIST] [--trace PIXELS] [--tolerance PIXELS]\n"
          "           [--no-fc-cache] [--fc-cache-nowait] [--verbose]\n"
          "Writes NeuronteSans-*.ttf, NeuronteMono-Regular.ttf and NeuronteSansMetric-*.ttf\n"
          "into the --out directory (default .) from the weight files in --weights\n"
          "(default weights), unless they are already there and made from the same weights.\n"
          "--faces takes regular,bold,italic,bolditalic,mono and metric-regular,\n"
          "metric-bold,metric-italic,metric-bolditalic (default all; the metric faces\n"
          "only when the weights hold them).\n"
          "--stamp names the record of what the files were made from (default\n"
          "neuronte-fontfile.stamp in the --out directory).\n"
          "--check writes nothing: exit 0 when a run would write nothing, 1 otherwise.\n"
          "--verify hashes every file, not only those whose time changed.\n"
          "--fc-cache-nowait starts fc-cache and does not wait for it (it sleeps\n"
          "two seconds after writing a cache).\n",
          stderr);
    exit(2);
}

int main(int argc, char **argv)
{
    static listing l;
    static weights ws, plan;
    static sources src, msrc;
    static sans_kept kept, mkept;
    static const struct {
        const char *key, *file, *family, *style, *psname;
        int bold, italic, mono, metric;
    } all[] = {{"regular", "NeuronteSans-Regular.ttf", "Neuronte Sans", "Regular", "NeuronteSans-Regular", 0, 0, 0, 0},
               {"bold", "NeuronteSans-Bold.ttf", "Neuronte Sans", "Bold", "NeuronteSans-Bold", 1, 0, 0, 0},
               {"italic", "NeuronteSans-Italic.ttf", "Neuronte Sans", "Italic", "NeuronteSans-Italic", 0, 1, 0, 0},
               {"bolditalic", "NeuronteSans-BoldItalic.ttf", "Neuronte Sans", "Bold Italic",
                "NeuronteSans-BoldItalic", 1, 1, 0, 0},
               {"mono", "NeuronteMono-Regular.ttf", "Neuronte Mono", "Regular", "NeuronteMono-Regular", 0, 0, 1, 0},
               {"metric-regular", "NeuronteSansMetric-Regular.ttf", "Neuronte Sans Metric", "Regular",
                "NeuronteSansMetric-Regular", 0, 0, 0, 1},
               {"metric-bold", "NeuronteSansMetric-Bold.ttf", "Neuronte Sans Metric", "Bold",
                "NeuronteSansMetric-Bold", 1, 0, 0, 1},
               {"metric-italic", "NeuronteSansMetric-Italic.ttf", "Neuronte Sans Metric", "Italic",
                "NeuronteSansMetric-Italic", 0, 1, 0, 1},
               {"metric-bolditalic", "NeuronteSansMetric-BoldItalic.ttf", "Neuronte Sans Metric", "Bold Italic",
                "NeuronteSansMetric-BoldItalic", 1, 1, 0, 1}};
    /* Without --faces: every face whose files are there. Neuronte Sans
     * Metric is left out of a weights directory that has none of its
     * files, and the options are then the ones a run before it recorded. */
    const char *wdir = "weights", *odir = ".", *faces = NULL;
    const char *all_faces = "regular,bold,italic,bolditalic,mono,"
                            "metric-regular,metric-bold,metric-italic,metric-bolditalic";
    const char *stamp_arg = NULL, *stamp_name;
    options opt = {2, 64, 1, 1, 1, 0.1, "9,12,13,15,16,17,20", "0x20-0x7E,0xA0-0xFF"};
    int check = 0, force = 0, verify = 0, cache = 1, i, k, wrote = 0, sans = 0, mono = 0, metric = 0;
    int touched, outputs;
    buf stamp = {0}, fresh = {0};
    static char stamp_path[2 * MAX_PATH_LEN], stamp_dir[MAX_PATH_LEN];
    double t0 = now();

    for (i = 1; i < argc; i++) {
        const char *a = argv[i], *v = i + 1 < argc ? argv[i + 1] : NULL;
        if (!strcmp(a, "--check"))
            check = 1;
        else if (!strcmp(a, "--force"))
            force = 1;
        else if (!strcmp(a, "--verify"))
            verify = 1;
        else if (!strcmp(a, "--no-fc-cache"))
            cache = 0;
        else if (!strcmp(a, "--fc-cache-nowait"))
            cache = 2;
        else if (!strcmp(a, "--no-strikes"))
            opt.strikes = 0;
        else if (!strcmp(a, "--no-outlines"))
            opt.outlines = 0;
        else if (!strcmp(a, "--tight-boxes"))
            opt.cover = 0;
        else if (!strcmp(a, "--verbose"))
            verbose = 1;
        else if (!v)
            usage();
        else if (!strcmp(a, "--weights"))
            wdir = v, i++;
        else if (!strcmp(a, "--out"))
            odir = v, i++;
        else if (!strcmp(a, "--stamp"))
            stamp_arg = v, i++;
        else if (!strcmp(a, "--faces"))
            faces = v, i++;
        else if (!strcmp(a, "--threads"))
            opt.threads = atoi(v), i++;
        else if (!strcmp(a, "--trace"))
            opt.trace_px = atoi(v), i++;
        else if (!strcmp(a, "--tolerance"))
            opt.tol = atof(v), i++;
        else if (!strcmp(a, "--mono-sizes"))
            opt.mono_sizes = v, i++;
        else if (!strcmp(a, "--mono-chars"))
            opt.mono_chars = v, i++;
        else
            usage();
    }
    if (opt.threads < 1 || opt.trace_px < 16 || opt.trace_px > 512 || !(opt.tol >= 0.0) ||
        strlen(wdir) >= MAX_PATH_LEN || strlen(odir) >= MAX_PATH_LEN ||
        (stamp_arg && strlen(stamp_arg) >= MAX_PATH_LEN) || (faces && strpbrk(faces, " \t\n")) ||
        strpbrk(opt.mono_sizes, " \t\n") || strpbrk(opt.mono_chars, " \t\n"))
        usage();
    list_dir(&l, wdir);
    if (!faces)
        faces = newest(&l, "neuronte-metric-sans-large-v") || newest(&l, "neuronte-metric-sans-scaled-v")
                    ? all_faces : "regular,bold,italic,bolditalic,mono";
    for (k = 0; k < (int)(sizeof all / sizeof all[0]); k++)
        if (listed(faces, all[k].key))
            *(all[k].mono ? &mono : all[k].metric ? &metric : &sans) = 1;
    if (!sans && !mono && !metric)
        die("--faces names no face");
    if (stamp_arg)
        snprintf(stamp_path, sizeof stamp_path, "%s", stamp_arg);
    else
        snprintf(stamp_path, sizeof stamp_path, "%s/neuronte-fontfile.stamp", odir);
    split_path(stamp_path, stamp_dir, &stamp_name);

    /* What the files are made from: this generator, the options that change
     * bytes, and every weight file that is used. */
    pprintf(&stamp, "neuronte-fontfile %d\n", GENERATOR);
    pprintf(&stamp, "options faces=%s trace=%d tolerance=%g strikes=%d outlines=%d boxes=%d mono-sizes=%s mono-chars=%s\n",
            faces, opt.trace_px, opt.tol, opt.strikes, opt.outlines, opt.cover, opt.mono_sizes, opt.mono_chars);

    /* Current already? Answered from names, sizes and times: no net is
     * opened and, unless a time changed, no file is read. */
    plan.names_only = 1;
    open_sources(&src, &plan, &l, sans, mono);
    if (metric)
        open_metric(&msrc, &plan, &l, &src);
    if (!force && is_current(stamp_path, &stamp, &plan, wdir, odir, verify, &fresh, &touched, &outputs)) {
        /* The same bytes under a new time (a copy, a restored backup):
         * note the times so that the next run need not read the files. */
        if (touched && !check && write_whole(stamp_dir, stamp_name, fresh.p, fresh.n))
            fprintf(stderr, "neuronte-fontfile: %s: %s\n", stamp_path, strerror(errno));
        if (verbose)
            fprintf(stderr, "neuronte-fontfile: %s is current (%d files, %.4f s)\n", odir, outputs, now() - t0);
        return 0;
    }
    if (check) {
        if (verbose)
            fprintf(stderr, "neuronte-fontfile: %s is not current\n", odir);
        return 1;
    }

    open_sources(&src, &ws, &l, sans, mono);
    if (metric)
        open_metric(&msrc, &ws, &l, &src);
    /* The mark for what no bundle was trained on comes from the first one. */
    if (src.terms)
        neuronte_set_untrained(src.term[0], 1);
    for (i = 0; i < ws.count; i++)
        stamp_line(&stamp, "weight", ws.w[i].sha, &ws.w[i].t, ws.w[i].name);

    if (mkdirs(odir) || mkdirs(stamp_dir))
        die("%s: %s", odir, strerror(errno));
    sweep(odir);
    for (k = 0; k < (int)(sizeof all / sizeof all[0]); k++) {
        face f;
        buf file = {0};
        sha256 s;
        char sha[65], path[2 * MAX_PATH_LEN];
        filetime t;
        double t1 = now();
        int points = 0;
        if (!listed(faces, all[k].key))
            continue;
        memset(&f, 0, sizeof f);
        f.file = all[k].file;
        f.family = all[k].family;
        f.style = all[k].style;
        f.psname = all[k].psname;
        f.bold = all[k].bold;
        f.italic = all[k].italic;
        f.mono = all[k].mono;
        if (f.mono)
            build_mono(&f, &src, &ws, &opt);
        else if (all[k].metric) {
            if (!msrc.large && !msrc.scaled)
                die("the weights hold no scalable Metric face (neuronte-metric-sans-large-*.nrt or "
                    "neuronte-metric-sans-scaled-*.nrt)");
            if (!mkept.nglyphs) {
                keep_sans(&mkept, &msrc, &opt, listed(faces, "metric-italic") || listed(faces, "metric-bolditalic"));
                if (verbose)
                    fprintf(stderr, "neuronte-fontfile: the Metric nets drew in %.2f s\n", now() - t1);
            }
            build_sans(&f, &mkept, &msrc, &ws, &opt);
        } else {
            if (!kept.nglyphs) {
                keep_sans(&kept, &src, &opt, listed(faces, "italic") || listed(faces, "bolditalic"));
                if (verbose)
                    fprintf(stderr, "neuronte-fontfile: the desktop nets drew in %.2f s\n", now() - t1);
            }
            build_sans(&f, &kept, &src, &ws, &opt);
        }
        if (opt.cover)
            cover_strikes(&f);
        build_sfnt(&f, opt.outlines, &file);
        if (write_whole(odir, f.file, file.p, file.n))
            die("%s/%s: %s", odir, f.file, strerror(errno));
        sha_init(&s);
        sha_add(&s, file.p, file.n);
        sha_hex(&s, sha);
        snprintf(path, sizeof path, "%s/%s", odir, f.file);
        if (file_time(path, &t))
            die("%s: %s", path, strerror(errno));
        stamp_line(&stamp, "output", sha, &t, f.file);
        for (i = 0; i < f.nglyphs; i++)
            points += f.g[i].npts;
        if (verbose)
            fprintf(stderr,
                    "neuronte-fontfile: %s: %d glyphs, %d points, %d strikes, %zu bytes, ascent %d descent %d, %.2f s\n",
                    f.file, f.nglyphs, points, f.nstrikes, file.n, f.ascender, f.descender, now() - t1);
        wrote++;
        free(file.p);
    }
    if (write_whole(stamp_dir, stamp_name, stamp.p, stamp.n))
        die("%s: %s", stamp_path, strerror(errno));
    sync_dir(odir);
    sync_dir(stamp_dir);
    if (cache)
        fc_cache(odir, cache == 1);
    if (verbose)
        fprintf(stderr, "neuronte-fontfile: wrote %d files in %.2f s\n", wrote, now() - t0);
    return 0;
}
