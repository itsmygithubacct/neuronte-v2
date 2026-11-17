# Changes

## Unreleased

- **Font files for programs that cannot run a net.** `neuronte-fontfile`
  (C11, links only the library and libm) writes what the nets draw as five
  TrueType files: Neuronte Sans in four styles from the desktop faces (154
  characters) and Neuronte Mono from the terminal bundles in the weights
  directory (3,205 characters with the files of 2.3.0). Each holds an
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
  eight terminal cells (4.5 MB, 1.1 s on four threads of a Core
  i7-6820HQ); the cells, characters and styles are settings, for all
  bundles or one. A fresh process then has its first 95 ASCII cells at
  9x18 in 0.2 ms in place of 23 ms, and ASCII and Latin-1 in four styles
  in 0.33 ms in place of 407 ms. A program must ask the file to gain
  this; none here does yet. Colour glyphs are not covered.
  [docs/neuronte-cells.md](docs/neuronte-cells.md).
- `make test` also makes the files twice and checks them: the same bytes
  each time, every embedded bitmap exactly the library's pixels (15,616
  Sans glyphs and 1,330 Mono cells, none differs), every outline within
  65 pixels of its net's 8,192, and 95 checks of the setup command. It
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
