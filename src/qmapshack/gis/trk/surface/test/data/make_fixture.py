#!/usr/bin/env python3
"""Write fixture.osm.pbf, the small OSM extract the surface unit tests use (needs pyosmium).

Usage: make_fixture.py
Also writes fixture_cells.tsv, the cell lookups of the reference implementation (osmsurface.py).

Around 50.0 N / 5.0 E:
  way 10  highway=track tracktype=grade2   along lat 50.0, lon 5.000 .. 5.010
  way 11  highway=path                     along lat 50.002, lon 5.000 .. 5.010
  way 12  highway=residential surface=asphalt  along lon 5.005, lat 49.999 .. 50.003
  way 13  building=yes                     ignored, no highway
  way 14  highway=primary                  refers to node 999, which is missing
  way 15  highway=cycleway                 along lat 50.01, lon 5.000 .. 5.002 (one sample cell shared
  way 16  highway=steps                    with way 16, which is shorter: majority test)
"""
import osmium
from osmium.osm.mutable import Node, Way

out = "fixture.osm.pbf"
w = osmium.SimpleWriter(out, overwrite=True)
nodes = {}


def node(nid, lat, lon):
    nodes[nid] = (lat, lon)


for i in range(11):  # way 10 and 11, every 0.001 deg (~72 m)
    node(100 + i, 50.0, 5.0 + i * 0.001)
    node(200 + i, 50.002, 5.0 + i * 0.001)
for i in range(5):
    node(300 + i, 49.999 + i * 0.001, 5.005)
node(400, 50.005, 5.0)
node(401, 50.005, 5.0001)
node(402, 50.0051, 5.0001)
node(500, 50.007, 5.0)
node(501, 50.007, 5.001)
node(600, 50.01, 5.0)
node(601, 50.01, 5.002)
node(700, 50.01, 5.0002)
node(701, 50.01, 5.0004)

for nid in sorted(nodes):
    lat, lon = nodes[nid]
    w.add_node(Node(id=nid, location=(lon, lat), tags={}))

w.add_way(Way(id=10, nodes=[100 + i for i in range(11)], tags={"highway": "track", "tracktype": "grade2"}))
w.add_way(Way(id=11, nodes=[200 + i for i in range(11)], tags={"highway": "path"}))
w.add_way(Way(id=12, nodes=[300 + i for i in range(5)],
              tags={"highway": "residential", "surface": "asphalt", "name": "Rue de Test"}))
w.add_way(Way(id=13, nodes=[400, 401, 402, 400], tags={"building": "yes"}))
w.add_way(Way(id=14, nodes=[500, 999, 501], tags={"highway": "primary"}))
w.add_way(Way(id=15, nodes=[600, 601], tags={"highway": "cycleway"}))
w.add_way(Way(id=16, nodes=[700, 701], tags={"highway": "steps"}))
w.close()

# Expected cell lookups from the reference implementation, on a grid over the fixture:
# lat, lon, label (Dutch), class, way type (Dutch); cells no way passes are left out.
import os  # noqa: E402
import sys  # noqa: E402

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from osmsurface import Surfaces  # noqa: E402

s = Surfaces(out, 50.005, 5.005, 5)
with open("fixture_cells.tsv", "w") as f:
    for i in range(-10, 141):
        lat = 49.9985 + i * 0.0001
        for j in range(-5, 116):
            lon = 4.9995 + j * 0.0001
            info = s.at(lat, lon)
            if info is None:
                continue
            f.write(f"{lat:.7f}\t{lon:.7f}\t{info[0]}\t{info[1]}\t{info[2]}\n")
print(os.path.getsize("fixture_cells.tsv"))
