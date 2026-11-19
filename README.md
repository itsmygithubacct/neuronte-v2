# Neuronte

Neuronte is a font made of small neural nets. You give it a character and
it writes that character's pixels into a buffer you own. There is no font
file, no glyph table and no bitmap atlas behind it. The weights are the
font.

This repository is **Neuronte 2**, the slim set: the C library and one
packed net for each job, and nothing else. Its weights are 144 MB. The full project,
with the training code, every generation of every net and the measurements
behind them, is [Neuronte 0.1.x](https://github.com/itsmygithubacct/neuronte).
Every net is a file of Neuronte, copied byte for byte under the same name:
the 33 desktop faces of 2.5.0 are files of Neuronte at commit
[bfd0a2d](https://github.com/itsmygithubacct/neuronte/tree/bfd0a2d066161eaa5abeae9747e151ff345702ef),
which no Neuronte tag holds yet; the Neuronte Sans Metric faces and the
wide bundles of 2.6.0 are files of Neuronte 0.1.7, as pushed on its
branches at [0aa0a4c](https://github.com/itsmygithubacct/neuronte/tree/0aa0a4c52ce11cb36273153c28c2fa6b8ade1e9c)
(Metric) and [985c177](https://github.com/itsmygithubacct/neuronte/tree/985c177443d330b7789fefe0001c10dd949640c6)
(wide); and every other net is a file of
[Neuronte 0.1.6](https://github.com/itsmygithubacct/neuronte/tree/v0.1.6).
The library is 0.1.7's source (`src/neuronte.c` and `include/neuronte.h`,
the same bytes at 0aa0a4c); every file of 2.5.1 draws with it exactly as it
did.
[CHANGES.md](CHANGES.md) lists what each version added.

![The same ten lines, 8 to 48 px, drawn by Neuronte 2.1 and by DejaVu Sans](docs/neuronte-2.1-and-dejavu.png)

Neuronte 2.1 on the left, DejaVu Sans through FreeType on the right,
enlarged two times. [Neuronte 2 and DejaVu Sans](docs/neuronte-and-dejavu.md)
explains the differences.

## Try it

It needs a C compiler and `make`, nothing else.

```sh
git clone https://github.com/itsmygithubacct/neuronte-v2
cd neuronte-v2
make demo
```

`make demo` builds the library and draws four lines in your terminal with
the desktop faces. Then draw your own:

```sh
./build/neuronte-say -s 14 "Hello from a neural net"
./build/neuronte-say -s 24 -b "Bold"          # -i for italic, 3 to 32 px
./build/neuronte-say -s 64 -o big.pgm "Big"   # also writes an image
```

Sizes from 1 to 16 use that size's own net, 17 to 32 one scalable net, and
larger sizes another. The source is [`demo/neuronte-say.c`](demo/neuronte-say.c), about a
hundred lines, and shows the whole of the text interface.

## What it draws

| Job | File | In depth |
|---|---|---|
| Terminal text: ASCII, accented Latin, Greek, Cyrillic, symbols, arrows, math signs, box and block characters, Braille | `neuronte-split-v9.nrt` | [split-v9](docs/neuronte-split-v9.md) |
| 1,864 more terminal characters, in four bundles opened beside the first: more Latin and Greek letters, arrows and math operators, technical and geometric signs, dingbats | `neuronte-term-latin-v1.nrt`, `neuronte-term-math-v1.nrt`, `neuronte-term-symbol-v1.nrt`, `neuronte-term-dingbat-v1.nrt` | [more characters](#more-terminal-characters) |
| 1,714 more terminal characters, in six bundles opened the same way: more Latin, Greek and Cyrillic letters, hexagrams, dominoes and cards, mathematical alphabets, Canadian syllabics, Lisu, Hebrew, Lao, faces and moons | `neuronte-term-letters-v1.nrt`, `neuronte-term-blocks-v1.nrt`, `neuronte-term-mathalpha-v1.nrt`, `neuronte-term-syllabics-v1.nrt`, `neuronte-term-scripts-v1.nrt`, `neuronte-term-rest-v1.nrt` | [more characters](#more-terminal-characters) |
| The symbols of a terminal's own bars: one cell, two cells, and nine weather symbols in colour | `neuronte-kilix-chrome-v1.nrt`, `neuronte-kilix-chrome-v2.nrt`, `neuronte-weather-colour-v1.nrt` | [bar symbols](#symbols-for-a-terminals-bars) |
| Desktop text, 1 to 16 px, upright: ASCII, the ellipsis and all of Latin-1 | `neuronte-ui1-v2.nrt` to `neuronte-ui16-v2.nrt` | [faces](#desktop-faces) |
| Desktop text, 3 to 16 px, italic | `neuronte-ui3-italic-v2.nrt` to `neuronte-ui16-italic-v2.nrt` | [faces](#desktop-faces) |
| Desktop text, 17 to 32 px, upright and italic | `neuronte-ui-scaled-latin1-v1.nrt`, with `neuronte-ui-scaled-latin1-italic-advances-v1.nrt` | [scaled face](docs/neuronte-ui-scaled-v0.md) |
| Desktop text above 32 px | `neuronte-ui-large-latin1-v2.nrt` | [large face](docs/neuronte-ui-large-v1.md) |
| Desktop text at the widths of Helvetica and Arial, Neuronte Sans Metric: 1 to 16 px, italics from 3 px, a drawn bold from 9 px, and 17 px up | `neuronte-metric-sans1-v1.nrt` to `neuronte-metric-sans16-v1.nrt` and their `-italic`, `-bold` and `-bold-italic` files, `neuronte-metric-sans-scaled-v1.nrt`, `neuronte-metric-sans-large-v1.nrt` | [Neuronte Sans Metric](#neuronte-sans-metric) |
| 1,261 wide characters, drawn for the box of two cells a terminal gives them: fullwidth forms, wide symbols, pictographs | `neuronte-wide-forms-v1.nrt`, `neuronte-wide-symbols-v1.nrt`, `neuronte-wide-pictographs-v1.nrt`, `neuronte-wide-pictographs2-v1.nrt`, `neuronte-wide-supplement-v1.nrt` | [wide characters](#wide-characters) |
| 300 colour emoji | `neuronte-emoji-v1.nrt` | [emoji-v1][emoji] |
| 167 colour icons | `neuronte-icon-v2.nrt` | [icon-v2][icon] |
| The Kilix flame | `neuronte-flame-v1.nrt` | [flame-v1][flame] |
| A mouse pointer | `neuronte-pointer-v1.nrt` | [pointer-v1][pointer] |
| Video in a terminal | `neuronte-video-masks-v0.nrt` | [the video player](tools/neuronte-video.md) |
| Text in programs that cannot run a net (a browser, GTK and Qt programs): TrueType files made from the desktop faces and the terminal bundles | every file above that draws text | [font files](docs/neuronte-fontfile.md) |

[FILES.md](FILES.md) lists every file with its size, its checksum and the
file of Neuronte 0.1.1 that did the same job.
[BENCHMARKS.md](BENCHMARKS.md) times a desktop drawn with this set, with
the faces of Neuronte 0.1.1, and with DejaVu Sans through FreeType. Once a
size's letters are kept, a string at 11 px takes 62 to 75 µs with Neuronte
2.1 against 193 to 261 µs with DejaVu, and Neuronte is ahead at every size
measured, up to 48 px.
[Neuronte 2 and DejaVu Sans](docs/neuronte-and-dejavu.md) explains how the
two differ: what each is, what reaches the screen, speed, size, coverage
and what each cannot do.

## Build it and check it

It needs a C compiler and `make`. The check also needs Python 3 with NumPy.

```sh
make          # build/libneuronte.so, build/libneuronte.a, build/neuronte-fontfile and build/neuronte-cells
make demo     # draws four lines of text in the terminal
make test     # checks every shipped file
```

`make test` does four things. It verifies every checksum. It checks the
packed text mask calls against a pixel loop. It draws every desktop face
from 1 to 16 px at its size, regular and bold, and compares each letter
with the face's drawing in `font/`: a face may have at most 2 wrong pixels,
none on a digit, and every advance must be exact. Today every face has 0.
And it compares every net's pixels, over a fixed list of characters, and
lines of text drawn by the two scalable faces, with the values recorded in
`tests/golden.json` when this set was cut.

It writes a [cells file](docs/neuronte-cells.md), compares every entry
with the library bit for bit, and opens some 11,000 damaged ones, once
more under AddressSanitizer.

It then makes the [font files](docs/neuronte-fontfile.md) twice, once
through `neuronte-setup` and once with another number of threads, and
checks that the two are the same bytes, that every embedded bitmap is
what the library draws (no pixel may differ) and that every outline
follows its net. That part takes a few minutes and writes only under
`build/`.

## Use it

The whole interface is [`include/neuronte.h`](include/neuronte.h), and every
file opens with the same call.

Terminal text is drawn a cell at a time:

```c
#include "neuronte.h"

neuronte *font;
if (neuronte_open_file("weights/neuronte-split-v9.nrt", &font) != NEURONTE_OK)
    return 1;
/* pixels: row-major 0xAARRGGBB, stride in pixels. A 16 x 32 cell at x, y. */
neuronte_draw_utf8(font, pixels, stride, width, height,
                   x, y, 16, 32, "Hello", 5, 0xFFFFFFFF);
neuronte_close(font);
```

Desktop text is drawn a line at a time, with each letter's own width. Open
the face for the size you want and draw at that size (from 17 px the file
is `neuronte-ui-scaled-latin1-v1.nrt` for every size to 32, and above that
`neuronte-ui-large-latin1-v2.nrt`):

```c
neuronte *face;
neuronte_open_file("weights/neuronte-ui14-v2.nrt", &face);
neuronte_set_binary(face, 1);            /* crisp pixels, as the face was drawn */
int w = neuronte_text_width(face, "Hello", 5, 0 /* bold */, 14);
neuronte_draw_text(face, pixels, stride, width, height,
                   x, y, 14, 0 /* bold */, "Hello", 5, 0xFFFFFFFF);
neuronte_close(face);
```

A program that draws the same small text over and over, as a desktop does,
can keep each letter's pixels after the net has drawn it once and lay lines
out of the kept letters with `neuronte_bits_pack()` and
`neuronte_bits_compose()`; no net runs then. [BENCHMARKS.md](BENCHMARKS.md)
has the effect.

A handle keeps the cells it has drawn, so drawing the same character again
is a copy. Do not share one handle between threads without a lock. The
[Use section of the 0.1.x README][use] covers colour files, palettes and
themes, coverage without drawing, and loading weights from memory.

## Font files for other programs

A browser or a GTK or Qt program takes its text from fontconfig and
FreeType and cannot run a net. `build/neuronte-fontfile` writes what the
nets draw as nine TrueType files, Neuronte Sans and Neuronte Sans Metric
in four styles each and Neuronte Mono, with the nets' own pixels embedded
for the small sizes:

```sh
./build/neuronte-fontfile --weights weights --out ~/.local/share/fonts/neuronte
```

On a system, `neuronte-setup` makes them at install and at start when they
are absent or the weights changed, and a fontconfig file makes them the
last choice for `sans-serif` and `monospace`, after any font that is
installed, and Neuronte Sans Metric the last choice for a program that
asks for Arial, Helvetica, Liberation Sans or Arimo. The Neuronte Sans and
Mono files are the same bytes as 2.5.1 made. [The page on them](docs/neuronte-fontfile.md) says what the
files hold, what they lack (kerning, shaping, hinting, a serif face) and
how each program tried draws them.

## Cells files for programs that call the library

A program that calls the library pays for a character the first time it
draws it, in every process. `neuronte-setup` also draws ASCII and
Latin-1 in four styles at the terminals' cells once
(`build/neuronte-cells`) and keeps the floats in a file every process
maps. A program takes a cell from it with `neuronte_cells_find()`
(`include/neuronte_cells.h`, a new part of the library; `src/neuronte.c`
is unchanged) and from the net when the file does not hold it; the two
are the same bytes. On a Core i7-6820HQ the first 95 ASCII cells of a
fresh process at 9x18 take 0.2 ms in place of 23 ms.
[The page on them](docs/neuronte-cells.md) has the calls, the settings
and what a cells file does not cover.

## How it works, briefly

Each net is a small multilayer perceptron. It is asked about one point in
a character's cell at a time and answers how far that point is from the
ink. The library asks it about every pixel and turns the answers into
coverage. Because the net answers for a point and not a pixel, the same
weights draw a cell of any size.

One large net for everything is slow, so the files here are **bundles** of
many small nets. A table sends each character to the one small net that was
trained on it, and only that net runs. That is why this set is several
times faster than Neuronte 0.1.1 at the same pixels. More:

- [The net][net]: its inputs, layers and output.
- [Split nets and the router][router]: how a bundle picks a net.
- [Character groups and tiles][decomposition]: how a job is cut into small nets.
- [Speed][speed] and [quality][quality]: how each is measured.
- [Changes in 0.1.2][changes]: what got faster, and by how much.

## Desktop faces

Each size from 1 to 16 px has its own net, because each small size is its
own hand-placed pixel design and not one shape scaled. A net reproduces its
size exactly; no net draws another size's design. Every face, at every
size, draws the same 192 characters: printable ASCII, the ellipsis and all
96 of Latin-1, U+00A0 to U+00FF (`é`, `ñ`, `ß`, `£`, `±`, `×`, `©`, `«`
and the rest). The soft hyphen, U+00AD, is drawn as a hyphen; a program
that hides soft hyphens does so before it draws.

From 17 to 32 px one file, `neuronte-ui-scaled-latin1-v1`, draws every size from
one design, the strokes the hand faces were first fitted to. The library
snaps its straight stems to whole pixels, so a stem is exactly 2 px wide
from 17 to 26 px, and it draws black and white like the small faces. It is
a different face from the hand-drawn ones it replaced, not a copy: against
them about a third of the letter widths differ, by at most 2 px, and the
shapes differ by some tens of pixels a letter. In exchange one 2.5 MB file
does the work of 32 files of 17.4 MB. It draws all of Latin-1: the 58
accented letters, each a letter of the face with a mark placed over it,
and the other 38 (signs, ligatures, fractions, `ß`, `Æ`, `Ð`, `Þ` and the
rest) drawn as strokes from their Unicode names and the face's own
letters ([card of the 38][signs]). Italic is the same letters sheared as
they are drawn (`NEURONTE_STYLE_ITALIC`), stepped by the advances in
`neuronte-ui-scaled-latin1-italic-advances-v1`; `demo/neuronte-say.c` shows how.
[Its card](docs/neuronte-ui-scaled-v0.md) has the measurements for every
size. Scaled below 17 px the design is not good enough to ship, and below
10 px it is not legible, which is why the small sizes stay hand-drawn.

Above 32 px one file, `neuronte-ui-large-latin1-v2`, draws any size with smooth
edges. It is the face of `neuronte-ui-large-v0` cut into four smaller nets,
about 13 times as fast the first time a size is used, with the same
Latin-1 characters added by four more nets. It did not
meet the accuracy limit set for it before it was trained (it is behind the
old net at 32 px, a size it is not used for, and on its worst letter at 48
and 96 px) and ships by the owner's decision;
[its card](docs/neuronte-ui-large-v1.md) gives the numbers.

The hand-drawn faces were made in groups, each with a card that gives the
recipe, the acceptance rule and the measured speed of every face in it:

| Faces | Card |
|---|---|
| 1 to 15 px; italics 3 to 12, 14 and 15 px | [small faces][faces-small] |
| 16 px; italics 13 and 16 px | [narrower single nets][faces-192-256] |
| Latin-1 of every face from 1 to 16 px, upright and italic, beside those nets | [pixel faces v2][faces-v2] |
| The 38 Latin-1 signs of the scaled and large faces | [Latin-1 signs][signs] |

Each `-v2` face is its `-v1` file's nets, unchanged, and new nets for the
96 Latin-1 characters, so its ASCII draws byte for byte as before; every
one of the 192 characters is exactly its drawing at the native em, regular
and bold, with both seeds, and `make test` checks the shipped files again.
Below 8 px a face has too few pixels to tell every Latin-1 character
apart, and some share a bitmap with another; the card lists them.

A desktop also needs to know where a face sits in its cell and how wide
each letter is. `font/ui-tops.json` gives the first, and each
`font/ui-EM-latin1.txt` (italic `font/ui-EM-latin1-italic.txt`) holds that
size's advances and pixels.

## Neuronte Sans Metric

Neuronte Sans Metric is the desktop face redrawn so that a line set in it
is as long as in Helvetica, and so as in Arial, Liberation Sans and Arimo,
which share Helvetica's widths: a page laid out for those fonts keeps its
line breaks. **Its advances are copied** from Adobe's Core 14 AFM metric
files for Helvetica and Helvetica-Bold, the one place in the project
where widths are copied ([PROVENANCE.md](PROVENANCE.md),
[THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md)); the letters are this
project's own strokes and pixel drawings, redrawn to those widths. It is a
new face beside Neuronte Sans, which does not change.

- `neuronte-metric-sansN-v1` (1 to 16 px) and `neuronte-metric-sansN-italic-v1`
  (3 to 16 px): every letter exactly its drawing at its advance. The
  italic is a pure slant at the upright advances. At 1 and 2 px the
  characters Helvetica gives 0 px keep 1 px (51 at 1 px, 4 at 2 px).
- `neuronte-metric-sansN-bold-v1` and `-bold-italic-v1` (9 to 16 px): a
  bold drawn at Helvetica-Bold's widths. Draw these files with `bold` = 0;
  they are the bold. The struck bold (`bold` = 1) of a pixel face touches
  its neighbours, because Helvetica-Bold is only a little wider than
  Helvetica.
- `neuronte-metric-sans-scaled-v1` (17 to 32 px, snapped) and
  `neuronte-metric-sans-large-v1` (33 px up). Their tables also shift the
  drawing of 12 characters (`" / \ ^ j © ® ¯ Î Ï î ï`) without moving the
  pen; the library applies the shift. **They FAILED the gate declared for
  them** (G-M1, 8 of 12 groups, both seeds) because Helvetica's box edges
  land on ties in the gate's reference, and ship by the owner's ruling of
  2026-11-18 on net against its own strokes at the same bar.

Each advance is held in thousandths of an em, and `neuronte_advance()`
rounds it, `(m * em + 500) / 1000` pixels. A line drawn a whole pixel at a
time drifts from Helvetica's length (by up to 5% at 6 to 10 px);
`neuronte_draw_text_exact()` keeps the pen in thousandths and ends a line
within half a pixel of it. The card gives the gates, the touching letters,
the speed and the notes for callers:
[Neuronte Sans Metric][metric]. The drawings, the widths tool and the
checks of the faces against their drawings are in Neuronte; here
`font/metric-sans-widths.txt` holds Adobe's numbers, and `make test`
checks the files' pixels and advances against `tests/golden.json`, and
the font files' advances against those numbers.

## More terminal characters

Ten more bundles draw 3,578 characters the terminal bundle was not trained
on. With it that is 4,919 characters. Each is a file of its own, opened
beside `neuronte-split-v9.nrt`; none of them draws ASCII or anything else
split-v9 draws, and no two share a character.

| File | Characters | Nets and the limits set before training |
|---|---|---|
| `neuronte-term-latin-v1.nrt` | 596 more Latin letters: Latin Extended-B, IPA, spacing modifiers, Latin Extended Additional, Greek Extended | 74 nets; 3 passed the limits set before training, 71 missed them and ship by the owner's choice; [card](https://github.com/itsmygithubacct/neuronte/blob/v0.1.5/weights/neuronte-term-latin-v1.md) |
| `neuronte-term-math-v1.nrt` | 465 arrows and mathematical operators | 47 nets; 24 passed, 23 missed and ship by the owner's choice; [card](https://github.com/itsmygithubacct/neuronte/blob/v0.1.5/weights/neuronte-term-math-v1.md) |
| `neuronte-term-symbol-v1.nrt` | 480 punctuation, letterlike, technical and geometric characters, and U+FFFD | 48 nets; 20 passed, 28 missed and ship by the owner's choice; [card](https://github.com/itsmygithubacct/neuronte/blob/v0.1.5/weights/neuronte-term-symbol-v1.md) |
| `neuronte-term-dingbat-v1.nrt` | 323 Miscellaneous Symbols and Dingbats | 35 nets; 11 passed, 24 missed and ship by the owner's choice; [card](https://github.com/itsmygithubacct/neuronte/blob/v0.1.5/weights/neuronte-term-dingbat-v1.md) |
| `neuronte-term-letters-v1.nrt` | 812 more letters of Latin, Greek and Cyrillic: the rest of Latin Extended-B, IPA and phonetic letters, small capitals, modifier letters, Latin Extended-C and -D, more Greek, Coptic and Cyrillic | 103 nets, each passing its limits with both seeds; [card](https://github.com/itsmygithubacct/neuronte/blob/v0.1.6/weights/neuronte-term-letters-v1.md) |
| `neuronte-term-blocks-v1.nrt` | 310 characters made by rule: 64 Yijing hexagrams, 87 Tai Xuan Jing figures, 100 domino tiles, 59 playing cards | 5 tiled nets and 1 group net, each passing its limits with both seeds; [card](https://github.com/itsmygithubacct/neuronte/blob/v0.1.6/weights/neuronte-term-blocks-v1.md) |
| `neuronte-term-mathalpha-v1.nrt` | 179 letters and digits of the sans-serif, monospace and double-struck mathematical alphabets | 17 nets, each passing its limits with both seeds; [card](https://github.com/itsmygithubacct/neuronte/blob/v0.1.6/weights/neuronte-term-mathalpha-v1.md) |
| `neuronte-term-syllabics-v1.nrt` | 168 Canadian syllabics | 15 nets, each passing its limits with both seeds; [card](https://github.com/itsmygithubacct/neuronte/blob/v0.1.6/weights/neuronte-term-syllabics-v1.md) |
| `neuronte-term-scripts-v1.nrt` | The 48 characters of Lisu | 6 nets, each passing its limits with both seeds; [card](https://github.com/itsmygithubacct/neuronte/blob/v0.1.6/weights/neuronte-term-scripts-v1.md) |
| `neuronte-term-rest-v1.nrt` | 197 characters: Hebrew letters, presentation forms and punctuation, Lao letters and vowel signs, the baht sign, faces and moon phases in one colour | 26 nets, each passing its limits with both seeds; [card](https://github.com/itsmygithubacct/neuronte/blob/v0.1.6/weights/neuronte-term-rest-v1.md) |

A program opens the eleven files and, for each character, draws with the
first whose `neuronte_untrained(handle, character)` is 0:

```c
static neuronte *pick(neuronte **bundle, int count, uint32_t character) {
    for (int i = 0; i < count; i++)
        if (neuronte_untrained(bundle[i], character) == 0)
            return bundle[i];
    return bundle[0]; /* nobody was trained on it: split-v9, with its mark if asked */
}
```

`weights/NAME.list.txt` beside each bundle lists the characters it was
trained on, as `U+XXXX` and `U+XXXX..U+YYYY`, for a program that would
rather build one table. `make test` checks every bundle against its list.
An invalid code (a surrogate, or a value above U+10FFFF) is drawn as
U+FFFD by the library, so `neuronte-term-symbol-v1`, which has U+FFFD,
reports those trained too.

Most of the nets of the first four bundles missed at least one of the
limits set for them before they were trained: 58 of the 204 passed with
both seeds, and the other 146 ship because the owner looked at them side by
side with the drawings and chose to. The six bundles of 2.4.0 were made by
an automatic rule: a group that failed was trained once more as two halves
and a half that failed again was left out, so every net in them passed;
their limits were amended while the nets were scored, and each card says
how. Four letters, U+1DB4 to U+1DB7, were left out that way. Each card says which limit each net missed and by how much. The
drawings are the project's own; no font file was read to make them.

## Symbols for a terminal's bars

Three bundles hold the symbols a terminal prints on its own bars. They are
not terminal bundles and are not in the count above; a program opens them
before the terminal bundles and draws a symbol with the first that was
trained on it. `weights/NAME.list.txt` or `weights/NAME.symbols.txt` names
what each holds.

| File | Symbols | Nets |
|---|---|---|
| `neuronte-kilix-chrome-v1.nrt` | The 39 symbols the Kilix terminal prints on its bars, one cell each | 6 nets, each passing its limits with both seeds; [card](https://github.com/itsmygithubacct/neuronte/blob/v0.1.6/weights/neuronte-kilix-chrome-v1.md) |
| `neuronte-kilix-chrome-v2.nrt` | 41 of those symbols drawn for a box of two cells | 8 nets, each passing its limits with both seeds; [card](https://github.com/itsmygithubacct/neuronte/blob/v0.1.6/weights/neuronte-kilix-chrome-v2.md) |
| `neuronte-weather-colour-v1.nrt` | Nine weather symbols in colour, two cells each, asked for by halves | 1 colour net, each passing its limits with both seeds; [card](https://github.com/itsmygithubacct/neuronte/blob/v0.1.6/weights/neuronte-weather-colour-v1.md) |

`neuronte-kilix-chrome-v2` is asked for a box of two cells, `w` by `h`
with `w` twice the cell's width: `neuronte_coverage(handle, symbol, w,
2 * h, 0, out)` gives `w` by `2 * h` values, and the mean of each pair of
rows is the box. `neuronte-weather-colour-v1` is asked as an emoji is, a half at
a time, with `neuronte_coverage_layers_half()` or `neuronte_draw_half()`.
In half cells under 14 pixels wide the library snaps edges, and the
picture then differs from the drawing by 4 to 6 of 255 on average.

## Wide characters

Five bundles draw 1,261 wide characters, the ones a terminal gives two
cells (East Asian Width W or F), for that box, the way
`neuronte-kilix-chrome-v2` does: ask for a box `w` by `h` as the cell `w`
by `2 * h` and take the mean of each pair of rows. They were drawn from
each character's Unicode name; no font was opened. Each set is a
`weights/NAME.symbols.txt`, not a `.list.txt`, because some of these code
points are also one-cell characters of a terminal bundle. A program opens
them before the terminal bundles for a character it gives two cells.

| File | Characters | Nets |
|---|---|---|
| `neuronte-wide-forms-v1.nrt` | The 103 fullwidth forms (U+FF01..FF60, U+FFE0..FFE6) | 12 group nets, each passing its limits with both seeds; [card][wide-forms] |
| `neuronte-wide-symbols-v1.nrt` | 282 wide signs below U+1F300: game tiles, enclosed and squared signs, emoticons, transport and map symbols, large coloured shapes | 73 nets; **29 FAILED** their limits and ship by the owner's word; 42 drawn characters left out; [card][wide-symbols] |
| `neuronte-wide-pictographs-v1.nrt` | 273 of Miscellaneous Symbols and Pictographs, U+1F300..1F448 | 78 nets; **27 FAILED**; 6 left out; [card][wide-pictographs] |
| `neuronte-wide-pictographs2-v1.nrt` | 266 of U+1F449..1F5FF (a second file: one bundle holds at most 127 group nets) | 68 nets; **16 FAILED**; 14 left out; [card][wide-pictographs2] |
| `neuronte-wide-supplement-v1.nrt` | 337 of Supplemental Symbols and Pictographs and Symbols and Pictographs Extended-A | 91 nets; **23 FAILED**; 12 left out; [card][wide-supplement] |

A failing group was halved once. Of the halves that failed again, the
owner shipped the 95 whose better seed missed by at most 25%; they are
marked FAILED on the cards. Characters Unicode tells apart only by colour
are drawn alike, by the owner's choice. None of the five is a default, and
Neuronte Mono does not take them (a font file's glyph is one cell).

## The nets, one by one

- **Terminal bundle.** `neuronte-split-v9` is [split-v8][split-v8] with 165
  more characters on fourteen small nets: check and cross marks, arrows,
  bullets and shapes, `± ≤ ≥ ≠`, currency, superscripts and fractions, math
  operators, Greek with tonos and the Cyrillic letters beyond Russian.
  Every character split-v8 drew, it draws the same. All fourteen new nets
  missed the limits set for them before they were trained and ship by the
  owner's choice after a side-by-side review;
  [the card](docs/neuronte-split-v9.md) says where each missed. A character
  the bundle was not trained on is still drawn, by a net that never saw
  it, as some other shape: `neuronte_untrained()` tells a program which
  characters those are, and `neuronte_set_untrained()` draws a plain mark
  in their place. split-v8 is built from parts, each with
  its own card: [ASCII in four groups][ascii], [Braille as tiles][braille],
  [symbols][symbol], [block, sextant and octant characters][pixel], and
  [accented Latin, Greek and Russian][accent]. Two of those parts, the pixel
  and accent groups, missed the strict limits set before they were trained
  and were chosen after a side-by-side review; the split-v8 card lists what
  differs.
- **Emoji.** [emoji-v1][emoji]: twelve nets over a partition balanced by
  difficulty, about five times as fast as the one it replaces.
- **Icons.** [icon-v2][icon]: four nets of three layers. It is about four
  times as fast as icon-v1 and a little less exact at 12 px.
- **Flame and pointer.** [flame-v1][flame] and [pointer-v1][pointer] are
  narrower nets of the same drawings.
- **Video.** [The player](tools/neuronte-video.md) plays a video in a
  terminal. Its default mode needs only `neuronte-video-masks-v0.nrt`.
  From 2.6.0 `TwoInk.rank()` in `tools/neuronte_video.py` ranks every
  two-colour candidate of every cell exactly with one matrix product, so
  the full mode can fit a short list of candidates per cell;
  `tests/check_rank.py` checks it.

## What is not here

- The older files: `neuronte-split-v5`, the `-v0` desktop faces and the
  rest. A program that used them names the files above instead.
- The hand-drawn faces from 17 to 43 px. `neuronte-ui-scaled-latin1-v1` and
  `neuronte-ui-large-latin1-v2` cover those sizes; the hand-drawn ones are in
  Neuronte 0.1.x and in Neuronte 2.1.0.
- The ASCII-only faces of 2.4.0 (`neuronte-uiN-v1`, `neuronte-uiN-italic-v1`,
  `neuronte-ui-scaled-latin1-v0` with its italic advances `-v0`, and
  `neuronte-ui-large-latin1-v1`) and their drawings `font/ui-N.txt` and
  `font/ui-N-italic.txt`. The files that replace them draw the same ASCII
  pixels; they are in Neuronte 2.4.0 and in Neuronte 0.1.x.
- The learned video helpers. They are tied to `neuronte-split-v5`, so the
  player here has no `--helper` option, and its cells mode draws with
  `neuronte-split-v9`.
- The drawings of Neuronte Sans Metric (`font/metric-sans-*.txt`), the
  tools that made them and its widths file, its cards, its checks against
  its drawings and the Helvetica PDF fixture, and the wide bundles'
  provenance and validation records. They are in Neuronte 0.1.7.
- Training code, weights as JSON, second seeds, scores and research
  records. They are in [Neuronte 0.1.x][repo], where every net can be
  retrained and rechecked. `train/glyphs.py` and `train/strokes.py` are here
  only because the video player and the check import them.

## Where things came from

[PROVENANCE.md](PROVENANCE.md) says how the drawings and nets were made.
The licence is MIT; see [LICENSE](LICENSE). The optional media library,
built with `make media`, links FFmpeg; see
[THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md).

[repo]: https://github.com/itsmygithubacct/neuronte
[readme]: https://github.com/itsmygithubacct/neuronte/blob/v0.1.2/README.md
[use]: https://github.com/itsmygithubacct/neuronte/blob/v0.1.2/README.md#use
[net]: https://github.com/itsmygithubacct/neuronte/blob/v0.1.2/README.md#the-net
[router]: https://github.com/itsmygithubacct/neuronte/blob/v0.1.2/README.md#split-nets-and-the-router
[speed]: https://github.com/itsmygithubacct/neuronte/blob/v0.1.2/README.md#speed
[quality]: https://github.com/itsmygithubacct/neuronte/blob/v0.1.2/README.md#quality
[changes]: https://github.com/itsmygithubacct/neuronte/blob/v0.1.2/CHANGES.md
[decomposition]: https://github.com/itsmygithubacct/neuronte/blob/v0.1.2/train/DECOMPOSITION.md
[split-v8]: https://github.com/itsmygithubacct/neuronte/blob/v0.1.4/weights/neuronte-split-v8.md
[ascii]: https://github.com/itsmygithubacct/neuronte/blob/v0.1.2/weights/neuronte-ascii-next.md
[braille]: https://github.com/itsmygithubacct/neuronte/blob/v0.1.2/weights/neuronte-braille-v1.md
[symbol]: https://github.com/itsmygithubacct/neuronte/blob/v0.1.2/weights/neuronte-symbol-v1.md
[pixel]: https://github.com/itsmygithubacct/neuronte/blob/v0.1.2/weights/neuronte-pixel-groups-v1.md
[accent]: https://github.com/itsmygithubacct/neuronte/blob/v0.1.2/weights/neuronte-accent-groups-v1.md
[emoji]: https://github.com/itsmygithubacct/neuronte/blob/v0.1.2/weights/neuronte-emoji-v1.md
[icon]: https://github.com/itsmygithubacct/neuronte/blob/v0.1.2/weights/neuronte-icon-v2.md
[flame]: https://github.com/itsmygithubacct/neuronte/blob/v0.1.2/weights/neuronte-flame-v1.md
[pointer]: https://github.com/itsmygithubacct/neuronte/blob/v0.1.2/weights/neuronte-pointer-v1.md
[faces-small]: https://github.com/itsmygithubacct/neuronte/blob/v0.1.2/weights/neuronte-ui-v1-small.md
[faces-192-256]: https://github.com/itsmygithubacct/neuronte/blob/v0.1.2/weights/neuronte-ui-v1-192-256.md
[faces-v2]: https://github.com/itsmygithubacct/neuronte/blob/bfd0a2d066161eaa5abeae9747e151ff345702ef/weights/neuronte-ui-v2.md
[signs]: https://github.com/itsmygithubacct/neuronte/blob/bfd0a2d066161eaa5abeae9747e151ff345702ef/weights/neuronte-ui-latin1-signs-v0.md
[metric]: https://github.com/itsmygithubacct/neuronte/blob/0aa0a4c52ce11cb36273153c28c2fa6b8ade1e9c/weights/neuronte-metric-sans-v1.md
[wide-forms]: https://github.com/itsmygithubacct/neuronte/blob/985c177443d330b7789fefe0001c10dd949640c6/weights/neuronte-wide-forms-v1.md
[wide-symbols]: https://github.com/itsmygithubacct/neuronte/blob/985c177443d330b7789fefe0001c10dd949640c6/weights/neuronte-wide-symbols-v1.md
[wide-pictographs]: https://github.com/itsmygithubacct/neuronte/blob/985c177443d330b7789fefe0001c10dd949640c6/weights/neuronte-wide-pictographs-v1.md
[wide-pictographs2]: https://github.com/itsmygithubacct/neuronte/blob/985c177443d330b7789fefe0001c10dd949640c6/weights/neuronte-wide-pictographs2-v1.md
[wide-supplement]: https://github.com/itsmygithubacct/neuronte/blob/985c177443d330b7789fefe0001c10dd949640c6/weights/neuronte-wide-supplement-v1.md
