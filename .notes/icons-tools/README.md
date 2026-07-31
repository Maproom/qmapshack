# Icon analysis throwaways

Ad-hoc measurement tools written for the icon analysis. **Not part of the build.** Kept so the
measurements can be re-run and the conclusions re-checked rather than taken on trust.

**Moved out:** `reframe.py`, `palette.py` and `themesvg.py` are now called by `svghygiene` on every
build, and `colorstat.py`, `roleab.py` and `contactsheet.py` import or accompany them — all six live
in `src/icons/tools/`. The build cannot depend on a notes folder. Rows for them below describe what
they do, not where they are.

| tool | backs | task |
|---|---|---|
| `hygiene.py` | **does** the phase-2 repair: strips empty flowRoot, outlines text, fits the canvas | 1 |
| `uidiff.py` | **gates** it: 259/15/17/36 baseline -> 277/15/18/17 hygiened -> all-identical regenerated | 1 |
| `bgaudit.py` | "only 1 of 329 icons has a white background" — kills the baked-background theory; also **derives the 21-icon opt-out list** | 1 (ph. 7a) |
| `themesvg.py` | the markup transform + `--explicit` (the black-arrowhead fix) + `--verify` gate | 1 (ph. 7a) |
| `palette.py` | accent normalisation + the `#000081` typo + the opt-out set | 1 (ph. 7a) |
| `colorstat.py` | colour census -> clustering -> deviation; killed the "near-miss navy" theory (10 uses / 6 icons) and found the real cause | 1 (ph. 7a) |
| `strokecheck.cpp` | the two-colour mechanism: class `fill` + class `color` + inline `stroke="currentColor"` | 1 (ph. 7a) |
| `darkcheck.cpp` | end-to-end through the real engine: navy icons **74 -> 15** after the stroke pass | 1 (ph. 7a) |
| `roleab.py` | the role A/B sheet: merge a candidate colour into `ink`, or give it its own | 1 (ph. 7a) |
| `widgetcheck.cpp` | `QSvgWidget` icons bypass the icon engine entirely; proves `CSvgtIcon::load` fixes it and follows a live palette switch | 1 (ph. 7a) |
| `contactsheet.py` | builds the review page Oliver actually looks at | 1 (ph. 7a) |
| `wptdiff.py` | "0/203 visible differences" at 96px — the "must not change visually" constraint | 2 |
| `svgdiff.cpp` | 151 severe → 19 → 1; the ink-box finding that exposed the root cause | 1 |
| `ccheck.cpp` | **Qt DOES resolve `currentColor`** — the 9 renderer facts phase 7a rests on | 1 (ph. 7a) |
| `svgt-proto/` | working proof of the phase-7a mechanism: static plugin + `currentColor` | 1 (ph. 7a) |
| `iconbench.cpp` | 47 / 0.81 / 126.81 / 1.13 µs — why there is no icon cache to build | 1 |
| `dpitest.cpp` | HiDPI deviation (SVG 1.81 vs PNG 13.91) | 1 |
| `qtfacts.cpp` | the three Qt behaviours the plans rest on — re-run if a decision looks wrong | 1 |

## hygiene.py + uidiff.py — the Task 1 pair

`hygiene.py` does the repair; `uidiff.py` proves it. Workflow, no repo change until you want one:

```bash
./hygiene.py --dry-run                                  # hygiened copies in a temp dir
./uidiff.py --svgdir <that dir> --max-visible 17
./hygiene.py                                            # rewrite src/icons/*.svg in place
./uidiff.py --max-visible 17
```

**`hygiene.py` never touches your Inkscape config.** `fit-canvas-to-selection` honours the
`/tools/bounding_box` preference while `-D` export always uses the visual (stroke-inclusive) box;
mismatched, frames shift 1-2px. It pins the pref via a throwaway `INKSCAPE_PROFILE_DIR`.

**It does not add a `viewBox`** — `fit-canvas` sets width/height to the drawing, which is enough
(measured: severe 151 -> 1 without one). Order matters: the empty-flowRoot strip must precede the
fit, because the fit keys off the very bbox the flowRoot inflates.

`uidiff.py` asserts the SVG <-> PNG correspondence in **both** directions (an orphan is a defect a
bucket count cannot show), fails on any render failure, and names the VISIBLE ones — that list is
the phase-2.2 eyeball worklist. Gate per phase:

```bash
./uidiff.py --max-visible 36        # baseline (phase 0 + 1.1 done -> no orphan flags needed)
./uidiff.py --max-visible 17        # after 2.1 hygiene
./uidiff.py                         # after 2.4 regen -- identical by construction
```

**Not everything in the plans has a tool.** The counts (file:line, grep tallies, byte totals) were
measured by hand and are reproducible from the commands quoted in the plans themselves. If a
number has neither a tool here nor a command in the plan, treat it as unverified.

The C++ ones build with:

```bash
g++ -O2 -std=c++17 -fPIC <file>.cpp -o <file> $(pkg-config --cflags --libs Qt6Svg Qt6Gui Qt6Core)
```

and need `QT_QPA_PLATFORM=offscreen`. The three phase-7a checks link engine sources and need more
modules than that line gives — each carries its own build command in its header comment.
`darkcheck` and `widgetcheck` compile `src/qmapshack/svgticon/*.cpp` directly, so they test the
shipping code rather than a copy of it. `wptdiff.py` needs inkscape + python3-pil, nothing else.

## bgaudit.py + contactsheet.py — the phase-7 (dark mode) pair

`bgaudit.py` renders each icon on transparency and classifies it by what is actually opaque:

```bash
./bgaudit.py --dir src/icons              # summary + the non-transparent ones
./bgaudit.py --dir src/icons --kind edge-bg
```

**What it settled: the baked-background theory is wrong.** 262 of 329 are already fully
transparent, and **exactly one icon has a white background** (`TrkProfile.svg`). There is no
population of white-backgrounded icons to strip.

**Read `bgcolor` before believing `opaque-bg`.** 53 icons score ~100% opaque, but almost all are
navy `#000080` — they are solid icons that fill their own canvas (`Save.svg` is a navy floppy),
not icons with a backdrop. Phase 1's hygiene fit every canvas to its drawing, so any solid
rectangular icon covers 100% by construction. The label means "opaque", not "has a background".

So dark mode is **not** a background problem. It is a *drawing colour* problem: `#000080` is the
most common fill in the set (1246 uses) and it dies on a dark ground.

`contactsheet.py` builds the visual review page — every icon on Qt Fusion's `#efefef` and
`#353535`, with a **visibility score** (mean luma distance from the ground, 0–255; under ~20 the
icon is melting in) so the worst sort first. The svg is embedded, metadata stripped, so the page
stays zoomable.

```bash
./contactsheet.py --dir src/icons --out /tmp/iconsheet.html
```

**It is deliberately not finished.** A sheet of raw icons on dark only proves "navy is invisible"
329 times. What the review needs is *original on light* beside *recoloured on dark* — so the
third column waits on a recolouring rule to exist. Add it there rather than starting a new tool.

Measured worst-on-dark, the natural test set for any candidate rule: `PointShow`, `CutMode1`,
`WptProx`, `CutMode2`, `FilterSplitTrack`, `WptProj`, `TextItalic`, `CSrcUnknown`.

Colour census (`grep -oiE 'fill:#[0-9a-f]{6}' src/icons/*.svg | ... | sort | uniq -c | sort -rn`):
`#000080` 1246, `none` 740, `#ffffff` 292, `#ff0000` 105, `#000000` 55, greens ~107
(`#00ff00`/`#55d400`/`#008000`). **127 icons use white as a drawing colour** (paper, label
fields) — those are the parts that read *worse* under a naive inversion, and red/green are
semantic. Neither survives a blind invert.

## ccheck.cpp + svgt-proto/ — the phase-7a mechanism, made runnable

**`ccheck.cpp` exists because the received wisdom is wrong.** "QtSvg does not support
`currentColor`" is the forum consensus and it is **false on Qt 6.9.2**. Nine facts, each decided
something:

```bash
g++ -O2 -std=c++17 -fPIC ccheck.cpp -o ccheck $(pkg-config --cflags --libs Qt6Svg Qt6Gui Qt6Core)
QT_QPA_PLATFORM=offscreen ./ccheck
```

1-3, 7: `currentColor` resolves via root `color=`, root `style="color:"`, `<g color=>`
inheritance, and on `stroke` too. **6: a plain fill is untouched** — semantic red/green survive
for free. **4: lowercase `currentcolor` renders black** ([QTBUG-46947](https://bugreports.qt.io/browse/QTBUG-46947),
open since 2015). **5: `currentColor` with no `color` set is black** — the reason every themed
source needs a default root `color=`. **8-9: a `<style>` class beats a root `color=`** — the KDE
idiom and ours do not mix.

`svgt-proto/` is the mechanism end to end, standalone:

```bash
cd svgt-proto && cmake -S . -B build && cmake --build build
QT_QPA_PLATFORM=offscreen ./build/svgt_proto
```

It builds the `QIcon` with a bare `addFile(":/icons/Foo.svgt")` — byte for byte what `uic` emits
into `setupUi()` — and proves the **same** `QIcon` follows blue → red → green with no rebuild, no
widget walk, no `.ui` change, **and nothing deployed**. The last check proves a `#ff0000` fill
ignores the theme.

**Three things in it are load-bearing; do not "simplify" them:**

- **`qt_add_plugin STATIC` + `Q_IMPORT_PLUGIN`.** A dynamic plugin re-creates the 3.3
  `qsvgicon.dll` trap. Users build QMapShack from source — nothing may need deploying.
- **Key `svgt`, not `svg`.** Qt's `qsvgicon` claims `svg` and wins; with `svg` our `create()`
  **never fires** (verified by instrumenting it). Ours won earlier *only* via `QT_PLUGIN_PATH`,
  which is exactly the fragility we are removing.
- **`recolored()` REPLACES the root `color=`, never inserts.** A duplicate `color=` makes the
  document invalid and `QSvgRenderer` draws **nothing**. The sources carry a default
  `color="#000080"` (so `mkicon` still exports navy and an unthemed render looks like today), so
  insert-instead-of-replace is a blank-icon bug waiting to happen.

The proto reads the ink colour from `/tmp/themecolor` so the test can change it between renders;
the real engine takes it from `QPalette`. It has **no cache** — a real one must key on name +
size + mode + state + **ink colour**, never a bare `static` (see CLAUDE.md).

## themesvg.py + palette.py — the phase-7a source passes

```bash
./themesvg.py --dir src/icons --explicit          # dry run; --write to apply
./themesvg.py --dir src/icons --verify            # gate: 299 identical | 0 CHANGED
./palette.py  --dir src/icons                     # dry run; --write to apply
```

**`--explicit` is the black-arrowhead fix.** 40 shapes in 29 icons declare no fill anywhere up
their tree and lean on SVG's implicit **black** default. A colour that was never written down
cannot be recoloured, so no theming rule could ever reach them. Purely additive — the shape
already rendered black and now says so; `uidiff` stays PASS. **Applied.**

**Never parse-and-reserialise an icon to change it.** `ET.tostring()` rewrote all 29 files
wholesale (dropped the xml declaration and Inkscape's namespaces, reflowed every attribute):
**582+/2089−** for a 40-attribute fix, and **both gates still passed** — the render was identical.
A passing gate is not a good diff. Analyse with the parser (fill inheritance needs the tree), then
patch the original text by element id: **40+/40−**. See `add_fill_by_id()`.

**`palette.py` carries three things and they are different in kind:**
- `TYPOS` — `#000081` -> `#000080`. `CSrcCourse` alone, luma delta **0.072**, invisible on light.
  It split the compass on dark (8 facets mapped, 4 not). A typo only theming could expose.
- `ACCENTS` — one value per hue that clears delta>40 on **both** grounds. **Deliberately changes
  the light look**; `uidiff` does not apply. Not blanket-mappable: `#800000`/`#550000`/`#445500`
  are shading variants, not the primary accent.
- `OPTOUT` — the **21** icons with their own non-neutral background (`Mime*`, `*Project`, `Off`,
  `ToWksUnchecked`). Their content sits on *their* background, so neither pass applies.
  **Not** bgaudit's 53+14: most of those are navy *objects* (`Save` is a navy floppy) or navy
  frames (`Image`, `Map`, `Poi`), which must be themed.

**`themesvg.py` is re-runnable now, and that was not free.** `theme()` used to drop the scheme
block while leaving the previous pass's `class="ink" fill="currentColor"` behind — and currentColor
with no colour set renders BLACK. An `untheme()` pre-pass restores the literal colours first, so
roles are re-derived from the art every time. Verified lossless: the transform gives the same
283 themed / 26 untouched on today's tree as on the pre-markup tree. Do not remove it.

```bash
./colorstat.py --dir src/icons                # census + clusters + deviation
./colorstat.py --dir src/icons --unreachable  # just the worklist
```

`colorstat.py` **seeds its clusters with the INK/PAPER anchors on purpose.** Once a pass converts
the exact matches to currentColor they stop being literals, so a purely data-driven clustering
loses the centre the strays orbit — `#000081` would become its own cluster and report nothing.

**Census fill AND stroke — this blind spot bit twice.** Fills alone missed 865 `stroke:#ffffff`,
609 `stroke:#000080`, 378 `stroke:#000000`; and it made `Off` look like a hole icon when its power
symbol is `stroke:#ffffff` ×2.

## wptdiff.py — the important one

Mechanises Task 2's load-bearing claim. Re-renders all 203 waypoints with the same **rendering**
flags `makeicons` uses (`-D`, forced square), downscales to 32, premultiplies, compares against
the shipped PNGs, and **exits non-zero if any icon changed visibly**.

The *export* flag differs — `makeicons` uses the deprecated `--export-png=`, this uses
`--export-type=png --export-filename=`. Verified byte-identical output on Inkscape 1.2.2.

**Covers `src/icons/waypoints/` only** — not `cache/32x32/`'s `trailhead`/`found`/`dnf`, which
are waypoint symbols too. A PASS does not cover those 3.

```bash
./wptdiff.py --size 96              # must print PASS / visible (>8) == 0
./wptdiff.py --size 96 --keep out/  # also keep the generated PNGs
```

It fails loudly (exit 1) rather than quietly passing, on: any icon visibly changed; inkscape
failing to render any file (silent under-coverage used to PASS); comparing fewer than all 203;
`--ref` naming a size with no shipped reference dir; and `--keep` pointing at the source or
reference tree (which would have deleted the 203 shipped PNGs it compares against, then PASSed).
All five verified by fault injection.

Run before and after any waypoint resolution change. This is the safety net Task 2 step 1 asks
for; extend it with anchor stamping and a size×dpr sweep for the focus work.

## svgdiff.cpp

Renders every `*.svg` in a directory with `QSvgRenderer` at 32px and diffs against the
`32x32/<name>.png` sibling. Prints TSV: status, mean diff, % hard-different pixels, Qt ink box,
PNG ink box, Qt warnings.

```bash
QT_QPA_PLATFORM=offscreen ./svgdiff /home/oeichler/projects/qmapshack/src/icons
QT_QPA_PLATFORM=offscreen ./svgdiff /home/oeichler/projects/qmapshack/src/icons/waypoints
```

The **ink box** columns are the useful part: they exposed the root cause (Qt drawing into ~19×19
of a 32×32 frame while the PNG filled it).

**This is NOT the starting point for Task 2** (an earlier draft said it was). It renders SVG via
`QSvgRenderer` and diffs SVG-vs-PNG at a hardcoded 32px; Task 2 never renders SVG in Qt at all and
needs inkscape-PNG vs inkscape-PNG across resolutions. Use `wptdiff.py`. Only `inkBox()` is
reusable here.

Caveat: `svgdiff.cpp:67` reads via `QImage::pixel()`, which *un*-premultiplies
`Format_ARGB32_Premultiplied` back to plain ARGB — so it does not implement gotcha #1 below the
way `wptdiff.py` does. Harmless in practice (`qUnpremultiply` maps alpha=0 to 0), but do not cite
it as the reference implementation.

Bucketing used in the plans:

```bash
tail -n +2 result.tsv | awk -F'\t' '$4>=0 {if($4>=40)a++; else if($4>=20)b++; \
  else if($4>=10)c++; else if($4>=3)d++; else e++} \
  END {printf "severe %d | bad %d | visible %d | minor %d | ok %d\n",a,b,c,d,e}'
```

## iconbench.cpp

Cost per paint: fresh vs `static` `QIcon`, PNG vs SVG, raw `QSvgRenderer`, pixmap blit.
Produced the 47 µs / 0.81 µs / 126.81 µs / 1.13 µs table that killed the "build an icon cache"
idea.

```bash
QT_QPA_PLATFORM=offscreen ./iconbench <path/to/icon.png> <path/to/icon.svg>
```

## dpitest.cpp

At dpr=2, how close is each source to ground truth (an inkscape-rendered 64px PNG)? Produced the
HiDPI numbers (SVG 1.81 vs PNG 13.91 for ShowAll).

```bash
inkscape -D -w 64 -h 64 icon.svg --export-type=png --export-filename=truth64.png
QT_QPA_PLATFORM=offscreen ./dpitest icon32.png icon.svg truth64.png
```

## qtfacts.cpp — the assumptions, made runnable

Three Qt behaviours were measured, not assumed, and each decided something. If a future Qt makes
one false, the decision it drove needs revisiting.

```bash
QT_QPA_PLATFORM=offscreen ./qtfacts ../../src/icons/Zoom.svg ../../src/icons/waypoints/32x32/FlagRed.png
```

1. **`QPixmap(svg)` gives one raster at natural size; `QIcon(svg)` renders per request.** Hence
   `<pixmap>` refs needed a viewBox, and Windows must ship `iconengines/qsvgicon` or icons go
   blurry — not blank, which is why nobody would report it.
2. **`QIcon` refuses to upscale a raster** (`pixmap(40)` on a 32px source returns 32). Hence
   `IWksItem`'s `QPixmap icon` cannot become a `QIcon` while waypoint symbols stay raster: the
   delegate stretches to the row today, a `QIcon` would centre at 32 instead.
3. **Qt renders `<marker>` but ignores `markerUnits="strokeWidth"`** — the SVG default, and what
   Inkscape's stock arrows are authored against. Two lines of stroke-width 1 and 8 sharing one
   marker get identical arrowheads; per spec the second must be 8×. Hence markers are baked into
   geometry (`object-stroke-to-path`): an arrowhead is the one part of a drawing sized by stroke
   width, so it is the one part Qt draws wrong. If this ever prints "scaled", the baking is
   obsolete.

## Measurement gotchas (all three were hit for real)

1. **Premultiply before diffing.** RGB is undefined padding where alpha=0; comparing it raw
   reports garbage (once produced "140/203 changed" when the truth was 0).
2. **`du` is not a byte count.** It rounds each file up to a 4K block — with ~200 tiny icons that
   measures the filesystem. Use `cat * | wc -c`.
3. **`QIcon::pixmap(size, dpr)` returns a pixmap carrying `devicePixelRatio`**, so
   `drawPixmap(0,0,pm)` draws it at *logical* size. Call `setDevicePixelRatio(1)` or `toImage()`
   before measuring device pixels.
