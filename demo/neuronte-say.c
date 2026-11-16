/* neuronte-say: draw a line of text with a Neuronte desktop face and show
 * it in the terminal, two pixel rows to a line of half blocks.
 *
 *   neuronte-say [-s SIZE] [-b] [-i] [-o OUT.pgm] [-d WEIGHTS_DIR] TEXT...
 *
 * SIZE is the em in pixels, 1 to 128 (default 16). Sizes to 32 use that
 * size's own net and draw black and white; larger sizes use the scalable
 * net and draw with grey edges, which the terminal shows cut at half.
 * -b is bold, -i italic (3 to 32). -o also writes the pixels as a PGM
 * image. The faces hold printable ASCII and the ellipsis. MIT licence. */
#include "neuronte.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int ink(const uint32_t *px, int w, int h, int x, int y)
{
    if (y >= h)
        return 0;
    return (px[(size_t)y * w + x] & 0xFF) < 128;
}

int main(int argc, char **argv)
{
    const char *dir = "weights", *out = NULL;
    char path[1024], text[4096];
    int em = 16, bold = 0, italic = 0, i, w, h, x, y, top, bottom, rc;
    size_t len = 0;
    uint32_t *px;
    neuronte *face;

    for (i = 1; i < argc && argv[i][0] == '-' && argv[i][1]; i++) {
        if (!strcmp(argv[i], "-b"))
            bold = 1;
        else if (!strcmp(argv[i], "-i"))
            italic = 1;
        else if (!strcmp(argv[i], "-s") && i + 1 < argc)
            em = atoi(argv[++i]);
        else if (!strcmp(argv[i], "-o") && i + 1 < argc)
            out = argv[++i];
        else if (!strcmp(argv[i], "-d") && i + 1 < argc)
            dir = argv[++i];
        else if (!strcmp(argv[i], "--")) {
            i++;
            break;
        } else
            break;
    }
    if (i >= argc || em < 1 || em > 128 || (italic && (em < 3 || em > 32))) {
        fprintf(stderr, "usage: %s [-s SIZE] [-b] [-i] [-o OUT.pgm] [-d WEIGHTS_DIR] TEXT...\n"
                        "SIZE is 1 to 128; italic is 3 to 32\n", argv[0]);
        return 2;
    }
    text[0] = 0;
    for (; i < argc; i++) {
        size_t n = strlen(argv[i]);
        if (len + n + 2 > sizeof text) {
            fprintf(stderr, "text too long\n");
            return 2;
        }
        if (len)
            text[len++] = ' ';
        memcpy(text + len, argv[i], n + 1);
        len += n;
    }

    if (em <= 32)
        snprintf(path, sizeof path, "%s/neuronte-ui%d%s-v1.nrt", dir, em, italic ? "-italic" : "");
    else
        snprintf(path, sizeof path, "%s/neuronte-ui-large-v0.nrt", dir);
    rc = neuronte_open_file(path, &face);
    if (rc != NEURONTE_OK) {
        fprintf(stderr, "%s: cannot open (%d). Run from the top of the tree or pass -d.\n", path, rc);
        return 1;
    }
    if (em <= 32)
        neuronte_set_binary(face, 1);

    /* Each letter's cell is em wide and two em high; leave an em at the right
     * for the last cell and for an italic's lean. */
    w = neuronte_text_width(face, text, len, bold, em) + em + 2;
    h = 2 * em;
    if (w < 1 || (px = malloc((size_t)w * h * sizeof *px)) == NULL) {
        fprintf(stderr, "cannot lay out the text\n");
        return 1;
    }
    for (i = 0; i < w * h; i++)
        px[i] = 0xFFFFFFFFu;
    rc = neuronte_draw_text(face, px, w, w, h, 1, 0, em, bold, text, len, 0xFF000000u);
    if (rc < 0) {
        fprintf(stderr, "draw failed (%d)\n", rc);
        return 1;
    }

    /* Keep the rows that hold ink. */
    top = h;
    bottom = -1;
    for (y = 0; y < h; y++)
        for (x = 0; x < w; x++)
            if ((px[(size_t)y * w + x] & 0xFF) != 0xFF) {
                if (y < top)
                    top = y;
                bottom = y;
                break;
            }
    if (bottom < 0)
        top = bottom = 0;
    /* And the columns: stop one past the last inked one. */
    for (i = w - 1; i > 0; i--) {
        for (y = top; y <= bottom; y++)
            if ((px[(size_t)y * w + i] & 0xFF) != 0xFF)
                break;
        if (y <= bottom)
            break;
    }
    rc = i + 2 < w ? i + 2 : w;

    for (y = top; y <= bottom; y += 2) {
        for (x = 0; x < rc; x++) {
            int a = ink(px, w, bottom + 1, x, y), b = ink(px, w, bottom + 1, x, y + 1);
            fputs(a && b ? "\xe2\x96\x88" : a ? "\xe2\x96\x80" : b ? "\xe2\x96\x84" : " ", stdout);
        }
        putchar('\n');
    }

    if (out) {
        FILE *f = fopen(out, "wb");
        if (!f) {
            perror(out);
            return 1;
        }
        fprintf(f, "P5\n%d %d\n255\n", rc, bottom - top + 1);
        for (y = top; y <= bottom; y++)
            for (x = 0; x < rc; x++)
                fputc((int)(px[(size_t)y * w + x] & 0xFF), f);
        if (fclose(f) != 0) {
            perror(out);
            return 1;
        }
    }
    free(px);
    neuronte_close(face);
    return 0;
}
