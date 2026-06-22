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

## Working list: CDemVRT / IDem cleanup

Found during a review of `src/qmapshack/dem/CDemVRT.{h,cpp}` and its base class
`src/qmapshack/dem/IDem.{h,cpp}` (2026-06-22). Tackle one at a time; check off when done
and remove the detail once fixed (or leave a one-liner if worth remembering long-term).

### Bugs

- [x] **Opacity boost inverted at high opacity** — `CDemVRT.cpp:447`. Fixed: now
  `qreal o2 = qMin(o1 + 0.4, 1.0);` instead of falling back to plain `o1` when
  `o1 + 0.4 >= 1.0`.

- [x] **Destructor closes GDAL dataset without taking `mutex`** — `CDemVRT.cpp:196-201`.
  Fixed: destructor now does `QMutexLocker lock(&mutex);` before the `GDALClose()` calls,
  matching `draw()`/`getElevationAt()`/`getSlopeAt()`.

  Follow-up investigation on "can the draw thread actually be deleting-while-drawing":
  traced `CDemDraw::drawt()` (`CDemDraw.cpp:348-365`) and `CDemItem::activate()`/
  `deactivate()` (`CDemItem.cpp:167-243`, where `delete demfile` actually happens in
  normal use) — both already take the static `CDemItem::mutexActiveDems`
  (`QRecursiveMutex`), and `drawt()` holds it for its *entire* iteration over all active
  DEMs, not just per-item. So in the exercised (de)activation path, the draw thread can
  never be mid-`draw()` on an item while it's being destroyed — `deactivate()` blocks on
  that mutex until `drawt()`'s current pass finishes. There's no explicit "stop the
  thread" signal/API needed for this case; `mutexActiveDems` already serializes it. The
  new `CDemVRT::mutex` lock in the destructor is defense-in-depth for the class's own
  invariant, independent of callers using `mutexActiveDems` correctly.

  One real gap found along the way (separate, not fixed yet): `CDemList::slotRemove()`
  (`CDemList.cpp:253-265`) deletes a `CDemItem` directly via
  `delete treeWidget->takeTopLevelItem(index)`, bypassing `deactivate()` and
  `mutexActiveDems` entirely. `~CDemItem()` is empty and never touches `demfile`, so this
  can't trigger the dataset-close race — but if the removed item ever had an active
  `demfile`, the `CDemVRT` (parented to `CDemDraw`, not to `CDemItem`) would just leak
  rather than crash. `actionRemove` is only enabled for `eStatus::Missing` items, which
  per `CDemItem::loadConfig()` never reach activation, so this looks unreachable today —
  add to the list as low-priority hardening (call `item->activate(false)` before delete)
  if we want to be defensive.

- [x] **Bounding box from wrong corner pairs** — `CDemVRT.cpp:326-329`. Fixed: now takes
  `std::min`/`std::max` over all four transformed corners per axis instead of just the
  nominally adjacent pair, so a rotated geotransform or skewing reprojection can no
  longer move the true extreme corner outside the computed bbox. Needed `#include
  <algorithm>`.

- [x] **Error message describes a check that doesn't exist** — `CDemVRT.cpp:73-98`.
  Fixed: "raster count != 1" and "band is null" now say "DEM must have exactly one
  raster band:"; added a real `pBand->GetRasterDataType()` check (`GDT_Int16`,
  `GDT_UInt16`, `GDT_Int32`, `GDT_UInt32`, `GDT_Float32` — i.e. literally 16-bit or
  32-bit, no 8-bit `Byte` and no 64-bit `Float64`) that now actually owns the original
  "DEM must have one band with 16bit or 32bit data:" message. Reads still request
  `GDT_Float32` from GDAL regardless of source type, so this is a sanity gate on
  plausible elevation encodings, not a requirement of the I/O path itself.

- [x] **`GetFileList()` not null-checked** — `CDemVRT.cpp:46-48`. Fixed: loop condition
  is now `while (fileList != nullptr && fileList[n] != nullptr)`. A null list now leaves
  `n == 0` (no files to verify, not an error) and falls through to the existing checks;
  `CSLDestroy(nullptr)` is already documented-safe.

### Readability / maintainability

- [x] **Duplicated bilinear-interpolation math** — fixed: added
  `static qreal IDem::bilinear(a, b, c, d, x, y)` (`IDem.h`/`IDem.cpp`) and reused it in
  both `CDemVRT::getElevationAt` (`CDemVRT.cpp`) and the 4x4 branch of
  `IDem::slopeOfWindowInterp` (`IDem.cpp`), replacing the 9 dense hand-expanded lines.

- [x] **Macros leak past their function** — fixed: `ZFACT`/`ZFACT_BY_ZFACT`/`AZ` are now
  `constexpr qreal zFactor`/`zFactorSquared`/`azimuth` (compile-time, since `DEG_TO_RAD`
  is a literal); `SIN_ALT`/`ZFACT_COS_ALT` are `const qreal sinAltitude`/
  `zFactorCosAltitude` (not `constexpr` — they call `qSin`/`qCos`, which aren't constexpr
  functions). Bonus: these are now computed once per `hillshading()` call instead of once
  per pixel, since the macros were re-expanded (and re-evaluated) at every use inside the
  pixel loop.

- [x] **Dead code** — fixed: removed both.
  - `fillWindow4x4()` template (`IDem.cpp`) — confirmed zero call sites anywhere in the
    repo, deleted.
  - `using IDem::drawTile;` (`CDemVRT.h`) — confirmed `CDemVRT` is the only `IDem`
    subclass (`CDemWCS` extends `CDemVRT`, not `IDem`) and `IDem::drawTile()` had no
    call sites at all, so removed the `using` plus the now-unused `IDem::drawTile()`
    declaration/definition itself. `drawTileLQ()` (`IDrawObject`) stays — it's still
    used by the unrelated `IMap::drawTile()`.

- [ ] **Duplicated GDAL progress-callback lambda** — `CDemVRT.cpp:107-110` and `392-395`,
  identical bodies. Factor into one static helper. Also: the lambda parameter is named
  `dem`, shadowing the `IDem::dem` member — rename for clarity.

- [ ] **File-existence-check loop is hard to follow** — `CDemVRT.cpp:44-62`
  Mixes a `#ifdef Q_OS_WIN32` fallback with an `n = -1; break;` error-signaling pattern.
  Pull into a static helper (`allReferencedFilesExist(dataset, missingFile)`) so the
  constructor reads as "open → validate → set up warp → set up projection."

- [ ] **Inconsistent cast style** — `CDemVRT.cpp:37`
  `(GDALDataset*)GDALOpen(...)` uses a C-style cast while the rest of the file uses
  `GDALDataset::FromHandle(...)`.

- [ ] **Leftover `qDebug()` spam** in the constructor (separator lines, cryptic `"FF"`/
  `"RR"` tags for the transforms) — fires for every DEM loaded. Remove or move behind a
  `QLoggingCategory`.

- [ ] **Undocumented encoding convention** — `IDem.cpp:162-170, 206-214`
  `slotSetFactorHillshade(int)`/`getFactorHillshading()` encode UI slider values as
  "negative → 1/|f|, 0 → neutral, positive → direct multiplier" with no comment
  explaining why.

- [ ] **Unexplained `QThread::msleep(100)`** — `CDemVRT.cpp:293`
  No comment explaining it throttles the draw thread when this DEM has nothing to
  contribute this frame — easy to mistake for forgotten debug code.

- [ ] Minor: `factorHillshading = 0.1666666716337204` (`IDem.h:200`) is a float-rounded
  literal of 1/6 — `1.0 / 6.0` reads better, no behavioral difference.

---

## Memory

Store all project-specific learnings, feedback, and notes in this file rather than
the auto-memory system (`~/.claude/projects/.../memory/`). Append new entries under
the relevant existing section, or create a new `###` subsection here when nothing fits.

Do not run `cmake --build` or any build command — Oliver builds externally.

Do not add `Co-Authored-By` lines to commit messages.

### CMapItemDelegate — forward declaration pitfall

`animations_t` is defined after `getAnimations()` in the private section. The forward
declaration `struct animations_t;` before `getAnimations()` is required — do not remove it.


