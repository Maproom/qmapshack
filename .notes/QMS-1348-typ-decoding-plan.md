# QMS-1348 — Garmin TYP decoding errors

Ticket: QMS-#1348, parent QMS-#1343 (`img-mkgmap-gap-plan.md`, rows T1–T6). Branch `QMS-1348` from `dev` at
`009229d8f`. Development ticket: no `changelog.txt` line; PR to `dev` only when complete.

## State (2026-11-27)

Done: T1–T6 (2026-11-27). Verified:
- `test_CGarminImg` 39/39, `/usr/bin/ctest` 5/5. Each new check failed before its fix: T6 `getFid()` 2, T2 line
  0x18a00 missing, T3 alpha 0xf0, T4 first pixel `#00000000`, T5 label empty, T1 13 label languages instead of 1.
- Documentation: all 113 English shots render identical to `dev` (`shots.py replay`, pixel compare).
- BaseCamp 4.7.5 vs QMapShack, one map with the `styles.txt` styles (16x16 icons): areas 0x02 and 0x18a00, line 0x01
  (red, black border) match. BaseCamp draws no line 0x18a00 and no POI on the map; its POI tooltips show the icons
  as QMapShack draws them. BaseCamp ignores a gmap with family 6357 (6363-6365 load); cause unknown.

## Test harness

- `CGarminTyp` is in `test_CGarminImg`. `test/stub/CMainWindow.h` comes first on the include path and stands in for
  the application's header (which needs the generated main window UI); `CMainWindowStub.cpp` defines
  `getBestWidgetForParent()`. Its message boxes open only for an unknown colour scheme (`default:` of the switch).
- `make_fixtures.py` compiles `test/typ/<name>.txt` into `<out>/<name>.typ`; options from `; mkgmap:` on line 1.
- Icons are premultiplied `BLImage`s; the test wraps them in a `QImage` to read pixels.

## Findings (mkgmap r4924, `javap -c`, TYP byte dumps)

| | mkgmap writes | QMS before |
|---|---|---|
| T1 | scheme 5 (3 colours, 4th none): colours, line width, total width, then label | one width byte, label misread |
| T2 | `type << 5 \| subType`, type = value >> 8 (0x18a for 0x18a00), bit 0x2000 = extended (`TypElement.getTypeForFile`, `CommonSection`) | `& 0x7F` kept 0x0a |
| T3 | 4 bit transparency, 0 opaque … 15 transparent | `~t << 4`: opaque → 240 |
| T4 | 0 colours = true colour, blue/green/red per pixel; mode 0x10: transparent colour first; mode 0x20: 28 bit per pixel LSB first with 4 bit transparency (`TrueImage.write`, `BitWriter.putn`) | skipped or empty |
| T5 | code page 0 in the TYP header without `--code-page`, even with `CodePage=` in `[_id]` | codec "CP0" = none, labels dropped |
| T6 | FID at 0x2f, product at 0x31 | read swapped; `getFid()`/`getPid()` have no other caller |

- `getCodec()`: 0 → Latin1; unknown `CP<n>` falls back to `windows-<n>` (Qt has 949 only as windows-949).
- Points keep `& 0x7FF`: it holds the full type for extended points.
- mkgmap writes POI flags `1 | 2 (night image) | 4 (labels) | 8 (font colours)` (`TypPoint.write`), so its night
  images always come with bits 1 and 2 set. A true colour icon with night flag 0x02 alone, or a night image that
  does not decode, keeps the day icon and drops the labels: their position in the stream is unknown.
- True colour without a colour table accepts only modes 0x00, 0x10, 0x20; mode 0x20 rows run on without padding
  (one `BitWriter` per image; odd-width test icon 0x2f0a).
- An unknown colour scheme in a test TYP would end the test with qFatal: the message box needs a QApplication and the
  test is `QTEST_GUILESS_MAIN`. Left so: a crash is a visible failure.

Review (code-review, 2026-11-27): the points above, the `readTyp()` comment, `quint32(byte)` in `decodeColorTable()`.
After the fixes: `test_CGarminImg` 39/39, `ctest` 5/5, 113 shots identical to `dev`.
Clang-Tidy and Clazy (Qt Creator's, `/opt/Qt/Tools/QtCreator/libexec/qtcreator/clang/bin`, `-p` with `ninja -t
compdb`): own warnings fixed (QColor literals, `QStringView` for `mid()`, `removeIf`/`removeAt`); left: `mirrored()`
deprecated (minimum Qt 6.8), `#pragma pack` from `CGarminImg.h` (`pack(0)` outside Windows; not changed without a
Windows build).

Done.
