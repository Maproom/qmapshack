# QMS-1352 — overlay maps by their draw priority

Ticket: QMS-#1352, parent QMS-#1343 (`img-mkgmap-gap-plan.md`, "TRE draw priority"). Branch `QMS-1352` from `dev` at
`77bc4e8f0`. Development ticket: no `changelog.txt` line; PR to `dev` only when complete.

## State (2026-11-27)

Closed (2026-11-27) without code: BaseCamp ignores the draw priority within a map product; the later tile is drawn on
top.

## mkgmap r4924 (`javap -c`, `TREHeader.writeFileHeader`)

- TRE 0x3F POI/display flags (bit 0x02 transparent, `--transparent`), 0x40-0x42 display priority (`put3u`),
  `--draw-priority`, default 25.
- Real maps (TRE byte dump): OMTB base tiles 25, OMTB 7357/7423 tiles 24 (flags 0x03); OpenFietsMap base 25, overlay
  tiles 20 (flags 0x09/0x0d).

## QMapShack

- `hdr_tre_t` skips 0x40 (`byte0x00000040_0x00000049`); `isTransparent` is read (`CGarminImg.cpp:308-309`), nothing
  in `CMapIMG.cpp` uses it.
- All tiles of one IMG go into one list per kind; polygons are drawn by type draw order across tiles
  (`CMapIMG.cpp:986-1002`), then lines, then points. Tile order and priority play no part. Not run on the test maps.
- Several IMG files: the map list order (user's choice) decides.

## BaseCamp 4.7.5 (one product = two tiles, base priority 25)

BaseCamp shows one map product at a time, so priority can only matter between tiles of one product.

| Variant | Tile 1 (lower number) | Tile 2 | Overlay priority | On top |
|---|---|---|---|---|
| prio 20/30 (transparent), opaque 20/30 | base lake 0x3c | overlay park 0x4e | 20, 30 | park (tile 2) |
| swap t20/t30 (transparent), s20/s30 | base park 0x4e | overlay lake 0x3c | 20, 30 | lake (tile 2) |
| first 20/30 | overlay lake 0x3c | base park 0x4e | 20, 30 | park (tile 2) |

In every variant the second tile covers the first; priority 20 vs 30 and the transparent bit change nothing.
Screenshots: scratchpad `prio/` (b30a, b20a, bo20, bo30, ss20, ss30, st20, st30, sf20, sf30), not kept.

Not checked: a Garmin device (several gmapsupp files or products at once, where priority may apply).
