# QMS-1347 — Garmin label decoding errors

Ticket: QMS-#1347, parent QMS-#1343 (`img-mkgmap-gap-plan.md`, rows L1/L2/L5–L8). Branch `QMS-1347` from `dev`
at `8cf370f03`. Development ticket: no `changelog.txt` line; PR to `dev` only when complete.

## State (2026-11-27)

Done: L1, L2, L5, L6, L7, L8 (2026-11-27). Verified:
- `test_CGarminImg`: 37/37 pass; `/usr/bin/ctest` 5/5 (`/usr/local/bin/ctest` is a broken pip shim). Each new decode
  case failed before its fix: labels6 crashed in `processLabel` (null `IUnit`), cp874-949 not decoded, labelsutf8 long
  label cut, routable "Main Street" at NET1 offset 0 not decoded, cp932 0x1c read as U+007F (Shift_JIS).
- GUI (Xephyr, own HOME, `-c` with `[Canvas] mapPath`, view moved via `posFocus` in radians): 8-bit and 6-bit roads
  show as BaseCamp zoomed in; peaks "Gipfel, 376 m"/"376 m", height points and contours "376 m", "Contour 1234",
  "Route 66".
- Not unit tested: `getLabelText()` formatting (the test has no `IUnit`); covered by the GUI run only.
- L2 changed only the polyline loop: mkgmap sets the NET bit only for a line with a `RoadDef` (`Polyline.class`).
- L6: `windows-874/932/936/949` (only lower case found on first lookup); `processLabel` decodes between separators.

- Review (code-review, 2026-11-27): fixed `///` doc style, stale `CUnitStub.cpp` comment, height point `getName()`,
  unlabelled road back in `routable.mp`; POI heights limited by type (above); `calcOffset()` returns 0xFFFFFFFF when
  fewer than 4 bytes were read (pre-existing over-read on a damaged file; no test, the over-read is silent).

- Documentation: all 113 English shots render identically before and after (`shots.py replay`, pixel compare).

Fixtures: `make_fixtures.py <mkgmap r4924 jar> <dir>`. Done.

Decided (2026-11-27), option 2: the decoded label keeps the separators at their position; only the display text
resolves them. QMS-#1350 needs the positions to shorten road names.

### What the separators do (BaseCamp 4.7.5, 2026-11-27)

mkgmap r4924 `PrefixSuffixFilter` (`javap -c`, constant pool): a prefix ending in a blank gets 0x1e in place of the
blank, any other prefix 0x1b; a suffix starting with a blank gets 0x1f in place of it, any other 0x1c.

BaseCamp, 8-bit routable test map (roads 0x04, `EndLevel=2`, levels 24/22/20/18), scale bar 500 m vs 300 m:

| Label | 500 m | 300 m and closer |
|---|---|---|
| `Rue de la~[0x1e]Concorde` | Concorde | Rue de la Concorde |
| `Haupt~[0x1c]strasse` | Haupt | Hauptstrasse |
| `Main~[0x1f]Street` | Main | Main Street |
| `West~[0x1e]Lincoln~[0x1f]Road` | Lincoln | West Lincoln Road |
| `Carrer d'~[0x1b]Europa~[0x1c]kalea` | Europa | Carrer d'Europakalea |
| `Rue d'~[0x1b]Alsace` | Alsace | not seen |
| `Alpha~[0x1d]Beta` | Alpha | Alpha |

- Zoomed in: 0x1b/0x1c show as nothing, 0x1e/0x1f as a blank. Zoomed out: only the part between prefix and suffix
  marker (QMS-1350). Whether the switch follows the scale or the map level is not known.
- 0x1d: the text after it is never shown on the map. Its purpose (search, alternate name) is not known.
- Not done: 6-bit map in BaseCamp, BaseCamp's search on prefix vs core word. Rebuild: one `.mp` with these labels,
  `mkgmap --gmapi --index --route --code-page=1252 --lower-case --family-id=<id>`, gmap into the prefix's `Maps/`.
- BaseCamp under Wine opened two tabs in the host's browser during the run (unverified cause: `winebrowser`).

### 0x1f as height (BaseCamp 4.7.5, 2026-11-27, metric units)

| Object | Label | Map | Tooltip |
|---|---|---|---|
| Peak 0x6616 | `Gipfel~[0x1f]1234` | Gipfel, 376 m | Gipfel |
| Peak 0x6616 | `~[0x1f]1234` | , 376 m | Landmerkmal (type name) |
| Peak 0x6616 | `Spitze 1234` | Spitze 1234 | Spitze 1234 |
| Height point 0x6300 | `1234` | 376 | 376 m |
| Height point 0x6300 | `~[0x1f]1234` | nothing | nothing |
| Contour 0x21 | `1234` | 376 | 376 m Höhe |
| Contour 0x20 / 0x22 | `~[0x1f]1234` / `Contour~[0x1f]1234` | nothing | raw text, 0x1f as a box |
| Road 0x06 | `Route~[0x1f]66` | Route 66 | |

- Decided (2026-11-27): QMS deviates from BaseCamp where BaseCamp drops or mangles a height: peak `~[0x1f]1234` →
  "376 m" (BaseCamp ", 376 m"), height point `~[0x1f]1234` → "376 m" (BaseCamp nothing), contour `~[0x1f]1234` →
  "376 m", `Contour~[0x1f]1234` → "Contour 1234" (BaseCamp no label).
- On a point, 0x1f separates the name from a height in feet. On a line it is the suffix separator, never a height.
- A contour's height is its plain number, in feet.
- Other point types, label `<Name>~[0x1f]1234` (BaseCamp 4.7.5, 2026-11-27): 0x6401 bridge, 0x6508 waterfall,
  0x6603 basin, 0x6616 summit show "<Name>, 376 m"; 0x4400 fuel and 0x0b00 city show only "<Name>", the part after
  0x1f hidden. 0x2a00 restaurant, 0x2f08 bus stop, 0x2f0b parking are not drawn at 300/150/70 m; tooltip "<Name>"
  only (a summit's tooltip is also the name only, so it tells nothing).
- Decided (2026-11-27): a height after 0x1f shows only on 0x6200/0x6300 and 0x6400-0x66FF ("<Name>, 376 m"); any
  other point shows the name only (`CGarminPoint::getLabelText()`). 0x64-0x66 extended from one checked type each.
  GUI run: Fall/Becken/Gipfel/Bruecke "…, 376 m"; Tanke, Stadt, Pizza, Bus, Parken name only.

QMS today, by the code (not run on this map): splits at 0x1b-0x1f (6-bit: 0x1d-0x1f) and joins with " ", so
0x1e/0x1f display right, 0x1b/0x1c get a blank (8-bit) or garbage (6-bit), "Alpha Beta" shows both parts.

## Findings (step 1)

| | Test | Today |
|---|---|---|
| L1 | `labels6.mp`: `RUE D'~[0x1b]ALSACE`, `MAIN ~[0x1c]STREET` | "RUE D'+ALSACE", "MAIN ,STREET" |
| L2 | QMS-1345: gdb `p.lbl_info = 0`, `lbl_in_NET = true`; `CGarminImg.cpp:888/890` need `lbl_info != 0` | first road in NET1 unlabelled |
| L5 | `labels8.mp`, `labelsutf8.mp`, same labels | "Rue d' Alsace", "Main  Street" |
| L6 | `cp874/932/936/949.mp` | Latin-1 garbage (`IGarminStrTbl.cpp:30-44` knows 1250-1258, 950, 850, 65001) |
| L7 | 170-char label (mkgmap max, `LBLFile.java:64`) | 6/8-bit: 170 chars; UTF-8 with "é": 103 chars (200-byte cap, `CGarminStrTbl{6,8,Utf8}.cpp`) |
| L8 | code read; `QString("376 m").toFloat()` = 0 | not reproduced with a map |

- L1 cause: mkgmap writes 0x1b/0x1c as `0x1B` + `0x2B`/`0x2C` (`Format6Encoder.java:81-83`); QMS reads
  `str6tbl3[c1]` (`CGarminStrTbl6.cpp:152`, 27 entries, `:81-82`).
- L5: `CGarminStrTbl8.cpp:52-61` (and UTF-8) split at 0x1b-0x1f into separate labels; callers join with " ".
- L8 path: label after 0x1f → `processLabel` makes "<value> <unit>" (`IGarminStrTbl.cpp:140-147`), then
  `getLabelText` for contours 0x20-0x25 / height points 0x6200/0x6300 calls `toFloat()` on it
  (`CGarminPolygon.cpp:397-399`, `CGarminPoint.cpp:118-122`). Trigger: `Label=~[0x1f]1234` on such a type.
- mkgmap `.mp` reader: `~[0x1b]` becomes the character (`PolishMapDataSource.java:665-697`); `CodePage=65001`
  fails (`cp65001`), so `labelsutf8.mp` is Latin-1 without CodePage, built with `--code-page=65001`.

## Step 2 (planned)

- L1: 6-bit escape 0x1B + 0x2B/0x2C → 0x1b/0x1c; any other index past `str6tbl3` skipped.
- L5: decoders keep 0x1b/0x1c/0x1e/0x1f in the label instead of splitting. One display helper: 0x1b/0x1c → nothing,
  0x1e/0x1f → blank, text from 0x1d on dropped. Callers that build display text from `labels` use it
  (`CGarminPoint/CGarminPolygon::getLabelText()`, `CMapIMG.cpp` 1329, 1367, 1547, 1688; tests 144, 149, 349).
- L5/L8: the decoder keeps 0x1f raw; it does not know the object kind, and 0x1f means a height only on points (see
  below). The feet conversion moves from `processLabel` to the point's display text, done once (L8).
  Display text for a point: "<name>, <height>" as BaseCamp; a 0x6300 height point shows only a plain number.
- `labels8.mp:24` `Main ~[0x1c]Street` has a blank mkgmap would not write (" Street" → 0x1f); fixtures get both kinds.
- L2: accept NET label offset 0 (`CGarminImg.cpp:888/890` and the polygon loops); drop the unlabelled first road
  and its comment from `routable.mp`.
- L6: codecs for 874/932/936/949 (check the names Qt6 Core5Compat knows).
- L7: read labels up to the end of the label, not a fixed 200 bytes.
- L8: format the height once; needs a fake `IUnit` in the test (`CUnitStub.cpp:21` sets none; the Tannheimer probe
  crashed on a null `IUnit` at `IGarminStrTbl.cpp:145`).
- Test: `readMp` must turn `~[0x1b]`/`~[0x1c]` into the expected display text; each fix with a test that fails
  without it. Then all tests, ctest, quickstart shots, GUI (Xephyr, own HOME), independent review, commit, PR.
