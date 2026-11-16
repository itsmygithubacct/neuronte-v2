# Changes

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
