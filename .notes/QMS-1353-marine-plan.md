# QMS-1353 — marine extra data

Ticket: QMS-#1353, parent QMS-#1343 (`img-mkgmap-gap-plan.md`, "Extra-byte content"). Branch `QMS-1353` from `dev` at
`77bc4e8f0`. Development ticket: no `changelog.txt` line; PR to `dev` only when complete.

Done: PR #1369 merged as `aa5cdd434` (2026-11-27), ticket closed. Follow-up: QMS-#1370 point labels beside the symbol.

## State (2026-11-27)

Step 1a done, not committed (2026-11-27): `CGarminMarine` decodes the extra data; `CGarminPoint`/`CGarminPolygon` keep
the raw bytes (`extra`); tests `marineBytes` (bytes per the Java source) and `marine` (fixture `mp/marine.mp` against
its keys). `test_CGarminImg` 80/80, ctest 5/5, qmapshack builds. `make_fixtures.py` now runs mkgmap in an English
locale: in German, `Depth=5.5` became 55 m.
Step 1b done, not committed (2026-11-27): hover info per marine point (light in chart notation, sectors, heights,
racon, leading angle, note and designators from LBL, buoy colour, depth/height with one decimal, position, facilities),
depth/height of lines and areas, obstruction label "Name - 3" as BaseCamp. `test_CGarminImg` 89/89; GUI check on
`marine.img` (Xephyr) shows every point's text.
Step 2a done, not committed (2026-11-27): marine lines without a TYP style drawn from their extra data
(`CMapIMG::drawMarineLines()`): BaseCamp's colour table, dashes, ticks; defaults by type without colour (0x103xx blue,
0x10105-0x10107 black, 0x105xx pink, else dark grey). GUI check on `styles.mp`: colours (sampled pixels) and patterns
as BaseCamp. Next, step 2b: light rings with sectors.
- Blend2D ignores a stroke dash array (test program in scratchpad `bltest/`: a 3/3 dash strokes solid), so the marine
  dashes are cut by hand (`dashedPath()`). Not in this ticket: `applyPen()`'s Qt dash styles, used by the built-in
  defaults without a TYP (`CMapIMG.cpp` 0x14, 0x1C-0x1E, 0x26 `Qt::DotLine`), are drawn solid for the same reason
  (not looked at on a map). TYP dashes are bitmaps (`img2line`) and unaffected.
Step 2b done, not committed (2026-11-27): `CMapIMG::drawLightSectors()` as decided under "Lights on the map": arcs at
0.2 nm, 4 px, OpenSeaMap's colours, dashed grey limit lines, bearings turned by 180°, below 10 m/px. GUI check on
`rings.mp`: sectors 0-90 south-west, 90-180 north-west, three-colour light as expected; all-round lights no ring.
Step 3 draft 2 coded, not committed (2026-11-27): `map/garmin/CMarineSymbol` composes the symbols from INT 1 parts
(see "Marine symbols (step 3)"); `CMapIMG::drawPois()` draws them for marine points the TYP does not style, the label
beside the symbol. Contact sheet against the IHO images: scratchpad `symbols/contact.html` (renders from the scratch
tool `symtool/`, which compiles `CMarineSymbol.cpp`). GUI check on a map with every type (`symmap/symbols.mp`).
Safe water and special confirmed (Trinity House, `.../buoy-rental/safe-water-marks-buoys`: "Single red sphere",
"Red and white vertical stripes", "Spherical; pillar or spar with spherical topmark"; `.../special-marks-buoys`:
"Single yellow “X” shape", "Yellow", shape "Optional, but not conflicting with lateral marks"). Size kept (user, 2026-11-27: fine on normal and HiDPI screens;
`QT_SCALE_FACTOR=2` run sharp, `CMapIMG.cpp:932` scales the context).
Before the PR (2026-11-27): Clang-Tidy/Clazy clean but `#pragma pack` and an older `strict-iterators` at
`CGarminImg.cpp:701`; two float loop counters fixed. Code review: 5 findings fixed (city points 0x02xx taken as buoys,
non-marine extended lines drawn, empty info text, viewer names in comments, clutter box); test `marineTypes()`;
`test_CGarminImg` 90/90. Translations: the 35 new strings in `qmapshack_de/es/it.ts`, inserted as lupdate writes them
(only the three contexts, the rest untouched), all `type="unfinished"` until reviewed (German by the user).
Documentation: only `quickstart-vector-map` uses a Garmin map; compared against a replay of `dev` on this machine.
- Decided (2026-11-27): point labels beside the symbol for all points (as BaseCamp) is its own ticket, not QMS-1353.
  Today `drawPoints()`/`drawPois()` centre icon and label on the point (`CMapIMG.cpp` 1475/1481, 1595/1601); only the
  marine branch places the label beside the symbol. The new ticket needs the documentation images taken again.

Decided (2026-11-27): full marine look built in (marine maps carry no TYP for marine devices,
OSM wiki "OpenSeaMap and Garmin nautical chart plotter"). Steps on this branch, PR at the end:
1. Decode all extra data, with tests; values into labels and hover info.
2. Lines in their style and colour; light rings with sectors.
3. Marine symbol set: a draft of 3-4 for review first, then the rest.

## mkgmap r4924 (`javap -c`)

- `.mp` keys (`PolishMapDataSource.makeExtTypeAttributes`): `Depth`/`DepthUnit`, `Height`/`HeightUnit`,
  `HeightAboveFoundation`, `HeightAboveDatum` (unit `f` = feet), `Color` (number), `Style`, `Light`, `LightType`,
  `Period`, `Position`, `Racon`, `Note`, `LeadingAngle`, `InternationalDesignator`, `LocalDesignator`, `FacilityPoint`.
- OSM: marine style (`styles/marine/points`) maps `seamark:*` to `mkgmap:xt-*`.
- `ExtTypeAttributes.encodeExtraBytes()` branches on `type & 0xFF00`: points 0x100 (lights, incl. sectors and
  character), 0x200 (buoys/beacons), 0x300/0x400, 0x500; lines 0x300; polygons 0x400/0x500/0x600. Not decoded further.
- `colour()`: 0 none, 1 red, 2 green, 3 yellow, 4 white, 5 black, 6 black-yellow, 7 white-red, 8 black-red,
  9 white-green, 10 red-yellow, 11 red-green, 12 orange, 13 black-yellow-black, 14 yellow-black, 15 yellow-black-yellow,
  16 red-white, 17 green-red-green, 18 red-green-red, 19 black-red-black, 20 yellow-red-yellow, 21 green-red,
  22 black-white, 23 white-orange, 24 orange-white, 25 green-white.

## Extra-byte encoding (mkgmap r4924 `ExtTypeAttributes.java`, `svn export -r 4924`)

`t` = type & 0xFFFF, `r` = t & 0xFF00. Distances: 0x10 metric (else feet), 0x08 tenths. LBL offsets are 3 bytes LE.

- Point, `r` 0x0100 light: `E0|flags0` (1 racon, 2 note, 4 local desig, 8 int desig), `(nob<<1)|1`,
  `0x80|lightType` (0x40 = period bit 8), `flags1` (1 periods record follows, 2 leading angle, 8 several lights),
  `lightsDef` (base 0x22; height above foundation 0x40 one byte / 0x80 two, 0x20 cleared for feet; above datum 0x04 one
  byte / 0x08 two, 0x02 cleared for feet), [haf], [had], period (tenths of s, low 8 bits), [note], [local desig],
  [int desig], [leading angle, 2 bytes, tenths of degree]; lights: several = per light 2 bytes
  `colour<<12 | angle*10`, 0x8000 on the last, plus 1 range byte if colour != 0; one = `colour<<5 | range&0x1f`;
  then `0x80+n` and n light phases then the eclipse phases (each split in steps of 0x3f), or a Morse letter, or 0x01.
- Point, `r` 0x0200 buoy/beacon: `E0|flags0`, `(nob<<1)|1`, `(lightColour<<6)|colour`, `flags1` (1 = light colour
  bit 2, 2 periods record follows), [note], [local desig], [int desig], `0x80 if period | lightType`,
  [period total, then `0x81`/`0x82` + phases, or 0x01], or Morse letter, or 0x01.
- Point, `r` 0x0300 depth/height, 0x0400 obstruction: `0x80` (value <= 255, 2 bytes) or `0xA0` (3 bytes)
  `| 0x10 | 0x08 | position` (0x0400 only, bits 0-2: 0 unknown, 2 doubtful, 3 existence doubtful, 4 approximate,
  5 reported), value LE.
- Point, `r` 0x0500 label: 1 byte, `style & 0xF` (colour).
- Point, `t` 0x0903 facility: 3 bytes, `0xA0|f&0x1f`, `f>>5`, `(f>>13)&7 | (f>>12)&0x18`.
- Line or polygon `r` 0x0300, line `t` 0x0105/0x0106/0x0107: depth/height as points 0x0300.
- Line `r` 0x0400-0x0600: `style & 0xFF00 == 0`: 1 byte colour `style & 0xF`; else 2 bytes `0x80|colour`,
  `((style>>9)&0x18) | ((style>>8)&3)`.
- Colours (buoys, labels, lines): `colour()` list above. Light colours: 0 unlit, 1 red, 2 green, 3 white, 4 blue,
  5 yellow, 6 violet, 7 amber. Light types: 0 none, 1 fixed, 2 isophase, 3 flashing, 4 group flashing, 5 composite
  group flashing, 6 occulting, 7 group occulting, 8 composite group occulting, 9 long flashing, 10 group long
  flashing, 11 Morse, 12 quick, 13 group quick, 14 group quick and long flashing, 15 interrupted quick, 16 very quick,
  17 group very quick, 18 group very quick and long flashing, 19 interrupted very quick, 20 ultra quick,
  21 interrupted ultra quick, 22 fixed and occulting, 23 fixed and group occulting, 24 fixed and isophase, 25 fixed
  and flashing, 26 fixed and group flashing, 27 fixed and long flashing, 28 alternating, 29 alternating occulting,
  30 alternating flashing, 31 alternating group flashing.
- `.mp` keys (`PolishMapDataSource.makeExtTypeAttributes`): `Depth`/`Height`/`HeightAboveFoundation`/
  `HeightAboveDatum` with `…Unit=f` for feet; `Color` and `Style` become `style = (Style<<8)|Color`;
  `FoundationColor` is a buoy's colour; `Light`, `LightType`, `Period`, `Position`, `Note`, `LocalDesignator`,
  `InternationalDesignator`, `FacilityPoint`, `Racon`, `LeadingAngle`. `Light` uses `(colour,range,angle),...` or
  `colour,range,angle/...`.
- Ambiguous: a light phase of exactly 6.3 s is written as one 0x3f byte, which also starts a longer phase.

## Test map `marine.mp` (scratchpad, not kept)

Points: light 0x10100 red (Period 5, HeightAboveDatum 12), light 0x10100 green, buoy 0x10206 Color 1, 0x10300 Depth 12,
0x10301 Depth 5.5, 0x10400 Depth 3, 0x10500. Lines: 0x10301 Depth 10, 0x10400 Style 1 Color 1, 0x10500. Polygons
0x10400 Depth 7, 0x10500 Depth 20, 0x10600 Color 1. No TYP.

## BaseCamp 4.7.5, 500 m scale bar

- Lights: light symbol with a ring in the light's colour (red, green).
- Buoy 0x10206 Color 1: red buoy symbol.
- 0x10400 Depth 3: dotted circle, label "Obstruction - 3".
- 0x10300/0x10301 soundings: small dot / nothing, label only, no depth shown at that scale.
- Lines: 0x10301 blue, 0x10400 Style 1 dashed blue, 0x10500 pink.
- Polygons: no visible fill; "Area 600" tooltip.

## BaseCamp 4.7.5 with the fixture `mp/marine.mp` (English-locale build), 300 m scale bar

Map: lights as rings in their colours, sector lights as coloured arcs; buoy red with green light; soundings label only
(no value); "Obstruction - 3"; "Label Yellow" in a coloured font; facility with a symbol; lines in their styles
(0x10600 Style 0x13 red ticked, 0x10500 Style 1 green dashed). Tooltips: the label only, except "Obstruction - 3.0 m".
Details (double click), German UI:

| Object | Details |
|---|---|
| Light Single 0x10100 | Leuchtfeuerart: Unbekannt; Wiederkehr: 4.0 Sek.; Farbe: Rot; Bereich: 10 nm |
| Light Sectors 0x10101 | Leuchtfeuerart: Blitzfeuer; Wiederkehr: 5.0 Sek.; Lichtsektor: Rot, 10nm, 0.0° - 120.0° (one line per sector, the last to 0.0°); Höhe über Kartenbezugssystem: 9.1 m; Höhe über Fundament: 12.0 m; Radarantwortbake: Ja |
| Light Morse 0x10102 | Leuchtfeuerart: Morsecode "85" (the byte, 'U'); Farbe: Weiss; Bereich: 6 nm |
| Light Note 0x10103 | Leuchtfeuerart: Unbekannt; Farbe: Grün; Bereich: 3 nm; Leitsektor: 123.4 °; Hinweis: Harbour entry |
| Buoy Red 0x10206 | Farbe des Fundaments: Rot; Leuchtfeuerfarbe: Grün; Wiederkehr: 3.0 Sek. |
| Sounding Metre / Tenth 0x10301 | Tiefe: 12.0 m / Tiefe: 5.5 m |
| Sounding Feet 0x10302, 40 ft | Höhe: 12.2 m |
| Obstruction 0x10400, 3 m, approximate | title "Obstruction - 3.0 m"; Position; Positionsgenauigkeit: Annäherung; Höhe: 3.0 m |
| Facility 0x10903, 0x10021 | Einrichtungen: Bootsrampe, Marina, Behindertengerecht |

- Decided (2026-11-27): the light is described in chart notation (INT 1 section P), not BaseCamp's words. Source:
  CHS Chart 1 pages "Light characters", "Colours", "Examples of a full light description"; the notation is only in
  their IHO symbol images (`p10_*-iho.svg`, `p11_*-iho.svg`, `p16_*-iho.svg`), read by rendering them.
  - Characters (P10): 10.1 `F`, 10.2 `Oc` `Oc(2)` `Oc(2+3)`, 10.3 `Iso`, 10.4 `Fl` `Fl(3)` `Fl(2+1)`, 10.5 `LFl`,
    10.6 `Q` `Q(3)` `IQ`, 10.7 `VQ` `VQ(3)` `IVQ`, 10.8 `UQ` `IUQ`, 10.9 `Mo(A)`, 10.10 `FFl`, 10.11 `Al.WR`.
  - Colours (P11): `W` (only on sector and alternating lights), `R`, `G`, `Bu`, `Vi`, `Y`; orange `Y` or `Or`,
    amber `Y` or `Am`.
  - Full description (P16): `Fl(3)WRG.15s21m15-11M` = character, colours, period `15s`, elevation `21m`, range `15-11M`.
  - mkgmap's types without a Chart 1 image (group quick and long flashing, fixed and occulting, …) are composed from
    these parts; the group count is not in the data (the marine style puts `seamark:light:group` into the note).
- Decided (2026-11-27): a Morse light is written as on charts, `Mo(U)` (INT 1 P 10.9; CHS Chart 1 page
  "Light characters", images `p10_9-iho.svg`/`p10_9-chs.svg` read "Mo(A)"); BaseCamp's byte value "85" is not copied.
- The two light heights are the other way round from mkgmap's names: mkgmap's height above foundation (30 ft) shows
  as "above datum" 9.1 m, its height above datum (12 m) as "above foundation".
- Whether a value is a depth or a height follows the type (0x10301 depth, 0x10302 and 0x10400 height); the full list
  is not known.
- Not looked at: lines' and areas' details, Label Yellow's details, Sounding Deep.

## BaseCamp 4.7.5: line colour and style (test map `styles.mp`, scratchpad, not kept)

Lines 0x10400 with `Color` 0-15; 0x10400 and 0x10600 with every style value mkgmap keeps; 500 m scale bar, 1 px wide.
- Colour (sampled pixels, anti-aliased, so approximate): 0 dark grey `#727267`, 1 blue `#2ba3f8`, 2 green `#6ade60`,
  3 pale cyan, 4 red `#fc484d`, 5 magenta `#e828e4`, 6 yellow `#fdea5d`, 7 orange `#fd9b4e`, 8 black `#171715`,
  9 light blue `#8ce8f8`, 10 brown `#ad8e5b`, 11-15 pale tints (green, green, pink, khaki, near white). Not mkgmap's
  buoy colour list (there 1 is red).
- Style bits 0-1 dash: 0 solid, 1 short dashes, 2 long dash short gap, 3 long dashes. Bits 4-5 ticks: 0x10 on one
  side, 0x20 on the other, 0x30 both (crosses); the dash applies between ticks. Same on 0x10400 and 0x10600.
- 0x10500/0x10600 without extra data: not looked at.

## Lights on the map

- BaseCamp 4.7.5 (test map `rings.mp`, scratchpad, not kept): a ring per light, radius 4 px per nm, at least 20 px,
  at most 100 px, the same at every zoom; sector lights as coloured arcs; a sector 90-180 lies north-west of the light.
- INT 1 P40-P46 (CHS "Light sectors", IHO images): a flare per light; sector lights with dashed limit lines and one
  arc per sector, coloured on multicoloured charts (white as yellow); no ring for an all-round light.
- OpenSeaMap renderer (`github.com/OpenSeaMap/renderer`, `searender/render.c`, `rules.c`): `renderFlare` a flare in the
  light's colour, magenta for several colours; `renderSector` arcs at `LITRAD` or 0.2 nm (map distance), dashed grey
  `#808080` limit lines, bearings turned by 180° (`540 - SECTR1`); colours white `#ffff00`, red `#ff0000`, green
  `#00ff00`, blue `#0000ff`; sectors from zoom 14 (major lights 12), flares from 14 (major lights always).
- mkgmap stores no light radius (no "radius" in `ExtTypeAttributes.java` or `styles/marine/points`).
- Decided (2026-11-27): lights as OpenSeaMap draws them (INT 1): sector arcs at 0.2 nm with dashed grey limit lines,
  white as yellow, only zoomed in (about 10 m/px); the flare comes with the symbols (step 3).

## Marine symbols (step 3)

- Types from mkgmap's marine style (`styles/marine/points`): lights 0x10100, 0x10107 red, 0x10108 green; buoys
  0x10200 lateral, 0x10202-0x10205 cardinal N/S/E/W, 0x10207 spar/pillar, 0x10208 isolated danger, 0x10209 port,
  0x1020a starboard, 0x1020b special purpose, 0x1020c safe water, 0x1020d barrel; beacons 0x10206 lateral/safe water,
  0x1020e-0x10211 cardinal N/S/E/W, 0x10212 isolated danger, 0x10213 port, 0x10214 starboard, 0x10215 special.
- INT 1 (CHS Chart 1, IHO images, rendered in scratchpad `chs/ref/`): buoys Q20-Q26 lean right on a waterline with a
  position circle; Q2 green/black filled black, Q3 other colours outlined with a letter, Q4 bands top to bottom, Q5
  stripes darker first; beacons Q80-Q83 an upright bar on the position circle; topmarks Q9.
- OpenSeaMap's symbol SVGs (`searender/symbols`) are not reused: the repository has no licence file and the two read
  (`Light.svg`, `Beacon.svg`) carry none; its C sources are GPL 3.
- Decided (2026-11-27): buoys and beacons filled in their colours (bands for several), as OpenSeaMap's tiles, composed
  in code from INT 1 parts (shape by type, colours from the extra data, topmark by category).
- Read off the IHO images: P1 a magenta flare from a star at the position (not a dot); K40 obstruction a dotted circle
  filled light blue, "Obstn"; K41 the same with the depth inside. Cardinal topmarks (Q130.3): N two cones up (BY),
  S two cones down (YB), E cones apart (BYB), W cones together (YBY); pillar and spar buoys lean.
- Not seen as IHO images: Q130.4-6 (not downloaded; chs-shc.gc.ca refused connections afterwards, curl exit 7).
  Isolated danger from Trinity House ("isolated-danger-mark-buoys" page): "Two black spheres, one above the other",
  body "Black with one or more broad horizontal red bands", shape "pillar or spar preferred". Safe water (red sphere,
  red and white vertical stripes) and special (yellow X) only from a web search summary, not checked. Drafts to fix: flare on a star, can buoy leaning, beacon bar wider.

## QMapShack (build of 2026-11-27, same map as IMG), 1 km scale bar

Labels of four points only ("Light Red", "Buoy Red", "Sounding A", "Obstruction"), no symbols, no lines. Marine
extended types have no default look without a TYP.
