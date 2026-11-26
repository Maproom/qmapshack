# QMS-1357 — IMG reader split out of CMapIMG, decoder test with mkgmap test maps

Ticket: QMS-#1357, parent QMS-#1343 (`img-mkgmap-gap-plan.md`). One PR, no change in behaviour.

## Facts (read in `map/CMapIMG.{h,cpp}`, `map/garmin/`, `units/IUnit.*`)

- Parsing and decoding (`readFile`, `readBasics`, `readSubfileBasics`, `processPrimaryMapData`, `loadSubDiv`) call no
  GUI code. They reach `IMap` only through `getShowPOIs/Polylines/Polygons()` in `loadSubDiv` and
  `map->needsRedraw()` in `loadVisibleData`.
- `CMapIMG()` reads `CMainWindow::self().getMapFont()` and shows a `QMessageBox` on a read error.
- `subfile.strtbl` is created with `this` as QObject parent; `CFileExt::cnt` is defined in `CMapIMG.cpp`.
- `IGarminStrTbl.cpp:145`, `CGarminPoint.cpp:110`, `CGarminPolygon.cpp:379` format elevations through
  `IUnit::self()`; `IUnit.cpp` includes `CMainWindow.h` and calls it (line 821).
- `CGarminTyp.cpp` shows `QMessageBox` through `CMainWindow::getBestWidgetForParent()` (lines 426, 775).
- `CMapItem.cpp:218` is the only place creating a `CMapIMG`.
- `doc/shots/fixtures/default/maps/Tannheimer_Tal.img` is a gmapsupp with TYP; `shots.py replay` renders it.
  Which shots show it: not checked.
- `test_SurfaceDurbuy` finds outside data through `QMS_SURFACE_*` environment variables and `QSKIP`s without them.

## Design

`map/garmin/CGarminImg.{h,cpp}`, a GUI-free reader. `CMapIMG` owns one and keeps everything drawn.

Moves to the reader:
- the packed header structs, `maplevel_t`, `subfile_part_t`, `subdiv_desc_t`, `subfile_desc_t`, `map_level_t`;
- `filename`, `mask*`, `mapdesc`, `subfiles`, `transparent`, `maparea`, `maplevels`, `copyrights`;
- `readFile`, `readBasics`, `readSubfileBasics`, `processPrimaryMapData`, `loadSubDiv`, `isCompletelyOutside`;
- the subfile/subdiv loop of `loadVisibleData` as `load(level, viewport, filter, abort, out)`: `filter` carries the
  three show flags and `fast`, `abort` is a callable for `needsRedraw()`, `out` the four vectors;
- `typData()`: the raw bytes of the first TYP subfile, for `setupTyp()`;
- `int CFileExt::cnt`.

Stays in `CMapIMG`: TYP properties and `setupTyp()`, languages, `fm`, `scale2bits`, `draw*`, labels, tooltips,
`getInfo*`, `findPolylineCloseBy`, the decoded vectors, the `QMessageBox` on a read error.

- Translations: the reader declares `Q_DECLARE_TR_FUNCTIONS(CMapIMG)`, so its moved messages keep their context
  and existing `.ts` entries.
- Errors: the reader throws its own `exce_t`; `CMapIMG()` catches it as today.
- `strtbl` ownership: the reader is a QObject and parents them, as `CMapIMG` does now.

## Test

`map/garmin/test/test_CGarminImg.cpp`, QTest, added under `BUILD_TESTING` like `gis/trk/surface/test`.
- Fixture folder from `QMS_IMG_FIXTURES`; `QSKIP` without it.
- Per fixture a hand-written Polish-format `.mp` in `map/garmin/test/mp/`; the test parses it and compares
  each point, line and polygon (type, label, coordinates within the level's resolution) with what the reader
  decodes at level 0.
- The three mkgmap test tiles (`63240001–3.img`) only have to load without error.
- First fixture: points, lines, polygons with labels, codepage 1252, one level. Bug fixtures come with their
  tickets.

`map/garmin/test/make_fixtures.py` builds every `.mp` with a given `mkgmap.jar` into a given folder. Paths via
`pathlib`, files written with `newline="\n"`. mkgmap release pinned in the script: r4924.

## Open

- **IUnit in the test:** linking `IUnit.cpp` pulls in `CMainWindow`. Either a test-only `IUnit` subclass with stubs of
  the out-of-line members, or the decoders keep the raw elevation and formatting moves to display (a behaviour
  change, so not in this PR). Decide when the first link fails.
- mkgmap may simplify or merge lines even at one level: unverified; the comparison may need to match by label.
- Before/after check of the move: `shots.py replay` must report no changed image. Not run yet.

## Steps

1. Reader class, `CMapIMG` uses it; clang-format, syntax check; user builds; `shots.py replay`.
2. Fixture script and first `.mp`; build fixtures with r4924.
3. Test target and test; user builds and runs ctest.
4. `changelog.txt` line, PR with documentation check (none expected: no visible change).
