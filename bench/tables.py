"""Print the tables of BENCHMARKS.md from the result files beside this script.

usage: python3 bench/tables.py i7-6820hq i7-10700f
"""
import json, os, statistics as st, sys
from collections import defaultdict

HERE = os.path.dirname(os.path.abspath(__file__))
SETS = (('old', '0.1.1 faces'), ('new', 'Neuronte 2.0'), ('new21', 'Neuronte 2.1'), ('none', 'DejaVu'))
med = st.median


def load(name):
    g = defaultdict(list)
    for line in open(os.path.join(HERE, 'results-%s.jsonl' % name)):
        j = json.loads(line)
        g[(j['mode'], j['start'])].append(j['r'])
    return g


for name in sys.argv[1:]:
    g = load(name)
    print('\n### Intel Core %s\n' % name.replace('hq', 'HQ').replace('f', 'F'))
    print('Start, seconds from process start (median of 3; lowest to highest first frame in brackets):\n')
    print('| Set | Start | Modules loaded | First frame | Start-up letters all drawn |\n|---|---|---:|---:|---:|')
    for m, label in SETS:
        for s in ('cold', 'warm'):
            rs = g[(m, s)]
            ff = [r['first_frame_s'] for r in rs]
            print('| %s | %s | %.3f | %.3f (%.3f to %.3f) | %s |' % (
                label, s, med([r['import_s'] for r in rs]), med(ff), min(ff), max(ff),
                '%.3f' % med([r['letters_done_s'] for r in rs]) if m != 'none' else 'n/a'))
    r0 = g[('none', 'warm')][0]
    print('\nDrawing %d strings (%s characters) at each size, regular weight. "First use" is the first pass less one later pass:\n'
          % (r0['strings'], format(r0['chars'], ',')))
    print('| Size | First use, 0.1.1 faces | First use, Neuronte 2.1 | Each later string, Neuronte 2.0 | Each later string, Neuronte 2.1 | Each later string, DejaVu | DejaVu time ÷ Neuronte 2.1 time |')
    print('|---:|---:|---:|---:|---:|---:|---:|')
    for em in (8, 10, 11, 12, 14, 16, 20, 24, 32, 48):
        k = str(em)

        def first(m):
            return med([r['text'][k]['first_pass_s'] - r['text'][k]['steady_us_per_string'] * r['strings'] / 1e6
                        for r in g[(m, 'cold')]])

        def later(m):
            return med([r['text'][k]['steady_us_per_string'] for r in g[(m, 'warm')]])

        small = em in (10, 11)
        print('| %d px%s | %s | %s | %.0f µs | %.0f µs | %.0f µs | %.1f× |' % (
            em, ' (scalable)' if em == 48 else '',
            'at start-up' if small else '%.0f ms' % (1e3 * first('old')),
            'at start-up' if small else '%.0f ms' % (1e3 * first('new21')),
            later('new'), later('new21'), later('none'), later('none') / later('new21')))
    w = lambda m: med([r['text']['11']['measure_us_per_string'] for r in g[(m, 'warm')]])
    print("\nMeasuring a string's width at 11 px: Neuronte %.1f µs, DejaVu %.1f µs." % (w('new21'), w('none')))
