#!/usr/bin/env python3
"""Colour census + deviation report: why icons still show navy on a dark ground.

themesvg.py themes a colour only if it is an EXACT member of INK/PAPER. Anything
one bit off -- #000081, #0000b2, #00007f -- keeps its literal fill, so it renders
navy no matter what the theme does. This tool finds those.

Three passes, in order of what they answer:

  1. census    every colour token still literal in the sources, by use and by icon
  2. cluster   group them by RGB distance -- the "distinguished colours" of the set
  3. deviate   per cluster, how far each member sits from the cluster's dominant
               value, and which members are UNREACHABLE by the current ink/paper map

Only pass 3 is actionable: it is the list of colours that look navy to a human and
are invisible to themesvg.py.

  ./colorstat.py --dir src/icons                 # all three passes
  ./colorstat.py --dir src/icons --threshold 60  # looser clustering
  ./colorstat.py --dir src/icons --unreachable   # just the worklist
"""

import argparse
import math
import os
import re
import sys
from collections import defaultdict

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from themesvg import INK, PAPER, STYLE_ID, norm  # noqa: E402

# Colour-bearing SVG properties. `color` is included because it is what a
# currentColor fill resolves against -- an off-navy sitting in a class default is
# exactly as unthemed as an off-navy sitting in a fill.
PROPS = ("fill", "stroke", "stop-color", "color", "flood-color", "solid-color")

# style="fill:#abc" and fill="#abc" both occur; hygiene did not normalise them.
TOKEN_RE = re.compile(
    r"\b(" + "|".join(PROPS) + r")\s*[:=]\s*\"?\s*(#[0-9a-fA-F]{3,6})\b")


def rgb(c):
    c = norm(c)
    return tuple(int(c[i:i + 2], 16) for i in (1, 3, 5))


def dist(a, b):
    return math.sqrt(sum((x - y) ** 2 for x, y in zip(rgb(a), rgb(b))))


def luma(c):
    r, g, b = rgb(c)
    return 0.2126 * r + 0.7152 * g + 0.0722 * b


def scan(path):
    """-> {colour: count} for one icon, split into style-block and body use.

    The style block carries the class DEFAULTS (.ink{color:#000080}); those are
    themed and must not be reported as literals. Everything outside it is a
    colour the theme cannot reach.
    """
    with open(path, encoding="utf-8") as fh:
        text = fh.read()
    block = re.search(r'<style\b[^>]*\bid="' + re.escape(STYLE_ID) + r'"[^>]*>(.*?)</style>',
                      text, re.S)
    themed, body = defaultdict(int), defaultdict(int)
    if block:
        for _, col in TOKEN_RE.findall(block.group(1)):
            themed[norm(col)] += 1
        text = text[:block.start()] + text[block.end():]
    for _, col in TOKEN_RE.findall(text):
        body[norm(col)] += 1
    return themed, body


def cluster(colors, threshold):
    """Greedy, seeded by usage -- the most-used colour of a neighbourhood defines it.

    Deliberately not k-means: the question is not "what are k groups" but "which
    stray values orbit a colour the set already treats as canonical", and the
    canonical one is by construction the popular one.
    """
    # Seed with the themed anchors. Once a pass converts the exact matches to
    # currentColor they stop being literals, so a purely data-driven clustering
    # loses the very centre the strays orbit -- #000081 would become its own
    # cluster and report nothing. Seeding keeps a near-miss visible after the
    # colour it misses has been themed away.
    centers = [c for c in sorted(INK) + sorted(PAPER)]
    members = defaultdict(list)
    for col, n in sorted(colors.items(), key=lambda kv: (-kv[1], kv[0])):
        near = min(centers, key=lambda c: dist(c, col)) if centers else None
        if near is not None and dist(near, col) <= threshold:
            members[near].append((col, n))
        else:
            centers.append(col)
            members[col].append((col, n))
    return centers, members


def reachable(c):
    """Would themesvg.py theme this colour today?"""
    c = norm(c)
    return "ink" if c in INK else "paper" if c in PAPER else None


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--dir", default=os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
    ap.add_argument("--threshold", type=float, default=48.0,
                    help="RGB distance that still counts as the same colour (default 48)")
    ap.add_argument("--unreachable", action="store_true",
                    help="only the worklist: unthemed colours orbiting a themed one")
    args = ap.parse_args()

    svgs = sorted(f for f in os.listdir(args.dir) if f.endswith(".svg"))
    if not svgs:
        sys.exit(f"no svgs in {args.dir}")

    uses = defaultdict(int)
    icons = defaultdict(set)
    themed_uses = defaultdict(int)
    for name in svgs:
        themed, body = scan(os.path.join(args.dir, name))
        for col, n in body.items():
            uses[col] += n
            icons[col].add(name)
        for col, n in themed.items():
            themed_uses[col] += n

    centers, members = cluster(uses, args.threshold)

    if not args.unreachable:
        print(f"=== 1. census -- {len(svgs)} icons, {len(uses)} distinct literal colours ===\n")
        print(f"{'colour':>9}  {'uses':>5}  {'icons':>5}  {'luma':>5}  themed-by")
        for col, n in sorted(uses.items(), key=lambda kv: -kv[1]):
            print(f"{col:>9}  {n:5}  {len(icons[col]):5}  {luma(col):5.0f}  "
                  f"{reachable(col) or '-'}")

        print(f"\n=== 2. clusters (RGB distance <= {args.threshold:g}) ===\n")
        for c in centers:
            ms = sorted(members[c], key=lambda kv: -kv[1])
            if not ms:
                continue  # a seeded anchor fully themed away -- nothing literal left
            total = sum(n for _, n in ms)
            print(f"{c}  {total:5} uses  {len(ms):2} value(s)  luma {luma(c):3.0f}"
                  f"  [{reachable(c) or 'not themed'}]")

    print("\n=== 3. deviation from each cluster's dominant value ===\n"
          if not args.unreachable else "=== unreachable colours ===\n")

    worklist = []
    for c in centers:
        ms = sorted(members[c], key=lambda kv: -kv[1])
        strays = [(col, n) for col, n in ms if col != c]
        if not strays:
            continue
        if not args.unreachable:
            print(f"{c} ({uses[c]} uses, {reachable(c) or 'not themed'})")
        for col, n in strays:
            d = dist(c, col)
            r = reachable(col)
            # The bug: the cluster's centre is themed, this member is not. A human
            # reads them as one colour; the theme moves only one of them.
            broken = reachable(c) is not None and r is None
            if broken:
                worklist.append((col, n, c, d))
            if not args.unreachable:
                print(f"   {col}  d={d:5.1f}  {n:4} uses  {len(icons[col]):3} icons  "
                      f"{'<-- UNREACHABLE' if broken else ('themed:' + r if r else '')}")
        if not args.unreachable:
            print()

    print("=== worklist: unthemed colours inside a themed cluster ===\n")
    if not worklist:
        print("  none -- every colour near a themed anchor is itself themed")
    for col, n, c, d in sorted(worklist, key=lambda t: -t[1]):
        print(f"{col}  {n:4} uses  d={d:5.1f} from {c}  "
              f"{' '.join(sorted(os.path.splitext(i)[0] for i in icons[col]))}")
    print(f"\n{sum(n for _, n, _, _ in worklist)} uses across "
          f"{len({i for col, _, _, _ in worklist for i in icons[col]})} icons")


if __name__ == "__main__":
    main()
