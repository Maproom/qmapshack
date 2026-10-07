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
  change, run `clang-format`, report, stop. For reference, the user runs
  `cmake --build build --target qmapshack -j$(nproc)`.
- **Check the facts before changing anything**: code, ticket or measurement first. Say what could
  not be checked.
- **Run it before reporting it.** Verify headlessly with `doc/tools/shots.py` (`-o <scratch> replay`,
  `selftest`), read the images and the JSON/INI written. Name the command that showed it, or say
  nothing was run and what would settle it.
- **Syntax-check changed files**: `.notes/syntax-check.sh <file.cpp>...` (`c++ -fsyntax-only`, the
  build's flags, the Qt from `build/CMakeCache.txt`). Catches shadowing and stale identifiers; not
  moc, AUTOUIC, linking, or anything inside `#if defined(Q_OS_WIN32)`.
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
- **All nine `.qm` catalogs are in the binary under `:/locale`**
  (`strings -a -e l build/bin/qmapshack | grep qmapshack_de.qm`), but `prepareTranslator()` reads
  only the filesystem, so a build-tree run is English. A `:/locale` fallback would fix that.
- **Standard dialog buttons are translated by the platform theme, not a `.qm`**, following
  `LANGUAGE`/`LC_MESSAGES`. `QT_QPA_PLATFORMTHEME=` (empty) still loads `kde` via
  `XDG_CURRENT_DESKTOP`; `generic` does not. Offscreen loads no theme and ignores the variable.
- **An unknown `QT_QPA_PLATFORMTHEME` still loads the desktop's theme**, so the rest is pinned in
  the application. Generic theme vs none (Qt 6.10.2, Fusion) differs only in
  `SH_DialogButtonBox_ButtonsHaveIcons` and `QIcon::themeName()`:
  `CQmsStyle::pinThemeIndependentHints()` answers the hint 0; nothing pins `QIcon::themeName()`. Any
  further difference goes into that override, never a per-symptom patch.
- **`QLocale::system()` follows `LANGUAGE`, not only `LANG`.** A desktop-independent run passes
  `--locale`.
- **Stack smashing at startup after a checkout:** stale objects; `ninja -C build -t clean qmapshack`.
- **`qms_options`** is the INTERFACE target carrying the project's warning set. First-party targets
  link it `PRIVATE`; bundled 3rdparty must not. Add flags through `qms_add_flag_if_supported()`.
- **`target_link_libraries` is keyword form everywhere.** Plain and keyword signatures cannot be
  mixed on one target, so a new call must say `PRIVATE`.
- **Dependencies are imported targets**: `GDAL::GDAL`, `PROJ::proj`, `JPEG::JPEG`,
  `ROUTINO::ROUTINO`. `ROUTINO_XML_PATH` stays a plain variable — qmapshack passes it as a define.
- **`Qt6::GuiPrivate` comes with `Gui` in 6.8, is a package of its own from 6.9 and required from
  6.10**, so it is an `OPTIONAL_COMPONENTS` find with no version test; a missing package still fails at
  `target_link_libraries`. `QT_NO_PRIVATE_MODULE_WARNING` is set: documentation mode never ships, so
  being tied to one Qt build is accepted. Only 6.10.2 is tested.
- **Defines are per target**: `HELPPATH` on qmapshack and qmaptool, `ROUTINO_XML_PATH` and
  `HAVE_DBUS` on qmapshack. Global on purpose: `_CRT_SECURE_NO_WARNINGS`, `/MP` and `/utf-8` under
  MSVC, which the bundled FIT SDK needs, and `-march=native`.
- **Install paths** come from `GNUInstallDirs` on UNIX and stay relative to the application
  directory on Windows, where `CAppSetupWin` resolves `HELPPATH` against it. Six cache variables
  survive so a packager can override them. `HTML_INSTALL_DIR` is `share/doc/HTML`, not
  `CMAKE_INSTALL_DOCDIR` — changing it moves the installed help.
- **`CMAKE_RUNTIME_OUTPUT_DIRECTORY` is set, the four per-config variables are deliberately not.**
  They are the only thing a multi-config generator consults for its `$<CONFIG>` suffix, so pinning
  them collapses every configuration into one `bin`, which leaves a `.pdb` from the previous
  configuration beside a mismatched `.exe` and makes the build report the other config's binary as
  up to date. `msvc_64/CopyFilesGis.bat` reads `bin\Release\` and wants the suffix. Single-config
  generators resolve the per-config variable to the same path as the plain one, so Linux and the
  macOS release path (`build-QMS.sh` uses `Unix Makefiles`; its Xcode branch is marked BROKEN and
  exits before bundling) are flat whatever the build type.
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

### Translator loading

`IAppSetup::prepareTranslators(appPath, appPrefix, qtPath)` installs both catalogs, and the
application catalog is the gate: no `<appPrefix><locale>.qm` means the GUI is English *including*
Qt's own strings. Skipping `qtbase_<locale>.qm` is not enough for that - a desktop environment
installs a Qt catalog for the system locale itself, from the platform plugin, before the application
touches a translator (measured on KDE: a bare `QApplication` with no translator installed answers
`translate("QPlatformTheme", "Cancel")` with `Abbrechen`, and `QT_QPA_PLATFORM=offscreen` turns it
back into `Cancel`). So the untranslated branch installs `CSourceTextTranslator`, which answers every
lookup with the source text: translators are asked newest first and a non-null answer ends the
lookup, so it wins over the desktop's. `QCoreApplication::translate()` applies `replacePercentN()` to
the result afterwards, so `%n` still works. It exists twice, once per app. `qmt_rgb2pct` keeps its
own unguarded two-liner - it is a command line tool.

Not every string in the window goes through those catalogs. A desktop integration translates its own
contributions with catalogs of its own, picked by `LANGUAGE` and loaded while the platform plugin
comes up - on KDE the standard button labels come from `KStandardGuiItem` in
`/usr/share/locale/<lang>/LC_MESSAGES/kwidgetsaddons6_qt.qm`, which no Qt or QMapShack catalog covers,
so `--locale it` alone leaves them German on a German desktop (measured). `IAppSetup::exportLocaleEnv()`
therefore runs as the first statement of `main()`, before `QApplication`, and puts a `--locale` value
into `LANGUAGE`. It has to parse argv by hand - `CCommandProcessor` needs an application instance,
which is already too late.

Running from the build tree never finds the application catalog: `applicationDirPath` is
`<build>/bin`, so the path resolves to `<build>/share/<app>/translations` while the `.qm` files are
built into `<build>/src/<app>/`. A build-tree run is therefore always English.

**`QTranslator::load()`'s return value is no existence test.** It strips `_<suffix>` and `.<suffix>`
off the name until something loads, so `load("qmapshack_pl", dir)` succeeds with the installed,
untranslated `qmapshack.qm` (measured, Qt 6.10.2). Test with `QFileInfo::exists()` on the exact
name, and pass the full `<prefix><locale>.qm` to `load()`.

---

## Code style

**All C++ is formatted with clang-format.** After editing any `.cpp` or `.h`:

```bash
clang-format -i <file> [<file> ...]
```

Style is `.clang-format` in the project root (Google base, 120 columns). Always accept its output —
never revert or hand-tune it, and keep reformat hunks it makes on unrelated pre-existing drift.

**Win32 includes are the one exception.** `<windows.h>` must precede every other SDK header, or MSVC
fails with `winnt.h: #error "No Target Architecture"`. clang-format sorts alphabetically and puts
`errhandlingapi.h`, `fileapi.h`, `winbase.h` in front of it, so include only `<windows.h>` (it pulls
those in) or guard the block with `// clang-format off` as `CMainWindow.cpp` does.

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

### IGarminStrTbl label cache (the `get`/`decode` split)

Profiling (Tracy) showed label decoding (`strtbl.get`) was the single largest slice of
`loadVisibleData` — an `mmap` (via `CFileExt::data()`) plus a codepage decode **per labelled object,
re-done every frame** even though label content is immutable. To fix that, `IGarminStrTbl::get()` is a
**non-virtual** cache wrapper over a bounded LRU `QCache<quint64, QStringList>` (key `(type << 32) |
offset`); on a miss it delegates to the protected pure-virtual **`decode()`** (the per-coding work,
implemented by `CGarminStrTbl6/8/Utf8`). Do not re-merge `get`/`decode` or make `get` virtual again —
the split exists so the cache lives in exactly one place. The cap (`labelCacheMaxEntries`) bounds
memory when panning huge gmapsupp files; a hit needs no `mmap` at all, which also shrinks the
per-subfile `CFileExt::free()` `munmap` loop. The cache is touched only from the draw thread
(`get()` ← `loadSubDiv` ← `loadVisibleData` ← `draw`, all serialized), so it needs no lock.
Tracy zones: `strtbl.get` = wrapper (hits+misses), `strtbl.decode` = misses only — compare their call
counts to read the hit rate.

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

`CUiTheme::pinColorScheme(bool)` replaces the application palette with Fusion's so output cannot
depend on the desktop. Once, before the first window; not a scope, not undoable. The palette is what
matters (`paletteIsDark()` reads it): `setColorScheme()` is inert on X11 and `standardPalette()`
asks the platform theme.

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
- **Resolve a cue's colour through `cueColorGroup()`, never the palette's current group.** The
  current group turns `Inactive` while the window is not the active one, and KDE's
  `[ColorEffects:Inactive] ChangeSelectionColor` mutes `Highlight` to near the button face there
  (measured `#1b4155` on `#292c30`), so a cue drawn with the one-argument `color()` overload
  disappears whenever the app loses focus. A cue marks state, not focus, so `Active` is pinned and
  only the disabled dimming is kept.
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

### Applying a new viewport size

A draw thread holds a reference to its buffer with the mutex unlocked, so the buffers cannot be
rebuilt while it runs and a resize has to be deferred. `CCanvas::slotUpdateDrawContextViewport()` is
the only path that applies size and pixel ratio, and three rules keep it honest:

- **Never pass a size captured earlier.** It reads `size()`/`devicePixelRatio()` at apply time. A
  retry runs long after the event that scheduled it, and a queued resize event for an intermediate
  geometry is delivered *after* the synchronously sent event for the final one — the Windows
  fullscreen → maximized sequence does exactly that.
- **All or nothing** (`canResize()` before `resize()`). A partially applied resize leaves the layers
  with different viewports, which shows as map content no longer matching the GIS overlay.
- **`paintEvent()` reconciles.** A context left behind would never be noticed otherwise: a canvas
  shown again at an unchanged geometry gets no resize event, so the divergence would survive panning,
  zooming and view switches until the user drags a splitter.

Retries come from `timerViewport` and from each context's `finished`. `print()` is the one caller
that changes the size to something other than the canvas' own, so it must `waitForDrawContexts()`
before *and* after — its second `draw()` pass restarts the threads.

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

The same trap runs the other way for `*.bat`/`*.cmd` (`text eol=crlf`): a blob committed with CRLF
before the attribute existed reports permanently modified, because git normalises the working-tree
copy to LF and compares against the CRLF blob. `git checkout` and `git stash` — including
`rebase --autostash`, which leaves a half-built `.git/rebase-merge/` behind when it fails — cannot
clear it. To rebase without committing anything, put `<path> -text` in `.git/info/attributes`
(local, untracked, beats the tracked `.gitattributes`), rebase, delete it, then
`rm <path> && git checkout -- <path>` so the file is re-materialised with the attribute's endings.
`git ls-files --eol '*.bat'` lists the offenders: `i/crlf` is a blob still awaiting
`git add --renormalize`. `msvc_64/build_routino.bat` is one.

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

## Tile cache

`CDiskCache::cleanupRemovedMaps()` (from `CMapDraw::loadMapList()`) **deletes the cache directory of
every map the configuration does not know**, and `defaultCachePath()` ignores `--config`. A scripted
run must call `CMapDraw::setCacheRoot()` and `CGisListWks::setDatabasePath()` before `CMainWindow`,
or it prunes the user's tiles and empties their workspace.

That root covers startup only: `Canvas/cachePath` in the configuration wins afterwards
(`saveMapPath()` writes it on every exit). `shots.py` injects it so session and replay share one
cache. A doc run without `--config` is refused (`CShotEntry::prepare()`): it would write the user's
settings.

**A failed tile is a hole until the `CDiskCache` is replaced.** A failed or undecodable reply, or a
cache file that does not load, stays in `cache` as the 256 px transparent dummy. Keep it there:
without it `contains()` is false and a failing server redraws forever; a null image would set
`tileSizePx` to 0. `restore()` returns false for a hole, `CMapTMS`/`CMapWMTS::draw()` count holes
per draw (`failedTiles()`), and learn `tileSizePx`/`tileScale` only from real tiles — learned from a
hole, a 512 px source flips it every draw.

### The map list outlives its `CMapDraw`

`mapList` is parented to the canvas but deleted via `CCanvas::destroyed` → `deleteLater`, while
`CMapDraw` dies with the canvas, so until the next event loop turn every `CMapItem` holds a dangling
`CMapDraw* map`. Anything a `CMapItem` queues must watch its owner — `loadConfig()`'s 100 ms deferred
activation carries a `QPointer<CMapDraw>`. `CPrintDialog`'s short-lived canvas hits it.

---

## Dock widgets

`QDockWidget::setFeatures()` disables `toggleViewAction()` unless `DockWidgetClosable` is in the set,
so a docker missing that flag has a permanently greyed-out *Window* menu entry — and a floating one
loses its close button too. Every docker in both apps must keep `DockWidgetClosable`.

---

## Minimum sizes — a preference is not a requirement

Qt enforces `minimumSizeHint()` on a window with a layout, so anything that inflates it decides the
smallest the main window can be. The track details page once pinned it to 1648x717 — unusable on a
1366x768 screen — through four separate places that read a *preferred* width as a *required* one.
All four are easy to write again:

- **A `QLabel` without `wordWrap` reports its whole unwrapped string as a minimum.** With `wordWrap`
  it reports its widest word — 436 → 52 for one filter label. Qt 6 turns `hasHeightForWidth()` on by
  itself there; the `setSizePolicy()` line usually recommended alongside is measured to change
  nothing. Give any label holding a sentence `wordWrap`, especially one whose text is set at runtime
  or translated: `labelInfo`'s minimum used to move with the interface language.
- **`QSplitter` sizes a child through `qSmartMinSize()`**, which takes `qMax(sizeHint,
  minimumSizeHint)` unless the child's size policy carries a `ShrinkFlag`. `MinimumExpanding` and
  `Minimum` have none, so the child's preferred width becomes a hard floor — 222 px of the details
  page. Use `Expanding` or `Preferred` for a splitter child that may shrink.
- **A `QTreeWidget` row is as high as the item's size hint, which does not follow the width.** A
  wrapped label in an item widget is clipped as the tree narrows; recompute the item size hints from
  `heightForWidth()` on resize, as `CDetailsTrk::updateFilterRowHeights()` does.
- **A combo box beside a field frame adds both widths.** Stacking it above cost the speed filters
  nothing and saved 190 px.

Measure before believing any of this of a given widget: walk the tree printing `minimumSizeHint()`,
and remember that a `sizeHint()` is what the widget *wants*.

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
- `helpers/COverviewAdvisory.{h,cpp}` — the whole concept: `advice_t`, `file_info_t`,
  `geometry_t`, `read_deadline_t`, `probe()`, `setGeometry()`, `onRenderTimeout()`, suppression
- `helpers/CGdalVrtUtil.{h,cpp}` — small GDAL conveniences only (`closeDataset()`, `isFileUtf8()`,
  `allReferencedFilesExist()`, `toMeters()`, `progressCallback()`). Nothing advisory lives here
- `helpers/CVrtAdvisoryDialog.{h,cpp}` + `.ui` — the fix/info/combine dialog
- `helpers/CVrtCombiner.{h,cpp}` — "Combine files..." grid-split/footprint logic
- `dem/CDemVRT.{h,cpp}`, `map/CMapVRT.{h,cpp}` — each hold one `COverviewAdvisory advisory` and no
  advisory state of their own
- `dem/CDemWCS.cpp` — opts out via `advisory.setEnabled(false)`
- `map/IMap.h`, `dem/IDem.h`, `map/IMapItem.h`, `map/CMapItemDelegate.{h,cpp}` — badge + on-demand info
- `map/CMapItem.cpp`, `dem/CDemItem.cpp`, `map/CMapList.cpp`, `dem/CDemList.cpp` — context-menu entry
- `canvas/CCanvas.cpp` — owns and shows the dialog

**Invariant: the dataset is always a VRT.** `new CMapVRT`/`new CDemVRT` happen only for `.vrt`
files; every other format has its own class, and `CDemWCS` opts out. `probe()` relies on this —
there is no "concrete raster format" branch.

**Invariant: the advice is immutable per instance.** Any DEM/map list change — including a
successful Fix/Combine (`sigContainerRebuilt` → `setupDemPath`/`setupMapPath`) — destroys the
instance and creates a new one that probes again; nothing updates in place. `probe()` caches
`needsAttention()` so the delegate's per-paint badge poll (`showsWarning()`) is O(1) instead of
walking `perFileInfo`.

**Trigger:** `draw()` wraps `ReadRaster()` with a 5 s deadline (`read_deadline_t` + GDAL's own
progress-abort hook — do not add a second warp-options progress callback, it froze the UI). On
timeout `onRenderTimeout()` fires the advisory (render thread → GUI thread) once per loaded
instance per session, and only when `showsWarning()` — the same condition as the proactive tree
badge.

**Never request a redraw from a timeout path.** The view is unchanged, so the retry hits the same
deadline and retries again — `emitSigCanvasUpdate()` → `slotTriggerCompleteUpdate()` → `update()` →
draw thread, unthrottled. On a dataset that stays slow that is an unbounded loop, seen as a
never-finishing calculation with a blinking layer bar.

**The dialog is application-modal** (`setModal(true)`): while it is open the map/DEM it is about
must not be read. A pan's `draw()` or a mouse-move's `getElevationAt()` racing a Fix/Combine file
rewrite (an external gdaladdo/gdalbuildvrt process — the in-process dataset mutex cannot guard it)
crashes GDAL. Modal blocks user input; a background redraw from a sibling layer during a job is a
known unguarded residual.

### `COverviewAdvisory::probe(dataset, band, isPaletteIndexed, maxFactor)`

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

### A "source file" is a leaf raster, not what `GetFileList()` returns

`GetFileList()` reports **one level only**. Sources with heterogeneous projections are commonly
wrapped one `gdalwarp -of VRT` each and then `gdalbuildvrt`-ed, so the container's sources are
themselves VRTs. `collectLeafSources()` follows those down to the real rasters; `perFileInfo`,
`filesToFix()`, the subfile count and the disk-usage sum all run on the leaves.

Measured on GDAL 3.12:

- **`gdaladdo` on a warped VRT builds nothing.** It ignores `-ro`, writes `<OverviewList>` into the
  `.vrt` itself and exits 0 in ~40 ms. Those overviews are resampled from the source on the fly, so
  reads still hit the full-resolution leaf — and `probeSource()` reads the declaration back as
  sufficient, clearing the badge for good. On a **plain** VRT the same command builds a real
  `.vrt.ovr`.
- **A warped VRT needs no `<OverviewList>` of its own** — it reports its source's overviews. Never
  add a step that writes one into an intermediate VRT.
- Leaf `.ovr` is the only thing that matters. 10000×10000 warped source, full extent into a 1200 px
  buffer: 1.20 s bare, 0.41 s with the fake `<OverviewList>` alone, 0.03 s with a real leaf `.ovr`.

A nested VRT with its **own** `.ovr` is a leaf — real data, the rule step 1 applies to the container.
Nesting alone is not a defect: a VRT-of-VRTs over leaves with overviews is the fastest layout above,
so never warn on the nesting itself.

`isVrtFile()` asks `GDALIdentifyDriver()` (header sniff, ~5 µs), not the file extension.

### "Fix overviews"

`fixPlan()` is the single source for the after-fix table, the summary counts, the confirmation
dialog, the gdaladdo queue and the failure cleanup. It returns `{path, action}` steps —
`BuildNew`/`CleanRebuild` on a source raster, `AddList`/`UpdateList` on the container. Never derive
any of those five from `perFileInfo` again; that is how they drift apart.

`slotFixOverviews()` / `finishFixOverviews()`:

1. `gdaladdo` runs on every source short of `suggestedLevels` (or on `filename_` itself if there are
   no source files). Recipe: `-r` (nearest for palette, average otherwise),
   `COMPRESS_OVERVIEW=DEFLATE`, plus `PREDICTOR_OVERVIEW=2` for non-palette data — matches the
   source predictor for a ~2–3× smaller `.ovr`, harmful on palette indices. `.ovr` block size is
   inherited from each source automatically.
2. `fixContainerOverviewList()` rewrites just the `<OverviewList>` element to
   `advice_.suggestedLevels` — no full `gdalbuildvrt` re-run, no re-probe. Container steps carry no
   command; they are this XML edit.

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
- **Offered only when every source sits in one directory** (`sharedSourceDir()`). Tiles are written
  beside the sources and the container is rewritten to reference them, so sources spread over
  several directories have no right answer — the button is withdrawn and the warning says why.
  *Fix overviews* has no such limit: each `.ovr` lands beside its own source wherever that is.
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
- **Table sizing:** `sizeTableBrowser()` sizes every table browser, capped at `kMaxTableWidth` (900)
  and `kMaxTableHeight` (250); past either the browser scrolls and the dialog keeps its size. Pass
  it the narrow-tag HTML: `idealWidth()` on the `width="100%"` variant returns the widget width, not
  the content. Row counts include the header row, plus the container's synthesized row for the
  current-state table.
- **Confirmations:** the fix plan goes through `confirmList()` (`QDialog` + `QTextBrowser` +
  `QDialogButtonBox`) because a `QMessageBox` cannot scroll its text. `confirmYesNo()` is a
  `QMessageBox` and takes plain text only.
- `raster_geometry_t` comes from `CGdalVrtUtil::sourceGeometry(pre-warp source)` — the source file's
  own size and resolution, matching `gdalinfo` (exact metres for a projected CRS,
  `kMetersPerDegree` approximation for geographic). **Not the warped grid**: reading the warped
  geotransform gives wrong pixel sizes (+26% for a UTM source drawn in EPSG:4326).
- **Badge and info:** `showsOverviewWarning()` (`!suppress && needsAttention()`) drives the tree
  badge; `hasOverviewInfo()` drives the "Overview Info..." context-menu entry regardless of
  attention state. `CMapItemDelegate::overviewBadgeRect()` is shared by
  `paint()`/`editorEvent()`/`helpEvent()` so painted, clickable and tooltip areas cannot drift.
- **Lifecycle:** `closeEvent()`/`reject()` both confirm-cancel a running job and clean up partial
  output. `CCanvas::showOverviewAdvisory()` allows **one advisory at a time, application-wide** —
  the dialog is `show()`n, not `exec()`d, so the event loop keeps running and modality blocks only
  user input, never a queued `sigOverviewAdvisory` from another layer's render timeout. It searches
  from `CMainWindow`, not the canvas, because a second canvas parents its own dialogs. A dropped
  advisory keeps its tree badge, so the file stays reachable through *Overview Info…*.

---

## Open work

Analysed designs live in `.notes/`, each wanting its own branch. De-freeze one by pointing at the
file.

- `QMS-1135-overview-restore-plan.md` — transactional rollback when a *Fix overviews* job is
  cancelled.
- `waypoint-icon-resolution-plan.md` — 32 → 96 px waypoint icons, gated on storing `icon_t::focus`
  relative.
- `doc-image-publish-plan.md` — `shots.py publish` and the panel's Publish exist (#1254); left: its
  "Left to do".
- `QMS-1251-recorder-signals-plan.md` — the recorder. `shots.py selftest` verifies it.
- `QMS-1257-state-process-plan.md` — #1257 committed; left: the live checks and one proposal.
- `QMS-1266-per-page-base-plan.md` — each page owns a base, copied once at creation, and a fixture
  holding only what differs from the default. Implemented; the plan keeps the design.

### Documentation subsystem (QMS-1217)

`.notes/QMS-1217-documentation-images.md` is the one plan: §1-§8 design, §9 evidence (measured, do
not re-derive), §10 what the demo does not do, §11 the sub-tickets.

**One branch, one commit per sub-ticket.** #1245-#1257 are commits on `QMS-1217` (based on `dev`),
never branches of their own. #1245-#1257 and #1266 exist: hermetic run, render path, shot file, exposure
catalog, fixture, `shots.py`, recorder, row buttons, replay queue, launcher/panel/channel, state process
and F9, writer-facing labels, context-menu shots, a base and fixture per page.

```
doc/pages/<page>.md            the only source of shot names
doc/shots/<page>.json          the shots and the recorded scenarios
doc/shots/<page>/<name>.ini    one scenario's whole configuration
doc/shots/<page>.ini           the page's base, copied once when the page was created
doc/shots/fixtures/default/    the fixture; its shots.ini only seeds a new page's base
doc/shots/fixtures/<page>/     what the page's fixture differs in, one part (maps/, projects/...) each
```

The writer's guide is `doc/WRITING.md`: short, facts only, updated with every change a writer sees.

#### Build and entry

- **Developer-only, behind `-DQMS_DOC_MODE=ON`.** No source tests the option: CMake compiles
  `shoot/CShotEntry.cpp`/`CShotOptions.cpp` or the `*Stub.cpp` pair, and `shoot/fonts.qrc` only
  with it. A user's binary rejects `--shoot`, `--doc`, `--color-scheme` and companions through
  `QCommandLineParser`; `main.cpp`, `CCommandProcessor` and `CAppOpts` carry no `#if`.
  `src/qmaptool/setup/` never had the switches.
- **`CShotEntry` is all `main.cpp` knows**: `pinEnvironment()`, `createApplication()`, `isDocRun()`,
  `prepare()`, `run()`. `CShotOptions` defines and reads every switch; they reach the app as
  `CAppOpts::doc`, empty without the subsystem.
- **`prepare()` runs before `CMainWindow`** and sets the cache root, the workspace database
  (`CGisListWks::setDatabasePath()`), `CUiTheme::pinColorScheme()`,
  `CQmsStyle::pinThemeIndependentHints()` and registers the bundled DejaVu
  fonts (offscreen on Windows has no font database). It fails without `--config`, and `main()`
  exits 1.
- **A doc run skips the splash and `CSingleInstanceProxy`**, which would hand the arguments to a
  running QMapShack and exit.
- **`pinEnvironment()` sets `QT_QPA_PLATFORMTHEME=generic` except on Windows**, where a demo run
  with it crashed (access violation) and there is no desktop theme to keep out. macOS unmeasured.
- **A Windows build owns no console** (GUI subsystem). `attachParentConsole()`, first thing in
  `main()`, attaches to the parent's console for `--shoot`/`--doc` and reopens a stream on `CONOUT$`
  only if its handle is not already a pipe or file. Untested on Windows. Anything the writer must
  see is a dialog, never only a log line.

#### Rendering (`CShotWriter`)

- **Render at dpr 1, never `QWidget::grab()`**, which uses the widget's ratio. A canvas stays the
  exception: `IDrawContext` sizes its buffers by the widget's ratio, so a HiDPI map is resampled.
- **An opaque picture carries no alpha**: `renderAtDpr1()` converts to `RGB32` when no pixel uses
  it. Picture weight ships in every user's `.qch`.
- **Resize a window, never a widget inside one**: a child resized to its hint stays that size and
  the parent's layout is not re-applied.
- **A map is complete when `CCanvas::isDrawComplete()` says so**: no redraw outstanding,
  `drawContextViewportIsCurrent()`, no context running, and only then
  `CMapDraw::pendingTiles()` — `drawt()` holds `CMapItem::mutexActiveMaps` for its whole run.
- **`settleStable()` asks `isDrawComplete()` before it renders, never after**, or a draw finishing
  mid-render passes with the load indicator in it. It skips a canvas not `isVisibleTo(w)` and
  refuses one that is but not `isVisible()` (a hidden canvas never clears `needsRedraw`). It also
  refuses a picture with any `failedTiles()`.
- **`processEvents(flags, ms)` does not wait**; wait with a `QEventLoop` quit by a `QTimer`.
- **A main window picture depends on its resize history**: a window's minimum silently wins, and
  returning to a size can shift the right dock column by 1 px. Two runs on fresh configurations are
  byte-identical.
- **No `MainWindow/geometry` maximizes the window 500 ms after construction**; `CShotRunner` waits
  1000 ms before the first shot.
- **`-platform offscreen` takes no parameters**; resize the window instead of pinning a screen size.
- **Close the main window before leaving `exec()`**: destroying it while shown hides the docks after
  `CMainWindow::docks` is gone (SIGSEGV on exit).

#### Shot files and exposures (`CShotPage`, `CShotRegistry`)

- **A window's size has one record, the shot's `size`**, applied to the main window or a window
  only; a shot the main window sizes must give one (exposures exempt). `layout` holds `saveState()`,
  the tab and every splitter's state, never `saveGeometry()`. `shootOne()` resizes before
  `restoreState()`, because dock extents are pixels.
- **A scenario shot must have a `size`: the main window's**, applied before the replay. A window a
  step opened is rendered at its own size hint.
- **One scenario per process.** `CShotPage::run()` refuses a scenario shot `--only` matches unless
  `--shoot-scenario` is given; `shots.py` starts one process per scenario with its own `.ini`. Only
  the same scenario is replayed again in a process — measured with a details tab: the second replay
  reuses the tab the first opened (self test).
- **The `tab` index and splitter states are applied with the leading `layout`, before the steps**:
  they are taken when the recording starts, so applied last they undo a step (Edit's details tab).
- **A context menu is photographed by its shot, not its scenario**: Ctrl+Shift+F9 on a menu a
  context request opened stores that request's step as the shot's `open`
  (`CShotRecorder::contextMenuStep()`, recording or not); `shootOne()` appends it to the steps and
  takes the picture inside the menu's `exec()`. A row step reveals a row under a collapsed parent
  (`reveal()`, `scrollTo()` expands it), so a menu taken on a row expanded by hand replays. A shot with
  `open` sizes the main window before the steps and renders the menu at its own size, like a scenario's. The
  highlighted entry is stored as `active` (the action's `objectName`, searched in `menu->actions()`:
  a context menu's actions belong to the view that built it) and set with `setActiveAction()`.
- **A canvas step waits for `isDrawComplete()` first** (`CCanvasHandler::settle()`): items' pixels
  are updated by the draw, so a `hit` or click right after a wheel zoom finds nothing (measured: 4 of
  8 replays of a zoom-then-select scenario failed without it).
- **A `set` goes through `driveProperty()`**, which checks `indexOfProperty()` (an undeclared name
  becomes a dynamic property) and reads the value back. It is put back after the picture, newest
  first, also on failure.
- **`addressOf()` returns `std::optional`**: empty string is the main window, nothing is outside it.
  `CCanvas::zoom()` clamps, so `applyView()` reads the view back. A view is centre plus zoom level,
  never a rectangle (`zoomTo()` snaps).
- **An objectName stops being an address once a second widget takes it** (the map tree becomes
  `IMapList/CMapTreeWidget#0`). `sliderOpacity`/`label_3` exist in the workspace dock and in every
  map row, so the workspace slider is `IGisWorkspace/QSlider#0`. `key_` names are no addresses:
  `CCanvas::generateKey()` hashes the clock.
- **`--only` is not a path glob**: `*` crosses `/`. Quote it.
- **An exposure is built by `shootOne()` and deleted when the shot returns.** Its `TYPE` needs
  `Q_OBJECT` or `SHOT_EXPOSE` does not compile; the built widget is checked against
  `TYPE::staticMetaObject`. A duplicate id fails every `--shoot` run.
- **Every comma in a `SHOT_EXPOSE` lambda must sit inside parentheses**; a brace list or template
  argument splits the macro. Move such a value into a helper in the anonymous namespace.
- **A value a dialog keeps a reference to is a static of its own factory, reset per build**; shared
  per type it leaks into the next exposure.
- **A fixture exposure gets what its real call site passes** (`CInputDialog` as `CDetailsWpt` asks,
  route/area dialogs named after the track). `CSelectProjectDialog` is built without the workspace
  tree. `CPrintDialog` is not exposed: its canvas is hidden and refused.
- **Some exposures show the machine**: `CAbout` (library versions), `CWptIconDialog`
  (`Paths/externalWptIcons`), `CSetupDatabase` (QMYSQL driver), `CPhotoViewer` (`showMaximized()`).
- **No `details` step**: a track's or project's `edit()` adds a tab, so the recorded input that
  opens it is the record; every other `edit()` is a modal `exec()` and belongs to the catalog.

#### Fixture (`doc/shots/fixtures/`, `CShotFixture`)

- **Plain git, not LFS:** keep rasters cropped and compressed.
- **A page's fixture holds only what differs** (`shots.py fixture_part()`): a part in
  `fixtures/<page>/` holding something replaces the default's part whole; an empty or missing one is
  the default's, and `SOURCE.md`, `README.md` and dotfiles are no content. The rule exists twice,
  `fixture_part()` and `CShotFiles::ownFixtureParts()`, each naming the other - change both.
  `default` is no page name. Every session start adds what the folder lacks: an empty folder per part
  and a git-ignored `README.md`. The panel header says which parts are the page's own, and the file
  watcher updates it when a part is filled or emptied.
- **A page owns its base**, `doc/shots/<page>.ini`. The launcher asks which base to copy when a page
  has none (`chooseBase()`); Base / Save writes it, Base / Copy replaces it. Nothing writes a
  fixture's `shots.ini`. `compose` falls back to the default's only for a page without a base yet.
- **`CShotFixture::load()`** loads the project the configuration names as `Shoot/fixtureProject`
  (`compose` writes the page fixture's first `*.qms`), waits for
  `CGisListWks::isWorkspaceLoaded()` (the restore runs ~1100 ms after `CMainWindow` and does not
  check duplicates) and `IGisProject::isLoading()`, refuses a workspace already holding the project,
  and hands the first trk/wpt/rte/area to `CShotContext`.
- **`shots.py compose`** adds to the base the absolute `Canvas/cachePath`, `mapPath`,
  `demPaths`, `poiPaths`, `Route/routino\paths`, `Shoot/fixtureProject` and a `Database/Entries` on
  a scratch copy of the fixture's first `database/*.db`, plus `Database/saveOnExit=false`. A scenario `.ini` is a whole
  configuration, never a patch; settings are read in constructors, so one process per scenario.
- **A map or DEM activates from the configuration only with more than two keys**
  (`noShadowConfig()`): store `opacity`, `minScale`, `maxScale` with `isActive`.
- **The DEM draws over maps and POIs**; the base uses opacity 20.
- **Tile files expire during a run** (`cacheExpiration`, 20 s timer) and come back as holes
  offline; refresh modification times on an old cache first.
- **Network reachability changes the right dock column** (database vs routing dock height), cause
  unknown.
- **No configuration sets checked POI categories** (`CPoiFilePOI::categoryActivated`) or the
  database tree's expanded folders; a scenario records them.

#### `doc/tools/shots.py`

- **Starts every headless run**: `replay`, `selftest`, `unused`, hidden `compose`. Python 3.9,
  standard library only. One process per page and scenario group into `doc/images/_check` (or
  `-o`), pinned command line and environment (`QT_QPA_PLATFORMTHEME=generic`, `LANGUAGE`,
  `--locale`), scratch working directory, killed after 600 s.
- **A run's result** is its `shoot:` warnings on stderr, its exit code (failure count) and whether
  `<id>.png` came out in its render folder (`doc/shots/_cache/render-*`); only one that did replaces
  `<out>/<id>.png`. `-v` adds `-d`: `qDebug` lines, including one per input frame.
- **`selftest`** (`--shoot-selftest`, `CShotSelfTest`) takes no pictures: it drives the app through
  `CShotSynth`, compares the recorded steps, replays them and compares the state. Exit code = failed
  cases.
- **The self test has a base of its own**, `doc/tools/selftest.ini`, with the default fixture: it
  checks the recorder, so a page's base can change without moving a case. `--page` only names the
  run. Its cases assume that base's arrangement; changing it means fixing the cases that break.
- **Directories come from the application** (`-d` prints `"CACHE"` and `"USER DATA"`). A running
  QMapShack is detected by `USER DATA/.QMapShack.lock` (`.lock` on macOS); `replay` refuses while it
  is held and never creates it. The leak guard compares mtime and size of everything below both
  directories before and after each run, `replay` and `selftest` alike.
- **SIGTERM is an exception in `shots.py`** (`sys.exit`), so every run kills the application it
  started and removes its scratch directory; SIGKILL leaves both behind (measured). `CShotsJob`
  therefore terminates before it kills, except on Windows, which has no such request for a console
  program.
- **INI for QSettings**: Qt key form (`routino\paths`), forward slashes, every path quoted (comma
  splits, semicolon comments).
- **A shot file is written as `json.dumps(indent=4, sort_keys=True, ensure_ascii=False)` plus a
  newline**, identical to `QJsonDocument::Indented`.
- **Output is UTF-8** (`CLogHandler`); decode it as such, not in the console code page.

#### Recorder (`CShotRecorder`, `CShotApplication`, `IShotHandler`, `CShotHandlers`)

- **A recording is its start state (`layout`, `view`) plus every change as a step**; nothing is
  taken at `stop()`. It always starts from the base.
- **`notify()` wraps a whole delivery**, so `CShotApplication` numbers a frame around each
  spontaneous input and the recording is sorted by frame: a step inside a dialog a slot opened is
  recorded first but carries the higher number. The doc run's `QApplication` comes from
  `CShotEntry::createApplication()`.
- **A step is kept only when the input went to the control that made it**
  (`inputReached()`; for an action, a widget it sits in). A signal fired while input went elsewhere
  is the application's answer, which replaying that input repeats.
- **One handler per class**, keyed by `QMetaObject`, nearest base wins. Connect once per object
  (`CShotRecorder::watched`); `Qt::UniqueConnection` refuses a lambda. Handler state that outlives a
  call is keyed by recorder too.
- **Record the intent, replay the input, check the outcome.** Checkables record `checked`; a combo
  is `set currentIndex` replayed with `activated`/`textActivated`; typing is `key` with the final
  text (select-all, Backspace, type, Enter); a spin box follows its line edit's `textEdited` plus
  the value signal (`updateEdit()` writes behind a `QSignalBlocker`); a date edit is `set dateTime`;
  a tab is `currentChanged`, counted for input in the tab bar and for Ctrl+Tab in a page only (a
  switch after a click in a page is the application's answer; a close retracts the switch it
  caused); a moved dock is `arrange` with
  `QMainWindow::saveState()` (`dockWidgetArea()` lies while floating).
- **A key press no handler made a step of is `keypress`**, held until the next frame because a
  matched `Shortcut` arrives after its key press; a button clicks at Space's release, so judge there.
  A mouse move does not end it: Space held while the pointer moved was a `keypress` and a `click`.
- **Editing ended by focus loss is `endedit`** (`recordOutcome()`) at the input that moved focus.
- **A surface is recorded as its input** (`CSurfaceHandler`): press to release is one `click`,
  `drag` or `dclick`, ordered at the press, in the surface's units (degrees plus item hit, plot x
  value, icon name); the release is a pixel offset. `held` uses the monotonic clock like
  `CMouseAdapter`. A hover is one amended `move`. A gesture ends when an event shows its button up; at
  `stop()` a held one is dropped with a warning. Something happening mid-gesture splits it into
  `press`, `move`, …, `release` (`CMouseAdapter::wheelEvent()` sets `ignoreClick`).
- **A widget with its own vocabulary is never a position**: canvas (geographic point), `IPlot`
  (`xValueAt()`/`pointOfXValue()`, the plot's own edge rule), `CIconGrid` (`iconAt()`/`rectOfIcon()`),
  row buttons (delegate name). Not yet covered: `CDateTimeEditor`, `CPhotoViewer`,
  `CRouterBRouterTilesSelectArea`. An `IScrOpt` overlay is an ordinary child widget of the canvas.
- **`IPlot` sets no `objectName`**; its mouse-focus owner tag is `ownerTag`. A code-built plot is
  addressed positionally (`framePlot/CPlotProfile#0`).
- **A menu entry is addressed by its action's `objectName`, never its text.** Name actions after the
  member; data-built menus use a prefix plus an untranslated key (`actionActivity_<act20_e>`,
  `actionColor_<GPX name>`, `actionWptIcon_<sym>`). POI entries: `IPoiItem::file`/`key`, where
  `file` is the base name plus the first 8 characters of `CPoiFileItem::key` (MD5 of the file's first
  4096 bytes), so namesakes differ and it is the same on every machine; a map POI by name and
  position. Every new menu entry needs one; nothing fails when it is missing — the recorder warns and
  drops the step.
- **A menu shown with `exec()` must be deleted when it returns**, or its leftovers shift addresses
  to `QMenu#n`.
- **A pick in an open menu is replayed as a click on the entry, not `trigger()`**: callers read
  `exec()`'s return. `QMenu` ignores a press unless the pointer moved enough, so
  `CShotSynth::arrive()` uses 8 moves. Replaying `trigger` closes open menus first. A disabled action
  ignores `trigger()` (`IPlot` enables its actions only while the menu is open), so that fails.
- **`openmenu`** is a menu opened by a tool button, menu bar or submenu, named by the menu's
  `objectName`; timer-opened ones are ordered at the last frame.
- **A completer popup has no parent**: its input counts for the line edit (`amend(..., alsoThrough)`).
  Enter in it fires `returnPressed`. Replayed typing hides it.
- **The Menu key's context menu is made by the platform plugin**, none offscreen; the self test
  injects it via `QWindowSystemInterface` (`Qt6::GuiPrivate`).
- **A mouse context menu goes to `qt_last_mouse_receiver`** and Qt drops a move to the current
  position, so replay moves the pointer onto the place first (`CShotSynth::arrive()`).
- **Replay and `CShotSelfTest` use `CShotSynth`, never QTest's `QWidget` functions**, which bypass
  window routing (grabs, popups, double-click synthesis, focus). Replay refuses a point another
  widget would get (`CShotSynth::missed()`).
- **Qt delivers a double click's second press only as `MouseButtonDblClick`**: an item view emits
  `clicked` then `doubleClicked`; the handler folds the `select` into one `dclick`, taking back the
  first click past hover steps. A delegate that takes the double click still leaves a `clicked`,
  which is no `select`.
- **A menu entry's own slot runs before `QMenu::triggered`**; frame numbers still order `trigger`
  first.
- **The wheel over a list goes to its viewport**: an item view's scroll is `scroll` (top row,
  `scrollTo(PositionAtTop)`), another scroll area's a share of its range.
- **A line edit's cell-editor status is decided when typed into**, not when watched.
- **Row buttons** (`CRowButtonHandler`): a `click` with `row` and `button` from the delegate's
  `sigButtonPressed`, emitted only where a press acted; the `select` the release makes is taken
  back. The row is named at the press, before the delegate acts. Any mouse button acts (`mouse:
  right`; its context menu is no step). A double click on a button is one `dclick` with `button`.
  Replay clicks `buttonRect()`, built by `delegateOptionOf()` (rect, font, `State_HasFocus`), and
  checks the signal. Database and map rows are named by what they show
  (`CShotAddress::namePathOf()`); a name with `/` or used twice has no path. Workspace device and
  geo search rows have none, so their buttons are dropped with a warning.
- **Not recorded**: a window closed by its title bar, re-docking through a window frame, a main
  window separator drag (reported), typing into a cell editor, touch (reported).
- **A replay starts from nothing**: `CShotReplay::replay()` calls `clear()` first and `shootOne()`
  after the picture, because steps are not idempotent. `clear()` calls `CCanvas::abortMouse()` (the
  screen options), `resetMouse()`, `slotStopRange` on a plot `isSelectingRange()` and `slotResetZoom` on one
  `isZoomed()` (a plot owns the range it started, so the track refuses any other reset; on an idle plot
  `slotStopRange` resets the track's mode and filter tab, and a details tab replays differently) and must
  `sendPostedEvents(nullptr, QEvent::DeferredDelete)` —
  `CMouseRangeTrk`'s destructor returns the track to `eModeNormal`, and `processEvents()` does not
  deliver `DeferredDelete`.
- **The deadline ends the process** (`std::_Exit(kDeadlineExitCode)` after the message): a step that
  never returns keeps every loop below it, `perform()` included, from returning — measured with a
  click that entered an endless `QEventLoop`: message at 60 s, exit code 1.
- **The picture is the replay's last step** (`whenReady`): a dialog a step opened is still inside its
  `exec()` there. Popups and modal dialogs are closed after it, or the loops below cannot return.
- **The next step is queued before a step runs, and may run inside it only when that step waits in
  an `exec()`**: `QThread::loopLevel()` above its level at the start and a popup or modal up that
  was not up when the step started (its own waits inside an open menu run a loop above it too). A popup
  alone is not enough — measured: `CShotSynth::mouse()` on a menu bar delivers the queued steps
  before it returns, at the same loop level with the menu up, so they close the menu before the
  click's own check.
- **Steps start only once the start state's map is drawn** (`settleStable()`): during a redraw the
  draw thread holds `CDemItem::mutexActiveDems` and `CDemDraw::getElevationAt()` answers `NOFLOAT`,
  so the status bar of the first replay lacks the elevation later ones show.
- **A `trigger` whose slot runs `exec()` is recorded only once that dialog closes** — measured:
  `CMainWindow::slotSetupUnits()`'s `exec()` returns before the recorder's `triggered` lambda runs.
  A recording stopped with the dialog open lacks it.
- **Closing an `exec()`'d menu by recorded input splits a held gesture** on the surface that opened
  it: `press`, `keypress`, `release` instead of one `click` (measured on the plot's menu).
- **`settle()` ends every finite running animation**; animations run on wall time. Endless ones are
  left alone.

#### Launcher, panel, channel (`CShotDocLauncher`, `CShotDocPanel`, `CShotDocState`, `CShotsJob`, `CShotFiles`, `CShotDocMode`)

**Structure: no flags, only objects that die with what they describe.** A flag that outlives the start
or run it belongs to is the bug this subsystem invites; keep what runs in these objects:

- **`CShotDocState`** is one state process: `QProcess`, channel socket and the one follow-up command
  sent after its first `ready`. The launcher holds it in a `QPointer`, replaces it with `stop()` +
  `deleteLater()`, and derives busy/recording from it. Every outcome is emitted queued and dropped
  after `stop()`.
- **`CShotsJob`** is one `shots.py` run: `finished(bool, error)` exactly once, always queued - also
  for a missing interpreter or a failed start - then it deletes itself. The finished handler clears
  the launcher's `QPointer` before anything reads it.
- **`CShotFiles`** is one page on disk and owns every rule: name validation, what a rebind loses,
  rename order with rollback, pictures deleted only after the shot file is committed (`QSaveFile`),
  "unused" over every page. The launcher only asks the question and calls it.
- **A shot id or scenario name that leaves its directory has no path** (`CShotFiles::staysInside()`:
  no `..`, `.`, empty part, leading `/`, `\` or `:`): every path builder returns empty, and
  `CShotWriter::write()` refuses it. `shots.py`'s `stays_inside()` is the same rule; change both.
- **A shot file with `shots` not a list or `scenarios` not an object is refused** like one that is no
  JSON: read as empty, the next write would drop it. The panel shows `shotFileProblem()` first.
- **A revert entry that does not parse reverts nothing**: read as `{}` it would remove the shot.
- **Tests:** `shots.py selftest` also runs `CShotDocSelfTest` (page rules on a temp checkout; job and
  state handles against real processes, including failed starts and stop with reports queued).
  `python3 -m unittest doc/tools/test_shots.py` covers `publish`, `unused --delete` and page names.
- **`command()` is a writer's action and says "not running" when it cannot be sent; `notify()`**
  (`sync`, `select`) is housekeeping and stays quiet, so it never overwrites the status of the action
  it follows. Without a listening channel no state is started: it could never report ready.
- **Picture references need `QRegularExpression::UseUnicodePropertiesOption`**: without it Qt's `\w`
  is ASCII and `images/p/größe.png` does not match, while Python's `re` does (measured, Qt 6).
- **A picture in `_work` is written through `QSaveFile` and published with `os.replace`**, so publish
  never moves half a picture and a picture taken again meanwhile is a new file, not lost.
- **The state process ends through `CShotDocMode::leave()`**: on `disconnected`, on `errorOccurred`
  while not connected (a failed connect emits no `disconnected`, measured), and on its window's close.
  It closes the main window, calls `exit(0)` and after 2 s `std::_Exit(0)`: a state started without a
  launcher ran `exit(0)` before `exec()` and stayed up until killed (measured).
- **A `QMessageBox` meant to have no button gets `setStandardButtons(NoButton)` before its first
  `show()`.** Shown without buttons it adds an OK button as its escape button; removing it after
  `show()` leaves Escape on a deleted button (measured: SIGSEGV) and lets a close request through.
- **A page lists and rebinds only its own ids (`<page>/<name>`)**; another page's picture referenced
  here belongs to that page's shot file.
- **A nested `QEventLoop` does not deliver a `DeferredDelete` posted outside it** (measured in
  `CShotDocSelfTest`); a test that checks a `deleteLater()` must send it with
  `sendPostedEvents(nullptr, QEvent::DeferredDelete)`.

- **Two processes.** `shots.py take <page>` starts the launcher: `--doc <repo> --doc-page <page>
  --doc-python`, on screen (`pinned_env(offscreen=False)`). It owns the panel and every file operation
  (`CShotFiles`) but the ones the state process does itself - the base configuration, a recording, and
  the shot and picture F9 or a region takes; it never renders. Each state
  process is started with `--doc-scenario <name|->` and killed on another pick; nothing is reset in
  place.
- **The launcher's main window is never shown**: `main.cpp` asks `CShotEntry::showsMainWindow()`, and
  `WA_DontShowOnScreen` covers `CMainWindow`'s own 500 ms `showMaximized` when no geometry is stored
  (measured: stays unmapped).
- **Launcher and state need separate workspace databases** (`<page>-launcher-workspace.db`): one
  shared SQLite file logged `database is locked` twice and delayed the fixture from 1.6 s to 12 s.
- **Channel** `QLocalServer` `qms-doc-<pid>`, one line per message, every report handled queued.
  Launcher → state: `region`, `update`, `record`, `stop`, `name`, `discard`, `select`, `sync`.
  State → launcher: `status`, `ready`, `tagged`, `recording`, `recorded`, `recorded-pending`,
  `recorded-none`, `trial-failed`. Before its `ready` a state answers every verb but `select`/`sync` with a
  `status` line.
- **Lifetime, measured on Linux:** state `kill -9` → panel stays and says so; state window closed →
  session ends; launcher `kill -9` → state quits on `disconnected` within 1 s; launcher SIGTERM →
  both end. SIGTERM only closes the main window (`CAppSetupLinux::closeOnSIGTERM`), so both the
  launcher and `CShotDocMode` filter `QEvent::Close` on it.
- **A `QProcess` that fails to start emits only `errorOccurred`**; `CShotDocState` and `CShotsJob`
  turn it into their one queued outcome.
- **`mayClose()` asks, never refuses silently**, and refuses only while a publish runs. Yes publishes
  and ends the session once it succeeded; a failed publish keeps the panel, the next close asks
  again. An ending session skips the question - work pictures stay in `_work` for the next one.
- **The panel's size and position and the application window's position** live in
  `doc/shots/_cache/doc-panel.ini` (`CShotFiles::placementFile()`), applied one event loop after show;
  a position whose screen is gone is ignored. The state saves the window's 300 ms after it stops moving, once ready.
  No `WindowStaysOnTopHint`; `reject()` swallows Escape.
- **Every panel button is a `QToolButton` with its label under the icon** (`newButton()`, labels at
  `kLabelScale` of the panel font), the name leading its tooltip. All share one height, each as wide
  as its label (Record as wide as Stop): one common width set by "Publish all" made the panel 565 px
  wide, so that button's label is "Publ. All" and the panel's minimum is 440 px (measured, DejaVu Sans
  10). The `Doc*` icons are drawn for it. Enabling follows what is possible:
  Rename/Delete need a scenario, Base needs `(base)` (`updateScenarioActions()`); Retake needs a shot
  naming `widget`/`exposure`, Revert a work picture or kept entry, Publish a work picture
  (`row_t::takeable`/`revertable`/`changed`, `updateShotActions()`).
- **Record asks for the name first** (`askRecordingName()`): the selected scenario is the default,
  OK only for a valid name, replacing an existing one asks. Stop stores under it without asking.
- **Selecting a scenario deselects a shot taken in another one; a rebind selects the shot and enters
  its new scenario.**
- **Publish is per shot or per page** (`shots.py publish --only <id>` / `<page>/*`); closing the
  session publishes all of `_work`. A `QFileSystemWatcher` on the page and shot file and their
  folders refreshes the panel 300 ms after an edit made outside it.
- **Every `shots.py` run uses `--doc-python`** (`shots.py take` passes `sys.executable`), never
  `python3` from `PATH` (a Store alias on Windows): `compose` before each state, `replay` for Retake
  and All - both into `_work`, so All marks every row that replays as taken again - and `publish`.
  `_check` is only `shots.py replay`'s default when run by hand.
- **Revert puts the shot's entry back too**: `storeShot()` keeps the entry from before the first take
  since publish as `_work/<id>.shot.json` (`{}` for a new shot); `revertShot()` restores it,
  `shots.py publish` deletes it with the picture it moves.
- **Deleting a scenario or rebinding a shot reduces the shot to its `id` and deletes both pictures**;
  rebind asks first whenever any key but `id`/`scenario` or a picture - also one without a shot -
  would go.
- **A page is a file directly in `doc/pages`**: `replay` reads `doc/shots/*.json` only, so
  `shots.py take`/`compose` refuse a folder part (`page_name()`).
- **Rename moves `<page>/<scenario>.ini` too**; left behind, the scenario reopens on the base. Renaming
  the running scenario restarts the state under the new name.
- **Scenario names are file names and arguments**: no `/ \ : * ? " < > |`, no leading `-`, no
  spaces or dots at the ends, not `-` or `(base)`, no Windows device name (`CON`, `nul.txt`), and none
  that differs from another scenario only in case (`caseTwinOf()`: one file on Windows and macOS).
- **Driving the panel with xdotool on a KDE desk**: `windowactivate` does not raise it over the
  writer's windows; check the window under the pointer belongs to the target pid before every click.

#### The state process (`CShotDocMode`)

- **The state re-applies `MainWindow/geometry` after the fixture is up**, and
  `portableGeometry()` zeroes the screen and its width in stored geometry so `restoreGeometry()` does
  not drop it on another screen size (>25 % difference). `placeWindow()` moves it to
  where the writer left the last state's window. Measured: a base stored at 1200x876 on a 1700 px screen came
  up 1200x872 on a 1280x900 one. `CShotDocSelfTest` fails when a Qt writes another geometry version.
- **(base) is a row, not a scenario**: shots in it have no `scenario` key (`--shoot-scenario -`);
  `CShotDocMode` captures and replays its arrangement and view; nothing is stored.
- **A scenario is never performed on top of itself** (`CShotContext::liveScenario()`): a shot of the
  live scenario photographs what is there; a build sets none and performs every scenario.
- **`CShotReplay::perform()` closes popups and modal dialogs only when it ran steps.** Without steps
  a window up is the writer's own; closing it anyway shut the About box F9 was pressed on.
- **F9 names a part the way the writer sees it** (`onScreenName()`): a dock's caption, a tab's text, a
  group box's title, a window's title, the class name only when nothing else names it. `qt_`-prefixed
  widgets are not offered, and the address is appended only where two entries would read the same -
  `chooseLivePart()` finds the part by `labels.indexOf()`, so two equal labels would pick the first.
- **Render before asking anything.** Focus cannot be restored across another window, and a project
  row's focus buttons fade in only with `State_HasFocus`, so `tag()` renders every candidate
  (`livePartsAt()`) at the key press, through `CShotWriter::renderAll()`: one `settleStable()` per window, and again only for a
  part whose map drew again since.
  `CShotRegionPicker` takes `Qt::NoFocus`.
- **F9 refuses a picture whose shot names another scenario** than the running one; its row starts
  that scenario.
- **A recording is stored only once it replays.** `record` snapshots the settings into
  `_cache/<page>-trial.ini` before the recorder runs, `name` parks the steps beside it in
  `<page>-trial.json`; the launcher starts a state composed `--from` that snapshot with
  `--doc-trial <name>`, which replays them, stores them with `CShotFiles::storeScenario()` and holds
  the scenario - or reports `trial-failed` with the first `shoot:` warning, stores nothing and the
  launcher returns to the base.
- **A scenario keeps the configuration it was recorded against**, so the base can move without moving
  a picture already taken. Save config therefore writes only `(base)`, through
  `CMainWindow::saveConfig()`, which is what `~CMainWindow` writes; a scenario is changed by recording
  it again. `compose --from <ini>` names the source file instead of resolving it from page and
  scenario.
- **Take again is headless**: the launcher runs `shots.py -o doc/images/_work replay --only <id>`,
  what a build does. a failure keeps the work picture.
- **A picture is written from what F9 rendered only for a live part.** An exposed dialog parented to
  the main window is in that render too, and its shot says `exposure`, which the build renders from.
- **Settings drift is what changed since the state was set up**, not a compare with the scenario's
  `.ini`: `snapshotSetup()` keeps the settings after `slotSetUp()`, after a trial and after base Save
  config. Compared as INI text - `QSettings` reads a one-entry `QStringList` back as a `QString` from
  another process' file and as `QStringList` from its own process' write (measured on
  `dem2/keysKnownDems`), so a `QVariant` compare reported drift on an untouched scenario.
- **`obey()` refuses launcher commands with a `status` while F9 or a region is in progress** (`busy`).

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

### `CDemVRT`/`IDem` rendering speed

- **Open:** batch `getElevationAt()`/`getSlopeAt()` point queries. Each is one small `RasterIO` call
  per point (e.g. per vertex of a track elevation profile) — a different path from `draw()`'s map
  rendering. Worth revisiting only if track-profile performance comes up.
- **Deliberately skipped:** caching `1/xscale`/`1/yscale` for `slopeOfWindowInterp()`. Modest win,
  and hoisting needs either a signature change touching its three callers or new cached members with
  a staleness trap — `xscale`/`yscale` are plain protected members assigned directly, with no setter
  to keep a reciprocal in sync.
