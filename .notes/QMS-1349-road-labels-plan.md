# QMS-1349 — all labels of a road, highway shields

Ticket: QMS-#1349, parent QMS-#1343 (`img-mkgmap-gap-plan.md`, rows L3/L4). Branch `QMS-1349` from `dev` at
`859b6849c`. Development ticket: no `changelog.txt` line; PR to `dev` only when complete.

## State (2026-11-27)

Done (2026-11-27). Decided: the map draws label 1, as a shield when it has a code
(as BaseCamp); the hover info lists all labels, joined by ", " (more than BaseCamp's tooltip).

- `IGarminStrTbl::netLabels()`: up to 4 NET1 label offsets; `get()` decodes each. `calcOffset()` lost its NET branch.
- Shield codes stay in the label: 8-bit/UTF-8 no longer skip bytes < 0x07; 6-bit 0x2A–0x2F → 0x01–0x06
  (`Format6Encoder`: code + 41); `processLabel()` passes them by the codec; `displayText()` drops a leading one;
  `shieldCode()` returns it.
- `CGarminPolygon::getLabelText()` is label 1 (map); `getInfoText()` all labels (hover, `CMapIMG::getInfoPolylines`).
- `CMapIMG::drawShield()`: upright shield on the middle of the longest straight stretch, overlap check as names.
  Like all road labels only below `kStreetNameThreshold`; BaseCamp shows shields at 500 m already.

Verified: `test_CGarminImg` 48/48, `ctest` 5/5; without the decoder changes labels6, labels8 and routable fail
("?A7", the 3 labels). GUI: six shields, "First Label", hover "A7, Hbox Road" and "First Label, Second Label,
Third Label". Clang-Tidy/Clazy clean but `#pragma pack` from `CGarminImg.h`. 113 shots identical to `dev`.

Review (code-review, 2026-11-27), all fixed: `drawShield()` resets dash, caps and join between `save()`/`restore()`
(the polylines leave their pen; not run with a dashed TYP line); `displayText()` drops 0x01-0x06 anywhere (test row
"code inside"); an empty NET label keeps its place, trailing empties dropped (no test: mkgmap writes no empty label);
interstate top rounded (GUI checked); header comment. After: 49/49, ctest 5/5, Clang-Tidy/Clazy clean but
`#pragma pack`, 113 shots identical.

## mkgmap r4924 (`javap -c`)

- NET1 record: up to 4 labels (`RoadDef.addLabel`, loop to 4), each 3 bytes (`writeLabels`, `put3u`), the last with
  bit 0x800000. `.mp` `Label`, `Label2`, `Label3` become labels 1–3 (NET1 dump of a test road).
- Default style (`examples/styles/default/lines:291-293`): motorway/trunk `name '${ref|highway-symbol:hbox}'` plus
  `addlabel '${name}'`; primary `box`, secondary/tertiary `oval`.
- `HighwaySymbolFilter`: code byte before the ref: 0x01 interstate, 0x02 shield, 0x03 round, 0x04 hbox, 0x05 box,
  0x06 oval.

## BaseCamp 4.7.5 (test map, one road per shield type with ref and `Label2` name, one road with 3 labels)

- Map: label 1 only. With a shield code it is drawn as a symbol: interstate blue shield with red top, shield white US
  route shield, round white ellipse, hbox blue box with red bars, box white box with black bars, oval white rounded
  box. Names (label 2) not drawn at 500 m or 200 m.
- Tooltip: the first label without a shield code ("Hbox Road" on the A7 road; "First Label" on the 3-label road).
  Labels 2 and 3 of the 3-label road appear nowhere.

## QMapShack today

- `calcOffset()` reads one 4-byte value at the NET1 record (`IGarminStrTbl.cpp:129`) → label 1 only.
- `CGarminStrTbl8.cpp:51`, `CGarminStrTblUtf8.cpp:50` drop bytes < 0x07 → shield code lost, ref as plain text.
- Map: "A7" as text on the road; hover info "A7".
