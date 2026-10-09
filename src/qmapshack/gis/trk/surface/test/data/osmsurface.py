# Reference implementation the C++ surface classification and cell lookup were ported from.
# Copyright (C) 2026 Gert Pellin <gert@pellin.be>, GPL-3.0-or-later. Used only to generate test data.
"""Surface and way type along a route, from an OSM .pbf extract (needs pyosmium).

Shared by loop.py (scoring) and surface.py (annotating GPX files for QMapShack).
Classes follow what a walker feels, like Komoot's "Ondergronden":
  0 verhard      asphalt, concrete, cobbles/pavers, and roads without a surface tag
  1 halfverhard  compacted, fine gravel, gravel, tracktype grade2
  2 onverhard    dirt, ground, grass, sand, rock, grade3-5, paths without a surface tag
  3 onbekend     tracks with neither surface nor tracktype
"""
import math
from collections import Counter

VERHARD, HALFVERHARD, ONVERHARD, ONBEKEND = 0, 1, 2, 3
KLASSE_NAAM = {VERHARD: "verhard", HALFVERHARD: "halfverhard", ONVERHARD: "onverhard", ONBEKEND: "onbekend"}

SURFACE_NL = {
    "asphalt": ("asfalt", VERHARD), "paved": ("verhard", VERHARD), "chipseal": ("asfalt", VERHARD),
    "concrete": ("beton", VERHARD), "concrete:lanes": ("betonsporen", VERHARD),
    "concrete:plates": ("betonplaten", VERHARD), "paving_stones": ("klinkers", VERHARD),
    "sett": ("kasseien", VERHARD), "cobblestone": ("kasseien", VERHARD),
    "unhewn_cobblestone": ("kasseien", VERHARD), "bricks": ("klinkers", VERHARD),
    "metal": ("metaal", VERHARD), "wood": ("hout", VERHARD),
    "compacted": ("verdicht steenslag", HALFVERHARD), "fine_gravel": ("fijn grind", HALFVERHARD),
    "gravel": ("grind", HALFVERHARD), "pebblestone": ("kiezel", HALFVERHARD),
    "grass_paver": ("grasdallen", HALFVERHARD),
    "unpaved": ("onverhard", ONVERHARD), "dirt": ("aarde", ONVERHARD), "earth": ("aarde", ONVERHARD),
    "ground": ("natuurlijke bodem", ONVERHARD), "grass": ("gras", ONVERHARD), "mud": ("modder", ONVERHARD),
    "sand": ("zand", ONVERHARD), "woodchips": ("houtsnippers", ONVERHARD), "rock": ("rots", ONVERHARD),
    "stepping_stones": ("stapstenen", ONVERHARD),
}
TRACKTYPE = {"grade1": ("verhard", VERHARD), "grade2": ("halfverhard", HALFVERHARD),
             "grade3": ("onverhard", ONVERHARD), "grade4": ("onverhard", ONVERHARD),
             "grade5": ("onverhard", ONVERHARD)}
WEGTYPE = {
    "path": "pad", "footway": "pad", "bridleway": "pad", "steps": "trap",
    "track": "bos-/veldweg", "cycleway": "fietspad",
    "residential": "straat", "living_street": "straat", "pedestrian": "straat",
    "unclassified": "weg", "service": "weg", "tertiary": "weg", "tertiary_link": "weg",
    "secondary": "drukke weg", "secondary_link": "drukke weg", "primary": "drukke weg",
    "primary_link": "drukke weg", "trunk": "drukke weg",
}


def classify(tags):
    """(ondergrond in het Nederlands, klasse, wegtype) for a way's tags."""
    hw = tags.get("highway", "")
    wegtype = WEGTYPE.get(hw, hw or "?")
    surface = tags.get("surface", "")
    if surface in SURFACE_NL:
        naam, klasse = SURFACE_NL[surface]
        return naam, klasse, wegtype
    if hw == "track":
        if tags.get("tracktype") in TRACKTYPE:
            naam, klasse = TRACKTYPE[tags["tracktype"]]
            return naam, klasse, wegtype
        return "onbekend", ONBEKEND, wegtype
    if hw in ("path", "bridleway") or (hw == "footway" and tags.get("footway") != "sidewalk"):
        return "onverhard (aangenomen)", ONVERHARD, wegtype
    return "verhard (aangenomen)", VERHARD, wegtype  # roads: asphalt in practice


def dist_m(a, b):
    la1, lo1 = map(math.radians, a)
    la2, lo2 = map(math.radians, b)
    h = math.sin((la2 - la1) / 2) ** 2 + math.cos(la1) * math.cos(la2) * math.sin((lo2 - lo1) / 2) ** 2
    return 2 * 6371000 * math.asin(math.sqrt(h))


def cell(la, lo):
    return round(la * 5500), round(lo * 3500)  # ~20 m


class Surfaces:
    """Per ~20 m cell the ways that pass, read once from the extract around a point."""

    def __init__(self, pbf, lat, lon, radius_km):
        import osmium  # pylint: disable=import-outside-toplevel
        dlat = radius_km / 110.57
        dlon = radius_km / (111.32 * math.cos(math.radians(lat)))
        box = (lat - dlat, lat + dlat, lon - dlon, lon + dlon)
        cells = self.cells = {}

        class H(osmium.SimpleHandler):
            def way(self, w):
                if "highway" not in w.tags:
                    return
                nodes = [(n.lat, n.lon) for n in w.nodes if n.location.valid()]
                if not nodes or not any(box[0] <= la <= box[1] and box[2] <= lo <= box[3] for la, lo in nodes):
                    return
                info = classify(dict(w.tags))
                for a, b in zip(nodes, nodes[1:]):
                    steps = max(1, int(dist_m(a, b) / 10))
                    for i in range(steps + 1):
                        key = cell(a[0] + (b[0] - a[0]) * i / steps, a[1] + (b[1] - a[1]) * i / steps)
                        cells.setdefault(key, Counter())[info] += 1

        H().apply_file(pbf, locations=True)

    def at(self, la, lo):
        """The way a walker is on here: the one passing this cell most (majority)."""
        c = self.cells.get(cell(la, lo))
        return c.most_common(1)[0][0] if c else None

    def paved_share(self, pts):
        hits = [self.at(la, lo) for la, lo in pts]
        hits = [h for h in hits if h]
        return sum(1 for h in hits if h[1] == VERHARD) / len(hits) if hits else 0.0
