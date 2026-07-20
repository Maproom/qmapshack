#!/usr/bin/env python3
"""Make the UI icon SVGs renderable by Qt, and repair the SVG -> PNG pipeline.

Task 1, phase 2.1. See .notes/icons-task1-ui-svg-plan.md.

Per file, in this order (the order is load-bearing):
  1. strip EMPTY <flowRoot> blocks -- invisible Inkscape leftovers whose only effect is
     inflating the drawing bbox. Must happen BEFORE the fit, because fit-canvas keys off
     that bbox. Blocks containing actual text are left alone.
  2. object-to-path   -- outline text, so rendering no longer depends on fonts being installed
  3. fit-canvas-to-selection -- make the page equal the drawing. THIS is the fix: 314 of 325
     sources declare a 64x64 page with the artwork in some sub-rect, so Qt (which honours the
     page) draws it small and off-centre while inkscape -D (which crops to the drawing) does not.
  4. export plain SVG

Note it does NOT add a viewBox. fit-canvas sets width/height to the drawing, which is
sufficient -- verified: severe mismatches drop 151 -> 1 without one.

Runs inkscape under a throwaway INKSCAPE_PROFILE_DIR pinning /tools/bounding_box to *visual*,
because fit-canvas honours that preference while `-D` export always uses the visual
(stroke-inclusive) box. Mismatched, frames shift 1-2px. Your own Inkscape config is untouched.

  ./hygiene.py --dry-run           # render into a temp dir, report, change nothing
  ./hygiene.py                     # rewrite src/icons/*.svg in place
  ./hygiene.py --dir /some/dir     # e.g. to hygiene a couple of new icons in isolation

ORDER MATTERS: hygiene runs BEFORE naturalsize.py. fit-canvas-to-selection rewrites the root
element, so a later re-run would strip the viewBox naturalsize added and silently resize every
<pixmap> label. Files that already have a viewBox are therefore skipped. Note also that a re-run
is not byte-idempotent even on untouched files -- re-fitting drifts the last decimal (40.069157
-> 40.069153) -- so treat this as a one-time repair, not something to run in a loop.

Verify with uidiff.py afterwards: VISIBLE should drop 36 -> 17.

Requires: inkscape 1.x.
"""
import argparse
import concurrent.futures
import glob
import os
import re
import shutil
import subprocess
import sys
import tempfile

ICONS = "/home/oeichler/projects/qmapshack/src/icons"

# Sentinel: the file already went through naturalsize.py, so re-hygiening it would strip the
# viewBox it depends on. Not an error -- just nothing to do.
SKIPPED = "skipped"

# <flowRoot ...> ... </flowRoot>, non-greedy, across newlines.
FLOWROOT = re.compile(r"[ \t]*<flowRoot\b.*?</flowRoot>\s*", re.S)
FLOWPARA = re.compile(r"<flowPara[^>]*>(.*?)</flowPara>", re.S)


def strip_empty_flowroots(text):
    """Remove flowRoot blocks that contain no text. Returns (new_text, n_stripped)."""
    n = 0

    def repl(m):
        nonlocal n
        block = m.group(0)
        if "".join(FLOWPARA.findall(block)).strip():
            return block  # has real text -- leave it alone
        n += 1
        return ""

    return FLOWROOT.sub(repl, text), n


def make_profile(tmp):
    """Throwaway inkscape profile pinning the bbox preference to visual (0)."""
    d = os.path.join(tmp, "profile")
    os.makedirs(d, exist_ok=True)
    with open(os.path.join(d, "preferences.xml"), "w") as f:
        f.write('<inkscape version="1.2.2" '
                'xmlns:inkscape="http://www.inkscape.org/namespaces/inkscape">\n'
                '  <group id="tools" bounding_box="0"/>\n'
                '</inkscape>\n')
    return d


def process(path, profile):
    """Hygiene one file in place. Returns (name, n_flowroots, error_or_None).

    Skips anything already carrying a viewBox: that is naturalsize.py's output, and
    fit-canvas-to-selection would silently strip it -- see SKIPPED below.
    """
    name = os.path.basename(path)
    try:
        raw = open(path, encoding="utf-8", errors="replace").read()
    except OSError as e:
        return name, 0, f"read failed: {e}"

    head = raw[:raw.index(">", raw.index("<svg"))] if "<svg" in raw else ""
    if "viewBox" in head:
        return name, 0, SKIPPED

    cleaned, n = strip_empty_flowroots(raw)
    if n:
        open(path, "w", encoding="utf-8").write(cleaned)

    env = dict(os.environ, INKSCAPE_PROFILE_DIR=profile)
    actions = ("select-all:all;object-to-path;"
               "select-all:all;fit-canvas-to-selection;"
               f"export-filename:{path};export-plain-svg;export-overwrite;export-do")
    r = subprocess.run(["inkscape", f"--actions={actions}", path],
                       stdout=subprocess.DEVNULL, stderr=subprocess.PIPE, env=env)
    if r.returncode != 0:
        return name, n, f"inkscape exit {r.returncode}: {r.stderr.decode()[:120]}"
    return name, n, None


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--dir", default=ICONS, help=f"SVG directory (default {ICONS})")
    ap.add_argument("--dry-run", action="store_true",
                    help="work on a temp copy; leave the repo untouched")
    ap.add_argument("--jobs", type=int, default=os.cpu_count() or 4)
    args = ap.parse_args()

    src = args.dir
    svgs = sorted(glob.glob(os.path.join(src, "*.svg")))
    if not svgs:
        sys.exit(f"no SVGs in {src}")

    tmp = tempfile.mkdtemp(prefix="hygiene")
    try:
        if args.dry_run:
            work = os.path.join(tmp, "svg")
            os.makedirs(work)
            for s in svgs:
                shutil.copy2(s, work)
            svgs = sorted(glob.glob(os.path.join(work, "*.svg")))
            print(f"dry run: working on a copy in {work}")

        profile = make_profile(tmp)
        print(f"hygiene: {len(svgs)} files, {args.jobs} jobs "
              f"(bbox pref pinned to visual, your config untouched)")

        results = []
        with concurrent.futures.ThreadPoolExecutor(max_workers=args.jobs) as ex:
            futs = {ex.submit(process, s, profile): s for s in svgs}
            for fut in concurrent.futures.as_completed(futs):
                results.append(fut.result())

        stripped = [(n, c) for n, c, e in results if c]
        skipped = [n for n, c, e in results if e is SKIPPED]
        errors = [(n, e) for n, c, e in results if e and e is not SKIPPED]

        print(f"\n  empty flowRoot stripped from {len(stripped)} file(s), "
              f"{sum(c for _, c in stripped)} block(s) total")
        for n, c in sorted(stripped):
            print(f"     {n} ({c})")

        if skipped:
            print(f"\n  {len(skipped)} file(s) already sized by naturalsize.py -- left alone "
                  f"(re-hygiening would strip their viewBox)")

        if errors:
            print(f"\n  FAILURES ({len(errors)}):")
            for n, e in sorted(errors):
                print(f"     {n}: {e}")
            # A silent inkscape failure leaves a file un-hygiened and Qt-unrenderable.
            sys.exit(1)

        print(f"\n  OK: {len(results)} files processed, 0 failures")
        if args.dry_run:
            print(f"  dry run -- {src} is unchanged. Hygiened copies kept in:\n     {work}")
            print(f"  verify them without touching the repo:\n"
                  f"     ./uidiff.py --svgdir {work} --max-visible 17")
        else:
            print(f"  {src} rewritten in place. Verify:\n"
                  f"     ./uidiff.py --max-visible 17")
        return 0
    finally:
        # On a dry run the copies ARE the deliverable -- keep them for uidiff.
        if not args.dry_run:
            shutil.rmtree(tmp, ignore_errors=True)


if __name__ == "__main__":
    sys.exit(main())
