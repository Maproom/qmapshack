# QMS-1350 — hide prefix and suffix of road names when zoomed out

Ticket: QMS-#1350, parent QMS-#1343. Branch `QMS-1350` from `dev` at `6feba1acf`. Development ticket: no
`changelog.txt` line; PR to `dev` only when complete. Separator facts: `QMS-1347-label-decoding-plan.md`, "What the
separators do".

## State (2026-11-27)

Decided (2026-11-27): line labels are drawn up to 50 m/px (BaseCamp: still at the 5 km bar); from 5 m/px on as short
names. Below 5 m/px unchanged.

Done (2026-11-27): committed as `[QMS-1350] Garmin maps: short road names when zoomed out`. Remaining:
3. The user pushes (`git push` is refused to Claude): `git -C ~/projects/qmapshack push -u origin QMS-1350`.
4. `gh pr create -R Maproom/qmapshack -B dev -H kiozen:QMS-1350 --title "[QMS-1350] Garmin maps: hide prefix and
   suffix of road names when zoomed out" --body-file <the PR body below>`.
5. After the merge: close QMS-#1350, fast-forward local `dev` to `upstream/dev`.
Test maps: `make_fixtures.py <mkgmap r4924 jar> <dir>`; the session's were in a scratchpad that may be gone.

What was done:
- `IGarminStrTbl::shortText()`: after the last prefix marker (0x1b/0x1e) before a suffix marker, up to the next suffix
  marker (0x1c/0x1f), through `displayText()`. `CGarminPolygon::getShortText()`: that of label 1, a contour's height.
- `CMapIMG`: `kLineLabelThreshold` 50 m/px for line labels at all; from `kShortLabelThreshold` 5 m/px `collectText()`
  takes the short text and skips minor contours 0x20 and boundaries 0x1c (unlabelled in BaseCamp zoomed out).
- Verified: `test_CGarminImg` 58/58 (9 `shortText` rows = BaseCamp's results), `ctest` 5/5, Clang-Tidy/Clazy clean but
  `#pragma pack`. GUI (separator map): full names at 300 m, short at 500 m and 1 km.
- Documentation: `quickstart-vector-map-detail-more.png` changed (the Tannheimer map gains SEEBACH, L261, a contour
  height at that zoom); taken again and published in de/en/es/it (`shots.py -o doc/<l>/images/_work replay --only`,
  `publish --only`). The page text is about Details and stays.

## BaseCamp 4.7.5: scale, not map level

Two test maps, same four roads (`Rue de la~[0x1e]Concorde`, `Main~[0x1f]Street`, `West~[0x1e]Lincoln~[0x1f]Road`,
`Plain Road`, type 0x04), levels A 24/22/20/18, B 24/23/22/21:

| Scale bar | A | B |
|---|---|---|
| 200 m | – | full |
| 300 m | full | full |
| 500 m | short | short |
| 700 m, 1 km, 1.5 km | short | short |
| 2 km, 3 km | – | short |
| 5 km | – | short, two of four (overlap) |

In B the roads look the same at 500 m and 300 m (same level), the names differ: the switch follows the scale. The bar
is about 100 px, so 300 m ≈ 3 m/px, 500 m ≈ 5 m/px.

## QMapShack

- Zoom steps (`IDrawContext::scalesDefault`): … 2, 3, 5, 7, 10 … m/px.
- Road names only below `kStreetNameThreshold` = 5 m/px (`CMapIMG.cpp:47-48`): full names up to 3 m/px, none from
  5 m/px. That is exactly where BaseCamp switches to short names, so the short names need road names drawn beyond the
  threshold.

## BaseCamp: which lines it labels zoomed out

Test map with one labelled line per type, 500 m to 1.5 km: motorway 0x01, residential 0x06, trail 0x16, stream
0x18 labelled; contours 0x22 ("914") and 0x21 ("610") labelled; minor contour 0x20 and boundary 0x1c not labelled.
QMapShack keeps one threshold for all lines.

## Review and measurement (2026-11-27)

- Review: type filter beyond 5 m/px (0x20, 0x1c); empty core falls back to the whole label; `kStreetNameThreshold`
  renamed `kShortLabelThreshold`; doxygen comments; tests for an empty core and `getShortText()` (contours not: no
  `IUnit` in the test). Multiple prefixes cannot occur: `PrefixSuffixFilter.applyPrefixes` returns at the first match.
- Draw time, map thread, OpenFietsMap Benelux over Amsterdam (`IDrawContext::run()` timer enabled locally, not
  committed): index 14 old 45-65 ms / new 48-73 ms; index 16 old 107-226 ms / new 119-161 ms. Neither this map nor the
  Freizeitkarte (names only at the finest level) has line labels at coarse levels, so nothing is added there.
- After the fixes: `test_CGarminImg` 60/60, `ctest` 5/5, Clang-Tidy/Clazy clean but `#pragma pack`; the published
  detail-more images equal fresh renders in de/en/es/it.

## PR body

````
<!---
Note:

- Lines starting with ### are section headers. Do not delete any of the sections.

- Lines starting with the label  [comment]: are instructions on how to fill in the section and will not be shown. Just add your answer below.

-->

### What is the linked issue for this pull request:
[comment]: # (Add the ticket number behind QMS-# )

QMS-#1350

### What you have done:
[comment]: # (Describe roughly.)

- Line labels are drawn up to 50 m/px instead of 5 m/px. From 5 m/px on they are short, as in BaseCamp: only the part
  between the prefix and suffix separators mkgmap writes ("Rue de la Concorde" → "Concorde", "West Lincoln Road" →
  "Lincoln"). BaseCamp switches by scale, not by map level, between its 300 m and 500 m scale bar.
- Zoomed out, minor contours and boundaries stay unlabelled, as in BaseCamp.
- Labels are only drawn where the map has them: the Freizeitkarte and OpenFietsMap carry no line labels at coarse
  levels, so they look as before; their draw times stay the same.
- The image quickstart-vector-map-detail-more shows the new labels; taken again in de/en/es/it.

### Steps to perform a simple smoke test:
[comment]: # (List the steps.)

1. python3 src/qmapshack/map/garmin/test/make_fixtures.py <path>/mkgmap.jar <dir>   (mkgmap r4924)
2. QMS_IMG_FIXTURES=<dir> ctest -R test_CGarminImg
3. Open <dir>/labels8.img: at 300 m the roads read "Rue de la Concorde", "Main Street"; zoomed out to 500 m and 1 km
   "Concorde", "Main".
4. Open doc/shots/fixtures/default/maps/Tannheimer_Tal.img with Details 5 zoomed out: stream and road labels appear.

### Does the code comply to the coding rules and naming conventions [Coding Guideline](https://github.com/Maproom/qmapshack/blob/dev/README_CODING.md):

- [x] yes. Clang-Tidy and Clazy on the changed files: only the existing #pragma pack warning from CGarminImg.h.

### Is every user facing string in a tr() macro?

- [x] yes, no new user facing strings.

### Did you check if the documentation (`doc/`) needs changes, and make them? Check the pages and images that show what you changed.

- [x] yes: quickstart-vector-map-detail-more.png taken again in de/en/es/it; the page text (Details) stays. The other 112 English images render identical.

### Did you add the ticket number and title into the changelog? Keep the numeric order in each release block.

- [ ] no: development ticket; QMS-#1343 gets the line when it is done.
````
