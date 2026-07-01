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

## Code style

**All C++ is formatted with clang-format.** After editing any `.cpp` or `.h` file run:

```bash
clang-format -i <file> [<file> ...]
```

The style is defined in `.clang-format` in the project root (Google base, 120-column limit).

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

### IDrawContext — logical vs device pixels (HiDPI)

`convertRad2Px()`/`convertPx2Rad()` work in **logical** viewport pixels (built from `center` and
`scale*zoomFactor`) so they match Qt mouse/widget coordinates. The draw **buffers** are **device**
pixels: `bufWidth/bufHeight = viewWidth/viewHeight * pixelRatio + 2*BUFFER_BORDER`, and `draw()`
divides the scale by `pixelRatio`. Never compare a `convertRad2Px()` result against `bufWidth`/
`bufHeight` — they only agree at `pixelRatio == 1`. Use `viewWidth`/`viewHeight` for viewport-fit
tests (this caused QMS-1142: zoom-to-track clipped on HiDPI). Test HiDPI paths on a normal screen
with `QT_SCALE_FACTOR=2 build/bin/qmapshack` (add `QT_SCALE_FACTOR_ROUNDING_POLICY=PassThrough`
for fractional factors like 1.5).

### Icons — regenerating PNGs from SVG

`resources.qrc` only registers the PNG rasters (e.g. `32x32/Foo.png`, `48x48/Foo.png`);
editing an icon's `.svg` source has no effect until those are re-exported. `src/icons/makeicons`
does this for every icon in the directory in one pass; to regenerate a single icon instead,
call inkscape directly the same way it does per-icon:
`inkscape -D -w <size> -h <size> Foo.svg --export-type=png --export-filename=<size>x<size>/Foo.png`.

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

### Overview-advisory system: render-timeout dialog + proactive tree badge

`CDemVRT`/`CMapVRT::draw()` wraps its `ReadRaster()` call(s) with a 5s deadline
(`CGdalVrtUtil::read_deadline_t` + `progressCallbackWithDeadline()`, reusing GDAL's
progress-abort hook — confirmed to be invoked automatically by `GDALWarpedVRT::IRasterIO`
during warped reads; don't add a second warp-options progress callback, it caused a UI
freeze in a prior attempt). On timeout with an inadequate overview situation,
`CDemDraw`/`CMapDraw::sigOverviewAdvisory` fires (render thread → GUI thread,
`QPointer<CDemVRT/CMapVRT>` payload) and `CCanvas` shows `COverviewAdvisoryDialog`
(non-modal, `Qt::WA_DeleteOnClose`). The DEM/map tree also shows a proactive warning
badge on the same underlying condition (`showsOverviewWarning()`, see below), without
waiting for a render to actually stall.

**Key classes/files:**
- `helpers/CGdalVrtUtil.{h,cpp}` — `collectOverviewFactors()`, `buildOverviewAdvice()`,
  `overview_advice_t`, `read_deadline_t`
- `helpers/COverviewAdvisoryDialog.{h,cpp}` + `.ui` — the fix-it dialog
- `dem/CDemVRT.{h,cpp}` / `map/CMapVRT.{h,cpp}` — DEM/map data sources
- `map/IMapItem.h`, `map/IMap.h`, `dem/IDem.h`, `map/CMapItemDelegate.{h,cpp}` — the badge

**`collectOverviewFactors()` branches:**
- Branch 1 (`GetOverviewCount() > 0`): reads factors from the VRT's own overview list -
  but only after a *backing check*, since GDAL reports `GetOverviewCount() > 0` purely
  from a declared `<OverviewList>`, even when no source file actually has overview data
  behind it. Branch 1 scans `GetFileList()` for a real `.ovr` sidecar or a source TIF
  with its own overviews; if none exist, it falls through to Branch 2 instead of
  trusting the declaration.
- Branch 2 (no usable band-level overviews): probes each referenced file individually
  via `GDALOpen`, converting each one's native overview factors into the VRT's own
  pixel-scale units so sources of different native resolution stay comparable.

Either branch fills `perFileInfo` with one entry per source file (Branch 1's backing
check reuses the same probing pass), giving the advisory dialog's first table a real
per-file breakdown rather than one row for the whole VRT.

**Only `<OverviewList>` in the VRT's own XML routes warped reads to source overviews.**
Running `gdaladdo` on the source files alone does not fix slow rendering for a mosaic
VRT — the VRT's `<OverviewList>` must also be added/updated, which is why "Fix it" does
both (`gdaladdo` per source file, then `editVrtXml()` via `QDomDocument`).
`VRT_VIRTUAL_OVERVIEWS` was tried as a cheaper alternative and doesn't work (post-warp
`GetOverviewCount()` looks correct but rendering stays slow) — removed from both
`CDemVRT` and `CMapVRT`.

**`weakestMaxFactor`:** the weakest referenced file's own deepest overview factor (1 if
none). Rescaled by `warpScale = max(xsize_px/preWarpXSize, ysize_px/preWarpYSize)` after
the warped VRT is created, since a warp can change the dataset's own pixel density.

**`suggestOverviewLevels(xsize, ysize)` target is the same rule for DEM and map:** stop
once the decimated size drops below the primary screen's longest dimension (fallback
1920px). No per-map scale/zoom-range parameter - intentional, not a gap.

**Dialog lifecycle:** `CDemVRT`/`CMapVRT::advisoryOpen` suppresses `emitSigCanvasUpdate()`
retries while the dialog is open, stopping the canvas repaint loop and the delegate's
processing animation. Closing mid-fix (`closeEvent()` and `reject()` are both
intercepted, since Cancel/Escape reach `QDialog::reject()` → `hide()` →
`WA_DeleteOnClose` without ever calling `closeEvent()`) confirms, cancels the running
`CShell` job, and deletes any `.ovr` sidecars it already wrote. `sigFixItDone()`
(emitted once all fix commands and any `editVrtXml()` succeed) reloads via
`CDemDraw::setupDemPath()`/`CMapDraw::setupMapPath()`.

**Badge (proactive warning in the tree):** `IMap`/`IDem` gained `showsOverviewWarning()`
(default false; `CMapVRT`/`CDemVRT` override as `!suppressOverviewAdvisory &&
overviewAdvice.needsAttention()` — a method, not a field baked in by
`buildOverviewAdvice()`, since `weakestMaxFactor` is rescaled by warp scale *after* that
call returns). `IMapItem` mirrors it plus `triggerOverviewAdvisory()`; `CMapItem`/
`CDemItem` gate on `status == eStatus::Active` and route clicks through the existing
`emitOverviewAdvisory()` signal, reusing the reactive dialog's wiring unchanged.
`CMapItemDelegate` draws the badge over `rectIcon` (the left format icon) — not
`rectButton` (the right-hand activate/deactivate toggle, which already has its own
click handler). "Flag VRTs mixing files of different resolution/overview depth" needed
no separate detector: per-file factors are already normalized into the VRT's own
pixel-scale units, so the existing weakest-link check already catches a mismatched
source that falls short of target.

**Test fixtures** for the detection logic live outside the repo in `~/Downloads/dem/`
and `~/Downloads/map/` (local to this machine, not portable) - each has its own
`README.md` describing what it covers and how to reset it; that's the source of truth,
not this file.

**External tool paths in qmapshack:** Always resolve via
`IAppSetup::getPlatformInstance()->findExecutable("toolname")` — never hard-code a bare name.
`CAppSetupWin` restricts `PATH` to the app directory to prevent DLL conflicts, so a bare name
silently produces `QProcess::FailedToStart` on Windows if the binary isn't co-located with the app.
