# Garmin IMG decoder vs mkgmap — gap list

Tickets: top QMS-1343; 1357 decoder test (reader split out of CMapIMG, mkgmap fixtures); 1344 C1–C2, 1345 R1–R3, 1346 R4–R5, 1347 L1/L2/L5–L8, 1348 T1–T6, 1349 L3/L4 road labels and
shields, 1350 prefix/suffix hiding, 1351 one-way arrows, 1352 draw priority, 1353 marine extra data, 1354 POI
details and copyright, 1355 TYP languages/icon sets/several TYPs.

Reference: mkgmap trunk (`svn export https://svn.mkgmap.org.uk/mkgmap/mkgmap/trunk`), encoder in
`src/uk/me/parabola/imgfmt/`. Nothing below was run against a real mkgmap
file yet. "read" = both sides read; "agent" = from a source-reading agent, not re-read; "unverified" = needs a run.

Effort: S ≤ 0.5 d, M 0.5–2 d, L > 2 d. Each item needs an mkgmap-built fixture and a BaseCamp comparison.

## Decoding bugs

| # | Bug | Where | Trigger | Status | Effort |
|---|---|---|---|---|---|
| C1 | Subfile offset truncated to 32 bit | `CMapIMG.cpp:924`, `CMapIMG.h:49`, `IGarminStrTbl` offsets | gmapsupp > 4 GiB | read | S |
| C2 | Image without TRE/RGN activates, `draw()` decrements `maplevels.constEnd()` of an empty vector | `CMapIMG.cpp:991-993, 479, 1467-1469` | `<name>_mdr.img` from `--index` | read | S |
| R1 | Extended point extra bytes skipped as 1 byte; mkgmap writes 1/2/3/nob+2 | `CGarminPoint.cpp:77-79`; mk `ExtTypeAttributes.java:564,686,761,765,788,797` | marine style, `mkgmap:xt-*` | read | S |
| R2 | Extended line/polygon extra bytes (0x80) not skipped | `CGarminPolygon.cpp:205-300` (`decode2`); mk `Polyline.java` | `xt-style`, depth areas | read | S |
| R3 | `lbl_in_NET` not reset by `decode2()` on the reused `p` | `CMapIMG.cpp:1728, 1844, 1864`; `CGarminPolygon.cpp:96` | routable map with labelled ext. lines | read | S |
| R4 | Levels matched by number, not bits, across tiles | `CMapIMG.cpp:1467-1473, 1613` | gmapsupp mixing `--levels` | agent | M |
| R5 | +180° (0x800000) read as −180° | `CMapIMG.cpp:1029-1035, 1158, 1204` | tiles at the antimeridian | agent | S |
| L1 | Format 6: `str6tbl3[c1]` out of bounds for escape 0x2B/0x2C (table has 27 entries) | `CGarminStrTbl6.cpp:81-82, 152` | ASCII map + `--road-name-config` | read | S |
| L2 | NET label at offset 0 skipped (`p.lbl_info` must be non-zero) | `CMapIMG.cpp:1789, 1813` | first road in NET1 | read (QMS side) | S |
| L3 | Only the first of up to 4 NET labels read; street name behind shield ref lost | `IGarminStrTbl.cpp:119-124` | default style motorways/primaries | read | S |
| L4 | Shield codes dropped, ref shown as plain text | `CGarminStrTbl8.cpp:62-64`, `CGarminStrTbl6.cpp` | default style | read | M (with drawing) |
| L5 | Invisible separators 0x1b/0x1c joined with a blank | `CGarminStrTbl8.cpp:52-60`, `CGarminStrTblUtf8.cpp:51-59`, `CGarminPoint.cpp:129`, `CGarminPolygon.cpp:384` | `--road-name-config` | agent | S |
| L6 | Codepages 874/932/936/949 fall back to Latin1 | `IGarminStrTbl.cpp:30-44`, `CMapIMG.cpp:1353` | `--code-page=932` etc. | read | S |
| L7 | Labels capped at 200 bytes | `CGarminStrTbl8.cpp:43`, `CGarminStrTblUtf8.cpp:42` | long CJK/Cyrillic names | unverified | S |
| L8 | `toFloat()` on "376 m" after the 0x1f height suffix gives 0 | `CGarminPolygon.cpp:378`, `CGarminPoint.cpp:109` | style using `height:` on contours | unverified | S |
| T1 | TYP line scheme 5 reads one width byte, mkgmap writes two; label read from the border-width byte | `CGarminTyp.cpp:675, 701, 789`; mk `TypLine.java:71-73` | 4-colour line with 4th "none" | read | S |
| T2 | TYP extended type masked `& 0x7F` | `CGarminTyp.cpp:240, 536` | `Type=0x18a00` | read (QMS side) | S |
| T3 | Mode-0x20 colour table: opaque → alpha 240 | `CGarminTyp.cpp:932`; mk `ColourInfo.java:160-163` | partially transparent POI icon | read | S |
| T4 | True-colour POI icons (0 colours) not decoded | `CGarminTyp.cpp:855-856, 958-960, 1071-1076` | `DayXpm="w h 0 0"` | agent | M |
| T5 | Codepage 0 → codec "CP0" → TYP strings dropped | `CGarminTyp.cpp:74` | mkgmap without `--code-page` | unverified | S |
| T6 | FID/PID read swapped (no visible effect) | `CGarminTyp.cpp:109` | – | read (QMS side) | S |

## Missing features

| Feature | Status | Effort |
|---|---|---|
| One-way arrows (direction flag decoded, unused) | agent | M |
| Extra-byte content: depth/height values, extended line styles | agent | M |
| Extra-byte content: lights | agent | L |
| TRE draw priority (0x40) for overlay maps | agent | M |
| TRE3 copyright list; map-info text in the LBL codepage | agent | S |
| Prefix/suffix hiding (0x1e/0x1f) at low zoom; 0x1d split | agent | M |
| POI address/phone/city/zip (LBL tables) | agent | M |
| Highway exits | agent | M |
| NET road data, house numbers | agent | L |
| TYP multi-language selection | agent | S |
| TYP icon sets | agent | M |
| Several TYPs per gmapsupp (needs MPS family ids) | agent | M |

Out of scope: DEM subfile, gmapi `.gmap` folders, TDB/MDX map sets, MDR search index, NOD (routing graph; NET only
points into it). NOD and SRT are not used by `CMapIMG.cpp` (grep); display-only relevance not tested.

## Rough total

- Bugs: ~20 items, mostly S → about 1–1.5 weeks including fixtures and BaseCamp comparison.
- Display features (M): about 2 weeks.
- L items (lights, NET road data): about 1–1.5 weeks.
- Prerequisite: an mkgmap jar and a set of small fixture maps (marine style, `--road-name-config`, codepages,
  TYP variants, `--index`) — about 1 day.
