"""Draw the sample lines of docs/neuronte-2.1-and-dejavu.png with the font
Kilix 95 has (Neuronte or DejaVu), save them as one strip and print the row
heights as JSON. Run from the Kilix 95 tests directory, once with
NEURONTE_HOME naming a built Neuronte tree and once with an empty directory.

usage: python3 render_dejavu_demo.py OUT.png > OUT.json
"""
import sys, json
import harness, theme as T
from PIL import Image, ImageDraw
LINES = [(8, False, "The quick brown fox jumps over the lazy dog. 0123456789"),
         (10, False, "File  Edit  View  Help      My Computer   Recycle Bin"),
         (11, False, "The quick brown fox jumps over the lazy dog. 0123456789"),
         (11, True, "Start   12:08 AM   budget 2026.txt   OK   Cancel"),
         (12, False, "Hamburgefonts {curly} [square] (round) <angle> #@&$%"),
         (14, False, "Sphinx of black quartz, judge my vow."),
         (16, False, "Pack my box with five dozen liquor jugs."),
         (24, False, "Neural nets as a font"),
         (32, False, "Hello, world 123"),
         (48, False, "Above 32 px")]
W = 560
rows = []
for em, bold, text in LINES:
    f = T.font_at(em, bold=bold)
    asc, desc = f.getmetrics()
    h = max(asc + desc, em) + 6
    im = Image.new("L", (W, h), 255)
    d = ImageDraw.Draw(im)
    d.fontmode = "1" if em <= 32 else "L"     # as Kilix 95 draws small text
    d.text((4, 3), text, font=f, fill=0)
    rows.append(im)
out = Image.new("L", (W, sum(r.height for r in rows)), 255)
y = 0
for r in rows:
    out.paste(r, (0, y)); y += r.height
out.save(sys.argv[1])
print(json.dumps({"kind": type(T.FONT).__name__, "heights": [r.height for r in rows], "labels": ["%d px%s" % (e, " bold" if b else "") for e, b, _ in LINES]}))
