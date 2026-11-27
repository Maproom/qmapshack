# QMS-1351 — one-way arrows on roads

Ticket: QMS-#1351, parent QMS-#1343 (`img-mkgmap-gap-plan.md`, "One-way arrows"). Branch `QMS-1351` from `dev` at
`77bc4e8f0`. Development ticket: no `changelog.txt` line; PR to `dev` only when complete.

## State (2026-11-27)

Investigated; no code changed. Result: BaseCamp draws no arrows from the direction bit; the arrows on OMTB are TYP
line bitmaps, which QMapShack already draws. Nothing to implement unless the user decides otherwise.

## QMapShack

- `CGarminPolygon.cpp:85` decodes RGN bit 0x40 into `CGarminPolygon::direction` (`CGarminPolygon.h:57`); nothing under
  `src/qmapshack/map/` reads it (grep `.direction`/`->direction`).

## mkgmap r4924 (`javap -c`)

- `.mp`: `DirIndicator=<n>`, n > 0 sets the line's direction (`PolishMapDataSource`); `MapBuilder$LineAddFilter`
  copies it to `Polyline.setDirection()`, written as bit 0x40 of the RGN type byte.
- OSM (`StyledConverter$WayTypeResult`): direction = the road's `oneway`; `mkgmap:has-direction=yes/no` overrides;
  a type listed in `--line-types-with-direction` gets it; a line that is no road gets none otherwise.
- Default style: `mkgmap:has-direction=true` on rivers, streams, rapids, waterfalls (`styles/default/lines:260`).
- Routing one-way is a separate flag in NET (`RoadDef.setOneway()`, `NET_FLAG_ONEWAY`).

## BaseCamp 4.7.5

Test map `oneway.mp` (scratchpad, not kept), lines west to east, routable, no TYP: 0x06 with direction bit and
one-way, direction only, one-way only, neither; 0x01 direction + one-way; 0x18 stream with and without direction.

- No arrows on any line at the 500 m, 200 m, 100 m scale bars, nor on "Dir Oneway" at 50 m (closest zoom).
- OMTB Luxembourg (own TYP `claslu.TYP`), Luxembourg city at 100 m and 70 m: arrow marks along many old-town streets.

## QMapShack on OMTB Luxembourg

The `.gmap` tiles 64230000/64230001 and `claslu.TYP` packed into one IMG (throwaway script; mkgmap packs only the
subfile it is given). Luxembourg old town at the 100 m and 50 m scale bars: the same arrow marks as BaseCamp along
the same streets. Packed again without `claslu.TYP`, same spot at the 100 m bar: no arrow marks. So they come from
the TYP.

Test run pitfalls: Qt's INI reader drops `@PointF(...)`, `posFocus` needs `@Variant(\x00\x00\x00\x1a` + two
big-endian doubles); `minScale`/`maxScale=-1` hides a map at every scale (`IDrawObject::isOutOfScale()`), leave
them out.
