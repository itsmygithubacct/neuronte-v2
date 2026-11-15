"""One Kilix 95 process: start the desktop offscreen, draw the first frame,
then time text drawing. Prints one JSON line. Run from the Kilix 95 tests
directory's parent with HOME set to the cache to use (empty = cold)."""
import json, os, sys, time
T0 = time.perf_counter()
sys.path.insert(0, os.path.join(sys.argv[1], "tests"))
import harness as H
import neuronte_face as N
import theme as T
t_import = time.perf_counter() - T0
desk = H.make_desk((1024, 768))
t_desk = time.perf_counter() - T0
desk.render()
t_first = time.perf_counter() - T0
# Letters still being drawn by the start-up thread (cold Neuronte start only).
thread = N.start_letters((T.FONT, T.SMALL))
if thread is not None:
    thread.join()
t_letters = time.perf_counter() - T0
N.save_letters()
renders = []
for _ in range(10):
    a = time.perf_counter(); desk.render(); renders.append(time.perf_counter() - a)

from PIL import Image, ImageDraw
import random
rng = random.Random(95)
letters = [chr(c) for c in range(32, 127)]
corpus = ["Start", "My Computer", "12:08 AM", "Recycle Bin", "File  Edit  View  Help", "budget 2026.txt",
          "The quick brown fox jumps over the lazy dog", "OK", "Cancel", "Network Neighborhood"]
corpus += ["".join(rng.choice(letters) for _ in range(rng.randint(4, 40))) for _ in range(190)]
chars = sum(len(s) for s in corpus)
image = Image.new("RGB", (1024, 64), (192, 192, 192))
draw = ImageDraw.Draw(image)
text = {}
for em in (8, 10, 11, 12, 14, 16, 20, 24, 32, 48):
    for bold in (False, True):
        a = time.perf_counter(); font = T.font_at(em, bold=bold); t_open = time.perf_counter() - a
        passes = []
        for _ in range(4 if em < 48 else 3):
            a = time.perf_counter()
            for s in corpus:
                draw.text((2, 2), s, font=font, fill=(0, 0, 0))
            passes.append(time.perf_counter() - a)
        a = time.perf_counter()
        for s in corpus:
            font.getlength(s)
        t_len = time.perf_counter() - a
        steady = sorted(passes[1:])[(len(passes) - 1) // 2]
        text["%d%s" % (em, "b" if bold else "")] = {
            "kind": type(font).__name__, "open_s": t_open, "first_pass_s": passes[0],
            "steady_us_per_string": 1e6 * steady / len(corpus), "steady_ns_per_char": 1e9 * steady / chars,
            "measure_us_per_string": 1e6 * t_len / len(corpus)}
N.save_letters()
print(json.dumps({"font": type(T.FONT).__name__, "weights": os.path.basename(N.weight_path(11)),
                  "import_s": t_import, "desk_s": t_desk, "first_frame_s": t_first, "letters_done_s": t_letters,
                  "render_ms_median": 1e3 * sorted(renders)[5], "strings": len(corpus), "chars": chars, "text": text}))
