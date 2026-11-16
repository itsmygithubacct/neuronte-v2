# Changes

## After 2.1.0, not yet numbered

- `make demo` and `demo/neuronte-say.c`: a line of text drawn by the
  desktop faces and shown in the terminal, with nothing to install. `-o`
  also writes the pixels as an image.
- [Neuronte 2 and DejaVu Sans](docs/neuronte-and-dejavu.md), an article
  on how the two differ, with a side-by-side picture.
- No weight file and no library source changed.

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
