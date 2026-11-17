"""Check the font files tools/neuronte-fontfile.c writes.

usage: python3 tests/check_fontfile.py build/libneuronte.so FONTS_DIR [SECOND_DIR]

The files are read here with a reader written for this test, from the
OpenType specification; no font library is asked what they hold.

1. Every file is a whole sfnt: the directory is sorted, every table's
   checksum and the file's are right, and the tables a TrueType font needs
   are there. glyf parses, every glyph's stated box is the box of its
   points, and maxp counts what glyf holds.
2. The characters are the nets': a Sans file maps exactly the characters the
   scalable desktop face was trained on and has advances for, and the Mono
   file exactly the characters the terminal bundles were trained on. The
   advances are the nets' (Sans) and 0.6 em (Mono).
3. Strikes. Every glyph of every strike is compared, pixel by pixel and
   advance by advance, with what the library's draw calls give for that
   character at that size: neuronte_draw_text() for the upright and bold
   faces and for the italic nets, neuronte_draw_scalar_styled() for the
   sheared italic of 17 px and up, neuronte_coverage() for a terminal cell.
   The limit is 0 pixels and 0 advances. Below 17 px the baseline row is
   taken from the face's drawing (font/ui-EM.txt), not from the generator's
   rule, so a strike set a row off fails.
4. Outlines. Each outline of Sans Regular, Sans Bold and Mono is filled at
   the size it was traced at and compared with the net's coverage there,
   cut at one half. A glyph may differ in at most OUTLINE_LIMIT of the
   cell's pixels; the worst and the mean are printed.
5. With Pillow built on FreeType: FreeType opens each file and gives back
   the strike's pixels for ASCII at three sizes. Skipped, and said so, when
   Pillow is not installed.
6. The stamps (FONTS_DIR/neuronte-fontfile.stamp, or the font steps'
   beside FONTS_DIR) name every weight file the faces are made from and
   every written file, each with its SHA-256 and size. With SECOND_DIR, a
   second run's files are the same bytes (the generator puts no clock and
   no thread order in its output).
"""
import ctypes
import hashlib
import os
import re
import struct
import sys

import numpy as np

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import check_set  # noqa: E402
from check_set import ROOT, glyphs, open_file  # noqa: E402

WEIGHTS = os.path.join(ROOT, "weights")
UPM = 1000
TRACE = 64                 # the em, and the terminal cell's width, the outlines are traced at
OUTLINE_LIMIT = 0.008      # of a cell's pixels: 65 of the 8,192 of a 64 x 128 cell
SANS = {"NeuronteSans-Regular.ttf": (0, 0), "NeuronteSans-Bold.ttf": (1, 0),
        "NeuronteSans-Italic.ttf": (0, 1), "NeuronteSans-BoldItalic.ttf": (1, 1)}
MONO = "NeuronteMono-Regular.ttf"
MONO_SIZES = (9, 12, 13, 15, 16, 17, 20)
MONO_CHARS = set(range(0x20, 0x7F)) | set(range(0xA0, 0x100))
BOLD, ITALIC = 1, 2

fails = []


def fail(text):
    fails.append(text)
    print("FAIL " + text)


def newest(prefix):
    best = None
    for name in os.listdir(WEIGHTS):
        m = re.fullmatch(re.escape(prefix) + r"(\d+)\.nrt", name)
        if m and (best is None or int(m.group(1)) > best[0]):
            best = (int(m.group(1)), name)
    return best[1] if best else None


# ---- the reader ----

def table_sum(data):
    data = data + b"\0" * (-len(data) % 4)
    return sum(struct.unpack(">%dI" % (len(data) // 4), data)) & 0xFFFFFFFF


class Font:
    def __init__(self, path):
        self.name = os.path.basename(path)
        with open(path, "rb") as fh:
            raw = self.raw = fh.read()
        version, count = struct.unpack(">IH", raw[:6])
        if version != 0x00010000:
            raise ValueError("not a TrueType sfnt")
        self.tables, tags = {}, []
        for i in range(count):
            tag, check, off, length = struct.unpack(">4sIII", raw[12 + 16 * i:28 + 16 * i])
            tag = tag.decode("latin-1")
            tags.append(tag)
            if off % 4 or off + length > len(raw):
                raise ValueError(f"table {tag} is misplaced")
            body = raw[off:off + length]
            if tag == "head":
                body = body[:8] + b"\0\0\0\0" + body[12:]
            if table_sum(body) != check:
                raise ValueError(f"table {tag} has a wrong checksum")
            self.tables[tag] = raw[off:off + length]
        if tags != sorted(tags):
            raise ValueError("the directory is not sorted")
        adjust = struct.unpack(">I", self.tables["head"][8:12])[0]
        zeroed = raw.replace(self.tables["head"], self.tables["head"][:8] + b"\0\0\0\0" + self.tables["head"][12:], 1)
        if (0xB1B0AFBA - table_sum(zeroed)) & 0xFFFFFFFF != adjust:
            raise ValueError("the file's checksum is wrong")
        for tag in ("cmap", "glyf", "head", "hhea", "hmtx", "loca", "maxp", "name", "post", "OS/2"):
            if tag not in self.tables:
                raise ValueError(f"no {tag} table")
        head = self.tables["head"]
        if struct.unpack(">I", head[12:16])[0] != 0x5F0F3CF5:
            raise ValueError("head has no magic number")
        self.upm = struct.unpack(">H", head[18:20])[0]
        self.mac_style = struct.unpack(">H", head[44:46])[0]
        if struct.unpack(">h", head[50:52])[0] != 1:
            raise ValueError("loca is expected long")
        self.count = struct.unpack(">H", self.tables["maxp"][4:6])[0]
        self.max_points, self.max_contours = struct.unpack(">HH", self.tables["maxp"][6:10])
        hhea = self.tables["hhea"]
        self.ascender, self.descender, self.line_gap = struct.unpack(">hhh", hhea[4:10])
        if struct.unpack(">H", hhea[34:36])[0] != self.count:
            raise ValueError("hmtx does not hold every glyph")
        hmtx = struct.unpack(">" + "Hh" * self.count, self.tables["hmtx"][:4 * self.count])
        self.advance, self.lsb = hmtx[0::2], hmtx[1::2]
        self.fixed_pitch = struct.unpack(">I", self.tables["post"][12:16])[0]
        self.selection = struct.unpack(">H", self.tables["OS/2"][62:64])[0]
        self.weight = struct.unpack(">H", self.tables["OS/2"][4:6])[0]
        self._cmap()
        self._names()
        self._glyf()
        self._strikes()

    def _cmap(self):
        t = self.tables["cmap"]
        self.cmap, formats = {}, set()
        for i in range(struct.unpack(">H", t[2:4])[0]):
            platform, encoding, off = struct.unpack(">HHI", t[4 + 8 * i:12 + 8 * i])
            fmt = struct.unpack(">H", t[off:off + 2])[0]
            formats.add((platform, encoding, fmt))
            found = {}
            if fmt == 4:
                seg = struct.unpack(">H", t[off + 6:off + 8])[0] // 2
                ends = struct.unpack(">%dH" % seg, t[off + 14:off + 14 + 2 * seg])
                at = off + 16 + 2 * seg
                starts = struct.unpack(">%dH" % seg, t[at:at + 2 * seg])
                deltas = struct.unpack(">%dH" % seg, t[at + 2 * seg:at + 4 * seg])
                offsets = struct.unpack(">%dH" % seg, t[at + 4 * seg:at + 6 * seg])
                if ends[-1] != 0xFFFF or any(offsets) or list(ends) != sorted(ends):
                    raise ValueError("cmap format 4 is not as written")
                for a, b, d in zip(starts[:-1], ends[:-1], deltas[:-1]):
                    for c in range(a, b + 1):
                        found[c] = (c + d) & 0xFFFF
            elif fmt == 12:
                groups = struct.unpack(">I", t[off + 12:off + 16])[0]
                for g in range(groups):
                    a, b, first = struct.unpack(">III", t[off + 16 + 12 * g:off + 28 + 12 * g])
                    for c in range(a, b + 1):
                        found[c] = first + c - a
            else:
                raise ValueError(f"cmap format {fmt}")
            for c, g in found.items():
                if self.cmap.setdefault(c, g) != g:
                    raise ValueError("two cmap subtables disagree")
        if (3, 1, 4) not in formats:
            raise ValueError("no Windows Unicode cmap")
        if max(self.cmap) > 0xFFFF and (3, 10, 12) not in formats:
            raise ValueError("characters beyond U+FFFF and no format 12 cmap")
        if any(g <= 0 or g >= self.count for g in self.cmap.values()):
            raise ValueError("cmap names a glyph that is not there")

    def _names(self):
        t = self.tables["name"]
        count, strings = struct.unpack(">HH", t[2:6])
        self.names = {}
        for i in range(count):
            platform, encoding, lang, name_id, length, off = struct.unpack(">6H", t[6 + 12 * i:18 + 12 * i])
            text = t[strings + off:strings + off + length]
            if (platform, encoding, lang) == (3, 1, 0x409):
                self.names[name_id] = text.decode("utf-16-be")

    def _glyf(self):
        loca = struct.unpack(">%dI" % (self.count + 1), self.tables["loca"][:4 * self.count + 4])
        glyf = self.tables["glyf"]
        self.contours, self.boxes = [], []
        most_points = most_contours = 0
        for g in range(self.count):
            data = glyf[loca[g]:loca[g + 1]]
            if not data:
                self.contours.append([])
                self.boxes.append(None)
                continue
            n, x0, y0, x1, y1 = struct.unpack(">hhhhh", data[:10])
            if n <= 0:
                raise ValueError(f"glyph {g}: not a simple glyph")
            ends = struct.unpack(">%dH" % n, data[10:10 + 2 * n])
            at = 10 + 2 * n
            if struct.unpack(">H", data[at:at + 2])[0]:
                raise ValueError(f"glyph {g} has instructions")
            at += 2
            points = ends[-1] + 1
            flags = list(data[at:at + points])
            at += points
            if any(f & ~0x37 or not f & 1 for f in flags):
                raise ValueError(f"glyph {g}: a flag this file should not use")
            xs, ys, v = [], [], 0
            for f in flags:
                if f & 0x02:
                    v += data[at] if f & 0x10 else -data[at]
                    at += 1
                elif not f & 0x10:
                    v += struct.unpack(">h", data[at:at + 2])[0]
                    at += 2
                xs.append(v)
            v = 0
            for f in flags:
                if f & 0x04:
                    v += data[at] if f & 0x20 else -data[at]
                    at += 1
                elif not f & 0x20:
                    v += struct.unpack(">h", data[at:at + 2])[0]
                    at += 2
                ys.append(v)
            if at > len(data) or len(data) - at > 3:
                raise ValueError(f"glyph {g}: its data and its length disagree")
            if (min(xs), min(ys), max(xs), max(ys)) != (x0, y0, x1, y1):
                raise ValueError(f"glyph {g}: its box is not the box of its points")
            if self.lsb[g] != x0:
                raise ValueError(f"glyph {g}: its left side bearing is not its box's left")
            cs, start = [], 0
            for e in ends:
                cs.append(np.array(list(zip(xs[start:e + 1], ys[start:e + 1])), float))
                start = e + 1
            self.contours.append(cs)
            self.boxes.append((x0, y0, x1, y1))
            most_points = max(most_points, points)
            most_contours = max(most_contours, n)
        if (most_points, most_contours) != (self.max_points, self.max_contours):
            raise ValueError("maxp does not count what glyf holds")

    def _strikes(self):
        self.strikes = {}
        if "EBLC" not in self.tables:
            return
        loc, dat = self.tables["EBLC"], self.tables["EBDT"]
        if struct.unpack(">HH", loc[:4]) != (2, 0) or struct.unpack(">HH", dat[:4]) != (2, 0):
            raise ValueError("EBLC or EBDT is not version 2.0")
        for s in range(struct.unpack(">I", loc[4:8])[0]):
            rec = loc[8 + 48 * s:56 + 48 * s]
            array, _size, subs = struct.unpack(">III", rec[:12])
            first, last, ppem_x, ppem_y, depth, flags = struct.unpack(">HHBBBb", rec[40:48])
            if ppem_x != ppem_y or flags != 1 or depth not in (1, 8):
                raise ValueError("a strike is not as written")
            cells = {}
            for i in range(subs):
                a, b, more = struct.unpack(">HHI", loc[array + 8 * i:array + 8 * i + 8])
                sub = array + more
                index_format, image_format, image = struct.unpack(">HHI", loc[sub:sub + 8])
                if (index_format, image_format) != (1, 1):
                    raise ValueError("a strike uses another index or image format")
                offs = struct.unpack(">%dI" % (b - a + 2), loc[sub + 8:sub + 8 + 4 * (b - a + 2)])
                for g in range(a, b + 1):
                    data = dat[image + offs[g - a]:image + offs[g - a + 1]]
                    h, w, bx, by, adv = struct.unpack(">BBbbB", data[:5])
                    row = (w + 7) // 8 if depth == 1 else w
                    if len(data) != 5 + row * h:
                        raise ValueError("a bitmap and its metrics disagree")
                    bits = np.frombuffer(data[5:], np.uint8).reshape(h, row)
                    if depth == 1:
                        bits = np.unpackbits(bits, axis=1)[:, :w] * np.uint8(255)
                    cells[g] = (bx, by, adv, bits)
            if cells and (min(cells), max(cells)) != (first, last):
                raise ValueError("a strike's first and last glyph are not its glyphs")
            self.strikes[ppem_x] = (depth, cells)


# ---- the library ----

def library(path):
    lib = check_set.load(path)
    lib.neuronte_untrained.argtypes = [ctypes.c_void_p, ctypes.c_uint32]
    lib.neuronte_set_untrained.argtypes = [ctypes.c_void_p, ctypes.c_int]
    lib.neuronte_advance_scalar.argtypes = [ctypes.c_void_p, ctypes.c_uint32]
    lib.neuronte_advance_scalar.restype = ctypes.c_uint32
    lib.neuronte_advance_steps.argtypes = [ctypes.c_void_p, ctypes.c_uint32, ctypes.c_int]
    lib.neuronte_coverage.argtypes = [ctypes.c_void_p, ctypes.c_uint32, ctypes.c_int, ctypes.c_int,
                                      ctypes.c_int, ctypes.c_void_p]
    lib.neuronte_coverage_styled.argtypes = [ctypes.c_void_p, ctypes.c_uint32, ctypes.c_int, ctypes.c_int,
                                             ctypes.c_int, ctypes.c_uint, ctypes.c_void_p]
    lib.neuronte_draw_scalar_styled.argtypes = ([ctypes.c_void_p, ctypes.c_void_p] + [ctypes.c_int] * 7
                                                + [ctypes.c_uint32, ctypes.c_uint32, ctypes.c_uint])
    return lib


def face_chars(lib, h):
    """The characters a desktop face draws: trained on, and in its advance table."""
    out = []
    for s in list(range(0x20, 0x3000)) + [0xFFFD]:
        if lib.neuronte_untrained(h, s) == 0 and lib.neuronte_advance_scalar(h, s) == s:
            out.append(s)
    return out


def utf8(scalar):
    return chr(scalar).encode("utf-8")


def cell_of(bits, bx, by, base, x0, shape):
    """A strike's bitmap set down in a buffer whose pen is at column x0."""
    out = np.zeros(shape, np.uint8)
    h, w = bits.shape
    top, left = base - by, x0 + bx
    if top < 0 or left < 0 or top + h > shape[0] or left + w > shape[1]:
        return None
    out[top:top + h, left:left + w] = bits
    return out


def drawing_base(em):
    """The row under the baseline in the cell, from the face's drawing: the
    drawing names the last row of a letter that stands on the line."""
    with open(os.path.join(ROOT, "font", f"ui-{em}.txt"), encoding="utf-8") as fh:
        words = []
        for line in fh:
            if not line.startswith("#"):
                break
            words.append(line[1:].strip())
    m = re.search(r"The baseline is row (\d+)", " ".join(" ".join(words).split()))
    return glyphs.UI_TOP[em] + int(m.group(1)) + 1


def sans_strikes(lib, font, bold, italic, handles):
    compared = wrong = pens = missing = 0
    for em in range(1, 33):
        if em <= 16:
            name = newest(f"neuronte-ui{em}-italic-v" if italic else f"neuronte-ui{em}-v")
        else:
            name = handles["scaled_name"]
        if not name:
            if em in font.strikes:
                fail(f"{font.name}: a {em} px strike and no net for it")
            continue
        if em not in font.strikes:
            fail(f"{font.name}: no {em} px strike")
            continue
        depth, cells = font.strikes[em]
        if depth != 1:
            fail(f"{font.name}: the {em} px strike is not 1 bit")
        h = handles.setdefault(name, open_file(lib, os.path.join(WEIGHTS, name)))
        lib.neuronte_set_binary(h, 1)
        slant = handles["slant"] if em > 16 and italic else None
        W, H, X = 3 * em + 8, 2 * em, em + 2
        if em <= 16:
            base = drawing_base(em)
        else:  # no drawing for these sizes: the row under the lowest ink of the upright H
            buf = np.zeros((H, W), np.uint32)
            lib.neuronte_draw_text(h, buf.ctypes.data, W, W, H, X, 0, em, 0, b"H", 1, 0xFFFFFFFF)
            base = int(np.nonzero(buf.any(axis=1))[0][-1]) + 1
        chars = face_chars(lib, h)
        if slant:  # that file holds the leant letters' advances only
            chars = [c for c in chars if lib.neuronte_advance_scalar(slant, c) == c]
        for c in chars:
            g = font.cmap.get(c)
            if g is None or g not in cells:
                missing += 1
                continue
            bx, by, adv, bits = cells[g]
            buf = np.zeros((H, W), np.uint32)
            if slant:
                rc = lib.neuronte_draw_scalar_styled(h, buf.ctypes.data, W, W, H, X, 0, em, 2 * em, c,
                                                     0xFFFFFFFF, ITALIC | (BOLD if bold else 0))
                pen = lib.neuronte_advance(slant, c, bold, em)
            else:
                text = utf8(c)
                rc = pen = lib.neuronte_draw_text(h, buf.ctypes.data, W, W, H, X, 0, em, bold, text, len(text),
                                                  0xFFFFFFFF)
            if rc < 0:
                fail(f"{font.name}: the library did not draw U+{c:04X} at {em} px")
                continue
            got = cell_of(bits, bx, by, base, X, (H, W))
            compared += 1
            if got is None:
                wrong += 1
                continue
            wrong += int(((got != 0) != (buf != 0)).sum())
            pens += adv != pen
        extra = set(cells) - {font.cmap[c] for c in chars if c in font.cmap} - {0}
        if extra:
            fail(f"{font.name}: {len(extra)} glyphs in the {em} px strike that the net does not draw")
    return compared, wrong, pens, missing


def mono_cell(ppem):
    return max(2, (6 * ppem + 5) // 10)


def mono_strikes(lib, font, source):
    compared = wrong = 0
    if tuple(sorted(font.strikes)) != MONO_SIZES:
        fail(f"{font.name}: strikes at {sorted(font.strikes)}, expected {MONO_SIZES}")
    for ppem, (depth, cells) in sorted(font.strikes.items()):
        w = mono_cell(ppem)
        h = 2 * w
        base = int(np.floor(font.ascender / (font.ascender - font.descender) * h + 0.5))
        want_glyphs = {font.cmap[c] for c in MONO_CHARS if c in font.cmap} | {0}
        if set(cells) != want_glyphs or depth != 8:
            fail(f"{font.name}: the {ppem} px strike does not hold ASCII and Latin-1 as 8-bit cells")
        for c in sorted(MONO_CHARS & set(font.cmap)):
            bx, by, adv, bits = cells.get(font.cmap[c], (0, 0, 0, None))
            if bits is None:
                continue
            cov = np.zeros((h, w), np.float32)
            lib.neuronte_coverage(source[c], c, w, h, 0, cov.ctypes.data)
            # The terminal's own rounding of a coverage to a byte.
            want = np.where(cov <= 0, 0, np.where(cov >= 1, 255, np.floor(cov * np.float32(255) + np.float32(0.5))))
            got = cell_of(bits, bx, by, base, 0, (h, w))
            compared += 1
            if got is None or adv != w:
                wrong += 1
                continue
            wrong += int((got != want.astype(np.uint8)).sum())
    return compared, wrong


# ---- outlines ----

def fill(contours, w, h, unit, base):
    """Pixels whose centres the outline encloses, by the nonzero winding rule."""
    wind = np.zeros((h, w + 1), np.int32)
    ys = np.arange(h) + 0.5
    for pts in contours:
        if len(pts) < 3:
            continue
        x = pts[:, 0] / unit
        y = base - pts[:, 1] / unit
        x2, y2 = np.roll(x, -1), np.roll(y, -1)
        for xa, ya, xb, yb in zip(x, y, x2, y2):
            if ya == yb:
                continue
            lo, hi = (ya, yb) if ya < yb else (yb, ya)
            rows = np.nonzero((ys >= lo) & (ys < hi))[0]
            if not len(rows):
                continue
            cross = xa + (ys[rows] - ya) * (xb - xa) / (yb - ya)
            cols = np.clip(np.ceil(cross - 0.5), 0, w).astype(int)
            np.add.at(wind, (rows, cols), 1 if yb > ya else -1)
    return np.cumsum(wind, axis=1)[:, :w] != 0


def edge_under(cov):
    """The row, as a fraction, where the lowest ink of a cell ends: the half
    coverage line between the last inked row and the one under it."""
    rows = np.nonzero((cov > 0.5).any(axis=1))[0]
    y = int(rows[-1])
    best = 0.0
    for x in np.nonzero(cov[y] > 0.5)[0]:
        a = float(cov[y, x]) - 0.5
        b = (float(cov[y + 1, x]) if y + 1 < cov.shape[0] else 0.0) - 0.5
        best = max(best, y + 0.5 + a / (a - b))
    return best


def outlines(lib, font, source, style, w, unit, label):
    h = 2 * w
    cov = np.zeros((h, w), np.float32)
    lib.neuronte_coverage_styled(source[ord("H")], ord("H"), w, h, 1, 0, cov.ctypes.data)
    base = edge_under(cov)
    worst, worst_c, total, count, over = 0, 0, 0, 0, 0
    for c, g in sorted(font.cmap.items()):
        lib.neuronte_coverage_styled(source[c], c, w, h, 1, style, cov.ctypes.data)
        want = cov > 0.5
        got = fill([p for p in font.contours[g]], w, h, unit, base)
        off = int((got != want).sum())
        total += off
        count += 1
        if off > worst:
            worst, worst_c = off, c
        over += off > OUTLINE_LIMIT * w * h
    print(f"check_fontfile: {label}: {count} outlines against the net at {w}x{h}, mean {total / count:.1f} "
          f"pixels differ, worst {worst} (U+{worst_c:04X}), limit {int(OUTLINE_LIMIT * w * h)}")
    if over:
        fail(f"{font.name}: {over} outlines differ from the net by more than the limit")


# ---- FreeType, through Pillow ----

def freetype(lib, fonts, handles, source):
    try:
        from PIL import ImageFont, features
        if not features.check("freetype2"):
            raise ImportError
    except ImportError:
        print("check_fontfile: FreeType check skipped: Pillow with FreeType is not installed")
        return
    compared = wrong = 0
    for name, sizes in (("NeuronteSans-Regular.ttf", (11, 14, 24)), (MONO, (15,))):
        font = fonts[name]
        for size in sizes:
            ft = ImageFont.truetype(os.path.join(font.dir, name), size, layout_engine=ImageFont.Layout.BASIC)
            depth, cells = font.strikes[size]
            for c in range(0x21, 0x7F):
                bx, by, adv, bits = cells[font.cmap[c]]
                mask = ft.getmask(chr(c), mode="1" if depth == 1 else "L")
                got = np.array(mask, np.uint8).reshape(mask.size[1], mask.size[0])
                rows, cols = np.nonzero(got.any(axis=1))[0], np.nonzero(got.any(axis=0))[0]
                got = got[rows[0]:rows[-1] + 1, cols[0]:cols[-1] + 1] if len(rows) else got[:0, :0]
                want = bits
                rows, cols = np.nonzero(want.any(axis=1))[0], np.nonzero(want.any(axis=0))[0]
                want = want[rows[0]:rows[-1] + 1, cols[0]:cols[-1] + 1] if len(rows) else want[:0, :0]
                compared += 1
                if got.shape != want.shape or (got != want).any() or ft.getlength(chr(c)) != adv:
                    wrong += 1
    print(f"check_fontfile: FreeType {features.version('freetype2')} gives back {compared} strike glyphs "
          f"and their advances, {wrong} differ")
    if wrong:
        fail("FreeType does not give back the strikes")


# ---- the stamp ----

def sha(path):
    with open(path, "rb") as fh:
        return hashlib.sha256(fh.read()).hexdigest()


def one_cell(name):
    """False for a bundle with a card NAME.symbols.txt: its drawings are for
    two cells or for halves, and no glyph of a font file is made from it."""
    return not os.path.exists(os.path.join(WEIGHTS, name[:-4] + ".symbols.txt"))


def stamp(folder, names):
    """The stamps beside FOLDER's files: one in it, or the steps' in the
    directory above it (STATE/NAME.stamp for STATE/fonts)."""
    paths = [os.path.join(folder, "neuronte-fontfile.stamp")]
    if not os.path.exists(paths[0]):
        up = os.path.dirname(os.path.abspath(folder))
        # The font steps' stamps: other steps keep stamps there too.
        paths = sorted(os.path.join(up, n) for n in os.listdir(up)
                       if n.endswith(".stamp") and "fontfile" in n)
    line = re.compile(r"(weight|output) ([0-9a-f]{64}) (\d+) (\d+\.\d{9}) (\S+)")
    weights, outputs = {}, {}
    for path in paths:
        with open(path, encoding="utf-8") as fh:
            lines = fh.read().splitlines()
        if len(lines) < 4 or not re.fullmatch(r"neuronte-fontfile \d+", lines[0]) or not lines[1].startswith("options "):
            fail(f"{os.path.basename(path)} does not start with the generator's version and options")
            continue
        for text in lines[2:]:
            m = line.fullmatch(text)
            if not m:
                fail(f"{os.path.basename(path)}: a line that is neither a weight nor an output: {text[:40]}")
                continue
            where = os.path.join(WEIGHTS if m.group(1) == "weight" else folder, m.group(5))
            if sha(where) != m.group(2) or os.path.getsize(where) != int(m.group(3)):
                fail(f"the stamp's SHA-256 or size of {m.group(5)} is not the file's")
            (weights if m.group(1) == "weight" else outputs)[m.group(5)] = m.group(2)
    expected = {n for n in os.listdir(WEIGHTS)
                if re.fullmatch(r"neuronte-ui\d+(-italic)?-v\d+\.nrt", n)
                or (re.fullmatch(r"neuronte-(term|kilix)-.*\.nrt", n) and one_cell(n))}
    expected |= {newest(p) for p in ("neuronte-split-v", "neuronte-ui-scaled-latin1-v",
                                     "neuronte-ui-scaled-latin1-italic-advances-v", "neuronte-ui-large-latin1-v")}
    if set(weights) != expected:
        fail(f"the stamps name {len(weights)} weight files, the faces are made from {len(expected)}")
    if set(outputs) != set(names):
        fail(f"the stamps name {sorted(outputs)} as written, expected {sorted(names)}")
    return len(weights), len(paths)


def main():
    if len(sys.argv) < 3:
        raise SystemExit(__doc__)
    lib = library(sys.argv[1])
    folder = sys.argv[2]
    names = list(SANS) + [MONO]
    fonts = {}
    for name in names:
        try:
            fonts[name] = Font(os.path.join(folder, name))
            fonts[name].dir = folder
        except (OSError, ValueError, struct.error, IndexError, KeyError) as e:
            fail(f"{name}: {e}")
    if fails:
        raise SystemExit(1)
    print(f"check_fontfile: {len(fonts)} files parse, checksums right, "
          f"{sum(f.count for f in fonts.values())} glyphs, "
          f"{sum(len(f.strikes) for f in fonts.values())} strikes")

    handles = {"scaled_name": newest("neuronte-ui-scaled-latin1-v")}
    large = open_file(lib, os.path.join(WEIGHTS, newest("neuronte-ui-large-latin1-v")))
    handles["slant"] = open_file(lib, os.path.join(WEIGHTS, newest("neuronte-ui-scaled-latin1-italic-advances-v")))
    sans_set = face_chars(lib, large)

    # 2. characters, names and advances
    for name, (bold, italic) in SANS.items():
        f = fonts[name]
        if sorted(f.cmap) != sans_set:
            fail(f"{name}: maps {len(f.cmap)} characters, the face draws {len(sans_set)}")
        style = {(0, 0): "Regular", (1, 0): "Bold", (0, 1): "Italic", (1, 1): "Bold Italic"}[(bold, italic)]
        if f.names.get(1) != "Neuronte Sans" or f.names.get(2) != style or f.upm != UPM or f.fixed_pitch:
            fail(f"{name}: family, style, em or pitch is not as stated")
        if f.weight != (700 if bold else 400) or bool(f.mac_style & 1) != bool(bold) or bool(f.mac_style & 2) != bool(italic):
            fail(f"{name}: weight or style bits are wrong")
        adv = handles["slant"] if italic else large
        for c in sans_set:
            src = adv if lib.neuronte_advance_scalar(adv, c) == c else large
            want = lib.neuronte_advance_steps(src, c, 0) * UPM // 100 + (UPM // 32 if bold else 0)
            if f.advance[f.cmap[c]] != want:
                fail(f"{name}: U+{c:04X} advances {f.advance[f.cmap[c]]}, the net's table says {want}")
                break
    # Text first, then a terminal's own symbols; never a bundle drawn for
    # another box than one cell.
    bundles = [newest("neuronte-split-v")]
    for kind in ("term", "kilix"):
        bundles += sorted(n for n in os.listdir(WEIGHTS)
                          if re.fullmatch(rf"neuronte-{kind}-.*\.nrt", n) and one_cell(n))
    source, bundle_handles = {}, []
    for name in bundles:
        h = open_file(lib, os.path.join(WEIGHTS, name))
        bundle_handles.append(h)
        for s in list(range(0, 0xD800)) + list(range(0xE000, 0x110000)):
            if s not in source and lib.neuronte_untrained(h, s) == 0:
                source[s] = h
    mono = fonts[MONO]
    if set(mono.cmap) != set(source):
        fail(f"{MONO}: maps {len(mono.cmap)} characters, the bundles were trained on {len(source)}")
    if (mono.names.get(1) != "Neuronte Mono" or not mono.fixed_pitch or set(mono.advance) != {UPM * 6 // 10}
            or mono.ascender - mono.descender != UPM * 12 // 10 or mono.line_gap):
        fail(f"{MONO}: family, pitch, advance or line height is not as stated")
    print(f"check_fontfile: Sans maps the {len(sans_set)} characters of the desktop face, "
          f"Mono the {len(source)} of {len(bundles)} bundles; advances are the nets'")

    # 3. strikes
    total = wrong = pens = 0
    for name, (bold, italic) in SANS.items():
        n, w, p, missing = sans_strikes(lib, fonts[name], bold, italic, handles)
        if missing:
            fail(f"{name}: {missing} characters a net draws at a strike's size have no bitmap there")
        total, wrong, pens = total + n, wrong + w, pens + p
    n, w = mono_strikes(lib, mono, source)
    print(f"check_fontfile: {total} Sans strike glyphs against the draw calls: {wrong} pixels and {pens} advances "
          f"differ; {n} Mono cells against neuronte_coverage(): {w} pixels differ")
    if wrong or pens or w:
        fail("strikes are not the library's pixels")

    # 4. outlines
    sans_source = {c: large for c in sans_set}
    outlines(lib, fonts["NeuronteSans-Regular.ttf"], sans_source, 0, TRACE, UPM / TRACE, "Sans Regular")
    outlines(lib, fonts["NeuronteSans-Bold.ttf"], sans_source, BOLD, TRACE, UPM / TRACE, "Sans Bold")
    outlines(lib, mono, source, 0, TRACE, 0.6 * UPM / TRACE, "Mono")

    # 5. FreeType
    freetype(lib, fonts, handles, source)

    # 6. stamp, and a second run
    count, stamps = stamp(folder, names)
    print(f"check_fontfile: {stamps} stamps name {count} weight files and {len(names)} written files, "
          "hashes and sizes right")
    if len(sys.argv) > 3:
        same = [n for n in names if sha(os.path.join(sys.argv[3], n)) == sha(os.path.join(folder, n))]
        print(f"check_fontfile: a second run wrote the same bytes for {len(same)} of {len(names)} files")
        if len(same) != len(names):
            fail("two runs gave different files")
    if fails:
        raise SystemExit(f"check_fontfile: {len(fails)} failures")
    print("check_fontfile: passed")


if __name__ == "__main__":
    main()
