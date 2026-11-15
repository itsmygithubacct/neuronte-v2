import json, os, statistics as st
from collections import defaultdict
import sys
HOSTS = [(a, a) for a in sys.argv[1:]]
def load(h):
    g = defaultdict(list)
    for l in open(os.path.join(os.path.dirname(os.path.abspath(__file__)), 'results-%s.jsonl' % h)):
        j = json.loads(l); g[(j['mode'], j['start'])].append(j['r'])
    return g
med = st.median
out = []
for h, cpu in HOSTS:
    g = load(h)
    out.append('\n### Intel %s\n' % cpu.replace('i7-', 'Core i7-').upper().replace('CORE I7', 'Core i7'))
    out.append('Start, seconds from process start (median of 3; lowest to highest first frame in brackets):\n')
    out.append('| Set | Start | Modules loaded | First frame | Start-up letters all drawn |\n|---|---|---:|---:|---:|')
    for m, name in (('old', '0.1.1 faces'), ('new', 'Neuronte 2'), ('none', 'DejaVu')):
        for s in ('cold', 'warm'):
            rs = g[(m, s)]; ff = [r['first_frame_s'] for r in rs]
            out.append('| %s | %s | %.3f | %.3f (%.3f to %.3f) | %s |' % (name, s, med([r['import_s'] for r in rs]), med(ff), min(ff), max(ff),
                       '%.3f' % med([r['letters_done_s'] for r in rs]) if m != 'none' else 'n/a'))
    out.append('\nDrawing 200 strings (4,329 characters) at each size, regular weight:\n')
    out.append('| Size | 0.1.1 faces: net time, first use | Neuronte 2: net time, first use | Ratio | Neuronte, each later string | DejaVu, each later string | Ratio |\n|---:|---:|---:|---:|---:|---:|---:|')
    for em in (8, 10, 11, 12, 14, 16, 20, 24, 32, 48):
        k = str(em)
        def net(m):
            # first pass less one steady pass, cold start: what the net costs once per process
            return med([r['text'][k]['first_pass_s'] - r['text'][k]['steady_us_per_string'] * r['strings'] / 1e6 for r in g[(m, 'cold')]])
        def steady(m):
            return med([r['text'][k]['steady_us_per_string'] for r in g[(m, 'warm')]])
        a, b = net('old'), net('new')
        note = '' 
        small = em in (10, 11)
        fa = ('%.0f ms' % (1e3 * a)) if not small else 'at start-up'
        fb = ('%.0f ms' % (1e3 * b)) if not small else 'at start-up'
        ratio = ('%.1f×' % (a / b)) if (not small and b > 0.004) else '—'
        out.append('| %d px%s | %s | %s | %s | %.0f µs | %.0f µs | %.1f× |' % (em, ' (scalable)' if em == 48 else '', fa, fb, ratio, steady('new'), steady('none'), steady('new') / steady('none')))
    ml = med([r['text']['11']['measure_us_per_string'] for r in g[('new', 'warm')]]); fl = med([r['text']['11']['measure_us_per_string'] for r in g[('none', 'warm')]])
    out.append('\nMeasuring a string\'s width at 11 px: Neuronte %.1f µs, DejaVu %.1f µs.' % (ml, fl))
print('\n'.join(out))
