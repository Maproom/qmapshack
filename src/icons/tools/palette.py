#!/usr/bin/env python3
"""Normalise the UI icon palette so one value per hue works on BOTH grounds.

Phase 7a, Oliver's decision: rather than teach the engine a per-scheme accent
table, fix the source once. Measured against Qt Fusion's window colours
(#efefef luma 239 / #353535 luma 53) -- a colour needs delta > 40 to each:

  #ff0000  luma  54  ->  dark delta   1   INVISIBLE (luma match with the ground)
  #ff5555  luma 121  ->  light 118 / dark  68   works on both
  #008000  luma  92  ->  dark delta  39   weak
  #55d400  luma 170  ->  light  69 / dark 117   works on both  (already used 72x)
  #0000ff  luma  18  ->  dark delta  35   weak
  #5555ff  luma  97  ->  light 142 / dark  44   works on both

THIS DELIBERATELY CHANGES THE LIGHT APPEARANCE (#ff0000 -> #ff5555 is visibly
lighter). That is accepted, and is why it is one reviewable step rather than a
smear across a mechanical diff -- same reasoning as phase 2.4.

Not everything is mapped, on purpose. #800000 / #550000 / #445500 are shading
and outline variants, not the primary accent; collapsing them would flatten the
art. Review on the contact sheet and extend deliberately.

  ./palette.py --dir src/icons            # dry run: what would change
  ./palette.py --dir src/icons --write
"""

import argparse
import os
import re
import sys

# The 21 icons that carry their OWN coloured background (Mime*/*Project/Off/
# ToWksUnchecked). Their content sits on THEIR background, not the app's, so
# neither theming nor accent normalisation applies -- MimeDemWCS is #326480 with
# white content; flip the content and you get black on mid-blue. Derived from
# `bgaudit.py --dir src/icons`, keeping every non-neutral background colour.
#
# MimeIMG was MISSED by that derivation: its tile is #000080, and a navy background
# reads as neutral, so the "non-neutral" test dropped it while its 9 siblings stayed.
# Themed, its tile went light and its white content went dark -- inverted from the rest
# of the family. The family is identifiable by geometry, not colour: every Mime tile is
# opaque=99.7% ring=95.2% in bgaudit. The navy SOLIDS at ring=98.4% (Export, ReferenceMap)
# are objects, not tiles, and stay themed. (QMapShack/QMapTool are opted out below as brand logos.)
OPTOUT = {
    "2NavProject", "DBProject", "FitProject", "GpxProject", "LogProject",
    "MimeDemVRT", "MimeDemWCS", "MimeGEMF", "MimeIMG", "MimeJNX", "MimeMAP", "MimeRMAP",
    "MimeTMS", "MimeVRT", "MimeWMTS", "Off", "QlbProject", "QmsProject",
    "SlfProject", "SmlProject", "TcxProject",
    "ToWksChecked", "ToWksPartially", "ToWksUnchecked",
    # Colour is semantic and must read the same in both themes: warning/stop signs,
    # search-provider brand marks, and the colour-picker swatch.
    "Attention", "Error", "NoGo", "NotPossible", "Record",
    "SearchGeonames", "SearchGoogle", "SearchNominatim", "SelectColor", "WptAvoid",
    # Fixed appearance in both themes: waypoint-type badges and rating stars carry a
    # white disc and neutral-grey frame that vanish on the dark ground once themed.
    "RatingStar", "RatingStarHalf", "RatingStarEmpty", "UnFocus",
    "waypoint_final", "waypoint_parking", "waypoint_physical", "waypoint_reference",
    "waypoint_trailhead", "waypoint_virtual",
    # Brand logos: navy map on a white card, kept identical in both themes.
    "QMapShack", "QMapTool",
    # Detailed object icons that theming breaks (cylinder + wordmark washes out on dark).
    "MySQLNoConn",
    # Speech-bubble lines sit on the icon's own fixed yellow bubble, not the app ground, so a
    # themed `lead` flips them light. They were pinned literal black per-element; once the build
    # applies the roles itself that pin is overwritten on every edit, so the icon has to be
    # fixed-appearance to hold.
    "Hint",
    # Card-and-stack icons: a white card with dark content plus grey frames, fixed in both
    # themes. Theming split the card (content wanted fixed-dark, frames wanted light) in a
    # way one role set can't durably express -- so they are hand-managed literal-colour icons.
    "MapLayer",
    "ToggleDatabase", "ToggleDem", "ToggleDocks", "ToggleGis", "ToggleMaps",
    "TogglePoi", "ToggleRealTime", "ToggleRouter",
}

# Multi-colour swatch bars: themed like any icon (they carry navy `ink` content that
# must survive dark mode), but every accent hue is a literal palette sample, so no
# accent mapping applies. themesvg.py keeps #0000ff off the `mark` role for these;
# palette.py leaves all their accents literal (typo fixes still apply).
KEEP_BLUE = {
    "PasteNormal",
    "ToolBar",
    "ToolBarSetup",
}

# A typo, not a colour. CSrcCourse alone carries #000081 next to #000080 -- luma
# delta 0.072, invisible to a human on light. But only #000080 is neutral-mapped,
# so on dark 8 facets go light and 4 stay navy and the compass falls apart. This
# is the whole of "the shape is unrecognizable"; nothing could have caught it
# before theming.
TYPOS = {
    "#000081": "#000080",
    # ToggleDocks, ToggleRouter. Also navy-that-missed: both icons use #000080 elsewhere,
    # and at delta 25 it is invisible on light but splits the icon on dark.
    "#000099": "#000080",
}

# One value per hue, each measured to clear delta>40 on both grounds.
ACCENTS = {
    "#ff0000": "#ff5555",   # 164 uses -- luma-matched the dark ground
    "#008000": "#55d400",   # 77
    "#00ff00": "#55d400",   # 39
    "#57e83c": "#55d400",   # 3
    "#668000": "#55d400",   # PathGreen's olive primary
    "#0000ff": "#5555ff",   # 74
}

COLOR_RE = re.compile(r"#[0-9a-fA-F]{6}")


def normalise(text, accents=True):
    """Apply typo fixes and (optionally) the accent palette. Text-only, no reparse."""
    mapping = dict(TYPOS)
    if accents:
        mapping.update(ACCENTS)

    def repl(m):
        return mapping.get(m.group(0).lower(), m.group(0))

    return COLOR_RE.sub(repl, text)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--dir", default=os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
    ap.add_argument("--write", action="store_true")
    ap.add_argument("--typos-only", action="store_true",
                    help="apply only the invisible typo fixes, not the accents")
    args = ap.parse_args()

    svgs = sorted(f for f in os.listdir(args.dir) if f.endswith(".svg"))
    if not svgs:
        sys.exit(f"no svgs in {args.dir}")

    changed = skipped = 0
    hits = {}
    for name in svgs:
        stem = name[:-4]
        path = os.path.join(args.dir, name)
        with open(path, encoding="utf-8") as fh:
            src = fh.read()
        if stem in OPTOUT:
            skipped += 1
            continue
        accents = not args.typos_only and stem not in KEEP_BLUE
        out = normalise(src, accents=accents)
        if out == src:
            continue
        changed += 1
        for old in list(TYPOS) + (list(ACCENTS) if accents else []):
            n = len(re.findall(re.escape(old), src, re.I))
            if n:
                hits[old] = hits.get(old, 0) + n
        if args.write:
            with open(path, "w", encoding="utf-8") as fh:
                fh.write(out)

    print(f"{len(svgs)} icons | {changed} changed | {skipped} skipped (self-backgrounded)")
    print("\nreplacements:")
    for old, n in sorted(hits.items(), key=lambda kv: -kv[1]):
        new = TYPOS.get(old) or ACCENTS.get(old)
        print(f"  {old} -> {new}   {n} uses")
    if not args.write:
        print("\n(dry run -- nothing written; pass --write)")


if __name__ == "__main__":
    main()
