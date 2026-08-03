#!/usr/bin/env python3
"""Reframe every UI icon onto one fixed 0 0 64 64 artboard.

Phase 4 of the icon unification. Replaces the per-icon fit-canvas look (each page ==
its own drawing, ~160 distinct sizes) with a single square artboard the whole set
shares. The drawing is scaled uniformly to fill the box on its longest side and
centred -- "fill / 64", so on-screen size is unchanged; only the artboard becomes
uniform. Aspect ratio is preserved (uniform scale, no distortion).

Mechanism: a wrapping <g transform="translate(..) scale(..)"> plus viewBox="0 0 64 64".
Verified to survive svghygiene's object-to-path + plain-svg export unchanged (that is
why svghygiene's fit-canvas step is retired in the same phase -- otherwise the build
would re-fit the page back to the drawing and undo this).

Visual bbox (stroke-inclusive), pinned like svghygiene, so a stroke at the edge is not
clipped by the box.

  ./reframe.py --dir ../../src/icons --dry-run    # report scale/offset per icon
  ./reframe.py --dir ../../src/icons              # rewrite in place

Requires: inkscape 1.x.
"""
import argparse
import concurrent.futures
import os
import re
import subprocess
import sys
import tempfile

BOX = 64.0
SVG_OPEN = re.compile(r"<svg\b[^>]*?>", re.S)


def make_profile(tmp):
    """Throwaway inkscape profile pinning the bbox preference to visual (0), as svghygiene does."""
    d = os.path.join(tmp, "profile")
    os.makedirs(d, exist_ok=True)
    with open(os.path.join(d, "preferences.xml"), "w") as f:
        f.write('<inkscape version="1.x" xmlns:inkscape="http://www.inkscape.org/namespaces/inkscape">\n'
                '  <group id="tools" bounding_box="0"/>\n</inkscape>\n')
    return d


def normalize_for_query(text):
    """Strip width/height so the bbox comes back in CONTENT (viewBox) units, not display px.

    An icon with a viewBox whose aspect differs from its width/height (e.g. viewBox 61x54
    shown at 32x30) makes --query-all report display px, which do not match the coordinates
    the paths are drawn in -- reframing on those numbers scales the drawing wrong. Removing
    width/height (adding a viewBox from them if there is none) makes 1 user unit == 1 content
    unit, so the query lands in the same space as the paths.
    """
    m = SVG_OPEN.search(text)
    tag = m.group(0)
    if 'viewBox="' not in tag:
        w = re.search(r'\bwidth="([\d.]+)', tag)
        h = re.search(r'\bheight="([\d.]+)', tag)
        if w and h:
            tag = tag[:-1] + f' viewBox="0 0 {w.group(1)} {h.group(1)}">'
    tag = re.sub(r'\s+(width|height)="[^"]*"', "", tag)
    return text[:m.start()] + tag + text[m.end():]


def visual_bbox(text, tmpdir, tag, env):
    """The drawing's visual bbox in CONTENT units: (x, y, w, h). Queried on a normalized copy."""
    q = os.path.join(tmpdir, f"q_{tag}.svg")
    with open(q, "w", encoding="utf-8") as f:
        f.write(normalize_for_query(text))
    out = subprocess.check_output(["inkscape", "--query-all", q], text=True,
                                  stderr=subprocess.DEVNULL, env=env)
    os.remove(q)
    first = out.splitlines()[0].split(",")
    return [float(v) for v in first[1:5]]


def reframe_text(text, bbox):
    bx, by, bw, bh = bbox
    if bw <= 0 or bh <= 0:
        raise ValueError(f"degenerate bbox {bbox}")
    scale = BOX / max(bw, bh)
    tx = BOX / 2 - scale * (bx + bw / 2)
    ty = BOX / 2 - scale * (by + bh / 2)
    m = SVG_OPEN.search(text)
    tag = re.sub(r'\s+(width|height|viewBox)="[^"]*"', "", m.group(0))
    tag = tag[:-1] + ' width="64" height="64" viewBox="0 0 64 64">'
    inner = text[m.end():]
    close = inner.rfind("</svg>")
    g = f'<g transform="translate({tx:.4f},{ty:.4f}) scale({scale:.6f})">'
    return text[:m.start()] + tag + g + inner[:close] + "</g></svg>", scale


def one(path, env, write, tmpdir):
    name = os.path.basename(path)
    try:
        text = open(path, encoding="utf-8").read()
        if 'viewBox="0 0 64 64"' in text and "translate" in SVG_OPEN.sub("", text)[:200]:
            return name, None, "already 0 0 64 64"
        bbox = visual_bbox(text, tmpdir, name[:-4], env)
        out, scale = reframe_text(text, bbox)
        if write:
            open(path, "w", encoding="utf-8").write(out)
        return name, scale, None
    except Exception as e:  # noqa: BLE001 -- report and continue, never abort the batch
        return name, None, f"ERROR: {e}"


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--dir", default=os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
    ap.add_argument("--dry-run", action="store_true")
    ap.add_argument("--jobs", type=int, default=os.cpu_count() or 4)
    ap.add_argument("files", nargs="*", help="specific icons; default: every *.svg in --dir")
    args = ap.parse_args()

    svgs = ([os.path.join(args.dir, f) for f in args.files] if args.files else
            sorted(os.path.join(args.dir, f) for f in os.listdir(args.dir) if f.endswith(".svg")))
    if not svgs:
        sys.exit(f"no svgs in {args.dir}")

    tmp = tempfile.mkdtemp(prefix="reframe")
    env = dict(os.environ, INKSCAPE_PROFILE_DIR=make_profile(tmp))
    try:
        done = errs = skip = 0
        scales = []
        with concurrent.futures.ThreadPoolExecutor(max_workers=args.jobs) as ex:
            for name, scale, err in ex.map(lambda p: one(p, env, not args.dry_run, tmp), svgs):
                if err and err.startswith("ERROR"):
                    errs += 1
                    print(f"  {name}: {err}", file=sys.stderr)
                elif err:
                    skip += 1
                else:
                    done += 1
                    scales.append(scale)
        print(f"{len(svgs)} icons | {done} reframed | {skip} already framed | {errs} errors")
        if scales:
            scales.sort()
            print(f"scale range: {scales[0]:.3f} .. {scales[-1]:.3f} "
                  f"(median {scales[len(scales)//2]:.3f}; <1 shrinks, >1 grows to fill 64)")
        if args.dry_run:
            print("\n(dry run -- nothing written)")
        return 1 if errs else 0
    finally:
        import shutil
        shutil.rmtree(tmp, ignore_errors=True)


if __name__ == "__main__":
    sys.exit(main())
