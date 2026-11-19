# Changes

## 2.6.0

Every file of 2.5.1 is still here byte for byte and draws the same pixels
and advances; `tests/golden.json` keeps all 52 of its entries unchanged.
The Neuronte Sans and Neuronte Mono files `neuronte-fontfile` makes are
the same bytes as 2.5.1 made (`tests/fontfile-golden.txt`). The set is 124
files, 144,108,937 bytes, from 66 files, 72,026,486 bytes.

- **The library is Neuronte 0.1.7's.** `src/neuronte.c` and
  `include/neuronte.h` are 0.1.7's, byte for byte: advance tables in
  thousandths of an em (version 4 at 100 units per em, or version 8 with
  a shift of each character's drawing that never moves the pen),
  `neuronte_advance_milli()`, `neuronte_advance_shift()`,
  `neuronte_text_width_exact()` and `neuronte_draw_text_exact()`. Every
  existing call is as it was, and the soname is still `libneuronte.so.0`.
- **Neuronte Sans Metric**, 48 new faces: the desktop face redrawn to
  Helvetica's widths, so a line is as long as in Helvetica or Arial.
  `neuronte-metric-sansN-v1` (1 to 16 px), `-italic-v1` (3 to 16 px), a
  drawn bold `-bold-v1` and `-bold-italic-v1` (9 to 16 px; draw them with
  `bold` = 0), `neuronte-metric-sans-scaled-v1` (17 to 32 px) and
  `-large-v1` (33 px up), each 192 characters. Its advances are numbers
  copied from Adobe's Core 14 AFM files for Helvetica: `PROVENANCE.md`
  and `THIRD_PARTY_NOTICES.md` say so and are scoped to it,
  `LICENSES/Adobe-Core14-AFM.txt` carries Adobe's notice, and
  `font/metric-sans-widths.txt` holds the numbers. The scaled and large
  faces FAILED their declared gate (G-M1) and ship by the owner's ruling.
- **Neuronte Sans Metric font files.** `neuronte-fontfile` writes
  `NeuronteSansMetric-Regular.ttf`, `-Bold`, `-Italic` and `-BoldItalic`,
  family "Neuronte Sans Metric", when the weights hold its faces. Each
  advance is `neuronte_advance_milli()` × UPM / 1000 from the weight's own
  column (Helvetica-Bold's widths in Bold), and a version 8 shift is in
  the left side bearing of the outline and in each strike glyph; the
  strikes of Bold from 9 to 16 px are the drawn bold faces. A new setup
  step, `22-fontfile-metric`, makes them after the Sans files and does
  nothing when the weights have no Metric face. `66-neuronte.conf` adds
  the family weakly, at the end, for a program that asks for Arial,
  Helvetica, Liberation Sans or Arimo, so any of those fonts that is
  installed is still chosen first. Without `--faces`, a weights directory
  with no Metric face gives the same options and files as 2.5.1. Neuronte
  Sans keeps its advances (the regular column and the library's
  1/32 em bold).
- **Wide characters**, five bundles: `neuronte-wide-forms-v1`,
  `-symbols-v1`, `-pictographs-v1`, `-pictographs2-v1` and
  `-supplement-v1`, 1,261 characters drawn for the box of two cells a
  terminal gives a wide character, each with its `NAME.symbols.txt`. 95 of
  their 322 nets FAILED their limits and ship by the owner's word, marked
  on their cards; 74 drawn characters are left out. None is a default, and
  Neuronte Mono does not take them.
- **Two-colour ranking for the video player.** `TwoInk.rank()` in
  `tools/neuronte_video.py` gives each cell the candidates with the least
  unconstrained two-colour error with one matrix product, so the full
  mode fits a short list instead of every candidate.
  `tests/check_rank.py`, part of `make test`, checks it against each
  candidate's own normal equations.
- `make test` also checks the Metric font files: their characters, names
  and advances (against `font/metric-sans-widths.txt` as well as the
  library), every strike glyph against the library's draw calls, the
  outlines with their shifts, FreeType's reading of them, and fontconfig's
  choice of family for Arial, Helvetica, Liberation Sans, Arimo and
  `sans-serif` with `66-neuronte.conf` alone (when `fc-match` is there).
  `tests/check_set.py` hashes the Metric scaled and large faces' lines of
  text as it does Neuronte Sans's.

## 2.5.1

- **The Sans files are made in the background at a start.** `early=`
  is `10-backend` as shipped, without `20-fontfile-sans`: a first start
  no longer holds the login screen back while the Sans files are made.
  `neuronte-setup.service` makes them after the login screen has
  started, before Mono; until then the other fonts installed are used, as
  the fontconfig file already prefers them. A machine with no other font
  puts the step back with `early=10-backend,20-fontfile-sans`.
  `tests/check_setup.sh` checks that `--early` makes no font file as
  shipped, and that the setting makes it make the Sans files.
- No weight file, drawing, `src/` or `include/` file changes: the set is
  the 66 files of 2.5.0. Only `system/`, its documentation and its test
  change.

## 2.5.0

- **All of Latin-1 in every desktop face, at every size.** The 33
  desktop files are replaced by files that draw printable ASCII, the
  ellipsis and all 96 characters from U+00A0 to U+00FF, 192 characters:
  `neuronte-ui1-v2` to `neuronte-ui16-v2` and `neuronte-ui3-italic-v2` to
  `neuronte-ui16-italic-v2` in place of the `-v1` faces, which drew ASCII
  only; `neuronte-ui-scaled-latin1-v1` with
  `neuronte-ui-scaled-latin1-italic-advances-v1` in place of their `-v0`,
  and `neuronte-ui-large-latin1-v2` in place of `-v1`, which drew the 58
  accented letters but not the other 38 (`£ ± × ÷ ß Æ ø § © « »` and the
  rest). Each new file is the file it replaces with nets added beside its
  nets, which are unchanged, so ASCII draws byte for byte as before at
  every size, and so do the 58 accented letters from 17 px up. A program
  that opened the old names opens the new ones instead.
- Every new face from 1 to 16 px is exactly its drawing, all 192
  characters, regular and bold, at its size: the gate was set before
  training, both seeds passed it, and six faces passed only after one or
  two groups of four characters were split in two, as declared. No
  character is left out. Below 8 px some characters share a bitmap with
  another; the [card of the faces](https://github.com/itsmygithubacct/neuronte/blob/bfd0a2d066161eaa5abeae9747e151ff345702ef/weights/neuronte-ui-v2.md) lists them. The
  38 of the scaled and large faces passed their limits with both seeds;
  [their card](https://github.com/itsmygithubacct/neuronte/blob/bfd0a2d066161eaa5abeae9747e151ff345702ef/weights/neuronte-ui-latin1-signs-v0.md) gives the numbers. The
  soft hyphen, U+00AD, is drawn as a hyphen.
- The drawings of the faces from 1 to 16 px are now
  `font/ui-N-latin1.txt` and `font/ui-N-latin1-italic.txt`, in place of
  `font/ui-N.txt` and `font/ui-N-italic.txt`: the same ASCII glyphs, byte
  for byte, and the 96 of Latin-1. The advance tables of the scaled and
  large faces are `font/ui-scaled-latin1-full-advances.txt`,
  `font/ui-scaled-latin1-full-italic-advances.txt` and
  `font/ui-large-latin1-full-advances.txt`, the earlier tables with the
  38 added.
- **Neuronte Sans has 192 characters**, from 154, each with a strike at
  every size from 1 to 32 px (3 to 32 px italic), where Latin-1 had no
  strike below 17 px. `neuronte-fontfile` takes the newest face of each
  name and is unchanged. The four Sans files are 866,856 bytes, from
  646,684, and take 2.38 s to make with four threads on a Core i7-6820HQ,
  from 1.76 s, which is what a first start holds the login screen back
  by. Neuronte Mono is unchanged.
- `make test` draws all 192 characters of every face against its
  drawing (11,520 letters, none wrong), and compares 23,808 Sans strike
  glyphs with the library's pixels. In `tests/golden.json` the entries of
  the 33 replaced files are renamed; for 32 of them the coverage hash and
  the number of nets change, because the list of characters it hashes
  holds eight of Latin-1 (U+00A0, U+00A3, U+00B1, U+00B2, U+00BC, U+00C9,
  U+00E9, U+00F1): the faces from 1 to 16 px did not draw any of them,
  and the scaled and large faces did not draw the four signs. Every other character of that list draws as before, and the
  advance and text hashes, which are of ASCII, are unchanged; so is every
  entry of the files that are not desktop faces.
- Each new file is a file of Neuronte at commit
  [bfd0a2d](https://github.com/itsmygithubacct/neuronte/tree/bfd0a2d066161eaa5abeae9747e151ff345702ef);
  every other file is unchanged from 2.4.0. `src/` and `include/` are
  unchanged. The set is 66 weight files, 72,026,486 bytes, from 66 and
  56,148,762.
- Documents: [docs/neuronte-and-dejavu.md](docs/neuronte-and-dejavu.md)
  now describes the set as it is, not 2.1.0's; and
  [docs/neuronte-fontfile.md](docs/neuronte-fontfile.md) no longer says
  that Hebrew is in no net (Neuronte Mono has 36 characters of the Hebrew
  block, 45 Hebrew presentation forms and 39 Lao characters, from
  `neuronte-term-rest-v1`).

## 2.4.0

- **1,714 more terminal characters**, 4,919 with `neuronte-split-v9` and
  the four bundles of 2.3.0, in six more bundles opened the same way:
  `neuronte-term-letters-v1` (812 Latin, Greek and Cyrillic letters),
  `neuronte-term-blocks-v1` (310 hexagrams, Tai Xuan Jing figures, domino
  tiles and playing cards), `neuronte-term-mathalpha-v1` (179 letters and
  digits of three mathematical alphabets), `neuronte-term-syllabics-v1`
  (168 Canadian syllabics), `neuronte-term-scripts-v1` (the 48 characters
  of Lisu) and `neuronte-term-rest-v1` (197 Hebrew and Lao characters,
  faces and moons). Nothing an earlier file draws has changed. Every net
  in them passed its limits with both seeds; the limits were amended while
  the nets were scored, and four letters whose group failed twice are left
  out. The cards in Neuronte 0.1.6 say how.
- **Symbols for a terminal's own bars**: `neuronte-kilix-chrome-v1` (39
  symbols, one cell), `neuronte-kilix-chrome-v2` (41 symbols for a box of
  two cells) and `neuronte-weather-colour-v1` (nine weather symbols in
  colour, two cells). The [README](README.md#symbols-for-a-terminals-bars)
  shows how each is asked.
- Every weight file is now a file of Neuronte 0.1.6; the 48 that were here
  before are unchanged. `src/neuronte.c` and `include/neuronte.h` are
  unchanged, and `src/cells.c` and `include/neuronte_cells.h` are 0.1.6's
  byte for byte.
- **Font files for programs that cannot run a net.** `neuronte-fontfile`
  (C11, links only the library and libm) writes what the nets draw as five
  TrueType files: Neuronte Sans in four styles from the desktop faces (154
  characters) and Neuronte Mono from the terminal bundles in the weights
  directory (4,958 characters with the files of 2.4.0: text first, then
  a terminal's one-cell symbols; a bundle drawn for two cells or in
  colour is left out). Each holds an
  outline of every character, traced from the net's coverage, and the nets'
  own pixels as embedded bitmaps for the small sizes (1 to 32 px for Sans).
  A file of bitmaps alone is rejected by Debian's fontconfig and Chromium
  then does not start; outlines alone do not look like the desktop faces
  below 33 px. Tried in Chromium 156, GTK 3 and 4, Qt 6 and FreeType.
  No kerning, shaping, hinting or serif face; nothing is read from any
  font. [docs/neuronte-fontfile.md](docs/neuronte-fontfile.md).
- **`neuronte-setup`**, run at install and at start, makes those files
  when they are absent or the weights changed, through a list of steps
  that others can join (`system/`). A start that finds them current costs
  14 ms; a first start makes the Sans files before the login screen
  (1.8 s on a Core i7-6820HQ) and Mono afterwards in the background.
  `make install-setup` installs it with its systemd units and a fontconfig
  file that makes the two families the last choice for `sans-serif` and
  `monospace`, after any font that is installed. A step may defer long
  work at a start to the run after the login screen, and `neuronte-setup
  --user` runs steps that must be done as each user.
- **Cells files for programs that call the library.** A new part of the
  library, `include/neuronte_cells.h` and `src/cells.c`
  (`neuronte_cells_open`, `_find`, `_count`, `_entry`, `_close`,
  `_write`), keeps coverage exactly as `neuronte_coverage_styled()`
  writes it in a file that every process maps read-only.
  `neuronte-cells`, run by `neuronte-setup` as the step `30-cells`,
  draws ASCII and Latin-1 of the newest split bundle in four styles at
  eight terminal cells, all the symbols of the `neuronte-kilix-*`
  bundles at Kilix's cells (6.1 MB together, 1.3 s on four threads of a
  Core i7-6820HQ), and the status marks of a terminal's bars (the idle dot,
  the moons, the sun and the cloud) for each bundle trained on them; the
  cells, characters, styles and marks are settings, for all bundles or
  one. A fresh process then has its first 95 ASCII cells at
  9x18 in 0.2 ms in place of 23 ms, and ASCII and Latin-1 in four styles
  in 0.33 ms in place of 407 ms. A program must ask the file to gain
  this; none here does yet. Colour glyphs are not covered.
  [docs/neuronte-cells.md](docs/neuronte-cells.md).
- `make test` also makes the files twice and checks them: the same bytes
  each time, every embedded bitmap exactly the library's pixels (15,616
  Sans glyphs and 1,330 Mono cells, none differs), every outline within
  65 pixels of its net's 8,192, and 111 checks of the setup command. It
  compares every entry of a cells file with the library bit for bit and
  opens some 11,000 damaged cells files, also under AddressSanitizer.
- `src/neuronte.c`, `include/neuronte.h` and every weight file are
  unchanged.

## 2.3.0

- **1,864 more terminal characters**, 3,205 with `neuronte-split-v9`, in
  four bundles opened beside it: `neuronte-term-latin-v1` (596 Latin and
  Greek letters), `neuronte-term-math-v1` (465 arrows and operators),
  `neuronte-term-symbol-v1` (480 punctuation, letterlike, technical and
  geometric characters) and `neuronte-term-dingbat-v1` (323 symbols and
  dingbats). A program draws a character with the first bundle whose
  `neuronte_untrained()` is 0; the [README](README.md#more-terminal-characters)
  shows how. Nothing split-v9 draws has changed.
- Of their 204 nets, 58 passed the limits set before training with both
  seeds; 146 missed at least one and ship by the owner's choice after a
  side-by-side review. The cards in Neuronte 0.1.5 say where.
- Each of the four has `weights/NAME.list.txt`, the characters it was
  trained on, and `make test` checks each bundle against its list, that no
  two share a character, and that none repeats a character of split-v9.
- The library is unchanged. Every weight file is now a file of Neuronte
  0.1.5; the forty that were here before are unchanged.

## 2.2.0

- **165 more terminal characters.** `neuronte-split-v9` takes the place of
  `neuronte-split-v8`: check and cross marks, arrows, bullets and shapes,
  `± ≤ ≥ ≠`, currency, superscripts and fractions, math operators, Greek
  with tonos and the Cyrillic letters beyond Russian. Every character
  split-v8 drew is drawn the same. A program that opened
  `neuronte-split-v8.nrt` opens `neuronte-split-v9.nrt` instead. All
  fourteen new nets missed the limits set for them before training and
  ship by the owner's choice; the card says where.
- **Untrained characters can be told apart.** `neuronte_untrained()` says
  whether a bundle was trained on a character, and
  `neuronte_set_untrained()` draws a mark in place of one it was not. Off
  unless asked for.
- The library is now the source of Neuronte 0.1.4, and every weight file
  is a file of 0.1.4.
- Documents, after the release: [BENCHMARKS.md](BENCHMARKS.md) gains a
  three-way timing of terminal cells (Neuronte 2.2.0, Neuronte 0.1.4,
  DejaVu) with the character counts, and says which desktop rows describe
  2.1.0.

## 2.1.1

- **One scalable face for 17 to 32 px**, `neuronte-ui-scaled-latin1-v0`, in place
  of 32 hand-drawn files: 2.5 MB where they were 17.4 MB. Its stems are
  snapped to whole pixels by the library. Italic is the same letters
  sheared, with `neuronte-ui-scaled-latin1-italic-advances-v0`. It is a different
  face from the ones it replaces: about a third of the letter widths at
  these sizes change, by at most 2 px, so a program that laid text out
  with the old widths should lay it out again. Sizes 1 to 16 px are
  unchanged.
- **A faster face above 32 px**, `neuronte-ui-large-latin1-v1`, in place of
  `neuronte-ui-large-v0`: about 13 times as fast the first time a size is
  used, with the same advances. It missed the accuracy limit set for it
  and ships by the owner's decision; its card says where.
- The library reads a drawing file's table of letter edges (format
  version 7). A file without one draws as before, and every file kept
  from 2.1.0 draws the same pixels. A 2.1.0 library refuses the new
  scaled file rather than draw it wrongly.
- **Latin-1 letters from 17 px up.** Both new faces draw the 58 accented
  letters from U+00C0 to U+00FF as well as ASCII. The faces from 1 to
  16 px still draw ASCII only.
- The set is 40 files and 32.0 MB of weights, from 70 files and 46.0 MB.

- `make demo` and `demo/neuronte-say.c`: a line of text drawn by the
  desktop faces and shown in the terminal, with nothing to install. `-o`
  also writes the pixels as an image.
- [Neuronte 2 and DejaVu Sans](docs/neuronte-and-dejavu.md), an article
  on how the two differ, with a side-by-side picture.

## 2.1.0

- **Packed text masks** (`src/bits.c`): `neuronte_bits_pack()`,
  `neuronte_alpha_pack()` and `neuronte_bits_compose()`. A program that
  keeps each letter after the net has drawn it once can lay a line out of
  kept letters in one call. With them the Kilix 95 desktop draws a string
  in less time than DejaVu through FreeType at every size measured.
- The video player's `--helper` option is gone. Its helper models are tied
  to a file this set does not ship.
- [BENCHMARKS.md](BENCHMARKS.md) and `bench/`: a desktop drawn with this
  set, with the faces of Neuronte 0.1.1, and with DejaVu Sans.

## 2.0.0

The first slim set: the library of Neuronte 0.1.2 and one packed net for
each job, 70 weight files, every one a file of 0.1.2 byte for byte.
