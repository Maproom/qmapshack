#!/usr/bin/env python3
"""Does each UI icon SVG still render to its shipped PNG?

Task 1 gate. See .notes/icons-task1-ui-svg-plan.md.

Renders every src/icons/*.svg with the same flags makeicons uses (-D, forced square) and
compares against the committed 32x32/<name>.png, premultiplied. Also asserts the SVG <-> PNG
correspondence is complete in BOTH directions -- a missing source is as much a defect as a
mismatch, and neither shows up in a bucket count.

Why this exists: makeicons is currently a footgun. On the tree today 36 of 327 icons do NOT
match their own source, so running the export today corrupts them. All 23 empty-flowRoot files
are stale (Inkscape silently added an empty text box, the drawing bbox inflated, -D now crops
differently, nobody re-exported), plus 12 text-bearing ones whose fonts no longer resolve the
same way. Worst: Save.png at 166/255.

Gates per phase (see the plan):

  # baseline -- documents the breakage, must still pass (phase 0 + 1.1 done: no orphan flags)
  ./uidiff.py --max-visible 36

  # after 2.1 (hygiene applied, PNGs NOT yet regenerated)
  ./uidiff.py --max-visible 17

  # after 2.4 (PNGs regenerated) -- identical by construction
  ./uidiff.py

Exits non-zero on: any render failure, any orphan beyond the allowance, or VISIBLE over
--max-visible. It names the VISIBLE ones -- that list is the phase-2.2 eyeball worklist.

Requires: inkscape 1.x, python3-pil.
"""
import argparse
import concurrent.futures
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

ICONS = "/home/oeichler/projects/qmapshack/src/icons"


def premultiply(img):
    """Alpha-weight the colour channels.

    Raw RGB is undefined padding where alpha=0; comparing it directly reports garbage. This
    once produced '140/203 changed' on a set where the true answer was 0.
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


def render(svg, outdir, size):
    out = os.path.join(outdir, os.path.basename(svg)[:-4] + ".png")
    # -D + forced square: makeicons' rendering flags. Its export flag is the deprecated
    # --export-png=; verified byte-identical output on 1.2.2.
    r = subprocess.run(
        ["inkscape", "-D", "-w", str(size), "-h", str(size), svg,
         "--export-type=png", f"--export-filename={out}"],
        stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    ok = r.returncode == 0 and os.path.exists(out)
    return os.path.basename(svg), ok


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--svgdir", default=ICONS, help="SVGs to render (default: the repo's)")
    ap.add_argument("--refdir", default=None,
                    help="reference PNGs (default: <repo icons>/<ref>x<ref>)")
    ap.add_argument("--ref", type=int, default=32, help="reference size (default 32)")
    ap.add_argument("--max-visible", type=int, default=0,
                    help="fail if more than N icons differ visibly (default 0)")
    ap.add_argument("--allow-orphan-svg", type=int, default=0,
                    help="tolerate N SVGs with no reference PNG")
    ap.add_argument("--allow-orphan-png", type=int, default=0,
                    help="tolerate N reference PNGs with no SVG source")
    ap.add_argument("--jobs", type=int, default=os.cpu_count() or 4)
    args = ap.parse_args()

    refdir = args.refdir or os.path.join(ICONS, f"{args.ref}x{args.ref}")
    if not os.path.isdir(refdir):
        sys.exit(f"no reference dir: {refdir}")
    if os.path.realpath(args.svgdir) == os.path.realpath(refdir):
        sys.exit("--svgdir and the reference dir are the same; refusing")

    svgs = sorted(glob.glob(os.path.join(args.svgdir, "*.svg")))
    if not svgs:
        sys.exit(f"no SVGs in {args.svgdir}")

    tmp = tempfile.mkdtemp(prefix="uidiff")
    try:
        print(f"rendering {len(svgs)} SVGs at {args.ref}px ({args.jobs} jobs)...")
        failed = []
        with concurrent.futures.ThreadPoolExecutor(max_workers=args.jobs) as ex:
            for name, ok in ex.map(lambda s: render(s, tmp, args.ref), svgs):
                if not ok:
                    failed.append(name)
        if failed:
            # A silent render failure would drop the icon from the comparison entirely,
            # shrinking the denominator and passing on reduced coverage.
            sys.exit(f"inkscape failed on {len(failed)} file(s): {', '.join(failed[:10])}")

        svg_names = {os.path.basename(s)[:-4] for s in svgs}
        png_names = {os.path.basename(p)[:-4] for p in glob.glob(os.path.join(refdir, "*.png"))}
        orphan_svg = sorted(svg_names - png_names)   # source with no shipped raster
        orphan_png = sorted(png_names - svg_names)   # raster with no source -> cannot swap

        buckets = {"identical (<1)": 0, "AA-noise (1-3)": 0, "minor (3-8)": 0, "VISIBLE (>8)": 0}
        visible, worst = [], []
        for name in sorted(svg_names & png_names):
            a = premultiply(Image.open(os.path.join(tmp, name + ".png")))
            b = Image.open(os.path.join(refdir, name + ".png"))
            if a.size != (args.ref, args.ref):
                a = a.resize((args.ref, args.ref), Image.LANCZOS)
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
                buckets["VISIBLE (>8)"] += 1
                visible.append((m, name))

        n = len(worst)
        print(f"\n=== {args.svgdir} rendered at {args.ref}px vs {refdir} (premultiplied) ===")
        for k, v in buckets.items():
            print(f"  {k:18} {v:3d} / {n}")

        if visible:
            print(f"\n  VISIBLY different ({len(visible)}) -- the eyeball worklist:")
            for m, name in sorted(visible, reverse=True):
                print(f"     {name:<28} {m:5.1f}")
        if orphan_svg:
            print(f"\n  SVG with no {args.ref}px PNG ({len(orphan_svg)}): {', '.join(orphan_svg)}")
        if orphan_png:
            print(f"\n  PNG with no SVG source ({len(orphan_png)}): {', '.join(orphan_png)}")
            print("     -> nothing to swap these to; see phase 0")

        problems = []
        if buckets["VISIBLE (>8)"] > args.max_visible:
            problems.append(f"{buckets['VISIBLE (>8)']} visibly different "
                            f"(allowed {args.max_visible})")
        if len(orphan_svg) > args.allow_orphan_svg:
            problems.append(f"{len(orphan_svg)} orphan SVG (allowed {args.allow_orphan_svg})")
        if len(orphan_png) > args.allow_orphan_png:
            problems.append(f"{len(orphan_png)} orphan PNG (allowed {args.allow_orphan_png})")

        print()
        if problems:
            print("  VERDICT: FAIL - " + "; ".join(problems))
            return 1
        print(f"  VERDICT: PASS - {n} compared, "
              f"{buckets['VISIBLE (>8)']} visibly different (allowed {args.max_visible})")
        return 0
    finally:
        shutil.rmtree(tmp, ignore_errors=True)


if __name__ == "__main__":
    sys.exit(main())
