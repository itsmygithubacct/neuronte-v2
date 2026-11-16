# Benchmarks

The desktop measurements below were made with Neuronte 2.0 and 2.1.0. Since
2.1.1 the sizes from 17 px up are drawn by two scalable faces and not by a
file for each size, so the rows for 20 px and larger describe 2.1.0. The
sizes up to 16 px use the same files as then. [Terminal cells, three
ways](#terminal-cells-three-ways) at the end was measured with 2.2.0.

Measured on 2026-11-15. Four ways of drawing the text of one desktop are
compared:

- **0.1.1 faces**: the Neuronte files that Plebian-OS 0.2.3-rc9 installs
  (Neuronte commit `4134bf9`, desktop faces `neuronte-ui{em}-v0.nrt`).
- **Neuronte 2.0**: this repository at `92ac10f` (`neuronte-ui{em}-v1.nrt`).
  The desktop lays kept letters into a line with Pillow calls from Python.
- **Neuronte 2.1**: the same weight files, with the library's packed text
  mask calls (`src/bits.c`). The desktop lays a line in one C call.
- **DejaVu**: no Neuronte at all. The desktop draws DejaVu Sans with
  FreeType through Pillow, as an ordinary Debian program does.

## Neuronte 0.1.2 and Neuronte 2 draw at the same speed

They were not timed against each other, because there is nothing to
differ: every weight file here is a file of Neuronte 0.1.2 byte for byte,
and the code that runs the nets is the same source. What differs is what
has to be fetched and kept:

| | Neuronte 0.1.2 | Neuronte 2 |
|---|---:|---:|
| Checked-out tree | 1,087 MB, 6,147 files | 47 MB |
| Weight files | every generation of every net | 70, one per job |

A cold download of the files an installer needs from this repository took
38 s on a line of about 3.5 MB/s. The speed of each net of 0.1.2 against
the net of 0.1.1 it replaces is in the
[0.1.2 change list](https://github.com/itsmygithubacct/neuronte/blob/v0.1.2/CHANGES.md)
and in each net's card.

## What was run

The program is the [Kilix 95](https://github.com/itsmygithubacct/kilix-95)
desktop of Plebian-OS 0.2.3-rc9, changed to open the `-v1` files and to
use the packed text mask calls where the library has them. Each run is one
new process. It starts the desktop without a screen at 1024 × 768 and
draws the first frame. Then, for each size, it draws 200 strings (4,329
characters: ten desktop labels and 190 random ASCII strings of 4 to 40
characters) four times with Pillow's `ImageDraw.text`, three times at
48 px. A cold run has an empty home directory. A warm run follows it with
the same home directory, so the letter cells the desktop saved are there.
Each set had three cold and three warm runs on each machine, one process
at a time, with nothing else being measured on the machine.

Both machines ran Debian 13 with Python 3.13.5 and Pillow 11.1.0, and
built the library with GCC 14.2.0 at `-O3`. The scripts and every run's
numbers are in [`bench/`](bench/); `python3 bench/tables.py i7-6820hq
i7-10700f` prints the tables below from them.

## What the numbers say

- **Start-up is the same.** The first frame arrives within 0.03 s of
  DejaVu's on every set.
- **The net runs once per letter, and the `-v1` faces make that short.**
  The desktop keeps each letter's pixels after the net has drawn it.
  Drawing the letters of a size for the first time took between 3.5 and 37
  times less with Neuronte 2.1 than with the 0.1.1 faces at every size
  from 8 to 32 px. At 32 px that is 0.2 s in place of 4.3 s on the older
  machine.
- **The scalable face above 32 px is the same file in every set.** Its
  first use at 48 px costs 2.9 to 4.4 s, most of it the net.
- **Once the letters are kept, Neuronte 2.1 draws a string faster than
  DejaVu at every size measured.** No net runs then. At 11 px, the
  desktop's own size, a string takes 62 to 75 µs against 193 to 261 µs:
  DejaVu takes 3.1 to 3.5 times as long. The margin narrows as letters
  grow, to 1.6 or 1.7 times at 32 px and 1.2 times at 48 px.
- **Laying the line in C is what did that.** With the same weights and
  the line laid from Python (Neuronte 2.0), a string took 5 times as long
  at 11 px and 12 to 15 times as long at 32 and 48 px, which was slower
  than DejaVu at every size.
- **Measuring a string is 4 times faster with Neuronte**, because a
  width is a sum of whole-pixel advances.

The desktop draws its own two sizes, 10 and 11 px, on a thread at
start-up, so for those the first-use cost is the gap between the first
frame and "start-up letters all drawn" in a cold start. Bold is the
regular letter struck twice and runs no net of its own; its numbers are in
the result files. Three runs of each kind is a small sample; the lowest
and highest first-frame times are given so the spread can be seen.

### Intel Core i7-6820HQ

Start, seconds from process start (median of 3; lowest to highest first frame in brackets):

| Set | Start | Modules loaded | First frame | Start-up letters all drawn |
|---|---|---:|---:|---:|
| 0.1.1 faces | cold | 0.321 | 0.447 (0.447 to 0.449) | 0.636 |
| 0.1.1 faces | warm | 0.319 | 0.415 (0.414 to 0.418) | 0.415 |
| Neuronte 2.0 | cold | 0.328 | 0.433 (0.433 to 0.434) | 0.433 |
| Neuronte 2.0 | warm | 0.323 | 0.417 (0.416 to 0.419) | 0.417 |
| Neuronte 2.1 | cold | 0.326 | 0.434 (0.433 to 0.435) | 0.434 |
| Neuronte 2.1 | warm | 0.325 | 0.419 (0.418 to 0.423) | 0.419 |
| DejaVu | cold | 0.341 | 0.440 (0.439 to 0.441) | n/a |
| DejaVu | warm | 0.341 | 0.438 (0.438 to 0.439) | n/a |

Drawing 200 strings (4,329 characters) at each size, regular weight. "First use" is the first pass less one later pass:

| Size | First use, 0.1.1 faces | First use, Neuronte 2.1 | Each later string, Neuronte 2.0 | Each later string, Neuronte 2.1 | Each later string, DejaVu | DejaVu time ÷ Neuronte 2.1 time |
|---:|---:|---:|---:|---:|---:|---:|
| 8 px | 68 ms | 7 ms | 251 µs | 64 µs | 239 µs | 3.8× |
| 10 px | at start-up | at start-up | 324 µs | 72 µs | 251 µs | 3.5× |
| 11 px | at start-up | at start-up | 380 µs | 75 µs | 261 µs | 3.5× |
| 12 px | 212 ms | 13 ms | 449 µs | 81 µs | 267 µs | 3.3× |
| 14 px | 293 ms | 14 ms | 576 µs | 89 µs | 279 µs | 3.1× |
| 16 px | 695 ms | 193 ms | 725 µs | 96 µs | 297 µs | 3.1× |
| 20 px | 816 ms | 25 ms | 1220 µs | 138 µs | 322 µs | 2.3× |
| 24 px | 1082 ms | 32 ms | 1703 µs | 167 µs | 354 µs | 2.1× |
| 32 px | 4303 ms | 220 ms | 3113 µs | 252 µs | 416 µs | 1.7× |
| 48 px (scalable) | 4387 ms | 3732 ms | 7121 µs | 480 µs | 565 µs | 1.2× |

Measuring a string's width at 11 px: Neuronte 5.5 µs, DejaVu 22.4 µs.

### Intel Core i7-10700F

Start, seconds from process start (median of 3; lowest to highest first frame in brackets):

| Set | Start | Modules loaded | First frame | Start-up letters all drawn |
|---|---|---:|---:|---:|
| 0.1.1 faces | cold | 0.232 | 0.320 (0.318 to 0.322) | 0.459 |
| 0.1.1 faces | warm | 0.237 | 0.306 (0.300 to 0.308) | 0.306 |
| Neuronte 2.0 | cold | 0.238 | 0.308 (0.306 to 0.325) | 0.334 |
| Neuronte 2.0 | warm | 0.239 | 0.311 (0.306 to 0.324) | 0.311 |
| Neuronte 2.1 | cold | 0.239 | 0.311 (0.310 to 0.316) | 0.335 |
| Neuronte 2.1 | warm | 0.237 | 0.308 (0.305 to 0.310) | 0.308 |
| DejaVu | cold | 0.251 | 0.326 (0.322 to 0.331) | n/a |
| DejaVu | warm | 0.250 | 0.325 (0.323 to 0.327) | n/a |

Drawing 200 strings (4,329 characters) at each size, regular weight. "First use" is the first pass less one later pass:

| Size | First use, 0.1.1 faces | First use, Neuronte 2.1 | Each later string, Neuronte 2.0 | Each later string, Neuronte 2.1 | Each later string, DejaVu | DejaVu time ÷ Neuronte 2.1 time |
|---:|---:|---:|---:|---:|---:|---:|
| 8 px | 48 ms | 5 ms | 195 µs | 49 µs | 178 µs | 3.6× |
| 10 px | at start-up | at start-up | 256 µs | 57 µs | 186 µs | 3.3× |
| 11 px | at start-up | at start-up | 294 µs | 62 µs | 193 µs | 3.1× |
| 12 px | 154 ms | 9 ms | 346 µs | 62 µs | 203 µs | 3.2× |
| 14 px | 206 ms | 10 ms | 442 µs | 69 µs | 209 µs | 3.0× |
| 16 px | 510 ms | 146 ms | 564 µs | 73 µs | 221 µs | 3.0× |
| 20 px | 603 ms | 18 ms | 939 µs | 103 µs | 238 µs | 2.3× |
| 24 px | 823 ms | 22 ms | 1302 µs | 124 µs | 263 µs | 2.1× |
| 32 px | 3291 ms | 160 ms | 2382 µs | 187 µs | 306 µs | 1.6× |
| 48 px (scalable) | 3318 ms | 2846 ms | 5449 µs | 354 µs | 416 µs | 1.2× |

Measuring a string's width at 11 px: Neuronte 4.3 µs, DejaVu 16.7 µs.

## Terminal cells, three ways

Measured on 2026-11-16 with Neuronte 2.2.0 (`neuronte-split-v9.nrt`),
Neuronte 0.1.4 with its default terminal bundle (`neuronte-split-v5.nrt`),
and DejaVu Sans Mono 2.37 through FreeType, with DejaVu Sans for the
characters Mono lacks. Each drew one terminal cell for each of the 873
characters all three have. The machines were idle and ran one at a time;
the figures are medians of 9 repeats and include the Python call into each
library.

- **first**: a character's cell the first time it is asked for.
- **again**: the same cell asked for again. Neuronte copies it from the
  handle's cache when it is still there; the cache is small, and with 873
  different characters some are drawn again. FreeType draws every time.

| Cell | Drawn with | Core i7-6820HQ first | again | Core i7-10700F first | again |
|---|---|---:|---:|---:|---:|
| 8x16 | Neuronte 2.2.0 | 421 µs | 68 µs | 322 µs | 51 µs |
| | Neuronte 0.1.4 | 744 µs | 122 µs | 585 µs | 95 µs |
| | DejaVu | 73 µs | 73 µs | 59 µs | 59 µs |
| 12x24 | Neuronte 2.2.0 | 992 µs | 152 µs | 753 µs | 116 µs |
| | Neuronte 0.1.4 | 1,676 µs | 266 µs | 1,295 µs | 203 µs |
| | DejaVu | 75 µs | 74 µs | 60 µs | 60 µs |
| 16x32 | Neuronte 2.2.0 | 1,825 µs | 278 µs | 1,298 µs | 199 µs |
| | Neuronte 0.1.4 | 3,002 µs | 461 µs | 2,326 µs | 366 µs |
| | DejaVu | 76 µs | 75 µs | 61 µs | 61 µs |

Neuronte 2.2.0 draws a cell for the first time about 1.7 times as fast as
Neuronte 0.1.4's default bundle. FreeType is 5 to 24 times as fast as
Neuronte 2.2.0 on a first draw, and its time barely changes with the cell
size while a net's grows with the pixels. A terminal that keeps each cell
after its first draw composes an 80 x 24 screen from kept cells in the same
time whichever drew them (4 to 6 ms at 8x16 in this test's Python).

Characters: the Neuronte 2.2.0 bundle was trained on 1,341, Neuronte
0.1.4's default bundle on 1,176, and the two DejaVu files map 6,065. All
three have 875. DejaVu has 5,026 that Neuronte 2.2.0 does not, and
Neuronte 2.2.0 has 302 that DejaVu does not: sextants, octants and other
legacy computing blocks.

The test that made these numbers, with three terminal panes that draw the
same typed text side by side, is in
kilix-bench, under
[`sidebyside/`](https://github.com/itsmygithubacct/kilix-bench/tree/work/neuronte-sidebyside/sidebyside).
