#!/usr/bin/env python3
"""Reproduce Task 2's load-bearing claim: regenerating the waypoint PNGs at a higher
resolution changes nothing visually.

Renders every src/icons/waypoints/*.svg at --size with the same rendering flags the committed
makeicons uses (-D, forced square), downscales the result back to 32, and compares against the
shipped 32x32 PNG.

The export flag differs: makeicons still uses the Inkscape 0.92 `--export-png=` (deprecated on
1.x but functional), this uses `--export-type=png --export-filename=`. Verified byte-identical
output on 1.2.2, so the comparison is sound -- but re-check that if you ever change either.

Verdict: "visible (>8)" must be 0. That is the whole "must not change in a visual aspect"
constraint, mechanised.

  ./wptdiff.py --size 96
  ./wptdiff.py --size 96 --keep out96/     # also keep the generated PNGs

Requires: inkscape (1.x), python3-pil.

Renders with inkscape and compares inkscape-PNG against the shipped PNG, because that is how
the waypoint set is produced -- a Qt-rendered comparison would measure the wrong pipeline.
"""
import argparse
import glob
import os
import shutil
import subprocess
import sys
import tempfile

try:
    from PIL import Image
except ImportError:
    sys.exit("need python3-pil")

WPT = os.path.join(os.path.dirname(os.path.abspath(__file__)), os.pardir, "waypoints")


def premultiply(img):
    """Alpha-weight the colour channels.

    Raw RGB is undefined padding where alpha=0, so comparing it directly reports garbage --
    this once produced '140/203 changed' when the true answer was 0.
    """
    img = img.convert("RGBA")
    px = img.load()
    for y in range(img.height):
        for x in range(img.width):
            r, g, b, a = px[x, y]
            f = a / 255.0
            px[x, y] = (int(r * f), int(g * f), int(b * f), a)
    return img


def mean_diff(a, b):
    pa, pb = a.load(), b.load()
    total = 0.0
    for y in range(a.height):
        for x in range(a.width):
            ca, cb = pa[x, y], pb[x, y]
            total += sum(abs(ca[i] - cb[i]) for i in range(4)) / 4.0
    return total / (a.width * a.height)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--size", type=int, default=96, help="target raster size (default 96)")
    ap.add_argument("--ref", type=int, default=32, help="shipped reference size (default 32)")
    ap.add_argument("--keep", metavar="DIR", help="keep generated PNGs here")
    args = ap.parse_args()

    refdir = os.path.join(WPT, f"{args.ref}x{args.ref}")
    if not os.path.isdir(refdir):
        sys.exit(f"no reference dir {refdir} -- --ref must name a shipped size (32)")

    outdir = args.keep or tempfile.mkdtemp(prefix="wptdiff")
    # Refuse to render into the shipped rasters: we delete stale PNGs below, which would
    # wipe the very references we compare against and then trivially PASS.
    if os.path.realpath(outdir) in (os.path.realpath(refdir), os.path.realpath(WPT)):
        sys.exit(f"refusing to use {outdir} as --keep: that is the source/reference tree")

    # A reused --keep dir would leak stale PNGs into the comparison and the verdict.
    if os.path.isdir(outdir):
        for stale in glob.glob(os.path.join(outdir, "*.png")):
            os.remove(stale)
    os.makedirs(outdir, exist_ok=True)

    svgs = sorted(glob.glob(os.path.join(WPT, "*.svg")))
    print(f"rendering {len(svgs)} waypoints at {args.size}px "
          f"(same rendering flags as makeicons)...")
    failed = []
    for s in svgs:
        out = os.path.join(outdir, os.path.basename(s)[:-4] + ".png")
        # -D + forced square: makeicons' rendering flags. Its export flag is the deprecated
        # --export-png=; verified byte-identical output on 1.2.2.
        r = subprocess.run(
            ["inkscape", "-D", "-w", str(args.size), "-h", str(args.size), s,
             "--export-type=png", f"--export-filename={out}"],
            stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL, check=False)
        # A silent render failure would drop the icon from the glob below, shrinking the
        # denominator and PASSing on reduced coverage -- the worst failure mode for this tool.
        if r.returncode != 0 or not os.path.exists(out):
            failed.append(os.path.basename(s))
    if failed:
        sys.exit(f"inkscape failed to render {len(failed)} file(s): {', '.join(failed[:10])}")

    buckets = {"identical (<1)": 0, "AA-noise (1-3)": 0, "minor (3-8)": 0, "visible (>8)": 0}
    worst, missing = [], []
    for p in sorted(glob.glob(os.path.join(outdir, "*.png"))):
        name = os.path.basename(p)
        ref = os.path.join(WPT, f"{args.ref}x{args.ref}", name)
        if not os.path.exists(ref):
            missing.append(name)
            continue
        a = premultiply(Image.open(p).convert("RGBA").resize(
            (args.ref, args.ref), Image.LANCZOS))
        b = Image.open(ref).convert("RGBA")
        if b.size != (args.ref, args.ref):
            b = b.resize((args.ref, args.ref), Image.LANCZOS)
        m = mean_diff(a, premultiply(b))
        worst.append((m, name))
        if m < 1:
            buckets["identical (<1)"] += 1
        elif m < 3:
            buckets["AA-noise (1-3)"] += 1
        elif m < 8:
            buckets["minor (3-8)"] += 1
        else:
            buckets["visible (>8)"] += 1

    n = len(worst)
    print(f"\n=== {args.size}px regenerated -> downscaled to {args.ref} "
          f"-> vs shipped {args.ref}px PNG (premultiplied) ===")
    for k, v in buckets.items():
        print(f"  {k:18} {v:3d} / {n}")
    print("\n  worst 6:")
    for m, nm in sorted(worst, reverse=True)[:6]:
        print(f"     {nm:<26} {m:.2f}")
    if missing:
        print(f"\n  no reference PNG ({len(missing)}): {', '.join(missing)}")

    # Coverage must be asserted, not assumed: zero comparisons also yields zero visible
    # differences, which would otherwise PASS.
    ok = buckets["visible (>8)"] == 0 and n == len(svgs)
    if n != len(svgs):
        print(f"\n  COVERAGE FAILURE: compared {n} of {len(svgs)} icons "
              f"-- a PASS here would be meaningless")
    print(f"\n  VERDICT: {'PASS' if ok else 'FAIL'} - "
          f"{buckets['visible (>8)']} icon(s) visibly changed (must be 0)")
    if not args.keep:
        shutil.rmtree(outdir, ignore_errors=True)
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(main())
