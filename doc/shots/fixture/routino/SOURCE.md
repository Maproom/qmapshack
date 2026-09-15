# Fixture Routino database

## tannheimer_tal-{nodes,relations,segments,ways}.mem

Map data © [OpenStreetMap contributors](https://www.openstreetmap.org/copyright),
[ODbL](https://opendatacommons.org/licenses/odbl/). Retrieved through the Overpass API, data as of
2026-09-15 07:09 UTC.

Extent: the DEM's, lat 47.472–47.565, lon 10.459–10.636. Taken are the ways with a `highway` tag and
their nodes, and the route and turn restriction relations. The Overpass reply carries `<note>` and
`<meta>` elements, which Routino's parser rejects. Built with Routino 3.4.1 and the arguments of
QMapShack's own database builder.

```bash
curl -A 'QMapShack documentation fixture' https://overpass-api.de/api/interpreter --data-urlencode \
  'data=[out:xml][timeout:180][bbox:47.472,10.459,47.565,10.636];(way["highway"];node(w);relation["type"~"^(route|restriction)$"];);out body;' \
  -o raw.osm
grep -v -E '^\s*<(note|meta)[ >]' raw.osm > area.osm
planetsplitter --dir=. --prefix=tannheimer_tal --tagging=/usr/share/routino/tagging.xml area.osm
```
