# QMS-1355 — TYP language selection, icon sets, several TYP files per gmapsupp

Ticket: QMS-#1355, parent QMS-#1343 (`img-mkgmap-gap-plan.md`), the last child: when it is done QMS-#1343 is closed
and gets its changelog line (see there). Branch `QMS-1355` from `dev` at `d709515f1`. Development ticket: no
`changelog.txt` line of its own; PR to `dev` only when complete.

## State (2026-11-28)

Steps: 1. language, 2. icon sets, 3. several TYP files per gmapsupp. Each checked against BaseCamp 4.7.5.
Step 2 done, not committed: icon sets need no drawing (below); test `typIconSet`. `test_CGarminImg` 132/132.
Step 3 done, not committed: `CGarminImg::readMapFamilies()` gives each tile its family from the MPS map list
(`L` records: product, family, map number), `load()` stamps it on every object (`family`). With two or more TYPs
`CMapIMG::setupTyp()` decodes each into `familyStyles` by its FID; `styleOf()` picks an object's styles, `draw()`
draws each layer family by family. One TYP or none: as before (`base`). Fixture `mp/families/` (`make_fixtures.py`:
a tile's options apply from that tile on, `<tile>.txt` is its TYP), test `mapFamilies` (fails without the MPS
reader, checked). `test_CGarminImg` 134/134, ctest 5/5. GUI: `fam/qgui/q1.png` A red, B blue as BaseCamp
(`fam/bAB.png`). Replay of `quickstart-vector-map`: no image changed.
Review (2026-11-28), fixed: each layer is one pass again, styles looked up per object (`styleOf(family)`): areas in
the first TYP's draw order, lines by type with 0x01-0x03 last, across all families; a family without a TYP takes the
built-in styles (`builtin`), not the first TYP's; the first TYP is decoded once; built-in names are cleared only when
the TYP's names can be read (a codec). GUI `fam/qgui2/q1.png`: family A's 0x01 over family B's line and area, family
C (no TYP) in built-in style. Tests 134/134, ctest 5/5, replay unchanged. Next: PR with QMS-1343's changelog line,
close QMS-#1343.

Step 1 done, not committed: `CGarminTyp::name()` picks a type's name; `languageCode()` maps the GUI language
(`IAppSetup::language()`) to the TYP code; a TYP entry's names replace the built-in ones, `firstLanguage` keeps the
first name's code. `CMapIMG` shows points, lines and areas through `name()`; the unused `languages` table and
`selectedLanguage` (never set, always `NOIDX`) are gone. Tests `typName` (the BaseCamp cases below), `languageCode`,
`typStyles` (built-in name replaced; fails without the fix, checked). `test_CGarminImg` 131/131, ctest 5/5. Not seen
in the GUI: Xephyr has no window manager, so no hover tooltip.

## Language (step 1)

Before: `selectedLanguage` was never set, so names came from code 0x00 only; a TYP's names were added beside the
built-in 0x00 name (`tr("Stream")`, `setupTyp()`), which then won.

BaseCamp 4.7.5 in German, unlabelled lines whose TYP names come in this order (scratch `lang/src`):
| Names in the TYP | Shown |
|---|---|
| 0x00, 0x02 German, 0x04 English | German |
| English, German | German |
| English | English |
| 0x00 | 0x00 |
| French, Dutch | French |
| French, English | French |
| English, French | English |
| English, 0x00 | 0x00 |
Rule: the user's language, else 0x00, else the TYP's first name; no English fallback. As mkgmap's TYP documentation
says.

## Icon sets (step 2), done: nothing to draw

mkgmap's `[_icons]` section: "a set of images at different resolutions … selected according the capabilities of the
device" (`doc/typ-compiler.txt`); class `TypIconSet`, written to its own header section. `CGarminTyp::parseHeader()`
reads points, polylines, polygons and draw order only. BaseCamp 4.7.5 draws no set (scratch `icons/b1.png`): a point
with its own icon and a set (8/24/32 px) shows its own 16 px icon, a type with only a set Garmin's built-in symbol.
QMapShack draws the same (`icons/qgui/q1.png`). Test `typIconSet` (`typ/iconset.txt`) keeps the TYP decoding.

## Several TYP files (step 3)

Before: `setupTyp()` took the first TYP subfile (`break`) for all tiles. A mkgmap gmapsupp carries the family per
tile in `MAKEGMAP.MPS` and per TYP in its header (FID at 0x2f); TYP subfiles are named `0000000A`, `0000000B`, ...

BaseCamp 4.7.5 (user, 2026-11-28: the reference): the families built as two `.gmap` folders draw each with its own
TYP (`fam/bAB.png`). It shows one map product at a time, a `--transparent` one too, so how overlapping families stack
is not seen; QMapShack keeps drawing layer by layer (areas, lines, points), each layer family by family.
