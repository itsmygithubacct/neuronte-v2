# Benchmarks

Measured on 2026-11-15. Three ways of drawing the text of one desktop are
compared:

- **0.1.1 faces**: the Neuronte files that Plebian-OS 0.2.3-rc9 installs
  (Neuronte commit `4134bf9`, desktop faces `neuronte-ui{em}-v0.nrt`).
- **Neuronte 2**: this repository at `92ac10f` (`neuronte-ui{em}-v1.nrt`).
- **DejaVu**: no Neuronte at all. The desktop draws DejaVu Sans with
  FreeType through Pillow, as an ordinary Debian program does.

## Neuronte 0.1.2 and Neuronte 2 are the same speed

They were not timed against each other, because there is nothing to
differ: every weight file here is a file of Neuronte 0.1.2 byte for byte,
and the library built from this repository has the same SHA-256 as the one
built from 0.1.2. What differs is what has to be fetched and kept:

| | Neuronte 0.1.2 | Neuronte 2 |
|---|---:|---:|
| Checked-out tree | 1,087 MB, 6,147 files | 47 MB, 162 files |
| Weight files | every generation of every net | 70, one per job |

A cold download of the 152 files an installer needs from this repository
took 38 s on a line of about 3.5 MB/s. The speed of each net of 0.1.2
against the net of 0.1.1 it replaces is in the
[0.1.2 change list](https://github.com/itsmygithubacct/neuronte/blob/v0.1.2/CHANGES.md)
and in each net's card.

## What was run

The program is the [Kilix 95](https://github.com/itsmygithubacct/kilix-95)
desktop of Plebian-OS 0.2.3-rc9, with the one change that it opens the
`-v1` files. Each run is one new process. It starts the desktop without a
screen at 1024 × 768 and draws the first frame. Then, for each size, it
draws 200 strings (4,329 characters: ten desktop labels and 190 random
ASCII strings of 4 to 40 characters) four times with Pillow's
`ImageDraw.text`, three times at 48 px. A cold run has an empty home
directory. A warm run follows it with the same home directory, so the
letter cells the desktop saved are there. Each set had three cold and
three warm runs on each machine, one process at a time, with nothing else
being measured on the machine.

Both machines ran Debian 13 with Python 3.13.5 and Pillow 11.1.0, and
built the library with GCC 14.2.0 at `-O3`. The scripts and every run's
numbers are in [`bench/`](bench/); `python3 bench/tables.py i7-6820hq
i7-10700f` prints the tables below from them.

## What the numbers say

- **Start-up is the same.** The first frame arrives within 0.03 s of
  DejaVu's on every set, and a little before it on a warm start.
- **The net runs once per letter, and Neuronte 2 makes that short.** The
  desktop keeps each letter's pixels after the net has drawn it. Drawing
  the letters of a size for the first time took 3 to 34 times less with
  Neuronte 2 than with the 0.1.1 faces at every size from 8 to 32 px. At
  32 px that is 0.2 s in place of 4.2 s on the older machine.
- **The scalable face above 32 px did not change.** It is the same file in
  both sets, and its first use at 48 px costs 3.4 to 4.4 s.
- **Once the letters are kept, DejaVu draws a string faster.** No net runs
  then; the time is Kilix 95 laying kept cells into a line in Python
  against FreeType doing the same in C. The two are level at 8 px.
  Neuronte takes about 1.4 times as long at 11 px, the desktop's own size, about
  5 times at 24 px and about 13 times at 48 px.
- **Measuring a string is 4 times faster with Neuronte**, because a
  width is a sum of whole-pixel advances.

"Net time, first use" is the first pass over the 200 strings less one
later pass. The desktop draws its own two sizes, 10 and 11 px, on a thread
at start-up, so for those the cost is the gap between the first frame and
"start-up letters all drawn" in a cold start. Bold is the regular letter
struck twice and runs no net of its own; its numbers are in the result
files.

### Intel Core i7-6820HQ

Start, seconds from process start (median of 3; lowest to highest first frame in brackets):

| Set | Start | Modules loaded | First frame | Start-up letters all drawn |
|---|---|---:|---:|---:|
| 0.1.1 faces | cold | 0.319 | 0.444 (0.443 to 0.472) | 0.636 |
| 0.1.1 faces | warm | 0.318 | 0.413 (0.412 to 0.416) | 0.413 |
| Neuronte 2 | cold | 0.326 | 0.432 (0.432 to 0.434) | 0.432 |
| Neuronte 2 | warm | 0.325 | 0.419 (0.418 to 0.421) | 0.419 |
| DejaVu | cold | 0.344 | 0.444 (0.441 to 0.444) | n/a |
| DejaVu | warm | 0.344 | 0.444 (0.440 to 0.444) | n/a |

Drawing 200 strings (4,329 characters) at each size, regular weight:

| Size | 0.1.1 faces: net time, first use | Neuronte 2: net time, first use | Ratio | Neuronte, each later string | DejaVu, each later string | Ratio |
|---:|---:|---:|---:|---:|---:|---:|
| 8 px | 67 ms | 9 ms | 7.6× | 250 µs | 240 µs | 1.0× |
| 10 px | at start-up | at start-up | — | 321 µs | 252 µs | 1.3× |
| 11 px | at start-up | at start-up | — | 377 µs | 260 µs | 1.4× |
| 12 px | 211 ms | 15 ms | 14.5× | 444 µs | 268 µs | 1.7× |
| 14 px | 284 ms | 15 ms | 18.3× | 568 µs | 279 µs | 2.0× |
| 16 px | 693 ms | 238 ms | 2.9× | 716 µs | 295 µs | 2.4× |
| 20 px | 821 ms | 26 ms | 32.2× | 1211 µs | 321 µs | 3.8× |
| 24 px | 1065 ms | 34 ms | 31.5× | 1690 µs | 355 µs | 4.8× |
| 32 px | 4227 ms | 214 ms | 19.7× | 3092 µs | 416 µs | 7.4× |
| 48 px (scalable) | 4403 ms | 4376 ms | 1.0× | 7069 µs | 565 µs | 12.5× |

Measuring a string's width at 11 px: Neuronte 5.5 µs, DejaVu 22.4 µs.

### Intel Core i7-10700F

Start, seconds from process start (median of 3; lowest to highest first frame in brackets):

| Set | Start | Modules loaded | First frame | Start-up letters all drawn |
|---|---|---:|---:|---:|
| 0.1.1 faces | cold | 0.235 | 0.320 (0.320 to 0.340) | 0.459 |
| 0.1.1 faces | warm | 0.234 | 0.302 (0.302 to 0.303) | 0.302 |
| Neuronte 2 | cold | 0.239 | 0.310 (0.305 to 0.315) | 0.335 |
| Neuronte 2 | warm | 0.238 | 0.307 (0.304 to 0.310) | 0.307 |
| DejaVu | cold | 0.253 | 0.326 (0.324 to 0.329) | n/a |
| DejaVu | warm | 0.251 | 0.323 (0.322 to 0.329) | n/a |

Drawing 200 strings (4,329 characters) at each size, regular weight:

| Size | 0.1.1 faces: net time, first use | Neuronte 2: net time, first use | Ratio | Neuronte, each later string | DejaVu, each later string | Ratio |
|---:|---:|---:|---:|---:|---:|---:|
| 8 px | 49 ms | 5 ms | 9.0× | 195 µs | 180 µs | 1.1× |
| 10 px | at start-up | at start-up | — | 250 µs | 187 µs | 1.3× |
| 11 px | at start-up | at start-up | — | 292 µs | 194 µs | 1.5× |
| 12 px | 154 ms | 11 ms | 13.8× | 344 µs | 200 µs | 1.7× |
| 14 px | 206 ms | 14 ms | 15.2× | 442 µs | 206 µs | 2.1× |
| 16 px | 511 ms | 146 ms | 3.5× | 558 µs | 221 µs | 2.5× |
| 20 px | 598 ms | 19 ms | 31.5× | 934 µs | 240 µs | 3.9× |
| 24 px | 830 ms | 24 ms | 34.3× | 1306 µs | 259 µs | 5.0× |
| 32 px | 3293 ms | 162 ms | 20.4× | 2377 µs | 311 µs | 7.6× |
| 48 px (scalable) | 3430 ms | 3383 ms | 1.0× | 5403 µs | 419 µs | 12.9× |

Measuring a string's width at 11 px: Neuronte 4.2 µs, DejaVu 16.5 µs.
