#!/usr/bin/env python3
"""Audit UI icons for a baked-in opaque background.

Theming an icon means letting the theme's background show through it. That only
works where the icon is transparent, so this answers: which icons are NOT?

Renders each svg on transparency with makeicons' flags (-D, forced square) and
measures coverage. Classification, in order:

  opaque-bg   >=98% of pixels opaque AND the 1px border ring >=95% opaque
              -> the icon paints its own background; theming cannot show through
  edge-bg     border ring >=95% opaque but interior has holes
              -> framed/bordered, still blocks the theme at the edges
  transparent everything else -> already themeable

`bgcolor` is the median colour of the border ring, i.e. what that background is.
Note a full-canvas fill is not automatically a "background": Save.svg is a navy
floppy that fills its canvas by design. Read bgcolor before concluding.

Requires: inkscape 1.x, python3-pil.
"""

import argparse
import os
import subprocess
import sys
import tempfile
from collections import Counter
from concurrent.futures import ProcessPoolExecutor

from PIL import Image

SIZE = 64
OPAQUE = 250  # alpha at/above this counts as opaque


def render(svg, outdir, size):
    out = os.path.join(outdir, os.path.basename(svg) + ".png")
    r = subprocess.run(
        ["inkscape", "-D", "-w", str(size), "-h", str(size), svg,
         "--export-type=png", f"--export-filename={out}"],
        stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    if r.returncode != 0 or not os.path.exists(out):
        return None
    return out


def ring_pixels(img):
    """The 1px border ring: where a background is unambiguous."""
    w, h = img.size
    px = img.load()
    for x in range(w):
        yield px[x, 0]
        yield px[x, h - 1]
    for y in range(1, h - 1):
        yield px[0, y]
        yield px[w - 1, y]


def classify(svg, size):
    with tempfile.TemporaryDirectory() as td:
        out = render(svg, td, size)
        if out is None:
            return (os.path.basename(svg), "RENDER-FAILED", 0.0, 0.0, "")
        img = Image.open(out).convert("RGBA")
        w, h = img.size
        alphas = [p[3] for p in img.getdata()]
        opaque_frac = sum(a >= OPAQUE for a in alphas) / len(alphas)

        ring = list(ring_pixels(img))
        ring_frac = sum(p[3] >= OPAQUE for p in ring) / len(ring)

        # what colour IS the ring -- the most common opaque ring colour
        opaque_ring = [p[:3] for p in ring if p[3] >= OPAQUE]
        if opaque_ring:
            (r, g, b), _ = Counter(opaque_ring).most_common(1)[0]
            bg = f"#{r:02x}{g:02x}{b:02x}"
        else:
            bg = ""

        if ring_frac >= 0.95 and opaque_frac >= 0.98:
            kind = "opaque-bg"
        elif ring_frac >= 0.95:
            kind = "edge-bg"
        else:
            kind = "transparent"
        return (os.path.basename(svg), kind, opaque_frac, ring_frac, bg)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--dir", default="src/icons", help="directory of svgs")
    ap.add_argument("--size", type=int, default=SIZE)
    ap.add_argument("--kind", help="only list this class")
    args = ap.parse_args()

    svgs = sorted(
        os.path.join(args.dir, f) for f in os.listdir(args.dir) if f.endswith(".svg"))
    if not svgs:
        sys.exit(f"no svgs in {args.dir}")

    with ProcessPoolExecutor() as ex:
        rows = list(ex.map(classify, svgs, [args.size] * len(svgs)))

    counts = Counter(r[1] for r in rows)
    bgcolors = Counter(r[4] for r in rows if r[1] in ("opaque-bg", "edge-bg") and r[4])

    for name, kind, of, rf, bg in sorted(rows, key=lambda r: (r[1], r[0])):
        if args.kind and kind != args.kind:
            continue
        if kind == "transparent":
            continue
        print(f"{kind:12} {bg:8} opaque={of:5.1%} ring={rf:5.1%}  {name}")

    print(f"\n--- {len(rows)} icons ---")
    for k, n in counts.most_common():
        print(f"{k:12} {n}")
    print("\nbackground colours (opaque-bg + edge-bg):")
    for c, n in bgcolors.most_common(10):
        print(f"  {c}  {n}")


if __name__ == "__main__":
    main()
