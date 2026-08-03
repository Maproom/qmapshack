# QMapShack — Claude project notes

QMapShack is a Qt/C++ desktop application for planning and analysing GPS tracks, routes, and waypoints. It loads map tiles, vector maps, and DEM data, and supports online and offline routing engines.

---

## Stack

- **Language:** C++20
- **GUI / framework:** Qt 6.8+
- **Key libs:** GDAL, PROJ 8+, Routino, QuaZip-Qt6
- **Build:** CMake 3.20+, Ninja; Debug build in `build/`; binaries in `build/bin/`
- **Bundled 3rdparty:** alglib, Garmin FIT SDK

---

## Source layout

```
src/
  qmapshack/        main app (21 subsystems)
    canvas/         map rendering
    gis/            tracks, routes, waypoints, DB, GPX, routing (gis/rte/router/)
    mouse/          mouse interaction; line editing in mouse/line/
    map/, dem/, poi/, grid/, plot/, realtime/, device/, tool/, helpers/, widgets/
  qmaptool/         map creation tool
  qmt_map2jnx/      utility
  qmt_rgb2pct/      utility
  common/           shared code
```

---

## Working on recent changes

When asked "what were we working on" or "what did we do last", always check `git status && git diff` first. Memory is stale; the live diff is the ground truth for in-progress work. Memory can supplement but never replace the actual diff.

---

## Tree icons are `QIcon`

`IWksItem::icon` and `IDBItem::icon` are `QIcon` and `getIcon()` returns `const QIcon&`, so the
delegates render at device resolution and non-GIS tree icons stay crisp on HiDPI.

GIS items (wpt/trk/rte/area) stay raster — `paintItem` pulls the pixmap back out of the `QIcon` and
stretches it.

**Two delegate paths cannot assume the icon can fill the cell, and both branch on `actualSize`:**
`paintDevice` (MTP subclasses overwrite the SVG with a raster read off the device) and
`paintGeoSearch` (`CGeoSearch::setIcon()` composes a raster when "accumulative results" is on). Do
not collapse either to a bare `QIcon::paint`, and do not add a stretch back: `actualSize(rect) ==
rect` answers it, and `QIcon` downscales rasters fine. `paintProject`/`paintGeoSearchError` are
genuinely SVG and paint direct.

**`IDBItem::icon` holds folder icons only.** The DB blob raster lives on `CDBItem` as its own
`QPixmap displayIcon` (`getDisplayIcon()`), so `CDBItemDelegate::paintItem` stretches it directly.
Do not re-add a `setIcon(const QPixmap&)` overload on `IDBItem`.

## Code style

**All C++ is formatted with clang-format.** After editing any `.cpp` or `.h` file run:

```bash
clang-format -i <file> [<file> ...]
```

The style is defined in `.clang-format` in the project root (Google base, 120-column limit).

**Always accept clang-format's output — never revert or hand-tune it.** A run may reformat lines
unrelated to the edit (pre-existing drift); keep those hunks and do not ask about them.

**Every control-flow block requires braces**, even single-liner bodies:

```cpp
// correct
if (condition) {
  doSomething();
}

// wrong — never do this
if (condition)
  doSomething();
```

**Documentation comments use `/** */` doxygen blocks.** Follow the style of existing blocks (`@brief`, `@param`, `@return`). Inline member docs use `/**< */`. Plain `//` comments are for non-doxygen annotations.

**Keep comments crisp and short — state only the fact needed to understand what the code does, not prose.** Prefer one terse line over a multi-sentence paragraph. Explain the non-obvious *why* (a subtle invariant, a workaround, a gotcha); don't narrate what the code plainly says or restate it at length.

**Pass `QString`/complex objects (`QVector`, `QImage`, etc.) by `const&` unless the function actually mutates them.** A reference parameter that's only read should be `const T&` — it documents read-only intent and doesn't block callers from passing temporaries. Only use a plain `T&` for genuine out-parameters or objects the function mutates in place.

**Prefer `QFileInfo::exists(path)` (static) over `QFileInfo(path).exists()`** when you only need an existence check — no need to construct a full `QFileInfo`.

**Prefer Qt's fixed-width typedefs over plain C++ types**: `qint32`/`quint32` over `int`/`unsigned int`, `qreal` over `double`, `qsizetype` over `size_t`/`ptrdiff_t` for sizes coming from Qt containers. Exception: when a parameter type is dictated by an external API (e.g. GDAL's `int*` out-param, or `size_t` in a `ReadRaster()` signature), match that API's type instead of casting.

**No method implementation may be placed before the ctor and dtor of that class** in a `.cpp` file. The constructor and destructor always come first, in that order; every other member function follows after both.

---

## Building

```bash
cmake --build build --target qmapshack -j$(nproc)
```

The build directory is `build/` in the project root.

---

## Architecture: mouse/line editor

The interactive line editor (for tracks, routes, and areas) lives in `src/qmapshack/mouse/line/`.

### Class hierarchy

```
IMouseEditLine          – owns the point list (SGisLine), undo/redo history,
│                          routing mode buttons, and the active ILineOp
├── CMouseEditTrk       – edits a track (IGisItemTrk)
├── CMouseEditRte       – edits a route (IGisItemRte)
└── CMouseEditArea      – edits an area overlay (IGisItemOvl)

ILineOp                 – base for one interactive editing operation
├── CLineOpAddPoint     – insert new points / extend the line
├── CLineOpMovePoint    – drag an existing point to a new position
├── CLineOpDeletePoint  – remove a point
└── CLineOpSelectRange  – select a range of points for bulk operations
```

`IMouseEditLine` delegates all mouse events to the active `ILineOp`. Switching the toolbar button deletes the current op and creates a new one.

### Routing event-loop re-entrancy hazard

**This is the most important invariant in the subsystem.**

`CRouterSetup::calcRoute()` shows a `CProgressDialog` that runs a nested Qt event loop (`QEventLoop::exec()`). While that loop is spinning, the full Qt event pipeline is live — mouse moves, button clicks, and right-clicks all fire normally.

Consequences for `ILineOp` subclasses:
- `mouseMove()` fires during routing, drifting `points[idxFocus].coord` to the current cursor position.
- A right-click calls `abortStep()` → `restoreFromHistory()`, which reallocates the `points` vector and invalidates any saved indices or pointers into it.
- A further left-click can re-enter `leftClick()` while routing is already running.

**The guard is `runRoutingAndPin(coord)`** (defined in `ILineOp`):
1. Pins `points[idxFocus].coord = coord` before routing so the position is locked.
2. Sets `isRouting = true` — subclasses check this at the top of `leftClick()` to block re-entrant clicks.
3. Calls `slotTimeoutRouting()` (which calls `finalizeOperation()` → `tryRouting()`).
4. After the event loop returns, checks `idxFocus` and `points.size()` for validity (abort during routing leaves both in an unknown state).
5. Restores `points[idxFocus].coord = coord` to undo any drift from step 1.
6. Returns `false` if the operation was aborted (caller must return immediately).

Any future `ILineOp` subclass that triggers routing **must** use `runRoutingAndPin()` and check its return value.

### CLineOpAddPoint state

Two flags drive the three hover modes:

| `isDragging` | `focusIsEndpoint` | Meaning |
|---|---|---|
| false | false | cursor near a mid-segment — click inserts a point there |
| false | true  | cursor near the first or last point — click extends the line |
| true  | either | a new point is attached to the cursor — click drops it |

### Lead lines vs sub lines (vector/track routing)

When vector or track routing is active, `updateLeadLines()` finds the underlying map/track polyline nearest to the active point and stores it in `leadLineCoord1/2` (geographic) and `leadLinePixel1/2` (screen). `GPS_Math_SubPolyline()` then finds which segment of that polyline lies between the two adjacent line points and stores the result in `subLineCoord1/2` / `subLinePixel1/2`. The sub-line is what actually becomes the routed sub-segment on drop.

---

## Architecture: routers

`IRouter` is the abstract base. The two concrete routers are:

- **`CRouterRoutino`** — offline routing using Routino databases.
- **`CRouterBRouter`** — routing via BRouter, either as a local process (`CRouterBRouterLocal`) or via its online HTTP API.

`CRouterSetup` is a singleton (`CRouterSetup::self()`) that owns the active router and exposes `calcRoute()` to the rest of the application. It emits `sigHasFastRouting(bool)` when the router capability changes (e.g. local BRouter starts or stops); `IMouseEditLine` listens to this to enable/disable the auto-routing button.

Only local BRouter supports fast (on-the-fly) routing. Online BRouter and Routino do not — they require the full route to be calculated at once via `calcRoute(const IGisItem::key_t&)`.

---

## Architecture: tree item delegates

The three `QStyledItemDelegate` subclasses used by the tree views:

- `src/qmapshack/gis/CWksItemDelegate.{h,cpp}` (workspace tree)
- `src/qmapshack/gis/CDBItemDelegate.{h,cpp}` (database tree)
- `src/qmapshack/map/CMapItemDelegate.{h,cpp}` (map-item tree)

### Row layout: CRowBuilder

All `getRectangles*()` methods use `CRowBuilder` (`helpers/CRowBuilder.{h,cpp}`) to compute their rects. It carves a row into icon, button, and text zones without any magic-number arithmetic in the delegates.

**Tuning parameters** (defined in `helpers/CDraw.h`):
- `kCellPad` — outer inset on all four sides of `opt.rect`
- `kInnerGap` — gap between icon, text column, and each tool button

**Typical call sequence:**
```cpp
CRowBuilder row(opt.rect, kCellPad, kInnerGap);
const QRect rectIcon   = row.takeLeft(row.height());   // square icon
row.markStatusColumn();                                 // snapshot width for status line
const QRect rectButton = row.takeButton(fmName.height()); // name-height square button
const QRect rectName   = row.nameSlice(fmName.height());
const QRect rectStatus = row.fullStatusSlice(fmStatus.height());
```

**Key methods:**
- `takeLeft(w)` / `takeRight(w)` — carve a full-height rect, advance by `kInnerGap`
- `takeButton(iconSize)` — carve a square button sized so `CDraw::drawToolButton` renders its icon at exactly `iconSize × iconSize` (compensates for the button's internal icon inset)
- `markStatusColumn()` — snapshot the remaining rect before buttons are carved
- `nameSlice(h)` — top strip of the remaining (button-narrowed) centre area
- `statusSlice(h)` — bottom strip of the remaining (button-narrowed) centre area; use when buttons are full-height (CMap)
- `fullStatusSlice(h)` — bottom strip of the snapshotted pre-button column; use when buttons are shorter than the row (CWks, CDB) so the status line extends under them
- `rowHeight(cellPad, nameH, statusH)` — matching `sizeHint` height from the same parameters

**Button height convention:**
- `CWksItemDelegate`, `CDBItemDelegate`: buttons are `takeButton(fmName.height())` — sized to the name row only; status line uses `fullStatusSlice` and spans the full width underneath
- `CMapItemDelegate`: button is `takeRight(row.height())` — spans the full row height; status line uses `statusSlice` and is narrowed by the button

---

## Memory

Store all project-specific learnings, feedback, and notes in this file rather than
the auto-memory system (`~/.claude/projects/.../memory/`). Append new entries under
the relevant existing section, or create a new `###` subsection here when nothing fits.

Do not run `cmake --build` or any build command — Oliver builds externally.

Do not add `Co-Authored-By` lines to commit messages.

### Rich text colours — `CUiTheme`

`CUiTheme` (`src/common/theme/`) is the only source of status colours for rich text, label
stylesheets and `setTextColor()`. Roles `Neutral/Ok/Warn/Error/Info/Code`, each a fixed light/dark
pair, light arm as authored. Tune there, never per site.

- **Set both colours or neither.** A hardcoded background inherits the palette's text colour and
  inverts on dark. Normal text takes palette colours.
- Pick the entry point by what the widget *is*:
  - permanently a status message, only shown and hidden → `markLabel(label, role)`
  - a value that is *sometimes* a status → `span()`/`spanBold()`, so the colour travels with the
    string (`CSetupExtTools`, where one label alternates between "not found" and a path)
  - fills a background (table cell, banner) → `css()`
  - text on the widget's own background → `cssForeground()`/`foreground()`
- Never bake a colour into a `tr()` string or a `.ui` `<string>` — the translation carries it.
- `paletteIsDark()` is `inline` in `CUiTheme.h` so `CSvgtIconEngine::roleColor()` can share it with
  nothing to link; the plugin needs only `target_include_directories(svgticonengine PRIVATE ..)`.
  Every consumer of the light/dark split uses it — a second copy of the threshold drifts silently.
- Re-render on a scheme switch from `QEvent::ApplicationPaletteChange || QEvent::PaletteChange`
  (`CSvgtIcon`, `CHelpBrowser`, `CListTrkPts`), not from `QStyleHints::colorSchemeChanged`: the
  colours resolve through `QGuiApplication::palette()`, and a palette event is delivered *because*
  that palette changed. A widget may get both events, so a re-render can run twice per switch.
- Printing: `const CUiTheme::ForceLight paperColours(printable)` in `CDetailsPrj::draw()`, beside
  the two palette branches it completes. It does not touch `QPalette`, so palette colours still
  need their own paper branch.
- `QTextBrowser` applies a `<link>`ed stylesheet (`loadResource`, `StyleSheetResource`), so
  `CHelpBrowser::loadResource()` appends a themed `code, pre` rule to whatever the packaged help
  ships. That CSS lives outside this repo; `Role::Code`'s light arm is picked to match its `code`
  background, so light mode renders unchanged.
- Untouched on purpose: `CShell`/`IToolShell` colour each line as it is appended, so lines already
  in the log keep the scheme they were written in; `IPlot`'s plot sheet, the halo
  under `CWksItemDelegate`'s progress bar and `CIconGrid`'s tiles are white by design.

### IDrawContext — logical vs device pixels (HiDPI)

`convertRad2Px()`/`convertPx2Rad()` work in **logical** viewport pixels (built from `center` and
`scale*zoomFactor`) so they match Qt mouse/widget coordinates. The draw **buffers** are **device**
pixels: `bufWidth/bufHeight = viewWidth/viewHeight * pixelRatio + 2*BUFFER_BORDER`, and `draw()`
divides the scale by `pixelRatio`. Never compare a `convertRad2Px()` result against `bufWidth`/
`bufHeight` — they only agree at `pixelRatio == 1`. Use `viewWidth`/`viewHeight` for viewport-fit
tests (this caused QMS-1142: zoom-to-track clipped on HiDPI). Test HiDPI paths on a normal screen
with `QT_SCALE_FACTOR=2 build/bin/qmapshack` (add `QT_SCALE_FACTOR_ROUNDING_POLICY=PassThrough`
for fractional factors like 1.5).

### Icons — hold a `QIcon`, never a `QPixmap`

A `QPixmap` is one raster frozen at the dpr it was built at: it cannot serve a larger request and
cannot follow a window to another screen, whatever the source format.

- Anything taking a `QIcon` (buttons, actions, tree/list items, a Designer `<iconset>`): reference
  `:/icons/Foo.svgt` and let the paint path ask for the size.
- Static icon in a dialog: `QSvgWidget` + `CSvgtIcon::load()`. Qt ships no widget that displays a
  `QIcon`, and `uic` bakes a `<pixmap>` into `QPixmap(path)` before any widget sees the path.
- Rich text `<img>`: `CSvgtIcon::htmlImageSrc()`, and always give both `width` and `height` —
  without them there is no HiDPI path.
- Canvas rasters (waypoints, POI, cache) are data, not icons.
- Never `static` a paint-path icon — a static pins the colour scheme live at first paint and hides
  the icon from a path-shaped grep. There are none in the tree; keep it that way.
- `actualSize(rect) == rect` is the test for "can this icon fill the cell".

`cmake/IconGate.cmake` fails the build on the regex-detectable violations. It deliberately cannot
see `QIcon(pixmapVariable)` or a `.pixmap(w,h)` missing its dpr.

**Left alone deliberately — do not "finish" these:** the three `.pixmap(w,h)` calls with no dpr
(`CSelectCopyAction.cpp:48/50`, `CInvalidTrk.cpp:34`); `IGridPlacer.ui`'s `line_3px_*` black PNGs;
`CGeoSearchWeb`'s service icons, where the stored path is user data and `defaultIcon` equality is
the "is this user-added" test — converting needs a settings migration plus an `isUserDefined` flag
first, or "Restore default list" erases the user's own services.

### Icons — nothing that stores an icon path may be pruned

No icon PNG is ever pruned and no dead qrc entry is removed. Two populations store a *path*, not a
picture, so a pruned PNG is a blank icon in somebody's existing file:

| population | stored in |
|---|---|
| `history_event_t::icon` | `.qms`, `.gpx`, DB `data` column |
| `getInfo()` HTML `<img src>` | DB `comment` column |

History icons are **PNG on disk, `.svgt` in memory**: `displayIconPath()` resolves PNG→`.svgt` on
load, `savedIconPath()` converts back on save, and neither marks the item changed. A saved file
stays readable by a build without the icon engine, where a `.svgt` path renders blank.

The `comment` column keeps its PNG paths — it exists for full-text search and is never rendered.

### Icons — drawing rules that no tool catches

Developer howto: `README_ICON.md`.

- **Draw structural line-art in `ink`.** Qt greys a disabled icon by lightness only, so an icon
  drawn just in `lead`/`paper` looks identical enabled and disabled on dark. Keep `lead` for a
  secondary outline. (`RatingStarEmpty`/`UnFocus` stay grey because grey *is* their meaning.)
- **Never give a shape's stroke the same role as its fill.** It renders invisible and every tool
  passes, because each colour is individually valid. Only a render shows it.
- **The letter or shape carries the meaning; colour is never the only cue** — `SQLite`/`MySQL` use
  a bold initial on the cylinder face plus a brand-colour cap, not colour alone.
- **Negation uses the set's own mark**, a red `#ff5555` disc with a `paper` slash, as `NotPossible`.
- **Bake lettering to paths** (`inkscape --actions "select-all;object-to-path"`), then strip the
  group's inline `style` back to `fill:currentColor`. The conversion resolves the class's `color`
  inline, and an inline value shadows the themed class. Never ship live `<text>`.
- A family (`Act*`, `Add*`, `Mime*`) shares stroke weight, corner radius and optical size.

### Icons — Qt renderer traps the pipeline works around

Qt has no SVG recolouring API, so `CSvgtIconEngine` rewrites the SVG text and loads via
`QSvgRenderer(QByteArray)`. Qt's renderer *does* resolve `currentColor` — only the setter is
missing. The traps that shape the pipeline:

- `currentColor` with no `color` set renders **black** (in Qt and inkscape); a **duplicate**
  `color=` renders **nothing**; lowercase `currentcolor` is black (QTBUG-46947); a `<style>` class
  beats a root `color=`, so the KDE/Breeze idiom does not mix with ours.
- `QSvgRenderer` **ignores a class-supplied `fill:`**. `recolored()` inlines the resolved fill as a
  presentation attribute; without it 19 icons render black.
- Qt ignores `markerUnits="strokeWidth"`, so `svghygiene` bakes markers into geometry. Set
  `stroke:none` wherever `stroke-width` is 0 **first**, or the shape becomes a filled block.
- `QIcon(":/x.svg")` needs both `imageformats/qsvg` and `iconengines/qsvgicon` deployed; all three
  platforms are confirmed OK.
- Qt 6.10.0 has a `currentColor` regression, fixed in 6.10.1 (QTBUG-141102).

Dark `ink` is `#9999ff`: it must stay legible on `paper` `#353535` at 4.5:1 while staying clear of
`lead` `#e0e0e0` (120 icons paint both) and `mark` `#66aaff` (19 icons paint both). That rules out
the azure family — a "more vibrant blue" means a more saturated navy, not a different blue.

### Icons — waypoints are data, not chrome

`src/icons/waypoints/` is named by the GPX `<sym>` vocabulary shared with Garmin and its exact look
is a frozen contract, so it **stays PNG on the canvas**: SVG would hand rendering to whichever Qt
the user has, and a Qt antialiasing change could silently restyle accepted iconography. External
user icons are PNG/BMP forever (`CWptIconManager.cpp:2010`), so the raster path must exist anyway.
Dark theming does not apply — they sit on map tiles, not the UI palette.

A waypoint symbol used as **UI chrome** (menu action, tool button) is under none of that and uses
the SVG. Only `FlagBlue.svg` and `PinBlue.svg` are registered; add others as UI needs them.

Gate any waypoint change with `src/icons/tools/wptdiff.py --size 96`, which must report
`visible (>8) == 0`.

**`icon_t::focus` is absolute pixels of the loaded raster**, hardcoded against 32. Any resolution
change breaks every anchor until focus is stored relative (0..1) — a prerequisite for touching
waypoint resolution at all. Two getters exist: `getWptIconScaledByName` holds `focus = focus *
scale`, `getWptIconByName` does not. `focus` is serialized but overwritten on every load via
`deriveSecondaryData()`, so a relative-focus change needs no migration.

### TODO — POI icons (SJJB) can be converted

POIs are not waypoints: no canvas freeze applies. 303 SVGs already ship under
`src/icons/poi/SJJB/svg/<category>/` and 249 of the 250 referenced PNGs have a counterpart, but
none are in `resources.qrc`. They are SJJB templates carrying a placeholder fill `#111111` that
their build recolours per category, so the colour has to be recovered per icon from the shipped
PNGs (16 categories, 6 of them with 2–3 variants).

Work: register ~250 SVGs, add a recolour step, change `CPoiIconCategory`'s `QPixmap` members to
paths plus ~302 literals in `CPoiFilePOI_TagMap.cpp` — the path *shape* changes, so that needs a
lookup table, not a regex — and a render cache keyed by (icon, size, dpr). The cache is a win
regardless: `CPoiFilePOI.cpp:204` `.scaled()`s a pixmap per POI per repaint.

Gate it or do not do it: render each recoloured SVG at 32, diff against its shipped PNG, require
`visible (>8) == 0`. A diff that measures **colour** reports ~240 false failures.

Two defects found while scoping: `CPoiFilePOI_TagMap.cpp:118` asks for
`health_pharmacy_dispencing` (a typo; the SVG is `pharmacy_dispensing`), and one SVG embeds a
raster that is not in the tree (`pastedpic_10102008_233747.png`).

### GIS item icons — serialization stores sym/colour, not the icon

`.qms`/DB persist the **symbol name** (`wpt.sym`, `serialization.cpp:86`) and the **colour**
(`trk.color`/`area.color`, `:614`/`:827`) — never a rendered icon. The icon is re-derived on load:
waypoint via `getWptIconByName(sym)`, track/area by loading `Track.png`/`Area.png` as a shape mask
and filling it with the data colour. The rendered pixmaps in the DB `items.icon` BLOB and `.qms`
`history_event_t.icon` are output caches, not sources of truth. **Serialization does not constrain
the source format** — do not repeat the belief that it does.

So only the waypoint symbol is a genuinely frozen raster, and for the canvas-freeze reason above.
**Tracks and areas can be SVG**: their PNG is used only as a silhouette mask
(`createMaskFromColor`), so an SVG rendered at the target size gives a crisp mask and the same data
colour. `QIcon` will not upscale a raster, so such a change must render the SVG at size rather than
wrap the old 32px PNG.

### CMapItemDelegate — forward declaration pitfall

`animations_t` is defined after `getAnimations()` in the private section. The forward
declaration `struct animations_t;` before `getAnimations()` is required — do not remove it.

### QImage::Format_Indexed8 + GDAL RasterIO/ReadRaster — row padding pitfall

Never pass `img.bits()` directly as the destination buffer for a GDAL `RasterIO`/`ReadRaster`
call when the image width isn't guaranteed to be a multiple of 4. Qt may pad
`QImage::bytesPerLine()` beyond the pixel width for 1-byte-per-pixel formats (`Format_Indexed8`),
but a GDAL read with no explicit line spacing assumes the buffer is tightly packed
(`bytesPerLine == width`), silently corrupting/skewing every row once they diverge.

The old `CMapVRT` tiling code (pre-GDAL-warp version) masked tile width with `& 0xFFFFFFFC` to
dodge exactly this for partial edge tiles — a sign the bug is real, not theoretical. The fix
(used by both `CDemVRT` and the current `CMapVRT`, see `map/CMapVRT.cpp`'s `draw()`): read into a
flat `QVector<quint8>` (no padding concerns, it's just linear memory) and build the `QImage` via
the constructor that takes an explicit `bytesPerLine` argument:
`QImage(buf.constData(), w, h, w, QImage::Format_Indexed8)`. Multi-byte-per-pixel formats
(`Format_ARGB32`, 4 bytes/px) aren't affected since `width * 4` is always a multiple of 4.

### CMapVRT/CDemVRT warped VRT — transparency outside the source footprint

`GDALAutoCreateWarpedVRT` resamples onto an axis-aligned bounding box around the (possibly
rotated) reprojected footprint, so corners with no source coverage exist whenever the source
isn't already axis-aligned with the target SRS. What fills those corners depends on the data path:

- Single-band palette/gray (`CMapVRT`): already handled. If the source declares a nodata value,
  GDAL falls back to using it as both src/dst nodata for the warp (we never set
  `padfSrcNoDataReal`/`padfDstNoDataReal` ourselves), so uncovered pixels come back as that nodata
  index — and the constructor already zeroes that index's alpha in the colortable. No source
  nodata declared means no automatic transparency here; `Format_Indexed8` has no separate alpha
  channel to retrofit one.
- Multi-band RGB(A) without its own alpha band (`CMapVRT`): fixed via a synthetic destination alpha
  band (`GDALWarpInitDefaultBandMapping` + `psOptions->nDstAlphaBand = nBandCount + 1`, mirroring
  `gdalwarp -dstalpha`). The warp tracks per-pixel source coverage into that band automatically;
  `rasterBandCount` is re-read from the warped dataset afterward so `draw()`'s band loop picks it
  up like any other band. Verified with a standalone `gdalwarp`/Pillow test (rotated 3-band source,
  `-dstalpha` on vs. off): without the alpha band, uncovered corners come back **solid black**
  (0,0,0) - GDAL's own warp fill, not the `img.fill(white)` pre-fill in `draw()`, which gets
  unconditionally overwritten by the per-band `ReadRaster` call regardless of coverage. With the
  alpha band, those same corners get alpha=0 and composite-out correctly.
- `CDemVRT`: no such handling exists. Uncovered elevation samples read back as whatever the
  destination buffer was zero-initialized to (not `NOFLOAT`), since `getElevationAt()`/`draw()`
  never check warp coverage explicitly. Hasn't been revisited — only matters for non-axis-aligned
  DEM sources.

### CDemVRT/IDem rendering-speed punch list

Rendering-speed pass on `CDemVRT`/`IDem` (2026-06-24, see `git log` on `CDemVRT.{h,cpp}`/
`IDem.{h,cpp}` for the done work): removed a per-pixel virtual-call+`QString` in elevation
shading, replaced `atan2`/`sin` with an algebraic identity in hillshading, made `draw()` reuse its
read/output buffers across frames instead of reallocating, hoisted several loop-invariant
per-pixel computations, and fused all 5 shading layers into 3 passes (`IDem::computeShading()`)
instead of one `threadPool` dispatch round per layer - each pass entered only if at least one of
its layers is active, so "is this layer enabled" is a once-per-call decision, never a per-pixel
one.

Still open: batch `getElevationAt()`/`getSlopeAt()` point queries (currently one small `RasterIO`
call per point, e.g. per vertex of a track elevation profile) - a different code path than
`draw()`'s map rendering, worth revisiting only if track-profile performance comes up.

Skipped: caching `1/xscale`/`1/yscale` for `slopeOfWindowInterp()` - modest win, and hoisting it
would need either a signature change touching its 3 callers or new cached members with a
staleness trap (`xscale`/`yscale` are plain protected members assigned directly, no setter to keep
a reciprocal in sync).

### DEM overview corruption — blank hillshade when zoomed out

Symptom: hillshading renders at close zoom but is blank far out. Cause: `CDemVRT::draw()` reads
via `ReadRaster()` with automatic overview selection, so at high `buf_scale` GDAL can pick a
corrupt/all-NoData overview level and return an all-NoData buffer. Check overview integrity
*before* suspecting `CDemVRT.cpp`/`IDem.cpp`: `gdallocationinfo -valonly -overview <N> <vrt> <x>
<y>` at several sample points. Fix by rebuilding the `.ovr` (`gdaladdo -ro -r average <vrt>
<factors>`, deleting the old `.ovr` first). Hit once on `Bayern_DGM1.vrt` (factor-16 overview
was all `-9999`).

### Overview-advisory system: render-timeout dialog + proactive tree badge

Warns when a VRT-backed map/DEM has missing/inadequate GDAL overview pyramids, or too
many source files, and can fix both.

**Key files:**
- `helpers/CGdalVrtUtil.{h,cpp}` — `buildOverviewAdvice()`, `suggestOverviewLevels()`,
  `handleRenderTimeout()`, `overview_advice_t`, `file_overview_info_t`, `raster_geometry_t`,
  `overview_advisory_state_t`, `read_deadline_t`, `kMetersPerDegree`
- `helpers/CVrtAdvisoryDialog.{h,cpp}` + `.ui` — the fix/info/combine dialog
- `helpers/CVrtCombiner.{h,cpp}` — the "Combine files..." grid-split/footprint logic
- `dem/CDemVRT.{h,cpp}`, `map/CMapVRT.{h,cpp}` — own `overviewAdvice`/`advisoryState`/`rasterGeometry`
- `dem/CDemWCS.cpp` — opts out via `supportsOverviewAdvisory=false`
- `map/IMap.h`, `dem/IDem.h`, `map/IMapItem.h`, `map/CMapItemDelegate.{h,cpp}` — badge + on-demand info
- `map/CMapItem.cpp`, `dem/CDemItem.cpp`, `map/CMapList.cpp`, `dem/CDemList.cpp` — context-menu entry
- `canvas/CCanvas.cpp` — owns/shows the dialog

**Invariant: the dataset is always a VRT.** `new CMapVRT`/`new CDemVRT` only happen for
files with suffix `.vrt`; every other format has its own class. `CDemWCS` opts out via
`supportsOverviewAdvisory=false`. `buildOverviewAdvice()` relies on this — no "concrete
raster format" branch exists.

**Invariant: `overviewAdvice`, tile count and overview state are immutable per instance.**
Any DEM/map list change — including a successful Fix/Combine (`sigContainerRebuilt` →
`setupDemPath`/`setupMapPath`) — destroys the `CDemVRT`/`CMapVRT` and creates a new one with
freshly-built advice; nothing updates it in place. `overviewNeedsAttention` caches
`needsAttention()` at setup so the tree delegate's per-paint badge poll (`showsOverviewWarning()`)
is O(1) instead of walking `perFileInfo`.

**Trigger:** `CDemVRT`/`CMapVRT::draw()` wraps `ReadRaster()` with a 5s deadline
(`read_deadline_t` + GDAL's own progress-abort hook — don't add a second warp-options
progress callback, it caused a UI freeze). On timeout, `handleRenderTimeout()` fires the
advisory (render thread → GUI thread) once per loaded instance per session, but only when
`showsOverviewWarning()` (`!suppress && needsAttention()`) — the same condition as the
proactive tree badge, so a render slow for an unrelated reason no longer pops it.

**The advisory dialog is application-modal** (`setModal(true)`): while it is open the
map/DEM it is about must not be read. A pan's `draw()` or a mouse-move's `getElevationAt()`
racing a Fix/Combine file rewrite (external gdaladdo/gdalbuildvrt process — the in-process
dataset mutex can't guard it) crashes GDAL. Modal blocks user input; a background redraw
from a sibling layer during a job is a known residual (not guarded).

**`buildOverviewAdvice(dataset, band, isPaletteIndexed, suggestedLevels)`:** a read can be
sped up by two additive sources — the container's own overview, and/or each source
file's own overview for whichever region is read.
1. Container's claim is trusted immediately only if a real `.ovr` file is in
   `GetFileList()`; a bare `<OverviewList>` is not trusted yet.
2. If the verified container factor already meets `targetFactor`, every source file is
   skipped (`perFileInfo` still lists them, `checked=false`).
3. Otherwise every source is probed. An unverified `<OverviewList>` becomes trusted if
   every source turns out to have its own overview; otherwise discarded.
4. `weakestMaxFactor = max(containerFactor, weakestSourceFactor)`.

`containerHasOwnOvr` is `true` only via step 1; a `checked=false` entry (step 2) always
implies `containerHasOwnOvr == true`.

**Factors are per-file pixel ratios (`fullResSize / overviewSize`)**, read from each
file's own band before any warp — no geotransform/CRS math involved, even when sources
and container differ in CRS.

**`suggestOverviewLevels(xsize, ysize, maxFactor)`:** doubles from 2 until the decimated
size drops below the primary screen's longest dimension (fallback 1920px) or `maxFactor`.
`CDemVRT` leaves `maxFactor` unbounded; `CMapVRT` caps at `kMaxMapOverviewFactor = 16`.

**`<OverviewList>` mechanics:** `gdalbuildvrt` (GDAL ≥ 3.2) writes it automatically only
at build time, not retroactively. GDAL won't complain about a declared factor with no
backing data — silently falls back to full-resolution reads, hence the verification
above. Minimum supported GDAL version: 3.10.

**"Fix overviews"** (`slotFixOverviews()`/`finishFixOverviews()`):
1. `filesToFix()` runs `gdaladdo` on every source short of `suggestedLevels` (or on
   `filename_` itself if there are no source files). Recipe: `-r` (nearest for palette,
   average else), `COMPRESS_OVERVIEW=DEFLATE`, plus `PREDICTOR_OVERVIEW=2` for non-palette
   data (matches the source predictor → ~2-3× smaller `.ovr`; harmful on palette indices).
   `.ovr` block size is inherited from each source automatically by GDAL — no need to set it.
2. `fixContainerOverviewList()` then rewrites just the `<OverviewList>` element to
   `advice_.suggestedLevels` (no full `gdalbuildvrt` re-run, no re-probe).

**Disk usage (`diskUsageBytes`/`diskUsageIsEstimate`):** the dataset's real on-disk footprint,
summed with `QFileInfo` over `GetFileList()` plus each source's `.ovr`/`.aux.xml` sidecars
(GetFileList omits *source* sidecars, includes only the container's own). Fully qualified →
exact total (precise); shallow/missing/no overviews → sub-files × 5/3 (estimate). The dialog
formats it with `QLocale::DataSizeSIFormat` to match `du --si` (not the 1024-based IEC default).

**Dialog table:** container is a synthesized row, graded by the same `rowStatus()` as
source rows. `htmlTd()`/etc. are static methods (not free functions) so they can call
`tr()`; pass plain `<`/`>` into them, they escape it themselves. `hasExistingOverviews()`'s
container fallback keys off `containerHasOwnOvr` (not `containerFactor > 0`) so the
"Update"/"Add `<OverviewList>`" wording agrees with the fix confirmation dialog.
`raster_geometry_t` comes from `CGdalVrtUtil::sourceGeometry(pre-warp source)` — the source
file's own size/resolution, matching `gdalinfo` (exact meters for a projected CRS,
`kMetersPerDegree` approximation for geographic). Not the warped grid: reading the warped
geotransform gave wrong pixel sizes (e.g. +26% for a UTM source drawn in EPSG:4326).

**Badge + on-demand info:** `showsOverviewWarning()` (`!suppress && needsAttention()`)
drives the tree badge; `hasOverviewInfo()` drives the "Overview Info..." context-menu
entry regardless of attention state. `CMapItemDelegate::overviewBadgeRect()` is shared by
`paint()`/`editorEvent()`/`helpEvent()` so painted/clickable/tooltip areas can't drift.

**Dialog lifecycle:** `advisoryState.open` suppresses `emitSigCanvasUpdate()` retries
while open. `closeEvent()`/`reject()` both confirm-cancel a running job and clean up
partial output. `sigContainerRebuilt()` reloads via `setupDemPath()`/`setupMapPath()`.
`CCanvas::showOverviewAdvisory()` dedupes by *filename* via
`findChildren<CVrtAdvisoryDialog*>()`, not a separate registry.

**Subfile-count check (independent of overviews):** `hasTooManySubfiles()` flags a VRT
with more than `kMaxSubfileCount` (50) source files — GDAL opens/stats every referenced
source overlapping a read region, so reading stays slow regardless of overviews.
`needsAttention()` is `needsOverviewFix() || hasTooManySubfiles()`; the two problems have
independent fixes ("Fix overviews" vs. "Combine files...") and independent gating.

**"Combine files..." (`CVrtCombiner`):** rewrites the container VRT to reference a
handful of large, compressed/tiled GeoTIFFs instead of its many small source files.
- **Splits the container's own resolved raster**, not the source files — `computeGrid()`
  cuts a plain pixel-window grid (`pixel_window_t`, row/col-tagged). The merge step is a
  pure crop (`gdal_translate -srcwin`), no resampling.
- **Layout comes from the VRT XML, no pixel reads.** `readVrtLayout()` parses
  `<VRTDataset rasterXSize/rasterYSize>` and every source's `<DstRect>` footprint.
  `tightenToFootprints()` crops each cell to the bbox of the footprints overlapping it, or
  drops it (`empty()`) if none does. Resolves in ms even for a huge VRT, so it runs inline
  on the GUI thread — no background scan, `CThread`, or `QProgressDialog`. (An earlier
  design read pixels with `GRIORA_Average` to find nodata; that read the whole multi-GB
  dataset — the `<DstRect>`s already say where the data is.)
- `kMaxOutputTiles` (40) / `kMaxPixelsPerTile` (150,000,000) are **tuning placeholders**,
  not settled values — need real tuning against a large VRT once exercised for real.
- `slotCombineFiles()` reads the layout, computes+tightens the grid, confirms with the
  user, backs up `filename_` to `filename_ + ".bak"`, runs one `gdal_translate -srcwin`
  per tile into the *source files'* directory (`group_r<row>_c<col>.tif` — can differ
  from the VRT's own directory), then one `gdalbuildvrt -overwrite`. Compression:
  `COMPRESS=DEFLATE`/`PREDICTOR=2` (not ZSTD — not a mandatory GDAL dependency),
  `TILED=YES`, `BLOCKXSIZE`/`BLOCKYSIZE=512`, `BIGTIFF=IF_SAFER`.
- `JobKind` (`FixOverviews`/`Combine`) dispatches `slotJobFinished()` to
  `finishFixOverviews()`/`finishCombine()`; both emit `sigContainerRebuilt()` on success.
- **Non-destructive:** original source files are never deleted. On cancel/failure,
  `filename_` is restored from `.bak` (only the final `gdalbuildvrt -overwrite` touches
  it, so a kill there is the only way it ends up truncated) and partial tiles are removed.
- **Known limitations:** footprint tightening trims only the nodata border between source
  tiles, not nodata *inside* a source; re-running Combine with a different grid size can
  leave stale `group_rX_cY.tif` files.

**External tool paths in qmapshack:** Always resolve via
`IAppSetup::getPlatformInstance()->findExecutable("toolname")` — never hard-code a bare name.
`CAppSetupWin` restricts `PATH` to the app directory to prevent DLL conflicts, so a bare name
silently produces `QProcess::FailedToStart` on Windows if the binary isn't co-located with the app.

### Follow-up (QMS-1156): convert non-UTF-8 VRT files to UTF-8 on load

Since QMS-1153 (UTF-8 process manifest) + QMS-1139 (removed the encoding workarounds), GDAL
receives filenames as UTF-8 everywhere. A guard rejects `.vrt` files whose bytes aren't valid
UTF-8, but that dead-ends users with legacy Windows-1252/Latin-1 VRTs. Follow-up ticket QMS-1156:
offer a confirmed, `.bak`-backed one-click conversion instead of just rejecting.

Verified GDAL facts (tested on 3.12) behind the design:
- GDAL's CPL XML parser **ignores the VRT `<?xml encoding?>` declaration** — `<SourceFilename>`
  bytes go to `open()` verbatim. Even a correctly-declared `ISO-8859-1` VRT fails. Only raw bytes
  matter, so a byte-level UTF-8 check has no false-rejection risk.
- On-disk source names are UTF-8, so transcoding a Latin-1 VRT reproduces the real names.
- GDAL doesn't hard-fail a bad path (exit 0, checksum -1, `ERROR 4` on stderr) — a quietly-broken
  dataset, which is what feeds the missing-file advisory / endless loop.

Design: transcode from candidate encodings (Windows-1252 → ISO-8859-1 → system ANSI), then
**verify by resolution** (every `<SourceFilename>` must now exist on disk) — never a blind guess;
first candidate that resolves all sources wins, else fall back to plain rejection. Trigger at load
time in `CMapVRT`/`CDemVRT` construction (a non-UTF-8 VRT never constructs, so the advisory dialog
can't be the entry point), reusing the advisory `.bak`/rewrite scaffolding. Non-platform-gated
(failure reproduced on Linux). Also rewrite the `<?xml encoding?>` declaration to UTF-8 for hygiene.
