# QMS-1354 — POI address, phone, highway exits; map copyright

Ticket: QMS-#1354, parent QMS-#1343 (`img-mkgmap-gap-plan.md`, "POI address/phone", "Highway exits", "TRE3 copyright
list"). Branch `QMS-1354` from `dev` at `aa5cdd434`. Development ticket: no `changelog.txt` line; PR to `dev` only
when complete. Work in chunks, each reviewed by the user before the next.

## State (2026-11-28)

Step 1 in progress: `mp/poi.mp`, `osm/exits.osm` built with r4924 and dumped (scratchpad `lbldump.py`); every
record as under "mkgmap encoding". Found: Hilltown is a city point reference (point 1, subdivision 2); the exit's
label is "<ref> <name>" ("12 Hilltown North"), its description `exit:to`. TRE3 holds the `Copyright=` line and
mkgmap's "program licenced under GPL v2"; the text after the TRE header, today's "copyright", is mkgmap's licence
text ("Map data (c) OpenStreetMap…"). Step 1 done: `make_fixtures.py` builds `osm/*.osm` too (options in the first
comment, without dashes); `test_CGarminImg` 97/97.
Step 2 done, not committed: `CGarminPlaces` reads the place tables and the LBL6 record into `CGarminPoint`
(`houseNumber` … `phone`) while a tile's points are decoded, as the marine texts are (not on hover as first planned);
cities cached per index. A city point reference counts the subdivision's indexed points from 1, subdivisions from 1
(`MapBuilder`, `div.startIndPoints()`), resolved by `CGarminImg::indexedPointLabel()`. Tests `base11`, `poiDetails`;
`test_CGarminImg` 102/102, Tannheimer_Tal loads. No display yet.
Steps 3-5 done, not committed (2026-11-28): exits (LBL9/LBL10, facilities chained to the one marked last);
copyrights = per tile TRE3 messages, then the text after the TRE header, decoded in the LBL code page and bounded to
the TRE part (the 0x7FFF read caused the "Mapping a file beyond its size" warnings); `getAddressText()`/`getExitText()`
as BaseCamp shows them (see below) in the hover info, the address also in the description of a waypoint made from the
POI. 24 strings in `qmapshack_de/es/it.ts`, `unfinished`; the 35 QMS-1353 entries' location paths fixed there.
`test_CGarminImg` 111/111, ctest 5/5. Not checked in the GUI: Qt shows no tooltip and the canvas takes no click on
Xephyr without a window manager (window never active; `WA_AlwaysShowToolTips` is what the doc panel uses).
Step 6 (user, 2026-11-28: symbols drawn ourselves; draft 1 style approved): `map/garmin/CPoiSymbol` draws 0x2A00
restaurant, 0x2B01 hotel, 0x2F01 fuel (white pictogram on a rounded square in the group's colour) and 0x6616 summit
(brown triangle). `CMapIMG::drawSymbol()` draws marine and POI symbols for points the TYP does not style, from
`drawPoints()` (RGN points: standard POIs) and `drawPois()` (extended points: marine); POI label beside the
symbol, capitals centred on it. Test `poiSymbols`; `test_CGarminImg` 112/112, ctest 5/5. GUI (Xephyr, `poi.img`):
restaurant drawn with its label beside it (scratchpad `qgui/g3x.png`). Contact sheet: scratch tool `poisym/`.
Step 6 decided (user, 2026-11-28): every POI and geographic type gets its own symbol (option 2), as SVG icons, not
drawn in code. Icon sets, all CC0 (`LICENSE.txt`/`LICENSE.md` in the clones of 2026-11-28): Maki (mapbox/maki, 215,
15 px), Temaki (rapideditor/temaki, 557, 15 px), osmic (gmgeo/osmic, 170, 14 px). Type → icon for all 130 types of
mkgmap's default style but the cities: scratchpad `map.txt` (98 Maki, 30 Temaki, 2 osmic); contact sheet
`svgsheet/contact.html` (white icon on the group colour's rounded square; geographic 0x64-0x66 coloured with a
light rim, no square). The code-drawn `CPoiSymbol` drafts are superseded.
Step 6 implemented (2026-11-28, user: "proceed like that for all other icons"): the 113 SVGs used, unchanged, in
`src/icons/garmin-poi/{maki,temaki,osmic}` with each set's licence and a README (source, commit);
`garmin-poi.qrc` in the app's and the test's resources. `CPoiSymbol` maps 130 types to them, renders each
symbol once per pixel ratio (QSvgRenderer, cached under a mutex) and blits it. Test `poiSymbols` draws all 130
(a wrong icon path fails it, checked); `test_CGarminImg` 112/112, ctest 5/5. GUI (Xephyr, scratch map of every
type `allpoi/allpoi.mp` as `qgui/maps/poi.img`): all 130 drawn with the label beside, sharp at 1x (`qgui/a6.png`,
`a7.png`, `a7crop.png`). A map entry with only `filename` and `isActive` stays unused (`CMapItem::noShadowConfig()`).
Dense points: a label may cover a neighbour's symbol, as with TYP icons (labels are checked against labels only).
Translations (user, 2026-11-28): German of the 24 checked; Spanish and Italian taken as written, all of
`CGarminPoint`, `CGarminMarine`, `CGarminPolygon` without `unfinished`. Documentation (2026-11-28): replays of
`quickstart-vector-map` en/de/es/it; only `detail-more` changed by the ticket (peak signs), taken again in all four
and `doc/html` regenerated; the others keep their images; page text unchanged (user). Open: PR and ticket text,
when asked.
Independent review (2026-11-28): a place table past the file's end threw `exce_t` on the draw thread (crash,
reproduced); fixed: `CGarminPlaces` gets a reader that returns nothing outside the LBL part, record offsets in
64 bit, `readFile()` bound in 64 bit. Also: POI records cached per LBL6 offset (file reads per draw, not measured),
indexed point size checked (8/9 bytes) and bounded to the subdivision, TRE3 only with a header of 0x3B or more,
symbol blitted at its own size. Tests `placesRecords` (2-byte indexes, country city, own flags, overnight
parking, facility chain, tables out of range) and `corruptPlaces` (fails without the fix, checked);
`test_CGarminImg` 114/114, ctest 5/5. Ticket text extended on GitHub (title and both sections).
mkgmap's default style emits 141 point types (`styles/default/points` of r4924): the candidates for the full set.

## Today's code

- `CGarminPoint` keeps `isLbl6`; `IGarminStrTbl::calcOffset(poi)` reads only the first 3 bytes of the LBL6 record
  (the name) and ignores the rest.
- `hdr_lbl_t` has `lbl6_glob_mask` and the offsets of LBL2-LBL11; none but LBL1/LBL6 is read.
- `CGarminImg.cpp:321` takes the string right after the TRE header as "copyright"; the TRE3 list (`tre3_offset`) is
  not read. `CMapIMG` joins them into `copyright`, shown only as the map item's tooltip (`CMapItem.cpp:251`).
  That string is mkgmap's licence text, see State.

## mkgmap encoding

Source: mkgmap trunk, `svn export` of `imgfmt/app/lbl` and `imgfmt/app/trergn` (2026-11-28); keys and tags from the
r4924 jar (`javap -v`). Trunk vs r4924 differences in these files: unverified.

LBL sections (`PlacesHeader`): LBL2 country (3), LBL3 region (5), LBL4 city (5), LBL5 POI index, LBL6 POI
properties, LBL7 POI type index, LBL8 zip (3), LBL9 highway (6), LBL10 exit facility (5), LBL11 highway data (3).
Indexes are 1-based; record sizes from the header, extra bytes skipped (`LBLFileReader`).

- **Country**: 3-byte label offset (`& 0x3FFFFF`). **Region**: 2-byte country index, 3-byte label. **Zip**: 3-byte
  label.
- **City**: 3 bytes, then 2-byte info. Info bit 0x4000: the 14 bits are a country index, else a region index. Bit
  0x8000: the 3 bytes are point index (1 byte) and subdivision number (2 bytes), the name is that point's label;
  else a label offset. MapBuilder does write point references (`setPointIndex` called there).
- **POI record** (`POIRecord.write`), at the RGN point's LBL6 offset: 3-byte name label, bit 0x800000 = own flag byte
  follows. Global flags `lbl6_glob_mask`: 0x01 house number, 0x02 street, 0x04 city, 0x08 zip, 0x10 phone,
  0x20 exit, 0x40 tide (never written). The own flag byte numbers only the fields set globally, in that order
  (`makeLocalMask`); bit 0x80 always set. Fields in order:
  - house number: first byte bit 0x80 → base-11 digits ('0'-'9', '-'), two per byte (`c1 * 11 + c2`), last byte
    bit 0x80, a single byte followed by 0xF8; else 1 + 2 bytes label offset ("221b").
  - street: 3-byte label. city: index, `numberToPointerSize(count)` bytes. zip: index, same.
  - phone: as house number.
  - exit: 3 bytes (label `& 0x3FFFFF`, 0x400000 overnight parking, 0x800000 facility index follows), highway index,
    facility index.
- **Highway** (LBL9): 3-byte label, 2-byte offset into LBL11 (in 3-byte records, 1-based), 1 byte 0.
  **Exit facility** (LBL10): 4 bytes (label 0-21, bit 23 last facility of this exit, type 24-27, direction 29-31
  "NSEWIOB", 7 undefined), 1 byte facilities bit set.
- **TRE3 copyright**: list of 3-byte LBL offsets, section after the subdivisions (`TREFile.writeCopyrights`).
  `--copyright-file`: the first message is "not displayed on a device, but shown in BaseCamp" (mkgmap help).

Input:
- `.mp` keys (Polish reader): `StreetDesc`, `HouseNumber`, `CityName`, `RegionName`, `CountryName`, `ZipCode`,
  `Phone`, `Copyright`. No exit keys.
- Exits only from OSM: `highway=motorway_junction` with `exit:road_ref`, `exit:to`; facilities from
  `exit:facility` (default style `points:196`, `highway=services`). Fixture needs an `.osm` source.

## Steps

Each step is one chunk for review.

1. **Fixtures**: `mp/poi.mp` (simple and complex house number and phone, street, city by name and as city point,
   region, country, zip, a POI with only some fields, `Copyright` lines) and `osm/exits.osm` (motorway, junction,
   services); `make_fixtures.py` builds `.osm` too. Dump the LBL sections of the built maps to check the layout above.
2. **POI properties**: decode LBL2/3/4/8 and the LBL6 record into a struct per POI (read on hover, not per draw);
   tests against the fixture's keys.
3. **Highway exits**: LBL9/10/11; tests.
4. **Copyright**: TRE3 list through LBL; decide with the user what the header string is and whether it stays. Tests.
5. **Display**: hover info and the map's copyright, laid out as BaseCamp shows them (BaseCamp run on the fixtures
   first). Translations, documentation check.
6. **Default POI symbols** (user, 2026-11-28: in this ticket): a POI type without a TYP entry gets a built-in symbol;
   today only its label is drawn (`setupTyp()` sets no `pointProperties`, `drawPois()` falls back to an empty
   `unknownProperty`). Source of the symbols open: drawn as `CMarineSymbol`, or Garmin's if their terms allow
   (unverified). BaseCamp's look per type first; documentation images taken again.
7. PR; ticket text extended by the symbols.

## BaseCamp 4.7.5 (fixtures `poi.mp`, `exits.osm`; scratch map of 16 exits, type k and bit k % 8 on exit k)

- POIs 0x2A00/0x2F06/0x2C04 not drawn on the map at any scale tried; found by search, the result's info card:
  `Restaurant One` / `12 Main Street` / `Hilltown, Upper Valley 87654` / `0815-4711`. No country. Search list:
  "Restaurant One, Hilltown, Upper Valley, Testland". Bank: `221b Baker Street` / `Rivertown, Lower Valley` /
  `+49 89 1234`.
- Exit drawn at 200 m bar as a small square with its label; click → "Exit 12 Hilltown North" / "Test Motorway";
  details: `Name: A 1 Exit 12 Hilltown North`, `Description: Hilltown` (exit_to), `Overnight Parking: No`, one line
  per facility `<description>: <type>, <direction>`. Facility bits not shown.
- Types (English UI): 0 24-hour Diesel Fuel With Restaurant, 1 Diesel Fuel With Large Vehicle Clearance,
  2 Automobile Fuel, 3 Restaurant, 4 Hotel/Motel, 5 Vehicle Repair and Service, 6 Diesel Engine Service,
  7 Commercial Vehicle Wash, 8 Campground and RV Service, 9 Medical Facilities, 10 Automated Teller Machines,
  11 Forest, Park, Preserve, or Lake, 12 Useful Services, Sites, or Attractions, 13 Fast Food, 14/15 empty.
- Directions: N North of Exit, S South of Exit, E East of Exit, W West of Exit, I Inner Side of Exit, O Outer Side of
  Exit, B Both Side of Exit.
- Tiles must not overlap: BaseCamp shows one tile per area, so `exits.osm` lies 0.1° north of `poi.mp`.

## Open checks

- BaseCamp's display of address, phone, exits and copyright (step 5).
- City point reference: which subdivision numbering, and whether QMS has that subdivision's points at hand.
- Does a real map (e.g. `doc/shots/fixtures/default/maps/Tannheimer_Tal.img`) carry POI properties at all.
