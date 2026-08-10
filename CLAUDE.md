# QMapShack — Claude project notes

Qt/C++ desktop app for planning and analysing GPS tracks, routes and waypoints. Loads map tiles,
vector maps and DEM data; supports online and offline routing engines.

---

## Stack

- **Language:** C++20
- **GUI / framework:** Qt 6.8+
- **Key libs:** GDAL, PROJ 8+, Routino
- **Build:** CMake 3.20+, Ninja; Debug build in `build/`, binaries in `build/bin/`
- **Bundled 3rdparty:** Garmin FIT SDK
- **Minimum GDAL:** 3.10

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

## Working rules

- **Never build.** No `cmake --build`, `ninja` or `make` — not even to verify an edit. Make the
  change, run `clang-format`, report, stop. If a compile is genuinely needed to be sure, say so.
  For reference, the user runs `cmake --build build --target qmapshack -j$(nproc)`.
- **Never commit or push** without being asked for that specific change.
- **No `Co-Authored-By` lines** in commit messages.
- For "what were we working on", read `git status && git diff` first — the live diff is ground
  truth, notes are not.
- Store project learnings in this file, not in the auto-memory system. Append under the matching
  section or add a new `###`. Keep it to forward-usable facts; git holds the history.
- Multi-session plans and specs go in the tracked `.notes/` directory, named `<topic>-plan.md`,
  with a pointer from `## Open work` here. Never invent another location.
- **This checkout (`~/projects/qmapshack`, remote `kiozen/qmapshack`) is where work happens.**
  `~/projects/qmapshack_master` tracks `Maproom/qmapshack` and is for reference only — never
  branch or edit there.

---

## Build system

Target-scoped CMake. Nothing is set at directory scope except the MSVC options below.

- **Never touch the translation block's gating.** `UPDATE_TRANSLATIONS` off means no lupdate target
  exists at all, so no build can rewrite the tracked `.ts` files; the catalogs are refreshed only
  when translation work is deliberately finished. Qt's always-present `update_translations` target
  is not a substitute. The `/locale` resource in the `else` branch stays too: every translator loads
  `.qm` from a filesystem path, so nothing reads it, but `qt_add_resources(<app> ...)` is what makes
  the `.qm` files inputs of the *application* target. Drop it and `--target qmapshack` stops
  producing the catalogs the packaging scripts copy out of `<build>/src/<app>/`.
- **`qms_options`** is the INTERFACE target carrying the project's warning set. First-party targets
  link it `PRIVATE`; bundled 3rdparty must not. Add flags through `qms_add_flag_if_supported()`.
- **`target_link_libraries` is keyword form everywhere.** Plain and keyword signatures cannot be
  mixed on one target, so a new call must say `PRIVATE`.
- **Dependencies are imported targets**: `GDAL::GDAL`, `PROJ::proj`, `JPEG::JPEG`,
  `ROUTINO::ROUTINO`. `ROUTINO_XML_PATH` stays a plain variable — qmapshack passes it as a define.
- **Defines are per target**: `HELPPATH` on qmapshack and qmaptool, `ROUTINO_XML_PATH` and
  `HAVE_DBUS` on qmapshack. Global on purpose: `_CRT_SECURE_NO_WARNINGS`, `/MP` and `/utf-8` under
  MSVC, which the bundled FIT SDK needs, and `-march=native`.
- **Install paths** come from `GNUInstallDirs` on UNIX and stay relative to the application
  directory on Windows, where `CAppSetupWin` resolves `HELPPATH` against it. Six cache variables
  survive so a packager can override them. `HTML_INSTALL_DIR` is `share/doc/HTML`, not
  `CMAKE_INSTALL_DOCDIR` — changing it moves the installed help.
- **`CMAKE_RUNTIME_OUTPUT_DIRECTORY` pins the four per-config variables too**, or a multi-config
  generator appends `Release/` and the macOS and Windows packaging scripts stop finding the binaries.
- **`ConfigureChecks.cmake` probes through the C++ compiler.** The project enables no C language, so
  `check_include_file` / `check_symbol_exists` hard-fail; use the `_cxx` variants.
- **`CMAKE_AUTOUIC` is OFF** — the `.ui` files are listed explicitly and go through `qt_wrap_ui`.
  `qt_standard_project_setup()` sits directly after `find_package(Qt6 ...)` because it flips
  AUTOMOC/AUTOUIC and appends to `CMAKE_INSTALL_RPATH`.
- **`Qt6::Core5Compat` is required, not legacy.** `QTextCodec` decodes the Garmin codepages
  (`CGarminTyp.cpp`, `IGarminStrTbl.cpp`) that `QStringDecoder` cannot.
- **`CMAKE_CXX_EXTENSIONS` is left ON** — the UNIX warning set passes `-fms-extensions`.
- `CMakePresets.json` holds three usable Linux presets; the Windows and macOS entries are untested
  stubs and the platform blocks in `CMakeLists.txt` still drive those builds. The developer-facing
  howto is `README_PRESETS.md`, linked from the README's Linux build section.

---

## Code style

**All C++ is formatted with clang-format.** After editing any `.cpp` or `.h`:

```bash
clang-format -i <file> [<file> ...]
```

Style is `.clang-format` in the project root (Google base, 120 columns). Always accept its output —
never revert or hand-tune it, and keep reformat hunks it makes on unrelated pre-existing drift.

- **Every control-flow block requires braces**, including single-statement bodies.
- **Doc comments are `/** */` doxygen blocks** (`@brief`, `@param`, `@return`); inline member docs
  are `/**< */`; plain `//` for non-doxygen annotations.
- **Comments state the fact needed to understand the code — one terse line.** Explain a non-obvious
  invariant, workaround or gotcha; never narrate what the code already says, and never the
  ticket/bug/PR history behind it.
- **Pass `QString` and complex objects (`QVector`, `QImage`, …) by `const&`** unless the function
  mutates them. Plain `T&` is for genuine out-parameters only.
- **Prefer static `QFileInfo::exists(path)`** over `QFileInfo(path).exists()` for a bare existence
  check.
- **Prefer Qt fixed-width typedefs**: `qint32`/`quint32`, `qreal`, `qsizetype` for Qt container
  sizes. Exception: match an external API's own types (GDAL's `int*` out-param, `size_t` in
  `ReadRaster()`).
- **Constructor and destructor come first in a `.cpp`**, in that order; every other member function
  follows both.

---

## Architecture: mouse/line editor

`src/qmapshack/mouse/line/`.

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

`IMouseEditLine` delegates all mouse events to the active `ILineOp`. Switching the toolbar button
deletes the current op and creates a new one.

### Routing event-loop re-entrancy — the subsystem's key invariant

`CRouterSetup::calcRoute()` shows a `CProgressDialog` running a nested `QEventLoop`. The full Qt
event pipeline stays live while it spins, so during routing:

- `mouseMove()` fires and drifts `points[idxFocus].coord` to the cursor.
- A right-click reaches `abortStep()` → `restoreFromHistory()`, which reallocates `points` and
  invalidates every saved index or pointer into it.
- A left-click can re-enter `leftClick()`.

**Any `ILineOp` that triggers routing must go through `runRoutingAndPin(coord)` and check its
return value.** It:

1. Pins `points[idxFocus].coord = coord` before routing.
2. Sets `isRouting = true` — subclasses test this at the top of `leftClick()` to block re-entry.
3. Calls `slotTimeoutRouting()` → `finalizeOperation()` → `tryRouting()`.
4. Re-validates `idxFocus` against `points.size()` after the loop returns (an abort leaves both
   unknown).
5. Restores `points[idxFocus].coord = coord`, undoing any drift.
6. Returns `false` if aborted — the caller must return immediately.

### CLineOpAddPoint state

| `isDragging` | `focusIsEndpoint` | Meaning |
|---|---|---|
| false | false | cursor near a mid-segment — click inserts a point there |
| false | true  | cursor near the first or last point — click extends the line |
| true  | either | a new point is attached to the cursor — click drops it |

### Lead lines vs sub lines (vector/track routing)

With vector or track routing active, `updateLeadLines()` finds the map/track polyline nearest the
active point → `leadLineCoord1/2` (geographic) and `leadLinePixel1/2` (screen).
`GPS_Math_SubPolyline()` then extracts the segment lying between the two adjacent line points →
`subLineCoord1/2` / `subLinePixel1/2`. The sub-line is what becomes the routed sub-segment on drop.

---

## Architecture: routers

`IRouter` is the abstract base:

- **`CRouterRoutino`** — offline routing from Routino databases.
- **`CRouterBRouter`** — BRouter, either a local process (`CRouterBRouterLocal`) or the online HTTP
  API.

`CRouterSetup::self()` is a singleton owning the active router and exposing `calcRoute()`. It emits
`sigHasFastRouting(bool)` when the router capability changes; `IMouseEditLine` listens and
enables/disables the auto-routing button.

Only local BRouter supports fast (on-the-fly) routing. Online BRouter and Routino need the whole
route at once via `calcRoute(const IGisItem::key_t&)`.

---

## Elevation smoothing — `CSmoothingSpline`

`helpers/CSmoothingSpline.{h,cpp}`, a penalized regression spline over equidistant nodes. Used only
by `CGisItemTrk::interp`, which feeds the "Interpolate elevation" filter and `CPlotProfile`'s
preview curve.

- **`m` counts nodes, not basis functions**: m − 1 spans, m + 2 coefficients.
- **The penalty is `D2'D2`, the second difference of the coefficients.** The exact curvature Gram
  matrix `∫B''_i B''_j` fits measurably worse — do not swap it in.
- **`lambda` is normalized against `trace(B'B)` and `numSpans^3`**, so it is independent of the point
  count, the node count and the units of x and y. The tuned value is `kElevationSmoothing` in
  `CGisItemTrk.cpp`; doubling it roughly doubles the smoothing.
- **Only points with `ele != NOINT` enter the fit**, appended in order — indexing the input arrays by
  `idxVisible` leaves holes for points without elevation.
- Below ~80 nodes the basis, not the penalty, limits the curve. That is where the quality setting
  changes the result.

---

## Architecture: tree item delegates

- `gis/CWksItemDelegate.{h,cpp}` — workspace tree
- `gis/CDBItemDelegate.{h,cpp}` — database tree
- `map/CMapItemDelegate.{h,cpp}` — map-item tree

### Row layout: CRowBuilder

Every `getRectangles*()` uses `CRowBuilder` (`helpers/CRowBuilder.{h,cpp}`) to carve a row into
icon, button and text zones — no magic-number arithmetic in the delegates.

Tuning parameters live in `helpers/CDraw.h`: `kCellPad` (outer inset on all four sides of
`opt.rect`) and `kInnerGap` (gap between icon, text column and each button).

```cpp
CRowBuilder row(opt.rect, kCellPad, kInnerGap);
const QRect rectIcon   = row.takeLeft(row.height());      // square icon
row.markStatusColumn();                                   // snapshot width for status line
const QRect rectButton = row.takeButton(fmName.height()); // name-height square button
const QRect rectName   = row.nameSlice(fmName.height());
const QRect rectStatus = row.fullStatusSlice(fmStatus.height());
```

- `takeLeft(w)` / `takeRight(w)` — carve a full-height rect, advance by `kInnerGap`
- `takeButton(iconSize)` — square button sized so `CDraw::drawToolButton` renders its icon at
  exactly `iconSize × iconSize` (compensates the button's internal icon inset)
- `markStatusColumn()` — snapshot the remaining rect before buttons are carved
- `nameSlice(h)` / `statusSlice(h)` — top / bottom strip of the button-narrowed centre area
- `fullStatusSlice(h)` — bottom strip of the *pre-button* column, so the status line runs under the
  buttons
- `rowHeight(cellPad, nameH, statusH)` — matching `sizeHint` height

**Button height convention:** `CWksItemDelegate` and `CDBItemDelegate` use
`takeButton(fmName.height())` (name row only) with `fullStatusSlice`; `CMapItemDelegate` uses
`takeRight(row.height())` (full row height) with `statusSlice`.

### CMapItemDelegate forward declaration

`animations_t` is defined after `getAnimations()` in the private section, so the
`struct animations_t;` forward declaration above `getAnimations()` is required. Do not remove it.

---

## Tree icons are `QIcon`

`IWksItem::icon` and `IDBItem::icon` are `QIcon`, and `getIcon()` returns `const QIcon&`, so
delegates render at device resolution and non-GIS tree icons stay crisp on HiDPI. GIS items
(wpt/trk/rte/area) stay raster — `paintItem` pulls the pixmap out of the `QIcon` and stretches it.

**Two delegate paths cannot assume the icon fills the cell and both branch on `actualSize`:**
`paintDevice` (MTP subclasses overwrite the SVG with a raster read off the device) and
`paintGeoSearch` (`CGeoSearch::setIcon()` composes a raster when "accumulative results" is on).
Never collapse either to a bare `QIcon::paint` and never add a stretch back —
`actualSize(rect) == rect` answers "can this icon fill the cell", and `QIcon` downscales rasters
fine. `paintProject` / `paintGeoSearchError` are genuinely SVG and paint direct.

**`IDBItem::icon` holds folder icons only.** The DB blob raster lives on `CDBItem` as its own
`QPixmap displayIcon` (`getDisplayIcon()`), which `CDBItemDelegate::paintItem` stretches directly.
Do not re-add a `setIcon(const QPixmap&)` overload on `IDBItem`.

---

## Colour scheme — `CUiTheme`

`CUiTheme` (`src/common/theme/`) is the only source of status colours for rich text, label
stylesheets and `setTextColor()`. Roles `Neutral/Ok/Warn/Error/Info/Code`, each a fixed light/dark
pair. Tune there, never per site.

### Choosing the entry point

| The widget… | Use |
|---|---|
| is permanently a status message, only shown/hidden | `markLabel(label, role)` |
| is a value that is *sometimes* a status | `span()` / `spanBold()`, so the colour travels with the string |
| fills a background (table cell, banner) | `css()` |
| draws text on its own background | `cssForeground()` / `foreground()` |
| is a row or cell in an item view | `setForeground()` / `setBackground()` |

Set **both** colours or neither — a hardcoded background inherits the palette's text colour and
inverts on dark. Set **nothing** on a plain item-view row so it keeps the palette. Grey out with
`QPalette::Disabled, QPalette::Text`, never `Qt::gray`. Never bake a colour into a `tr()` string or
a `.ui` `<string>`; the translation carries it.

### Following a live scheme switch — `changeEvent()`

Two measured facts (Qt 6.10.2) set the whole design:

- **`QEvent::PaletteChange` reaches every widget, at every nesting depth, exactly once.** So a
  widget that holds scheme-derived content rebuilds it in its own `changeEvent()` and needs nothing
  central to drive it. (`ApplicationPaletteChange` is the one that arrives only on the application
  object — that difference is what makes `CThemeRefresher` a filter, not a `changeEvent`.)
- **A themed colour is baked in when the content is set** — into a `QTextCharFormat`, a style
  sheet, a `QPalette`, a rendered pixmap — and the source is dropped. `QTextDocument::toHtml()`
  returns the baked colour, not the markup it came from, so `setHtml(toHtml())` repairs nothing.
  Only re-running whatever produced the content works.

So the rule is Qt's own, and it costs one override in the class that owns the content:

```cpp
void CFilterSpeed::changeEvent(QEvent* e) {
  QWidget::changeEvent(e);
  if (CUiTheme::isPaletteChange(e)) {
    updateUi();  // whatever already regenerates this widget's content
  }
}
```

- **Regenerate, do not patch.** The producer is normally a method that already exists
  (`updateData()`, `buildHelpText()`, `renderThemedContent()`), and re-running it fixes the palette
  colours (links) and the themed markup (`CUiTheme::span`) in one pass. That is why no rich-text
  widget subclass is needed: every browser in the tree has an owner that regenerates it.
- **Do not run something re-entrant from the handler.** `CDetailsPrj` restarts its timer because
  `slotSetupGui()` drives a nested event loop; `CGridPlacer` checks `points` is populated first.
- **A handler that answers with `setPalette()`/`setFont()` needs a re-entrancy guard** — both
  re-deliver the event to the same widget (measured: one `updateStyle()` emits two `FontChange`s).
  `CLineEdit`/`CTinySpinBox` use `applyingStyle`.
- **Do not hand a themed string to something that stores it.** Pass the role and resolve at render
  time: `CCanvas::reportStatus(key, role, msg)` keeps the role unresolved in `statusMessages`.
- A themed colour that is only *returned* (`getInfo()`, a tooltip string) is fine — a tooltip is
  built fresh each time, and the widget that displays stored markup is what regenerates.
- **`CThemeRefresher` covers the two cases a `QLabel` cannot fix for itself**: a `markLabel()` role
  (the style sheet holds the resolved colour, so the role is recorded on the label and applied
  again) and a baked anchor colour (the label's own text is re-applied). Both are automatic, so a
  `markLabel()` caller needs no `changeEvent()`. `installThemeRefresh()` belongs in `main.cpp`,
  beside `CQmsStyle::install()`, once per application — qmaptool needs it too. Do not extend the
  sweep further; anything else belongs in the owning widget's `changeEvent()`.

Everything below is why a given surface needs rebuilding at all:

- **Never cache a themed colour in a member.** Resolve in the apply path. A cached one is stuck on
  the scheme it was built under and looks correct until the user switches.
- **A brush put on an item view's item is resolved once** and outlives a scheme switch, so the view
  must rebuild the affected rows (`CTableTrk`). Plain rows around them switch on their own, which
  makes the mismatch easy to miss.
- **Every map layer paints into a buffer rebuilt only on demand**, so nothing themed on the canvas
  follows a switch by itself. `CCanvas::changeEvent` forces `slotTriggerCompleteUpdate(eRedrawAll)`
  — per canvas, not the static `triggerCompleteUpdate()`, which reaches only the visible one.
- **A `QTextBrowser` cannot be repaired generically.** `setHtml(toHtml())` re-parses the baked
  colour and changes nothing; the original markup has to be supplied again. Subclassing does not
  help either — `QTextEdit::setHtml()` is not virtual, so an override would only shadow it.
- **`setPalette()` freezes only the roles in the palette's resolve mask.** Build a fresh `QPalette`
  and set just the role you own; every other role keeps following the application palette.
  `setPalette(QPalette())` clears the override. A copy of `palette()` carries `resolveMask 0` and is
  a harmless no-op — the freeze comes from the `setColor()`, not the copy. `QFont` behaves the same
  — express an underline or weight as a bare `QFont` with only that attribute set.
- **A `.ui` `<palette>` override lands before the constructor body runs** and freezes the same way.
  `alpha="0"` on `Base`/`Window` is the transparent-field idiom and is fine; an opaque colour in any
  group is not.
- **`setPalette()` replaces the widget's own override, it does not merge into it** — including the
  one `setupUi()` applied. So a widget that sets its own palette cannot take any role from a `.ui`
  `<palette>`: `CLineEdit` owns the transparent `Base`/`Window` itself, and the `<palette>` blocks
  on `lineName` in the four `IDetails*.ui` are inert leftovers.
- **`setTextColor()` is deliberately outside the rule.** It colours the text appended after it,
  which is the shell-transcript idiom: those lines keep the scheme they were written in on purpose
  (`CShell`, `IToolShell`, and the tool/BRouter output browsers).

### Checked-state cues — `CQmsStyle`

`CQmsStyle` (`src/common/theme/`) is a `QProxyStyle` installed by `CQmsStyle::install()` from both
`main.cpp`. It marks the checked state of toggle tool buttons (`PE_PanelButtonTool` + `State_On`)
and checkable menu items (`CE_MenuItem`), which every style draws too faintly to read on a dark
palette.

- **Paint a themed cue in a style, never in a style sheet.** A style resolves at paint time, so it
  follows the palette, cannot go stale and needs no opt-in. An app style sheet using `palette()`
  resolves once, re-polishes every widget on re-apply, and that re-polish echoes another palette
  event.
- **`install()` re-creates the active style by name** — `setStyle()` deletes the style it replaces,
  so the running one cannot be the base. That is what preserves `-style` / `QT_STYLE_OVERRIDE`.
- **Paint the menu tint before delegating to the base**, or it dims the text it marks; `CE_MenuItem`
  fills the row only when selected. Checked rows also go bold, so the cue is not colour alone.
- **Never fill a checked button with `Highlight`** — `ink` drops to ~1.3:1 on it. A border over the
  untouched face keeps ~4.9:1.
- **Menus resist style sheets:** Qt ignores `QMenu::item:checked`, and `QMenu::indicator` needs an
  `image:`, which for a `.svgt` bypasses `CSvgtIconEngine` and renders black.

### Canvas and printing

- **An info bubble on the canvas is chrome, not map surface — it follows the palette.** Background
  `CDraw::bubbleBackground()` (`QPalette::Window`), rich text `CDraw::drawBubbleText()`.
  `CDraw::bubble()` applies the background itself and takes no colour; `CDraw::infoPanel()` is the
  pointer-less panel (doc + frame + text in one call, leaving the painter translated).
  `QTextDocument::drawContents()` ignores the painter's pen and takes plain text from
  `QPalette::Text`, so it cannot be used here. Anchors take `QPalette::Link` at `setHtml()` time and
  a `PaintContext` cannot override it — a bubble that is not palette-backed needs an `a { color: }`
  default style sheet set *before* the markup is parsed.
- **An `IScrOpt` overlay draws its sheet with `CDraw::bubbleBackground()`, resolved in `draw()`** —
  the table and `.svgt` toolbar icons on it follow the palette, so a white sheet strands them.
- **`CDraw::text()` haloes in white.** That is for text over map tiles, where the halo is what makes
  any colour readable; on a solid themed bubble it glows. Paint there with a plain `drawText()` and
  `CMainWindow::self().getMapFont()` (`CGisItemTrk::drawLimitLabels`).
- **Printing:** `const CUiTheme::CForceLight paperColours(printable)` in `CDetailsPrj::draw()`,
  beside the two palette branches it completes. It does not touch `QPalette`, so palette colours
  still need their own paper branch.
- `QTextBrowser` applies a `<link>`ed stylesheet (`loadResource`, `StyleSheetResource`), so
  `CHelpBrowser::loadResource()` appends a themed `code, pre` rule to whatever the packaged help
  ships. That CSS lives outside this repo; `Role::Code`'s light arm matches its `code` background,
  so light mode renders unchanged.
- `paletteIsDark()` is `inline` in `CUiTheme.h` so `CSvgtIconEngine::roleColor()` can share it with
  nothing to link (the plugin needs only `target_include_directories(svgticonengine PRIVATE ..)`).
  It is for that plugin and for `CUiTheme::isDark()` alone — **app code branches on
  `CUiTheme::isDark()`**, which is the same test plus the `CForceLight` override. Never write a
  third copy of the threshold; it drifts silently.

### Deliberately light — do not "fix"

- **`IPlot`'s sheet is white in every theme.** `eModeNormal` and `eModeSimple` both
  `fillRect(rect(), Qt::white)`; waypoint icons are painted straight onto it and are unthemable
  raster symbols authored for a light ground. A themed panel around a white plot is intended
  (`CScrOptRangeTool`).
- `CShell`/`IToolShell` colour each line as it is appended, so lines already in the log keep the
  scheme they were written in.
- The halo under `CWksItemDelegate`'s progress bar and `CIconGrid`'s tiles are white by design.
- `CIconGrid`, `CScrOptUnclutter`, `CPrintDialog`, the qmaptool overlays and `CMapIMG` are
  self-consistent light surfaces. `CDetailsOvlArea`'s white brush-style swatches are the neutral
  ground a pattern preview wants.
- **A hardcoded colour is a defect only once its contrast fails against both arms.** A mid-grey such
  as `Qt::darkGray` clears `#efefef` and `#353535` alike, and white is a legitimate neutral ground
  for a swatch or preview tile. Work out both grounds before filing one.

### Exercising a live scheme switch

`plasma-apply-colorscheme BreezeDark` with the app running, and `QT_QPA_PLATFORMTHEME` unset — on
`qt5ct` the Qt 6 platform theme never loads and no scheme change reaches the app.

Good probe: a DEM property panel with "Enable color shading" on and the grades combo at its **last**
entry, where the slope spins go read-write and underlined in `Role::eInfo`. The panel is cached on
the DEM (`IDem::getSetup()`), so it holds a stale scheme until the DEM is unloaded.

---

## IDrawContext — logical vs device pixels (HiDPI)

`convertRad2Px()` / `convertPx2Rad()` work in **logical** viewport pixels (built from `center` and
`scale*zoomFactor`) so they match Qt mouse/widget coordinates. The draw **buffers** are **device**
pixels: `bufWidth/bufHeight = viewWidth/viewHeight * pixelRatio + 2*BUFFER_BORDER`, and `draw()`
divides the scale by `pixelRatio`.

**Never compare a `convertRad2Px()` result against `bufWidth`/`bufHeight`** — they agree only at
`pixelRatio == 1`. Use `viewWidth`/`viewHeight` for viewport-fit tests.

Test HiDPI paths on a normal screen with `QT_SCALE_FACTOR=2 build/bin/qmapshack`, adding
`QT_SCALE_FACTOR_ROUNDING_POLICY=PassThrough` for fractional factors like 1.5.

---

## Icons

Developer howto: `README_ICON.md`.

### Hold a `QIcon`, never a `QPixmap`

A `QPixmap` is one raster frozen at the dpr it was built at: it cannot serve a larger request and
cannot follow a window to another screen, whatever the source format.

- Anything taking a `QIcon` (buttons, actions, tree/list items, a Designer `<iconset>`): reference
  `:/icons/Foo.svgt` and let the paint path ask for the size.
- Static icon in a dialog: `QSvgWidget` + `CSvgtIcon::load()`. Qt ships no widget that displays a
  `QIcon`, and `uic` bakes a `<pixmap>` into `QPixmap(path)` before any widget sees the path.
- Rich text `<img>`: `CSvgtIcon::htmlImageSrc()`, always with both `width` and `height` — without
  them there is no HiDPI path.
- Canvas rasters (waypoints, POI, cache) are data, not icons.
- **Never `static` a paint-path icon** — it pins the colour scheme live at first paint and hides the
  icon from a path-shaped grep. There are none in the tree; keep it that way.

`cmake/IconGate.cmake` fails the build on regex-detectable violations. It deliberately cannot see
`QIcon(pixmapVariable)` or a `.pixmap(w,h)` missing its dpr.

**Left alone deliberately — do not "finish" these:** the three `.pixmap(w,h)` calls with no dpr in
`CSelectCopyAction`/`CInvalidTrk`; `IGridPlacer.ui`'s `line_3px_*` black PNGs; `CGeoSearchWeb`'s
service icons, where the stored path is user data and `defaultIcon` equality is the "is this
user-added" test — converting needs a settings migration plus an `isUserDefined` flag first, or
"Restore default list" erases the user's own services.

### SVG line endings are pinned to LF

`.gitattributes` pins `*.svg`, `src/icons/svg.sha256` and `src/icons/svghygiene` to `text eol=lf`
because `cmake/IconHygiene.cmake` compares `file(SHA256)` of each **working-tree** `.svg` against
`src/icons/svg.sha256`. One CR changes the hash, and the build reports every icon as edited and
demands inkscape + python3.

**A blob committed with CRs cannot be cleaned by `checkout`/`restore`.** `eol=lf` normalises on the
way *in*, never on the way out, so the CRs land in the working tree while diff normalises the file
back — those paths report as modified forever, and any merge that touches them refuses to start
before doing anything. Only a commit carrying LF blobs fixes it.

When the local branch is a strict ancestor of the incoming one and the only dirt is that CR churn,
the merge is a fast-forward blocked by nothing real: `git reset --hard <remote>/<branch>`.

### Nothing that stores an icon path may be pruned

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

### Drawing rules no tool catches

- **Draw structural line-art in `ink`.** Qt greys a disabled icon by lightness only, so an icon
  drawn only in `lead`/`paper` looks identical enabled and disabled on dark. Keep `lead` for a
  secondary outline. (`RatingStarEmpty`/`UnFocus` stay grey because grey *is* their meaning.)
- **Never give a shape's stroke the same role as its fill.** It renders invisible and every tool
  passes, because each colour is individually valid. Only a render shows it.
- **The letter or shape carries the meaning; colour is never the only cue** — `SQLite`/`MySQL` use a
  bold initial on the cylinder face plus a brand-colour cap.
- **Negation uses the set's own mark**: a red `#ff5555` disc with a `paper` slash, as `NotPossible`.
- **Bake lettering to paths** (`inkscape --actions "select-all;object-to-path"`), then strip the
  group's inline `style` back to `fill:currentColor` — the conversion resolves the class's `color`
  inline, and an inline value shadows the themed class. Never ship live `<text>`.
- A family (`Act*`, `Add*`, `Mime*`) shares stroke weight, corner radius and optical size.

### Qt renderer traps the pipeline works around

Qt has no SVG recolouring API, so `CSvgtIconEngine` rewrites the SVG text and loads via
`QSvgRenderer(QByteArray)`. Qt's renderer *does* resolve `currentColor` — only the setter is missing.

- `currentColor` with no `color` set renders **black** (Qt and inkscape); a **duplicate** `color=`
  renders **nothing**; lowercase `currentcolor` is black (QTBUG-46947); a `<style>` class beats a
  root `color=`, so the KDE/Breeze idiom does not mix with ours.
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

### Waypoints are data, not chrome

`src/icons/waypoints/` is named by the GPX `<sym>` vocabulary shared with Garmin and its exact look
is a frozen contract, so it **stays PNG on the canvas**: SVG would hand rendering to whichever Qt
the user has, and a Qt antialiasing change could silently restyle accepted iconography. External
user icons are PNG/BMP forever (`CWptIconManager`), so the raster path must exist anyway. Dark
theming does not apply — they sit on map tiles, not the UI palette.

A waypoint symbol used as **UI chrome** (menu action, tool button) is under none of that and uses
the SVG. Only `FlagBlue.svg` and `PinBlue.svg` are registered; add others as UI needs them.

Gate any waypoint change with `src/icons/tools/wptdiff.py --size 96` — it must report
`visible (>8) == 0`.

**`icon_t::focus` is absolute pixels of the loaded raster**, hardcoded against 32. Any resolution
change breaks every anchor until focus is stored relative (0..1) — a prerequisite for touching
waypoint resolution at all. `getWptIconScaledByName` holds `focus = focus * scale`;
`getWptIconByName` does not. `focus` is serialized but overwritten on every load via
`deriveSecondaryData()`, so a relative-focus change needs no migration.

### GIS item icons — serialization stores sym/colour, not the icon

`.qms`/DB persist the **symbol name** (`wpt.sym`) and the **colour** (`trk.color`/`area.color`),
never a rendered icon. The icon is re-derived on load: waypoint via `getWptIconByName(sym)`,
track/area by loading `Track.png`/`Area.png` as a shape mask and filling it with the data colour.
The rendered pixmaps in the DB `items.icon` BLOB and `.qms` `history_event_t.icon` are output
caches. **Serialization does not constrain the source format.**

So only the waypoint symbol is a genuinely frozen raster. **Tracks and areas can be SVG**: their PNG
is only a silhouette mask (`createMaskFromColor`), so an SVG rendered at the target size gives a
crisp mask and the same data colour. `QIcon` will not upscale a raster, so such a change must render
the SVG at size rather than wrap the old 32px PNG.

---

## Dock widgets

`QDockWidget::setFeatures()` disables `toggleViewAction()` unless `DockWidgetClosable` is in the set,
so a docker missing that flag has a permanently greyed-out *Window* menu entry — and a floating one
loses its close button too. Every docker in both apps must keep `DockWidgetClosable`.

---

## GDAL

### `QImage::Format_Indexed8` + `RasterIO`/`ReadRaster` — row padding

**Never pass `img.bits()` as the destination buffer** when the width isn't guaranteed to be a
multiple of 4. Qt may pad `QImage::bytesPerLine()` beyond the pixel width for 1-byte-per-pixel
formats, but a GDAL read with no explicit line spacing assumes a tightly packed buffer
(`bytesPerLine == width`), silently skewing every row once they diverge.

Read into a flat `QVector<quint8>` and build the image with the explicit-stride constructor:
`QImage(buf.constData(), w, h, w, QImage::Format_Indexed8)` — as `CDemVRT` and `CMapVRT::draw()` do.
Multi-byte formats (`Format_ARGB32`) are unaffected since `width * 4` is always a multiple of 4.

### Warped VRT — transparency outside the source footprint

`GDALAutoCreateWarpedVRT` resamples onto an axis-aligned bounding box around the reprojected
footprint, so corners with no source coverage exist whenever the source isn't already axis-aligned
with the target SRS. What fills them depends on the path:

- **Single-band palette/gray (`CMapVRT`)** — handled. If the source declares a nodata value, GDAL
  uses it as both src/dst nodata (we never set `padfSrcNoDataReal`/`padfDstNoDataReal`), so
  uncovered pixels come back as that index and the constructor zeroes its alpha in the colortable.
  With no source nodata there is no automatic transparency; `Format_Indexed8` has no alpha channel
  to retrofit one.
- **Multi-band RGB(A) without its own alpha band (`CMapVRT`)** — handled by a synthetic destination
  alpha band (`GDALWarpInitDefaultBandMapping` + `psOptions->nDstAlphaBand = nBandCount + 1`,
  mirroring `gdalwarp -dstalpha`). The warp tracks per-pixel source coverage into it;
  `rasterBandCount` is re-read from the warped dataset afterwards so `draw()`'s band loop picks it
  up. Without it, uncovered corners come back solid black — GDAL's own warp fill, which
  unconditionally overwrites the `img.fill(white)` pre-fill in `draw()`.
- **`CDemVRT`** — no handling. Uncovered elevation samples read back as whatever the destination
  buffer was zero-initialised to (not `NOFLOAT`); `getElevationAt()`/`draw()` never check warp
  coverage. Only matters for non-axis-aligned DEM sources. **Open.**

### Blank hillshade when zoomed out

Symptom: hillshading renders at close zoom but is blank far out. `CDemVRT::draw()` reads via
`ReadRaster()` with automatic overview selection, so at high `buf_scale` GDAL can pick a corrupt or
all-NoData overview level and return an all-NoData buffer.

Check overview integrity *before* suspecting `CDemVRT.cpp`/`IDem.cpp`:
`gdallocationinfo -valonly -overview <N> <vrt> <x> <y>` at several sample points. Fix by rebuilding
the `.ovr` — delete the old one, then `gdaladdo -ro -r average <vrt> <factors>`.

### External tool paths

Always resolve via `IAppSetup::getPlatformInstance()->findExecutable("toolname")`, never a bare
name. `CAppSetupWin` restricts `PATH` to the app directory to prevent DLL conflicts, so a bare name
silently yields `QProcess::FailedToStart` on Windows if the binary isn't co-located with the app.

### ZIP archives — `CGdalZip`

`helpers/CGdalZip.{h,cpp}` reads ZIP archives through GDAL's `/vsizip/`. No ZIP library is needed.
Used by the BRouter installer only.

- **Address an archive as `/vsizip/{<absolute path>}/<entry>`.** The braces bypass GDAL's list of
  accepted archive suffixes, which lacks `.jar`.
- `VSIReadDirRecursive()` marks directories with a trailing slash; `fileList()` drops them.
- `extractAll()` writes plain files, no permissions and no symlinks, and rejects entries pointing
  outside the destination.

---

## Overview-advisory system

Warns when a VRT-backed map/DEM has missing or inadequate GDAL overview pyramids, or too many
source files, and can fix both.

**Files:**
- `helpers/CGdalVrtUtil.{h,cpp}` — `buildOverviewAdvice()`, `suggestOverviewLevels()`,
  `handleRenderTimeout()`, `overview_advice_t`, `file_overview_info_t`, `raster_geometry_t`,
  `overview_advisory_state_t`, `read_deadline_t`, `kMetersPerDegree`
- `helpers/CVrtAdvisoryDialog.{h,cpp}` + `.ui` — the fix/info/combine dialog
- `helpers/CVrtCombiner.{h,cpp}` — "Combine files..." grid-split/footprint logic
- `dem/CDemVRT.{h,cpp}`, `map/CMapVRT.{h,cpp}` — own `overviewAdvice`/`advisoryState`/`rasterGeometry`
- `dem/CDemWCS.cpp` — opts out via `supportsOverviewAdvisory=false`
- `map/IMap.h`, `dem/IDem.h`, `map/IMapItem.h`, `map/CMapItemDelegate.{h,cpp}` — badge + on-demand info
- `map/CMapItem.cpp`, `dem/CDemItem.cpp`, `map/CMapList.cpp`, `dem/CDemList.cpp` — context-menu entry
- `canvas/CCanvas.cpp` — owns and shows the dialog

**Invariant: the dataset is always a VRT.** `new CMapVRT`/`new CDemVRT` happen only for `.vrt`
files; every other format has its own class, and `CDemWCS` opts out. `buildOverviewAdvice()` relies
on this — there is no "concrete raster format" branch.

**Invariant: `overviewAdvice`, tile count and overview state are immutable per instance.** Any
DEM/map list change — including a successful Fix/Combine (`sigContainerRebuilt` →
`setupDemPath`/`setupMapPath`) — destroys the instance and creates a new one with fresh advice;
nothing updates it in place. `overviewNeedsAttention` caches `needsAttention()` at setup so the
delegate's per-paint badge poll (`showsOverviewWarning()`) is O(1) instead of walking `perFileInfo`.

**Trigger:** `draw()` wraps `ReadRaster()` with a 5 s deadline (`read_deadline_t` + GDAL's own
progress-abort hook — do not add a second warp-options progress callback, it froze the UI). On
timeout `handleRenderTimeout()` fires the advisory (render thread → GUI thread) once per loaded
instance per session, and only when `showsOverviewWarning()` (`!suppress && needsAttention()`) — the
same condition as the proactive tree badge.

**The dialog is application-modal** (`setModal(true)`): while it is open the map/DEM it is about
must not be read. A pan's `draw()` or a mouse-move's `getElevationAt()` racing a Fix/Combine file
rewrite (an external gdaladdo/gdalbuildvrt process — the in-process dataset mutex cannot guard it)
crashes GDAL. Modal blocks user input; a background redraw from a sibling layer during a job is a
known unguarded residual.

### `buildOverviewAdvice(dataset, band, isPaletteIndexed, suggestedLevels)`

A read can be sped up by two additive sources: the container's own overview, and each source file's
own overview for the region read.

1. The container's claim is trusted immediately only if a real `.ovr` file is in `GetFileList()`; a
   bare `<OverviewList>` is not trusted yet.
2. If the verified container factor already meets `targetFactor`, every source file is skipped
   (`perFileInfo` still lists them, `checked=false`).
3. Otherwise every source is probed. An unverified `<OverviewList>` becomes trusted if *every*
   source turns out to have its own overview; otherwise it is discarded.
4. `weakestMaxFactor = max(containerFactor, weakestSourceFactor)`.

`containerHasOwnOvr` is true only via step 1; a `checked=false` entry always implies
`containerHasOwnOvr == true`.

**Factors are per-file pixel ratios (`fullResSize / overviewSize`)**, read from each file's own band
before any warp — no geotransform/CRS math, even when sources and container differ in CRS.

**`suggestOverviewLevels(xsize, ysize, maxFactor)`** doubles from 2 until the decimated size drops
below the primary screen's longest dimension (fallback 1920 px) or `maxFactor`. `CDemVRT` leaves
`maxFactor` unbounded; `CMapVRT` caps at `kMaxMapOverviewFactor = 16`.

**`<OverviewList>` mechanics:** `gdalbuildvrt` (GDAL ≥ 3.2) writes it automatically at build time
only, never retroactively. GDAL does not complain about a declared factor with no backing data — it
silently falls back to full-resolution reads, hence the verification above.

### "Fix overviews"

`slotFixOverviews()` / `finishFixOverviews()`:

1. `filesToFix()` runs `gdaladdo` on every source short of `suggestedLevels` (or on `filename_`
   itself if there are no source files). Recipe: `-r` (nearest for palette, average otherwise),
   `COMPRESS_OVERVIEW=DEFLATE`, plus `PREDICTOR_OVERVIEW=2` for non-palette data — matches the
   source predictor for a ~2–3× smaller `.ovr`, harmful on palette indices. `.ovr` block size is
   inherited from each source automatically.
2. `fixContainerOverviewList()` rewrites just the `<OverviewList>` element to
   `advice_.suggestedLevels` — no full `gdalbuildvrt` re-run, no re-probe.

### "Combine files..." (`CVrtCombiner`)

Rewrites the container VRT to reference a handful of large compressed/tiled GeoTIFFs instead of many
small sources.

- **Splits the container's own resolved raster**, not the source files — `computeGrid()` cuts a plain
  pixel-window grid (`pixel_window_t`, row/col-tagged). The merge step is a pure crop
  (`gdal_translate -srcwin`), no resampling.
- **Layout comes from the VRT XML, no pixel reads.** `readVrtLayout()` parses `<VRTDataset
  rasterXSize/rasterYSize>` and every source's `<DstRect>` footprint. `tightenToFootprints()` crops
  each cell to the bbox of the footprints overlapping it, or drops it (`empty()`) if none does.
  Resolves in ms even for a huge VRT, so it runs inline on the GUI thread — no background scan,
  `CThread` or `QProgressDialog`.
- `kMaxOutputTiles` (40) and `kMaxPixelsPerTile` (150,000,000) are **tuning placeholders**, not
  settled values — they need real tuning against a large VRT.
- `slotCombineFiles()` reads the layout, computes and tightens the grid, confirms with the user,
  backs `filename_` up to `filename_ + ".bak"`, runs one `gdal_translate -srcwin` per tile into the
  *source files'* directory (`group_r<row>_c<col>.tif` — which can differ from the VRT's own
  directory), then one `gdalbuildvrt -overwrite`. Compression `COMPRESS=DEFLATE`/`PREDICTOR=2` (not
  ZSTD — not a mandatory GDAL dependency), `TILED=YES`, `BLOCKXSIZE`/`BLOCKYSIZE=512`,
  `BIGTIFF=IF_SAFER`.
- `JobKind` (`FixOverviews`/`Combine`) dispatches `slotJobFinished()` to `finishFixOverviews()` /
  `finishCombine()`; both emit `sigContainerRebuilt()` on success.
- **Non-destructive:** original sources are never deleted. On cancel/failure `filename_` is restored
  from `.bak` (only the final `gdalbuildvrt -overwrite` touches it) and partial tiles are removed.
- **Limitations:** footprint tightening trims only the nodata border *between* sources, not nodata
  inside one; re-running Combine with a different grid size can leave stale `group_rX_cY.tif` files.

### Subfile count, disk usage, dialog

- **Subfile-count check** (independent of overviews): `hasTooManySubfiles()` flags a VRT with more
  than `kMaxSubfileCount` (50) sources — GDAL opens and stats every source overlapping a read
  region, so reads stay slow regardless of overviews. `needsAttention()` is
  `needsOverviewFix() || hasTooManySubfiles()`; the two problems have independent fixes and
  independent gating.
- **Disk usage** (`diskUsageBytes`/`diskUsageIsEstimate`): the real on-disk footprint, summed with
  `QFileInfo` over `GetFileList()` plus each source's `.ovr`/`.aux.xml` sidecars (`GetFileList`
  omits *source* sidecars, includes only the container's own). Fully qualified → exact; shallow,
  missing or no overviews → sub-files × 5/3, flagged as an estimate. Formatted with
  `QLocale::DataSizeSIFormat` to match `du --si`, not the 1024-based IEC default.
- **Dialog table:** the container is a synthesized row graded by the same `rowStatus()` as source
  rows. `htmlTd()` and friends are static methods (not free functions) so they can call `tr()`; pass
  plain `<`/`>` into them, they escape it themselves. `hasExistingOverviews()`'s container fallback
  keys off `containerHasOwnOvr` (not `containerFactor > 0`) so the "Update" / "Add `<OverviewList>`"
  wording agrees with the fix confirmation dialog.
- `raster_geometry_t` comes from `CGdalVrtUtil::sourceGeometry(pre-warp source)` — the source file's
  own size and resolution, matching `gdalinfo` (exact metres for a projected CRS,
  `kMetersPerDegree` approximation for geographic). **Not the warped grid**: reading the warped
  geotransform gives wrong pixel sizes (+26% for a UTM source drawn in EPSG:4326).
- **Badge and info:** `showsOverviewWarning()` (`!suppress && needsAttention()`) drives the tree
  badge; `hasOverviewInfo()` drives the "Overview Info..." context-menu entry regardless of
  attention state. `CMapItemDelegate::overviewBadgeRect()` is shared by
  `paint()`/`editorEvent()`/`helpEvent()` so painted, clickable and tooltip areas cannot drift.
- **Lifecycle:** `advisoryState.open` suppresses `emitSigCanvasUpdate()` retries while the dialog is
  open. `closeEvent()`/`reject()` both confirm-cancel a running job and clean up partial output.
  `CCanvas::showOverviewAdvisory()` dedupes by *filename* via
  `findChildren<CVrtAdvisoryDialog*>()`, not a separate registry.

---

## Open work

Two analysed-but-unstarted designs live in `.notes/`, each wanting its own branch:
`QMS-1135-overview-restore-plan.md` (transactional rollback when a *Fix overviews* job is cancelled)
and `waypoint-icon-resolution-plan.md` (32 → 96 px waypoint icons, gated on storing `icon_t::focus`
relative). De-freeze either by pointing at the file.

### CMake — remaining

- Retire the platform blocks: the hardcoded `C:\...` cache defaults and the macOS
  `QT_DEV_PATH`/`ROUTINO_DEV_PATH`/… `FATAL_ERROR` gauntlet can move into `CMakePresets.json`, and
  `MacOSX/build-QMS.sh` and the msvc batch files can call `--preset`. Needs someone who can test
  both release paths.
- Shared `qms_common` library: ten files under `src/common/` are compiled once per app. Blocker is
  `src/common/help/CHelp.cpp`'s `"helpers/CSettings.h"`, which resolves per-app; the two copies are
  byte-identical, so moving that header to `src/common/helpers/` unblocks it.
- Confirm on their own platforms that these removals were inert: macOS `LINK_FLAGS`, the
  `-framework` entries in `CMAKE_C_FLAGS` and the three framework include dirs; Windows
  qmt_map2jnx's `Win32/` include dir. `msvc_64/cmake/{FindGDAL,FindPROJ,FindJPEG}.cmake` can go if
  the gisinternals GDAL ships `GDALConfig.cmake`.

### QMS-1156 — convert non-UTF-8 VRT files to UTF-8 on load

GDAL receives filenames as UTF-8 everywhere. A guard rejects `.vrt` files whose bytes aren't valid
UTF-8, which dead-ends users with legacy Windows-1252/Latin-1 VRTs. Offer a confirmed, `.bak`-backed
one-click conversion instead.

GDAL facts behind the design (tested on 3.12):
- The CPL XML parser **ignores the VRT `<?xml encoding?>` declaration** — `<SourceFilename>` bytes go
  to `open()` verbatim, so even a correctly-declared `ISO-8859-1` VRT fails. Only raw bytes matter,
  so a byte-level UTF-8 check has no false-rejection risk.
- On-disk source names are UTF-8, so transcoding a Latin-1 VRT reproduces the real names.
- GDAL does not hard-fail a bad path: exit 0, checksum -1, `ERROR 4` on stderr — a quietly broken
  dataset, which is what feeds the missing-file advisory.

Design: transcode from candidate encodings (Windows-1252 → ISO-8859-1 → system ANSI), then **verify
by resolution** — every `<SourceFilename>` must now exist on disk. First candidate that resolves all
sources wins, else fall back to plain rejection. Trigger at load time in `CMapVRT`/`CDemVRT`
construction (a non-UTF-8 VRT never constructs, so the advisory dialog cannot be the entry point),
reusing the advisory `.bak`/rewrite scaffolding. Not platform-gated — it reproduces on Linux. Also
rewrite the `<?xml encoding?>` declaration to UTF-8.

### POI icons (SJJB) can be converted to SVG

POIs are not waypoints — no canvas freeze applies. 303 SVGs already ship under
`src/icons/poi/SJJB/svg/<category>/` and 249 of the 250 referenced PNGs have a counterpart, but none
are in `resources.qrc`. They are SJJB templates carrying a placeholder fill `#111111` that their
build recolours per category, so the colour must be recovered per icon from the shipped PNGs
(16 categories, 6 with 2–3 variants).

Work: register ~250 SVGs, add a recolour step, change `CPoiIconCategory`'s `QPixmap` members to
paths plus ~302 literals in `CPoiFilePOI_TagMap.cpp` — the path *shape* changes, so that needs a
lookup table, not a regex — and a render cache keyed by (icon, size, dpr). The cache is a win
regardless: `CPoiFilePOI` currently `.scaled()`s a pixmap per POI per repaint.

Gate it or do not do it: render each recoloured SVG at 32, diff against its shipped PNG, require
`visible (>8) == 0`. A diff that measures **colour** reports ~240 false failures.

Two defects found while scoping: `CPoiFilePOI_TagMap.cpp` asks for `health_pharmacy_dispencing`
(a typo; the SVG is `pharmacy_dispensing`), and one SVG embeds a raster that is not in the tree
(`pastedpic_10102008_233747.png`).

### `CDemVRT`/`IDem` rendering speed

- **Open:** batch `getElevationAt()`/`getSlopeAt()` point queries. Each is one small `RasterIO` call
  per point (e.g. per vertex of a track elevation profile) — a different path from `draw()`'s map
  rendering. Worth revisiting only if track-profile performance comes up.
- **Deliberately skipped:** caching `1/xscale`/`1/yscale` for `slopeOfWindowInterp()`. Modest win,
  and hoisting needs either a signature change touching its three callers or new cached members with
  a staleness trap — `xscale`/`yscale` are plain protected members assigned directly, with no setter
  to keep a reciprocal in sync.
