"""Check the shipped set: checksums, faces against their drawings, and every
file's pixels against the recorded ones.

usage: python3 tests/check_set.py build/libneuronte.so [--write-golden]

1. Every line of weights/SHA256SUMS matches its file, and every file in
   weights/ is listed.
2. Every native desktop face (weights/neuronte-uiEM[-italic]-v1.nrt) is drawn
   with neuronte_draw_text() at its em, binary, regular and bold, for each of
   its 96 characters, and compared with the drawing in font/ui-EM[-italic].txt.
   The pen must move by the drawing's advance. A face may have at most BUDGET
   wrong pixels in all, and none on a digit.
3. Every packed file's coverage over a fixed list of characters and cells, and
   its advances where it has a table, hash to the values in tests/golden.json.
   --write-golden records them instead (done once, when the set is cut).
"""
import ctypes
import hashlib
import json
import os
import re
import sys

import numpy as np

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(ROOT, "train"))
import glyphs  # noqa: E402

BUDGET = 2
GOLDEN = os.path.join(ROOT, "tests", "golden.json")
SCALARS = (list(range(0x20, 0x7F)) + [0xA0, 0xC9, 0xE9, 0xF1, 0x100, 0x152, 0x17E]
           + [0x391, 0x3A9, 0x3B1, 0x3C9, 0x410, 0x42F, 0x430, 0x44F, 0x451]
           + list(range(0x2500, 0x2580, 5)) + list(range(0x2580, 0x25A0))
           + list(range(0x2800, 0x2900)) + list(range(0x1FB00, 0x1FB3C, 3))
           + list(range(0x1CD00, 0x1CDE6, 9)) + [0x2026, 0x20AC, 0x2190, 0x2713, 0xFFFD]
           + [0x1F600, 0x1F44D, 0x2764, 0x1F680, 0x100000, 0x100001, 0x100010, 0x10FFFD, 0x110000])
CELLS = [(8, 16), (16, 32), (11, 23)]
TEXT = b"The quick brown fox jumps over the lazy dog. 0123456789 !?#@&*()[]{}<>"


def load(path):
    lib = ctypes.CDLL(os.path.abspath(path))
    lib.neuronte_open_file.argtypes = [ctypes.c_char_p, ctypes.POINTER(ctypes.c_void_p)]
    for name in ("neuronte_layers", "neuronte_nets", "neuronte_advances", "neuronte_close"):
        getattr(lib, name).argtypes = [ctypes.c_void_p]
    lib.neuronte_close.restype = None
    lib.neuronte_coverage_layers.argtypes = [ctypes.c_void_p, ctypes.c_uint32, ctypes.c_int, ctypes.c_int,
                                             ctypes.c_int, ctypes.c_void_p, ctypes.c_void_p]
    lib.neuronte_advance.argtypes = [ctypes.c_void_p, ctypes.c_uint32, ctypes.c_int, ctypes.c_int]
    lib.neuronte_text_width.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_size_t, ctypes.c_int, ctypes.c_int]
    lib.neuronte_set_binary.argtypes = [ctypes.c_void_p, ctypes.c_int]
    lib.neuronte_draw_text.argtypes = ([ctypes.c_void_p, ctypes.c_void_p] + [ctypes.c_int] * 7
                                       + [ctypes.c_char_p, ctypes.c_size_t, ctypes.c_uint32])
    return lib


def open_file(lib, path):
    h = ctypes.c_void_p()
    rc = lib.neuronte_open_file(path.encode(), ctypes.byref(h))
    if rc != 0:
        raise SystemExit(f"FAIL {path}: open returned {rc}")
    return h


def checksums():
    listed, bad = set(), 0
    with open(os.path.join(ROOT, "weights", "SHA256SUMS")) as fh:
        for line in fh:
            want, name = line.split()
            listed.add(name)
            with open(os.path.join(ROOT, "weights", name), "rb") as f:
                if hashlib.sha256(f.read()).hexdigest() != want:
                    print(f"FAIL {name}: sha256 differs from SHA256SUMS")
                    bad += 1
    extra = set(os.listdir(os.path.join(ROOT, "weights"))) - listed - {"SHA256SUMS"}
    for name in sorted(extra):
        print(f"FAIL {name}: not in SHA256SUMS")
    print(f"check_set: {len(listed)} files match SHA256SUMS")
    return bad + len(extra)


def face(lib, name):
    """Wrong pixels of one native face against its drawing."""
    m = re.fullmatch(r"neuronte-ui(\d+)(-italic)?-v\d+\.nrt", name)
    em, italic = int(m.group(1)), bool(m.group(2))
    src = os.path.join(ROOT, "font", f"ui-{em}-italic.txt" if italic else f"ui-{em}.txt")
    h = open_file(lib, os.path.join(ROOT, "weights", name))
    lib.neuronte_set_binary(h, 1)
    W = H = 2 * em + 4
    X = Y = 2
    wrong, digits, pen, drawn = 0, 0, 0, 0
    for ch, (adv, art) in sorted(glyphs.pixel_face(em, src).items()):
        ink = np.array([[c == "#" for c in r] for r in art])
        for bold in (0, 1):
            want = np.zeros((H, W), bool)
            for dx in range(bold + 1):
                want[Y + glyphs.UI_TOP[em]:Y + glyphs.UI_TOP[em] + em, X + dx:X + dx + ink.shape[1]] |= ink
            buf = np.zeros((H, W), np.uint32)
            text = ch.encode()
            rc = lib.neuronte_draw_text(h, buf.ctypes.data, W, W, H, X, Y, em, bold, text, len(text), 0xFFFFFFFF)
            drawn += 1
            pen += rc != adv + bold
            off = int(((buf != 0) != want).sum()) + int(((buf != 0) & (buf != 0xFFFFFFFF)).sum())
            wrong += off
            digits += off if ch.isdigit() else 0
    lib.neuronte_close(h)
    ok = wrong <= BUDGET and digits == 0 and pen == 0
    if not ok:
        print(f"FAIL {name}: {wrong} wrong pixels ({digits} on digits), {pen} advances differ")
    return ok, drawn, wrong


def pixels(lib, name):
    h = open_file(lib, os.path.join(ROOT, "weights", name))
    layers = max(lib.neuronte_layers(h), 1)
    entry = {"nets": lib.neuronte_nets(h), "layers": lib.neuronte_layers(h)}
    dig = hashlib.sha256()
    for w, ht in CELLS:
        masks = (ctypes.c_float * (w * ht * layers))()
        ents = (ctypes.c_uint8 * layers)()
        for s in SCALARS:
            r = lib.neuronte_coverage_layers(h, s, w, ht, 0, masks, ents)
            dig.update(f"{s:x}:{w}x{ht}:{r};".encode())
            if r == 0:
                dig.update(bytes(masks))
                dig.update(bytes(ents))
    entry["coverage_sha256"] = dig.hexdigest()
    if lib.neuronte_advances(h) > 0:
        adv = hashlib.sha256()
        for em in (8, 12, 16, 24, 32, 48):
            for bold in (0, 1):
                adv.update(f"{em}:{bold}:{lib.neuronte_text_width(h, TEXT, len(TEXT), bold, em)};".encode())
                for s in range(0x20, 0x7F):
                    adv.update(f"{lib.neuronte_advance(h, s, bold, em)},".encode())
        entry["advance_sha256"] = adv.hexdigest()
    lib.neuronte_close(h)
    return entry


def main():
    if len(sys.argv) < 2:
        sys.exit(__doc__)
    lib = load(sys.argv[1])
    write = "--write-golden" in sys.argv[2:]
    failed = checksums()
    names = sorted(n for n in os.listdir(os.path.join(ROOT, "weights")) if n.endswith(".nrt"))
    faces = [n for n in names if re.fullmatch(r"neuronte-ui\d+(-italic)?-v\d+\.nrt", n)]
    letters = off = 0
    for n in faces:
        ok, drawn, wrong = face(lib, n)
        failed += not ok
        letters += drawn
        off += wrong
    print(f"check_set: {len(faces)} faces, {letters} binary letters at their em, {off} wrong pixels in all")
    got = {n: pixels(lib, n) for n in names}
    if write:
        with open(GOLDEN, "w") as fh:
            json.dump(got, fh, indent=1, sort_keys=True)
            fh.write("\n")
        print(f"check_set: wrote {GOLDEN}")
    else:
        with open(GOLDEN) as fh:
            want = json.load(fh)
        for n in sorted(set(got) | set(want)):
            if got.get(n) != want.get(n):
                print(f"FAIL {n}: pixels or advances differ from tests/golden.json")
                failed += 1
        print(f"check_set: {len(got)} packed files, {len(SCALARS) * len(CELLS)} coverage calls each, against tests/golden.json")
    if failed:
        sys.exit(1)
    print("check_set: passed")


if __name__ == "__main__":
    main()
