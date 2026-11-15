# Neuronte

A neural net that is its own font. A character goes in, as ASCII or as
UTF-8. The net writes that character's pixels into a buffer the caller
owns. No font file, glyph table or bitmap atlas is consulted. The weights
are the font.

This is Neuronte 2: the same library and nets as Neuronte 0.1.2, cut down to
one file per job. Nothing here was retrained or repacked. Every weight file
is byte for byte a file of 0.1.2 under the same name, and the library source
is 0.1.2's. The training code, the weights as JSON, second seeds, scores,
older generations of each net and the research records stay in the 0.1.x
repository, which is where each net's card, recipe and measurements are.

## What is here

| | |
|---|---|
| `src/`, `include/` | The library, C11, no dependencies beyond libm. `make` builds `build/libneuronte.so` and `.a`. `make media` builds the optional FFmpeg decoder library. |
| `weights/` | The packed nets, one per job, and `SHA256SUMS`. [FILES.md](FILES.md) lists each with its size, checksum and the 0.1.1 file it takes over from. |
| `font/` | The desktop faces' drawings (advances and pixels) and `ui-tops.json`, which a desktop needs to place them. |
| `tools/` | The terminal video player and its modules. `train/glyphs.py` and `train/strokes.py` are here only because the player imports them. |
| `tests/` | `make test` checks the whole shipped set. |

## The set

- **Terminal font:** `neuronte-split-v8.nrt`, one bundle for every character.
  No other terminal bundle is shipped.
- **Desktop faces:** one net per size, 1 to 32 px upright and 3 to 32 px
  italic, each the fastest net that draws its size, and
  `neuronte-ui-large-v0.nrt` above 32 px. Each size has its own net because
  the sizes are separately drawn pixel designs; no net draws another size's
  design.
- **Colour:** `neuronte-emoji-v1.nrt`, `neuronte-icon-v2.nrt`,
  `neuronte-flame-v1.nrt`, `neuronte-pointer-v1.nrt`.
- **Video:** `neuronte-video-masks-v0.nrt`, the file the default pixels mode
  draws with.

## What differs from 0.1.2 in use

- A consumer names these files; the `-v0` desktop faces, `neuronte-split-v5`
  and the other older files are not here.
- The video player's cells mode draws with `neuronte-split-v8`. The learned
  video helpers are bound to `neuronte-split-v5` and are not shipped, so
  `--helper` is not available.
- `neuronte-icon-v2` is less exact than `neuronte-icon-v1` at 12 px and equal
  at 16 px.

## The check

`make test` verifies every checksum, draws every native desktop face at its
size, regular and bold, and compares it with its drawing (at most 2 wrong
pixels a face, none on a digit, every advance exact), and compares every
packed file's pixels over a fixed character list with `tests/golden.json`,
which was recorded when the set was cut.

## Licence

MIT; see [LICENSE](LICENSE). The optional media library links FFmpeg; see
[THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md).
