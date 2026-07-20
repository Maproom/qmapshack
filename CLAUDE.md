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
delegates render at device resolution (`QIcon::paint(rect)`) and non-GIS tree icons stay crisp on
HiDPI. Their sources are SVG: all ~11 project types, both device icons, geo-search error, the 7
`CDBFolder*` types, and `CMapItem`/`CDemItem` (which do not rasterize to a 48 px cache).

**GIS items (wpt/trk/rte/area) stay raster** — Task 2 owns them. `paintItem` in `CWksItemDelegate`
and `CDBItemDelegate` pulls the pixmap back out of the `QIcon` and stretches it.

**Two delegate paths cannot assume the icon can fill the cell, and both branch on `actualSize`:**
`paintDevice` (MTP subclasses overwrite the SVG with a raster read off the device) and
`paintGeoSearch` (`CGeoSearchConfig::getCurrentIcon()` returns an SVG-backed `QIcon`, but
`CGeoSearch::setIcon()` *composes* a raster when "accumulative results" is on). Do not collapse
either to a bare `QIcon::paint`, and do not add a stretch back: `actualSize(rect) == rect` answers
it, and a raster larger than the cell needs no stretch — `QIcon` downscales rasters fine.
`paintProject`/`paintGeoSearchError` are genuinely SVG and paint direct.

**`IDBItem::icon` holds folder icons only.** The DB blob raster lives on `CDBItem` as its own
`QPixmap displayIcon` (`getDisplayIcon()`), so `CDBItemDelegate::paintItem` stretches it directly.
Do not re-add a `setIcon(const QPixmap&)` overload on `IDBItem`: it existed only so the delegate
could unwrap the pixmap straight back out via an `actualSize(1024, 1024)` guess. `IGisItem::showIcon()`
composes from the raw pixmap stashed in `displayIcon` (a COW copy); `displayIcon` stays a `QPixmap`
for the DB blob.

Why this is safe despite serialization (it stores sym/colour, not the icon): see "GIS item icons —
serialization…" below.

**Do not persist a themable icon.** Three places write icons to disk and all three deliberately
keep the `.svgt` form out of the saved data — a `.svgt` path is unreadable by a build without the
icon engine, and a rendered icon freezes the theme it was drawn in: web search services
(`CGeoSearchWeb::displayIconPath`, resolved at draw time only), history events
(`IGisItem::migrateIconPath`, pinned to the 48x48 PNG), and the database, which stores the display
icon as PNG **bytes** (`CDBProject.cpp`) so it cannot follow the theme at all. One deferred
decision, three sites — settle them together; see ".notes/icons-task1-ui-svg-plan.md", "Persisted
icons".

---

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

**The format is not the lever; the rasterization hook is.** A `QPixmap` is one raster frozen at the
dpr live when it was built: it cannot serve a larger request, and cannot follow a window to another
screen — whatever the source format. Measured (Qt 6.9.2): a `QPixmap` of an SVG reports
`actualSize` 35, the same file as a `QIcon` reports 1024.

- **Anything taking a `QIcon`** — buttons, actions, tree/list items, a Designer `<iconset>`:
  reference `:/icons/Foo.svg` and let the paint path ask for the size. Crisp at any dpr, no hook.
- **A static icon in a dialog** — `QSvgWidget` + `load()`. Qt ships no widget that displays a
  `QIcon`, and `uic` bakes a `<pixmap>` into `QPixmap(path)` before any widget sees the path.
- **Canvas rasters** (waypoints, POI, cache) are data, not icons — Task 2 owns them.
- **Rich text `<img src=>`** — see below; `width`/`height` is the trigger.

`cmake/IconGate.cmake` fails the build on the three regex-detectable violations. It deliberately
cannot see `QIcon(pixmapVariable)` or a `.pixmap(w,h)` missing its dpr — catching those would mean
false-flagging every `QIcon(path)`.

**Known, left alone (Oliver): three `.pixmap(w,h)` calls with no dpr.** `CSelectCopyAction.cpp:48/50`
(project icons — `getIcon()` is already an SVG-backed `QIcon`, and `.pixmap(32, 32)` discards that on
the last line) and `CInvalidTrk.cpp:34` (`QStyle::standardIcon`). `QSvgWidget` cannot take these:
they hold a `QIcon`, not a path. `pixmap(size, devicePixelRatioF())` fixes the blur but is wrong
twice over — in a ctor the widget has no window yet, so the ratio falls back to the primary screen,
and it goes stale if the dialog moves screens. Doing them properly wants a small `QIcon`-painting
widget. Three modal-dialog icons did not justify it; that is the whole reason, and it does not
generalise. `CSelectCopyAction.cpp:31/33` is a different thing — `getDisplayIcon()` is a `QPixmap`
by Task 2's design.

Two beliefs recorded here were themselves the bug, and are why blurry icons kept coming back: that
a `QLabel` `<pixmap>` or an HTML `<img>` is *a reason to keep a PNG*. `9e3499c5` pointed 58
`<pixmap>` refs at `.svg` and every one stayed blurry. Full measurements: `.notes/icons-png-audit.md`.

#### Rich text `<img>` — `width`/`height` is the trigger

Rich text rasterizes an image **once at its natural size**, then scales that raster to
`width × height × dpr`. Both halves matter:

- **No `width`/`height`** → natural size *is* the layout size, nothing to scale, so the render is
  bit-identical to a nearest-2× upscale at dpr 2 (**diff 0.000**). This is the measurement behind
  the old "rich text has no HiDPI path at all" claim — it only ever held for a bare `<img>`.
- **With `width`/`height`** → the scale target is dpr-aware, so HiDPI works.

Measured on `ActFoot` (Qt 6.9.2, mean diff vs a vector render at the device size; <3 ok, >8 visible):

| `<img>` | dpr1 | dpr2 | dpr3 | themeable |
|---|---|---|---|---|
| `width=16 height=16 src=.svg` | **8.447** | 3.403 | 2.382 | **yes** |
| `src=16x16/*.png` (bare) | 0.418 | 10.841 | 12.962 | no |
| `src=16x16/*.png` + `@2x` sibling | 0.418 | 0.427 | 6.694 | no |
| `width=16 height=16 src=32x32/*.png` | 10.766 | 0.427 | 6.694 | no |

**The SVG is worse at dpr 1** (a 60→16 downscale of the natural-size raster, poor filter) and better
above it. It is chosen anyway because it is the **only themeable option** — Qt has no recolour API,
so theming means rewriting the SVG bytes.

**Do not try to tune the dpr-1 regression away — it was measured out.** The error is zero only where
`logical × dpr` *equals* the natural size, i.e. where no scaling happens (a 64px SVG at `width=32`
on dpr 2 scores 0.001). Everything else scales, and the filter is bad at any ratio: re-canvasing
`ActFoot` to a natural 64 makes `width=16` an exact 4:1 downscale and it still scores **8.730**, no
better than the ragged 60px original's 8.447. Nor does a different `<img>` size fix it (16→8.447,
22→6.525, 24→4.157 — better, but never near the PNG's 0.418, and it changes the design).
One SVG cannot serve dpr 1 and dpr 2 here: crisp at dpr 1 needs natural 16, crisp at dpr 2 needs 32.
`addResource`/`setResourceProvider` cannot rescue these either — `getInfo()` output also goes to
`QLabel::setText` (private doc) and into the DB.

`@Nx` support is per-class, not global (measured): rich text **yes**; `QIcon` **yes**, but the lookup
is gated on the app's *live* dpr, so `pixmap(32, dpr=2)` under `QT_SCALE_FACTOR=1` returns 32, not 64;
`QPixmap`, `QPixmap::load`, `QImageReader` **no**, at any scale factor.

**`getInfo()` output is persisted**, so an `<img>` path in it is a data format, not just markup:
`CDBProject.cpp:297/341` writes it to the DB `comment` column, `CDBItem.cpp:29/41` reads it back and
re-renders it, and `CDBFolderSqlite.cpp:65` full-text indexes it. Changing a path leaves old rows on
the old icon (mixed until re-saved) and means the `16x16` PNGs must stay registered forever — the
same shape as the history-event icons.

**No icon PNG is ever pruned, and no dead qrc entry is removed** (Oliver). For the two populations
below the reason is **correctness** — they store an icon *path*, not a picture, so a pruned PNG is a
blank icon in somebody's existing file, which makes the dropped Phase 6.4 prune load-bearing rather
than merely cautious. For everything else — the ~79 qmaptool entries nothing references, the 14
bullet PNGs the `colorMap[].bullet` removal orphaned, the `48x48`/`32x32` rasters whose call sites
became SVG — the reason is simply that a few KB of qrc is not worth the chance of being wrong about
who reads a path. Do not "tidy" these.

| population | stored in | migrated on read? |
|---|---|---|
| `history_event_t::icon` | `.qms` (`gis/qms/serialization.cpp:128/141`), `.gpx` (`gis/gpx/serialization.cpp:261/154`), DB `data` column | **yes** — `IGisItem::migrateIconPath()` |
| `getInfo()` HTML (`<img src>`) | DB `comment` column | no — see the TODO below |

**`migrateIconPath()` is the pattern to copy.** It rewrites a `48x48/*.png` history path to its SVG
as the record is read: nothing is marked changed (the event hash covers `event.data`, not the icon),
so the list is uniform immediately and the corrected path reaches disk only if the item is saved for
some other reason. A path with no registered SVG is returned untouched, so an un-converted icon
still renders. Both call sites are reads — the `QDataStream` operator covers `.qms` *and* the DB.

**TODO — the `comment` column** still holds `<img src='://icons/16x16/*.png'>` from `getInfo()`, so
old DB rows render the old icon until re-saved. Same fix shape, but it is HTML in a column rather
than a bare path, and it is full-text indexed (`CDBFolderSqlite.cpp:65`) — rewriting on load is
tractable, nothing depends on it happening.

**`IGridPlacer.ui`'s `line_3px_*` rules stay as they are — decided (Oliver), reason not recorded.**
The four `QLabel`s are hardcoded black 100×3 / 3×100 PNGs, so dark mode gives 1.26:1 contrast.
`QFrame::HLine`/`VLine` would theme itself from the palette and drop both assets, but draws a 1px
etched line against their 3px. Proposed on that basis and declined; the dark-mode contrast is known
and accepted.

**`CGeoSearchWeb`'s service icons stay PNG — decided, do not "finish" this menu.** The submenu is
knowingly half-converted: chrome is SVG, the service table stays `://icons/32x32/SearchWeb*.png`.
The path is *user data* — the dtor writes it to QSettings and the ctor rebuilds `services` from it,
so editing `defaultServices()` reaches new installs only. And `defaultIcon` equality is the
"is this user-added" test in `CGeoSearchWebConfigDialog::slotReset()`: with a stored PNG path and an
SVG `defaultIcon`, "Restore default list" **erases the user's own services**. Converting needs a
settings migration plus an `isUserDefined` flag so identity stops riding on the icon string; Oliver's
call is that a uniformly blurry submenu beats that risk. (The erase path is read from the source, not
reproduced — confirm with a hand-added service before relying on it for anything but this decision.)

**`src/icons/mkicon` renders ONE svg to PNG**, and is the only thing in the tree that calls
inkscape:

```bash
./mkicon Apply.svg 32 48           # -> 32x32/Apply.png, 48x48/Apply.png
./mkicon --hicolor QMapShack.svg   # -> qmapshack/hicolor/<N>x<N>/apps/QMapShack.png
```

**When you edit a UI icon's artwork, regenerate its PNGs (existing sizes) and commit them with the
svg** — good practice even though the app draws the `.svgt` via `QIcon` and the rasters are not
really used today (Oliver). It keeps the baseline honest, and it is what `uidiff.py` diffs against.
`mkicon` is inkscape, not a cmake/make build, so running it is fine.

It replaced the old top-level `makeicons`, which rendered *every* svg on every run. That made a
raster exist whether or not anything wanted one, and it let sources drift from rasters unseen:
by the time it was measured, running it would have corrupted 35 of 327 icons.

`waypoints/makeicons` and `cache/makeicons` remain batch scripts and call `mkicon`. Batch is
correct there — every source in those directories has a raster by intent, since both sets are
frozen-appearance PNG by design.

**`hygiene.py`'s green run is not proof of repair outside `src/icons`.** Its skip rule is "has a
`viewBox` ⇒ already fitted by `naturalsize.py`", which holds in `src/icons` and misfires elsewhere:
an Inkscape **A4 page** carries a viewBox too. Measured on `src/qmapshack/pics/` — both files are
210×297mm pages, and `hygiene.py` fits `compass.svg` (no viewBox) to 47.5×48.3mm while **silently
skipping** `DockWidgets.svg` (viewBox `0 0 744.09 1052.36`, i.e. A4 in px), then prints
*"2 files processed, 0 failures"*. Point it outside `src/icons` and it needs a real guard —
compare the viewBox to the drawing bbox rather than test for its presence.

### Icons — why the SVGs don't render in Qt (the `-D` / missing-viewBox trap)

**Do not retry "just load the SVGs directly" before reading this.** It has been tried and it
fails for a non-obvious reason that is *not* a Qt limitation.

`makeicons` exports with `inkscape -D`, which crops to the **drawing's bounding box** and ignores
the page. Independently, **314 of 325 `src/icons/*.svg` have no `viewBox`** — they declare a
64×64 page while the artwork sits in an arbitrary sub-rect. So Inkscape crops tight and the PNG
fills the frame; Qt honours the page and draws the artwork small and off-centre.

Measured with `QSvgRenderer` at 32px vs the shipped PNGs (324 icons — the 325th,
`SelectEndPoint.svg.2020_04_11_11_11_42.0.svg`, is a committed Inkscape autosave backup and
should just be deleted): **151 severe today → 19 after fitting each canvas to its drawing → 1
after also converting text to paths.** The sources are the problem, not the renderer.

Also: **23 files carry an empty `<flowRoot>`** (Inkscape flowed-text box, zero content) whose only
effect is inflating the drawing bbox — `object-to-path` does not remove it, strip it first. 94
files reference fonts by name that may not exist on a user's machine.

**`QIcon(":/x.svg")` needs two runtime plugins**, and they are not interchangeable:
`imageformats/qsvg` alone gives one fixed raster at the SVG's natural size (scaled from there —
blurry), while `iconengines/qsvgicon` renders at any requested size. Both must be deployed or the
HiDPI benefit silently disappears. **All three platforms are confirmed OK** — Linux via the distro
packages, macOS via `macdeployqt` auto-detection, Windows confirmed by Oliver. Do not re-open this
from `msvc_64/copyfiles.bat`: it copies `qsvg.dll`, never mentions `iconengines`, and is dead
anyway (all-Qt5, untouched since 2022) — the live Windows packaging lives outside the repo.

**Qt has no SVG recolouring API** (verified against Qt 6.9.2 and 6.10.0): `QSvgRenderer` exposes
no `setCurrentColor` and no palette, and neither does `QIcon`. Dark-mode icon theming requires
rewriting the SVG text ourselves and loading via `QSvgRenderer(const QByteArray&)`.

**But Qt's renderer DOES resolve `currentColor`** — measured on 6.9.2 via `.notes/icons-tools/
ccheck.cpp`; the widespread "QtSvg ignores currentColor" claim is false. Only the *setter* is
missing, so "rewrite the bytes" means setting one root `color=` attribute, not parsing paths.
**A fill that is not `currentColor` is untouched**, so semantic red/green need no carve-out.
This is the basis of phase 7a — read the plan's Phase 7 before touching icon theming.
Traps: `currentColor` with no `color` set renders **black** (in Qt *and* inkscape, so a themed
source without a root default silently exports black PNGs from `mkicon`); a **duplicate** `color=`
makes the document invalid and renders **nothing**; lowercase `currentcolor` is black
(QTBUG-46947); a `<style>` class beats a root `color=` (so the KDE/Breeze idiom does not mix with
ours); Qt 6.10.0 has a `currentColor` regression fixed in 6.10.1 (QTBUG-141102).

**Qt renders `<marker>` but ignores `markerUnits="strokeWidth"`** (measured, Qt 6.9.2). That is
the SVG *default* when the attribute is absent, which it always is in Inkscape output: the arrow
is meant to scale by the referencing path's stroke width, which is why Inkscape's stock arrows
carry a `scale(0.4)`-style inner transform to be multiplied back up. Qt draws the marker at raw
size instead — two lines of stroke-width 1 and 8 sharing one marker get **identical** arrowheads.
So the arrowhead is the one part of a drawing whose size comes from stroke width, and the one part
that does not scale with the rest. This is a Qt bug, **not** an SVG problem: Inkscape and browsers
are correct. Reported externally on PR #1159 as "arrowheads far too small". `markerUnits=
"userSpaceOnUse"` with a pre-baked scale does **not** work either — Qt renders it clipped and
misplaced.

**Fix: bake markers into geometry** with inkscape's `object-stroke-to-path` (the action is
`object-stroke-to-path`; `stroke-to-path` does not exist, and `object-to-path` does **not** touch
markers — that is why the earlier hygiene pass left them). Follow with `vacuum-defs` or the
orphaned `<marker>` defs stay behind (+62% file size instead of +20%). Done for all 12 UI icons
that had live marker refs; **0 live marker refs remain**.

**Trap — `object-stroke-to-path` turns a zero-width stroke into a filled path of the whole
shape.** `stroke:#000000;stroke-width:0` paints nothing before the conversion and a solid black
rectangle after it. Hit for real on `CSrcDepth`/`CSrcCAD`/`CSrcCourse`, whose background rect
carried exactly that; all three came out as solid black squares, and the correlation was exact —
the only 3 files of the 11 with a zero-width stroke were the only 3 that failed. Set `stroke:none`
wherever `stroke-width` is 0 **before** converting.

**Grep for `marker-*:url(...)`, not for `<marker`.** 67 UI icons carried a `<marker>` *definition*;
only 11 ever referenced one. The other 56 were dead defs that rendered nothing. Counting defs
overstated the problem 6× — a def is not a use. Both are cleaned now: **0 marker defs and 0 live
refs remain** in `src/icons/*.svg`. The 56 needed `vacuum-defs` only (no `object-stroke-to-path`,
so their strokes stay strokes and stay editable) and shrank 310,462 -> 133,378 B, **-57%**. Dead
Inkscape defs are worth vacuuming on sight; they were over half the bytes of every file carrying
them.

**The two icon gates are not interchangeable.** `uidiff.py` renders with **inkscape**, so it
answers "is the artwork still the same" and is blind to every Qt bug on this list. `svgdiff`
renders with **Qt**, and is the only one that sees them. The marker fix needs both: `uidiff` must
stay 329/329 identical (it did), `svgdiff` proves the win (visible 2 -> 0). A meanDiff is also a
poor detector here — an arrowhead is a small share of an icon's area, so the worst offender
(`AreaMove`) scored only 7.4 while being obviously wrong to a human. Look at the render.

**Qt's `QIcon` already caches rasterization internally** — measured 1.13 µs for a cached SVG icon
vs 0.81 µs for a cached PNG. There is no icon cache worth building. Constructing a `QIcon` per
paint does cost (47 µs PNG / 127 µs SVG), but **do not `static` a paint-path icon to avoid it.**
There are no `static` icons in the tree; keep it that way:

- A `static` is built on first paint and pins the colour scheme live at that moment — which
  defeats the dark/light theming the SVG work exists for. Qt has no SVG recolouring API, so
  theming means loading rewritten bytes per scheme.
- The cost it buys back is unproven. The tree shows a handful of rows and repaints on
  hover/selection/scroll, not continuously, so an occasional 11 ms is invisible. A per-repaint
  figure is not a reason to optimise — establish repaint *frequency* first.
- It also hides icons from a path-shaped grep: a bulk swap matching `QIcon("` cannot see
  `QIcon iconShowAll("...")`. Hoisting a path behind a variable name is how a delegate icon gets
  silently left on PNG. **This is not hypothetical — it happened.** The Phase 5.3 sweep missed
  `QIcon(strIcon)` in the three realtime sources and `QIcon(groupIcon)` in `CDetailsTrk`, and the
  realtime ones were blurry *at dpr 1* (a 48px raster in a 64px cell) until they were found by
  walking call sites instead of grepping paths. Both are `.svg` now. `cmake/IconGate.cmake` cannot
  see this shape either.

If a repaint ever does prove hot, key a cache by *name plus colour scheme* so a theme change can
invalidate it — never a bare `static`.

**`actualSize(rect) == rect` is the test for "can this icon fill the cell"** (measured, Qt 6.9.2):
an SVG-backed `QIcon` returns the requested size at any size, a raster clamps to its natural size
once the request exceeds it (32px source: `actualSize(43)` -> 32). Use it where an icon member may
hold either format — a raster already *larger* than the cell still reports a fill and needs no
stretch, since `QIcon` downscales rasters fine (fact 2 is about *up*scaling only).

### Icons — UI icons and waypoint icons are two separate tracks

Decided with Oliver. They are different asset classes and get **opposite** treatment. **Read the
full plans before touching either:** `.notes/icons-task1-ui-svg-plan.md` and
`.notes/icons-task2-waypoint-resolution-plan.md`. Measurement tools in `.notes/icons-tools/`.

- **UI icons** (`src/icons/*.svg`) are decoration: they sit on the UI palette, may change freely,
  and are the ones unreadable on dark schemes. They become SVG. Task 1 — **phases 0–6 done**;
  phase 7 is split into **7a theming** (in progress) and **7b artistic overhaul** (needs an
  artist). **Dark mode is not a background problem** — 262 of 329 icons are already transparent
  and only `TrkProfile.svg` has a white background; the problem is the house navy `#000080`
  (1246 uses) as the *drawing* colour. See the plan's Phase 7 before touching it.
- **Waypoint icons** (`src/icons/waypoints/`) are **data**: drawn on map tiles (so dark-mode
  theming does not apply — they must stay readable over any map), named by the GPX `<sym>`
  vocabulary shared with Garmin, and their **exact look is a frozen contract** (Oliver: "commonly
  accepted and expected"). **They stay PNG on the canvas.**

  **But the freeze is about map rendering, not about the file.** A waypoint symbol used as *UI
  chrome* — a menu action, a tool button — is under none of that constraint and should use the
  SVG (decided with Oliver). Four such sites exist and are now SVG: `CGisListWks.cpp` ("Change
  Icon..."), `IScrOptRuler.ui`, and two in `IScrOptSelect.ui` — one of which had been pairing
  `WaypointOn.svg` (on) with `FlagBlue.png` (off) inside a single iconset. Only `FlagBlue.svg` and
  `PinBlue.svg` are registered in `resources.qrc`; add others as UI actually needs them, rather
  than registering all 203.

  The 203 sources were **not** hygiened by task 1 (which only covered `src/icons/*.svg`). Fixed:
  `hygiene.py --dir src/icons/waypoints` repaired the **24 that had no `viewBox`** (the other 179
  were already sized and are skipped — re-hygiening would strip their viewBox). It fits the canvas
  to the drawing rather than adding a viewBox, so those 24 still have none and that is correct —
  grep `viewBox` and you will still count 24, which is not a sign it failed; `svgdiff` is the
  check. They were exactly the unrenderable ones: mean diff vs their PNG **38.7 → ~0.2**, versus
  2.8 for the 179. Before
  the fix `PinBlue`/`FlagBlue` drew ~⅓ too small and offset, so registering a waypoint SVG without
  hygiene ships the very bug PR #1159 was reported for. Gate any such change with
  `wptdiff.py --size 96` — it must stay **123/61/19/0**; `makeicons` exports with `-D`, which
  crops to the drawing and ignores the page hygiene rewrites, so the shipped PNGs do not move.

Why waypoints stay raster — the constraint is itself the argument: SVG would hand rendering from
Inkscape to Qt, so appearance would become a function of whichever Qt version the user has, and a
Qt antialiasing change could silently restyle accepted iconography. PNG freezes the look under a
renderer we control. Also: external user icons are PNG/BMP forever
(`CWptIconManager.cpp:2010`), so the raster path must exist regardless — SVG would add a second
path, not remove one. And SVG is not smaller: the 203 waypoint SVGs are **737,383 bytes** on
disk vs **157,873 bytes** of shipped 32px PNG (`cat * | wc -c` — never `du`, it rounds each tiny
file up to a 4K block and so measures the filesystem, not the data).

The waypoints' real defect is resolution, not format: source is 32px, the size slider is 16..48
(`IMapIconSizesSetup.ui`), and the canvas is dpr-aware — so at the **default** 22 on a HiDPI
screen they are already upscaled (needs 44 device px, has 32). Fix is 96px in
`waypoints/makeicons`, which changes nothing visually — **reproduce with
`.notes/icons-tools/wptdiff.py --size 96`**, which must report `visible (>8) == 0` (currently
123 identical / 61 antialiasing / 19 minor / **0 visible**).

**`icon_t::focus` is absolute pixels of the *loaded* raster** — hardcoded against 32 for the
built-in table (314× `{16,16}`, 6× `{0,32}` for flag/pin tips, 1× `{0,31}` for `"Flag"`, **which
Oliver confirmed is a bug to fix**), but derived from `QImage::offset()` or the image centre for
external user icons of any size (`CWptIconManager.cpp:2056-2074`). Any resolution change silently
breaks every anchor until focus is stored relative (0..1). That refactor is a prerequisite for
touching waypoint resolution at all — and note there are **two** getters:
`getWptIconScaledByName` is the one holding `focus = focus * scale` (`:2115`), not
`getWptIconByName`.

`focus` **is** written to `.qms` and to the DB `data` column (`gis/qms/serialization.cpp:406`),
but it is **overwritten on every load** — the stream operator itself calls `deriveSecondaryData()`
(`serialization.cpp:745`), which runs `updateIcon()` per point (`CGisItemRte.cpp:206`). The bytes
are write-only, so a relative-focus change needs **no migration**. Both directions of this have
been got wrong; see the Task 2 plan before concluding anything about it.

### TODO — POI icons (SJJB) can be converted; the SVGs are already in the tree

**POIs are not waypoints** (Oliver): no canvas-freeze applies, so this is a real win, parked only
for size. Do not re-derive the following — it is measured.

**The sources are shipped and nobody registered them.** 303 SVGs under `src/icons/poi/SJJB/svg/
<category>/`; **249 of the 250** referenced PNGs have a counterpart; **0** are in `resources.qrc`,
which is why "there are no SVGs for these" is the natural belief. Qt renders their geometry
**exactly** — the shipped PNG and a 32px render of the SVG agree on opaque pixel count to within
antialiasing.

**Only the fill differs.** The SVGs are SJJB templates with a placeholder `#111111`; their build
recolours per category. Swap the paths and you get a map of near-black icons. The colour is
recoverable from the shipped PNGs (16 categories, 10 consistent, 6 with 2–3 variants — so per-icon
extraction is safer than per-category). Recolouring means rewriting the fill and loading via
`QSvgRenderer(QByteArray)` — the same mechanism phase 7a needs anyway.

**The work:** register ~250 SVGs · a recolour step · change `CPoiIconCategory`'s `QPixmap baseIcon`
and `QMap<QString, QPixmap> subCategories` to paths, plus ~302 literals in `CPoiFilePOI_TagMap.cpp`
(scripted, but the path *shape* changes — `png/<cat>_<stem>.n.32.png` → `svg/<cat>/<stem>.svg` —
so it needs a lookup table, not a regex) · a render cache keyed by (icon, size, dpr).

**The cache is a speed win regardless of icons:** `CPoiFilePOI.cpp:204` currently `.scaled()`s a
pixmap **per POI per repaint**.

**Two defects found while scoping:** `CPoiFilePOI_TagMap.cpp:118` asks for
`health_pharmacy_dispencing` — a typo; the SVG is `pharmacy_dispensing`. And one SVG embeds a
raster that is not in the tree (`pastedpic_10102008_233747.png`), so it will not render.

**Gate it or do not do it:** render each recoloured SVG at 32, diff against its shipped PNG, require
`visible (>8) == 0` — the mechanised version of "appearance must not change", as `wptdiff.py` does
for waypoints. **A diff that measures colour will report ~240 false failures** (the placeholder fill);
that mistake has already been made once.

### GIS item icons — serialization stores sym/colour, not the icon (so "must be raster" is wrong)

Do not repeat the belief that GIS item icons must stay PNG because they are serialized. What the
`.qms`/DB persist is the **symbol name** (waypoint `wpt.sym`, `serialization.cpp:86`) and the
**colour** (track/area `trk.color`/`area.color`, `:614`/`:827`) — never a rendered icon. The icon
is **re-derived on load**: waypoint via `getWptIconByName(sym)` (`CGisItemWpt.cpp:424`); track/area
by loading `Track.png`/`Area.png` **as a shape mask** and filling it with the data colour
(`CGisItemTrk::setIcon` / `CGisItemOvlArea::setIcon`, `createMaskFromColor(Qt::transparent)`).

A rendered pixmap is stored only in two **output caches**, neither a source of truth: the DB
`items.icon` BLOB (`getDisplayIcon().save(PNG)`, `CDBProject.cpp:283`, for quick DB-tree access) and
`.qms` `history_event_t.icon` (undo/redo snapshots). A cache holds output — render an SVG to a
pixmap and `save(PNG)` identically — so **serialization does not constrain the source format.**

Consequence: only the **waypoint** symbol is a genuinely frozen raster (canvas-freeze reason, not
serialization). **Tracks and areas can be SVG** — the PNG is used only as a silhouette mask, so an
SVG rendered at the target size gives a crisp mask and the same data colour (and beats today's
1-bit `createMaskFromColor` edges). Converting them belongs to **Task 2**, which now owns all
GIS-item icons; the full mechanism is in `.notes/icons-task2-waypoint-resolution-plan.md` under
"GIS item icons: what is actually serialized". The one real constraint: `QIcon` will not upscale a
raster (`qtfacts` fact 2), so the fix renders the SVG at size — it does not wrap the old 32px PNG.

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
