#!/usr/bin/env python3
"""TwoInk.rank: the exact shortlist for two-ink cells.

Uses numpy and Neuronte's own masks only: no font file, no rasteriser.

1. For every cell, rank's first choice has the least unconstrained
   two-colour error of all candidates, found here independently by
   solving each candidate's 2x2 normal equations (portable masks, and the
   nets' own masks in full mode).
2. A shortlist of every candidate fits every cell with the same error,
   and draws the same pixels, as no shortlist (ties may pick another
   character of equal error).
3. With the flat mask added, a flat cell keeps a zero-error fit.
"""
import sys
from pathlib import Path

import numpy as np

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools'))
import neuronte_video as nv  # noqa: E402

failures = 0


def check(ok, what):
    global failures
    print(('ok   ' if ok else 'FAIL ') + what)
    failures += not ok


def brute(two, img):
    """Least unconstrained error per cell and candidate, by normal equations."""
    w, h = two.width, two.height
    x = nv.patches(img, w, h).reshape(-1, w * h, 3).astype(np.float64)
    m = two.masks
    n = float(w * h)
    s1, s2 = m.sum(1), (m * m).sum(1)                      # K
    det = n * s2 - s1 * s1                                 # K
    sx = x.sum(1)                                          # N x 3
    smx = np.einsum('kp,npc->nkc', m, x)                   # N x K x 3
    xx = (x * x).sum(1)                                    # N x 3
    sxb = sx[:, None, :]                                   # N x 1 x 3
    S1, S2 = s1[None, :, None], s2[None, :, None]
    d = np.where(det > 1e-9, det, 1.0)[None, :, None]
    a = (S2 * sxb - S1 * smx) / d                          # background
    b = (n * smx - S1 * sxb) / d                           # foreground - background
    flat = det <= 1e-9
    a = np.where(flat[None, :, None], sxb / n, a)
    b = np.where(flat[None, :, None], 0.0, b)
    err = xx[:, None, :] - a * sx[:, None, :] - b * smx    # residual = |x|^2 - coef . A^T x
    return err.sum(2)


rng = np.random.default_rng(11)
for mode, (w, h) in (('portable', (6, 12)), ('full', (6, 12))):
    two = nv.TwoInk(mode, w, h)
    img = (rng.random((h * 4, w * 6, 3)) * 255).astype(np.uint8)
    img[:h] = np.linspace(0, 255, w * 6).astype(np.uint8)[None, :, None]
    img[h:2 * h, :w] = (90, 140, 30)                         # one flat cell
    err = brute(two, img)
    top = two.rank(img, 1).reshape(-1)
    best = err.min(1)
    got = err[np.arange(len(top)), top]
    check(np.allclose(got, best, rtol=1e-6, atol=1e-3 * w * h),
          f'{mode}: rank top-1 has the least unconstrained error in all {len(top)} cells ({len(two.masks)} candidates)')
    k = len(two.masks)
    a = two.fit(img)
    two.previous = None
    b = two.fit(img, two.rank(img, k))
    # Candidates can tie (a flat cell fits any mask with equal colours), so
    # compare what matters: each cell's error and the drawn result.
    check(np.allclose(a['sse'], b['sse'], rtol=1e-9, atol=1e-6) and np.array_equal(two.decode(a), two.decode(b)),
          f'{mode}: a shortlist of every candidate fits as well as none, and draws the same pixels')
    flat = np.where(two.masks.std(1) < 1e-9)[0][:1]
    c = two.fit(img, two.rank(img, 4, always=flat))
    check(c['sse'].reshape(-1)[6] < 1e-6, f'{mode}: the flat cell keeps a zero-error fit with the flat mask added')

sys.exit(1 if failures else 0)
