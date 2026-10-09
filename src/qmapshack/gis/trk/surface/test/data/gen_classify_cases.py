#!/usr/bin/env python3
"""Generate classify_cases.tsv from the reference classification (osmsurface.py).

Usage: gen_classify_cases.py > classify_cases.tsv
Columns: highway, surface, tracktype, footway, label (Dutch), class, way type (Dutch).
"""
import os
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from osmsurface import SURFACE_NL, WEGTYPE, classify  # noqa: E402

highways = sorted(WEGTYPE) + ["", "motorway", "construction", "platform"]
surfaces = sorted(SURFACE_NL) + ["", "weird"]
cases = set()
for hw in highways:
    for s in surfaces:
        cases.add((hw, s, "", ""))
for hw in ("track", "path", "footway", "bridleway", "residential"):
    for s in ("", "weird", "gravel"):
        for tt in ("", "grade1", "grade2", "grade3", "grade4", "grade5", "grade9"):
            for fw in ("", "sidewalk", "crossing"):
                cases.add((hw, s, tt, fw))
for hw, s, tt, fw in sorted(cases):
    tags = {k: v for k, v in (("highway", hw), ("surface", s), ("tracktype", tt), ("footway", fw)) if v}
    naam, klasse, wegtype = classify(tags)
    print("\t".join((hw, s, tt, fw, naam, str(klasse), wegtype)))
