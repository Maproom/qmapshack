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

### CDemVRT/CMapVRT render timeout + overview-advisory dialog (QMS-1128)

Implemented 2026-06-24. `CDemVRT`/`CMapVRT::draw()` wrap their `ReadRaster()` call(s)
with a 5s deadline (`CGdalVrtUtil::read_deadline_t` + `progressCallbackWithDeadline()`,
reusing GDAL's progress-abort hook). On timeout, if overviews are missing or inadequate
for the current view (`overviewAdvice.overviewsMissing || neededFactor > weakestMaxFactor`)
and the per-file `suppressOverviewAdvisory` flag isn't set and the dialog hasn't been
shown this session (`advisoryShownThisSession`), `CDemDraw`/`CMapDraw::sigOverviewAdvisory`
fires (render thread → GUI thread, `QPointer<CDemVRT/CMapVRT>` payload), `CCanvas` shows
`COverviewAdvisoryDialog` (`helpers/`, non-modal, `Qt::WA_DeleteOnClose`).

**Key classes/files:**
- `helpers/CGdalVrtUtil.{h,cpp}` — `collectOverviewFactors()`, `buildOverviewAdvice()`,
  `read_deadline_t`, `progressCallbackWithDeadline()`, `progressCallback()`
- `helpers/COverviewAdvisoryDialog.{h,cpp}` + `helpers/IOverviewAdvisoryDialog.ui` — the dialog
- `dem/CDemVRT.{h,cpp}` — DEM path; `map/CMapVRT.{h,cpp}` — map path

**`collectOverviewFactors()` — two branches:**
- Branch 1 (`pBand->GetOverviewCount() > 0`): reads factors from band overviews; `perFileInfo`
  gets the dataset itself as sole entry.
- Branch 2 (else-if, `GetOverviewCount()==0`): probes each file from `GetFileList()` via
  `GDALOpen`, collecting per-file factors; `perFileInfo` gets one entry per source file.

**`buildOverviewAdvice()` — command generation:**
- Uses `perFileInfo` to enumerate source files (skips filename itself).
- For Branch 1 VRTs (perFileInfo only has the VRT), `sourceFiles` is empty → `filesCommand`
  targets the VRT itself, `vrtCommand` is empty. **Known bug (Fix 2, pending):** should use
  `dataset->GetFileList()` (filtered: skip VRT path, `.ovr`, `.aux.xml`) to find source TIFs
  and target those instead.
- For Branch 2 VRTs, `filesCommand` has one command per source file, `vrtCommand` = gdaladdo
  on the VRT as fallback.
- **Critical finding (2026-06-30):** gdaladdo on source TIFs alone is NOT sufficient to fix
  slow rendering. The VRT must also have `<OverviewList>` added/updated. The current dialog
  only shows gdaladdo commands — for mosaic VRTs (Branch 2) this is incomplete and will not
  fix the problem. QMS-1135 must write `<OverviewList>` into the VRT XML as part of the fix.

**`weakestMaxFactor`:** the weakest source file's deepest overview factor (1 if no overviews).
Advisory fires on `overviewsMissing || neededFactor > weakestMaxFactor`. After warp creation,
`weakestMaxFactor` is rescaled by `warpScale = max(xsize_px/preWarpXSize, ysize_px/preWarpYSize)`.

**`VRT_VIRTUAL_OVERVIEWS` + `BuildOverviews("NONE")` — proven useless (2026-06-30):**
`VRT_VIRTUAL_OVERVIEWS` registers virtual overview levels on the warped VRT, but empirical
testing proves it does NOT route reads to source TIF overviews:
- `mosaic.vrt` (Branch 2, sources all have overviews): post-warp GetOverviewCount=17 via
  VRT_VIRTUAL_OVERVIEWS, but rendering was still slow (advisory fired).
- `mosaic_ovr_test.vrt` (identical + `<OverviewList>2 4 8 16 32 64 128</OverviewList>`):
  GetOverviewCount=7 inherited by GDALAutoCreateWarpedVRT, rendering was fast.
- Conclusion: the only reliable mechanism is `<OverviewList>` on the source VRT so
  `GDALAutoCreateWarpedVRT` inherits overview levels directly. VRT_VIRTUAL_OVERVIEWS can
  be removed entirely once `<OverviewList>` editing is in place.
**For Branch 1** (`<OverviewList>` already present): GDALAutoCreateWarpedVRT inherits
overview levels directly — VRT_VIRTUAL_OVERVIEWS was already guarded away here (Fix 1).

**GDAL callback verified:** `progressCallbackWithDeadline` IS called by `GDALWarpedVRT::IRasterIO`
during warped reads (confirmed by the advisory firing on the dev baseline without any extra
wiring). Do NOT add a separate warp-options progress callback — it caused a UI freeze in a
prior attempt.

**Debug instrumentation (2026-06-30):** `CDemVRT` constructor logs "OVR:" prefixed messages
at every decision point: pre-warp `GetOverviewCount`, which branch `collectOverviewFactors`
took, all per-file factors, generated command strings, post-warp `GetOverviewCount` (before
and after `VRT_VIRTUAL_OVERVIEWS`), `warpScale`, final `weakestMaxFactor`, and a one-line
assessment. Load each test VRT and grep for "OVR:" to verify behaviour.

**Test cases** in `~/Downloads/dem/` (backup: `~/Downloads/dem.backup/`):

| VRT | Pre-warp GetOverviewCount | Branch | Rendering |
|-----|--------------------------|--------|-----------|
| `mosaic.vrt` | 0 | Branch 2 (per-file probe) | Slow — VRT_VIRTUAL_OVERVIEWS doesn't help |
| `mosaic_ovr_test.vrt` | 7 (inherited) | Branch 1 | Fast — `<OverviewList>` fix verified |
| `mosaic_no_overviews.vrt` | 0 | Branch 2, no factors found | Slow (no overviews at all) |
| `mosaic_own_overviews_deep.vrt` | 7 (levels 2–128) | Branch 1 | Fast |
| `mosaic_own_overviews_shallow.vrt` | 2 (levels 2–4) | Branch 1 | Fast at low zoom, slow at high zoom |
| `DEM_10_Österreich.vrt` | >0 (.vrt.ovr sidecar) | Branch 1 | Fast |

`mosaic_ovr_test.vrt` = `mosaic.vrt` + `<OverviewList resampling="average">2 4 8 16 32 64 128</OverviewList>`,
no gdaladdo run — proves `<OverviewList>` alone routes warped reads to source TIF overviews.

Source TIFs: `austria.tif` 10m (ovr factors 2 4 8 16 32 64), `slovenia.tif` 20m (3 9 27 81 238),
`swiss_utm33.tif` 20m (5 10 25 50 100 198). `no_ovr/` symlinks same TIFs without .ovr sidecars.
VRTs use `<OverviewList>` XML (not .vrt.ovr sidecars) where applicable.

---

### QMS-1135 — In-app overview creation and proactive VRT health badge

Ticket: https://github.com/Maproom/qmapshack/issues/1135

**Goal:** Replace copy-pasteable command suggestions with in-app overview creation.
Show an orange warning badge in the DEM/map tree when a VRT's overview situation is
suboptimal — *before* a render stall. Clicking the badge opens a dialog that explains
the situation and provides buttons to fix it.

**Requirements (from ticket):**
- DEMs: full overview pyramid makes sense (used at every zoom level)
- Maps: only a limited overview range makes sense (they become unreadable at outer zoom
  levels and should be switched off via QMapShack's visibility settings)
- Prefer adding overviews to source sub-files over adding to the VRT itself
- Overviews always as standalone sidecar files (`-ro` flag, never embedded)
- VRTs mixing files of different resolution/overview depth should be flagged
- Orange triangle badge in the tree; clicking it opens a dialog
- Dialog provides a button to actually run the fix commands (via `IToolShell`/`CShell`;
  unification of these two classes is an open design question)
- Per-file suppression of the warning, persisted
- Confirmation before destroying/clearing existing overviews

**Current plan — phase 1 (in progress):**
Improve the situation analysis in `CDemVRT` constructor by expanding the "OVR:" debug
output, verify each test case behaves as expected, then build on a correct foundation.
The dialog still shows copy-pasteable commands for now; in-app execution comes later.


