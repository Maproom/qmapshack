#!/usr/bin/env python3
"""A/B a candidate colour: does it share the `ink` role, or get its own?

The question every new role faces. A colour that also appears alongside navy
cannot just be folded into `ink` -- that collapses two colours into one on dark
and the drawing loses the distinction. Only looking settles it.

  merged    candidate -> ink       (one colour; the distinction is lost)
  separate  candidate -> its own   (distinction kept, two lights on dark)

Decided #000000 -> its own role `lead` (142 icons drew with both). Reused for
#0000ff, which appears in 31 icons, 23 of them alongside navy.

Preview only, by substitution: colours already themed live in their class
defaults, the candidate is still a literal fill/stroke.

  ./roleab.py --color '#0000ff' --own '#66aaff' --out /tmp/blueab.html
"""

import argparse
import base64
import colorsys
import os
import re
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from palette import OPTOUT  # noqa: E402
from themesvg import TAG_RE, get_attr, norm, style_to_dict  # noqa: E402

DARK = "#353535"
INK = "#ccccff"
LEAD = "#e0e0e0"  # the shipped `lead`, so the sheet shows icons as they now render

STRIP = (
    (re.compile(r"<metadata\b.*?</metadata>", re.S), ""),
    (re.compile(r"<!--.*?-->", re.S), ""),
    (re.compile(r"<\?xml\b.*?\?>", re.S), ""),
    (re.compile(r"\s*\n\s*"), " "),
)


def strip(svg):
    """-> a data: URI for an <img>.

    NOT an inline <svg>. Inline, every icon's <style id="current-color-scheme">
    becomes a GLOBAL rule in the page, so .ink/.paper/.lead from one icon restyle
    every other icon -- including the untouched "today" column, which then shows
    colours it does not have. A data URI is its own document, so the styles are
    scoped. Still vector, still zoomable.
    """
    for pat, rep in STRIP:
        svg = pat.sub(rep, svg)
    return "data:image/svg+xml;base64," + base64.b64encode(svg.strip().encode()).decode()


def recolor(svg, candidate, color):
    """Theme for dark: class defaults per role, plus the literal `candidate` -> `color`."""
    m = re.search(r'(<style\b[^>]*\bid="current-color-scheme"[^>]*>)(.*?)(</style>)', svg, re.S)
    if m:
        # Rewrite each declaration by its class's role tokens, like the engine does.
        def fix(rule):
            name, body = rule.group(1), rule.group(2)
            roles = name.split("-")
            out, i = [], 0
            for decl in [d for d in body.split(";") if ":" in d]:
                prop = decl.split(":", 1)[0].strip()
                role = roles[i] if i < len(roles) else "ink"
                col = DARK if role.startswith("paper") else LEAD if role.startswith("lead") else INK
                out.append(f"{prop}:{col}")
                i += 1
            return f".{name}{{{';'.join(out)}}}"

        rules = re.sub(r"\.([\w-]+)\s*\{([^}]*)\}", fix, m.group(2))
        svg = svg[:m.start(2)] + rules + svg[m.end(2):]

    # Literal black, in both spellings, on fill and stroke only.
    def tag(m):
        t = m.group(0)
        for prop in ("fill", "stroke"):
            sd = style_to_dict(get_attr(t, "style"))
            if norm(sd.get(prop, "")) == candidate:
                sd[prop] = color
                t = re.sub(r'style="[^"]*"', 'style="' + ";".join(f"{k}:{v}" for k, v in sd.items()) + '"', t, count=1)
            elif norm(get_attr(t, prop) or "") == candidate:
                t = re.sub(r'\s' + prop + r'="[^"]*"', f' {prop}="{color}"', t, count=1)
        return t

    return TAG_RE.sub(tag, svg)


def hues(svg):
    """How many distinct saturated hues the icon draws with.

    A SORT ORDER, not a decision. Where the candidate is one swatch among several
    -- a palette, a multi-colour bar -- it is semantic like red and green and must
    stay literal rather than take a role. Many hues is a hint that this is such an
    icon, but it is only a hint: three hues is equally a red/green/blue marker set.
    The call is made by eye; this just floats the likely ones to the top.
    """
    body = re.sub(r"<style\b[^>]*>.*?</style>", "", svg, flags=re.S)
    out = set()
    for m in TAG_RE.finditer(body):
        sd = style_to_dict(get_attr(m.group(0), "style"))
        for prop in ("fill", "stroke"):
            v = norm(sd.get(prop, get_attr(m.group(0), prop) or ""))
            if not re.fullmatch(r"#[0-9a-f]{6}", v or ""):
                continue
            r, g, b = (int(v[i:i + 2], 16) / 255 for i in (1, 3, 5))
            h, lightness, s = colorsys.rgb_to_hls(r, g, b)
            if s > 0.45 and 0.15 < lightness < 0.85:
                out.add(round(h * 360 / 30) * 30)
    return len(out)


def uses(svg, candidate):
    body = re.sub(r"<style\b[^>]*>.*?</style>", "", svg, flags=re.S)
    for m in TAG_RE.finditer(body):
        sd = style_to_dict(get_attr(m.group(0), "style"))
        for prop in ("fill", "stroke"):
            if norm(sd.get(prop, get_attr(m.group(0), prop) or "")) == candidate:
                return True
    return False


CELL = """<div class="i"><div class="n">{name}<span class="h">{hues} hues</span></div>
<div class="r"><div class="c light"><img src="{light}" alt=""></div>
<div class="c dark"><img src="{merged}" alt=""></div>
<div class="c dark"><img src="{separate}" alt=""></div></div></div>"""

PAGE = """<style>
body{{background:#1b1b1b;color:#ddd;font:13px system-ui,sans-serif;margin:16px}}
h1{{font-size:16px}} .hdr{{position:sticky;top:0;background:#1b1b1b;padding:8px 0;z-index:1}}
.hdr span{{display:inline-block;width:104px;text-align:center;color:#999}}
.g{{display:flex;flex-wrap:wrap;gap:14px}}
.i{{width:320px}} .n{{color:#8ab;margin-bottom:3px;font-family:monospace}}
.h{{color:#a87;float:right}}
.r{{display:flex;gap:4px}} .c{{width:100px;height:100px;display:flex;align-items:center;
justify-content:center;border-radius:4px}} .c img{{width:64px;height:64px}}
.light{{background:#efefef}} .dark{{background:#353535}}
</style>
<h1>{cand} role A/B &mdash; {n} icons</h1>
<div class="hdr"><span>today (light)</span><span>merged: {cand}&rarr;ink</span>
<span>separate: {cand}&rarr;{own}</span></div>
<p style="color:#999;max-width:70ch">Sorted by distinct saturated hues. Where {cand} is one
swatch among several it is semantic, like red and green, and should stay literal instead of
taking a role &mdash; those need naming, not theming. The hue count only orders the page.</p>
<div class="g">{cells}</div>"""


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--dir", default="src/icons")
    ap.add_argument("--color", default="#0000ff", help="the candidate colour, still literal in the sources")
    ap.add_argument("--own", default="#66aaff", help="what it becomes on dark if it gets its own role")
    ap.add_argument("--out", default="/tmp/roleab.html")
    args = ap.parse_args()

    cells = []
    for name in sorted(os.listdir(args.dir)):
        if not name.endswith(".svg") or name[:-4] in OPTOUT:
            continue
        svg = open(os.path.join(args.dir, name), encoding="utf-8").read()
        cand = norm(args.color)
        if not uses(svg, cand):
            continue
        cells.append((hues(svg), CELL.format(
            name=name[:-4], hues=hues(svg), light=strip(svg),
            merged=strip(recolor(svg, cand, INK)),
            separate=strip(recolor(svg, cand, args.own)))))

    # Most hues first: the likeliest "leave it literal" cases lead.
    cells = [c for _, c in sorted(cells, key=lambda kv: -kv[0])]

    with open(args.out, "w", encoding="utf-8") as fh:
        fh.write(PAGE.format(n=len(cells), own=args.own, cand=args.color, cells="".join(cells)))
    print(f"{len(cells)} icons -> {args.out}")


if __name__ == "__main__":
    main()
