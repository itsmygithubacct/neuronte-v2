"""Draw one line with the font Kilix 95 has (Neuronte or DejaVu) and print its
measurements as JSON. Run from the Kilix 95 tests directory, with
NEURONTE_HOME naming a built Neuronte tree, or an empty directory for DejaVu.

usage: python3 compare_dejavu.py > result.json
"""
import sys, json, os
import harness, neuronte_face as N, theme as T
from PIL import Image
TEXT = "The quick brown fox jumps over the lazy dog. 0123456789"
def art(font, text, mode="1"):
    mask, off = font.getmask2(text, mode)
    im = Image.new("L", mask.size); im.im.paste(255, (0,0)+mask.size, mask) if mode=="1" else None
    if mode != "1":
        im = Image.frombytes("L", mask.size, bytes(mask))
    box = im.getbbox()
    return im, box, off
out = {"kind": type(T.FONT).__name__, "sizes": {}}
for em in (8, 11, 14, 16, 24, 32, 48):
    f = T.font_at(em)
    fb = T.font_at(em, bold=True)
    im, box, off = art(f, TEXT)
    g, gbox, _ = art(f, TEXT, "L")
    hist = g.histogram()
    asc, desc = f.getmetrics()
    out["sizes"][em] = {"width": f.getlength(TEXT), "bold_width": fb.getlength(TEXT), "ink_box": box, "ascent": asc, "descent": desc,
                        "grey": sum(hist[1:255]), "full": hist[255],
                        "adv": {c: f.getlength(c) for c in "ilmW 0."}, "AV": f.getlength("AV") - f.getlength("A") - f.getlength("V"),
                        "frac": any(abs(f.getlength(c) - round(f.getlength(c))) > 1e-6 for c in TEXT)}
f = T.font_at(11)
im, box, off = art(f, "Hamburgefonts 0123")
rows = []
px = im.load()
for y in range(box[1], box[3]):
    rows.append("".join("#" if px[x, y] else "." for x in range(box[0], box[2])))
out["art11"] = rows
print(json.dumps(out))
