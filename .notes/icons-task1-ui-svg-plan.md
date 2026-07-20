# PLAN — Task 1: UI icons to SVG

**Status:** **Phases 0–6 done** (branch `icon_mania`). 3.3 confirmed OK on all three platforms;
6.4 dropped. **Phase 7 is now split: 7a theming (in progress, the important one) and 7b artistic
overhaul (needs an artist, not started).** **Completely independent of Task 2**
(`.notes/icons-task2-waypoint-resolution-plan.md`) — no shared prerequisite, either order.

**Scope:** `src/icons/*.svg` — **324 real icons** (the glob returns 325; one is a committed
autosave backup, see below). UI chrome only: toolbar, menu, dialog, mime and tree-decoration
icons. Explicitly **not** `waypoints/`, `geocaching/icons/`, `poi/`, `lines/`, `8x8/` bullets —
those are data symbols and belong to Task 2 or stay raster permanently. `src/icons/cache/` is
left as is except for copying the chrome stars out (step 5); see Task 2 for its full state.

---

## Why (the three goals)

1. Drop the multi-size PNG export step.
2. Fix icons that are unreadable on dark UI schemes.
3. Eventually a unified, prettier icon set (low-colour; house colours are dark blue / dark green).

Goal 3 needs an artist and gates nothing else. Goal 2 turns out to be gated on goal 3 — see
"Dark mode" below.

---

## Root cause of "Qt renders SVG differently than Inkscape"

**Solved. It is not a Qt limitation.**

`src/icons/makeicons` exports with `inkscape -D`, which crops to the **drawing's bounding box**
and ignores the page. Independently, **314 of 325 SVGs have no `viewBox`** — they declare a
64×64 page while the artwork sits in an arbitrary sub-rect (e.g. `MimeVRT.svg`: artwork at
16.9,12.8 sized 35×35).

So Inkscape crops tight and the PNG fills the frame; Qt honours the page and draws the artwork
small and off-centre. Any future attempt to "just use the SVGs" fails the same way until the
sources are fixed.

### Measured (QSvgRenderer at 32px vs the shipped PNG)

**324 icons, not 325.** `src/icons/*.svg` globs 325, but
`SelectEndPoint.svg.2020_04_11_11_11_42.0.svg` is a **committed Inkscape autosave backup**
(13,786 B, sibling of the real `SelectEndPoint.svg`) with no shipped PNG. `svgdiff` reports it
`NO_PNG` and the bucketing awk filters it out. Delete it — it is junk, not an icon.

**Row 1 is the untouched tree** — no flowRoot strip, nothing modified; run `svgdiff` straight at
`src/icons` and you get exactly 151/51. Rows 2 and 3 have the **empty-flowRoot strip applied
first**, because that ordering is load-bearing: flowRoot inflates the drawing bbox, which is
exactly what `fit-canvas-to-selection` keys off, so fitting without stripping first gives a wrong
canvas.

| source state | severe (>=40% px wrong) | clean (<3%) |
|---|---|---|
| as-is today | **151** | 51 |
| + canvas fitted to drawing | 19 | 238 |
| + text converted to paths | **1** | **275** |

**4 PNGs in `32x32/` have no SVG source** — `FilterSplitTrack`, `GisProject`, `Path`,
`SelectNextPoint`. **Decided (Oliver): convert them to SVG** — Phase 0 of the plan below. Three
need no art: `GisProject` and `Path` are recoverable from git verbatim (verified to render back
to the shipped PNG), and `SelectNextPoint` derives from its `SelectEndNextPoint` sibling. Only
`FilterSplitTrack` needs drawing. Until Phase 0 lands, `uidiff` covers 324 of 328.

Residue after both: 24 icons at >=10%. At 32×32 a 10% diff is ~100 px, which thin-stroke
antialiasing alone produces — the genuinely broken count is likely under ten. Needs eyeballing,
not panic.

### The 24 stragglers (post-hygiene, >=10% px differing; cause = feature in the ORIGINAL svg)

| file | diff | features in original |
|---|---|---|
| SQLiteNoConn.svg | 45.4% | flowRoot, text |
| SQLite.svg | 38.4% | flowRoot, text |
| TrackMinMax.svg | 37.5% | text |
| DeviceSync.svg | 35.5% | **none — unexplained** |
| MySQLNoConn.svg | 29.1% | flowRoot, text |
| SearchGoogle.svg | 28.8% | text |
| SetupCoordFormat.svg | 26.7% | text |
| MySQL.svg | 25.0% | flowRoot, text |
| AutoSaveA.svg | 23.2% | **none — unexplained** |
| Precision.svg | 21.8% | text |
| O.svg | 18.8% | text |
| Font.svg | 18.0% | text |
| TextUnderlined.svg | 17.5% | text |
| LineMove.svg | 16.2% | marker |
| AreaMove.svg | 16.0% | marker |
| CSrcUnknown.svg | 15.5% | text |
| UnitSetup.svg | 13.0% | text |
| WptMove.svg | 11.7% | marker |
| TextBold.svg | 11.6% | text |
| MimePoiPOIActive.svg | 11.4% | text |
| MimeGEMF.svg | 10.9% | text |
| SaveGISAsGpx11.svg | 10.6% | flowRoot, text |
| PastePlain.svg | 10.3% | text, gradient |
| A.svg | 10.2% | text |

**17 of 24 contain text — and that probably means the PNG is the stale artifact, not the SVG.**
The shipped PNG was rasterized by Inkscape at some past date using whatever fonts existed on that
machine; `object-to-path` now outlines the text using **today's** font resolution. A legitimate
difference either way. So for these, "which one is correct" is a judgement call to be made by
eye — do **not** assume the SVG needs fixing.

3 are markers (`LineMove`, `AreaMove`, `WptMove`), and markers are their own defect: Qt renders
`<marker>` but ignores `markerUnits="strokeWidth"`, so arrowheads come out at raw size instead of
scaled by the path's stroke width. `object-to-path` does **not** touch markers — only
`object-stroke-to-path` bakes them — so the hygiene pass does not clear them. Isolating this takes
one 6-line SVG: two lines, stroke-width 1 vs 8, one shared marker; Inkscape scales the second arrow
8×, Qt draws both the same. **A renderer behaviour is cheap to isolate — do it rather than
reasoning about whether it matters.** All 12 icons with live marker refs are baked; see `CLAUDE.md`
for the mechanism and the zero-width-stroke trap.

**2 are genuinely unexplained** (`DeviceSync` 35.5%, `AutoSaveA` 23.2%) — no text, no flowRoot,
no marker, no gradient, no pattern, no embedded raster. These are the only two that warrant real
investigation.

### Junk found in the sources

- **23 files contain an empty `<flowRoot>`** — Inkscape flowed-text boxes with *zero* text
  content. Invisible; their only effect is inflating the drawing bbox (`ToggleGis.svg` reports
  artwork spanning 114×1044 units because of it). `object-to-path` does **not** remove them;
  strip them with a regex first. All 23 verified empty.
- **94 files reference fonts by name** (`Sans`, `Utopia`, `Adobe Helvetica`, `Nimbus Sans
  Narrow`, `Droid Sans`, ...) that may not exist on a user's machine. Icons should carry
  outlines, not font references.
- Only 6 files use `linearGradient`, 1 uses `radialGradient`, 3 embed raster, 3 use `<pattern>`.
  12 apply markers. **No filters, no masks, no clipPath, no CSS `<style>` blocks.** The set is
  well within what Qt renders — the geometry was the whole problem.

### Reproducing the measurement

`svgdiff.cpp` is saved in `.notes/icons-tools/`. Build and run:

```bash
g++ -std=c++17 -fPIC svgdiff.cpp -o svgdiff $(pkg-config --cflags --libs Qt6Svg Qt6Gui Qt6Core)
QT_QPA_PLATFORM=offscreen ./svgdiff /home/oeichler/projects/qmapshack/src/icons
```

The hygiene pass that produced **row 3**. **First set Inkscape's `/tools/bounding_box`
preference to *visual*** (see the Gotcha below) — the chain does not do it for you, and without
it the frame shifts 1-2px and the numbers will not reproduce:

```bash
# 1. strip empty flowRoot (regex, per file):  <flowRoot.*?</flowRoot>  -> ""
# 2. then, per file:
inkscape --actions="select-all:all;object-to-path;select-all:all;fit-canvas-to-selection;\
export-filename:$f;export-plain-svg;export-overwrite;export-do" "$f"
```

**Row 2 (19/238) has no saved reproducer.** It was measured on a run with `object-to-path`
omitted from the chain above; that run was not kept and there is no script for it. Per this
project's own rule (`.notes/icons-tools/README.md`), **treat 19/238 as unverified** — rows 1 and
3 are the reproducible ones, and they are the two the plan actually reasons from.
(Circumstantial support: row 2's 19 severe equals the straggler table's 17 text + 2 unexplained.
That is consistent with the number being real, and equally consistent with it being back-derived.)

**Gotcha:** `fit-canvas-to-selection` honours the `/tools/bounding_box` preference (geometric vs
visual). Inkscape's `-D` export uses the **visual** bbox (stroke-inclusive). Mismatch shifts the
frame by 1–2px. Set the preference to visual before batch-running, or 23/203 files drift (found
on the waypoint set; the same mechanism applies here).

---

## Verified state of the code

### Build / resources — nothing exists yet

- **`Qt6::Svg` is not linked anywhere.** A case-insensitive grep for "svg" across every
  `CMakeLists.txt` / `*.cmake` / `*.cmake.in` returns **zero hits**. Needs both
  `find_package(Qt6Svg)` and a `target_link_libraries` entry.
- **Zero SVGs are registered in any `.qrc`.** `src/qmapshack/resources.qrc` has 1455 entries
  (1442 PNG), `src/qmaptool/resources.qrc` has 86 (85 PNG). The SVGs are build-time sources only.
- Wiring is `qt6_add_resources(RC_SRCS ${RCS})` at `src/qmapshack/CMakeLists.txt:921`
  (`set(RCS resources.qrc)` at :912, into `MAININP` at :931). Not AUTORCC.
- Rasterization is a **manual developer step** (`makeicons` + inkscape); PNGs are committed.
  CMake never invokes it.

### Consumption

- **413 `<iconset>` entries across 95 `.ui` files**, referencing 174 unique resources.
  Baked into `setupUi()`, i.e. constructed once.
- C++: 352 `QPixmap("://icons/...")`, 123 `QIcon("://icons/...")`, 42 `QIcon(":/icons/...")`,
  19 `setIcon(QIcon("..."))`, 94 `addAction(QIcon(...), ...)`, 7 `addItem/addTab`.
- **Two path spellings** for the same resources: `://icons/` (475 ctor occurrences) vs
  `:/icons/` (44). Note raw `://icons/` appears 1006 times overall — most are TRACK2 table rows,
  not constructors.
- `QIcon::fromTheme`: 0 uses. `QIcon::addPixmap`: 0 uses — every QIcon is single-resolution.

### The per-paint problem (prerequisite — see Step 3)

**There is exactly one `static QIcon` in the whole codebase:** `CMapItemDelegate.cpp:263`
(`overviewWarningIcon`).

**Two disjoint groups. Do not add them together — they are counted by different patterns.**

**Group A — fresh ctor from a resource path**: **29 constructions across 24 lines**. Ternaries
build two per line, so count occurrences, not lines (`grep -c` lies here).

**Mind the spelling and the type.** Both `:/icons/` and `://icons/` occur, and Group A is *not*
all `QIcon` — 6 of the 29 are `QPixmap` passed to `drawPixmap`. The matching pattern is
`(QIcon|QPixmap)\(":+/+icons/` **restricted to the five paint-path files in the table below** —
run unrestricted it returns 519, the codebase-wide constructor total from step 4, not 29. Note
`:+/icons/` does **not** match `://icons/` (two slashes), a trap that silently undercounts. Of the 23 `QIcon` sites only `CWksItemDelegate.cpp:302` uses
the `://` spelling; the other 22 use `:/`.

| file | lines | ctors |
|---|---|---|
| `gis/CWksItemDelegate.cpp` | 14 | **18** |
| `gis/CDBItemDelegate.cpp` | 3 | 3 |
| `map/CMapItemDelegate.cpp` | 1 | 2 (+1 static, not counted) |
| `helpers/CPhotoViewer.cpp` | 3 | 3 |
| `gis/wpt/CGisItemWpt.cpp` | 3 | 3 (canvas — see below) |
| **total** | **24** | **29** |

Split: **19 plain lines (19 ctors) + 5 ternary lines (10 ctors) = 29.** Both shapes are
static-able, but not identically:

1. **Plain** — `static const QIcon x(":/icons/...")`. The one-word fix.
2. **Ternary** (5 lines: `CWksItemDelegate.cpp:373/:417/:496/:635`, `CMapItemDelegate.cpp:269`) —
   `isVisible ? QIcon(A) : QIcon(B)` needs **two** statics plus a select.

**Group B — `QIcon(icon)` wrapping a per-item `.scaled()` pixmap: 8 sites**
(`CWksItemDelegate.cpp:369/:492/:609/:629/:662`, `CDBItemDelegate.cpp:216/:275`,
`CMapItemDelegate.cpp:259`). These construct no resource path, so they are **not** among the 29.
They **cannot be made static at all** — the pixmap differs per row — and they carry the
`SmoothTransformation` scale, plausibly the real per-row cost. They are TRACK2 (they scale the
*item's* icon), hence out of Task 1's scope.

So Task 1's step 3 addresses Group A only. **The `~11 ms → ~0.3 ms` projection assumes that is the
whole per-paint load; it is not** — Group B stays. Treat it as a ceiling, not a forecast.

- `gis/CWksItemDelegate.cpp` — **14 sites, 0 static**: `:302` (star, **inside `for i < N`**,
  N up to 5), `:373`, `:383`, `:388`, `:391`, `:397`, `:406`, `:408`, `:417`, `:421`, `:496`,
  `:613`, `:630`, `:635`.
- `gis/CDBItemDelegate.cpp` — **3 sites, 0 static**: `:72`, `:75`, `:78` (`drawCheckStateButton`).
- `map/CMapItemDelegate.cpp` — `:269` fresh; `:263` static (the only correct one).
- `helpers/CPhotoViewer.cpp` `paintEvent` — `:113`, `:119`, `:122`, 3 fresh PNG decodes/repaint.
- `helpers/CDraw.cpp:197` `drawToolButton` takes `const QIcon&` and caches nothing; **all 14
  call sites pass a temporary** (`CWksItemDelegate` ×10, `CDBItemDelegate` ×3,
  `CMapItemDelegate` ×1).

**Task 1 also touches the map canvas** (contrary to first assumption):
`gis/wpt/CGisItemWpt.cpp:605-607` draws `MoveArrow.png`, `EditDetails.png`, `SizeArrow.png` —
UI chrome — onto the canvas per `drawItem`, freshly constructed. One item (the selected
waypoint's bubble), not a loop.

### Measured cost (iconbench.cpp, saved in .notes/icons-tools/)

| | µs per paint |
|---|---|
| fresh `QIcon(png)` per paint | **47.06** |
| fresh `QIcon(svg)` per paint | **126.81** |
| static `QIcon(png)`, paint only | **0.81** |
| **static `QIcon(svg)`, paint only** | **1.13** |
| raw `QSvgRenderer` reparse+render | 79.66 |
| pre-rasterized pixmap blit | 0.46 |

**Qt's `QIcon` already caches rasterization internally — a cached SVG icon costs 1.13 µs vs
0.81 µs for PNG. There is no icon cache for us to build.** The only thing that matters is not
constructing the QIcon per paint.

~8 icons/row × ~30 visible rows: **~11 ms/repaint today**; naive SVG swap → **~30 ms**
(visible lag); after `static` → **~0.3 ms**.

### HiDPI — the real payoff

At dpr=2, `QIcon` returns a 32×32 pixmap for a PNG source regardless of the requested size, then
it gets upscaled into a 64px slot. An SVG source returns a true 64×64.

Deviation from ground truth (inkscape @64px), 0 = perfect:

| icon | SVG | 32px PNG |
|---|---|---|
| ShowAll | **1.81** | 13.91 |
| Zoom | **0.24** | 4.06 |
| Save | 4.54 | 4.93 |

(`Save` barely improves — it is one of the 23 flowRoot casualties.)

Note the canvas *is* dpr-aware (`canvas/IDrawContext.cpp:53`, `canvas/CCanvas.cpp:1303`);
`devicePixelRatio` appears 7× in the codebase, all in map/DEM/canvas code, none in icon code.

### Dark mode — harder than it looks

**Verified against Qt 6.9.2 (system) and Qt 6.10.0 (`/opt/Qt`): `QSvgRenderer` has NO colour
API.** The colour-relevant surface is empty; the API is `isValid / defaultSize / viewBox /
setViewBox / aspectRatioMode / options / setDefaultOptions / animation* / boundsOnElement /
elementExists / transformForElement / render / load`. No `setCurrentColor`, no palette. `QIcon`
has none either.

**Qt will not recolour an SVG for you.** The only hook is `QSvgRenderer(const QByteArray&)` —
load the text, rewrite the colours ourselves, hand over the bytes. So dark mode is a
substitution layer **we write**, and it needs the icons to use a known consistent palette to map
from. That couples it to goal 3 (the redesign): realistically dark mode lands *with* the
redesign, not before it.

Tinting the rasterized pixmap (composition SourceIn) only works for monochrome icons; these are
multi-colour, so it is not an option.

### Packaging

`QIcon(":/x.svg")` needs two **runtime plugins**, separate from linking `Qt6::Svg`:
`plugins/imageformats/libqsvg.so` and `plugins/iconengines/libqsvgicon.so`. Both present on this
box (system Qt 6.9.2 and /opt/Qt/6.10.0). `windeployqt` normally picks them up, but this is a new
deployment dependency on Windows and macOS. **Failure mode is silently blurry icons, not blank**:
ship `qsvg` without `qsvgicon` and every icon still renders, just once at natural size and scaled
from there — so nobody reports it. Only dropping the `Qt6::Svg` link entirely, which takes both
plugins with it, gives the loud blank-icon failure. Measured in 3.3; confirm against packaging
before committing to the swap.

---

## Implementation plan — small, testable steps

Each step is independently revertible and has a stated gate. **Only step 2.4 changes what the
user sees**; everything after it is provably invisible, which is the whole point of the ordering.

### Phase 0 — restore the missing SVG sources — **DONE**

`32x32/` shipped 4 PNGs with no `.svg`. Decided (Oliver): convert them. Outcome — only one
needed drawing:

| # | icon | outcome |
|---|---|---|
| 0.1 | `GisProject.svg` | **recovered verbatim** from `fdfbed1f` (deleted as collateral by `068f7ed9` "Update project icons"). Renders back to the shipped PNG at mean diff **0.6**. |
| 0.2 | `Path.svg` | **recovered verbatim** from `a58e49e0` (deleted by `10858eaa`). Mean diff **0.4**. |
| 0.3 | `SelectNextPoint` | **deleted, not drawn.** It had **zero references in `src/` and was in neither `.qrc`** — never compiled into any binary. A stray PNG pair left by `894e256b [QMS-134] Add new range tool`; every sibling is qrc:2 + 1 ref, it alone was 0/0. |
| 0.4 | `FilterSplitTrack.svg` | **reconstructed** as strokes in the house style (red zigzag + 3 black cut marks, matching `FilterLoopsCut`'s idiom: `stroke:#ff0000;stroke-linecap:round`). Live — `IFilterSplitTrack.ui:45`, both qrc. Reads as the same icon but scores **18.2** vs the old PNG: stroke widths and the bottom-left bend are approximate, and the old PNG carries padding the reconstruction does not (it was hand-added as a raster by `c02362d2`, never `-D`-exported, so there is no crop relationship to preserve). **Accepted as-is** (Oliver) — it is chrome, and Phase 7 redesigns it anyway. |

Lesson: two of the four were free, one was dead, one needed art. **Check whether an orphan is
even alive before drawing it.**

### Phase 1 — tooling (no repo change)

| # | step | gate |
|---|---|---|
| 1.1 | ~~Delete `SelectEndPoint.svg.2020_04_11_11_11_42.0.svg`~~ **DONE** (autosave backup, 13,786 B) | 327 SVGs, 327 PNGs, **zero orphans in either direction** |
| 1.2 | ~~Write `hygiene.py`~~ **DONE** — `.notes/icons-tools/hygiene.py` | strips empty `<flowRoot>` (that order matters — the fit keys off the bbox it inflates) -> `object-to-path` -> `fit-canvas-to-selection` -> plain SVG. Pins `/tools/bounding_box` to visual via a throwaway `INKSCAPE_PROFILE_DIR`, so your Inkscape config is untouched. **Adds no `viewBox`** — fit-canvas sets width/height, which is sufficient. Verified: 23 flowRoots stripped, 325 processed, 0 failures, `--dry-run` leaves the repo clean. |
| 1.3 | ~~Write `uidiff.py`~~ **DONE** — `.notes/icons-tools/uidiff.py` | Verified: reproduces the **257/15/17/35** baseline exactly, and **275/15/18/16** on hygiened copies. Asserts SVG<->PNG correspondence both ways, fails on render failure, names the VISIBLE ones as the 2.2 worklist. |

### Phase 2 — repair the SVG -> PNG pipeline

**`makeicons` is a footgun today.** Run it on the committed SVGs and it corrupts **35 of 324**
icons, because the sources drifted from the rasters years ago. Verified by regenerating every
PNG from its own committed SVG:

```
identical (<1)     259        <- PNG matches its source
AA-noise (1-3)      15
minor (3-8)         17
VISIBLE (>8)        36        <- PNG does NOT match its source
```

(Run `./uidiff.py --max-visible 36`, no allowance flags — post-phase-0 the correspondence is
complete. 35 of the 36 are the pre-existing drift; the 36th is the `FilterSplitTrack`
reconstruction, which is a knowingly-new source.)

The correlation is exact: **all 23 empty-flowRoot files are stale** (someone opened them in
Inkscape, it silently added an empty text box, they saved and never re-exported — the flowRoot
inflates the drawing bbox, so `-D` crops differently), plus 12 more
(`AutoSaveA CSrcUnknown DeviceSync Font MimeIMG MimeJNX O Precision SearchGoogle SetupCoordFormat
TrackMinMax UnitSetup`). Worst: `Save.png` at 166/255.

**In every case the PNG is the stale artifact, not the SVG** — the drift is always "someone
changed the source and never re-exported". Confirmed for the two that looked mysterious:

- **`DeviceSync` (26.1) and `AutoSaveA` (15.8)** were not caused by hygiene — pre- and
  post-hygiene renders are byte-for-byte the same score. The drift starts precisely at
  `efa9b98c` "Redesign icons for focus, autosave, sync, and DB buttons" (2026-02-16): at
  `efa9b98c~1` the SVG and PNG agreed at **0.0**; that commit shipped a source and a raster
  already out of sync. Oliver reviewed both SVGs and confirmed they are correct, so the SVG wins
  and 2.4 fixes them.
- **Not a font problem**, despite appearances. Both icons are a single letter, and 32 of the
  SVGs carry vestigial `font-family` / `font-stretch:semi-condensed` style on a `<path>` — the
  fingerprint Inkscape leaves when text is converted to outlines. But `Nimbus Sans Narrow` *is*
  installed and `fc-match` resolves it, `AutoSaveA` has no `font-family` at all, and the letters
  are already paths, so no font is consulted at render time. Those attributes are dead metadata.

**This phase is worth doing even if SVG is never adopted** — it makes `makeicons` safe to run.

| # | step | gate |
|---|---|---|
| 2.1 | Run `hygiene.py` on `src/icons/*.svg`. **Do not regenerate PNGs yet** | `./uidiff.py --max-visible 17` -> **277/15/18/17** (stale drops 36 -> 17); `svgdiff` -> severe **151 -> 1**. App untouched: the PNGs still ship. |
| 2.2 | ~~Eyeball the 17 that still differ~~ **DONE** | **All 17 resolved; the SVG is authoritative in every case.** `FilterSplitTrack` (18.2) is the accepted phase-0 reconstruction. `DeviceSync` (26.1) + `AutoSaveA` (15.8): SVGs reviewed by Oliver and correct — see below. The other 14 are text whose outline no longer matches the old raster. **Nothing to fix: 2.4 regenerates the PNGs from the good sources.** |
| 2.3 | Commit the hygiened SVGs | no app change yet |
| 2.4 | Regenerate PNGs (`./makeicons`), commit | `./uidiff.py` (default gate) -> all identical *by construction*. **17 icons change appearance — the only visual change in all of Task 1.** |

**Why 2.4 matters:** once SVG and PNG render identically, every swap in Phase 4 is provably a
no-op. Skip it and those 17 changes get smeared across a 500-site mechanical diff where nobody
can review them. This front-loads the entire visual risk into one reviewable step.

### Phase 3 — enable SVG (no behaviour change)

| # | step | gate |
|---|---|---|
| 3.1 | ~~`find_package(Qt6Svg)` + link `Qt6::Svg`~~ **DONE, builds** (both apps) | confirmed building by Oliver; nothing looks different. Nothing includes a QtSvg header — the module is linked so `windeployqt`/`macdeployqt` detect the runtime plugins. |
| 3.2 | ~~Add `src/icons/*.svg` to both `.qrc`~~ **DONE** | +306 entries (qmapshack) / +40 (qmaptool), mirroring the icons each app already registers, not a blanket 327. All refs resolve. |
| 3.3 | ~~Confirm Windows ships `iconengines/qsvgicon.dll`~~ **DONE** — confirmed by Oliver, the live Windows packaging deploys it | Phase 3 complete; no packaging change needed |

**3.3 — the packaging trap, measured.** Two plugins matter and they are not interchangeable:

| path | result |
|---|---|
| `QPixmap(svg)` — `imageformats/qsvg` alone | **39x39 fixed** (the SVG's natural size) |
| `QIcon(svg)` — `iconengines/qsvgicon` | 16 / 32 / 64 / 256 — any size, crisp |

Without `qsvgicon`, `QIcon(":/x.svg")` falls back to `QPixmapIconEngine` -> `QImageReader` -> the
qsvg image plugin -> one fixed raster at natural size, then scaled. **Icons do not go blank; they
go blurry.** That is worse: nobody reports it, and the HiDPI payoff — the entire point of Task 1 —
silently evaporates. (An earlier draft of this plan said "blank icons"; wrong, and wrong in the
direction that would have let it ship.)

Per platform:
- **Linux** — the distro packages both plugins. Fine.
- **macOS** — `macdeployqt` (`MacOSX/bundle-common-func.sh:143`) auto-detects plugins from the
  linked modules, so 3.1's `Qt6::Svg` covers it.
- **Windows** — **confirmed OK by Oliver**: the live packaging deploys `qsvgicon.dll`. It lives
  outside the repo (README points at the wiki), so the in-repo scripts could never answer this.
  Do not re-derive the answer from them: `msvc_64/copyfiles.bat` and `CopyFilesGis.bat` copy
  plugins *by name*, list `imageformats\qsvg.dll`, never mention `iconengines`, and are dead
  anyway (26 and 24 references to `Qt5*.dll`, zero to Qt6, untouched since 2022/2023) — they
  would copy DLLs a Qt6 build does not produce.


### Phase 4 — dropped. Do not add it.

An earlier draft of this plan proposed hoisting the per-paint icons into `static`s, on the
argument that the SVG swap would otherwise turn ~11 ms/repaint into ~30 ms of tree lag. **Do not
do it.** Three reasons, in order of weight:

- **A `static` defeats the goal of the whole task.** Task 1 exists to enable dark/light theming;
  Qt has no SVG recolouring API, so theming means loading rewritten SVG bytes per scheme. A
  function-local static is built once on first paint and pins whichever scheme was live then. That
  kills the pattern on correctness, independent of any timing argument.
- **The premise was never established.** ~11 ms is a cost *per repaint*, and the figure was never
  paired with how often a repaint happens or how many rows are on screen. The tree shows a handful
  of items and repaints on hover, selection and scroll — not continuously. An occasional 11 ms is
  invisible. Measure frequency before optimising, not just unit cost.
- **It hides icons from the Phase 5 swap.** A bulk swap matches `QIcon("` — construction with the
  path inside the parens. A static moves the path behind a variable name (`QIcon iconShowAll(...)`,
  `QIcon showAll{...}`), which that pattern cannot see, so those icons silently stay PNG and capped
  at 32 px while a census counts them as converted.

`QIcon` renders per request and caches internally, so a fresh `QIcon` per paint costs a
construction, not a rasterization. If a repaint ever does prove hot, the fix is a cache keyed by
*name plus colour scheme* that a theme change can invalidate — never a bare `static`.

Group B (the 8 `QIcon(scaled-pixmap)` sites) stays — is TRACK2.

### Phase 5 — swap PNG -> SVG, each step provably invisible

| # | step | gate |
|---|---|---|
| 5.1 | ~~**Pilot: one `.ui` file**~~ **DONE — needs a build to confirm** | `gis/trk/filter/IFilterSplitTrack.ui`: chosen because it carries **both** ref forms. Its 2 iconsets are swapped; its `<pixmap>` is deliberately left as PNG. Open Track details -> filter tab; Apply and Help must look unchanged. |
| 5.2 | Bulk `.ui` — **`<iconset>` only** | per-dialog screenshots |

### `<iconset>` is swappable, `<pixmap>` is NOT — verified against uic's output

```cpp
<pixmap>   ->  label->setPixmap(QPixmap(":/icons/48x48/QMapShack.png"));     // natural size
<iconset>  ->  icon.addFile(":/icons/32x32/Map.png", QSize(), Normal, Off);  // sized on demand
```

A `QIcon` asks the qsvgicon engine for the size it needs, so swapping an iconset is **invisible**.
A `QLabel` pixmap has no size request: it renders the SVG at its *natural* size, which after
hygiene is the drawing bbox in the file's own units — arbitrary (`Apply` 40.07, `Help` 40.26,
`FilterSplitTrack` 61.78). So `48x48/FilterSplitTrack.png` (48x48) would become **61x61**: a 27%
resize, differing per icon.

**Leave all 59 `<pixmap>` refs (22 at 32px, 37 at 48px) as PNG.** They are QLabel decorations, not
icons in the QIcon sense. No single natural size could serve them anyway — they are a mix of 32
and 48. Consequence: Phase 6 cannot remove those icons' PNGs. Fixing that properly means giving
each label an explicit size or converting it to an icon, which is a behaviour change and its own
task.

**Swappable `.ui` surface:** 885 raw occurrences at 32x32/48x48 (Designer writes each iconset path
twice), minus the 59 `<pixmap>` refs. **Never touch** the 12 refs to `8x8/` bullets (hand-made
PNGs, no SVG source) or the 6 to `waypoints/` (Task 2, frozen).

Precondition verified before swapping: of the **188 distinct icons** referenced from `.ui` at
32/48, **all 188** have an SVG source on disk **and** are registered in the correct app's qrc.
| 5.3 | ~~Code ctor sites~~ **DONE — `QIcon` only** | 141 `QIcon` refs across 25 files, all resolving. `CWptIconManager` was never at risk: its table holds bare strings, not ctors. |
| 5.4 | ~~Decouple the chrome stars~~ **DONE** | `RatingStar.svg` / `RatingStarEmpty.svg` copied out of `cache/`, hygiened, registered. `CGisItemRate::updateStars` and `CWksItemDelegate`'s `icons_t` use them; `CDetailsGeoCache` keeps `cache/`. No code path from chrome to `icons/cache/` remains. |

### The 42 `QPixmap` code sites stay raster — same reason as `<pixmap>`, plus one worse

Nearly all are `icon = QPixmap("://icons/32x32/XProject.png")` feeding `IGisItem::setIcon` ->
`showIcon()` -> `displayIcon`, **and `displayIcon` is what gets written into the DB `data` blob**.
Swapping them would change the icon's natural size (32 -> ~40), hence `displayIcon`'s dimensions
and every stored blob — for no gain, since the delegate raster-scales them either way. (Making
those crisp needs the delegate to call `QIcon::pixmap(size)`; that is Group B, TRACK2, out of
scope.)

They also **conflict** with the natural sizes 5.2 set: `SearchGoogle` is a 48px `<pixmap>` in
`.ui` but a 32px `QPixmap` in code. No single natural size serves both. `QIcon` sites have no such
problem — the engine is asked for a size every time.

### Final reference census

| | count | |
|---|---|---|
| `.ui` svg refs | **890** | all resolve in the right qrc |
| `.ui` png refs | **13** | 12 `8x8/` bullets (no SVG source), 1 `PointHide` label |
| code `QIcon` svg | **172** | |
| code `QIcon` png | **0** | |
| code `QPixmap` png @32/48 | **42** | GIS item icons; icon is derived from sym/colour, not stored — Task 2 owns converting these (see cat. 2) |

A `waypoints/` ref is **not** automatically "task 2, frozen": the freeze is on **canvas
rendering**, not on the file. The 4 sites that are UI chrome (`CGisListWks.cpp`, `IScrOptRuler.ui`,
2 in `IScrOptSelect.ui`) are SVG — see the waypoint-track note in `CLAUDE.md`. Only `FlagBlue.svg`
and `PinBlue.svg` are registered in the qrc; the other 201 stay unregistered until UI needs them.

**Recount this table, do not read it.** A `QIcon("`-shaped grep is what produced it, and three
kinds of site hide from that shape: a path behind a variable name (`QIcon iconShowAll(...)`), an
`addFile(QString(...))` (`CWptIconSelectWidget.cpp`), and a `QPixmap` ctor feeding a `drawPixmap`
(the `CGisItemWpt`/`CPhotoViewer` bubble icons). All are `QIcon`-on-SVG now, but the lesson is that
the census is only as good as the grep behind it.

### Why the remaining PNGs are PNG — three categories, all deliberate

**1. Fixed-appearance symbols — raster is CORRECT, not a compromise.** The `8x8/` bullets (20) and
`lines/` swatches (17) are **colour swatches**: they exist to show a specific colour from
`IGisItem::colorMap`. Theming them would make them lie — a "dark red" bullet must stay dark red in
dark mode. They must never be recoloured, they are 8px, and vector buys nothing. Same reasoning as
the 6 `waypoints/` refs. **Do not "fix" these; generating SVG sources for them would be a
regression in intent.** (Decided with Oliver.)

**2. GIS item icons (42 `QPixmap` sites) — handed to Task 2, and the "must be raster" reason is
mostly wrong.** The `.qms`/DB do **not** store a rendered icon; they store the *symbol name*
(waypoint `wpt.sym`) and the *colour* (track/area `trk.color`/`area.color`), and the icon is
**re-derived on load** — waypoint via `getWptIconByName(sym)`, track/area by using `Track.png`/
`Area.png` as a shape mask filled with the data colour. A rendered pixmap is persisted only in two
*output caches* (the DB `items.icon` BLOB and `.qms` history snapshots), and a cache stores output —
render an SVG to a pixmap and save that just as well. So **serialization does not force the source
format.** Tracks and areas can be SVG (crisp mask + data colour); only the **waypoint** is a
genuinely frozen raster, and that is the canvas-freeze reason, not serialization. Full analysis and
the derive mechanism live in the Task 2 plan under "GIS item icons: what is actually serialized" —
**do not re-derive it.** One real constraint to remember: `QIcon` will not upscale a raster
(`pixmap(40)` on a 32px source returns 32 — `qtfacts` fact 2), so the fix is to render the SVG at
the target size, not to wrap the old 32px PNG in a `QIcon`.

**3. Everything reached through `QIcon` is SVG** — themeable, HiDPI, sized on demand. That is the
dividing line, and it is a property of the API, not an accident: a `QPixmap` fixes the size at
load and discards the vector benefit regardless. One exception remains: `CGisListWks.cpp:225`, a
`waypoints/` symbol frozen by task 2.

Treat that as an intent, not an observation — re-grep it rather than reading it.

The `QPixmap`-vs-`QIcon` split is about *what the icon is*, not which call the code happens to
use. `CGisItemWpt`'s bubble arrows and `CPhotoViewer`'s buttons are chrome that happened to be
drawn via `drawPixmap`, which reads as "raster by design" and is easy to pass over; they are
`QIcon` on SVG. Of the GIS item icons, only the **waypoint** symbol and the fixed-appearance
swatches are genuinely raster; tracks/areas are raster today only incidentally (cat. 2, Task 2).

### Phase 6 — make SVG the default (was: remove the UI PNGs)

**Rescoped.** The original aim was binary size; that premise was wrong. The svg payload is
~2355 KiB and dropping every unreferenced PNG would recover only ~528 KiB — a poor trade for
deleting 407 files on the strength of a grep. The real goal is **making it clear that SVG should
be used** (Oliver).

| # | step | state |
|---|---|---|
| 6.1 | `src/icons/mkicon` renders one svg at named sizes; the only inkscape caller in the tree | **DONE** |
| 6.2 | Delete the top-level `makeicons` batch script | **DONE** |
| 6.3 | `waypoints/makeicons`, `cache/makeicons` keep batching but call `mkicon` | **DONE** |
| 6.4 | ~~Prune the ~407 unreferenced PNGs from the qrcs~~ | **DROPPED (Oliver).** ~528 KiB back for deleting 407 files on the strength of a grep — little gain, real chance to break. Do not revive. |

**Why the batch script had to go, and why the subfolder ones stay.** A blanket rebuild makes a
raster exist for every icon whether or not anything wants one — so referencing a PNG is always
the path of least resistance, and SVG-by-default is a convention nobody is forced to follow. It
also hid the rot: running the old script would have corrupted 35 of 327 icons. `waypoints/` and
`cache/` are different — every source there has a raster *by intent*, because both sets are
frozen-appearance PNG by design, so batch is correct and the drift is bounded (waypoints max 5.5,
below the visible threshold; `cache/`'s five rotted files — `letterbox` 109, `corrected` 41,
`minicon`/`maxicon` 25, `unknown` 9 — are all dead icons nothing references).

**Why 6.4 is a trap** (the reason it was dropped — a prune must match on the BASENAME, not the
path, and even then it is a grep deciding what compiles). `CActivityTrk`'s `DESCRIPTOR_ENTRY`
builds paths by preprocessor concatenation (`"://icons/16x16/" icon`), which no path-based grep
can see; matching basenames finds it. Verified complete: of the four places a `.png` name is built
at runtime, only `CDetailsGeoCache.cpp:117` (`geocaching/attributes`) touches a qrc resource — the
others write temp files, TwoNav exports and the tile cache. There is no runtime path
concatenation. Basename matching is conservative: it cannot tell `32x32/Save.png` from
`48x48/Save.png`, so it errs toward keeping.

### Phase 7 — dark mode + redesign

**Split into two tracks (Oliver).** They ship independently:

- **7a — theming (light/dark).** The important one. In progress.
- **7b — artistic overhaul.** Needs an artist and a palette decision. Not started, gates nothing.

Remember Qt has **no** SVG recolour API — this is a substitution layer we write (see "Dark mode"
above).

#### 7a — constraints (Oliver, explicit)

- **Recolour at runtime.** Users switch theme while the app is running. A pre-recoloured second
  copy of each svg on disk is disfavoured — but debatable, and up to what the mechanism costs.
- **Proportionate change.** "Don't rewrite QMapShack" is about blast radius, not about the code
  being untouchable — changing code is fine.
- **The icons were drawn for a light scheme.** Any transform inherits that; it is a retrofit.
- Ideal (acknowledged as wishful): Qt does it for us and we write nothing.

#### 7a — DECIDED: reaching dark mode is the platform's job, not ours (Oliver)

**No in-app light/dark toggle, no `setColorScheme`, no app-supplied dark `QPalette`.** A tester
puts *their system* into dark mode however suits their platform and QMapShack follows the
platform palette — which is already all it does (verified: it sets no palette/style/colour scheme
of its own, only a small macOS stylesheet tweak in `CAppSetupMac.cpp`). Per-platform paths for
testers: dark desktop on Linux (or `QT_QPA_PLATFORMTHEME=gtk3`/`qt6ct`), `QT_QPA_PLATFORM=windows:darkmode=2`
on Windows (**`=2` swaps the palette; `=1` only themes the title bar and would NOT trigger our
icons**), system appearance on macOS. Env-var forcing is deliberately NOT relied on — it is
platform-specific and non-portable; the tester's own theme is the test surface.

**Consequence for us:** the only dark-mode wiring we own is the engine reading the ink from
`QPalette` and repainting on `QEvent::PaletteChange` (so a live system switch is followed; a
pre-launch switch is correct from startup anyway).

#### 7a — measured facts (do NOT re-derive; tools in `.notes/icons-tools/`)

**The baked-background theory is dead.** `bgaudit.py` over all 329: **262 are already fully
transparent**, and **exactly one icon has a white background** — `TrkProfile.svg` (confirmed by
hand: bottom shape `fill:#ffffff` + black stroke frame). There is no white-background population
to strip, and **theming is therefore not a background swap.**

The 53 `opaque-bg` + 14 `edge-bg` are a **labelling trap, not a finding**: nearly all are navy
`#000080`, i.e. solid icons that fill their own canvas (`Save.svg` is a navy floppy) or navy
frames touching the edges (`Image.svg`, `Map.svg`, `Poi.svg`). Phase 1's hygiene fit every canvas
to its drawing, so a solid rectangular icon covers 100% **by construction**. Read `bgcolor`.

**The real problem is the drawing colour.** Census over `src/icons/*.svg`:

| fill | uses | note |
|---|---|---|
| `#000080` | 1246 | house navy — **the whole problem**; invisible on a dark ground |
| `none` | 740 | |
| `#ffffff` | 292 | in **127 icons**, as *content* (paper, label fields) — reads fine on dark |
| `#ff0000` | 105 | **semantic** — a red warning must stay red |
| greens | ~107 | `#00ff00`/`#55d400`/`#008000` — **semantic** (ok/active) |
| `#000000` | 55 | |

Two consequences for the "invert dark to light" idea, both load-bearing:
1. **White is content in 127 icons.** A naive invert makes those *worse*, not better — they are
   the parts that already work on dark.
2. **Red and green are semantic.** Inverting red to cyan is a bug, not a restyle. Any rule needs
   a hue carve-out, or must act on lightness only.

So a blind RGB invert is out. An HSL-lightness flip is the obvious next candidate — unverified.

#### 7a — test set

Measured worst-on-dark by `contactsheet.py` (mean luma distance from `#353535`); a rule that fixes
these and breaks nothing else is probably right:

`PointShow`, `CutMode1`, `WptProx`, `CutMode2`, `FilterSplitTrack`, `WptProj`, `TextItalic`,
`CSrcUnknown`

#### 7a — the mechanism: `currentColor` + a STATIC icon-engine plugin on key `svgt`

**Decided and proven end to end.** Working reference implementation: `.notes/icons-tools/svgt-proto/`
(builds and runs standalone; `cmake -S . -B build && cmake --build build`, then
`QT_QPA_PLATFORM=offscreen ./build/svgt_proto`). Renderer facts: `.notes/icons-tools/ccheck.cpp`.

**"Qt has no SVG recolour API" is true but was misleading us.** There is no setter — but Qt's
renderer *does* resolve `currentColor` (measured on 6.9.2, all of: root `color=`, root
`style="color:"`, `<g color=>` inheritance, and `stroke="currentColor"`). We edit the bytes; Qt
does the rest. The forum consensus that QtSvg ignores `currentColor` is **wrong** for Qt 6.9.

How it fits together:

| piece | what it does |
|---|---|
| source svg | ink parts get `fill="currentColor"`; root carries `color="#000080"` |
| `.qrc` | `<file alias="icons/Foo.svgt">icons/Foo.svg</file>` — **alias only**, disk keeps `.svg` |
| `.ui` / C++ | reference `:/icons/Foo.svgt` |
| the engine | static `QIconEnginePlugin`, key `svgt`; replaces root `color=` with the palette ink |

**Why each piece is the way it is — all four are load-bearing:**

1. **STATIC plugin (`qt_add_plugin STATIC` + `Q_IMPORT_PLUGIN`).** Compiled into the binary:
   nothing to deploy, nothing for a from-source build to get wrong. Oliver's constraint —
   users compile QMapShack themselves and packaging spans platforms, so a dynamic plugin would
   re-create the 3.3 `qsvgicon.dll` trap (not deployed → no crash → icons silently wrong).
   **Verified with nothing on the plugin path and no `iconengines` dir.**
2. **Key `svgt`, NOT `svg`.** Qt's own `qsvgicon` claims `svg` and **beats a static plugin to
   it** — with key `svg` our engine's `create()` is *never called* (verified: instrumented it,
   it never fires; the trace shows `libqsvgicon.so` loading instead). Inference from the
   observed behaviour: dynamic plugins are indexed before static and first match wins. Worse
   than losing consistently — on a Qt install *without* `qsvgicon` ours would win, so the same
   code would behave differently per machine. A distinct key makes it deterministic.
3. **`.qrc` alias, disk keeps `.svg`.** Inkscape, `hygiene.py`, `mkicon`, `uidiff.py` and every
   `*.svg` glob keep working untouched. Only the resource path carries the suffix.
4. **Root `color="#000080"` default = graceful degradation.** Inkscape, browsers and Qt's own
   `qsvgicon` all render it navy — i.e. **exactly today's appearance**. If the engine never runs,
   nothing looks broken. This is what makes the whole approach safe to ship.

**Semantic colour is free.** A fill that is not `currentColor` is untouched by the theme
(verified: theme=green, a `#ff0000` fill still renders `#ff0000`). No hue carve-out, no
per-colour rule, no red-becomes-cyan bug. Only what we *mark* as ink moves.

**No `.ui` edits are needed for the mechanism** — one `QIcon`, built once by `uic`'s `addFile()`
in `setupUi()`, follows the colour live with no rebuild and no widget walk (verified). The
references do change `.svg` → `.svgt`, which is mechanical and is the same sweep Phase 5 already
did once for PNG→SVG. A missed reference gives a **blank** icon: visible and greppable.

**Decisions (Oliver):** suffix `.svgt`. Originally "alias only, no dual registration"; **revised
to blanket dual registration** during implementation (see below) once qmaptool and shared code
turned out to need `:/icons/Foo.svg` to keep resolving.

#### 7a — IMPLEMENTED: engine + resource cutover

The engine and the `.svg`→`.svgt` cutover are in the tree (qmapshack only).

- **Engine:** `src/qmapshack/svgticon/` (`CSvgtIconEngine` + `CSvgtIconEnginePlugin`, key `svgt`),
  a `qt_add_plugin STATIC` linked into the executable and imported with `Q_IMPORT_PLUGIN` in
  `main.cpp`. It rewrites the **`.ink`/`.paper` class colours** in the `<style id="current-color-scheme">`
  block (NOT a root `color=` -- a class beats a root attribute in Qt and inkscape, so the root
  model themed nothing; measured). Two roles because a navy body and a light label move in
  opposite directions on dark. Render cached in `QPixmapCache` keyed on file+size+scale+**both
  colours**. Non-currentColor fills are untouched (semantic red/green survive). Verified on Qt
  6.10.1 with a real themed source (551 px follow the palette).
  - **`QMS_ICON_INK` — RESOLVED, switch deleted.** The A/B was `palette` (roles from the platform
    theme) vs `fixed` (a hand-picked light/dark pair). **`fixed` won**; the palette arm and the env
    switch are gone. Reason: no palette role carries navy, so `.ink` had to land on
    **`QPalette::Link`** — `#0000ff` on a stock light theme and an arbitrary theme-defined blue
    elsewhere, i.e. the brand colour became whatever the desktop calls a hyperlink. Note this line
    (and the class doc) previously said `.ink`=WindowText; that was **wrong**, the code always used
    `Link`, and the two docs agreeing with each other is how it went unnoticed. The shipped scheme
    is the authored palette on light (`#000080`/`#ffffff`/`#000000`/`#0000ff`) and
    `#ccccff`/`#353535`/`#e0e0e0`/`#66aaff` on dark.
- **Cutover:** 1074 flat `:/icons/*.svg` refs → `.svgt` across 174 `.cpp`/`.ui` files, plus the
  one macro miss `CActivityTrk.cpp` (`"://icons/" icon ".svg"`, adjacent-literal, invisible to a
  path regex). Authoritative coverage was taken from the **branch diff** vs the merge-base, not a
  regex — that is how the macro miss was found.
- ~~**Blanket dual registration**~~ — **DONE, duplicates dropped.** Every flat icon used to be in
  `resources.qrc` twice (plain `.svg` for qmaptool and shared code, `.svgt` for the engine) because
  qmaptool had no engine. The engine now lives in `src/common/svgticon`, is built once and linked by
  both apps, so `common/help/CHelpBrowser.cpp` uses `.svgt` and the 309 plain copies are gone.
  The blank-icon safety net went with them: a missed ref no longer falls back to an unthemed icon,
  it renders empty. Checked by linking each app's compiled qrc object against the engine and
  resolving every registered icon — qmaptool 43/43, qmapshack 309/309, all non-blank.
- ~~**qmaptool left on `.svg`**~~ — **DONE.** qmaptool links the shared engine, its 43 icons are
  registered as `.svgt` and its 135 refs cut over. **`waypoints/*.svg` (FlagBlue/PinBlue) still on
  `.svg`** — that pair needs the black-ink markup (no default root `color=`) and stays deferred to
  the waypoint-chrome step.
- **`migrateIconPath()` neutralised for the duration:** it now maps any history-event
  icons/<name>.{png,svg,svgt} to the always-present 48x48 **PNG** rather than migrating into the
  in-flux themed form. Stable/master has no `migrateIconPath`, so no released build ever wrote a
  `.svg`/`.svgt` history path — every real file stores 48x48 PNG. This keeps the dev branch from
  baking a `.svgt` path (unreadable elsewhere) into saved files. **Revisit once the on-disk form
  is final.**
- **Markup — first batch applied.** `themesvg.py --write` marked up **279** sources with
  `currentColor` + `.ink`/`.paper` classes; **21 opt-out** (`palette.py:OPTOUT`, imported so it is
  one source of truth); 30 nothing to theme. Light look unchanged (`--verify`: 300 identical / 0
  CHANGED on the originals). `theme()` was rewritten to **text-patch by fill colour** (no
  `ET.tostring` reserialise): diff is 3020+/3020− across 279 files, 1:1 line swaps, Inkscape
  namespaces/formatting intact and round-trip-safe (loaded+saved in inkscape, currentColor +
  classes + style block survive). So icons now follow the theme in dark mode.
  - **Still deferred (iterate next):** the `keep` role (semantic sign-white in `NoGo`/`NotPossible`
    currently tagged `paper`, so it wrongly flips on dark); **two-colour strokes** (~1030 elements
    with a themed fill and a differently-coloured stroke -- only same-colour strokes follow today);
    and the **accent normalisation** (`palette.py` reds/greens, the step that moves the light look).

#### 7a — traps, all measured

- **Never insert a second `color=`.** A duplicate attribute makes the document **INVALID** and
  `QSvgRenderer` draws **nothing**. Since the sources carry a default `color=`, the engine must
  **replace** the root attribute. The naive `data.insert(i+4, ...)` in an early draft is exactly
  this bug. See `recolored()` in the proto.
- **`currentColor` with no `color` set renders BLACK** — in Qt *and* in inkscape (both measured).
  So a themed source without its default `color=` silently exports **black PNGs** from `mkicon`,
  turning the navy `<pixmap>`/`QPixmap` rasters (which Phase 5 deliberately kept) black. The root
  `color="#000080"` is not decoration; it is what keeps the PNG pipeline correct.
- **Lowercase `currentcolor` silently renders black** — Qt's match is case-sensitive
  ([QTBUG-46947](https://bugreports.qt.io/browse/QTBUG-46947), open since 2015). Legal CSS, and
  some tools emit it. Worth linting our own sources for.
- **A `<style>` class beats a root `color=` attribute** (verified). So the KDE/Breeze
  style-block idiom and the simple root-attribute idiom **cannot be mixed carelessly** — if a
  source has `.ColorScheme-Text{color:…}`, injecting root `color=` does nothing. We use the root
  attribute; do not import Breeze-style icons without converting them.
- **Qt 6.10.0 has a `currentColor` regression**, fixed in 6.10.1
  ([QTBUG-141102](https://bugreports.qt.io/browse/QTBUG-141102)). Not our Qt (6.9.2), but it will
  reach users.
- **The engine re-parses the svg on every paint.** The proto has no cache; a real one needs it
  keyed on **name + size + mode + state + ink colour** so a theme flip is a natural miss. This is
  exactly what the "no bare `static`" note in CLAUDE.md already prescribes — a bare `static`
  pins the scheme live at first paint and defeats the whole task.
- Repaint on theme change still needs a trigger (`QEvent::PaletteChange` → `update()`).
  Unverified whether Qt's own palette propagation already covers it in practice.

#### 7a — waypoint chrome icons are IN scope (Oliver)

`FlagBlue.svg` / `PinBlue.svg` are the only 2 of the 203 waypoint svgs in `resources.qrc`, used at
4 UI sites (`CGisListWks.cpp:211`, `IScrOptRuler.ui:41`, `IScrOptSelect.ui:96,245`). **They should
be themed** — as UI chrome they are under none of the canvas freeze (the canvas draws the *PNG*).

**Safe, and measured:** both are black ink (`#000000` fill+stroke) plus a blue body (`#0000ff`).
Convert the **black to `currentColor` and leave the blue** — the blue is the symbol's identity and
reads on both grounds. Because `currentColor` with no `color` set renders **identically to
`#000000` in inkscape** (verified byte-for-byte), `waypoints/makeicons` still exports the same
32px PNGs, so the frozen canvas contract does not move. **Gate it anyway** with
`./wptdiff.py --size 96` — must stay 123/61/19/0.

Note these two want **no** default root `color=` (their ink is black, and black is what the PNG
export must keep) — unlike the UI icons, whose default must be `#000080`.

#### 7a — Oliver's visual review (26 of 329 flagged) — the five patterns

He reviewed the whole set on `#ccccff` ink and flagged 26. They collapse into five patterns,
each with a measured cause. **This review is the most valuable artifact of the phase — three of
the five could not have been found by analysis.**

| # | pattern | icons | measured cause |
|---|---|---|---|
| 1 | semantic colours too dark on dark | 13 | `#ff0000` luma **54** vs ground `#353535` luma **53** — a *luminance match*, visible by hue only. And "green" is **six** colours: `#008000`(77) `#55d400`(72) `#00ff00`(39) `#57e83c` `#668000` `#445500` |
| 2 | white is genuinely semantic | `NoGo`, `NotPossible`, `Off` | real white that is part of a sign — must never flip. `NoGo`/`NotPossible` need the `keep` role; `Off` is already covered by its opt-out |
| 3 | ~~"white" is actually a hole~~ | — | **DISSOLVED, see below. No members.** |
| 4 | self-backgrounded icons must opt out | `MimeDemWCS` + family | bg `#326480` (unmapped, stays) + `#ffffff` content (flips to black) = **black on mid-blue**. Content sits on the icon's OWN background, not the app's |
| 5 | `#0000ff` is ink, not accent | `ReloadImage`, `WptProx`, `WptEditProx` | luma **18**; 74 fills + 56 strokes, unmapped, stays near-black |

Opt-outs he named: `WptAvoid` (fine unconverted), `Screenshot` (eye stays), `SearchGeonames` (fine).

**Pattern 3 was my error — both members re-diagnosed, and the same blind spot caused it twice.**
I counted **fills only**, as I had when I first censused the palette and missed 865 `stroke:#ffffff`
/ 609 `stroke:#000080` / 378 `stroke:#000000`. **Always census fill AND stroke.**

- **`Off`** — its power symbol is `stroke:#ffffff` ×2, a *real white stroke*, not a hole. It is
  pattern 2 (semantic white), and its **opt-out already fixes it**: no recolour, white stays white.
- **`CSrcCourse`** — genuinely has **zero white** (fill or stroke); its "white" really is the page.
  But that is **not** what broke it. It carries 8× `#000080` and 4× **`#000081`** — luma delta
  **0.072**, physically invisible on light, and the only file in the tree with that colour. Only
  `#000080` is neutral-mapped, so on dark 8 facets went light and 4 stayed navy: the compass fell
  apart. **A typo that only theming could expose.** `palette.py`'s `TYPOS` fixes it.

So no icon needs a paper fill added, and the "art assumes a light page" theory is dead.

**`bgaudit.py`'s `opaque-bg`/`edge-bg` list is the pattern-4 opt-out set.** The tool that looked
like a dead end (it disproved the white-background theory) turns out to name exactly the icons
that must not be themed.

#### 7a — DECIDED: markup, not substitution

Byte substitution of the neutral palette was tried and **is dead**. Three independent proofs, all
measured:

1. **It cannot see an implicit fill.** 40 shapes rely on SVG's black default (below) — there is
   no colour token to replace.
2. **It cannot tell semantic white from paper white** (`NoGo`'s sign vs `Save`'s label).
3. **It cannot know an icon carries its own background** (`MimeDemWCS`).

All three need **per-icon intent in the art**. Role vocabulary (to build):
`ink` / `paper` (themed, inverted) · `keep` (never touched — sign white, Mime backgrounds,
`Screenshot`'s eye) · plus whole-icon opt-out for pattern 4.

#### 7a — DECIDED: normalise the accent palette at source (Oliver)

Rather than teach the engine a per-scheme accent table, **fix the source palette once**. Measured,
a single value per hue clears delta>40 against **both** grounds (`#efefef` and `#353535`):

| hue | today | → | works on both |
|---|---|---|---|
| red | `#ff0000` (164 uses) — dark-delta **1** | → | `#ff5555` (light 118 / dark 68) |
| green | six values, `#008000` dark-delta **39** | → | `#55d400` (light 69 / dark 117) — already used 72× |
| blue | `#0000ff` (74) — dark-delta **35** | → | `#5555ff` (light 142 / dark 44) |

**This deliberately changes the LIGHT appearance** (`#ff0000` → `#ff5555` is visibly lighter), so
the "light render never moves" gate does **not** apply to this step. Treat it like phase 2.4:
front-load the visual risk into one reviewable step. **Not blanket-mappable** — `#800000`,
`#550000`, `#445500`, `#668000` are shading/outline variants, not the primary accent. Decide per
role, review on the sheet.

#### 7a — source fixes (defects under every option; do first)

| # | fix | state |
|---|---|---|
| a | write down the 40 implied black fills | **DONE, in the working tree** — 29 files, 40 lines; `uidiff` PASS, `--verify` 0 CHANGED |
| b | ~~add an explicit paper fill to hole-interior icons~~ | **DROPPED** — pattern 3 dissolved; no icon needs it |
| c | mark the self-backgrounded icons as whole-icon opt-out | **DONE** — `palette.py:OPTOUT`, **21** icons, derived from `bgaudit` by keeping every non-neutral background colour |

**The opt-out set is 21, not ~67.** `bgaudit`'s 53 `opaque-bg` + 14 `edge-bg` is the wrong list:
most of those are navy — solid navy *objects* (`Save.svg` is a navy floppy) or navy frames
(`Image`, `Map`, `Poi`), which must be themed. Only the icons whose background is a **non-neutral
identity colour** opt out: the `Mime*` / `*Project` family plus `Off` and `ToWksUnchecked`.

**Trap hit for real in (a): never parse-and-reserialise an icon to change it.** `ET.tostring()`
rewrote all 29 files wholesale — dropped the xml declaration and Inkscape's namespaces, reflowed
every attribute — turning a 40-attribute fix into **582+/2089−**, unreviewable, and probably
harmful to future Inkscape editing. The render was identical and `uidiff` PASSed, so **the gate
did not catch it**: a passing gate is not a good diff. Fixed by analysing with the parser (fill
inheritance needs the tree) but **patching the original text by element id** — same fix,
**40+/40−**. See `add_fill_by_id()`. All 40 targets have ids; do not assume that of future ones.

#### 7a — DONE: two-colour strokes (the "still navy on dark" report)

Oliver reported navy lines/areas surviving on dark and guessed the cause was colours *near*
`#000080` that the exact-match map misses. **Measured, that theory is real but tiny** — 10 uses in
6 icons. `colorstat.py` (new) does the census -> clustering -> deviation -> worklist.

The actual cause was **strokes**. All 166 remaining literal `#000080` were `stroke`, in 74 icons,
**zero fills**: the fill batch themed fills, and `retag_tag` only followed a stroke when it was the
*same* colour as the fill, so `fill:#ffffff;stroke:#000080` kept a literal navy outline.

**Mechanism** (`strokecheck.cpp`, and it is not what the old note said): the class serves the fill
directly and its `color` feeds an inline stroke.

```
.paper-ink{fill:#ffffff;color:#000080}   <path class="paper-ink" style="stroke:currentColor"/>
```

The class name is a `-`-separated list of role tokens, **one per declaration, in declaration
order**, so the engine assigns roles positionally without parsing colours. `CSvgtIconEngine`
rewrites declaration-by-declaration now; its old regex assumed `color` was the only declaration
and would have silently dropped a `fill`.

**The old note's asymmetry claim did not reproduce.** It records "Qt honours `fill` from a class and
silently IGNORES `stroke` from one" (6.9.2). On 6.10.1 a class stroke *is* honoured, and qtsvg's
`cssStyleLookup()` -> `parseCSStoXMLAttrs()` merges CSS into the attribute set for `fill`, `stroke`
and `color` alike **on the 6.8 branch** — our minimum. So it is most likely a measurement artifact,
not a version difference. Design unchanged: strokes stay inline, which works either way and costs
nothing. That is also why class-`fill` is safe at 6.8 though only 6.10.1 was installed to measure.

**Two traps hit, both worth keeping:**

1. **The transform was not re-runnable.** `theme()` dropped the scheme block but left last pass's
   `class="ink" fill="currentColor"` behind — and currentColor with no colour set renders BLACK.
   The fill batch and this one touch the same icons, so this was live, not hypothetical. Fixed with
   an explicit `untheme()` pre-pass that restores the literal colours; `theme()` is now idempotent.
   Proof it is lossless: new transform on **today's** tree gives 283 themed / 26 untouched —
   identical to new transform on the **pre-markup** tree.
2. **A passing gate is still not a good diff** (the plan's own warning, hit again). The first write
   was 2740+/4898− because removing a class and re-adding it appends at the tag end and rstrips the
   newline before it, reflowing every touched element. Fixed by blanking the attribute in place and
   leaving a `SCHEME_MARK` where the block was. Same edit, reviewable diff.

**Regex vs parsing the tree** (Oliver asked): the split is deliberate and both halves are measured.
*Editing* stays textual — `ET.tostring()` once turned a 40-attribute fix into 582+/2089−. *Analysis*
that needs inheritance uses a parser. The one thing a per-tag edit cannot see is an inherited
`fill:currentColor` resolving against the wrong element's `color`; measured **0 cases** (154 shapes
inherit a fill, none with a class of their own). That is a property of these icons, not of SVG, so
it is now asserted by `inheritance_conflicts()` inside `--verify` rather than assumed — the next
role added is what would break it.

**Result** — `darkcheck.cpp` renders every icon through the real engine at the dark pair and counts
navy pixels: **74 icons -> 15**. All 15 residuals are already-known next steps, none are strokes:

| cause | icons |
|---|---|
| `#000081` / `#000099` near-miss | `CSrcCourse`, `ToggleDocks`, `ToggleRouter` |
| `#0000ff` pure blue, unthemed (luma 18) | the other 12 |

Gates after the pass: `--verify` **283 identical / 0 CHANGED**; `uidiff` **329/329 identical, 0
visible**. `uidiff`'s verdict is still FAIL on one **pre-existing** orphan — `RatingStarHalf.svg`
has no `32x32` PNG, absent at `HEAD` too. Unrelated to this pass; needs a decision of its own.

**Note the build rewrites your tree.** `svghygiene` reserialises every edited icon at build time, so
the 283 written files collapse to **97** genuinely-changed ones (the rest differed only by the
`<style>` block being one line vs Inkscape's two). Re-run both gates *after* building, not before.

#### 7a — DONE: QSvgWidget icons never reached the engine

Oliver: "in the setup range tool dialog icons are still unchanged for dark theme, probably a
leftover from the png->svg peril". Correct, and it was systematic rather than one dialog.

**`QSvgWidget` draws through `QSvgRenderer` and never constructs a `QIcon`** — so
`CSvgtIconEngine`, being an *icon* engine, never ran for it. Every static explanatory icon in the
setup/filter dialogs is a `QSvgWidget` (commit 43bc83ec adopted them for HiDPI), so they all kept
their authored navy while the toolbars around them followed the theme. **57 call sites in 39
files.** This is the same class of gap as the `<pixmap>` and `QPixmap` sites already documented —
the rule is "anything that does not go through `QIcon` does not get themed", and `QSvgWidget` was
the member of that set nobody had listed.

Fixed with `CSvgtIcon::load(widget, path)` (`svgticon/CSvgtIcon.{h,cpp}`): recolours via the
engine's own `recolored()`/`roleColor()`, then keeps following the theme through a shared,
stateless event filter that re-themes on `ApplicationPaletteChange`. The source path lives in a
widget property — re-theming needs the ORIGINAL document, and recolouring already-recoloured bytes
would drift. Chosen over a promoted `CSvgtWidget` subclass because the call sites were being edited
anyway and 42 `.ui` files stay untouched.

`widgetcheck.cpp` drives the real widget: 0 navy on dark, follows a live switch back to light.

**Two traps, both worth keeping:**

1. **`.svgt` is a `.qrc` alias, not a file.** It resolves under `:/icons`, and there is no
   `src/icons/Foo.svgt` on disk. The check was first pointed at one and **passed vacuously** — a
   widget that failed to load draws nothing, and nothing contains no navy. `widgetcheck` now
   asserts the icon rendered at all before asserting anything about its colour.
2. Same silent failure existed in the helper, so a bad path now `qWarning()`s instead of leaving a
   blank widget that reads as "no icon here". Both verified by fault injection.

**Still unthemed: qmaptool's 2 sites** (`CToolGrid.cpp`, `CAbout.cpp`). They need the engine shared
first — that is step 5 below, and this helper should move with it rather than be duplicated.

#### 7a — DONE: black is a third role (`lead`)

142 icons draw with both navy and black, so sharing one role collapses them on dark and the
drawing loses its detail. Reviewed on `blackab.py`'s sheet; separate won.

Roles are now `ink` (brand) / `paper` (ground) / `lead` (neutral detail). In palette mode `lead`
takes `WindowText` and `ink` takes `Link` — mapping both to `WindowText` would re-collapse them.
Fixed mode: lead `#e0e0e0` on dark, `#000000` on light. `roleColor()` takes a `Role` enum now, and
the pixmap cache key includes all three colours.

Also normalised two navy typos (`#000081` CSrcCourse, `#000099` ToggleDocks/ToggleRouter) via
`palette.py --typos-only`. Both are invisible on light and split the icon on dark.

Gates: `--verify` 304 identical / 0 CHANGED; `uidiff` 329/329, 0 visible. Navy on dark: 15 -> **6**.

**`#0000ff` is NOT a typo and is still open.** 74 uses in 31 icons, and 23 of them also use navy —
a second, brighter blue. Merging it into `ink` collapses a distinction in those 23, the same trap
black had. It needs its own A/B before any markup.

#### 7a — DONE: `#0000ff` is a fourth role (`mark`), with a keep-list

74 uses in 31 icons, 23 of them alongside navy — a second, brighter blue, so folding it into
`ink` would collapse that distinction. It became `mark` (it marks the element being acted on):
palette mode `Highlight`, fixed mode `#66aaff` dark / `#0000ff` light.

**Not everywhere, though.** In some icons the blue is one swatch of a palette or a multi-colour
bar — there it is semantic like red and green and must stay literal. Detection was tried and
dropped: hue count does not separate a colour bar from a red/green/blue marker set, and what
actually distinguishes them is geometry. So it is a confirmed per-icon list, `palette.KEEP_BLUE`
= `PasteNormal`, `ToolBar`, `ToolBarSetup` (28 of 31 themed). `roleab.py` sorts by hue count to
make that review quick, but the call is by eye.

Gates: `--verify` 304 identical / 0 CHANGED; `uidiff` 329/329, 0 visible.
**Navy on dark: 0 icons** (from 74 at the start of this work).

#### 7a — DONE: MimeIMG was missing from OPTOUT

Reported as "MimeIMG in dark mode lacks the white background, looks different from the others".
Not a source-art defect — a hole in how OPTOUT was derived.

Its tile is `#000080`. The rule was "keep every non-neutral background colour", and a navy
background reads as neutral, so it dropped out while its 9 siblings stayed. Themed, its tile went
light and its white content went dark — inverted from the rest of the family.

**The family is identifiable by geometry, not colour:** every Mime tile is `opaque=99.7%
ring=95.2%` in `bgaudit`. The navy SOLIDS at `ring=98.4%` (`Export`, `QMapShack`, `ReferenceMap`)
are objects, not tiles, and must stay themed. `MimePoiPOI`/`MimePoiPOIActive` are navy *frames*
(47.7%/57.3% opaque) and also stay themed, per the earlier decision.

**OPTOUT was not retroactive.** `themesvg.py` skipped those icons, so adding a name to the list
left an already-themed icon themed forever. It now un-themes them, which is what makes the list
authoritative. `bare()` is `untheme()` plus the cleanup `theme()` normally does.

#### 7a — RESUME HERE (state)

**Committed on `icon_mania`** (see "IMPLEMENTED" above for detail): the `.svgt` engine + plugin,
the resource cutover (blanket dual reg), `migrateIconPath` neutralised to PNG, and the **first
markup batch** (279 icons, ink/paper + opt-out; `theme()` text-patched). The branch is testable
in the real app now: build, then `QMS_ICON_INK=palette|fixed` + a dark desktop.

**Next, in order:**
1. ~~**A/B the ink mapping in real life**~~ — **DONE.** `fixed` won, the palette arm and
   `QMS_ICON_INK` are deleted. Both arms reviewed by Oliver in light and dark on the real apps.
2. **`keep` role** for semantic sign-white (`NoGo`, `NotPossible`) so it does not flip on dark.
3. ~~**Two-colour strokes**~~ — **DONE**, in the working tree. See "DONE: two-colour strokes"
   above. 97 icons, `.paper-ink`-style classes, both gates green, navy icons 74 -> 15.
   **Next up is the residue it exposed, which Oliver asked to keep as its own reviewable step:**
   black (475 uses, ~170 icons) and `#0000ff` (74 uses, 31 icons) are dark and **not in `INK`** —
   together 916 dark unthemed uses across 241 of 330 icons. Decide whether black joins `ink` or
   gets its own role before writing markup; `--explicit` already made the 40 implied blacks
   reachable, so this is the pass that pays that off. The 3 near-miss icons (`#000081`,
   `#000099`) go with it — `palette.py:TYPOS` covers `#000081` only.
4. **Accent normalisation** — still **NOT applied** (`palette.py` dry-run only). The open judgement
   is **how far the light look may move**: 103 icons move, 19 by >=12; biggest `Error`(32)
   `NoGo`(30) `NotPossible`(27) `RouteOn`(22) `AddRte`(20), all `#ff0000`->`#ff5555`. Decide the
   reds before `palette.py --write`; it changes the light render, so re-baseline rather than gate.
5. **Share the engine with qmaptool**, then drop the plain-`.svg` dual-reg copies; do the 2
   `waypoints/*.svg` (black ink, no default `color=`); revisit `migrateIconPath` for the final form.
   **Move `CSvgtIcon` with it** — qmaptool's 2 `QSvgWidget` sites are still unthemed for the same
   reason, and the helper should be shared rather than copied.
6. **Audit for other non-`QIcon` paths.** `QSvgWidget` was an unlisted member of the "does not go
   through `QIcon`, so is never themed" set, alongside the known `<pixmap>` and `QPixmap` sites.
   Worth one sweep for anything else that renders an icon resource directly.

Review page (regenerate + republish to the same URL after any change):

```bash
./.notes/icons-tools/contactsheet.py --dir src/icons --ink '#ccccff' --out /tmp/iconsheet.html
```

Published at `https://claude.ai/code/artifact/a23f8d86-0a34-4c07-a3a4-df915458ce2d` — the page is
live and persists; it never needs "restoring", only reloading. To **update** it from a new
conversation you must pass that URL as the Artifact tool's `url`, or a new one is minted and he
loses the tab he has open. (`action: "list"` finds it if the URL is ever lost.)

His flags live in *his* browser's localStorage keyed by icon name. They **survive a republish** —
do not treat a regenerate as a reset — and **they never reach us except by him pasting the
report.** His 26 from the first review are still set, and several are now fixed (`CSrcCourse`,
`MimeDemWCS`, `Off`, `ReloadImage`/`WptProx`/`WptEditProx`): the intended workflow is "Flagged
first" -> confirm each -> unflag, so what remains is the real worklist. Do not suggest clearing
them without saying what is lost.

**Gates — run both after any source change:**

```bash
./.notes/icons-tools/uidiff.py                      # PASS, 0 visible  (light render unmoved)
./.notes/icons-tools/themesvg.py --dir src/icons --verify   # 299 identical | 0 CHANGED
```

`uidiff` does **not** apply to the accent normalisation — that step changes the light render on
purpose. Re-baseline it there (regenerate the PNGs) rather than treating a failure as a bug.

#### 7a — open

- **Source fixes (b) and (c)** above.
- **The accent normalisation**, per the decision above — then re-review on the sheet, since the
  risk moves to the *light* side. `contactsheet.py` will need a light-today vs light-after
  column to make that judgeable.
- **The markup migration itself**: assign roles across ~299 icons. `themesvg.py` does ink/paper
  today and is gated (299 identical / 0 CHANGED); it needs `keep` + whole-icon opt-out, and it
  must learn the **two-colour element** (1030 of them in 118 files: a themed fill AND a
  differently-themed stroke). Expressible — verified in Qt *and* inkscape — as **`fill` from a
  class + `color` from a class + inline `stroke="currentColor"`. Qt honours `fill` from a class
  and silently IGNORES `stroke` from one** (inkscape honours both); that asymmetry is
  undocumented, so keep strokes inline.
- **What ink colour does a theme supply?** Palette role (`QPalette::WindowText`?) vs a fixed pair.
  Undecided. Ink `#ccccff` (hue 240, L 90%) is the current candidate — Oliver reviewed on it.
- Static-plugin ordering (trap 2) is an **inference** from observed behaviour, not read out of
  Qt's source. It does not change the design — a distinct key is right regardless — but do not
  quote the mechanism as fact.
- `Area`/`AreaOn` put `fill:black` (a **named** colour) inside a `<pattern>`; the pattern icons are
  also on CLAUDE.md's "unverified in Qt" list. Both need handling; neither is covered yet.
- 44 icons carry off-palette darks the neutral map misses — `#000081` (**navy off by one**),
  `#0000b2`, `#0000ff`, `#800080`. Normalise with the accent pass.

## Rejected / superseded

- **"Build an icon cache before switching to SVG."** Rejected — measured wrong. Qt's `QIcon`
  already caches rasterization (1.13 µs cached SVG vs 0.81 µs cached PNG). The real issue is
  per-paint *construction*, fixed by `static`. The original justification cited `CIconGrid`
  (the **waypoint picker**, Task 2) and `CMapIMG` (whose icons come from the **TYP file**, not
  resources — only the `bullet_blue` cluttered-POI fallback is a resource, and it is canvas data
  that never becomes SVG under Task 1). Both examples were Task 2.
- **"Fixing the 41 scattered resize calls is a Task 1 benefit."** Rejected. The count is **42**
  (`grep -rE '\.scaled\(|\.scaledToWidth\(|\.scaledToHeight\('` over `src/` — note plain
  `.scaled(` alone is 38; the other 4 are `scaledTo*`). The overwhelming majority operate on
  waypoint/canvas/item pixmaps, not UI chrome — e.g. `CWptIconManager.cpp:2116`,
  `CGisItemWpt`, the POI icons, `widgets/CIconGrid.cpp:85`, and the 5 `item.getIcon().scaled(...)` sites
  in `CWksItemDelegate`. The clearest UI-chrome cluster is the 10 geocache dialog rating stars
  (`CDetailsGeoCache.cpp:63-103`), which are set once on QLabels and are not a paint path at all.

  **Caveat on the split:** a per-site 11/23/8 classification was produced by a subagent and is
  **not independently verified** — do not quote it. The load-bearing point stands on its own: this
  cleanup is overwhelmingly not UI chrome, so it is not a Task 1 benefit. Task 2's plan does not
  currently claim it either; if you want it owned, put it there explicitly.
- ~~**"SVG gives you dark mode."** Rejected — Qt has no recolour API.~~ **Half right, and the
  wrong half stuck.** There is no recolour *API* — but Qt's renderer resolves `currentColor`
  (measured, `ccheck.cpp`), so SVG *does* give you dark mode once the sources are marked up and a
  static icon engine sets the root `color=`. The original claim was read as "SVG cannot do this",
  which is false and would have sunk phase 7a. See Phase 7 above.

---

## Persisted icons — one deferred decision, three sites

Everywhere an icon path or an icon's PIXELS get written to disk, the themable form has been
kept OUT of the saved data on purpose: a ".svgt" path means nothing to a build without the
icon engine, and a rendered icon freezes whatever theme was active. All three are parked
until the icon rework settles and the final on-disk form is decided. Decide them together —
they are the same question asked in three places.

1. **Web search services** — `CGeoSearchWeb::displayIconPath()`. The settings keep naming
   `icons/32x32/SearchWebX.png`; the vector is resolved at DRAW time only, so a tester moving
   back to stable still has a working menu. `defaultIcon`/`defaultServices()` deliberately
   still name the PNG so nothing ever persists a ".svgt".

2. **History events** — `IGisItem::migrateIconPath()`. Maps any `icons/<name>.{png,svg,svgt}`
   to the always-present 48x48 PNG rather than migrating into the in-flux themed form. A
   stray SVG path from an earlier test build heals to PNG on the next save.

3. **GIS items in the database** — `CDBProject.cpp` writes `item->getDisplayIcon()` as PNG
   BYTES (`:294`, `:338`, `:389`), read back by `CDBItem.cpp:35`. This one is not a path: the
   icon is rasterised at save time, so it cannot follow the theme at all, and a database
   written in dark mode carries dark icons into a light session. The other two can be settled
   by choosing a path format; this one needs a decision about whether the database stores a
   rendering or a reference.

## Open questions

- **`DeviceSync.svg` and `AutoSaveA.svg`** differ 35.5% / 23.2% post-hygiene with **no known
  cause** (no text, flowRoot, marker, gradient, pattern or embedded raster). The only two
  stragglers not explained by the text/font story. Start here.
- The 17 text-bearing stragglers: decide by eye whether the outlined SVG or the old PNG is the
  one you want (see the straggler table — the PNG may be the stale one).
- Do the 3 `<pattern>` and 3 embedded-raster icons survive Qt? (Not separately measured; they
  were not in the worst list.)
- `src/icons/qmapshack/hicolor` ships 17 rasterized sizes and **no `scalable/apps/*.svg`**,
  despite the source SVG being right there. Free win, unrelated to the rest.

---

## For the maintainer discussion (raise before release)

Collected as the work proceeds, so none of it is rediscovered at review time.

1. ~~**Windows must ship `plugins/iconengines/qsvgicon.dll`.**~~ **SETTLED — Oliver confirmed the
   live Windows packaging deploys it.** Kept here only so it is not re-opened: without that plugin
   `QIcon(":/x.svg")` falls back to the qsvg *image* plugin, rasterizes once at the SVG's natural
   size and scales, so icons go **blurry, not blank** — nobody would have reported it and the
   HiDPI benefit would have silently disappeared on Windows only.
2. **17 icons changed appearance** in the pipeline repair. All subtle except `FilterSplitTrack`,
   which is a reconstruction (its source had been lost). Before/after is reproducible with
   `./uidiff.py`.
3. **`makeicons` behaviour changed**: the 16px rule is anchored to `^Act[A-Z]`, and three dead
   16px files it used to emit are gone (`Activity`, `NoActivity`, `MimePoiPOIActive` — none were
   ever in a qrc; the first two had been committed for years).
4. **`SelectNextPoint.png` deleted** (32 and 48): zero references, in no qrc, never compiled in.
5. **The UI icon set is deliberately mixed, and should stay that way.** Chrome reached via
   `QIcon` is SVG. Two groups stay raster on purpose: colour swatches (`8x8/` bullets, `lines/`)
   because theming them would make them misreport the colour they stand for; and the waypoint
   symbol because task 2 freezes it (canvas appearance). The 42 GIS-item `QPixmap` sites are
   **not** a raster-by-design group — their icon is derived from sym/colour, and tracks/areas can
   be SVG; Task 2 owns converting them (cat. 2 above, and the Task 2 plan). The one real constraint
   is that `QIcon` will not upscale a raster, so the fix renders the SVG at size rather than
   wrapping the old PNG.
6. **Dark mode needs a palette decision and an artist** — Qt has no SVG recolour API, so it is a
   substitution layer we write, and it is gated on the redesign. See Phase 7.
