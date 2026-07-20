#!/usr/bin/env python3
"""Give the <pixmap>-referenced icons an explicit natural size, so they can be swapped to SVG.

Task 1, phase 5.2. Run AFTER hygiene.py -- it depends on the fitted canvas, and re-running
hygiene would undo it.

Why: uic turns <iconset> into `icon.addFile(path, QSize(), ...)`, so QIcon asks the qsvgicon
engine for whatever size it needs and the natural size is irrelevant. But it turns <pixmap> into
`label->setPixmap(QPixmap(path))`, and a QLabel makes no size request -- so the SVG renders at
its natural size. After hygiene that is the drawing bbox in the file's own units (arbitrary:
Apply 40.07, FilterSplitTrack 61.78), which would resize the label.

Fix: give those icons a viewBox and an explicit width/height matching the size the label shows.
Setting width/height WITHOUT a viewBox would crop rather than scale -- the viewBox is what turns
size into metadata.

Non-square sources are stretched to a square, which is exactly what makeicons already does
(`-D -w N -h N` crops to the drawing then forces N x N), so the raster it replaces has the same
stretch.

  ./naturalsize.py --dry-run
  ./naturalsize.py

Verify with `./pixmapcheck.py`: QPixmap(svg).size() must equal the PNG it replaces.
"""
import argparse
import collections
import glob
import os
import re
import sys

REPO = "/home/oeichler/projects/qmapshack"
ICONS = f"{REPO}/src/icons"

# PointHide is shown at 48 in four filter dialogs and at 32 in IRangeToolSetup.ui only.
# 48 wins; that one 32px ref stays PNG rather than resizing a setup dialog.
FORCE = {"PointHide": 48}


def pixmap_usage():
    """{icon: {sizes}} for every icon referenced by a <pixmap> tag in any .ui."""
    use = collections.defaultdict(set)
    for ui in glob.glob(f"{REPO}/src/**/*.ui", recursive=True):
        for m in re.finditer(r'<pixmap[^>]*>:/icons/(32x32|48x48)/([^<]+)\.png</pixmap>',
                             open(ui).read()):
            use[m.group(2)].add(int(m.group(1).split("x")[0]))
    return use


def apply_size(path, size, dry):
    """Add a viewBox from the current width/height, then set width/height to size x size."""
    s = open(path, encoding="utf-8").read()
    head = s[:s.index(">", s.index("<svg"))]

    if "viewBox" in head:
        return None, "already has a viewBox -- refusing to guess"

    w = re.search(r'\bwidth="([\d.]+)(?:px)?"', head)
    h = re.search(r'\bheight="([\d.]+)(?:px)?"', head)
    if not (w and h):
        return None, "no width/height on the root element"
    W, H = float(w.group(1)), float(h.group(1))

    new = head
    new = re.sub(r'\bwidth="[^"]*"', f'width="{size}"', new, count=1)
    new = re.sub(r'\bheight="[^"]*"', f'height="{size}"', new, count=1)
    # viewBox carries the old user-unit extent, so the art scales into the new width/height
    new = new.replace("<svg", f'<svg viewBox="0 0 {W:.6g} {H:.6g}"', 1)

    if not dry:
        open(path, "w", encoding="utf-8").write(new + s[len(head):])
    return (W, H), None


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--dry-run", action="store_true")
    args = ap.parse_args()

    use = pixmap_usage()
    if not use:
        sys.exit("no <pixmap> icon refs found -- did the .ui layout change?")

    print(f"  {len(use)} icons are referenced by a <pixmap> tag")
    done, skipped, errors = [], [], []
    for name in sorted(use):
        sizes = sorted(use[name])
        if len(sizes) > 1 and name not in FORCE:
            skipped.append((name, sizes, "used at several sizes and not in FORCE"))
            continue
        size = FORCE.get(name, sizes[0])
        path = os.path.join(ICONS, f"{name}.svg")
        if not os.path.exists(path):
            errors.append((name, "no SVG source"))
            continue
        old, err = apply_size(path, size, args.dry_run)
        if err:
            errors.append((name, err))
        else:
            done.append((name, old, size, sizes))

    for name, old, size, sizes in done:
        note = f"  (shown at {sizes}, forced {size})" if len(sizes) > 1 else ""
        print(f"     {name:28} {old[0]:.2f}x{old[1]:.2f} -> {size}x{size}{note}")
    for name, sizes, why in skipped:
        print(f"     SKIP {name}: {why} {sizes}")
    for name, why in errors:
        print(f"     FAIL {name}: {why}")

    print(f"\n  {len(done)} sized, {len(skipped)} skipped, {len(errors)} failed"
          + ("   (dry run, nothing written)" if args.dry_run else ""))
    return 1 if errors else 0


if __name__ == "__main__":
    sys.exit(main())
