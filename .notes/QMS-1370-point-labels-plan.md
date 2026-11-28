# QMS-1370 — point labels beside the symbol

Ticket: QMS-#1370, parent QMS-#1343 (`img-mkgmap-gap-plan.md`). Branch `QMS-1370` from `dev` at `120e6520c`.
Development ticket: no `changelog.txt` line; PR to `dev` only when complete.

Done: PR #1375 merged as `64dcc38d6` (2026-11-28), ticket closed.

## State (2026-11-28)

Step 1 done: branch, this plan. Step 2 (BaseCamp measured) done, see below.
Step 3 done, not committed (2026-11-28): `CGarminPoint::labelBox()` places every point label (TYP icon, built-in symbol,
none); `CMapIMG::addLabel()` measures in the label's own font, skips NoLabel and empty labels, stores the pen origin;
`drawLabels()` draws there. Clutter box centred on the TYP icon. Member `fm` gone. Test `pointLabelBox` (BaseCamp's
8/16/24 px rows). Built (user allowed builds in this session); `test_CGarminImg` with fixtures 114/114, ctest 5/5.
GUI (Xephyr, test map, `qgui/q2.png`): every label beside its icon, text box bottom on the point, as `bc/r4.png`;
capitals about 3 px higher than BaseCamp's (cap bottom 404 vs about 407 at the 16 px icon), presumably the larger
descent of the map font. Next: step 4.
Seen, not changed: a TYP icon at a fractional pixel position is drawn blurred (`blitIcon()` at `pos - size/2`).
Code review (2026-11-28), fixed: label fonts taken once per draw (`labelFonts`), not per label; box measured with the
QRawFont advances `buildTextRun()` draws with; `labelFont()` steps a pixel-sized font in pixels, at least 1. Rebuilt,
`test_CGarminImg` 114/114, ctest 5/5; GUI `qgui/q3.png` differs from `q2.png` only in the small labels, 1 px lower
(descent rounded up). Left: the draw thread reads `getMapFont()` unlocked (once per draw now; `drawPolylines()` too).
Step 4 done (2026-11-28): replay of `quickstart-vector-map` against the published images (en/de/es/it); only
`detail-more` changed by the ticket (2021-2954 px); en/de's other five differ by the shifted map of QMS-1330, kept
(`shoot/CLAUDE.md`, step 3). `detail-more` taken again and published in all four, `doc/html` regenerated.
Decided (user, 2026-11-28): 1 px gap for every font size; BaseCamp's 0 px with the small font is to the first ink pixel
of its own font.
Decided (user, 2026-11-28): BaseCamp's rule for every point, the QMS-1354 symbols included ("capitals centred"
goes).

Steps:
1. Branch and plan.
2. Measure BaseCamp's placement.
3. `CMapIMG::drawPoints()`/`drawPois()`: label beside a TYP icon, one placement shared with `drawSymbol()`; clutter box
   centred on the icon. Test, GUI check.
4. Retake the `quickstart-vector-map` images (en/de/es/it; its `Tannheimer_Tal.img` carries `00000MAP.TYP`), compare
   with a replay of `dev`, regenerate `doc/html`.

## Code today

- `drawSymbol()` (built-in marine and POI symbols): label beside the symbol, capitals centred on it (QMS-1354).
- `drawPoints()`/`drawPois()` with a TYP icon: `rect.moveCenter(pt.pos)`, label centred over the icon.
- Clutter box of a TYP icon is `QRectF(pt.pos, size)` while the icon is blitted at `pos - size/2`: box shifted by half
  the icon size.

## BaseCamp 4.7.5

Test map: scratch `lbl1370/src/70130001.mp` + `lbl1370.txt` (TYP, FID 7013): icons 8x8, 16x16, 24x24, 32x12 with
Small/Normal/Large font, one NoLabel, a city 0x0400, restaurant 0x2A00 and 0x6401 without TYP. POI subtypes above 0x1f
make mkgmap exit without a message. Most of these types BaseCamp draws only zoomed in (50 m bar); 0x2f04 already at
500 m.

One rule for all (measured at the 50 m bar, `bc/r4.png`):
- Label left edge 1 px right of the icon (0 px with the small font).
- Bottom of the text box (descenders) on the point's y, the icon's centre: the text sits in the upper half, above the
  centre. Same for the city (a dot, no icon), the built-in restaurant symbol and the bridge.
- NoLabel: icon only. TYP font sizes Small/Normal/Large honoured.

QMapShack today on the same map (`qgui/q1.png`): TYP labels centred over the icon; restaurant label capitals centred on
the symbol, about half a cap height plus the descent lower than BaseCamp.

Seen, not in this ticket: BaseCamp shows all-caps labels in title case and TYP labels without a custom colour in blue;
it draws a dot for a city, QMapShack nothing.
