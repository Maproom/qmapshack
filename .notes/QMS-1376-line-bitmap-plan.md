# QMS-1376 — line bitmaps drawn on the wrong side of the line

Ticket: QMS-#1376, parent QMS-#1343 (`img-mkgmap-gap-plan.md`). Development ticket: no `changelog.txt` line; PR to
`dev` only when complete. Branch `QMS-1376` from `dev` at `64dcc38d6`.

Done: PR #1377 merged as `d709515f1` (2026-11-28), ticket closed.

## State (2026-11-28)

Fixed, not committed: `CGarminTyp.cpp` mirrored every line bitmap top to bottom (`mirrored(false, true)` before
`toOwnedBLImage()`, there since 2019); now the rows stay in order. Test: `styles.txt` line 0x03 (first row solid),
`typStyles` checks the rows; it fails with the old mirroring (checked). `test_CGarminImg` 114/114. GUI: the cliff test
map matches BaseCamp in star, both squares and zigzags (`cliff/cmp_star2.png`, `cmp_sq2.png`, `cmp_zz2.png`).
Documentation: replay of `quickstart-vector-map` against the repo: only `detail-more` changed (328 px, the cliffs at
the top), contact sheet `contact1376/index.html`. Taken again and published in en/de/es/it (user, 2026-11-28),
`doc/html` regenerated. Open: review, PR.

## Found (2026-11-28)

Test map: TYP line bitmap 32x5, solid first row with a gap at 28-31, sawtooth rows below (`Xpm="32 5 2 1"`; a line
bitmap takes two colours, three make mkgmap's TYP compiler exit without a message). Type 0x14 with
`UseOrientation=Y`, 0x18 with `N` (BaseCamp draws nothing for line types 0x30/0x31). Lines: a star of 8 rays drawn
outward, a clockwise and a counter-clockwise square, a zigzag with and without `DirIndicator=1`. Source was in the
session scratchpad (`cliff/src/70140001.mp`, `cliff.txt`); rebuild it from this description.

- BaseCamp 4.7.5: the bitmap's first row lies left of the line's direction; teeth inside the clockwise square,
  outside the counter-clockwise one. Same with `UseOrientation=Y` and `N`.
- QMapShack: flipped top to bottom only. East ray, pixel dump: first row below the line, teeth above; the gap still
  after 28 px from the line's start, so the direction along the line is right.
- `UseOrientation=N` is line flag 0x02 (mkgmap `TypLine`: `if (!useOrientation) flags |= 2`); `CGarminTyp.cpp` leaves
  it unread (`renderMode`). BaseCamp ignores it too: keep it unread.
- Not found yet: where the rows turn. `decodeBitmap()` (`CGarminTyp.cpp:578`) reads them in order;
  `CMapIMG::drawPolylines()` blits each segment rotated by its angle at `-h/2`.
