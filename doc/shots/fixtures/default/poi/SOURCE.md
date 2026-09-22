# Fixture POI

## tannheimer_tal.poi

Map data © [OpenStreetMap contributors](https://www.openstreetmap.org/copyright),
[ODbL](https://opendatacommons.org/licenses/odbl/). Extracted from the mapsforge POI file
<https://download.mapsforge.org/pois/europe/austria.poi>, written 2026-03-14 04:21 UTC.

Changes: cut to lat 47.472–47.565, lon 10.460–10.636, which covers both example projects, the map
and the DEM; converted from POI version 3 to version 2, the only one QMapShack reads. Version 2 is
mapsforge's schema at commit 2ce3237cdc: `poi_index` is an R-tree with `minLat`, `maxLat`, `minLon`,
`maxLon` where version 3 has `lat` and `lon`. Categories, POI data and the metadata `date` are
copied unchanged.

```bash
curl -O https://download.mapsforge.org/pois/europe/austria.poi
python3 poi_v3_to_v2.py austria.poi tannheimer_tal.poi
```

`poi_v3_to_v2.py`:

```python
import sqlite3, sys
src, dst = sys.argv[1], sys.argv[2]
MIN_LAT, MAX_LAT, MIN_LON, MAX_LON = 47.472, 47.565, 10.460, 10.636

s = sqlite3.connect(f'file:{src}?mode=ro', uri=True)
meta = dict(s.execute('select name, value from metadata'))
assert meta['version'] == '3', meta['version']

d = sqlite3.connect(dst)
# mapsforge DbConstants at 2ce3237cdc, the version 2 schema
d.executescript('''
CREATE TABLE poi_categories (id INTEGER, name TEXT, parent INTEGER, PRIMARY KEY (id));
CREATE TABLE poi_data (id INTEGER, data TEXT, PRIMARY KEY (id));
CREATE TABLE poi_category_map (id INTEGER, category INTEGER, PRIMARY KEY (id, category));
CREATE VIRTUAL TABLE poi_index USING rtree(id, minLat, maxLat, minLon, maxLon);
CREATE TABLE metadata (name TEXT, value TEXT);
''')
d.executemany('insert into poi_categories values (?,?,?)', s.execute('select id, name, parent from poi_categories'))
rows = s.execute('select id, lat, lon from poi_index where lat between ? and ? and lon between ? and ? order by id',
                 (MIN_LAT, MAX_LAT, MIN_LON, MAX_LON)).fetchall()
ids = [r[0] for r in rows]
d.executemany('insert into poi_index values (?,?,?,?,?)', [(i, la, la, lo, lo) for i, la, lo in rows])
data = {i: v for i, v in s.execute(f'select id, data from poi_data where id in ({",".join("?"*len(ids))})', ids)}
d.executemany('insert into poi_data values (?,?)', [(i, data[i]) for i in ids])
cmap = s.execute(f'select id, category from poi_category_map where id in ({",".join("?"*len(ids))}) order by id, category', ids).fetchall()
d.executemany('insert into poi_category_map values (?,?)', cmap)
d.executemany('insert into metadata values (?,?)', [
    ('bounds', f'{MIN_LAT},{MIN_LON},{MAX_LAT},{MAX_LON}'),
    ('comment', meta['comment']),
    ('date', meta['date']),
    ('language', meta['language']),
    ('version', '2'),
    ('ways', meta['ways']),
    ('writer', meta['writer']),
])
d.commit()
d.execute('VACUUM')
d.close()
print(f'{len(ids)} POIs, {len(data)} data rows, {len(cmap)} category rows')
```
