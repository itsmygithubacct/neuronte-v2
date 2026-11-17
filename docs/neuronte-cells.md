# Cells files: likely characters drawn before a program asks

A program that calls the library pays for a character the first time it
draws it, in every process: the library's cache belongs to one handle.
A **cells file** is that work done once, at setup, and kept where every
process maps it read-only. It holds exactly the floats
`neuronte_coverage_styled()` writes, so a program may take a cell from
the file when it is there and from the net when it is not, and nothing
it draws changes by a bit.

Programs that take their text from fontconfig do not need this: for
them the font file is the cache (`docs/neuronte-fontfile.md`).

## What it saves

A fresh process on a Core i7-6820HQ, `neuronte-split-v9`, the files in
the page cache, median of 9 runs, under the machine's benchmark lock.
"Get" is the time to have every cell; "open" is
`neuronte_cells_open()`.

| Characters, cell | No cells file | Cells file: open + get |
|---|---:|---:|
| ASCII (95), regular, 7x14 | 14.1 ms | 0.17 + 0.02 ms |
| ASCII, regular, 9x18 | 23.0 ms | 0.18 + 0.02 ms |
| ASCII, regular, 12x24 | 39.4 ms | 0.24 + 0.04 ms |
| ASCII and Latin-1 (189), four styles, 7x14 | 250 ms | 0.17 + 0.13 ms |
| ASCII and Latin-1, four styles, 8x17 | 346 ms | 0.18 + 0.15 ms |
| ASCII and Latin-1, four styles, 9x18 | 407 ms | 0.18 + 0.16 ms |
| ASCII and Latin-1, four styles, 10x20 | 507 ms | 0.18 + 0.18 ms |
| ASCII and Latin-1, four styles, 12x24 | 717 ms | 0.17 + 0.31 ms |

What it does not save: opening the weight file, 4.7 to 4.8 ms in every
row with or without a cells file, and any character, cell or style the
file does not hold. A cell the file lacks costs what it always did (95
ASCII at 13x26: 47.4 ms with the file open and every find a miss, 45.9
ms without it, which is the noise of the measurement; the open itself
is the 0.17 ms). A file on a disk that has not been read since the
machine started was not measured.

## Using one

```c
#include "neuronte_cells.h"

/* Once, after neuronte_open_file(): cells stays NULL when there is no
 * file, or it is damaged, or it was made from other weights. */
neuronte_cells *cells = NULL;
neuronte_cells_open("/var/lib/neuronte/cells/neuronte-split-v9.cells",
                    "/usr/share/neuronte/weights/neuronte-split-v9.nrt", 0, &cells);

/* For each cell: from the file when it holds it, else from the net. */
static int cell(const neuronte *n, const neuronte_cells *cells, uint32_t scalar, int w, int h,
                unsigned style, float *out)
{
    const float *kept = cells ? neuronte_cells_find(cells, scalar, w, h, 0, style,
                                                    NEURONTE_CELLS_SNAP_FILE) : NULL;
    if (!kept)
        return neuronte_coverage_styled(n, scalar, w, h, 0, style, out);
    memcpy(out, kept, (size_t)w * h * sizeof *out);
    return NEURONTE_OK;
}
```

`tests/test_cells.c` holds this function and tests it for a character
the file holds, a cell it does not hold, and no file. A caller that
only reads the floats can use the pointer and skip the copy; it stays
valid until `neuronte_cells_close()`.

The cells file of a weight file is in the `cells` directory of
Neuronte's state directory (`/var/lib/neuronte/cells`, or under
`state=` of `/etc/neuronte/setup.conf`) and has the weight file's name
with `.cells` in place of `.nrt`. A program that has a faster way to
draw (a GPU) asks the cells file first: a hit costs a search of a
sorted index and nothing else.

Rules a caller must keep:

- **The key is everything that changes the bytes.** The scalar, the
  cell's width and height, the samples per axis as passed (0, the
  library's own choice, is what both terminals pass and what the
  setup's files hold; it is not matched with the number the library
  would choose), the style, and the handle's snap threshold.
- **The snap threshold is stated by the caller**, because the library's
  source is unchanged and has no call that reads a handle's settings. A
  caller that never called `neuronte_set_snap()` passes
  `NEURONTE_CELLS_SNAP_FILE`. One that did must pass the number it gave,
  and gets NULL from every file made under another.
- **`neuronte_coverage()`** writes what `neuronte_coverage_styled()`
  writes for style 0, so the same file answers it: ask with style 0.
- **Only trained characters are in a file**, and those are drawn the
  same whatever `neuronte_set_untrained()` says, so that setting does
  not matter here. The untrained mark is never in a file; it is a few
  comparisons to draw. `neuronte_set_binary()`, `neuronte_set_theme()`
  and `neuronte_set_samples()` change the draw calls, not coverage.
- **A file belongs to one weight file.** `neuronte_cells_open()`
  compares the weight file's size and modification time with the cells
  file's header and hashes the weight file only when the time differs
  (hashing `neuronte-split-v9` at every open would cost most of what
  the file saves). A weight file changed with its size and time kept is
  therefore seen only with the flag `NEURONTE_CELLS_VERIFY`.
- **Replace, never rewrite.** The file is mapped. A new one is written
  beside it and renamed over it, as `neuronte_cells_write()` does; a
  process that has the old one mapped keeps it.

What is checked when a file is opened: a check value over the header
and the whole index (a file with any byte of them changed is refused),
then every field and every entry against the size of the file, so that
no later call reads outside it whatever the file holds. The floats are
not read at open: a float changed on the disk is not seen there.
`neuronte-cells --verify` draws every entry again and sees it.

## Making them: `neuronte-cells` and the step `30-cells`

```
neuronte-cells --weights DIR --out DIR [--stamp FILE] [--conf FILE]
               [--cells LIST] [--chars SET] [--styles LIST]
               [--check] [--verify] [--force] [--threads N] [--list] [--verbose]
```

`neuronte-setup` runs it as the step `30-cells`, after the font steps
and not before the login screen: a program that finds no cells file
asks the net. The files are the system's, written by root for everyone
to read, one copy for all users. Unlike a GPU driver's cache they
depend on the weights alone and hold no code, so there is nothing to
make for each user.

The bundles come from the weights directory: the newest
`neuronte-split-vN`, every `neuronte-term-*` and every
`neuronte-kilix-*`, and any other weight file the settings name. A
weight file with a colour net, or with no table of the characters it is
trained on, gets no cells file.

Settings, in `/etc/neuronte/setup.conf`:

| Key | Meaning | Default |
|---|---|---|
| `cells` | the cells, `WxH` with commas | `7x14,8x16,8x17,9x18,10x19,10x20,11x22,12x24` |
| `cells_chars` | `ascii`, `latin1`, `all`, `none`, or scalars and ranges in hexadecimal (`2500-257F`) | `latin1` |
| `cells_styles` | `regular`, `bold`, `italic`, `bolditalic` | all four |
| `cells.NAME`, `cells_chars.NAME`, `cells_styles.NAME` | the same for one weight file (its name without `.nrt`) | |

Where the defaults come from:

- **The cells** are the ones the two terminals ask for. The
  Neuronte-only terminal's cell is round(0.6 em) wide and twice as
  high, which is 7x14, 8x16, 9x18, 10x20, 11x22 and 12x24 for 9 to 15
  point at 96 dots to the inch. Kilix's cell is the cell of its font at
  the user's size on the user's display: 9x18 was measured on one
  machine, and 8x17, 10x19, 10x20 and 12x24 are the others its tests
  use. A machine whose terminal has another cell
  misses every time until `cells=` names it: the cell depends on the
  font size and the display, which setup cannot know.
- **The characters** are ASCII and Latin-1, of which `neuronte-split-v9`
  is trained on 189. The four `neuronte-term-*` bundles are trained on
  no ASCII or Latin-1 character (one of them on a single one), so as
  shipped they get no cells file worth the name: their characters are
  uncommon by nature. `all` is a setting, and what it costs is below.
- **A `neuronte-kilix-*` bundle** is small and is Kilix's own: all its
  characters, regular only, at Kilix's five cells and at the two-cell
  boxes of each, which Kilix asks for at twice the width and twice the
  height (16x34, 18x36, 20x38, 20x40, 24x48). No such bundle is in this
  repository yet; the rule is tested with another weight file under
  such a name.

What they cost, same machine, the weights of 2.3.0:

| Made | 4 threads | 1 thread | On disk |
|---|---:|---:|---:|
| As shipped: 189 characters of split-v9, 8 cells, 4 styles (6,048 entries) | 1.08 s | 3.64 s | 4.5 MB |
| `cells_chars=all`, `cells_styles=regular`: 3,205 characters of five bundles | 2.84 s | not measured | 19 MB |
| `cells_chars=all`, four styles: 102,560 entries | 10.6 s | 37.3 s | 73 MB |

The bytes are the same at any number of threads. With
`cells_chars=all` the cells file of split-v9 is 31.7 MB and opens in
1.06 ms in place of 0.17 ms, because the index is checked at every
open; the 95 ASCII cells then cost the same 0.02 ms.

`neuronte-cells --check`, which the step runs at every start, compares
sizes and times with its stamp and opens nothing: 2.0 ms. `--verify`
opens each file against its weights, hashes them, and draws every entry
again: 1.05 s for the files as shipped.

## What a second kind would need

This file holds one plane of coverage for an entry. A colour glyph
(the emoji and icon nets) is drawn as two halves, each of up to six
layers with a palette entry for each layer, and its worst first use is
far larger than a letter's (a whole emoji set at a new size is about 3
s on one thread). A cells file for those would be another **kind** in
the same header: the key gains the half, an entry holds its count of
layers, their palette entries and one plane for each, and the snap
threshold, which matters for colour glyphs below a width, is already in
the header. Version 1 readers refuse a kind they do not know, so such a
file can be added without breaking them. It is not built.

## Tests

- `tests/test_cells.c` (`make test` runs it twice, the second time
  built with AddressSanitizer and the undefined behaviour checks):
  3,018 entries of 213 characters, each compared bit for bit with
  `neuronte_coverage_styled()` on an untouched handle and on one with
  the untrained mark, binary letters, the light theme and a sample
  count set, and every style 0 entry with `neuronte_coverage()`; the
  writer's refusals, and that a refused write leaves the old file; a
  file made under another snap threshold; other weights, weights
  changed with a new time, weights changed with size and time kept
  (seen only with `NEURONTE_CELLS_VERIFY`); and some 11,000 damaged
  files: every byte of the header and index changed to several values,
  every cut length, longer files, random changes, headers that claim
  other counts. None opens. Each is then tried again with the header's
  check value made right for the damage, so that the checks behind it
  are reached: those that open (about 460) are read through, every
  float of every entry, in memory of exactly the file's size.
- `tests/check_setup.sh`, part 9: the step and the settings.
