# QMapShack — Claude project notes

Qt/C++ desktop app for planning and analysing GPS tracks, routes and waypoints. Loads map tiles,
vector maps and DEM data; supports online and offline routing engines.

---

## Stack

- **Language:** C++20
- **GUI / framework:** Qt 6.8+
- **Key libs:** GDAL, PROJ 9.4+, Routino
- **Build:** CMake 3.20+, Ninja; build tree `build/`, binaries in `build/bin/`
- **Bundled 3rdparty:** Garmin FIT SDK
- **Minimum GDAL:** 3.10

---

## Source layout

```
src/
  qmapshack/        main app
    canvas/         map rendering
    gis/            tracks, routes, waypoints, DB, GPX, routing (gis/rte/router/)
    mouse/          mouse interaction; line editing in mouse/line/
    map/, dem/, poi/, grid/, plot/, realtime/, device/, tool/, helpers/, widgets/
  qmaptool/         map creation tool
  qmt_map2jnx/      utility
  qmt_rgb2pct/      utility
  qms_surfacedb/    surface database tool
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
- **Syntax-check changed files**: `.notes/syntax-check.sh <file.cpp>...` (each file's command from
  `ninja -t compdb` with `-fsyntax-only`; a header is checked through a `.cpp`). Catches shadowing and stale identifiers; not
  moc, AUTOUIC, linking, or anything inside `#if defined(Q_OS_WIN32)`.
- **Never commit or push** without being asked for that specific change.
- **No `Co-Authored-By` lines** in commit messages.
- **Working on a ticket adds its line to `changelog.txt`** as part of the change, under the top
  version block, sorted by ticket number: `[QMS-nnnn] <commit subject>`. Skip it only if the line
  is already there.
- For "what were we working on", read `git status && git diff` first — the live diff is ground
  truth, notes are not.
- Store project learnings in this file or the subsystem's own `CLAUDE.md` (see "Subsystem notes
  elsewhere"), not in the auto-memory system. Before adding, check it is not already said here or in
  `.notes/`. One or two lines of rule; no measurements, anecdotes or history unless the number is the
  rule. Git holds the history.
- Multi-session plans and specs go in the tracked `.notes/` directory, named `<topic>-plan.md`,
  with a pointer from `## Open work` here. Never invent another location.
- **This checkout (`~/projects/qmapshack`, remote `kiozen/qmapshack`) is where work happens.**
  `~/projects/qmapshack_master` tracks `Maproom/qmapshack` and is for reference only — never
  branch or edit there.

---

## Build system

Target-scoped CMake. Directory scope holds only platform and toolchain flags (PIC, MSVC, Apple
libc++, ASan).

- **Never touch the translation block's gating.** With `UPDATE_TRANSLATIONS` off no lupdate target
  exists, so no build rewrites the tracked `.ts` files; Qt's `update_translations` is no substitute.
  Keep the `/locale` resource in the `else` branch: nothing reads it, but it makes the `.qm` files
  inputs of the application target, which the packaging scripts copy from `<build>/src/<app>/`.
- **All ten `.qm` catalogs (base plus nine languages) are in the binary under `:/locale`**, but
  `prepareTranslators()` reads only the filesystem, so a build-tree run is English.
- **Standard dialog buttons are translated by the platform theme, not a `.qm`**, following
  `LANGUAGE`/`LC_MESSAGES`. `QT_QPA_PLATFORMTHEME=` (empty) still loads `kde` via
  `XDG_CURRENT_DESKTOP`; `generic` does not. Offscreen loads no theme and ignores the variable.
- **An unknown `QT_QPA_PLATFORMTHEME` still loads the desktop's theme**, so a documentation run
  pins the rest. Generic theme vs none (Qt 6.10.2, Fusion) differs only in
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
  MSVC, which the bundled FIT SDK needs, and `-march=native` with `BUILD_FOR_LOCAL_SYSTEM`.
- **Install paths** come from `GNUInstallDirs` on UNIX and stay relative to the application
  directory on Windows, where `CAppSetupWin` resolves `HELPPATH` against it. Six cache variables
  survive so a packager can override them. `HTML_INSTALL_DIR` is `share/doc/HTML`, not
  `CMAKE_INSTALL_DOCDIR` — changing it moves the installed help.
- **`CMAKE_RUNTIME_OUTPUT_DIRECTORY` is set, the four per-config variables deliberately not**: a
  multi-config generator needs them unset for its `$<CONFIG>` suffix (`msvc_64/CopyFilesGis.bat` reads
  `bin\Release\`); pinned, every configuration shares one `bin` and mixes `.pdb` and `.exe`.
- **The project enables C as well as C++**: PROJ's imported target can carry C compile features
  (Homebrew's `c_std_99`). `ConfigureChecks.cmake` still probes with the `_cxx` variants.
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

- **The application catalog is the gate** (`IAppSetup::prepareTranslators()`): without
  `<appPrefix><locale>.qm` Qt's own strings are English too. The desktop installs a Qt catalog before
  the app runs, so the untranslated branch installs `CSourceTextTranslator`, which answers every
  lookup with the source text and wins as the newest translator. One copy per app; `qmt_rgb2pct`
  keeps its own two-liner.
- **A desktop integration translates its own strings by `LANGUAGE`** while the platform plugin loads
  (KDE's button labels come from `kwidgetsaddons6_qt.qm`). So `IAppSetup::exportLocaleEnv()`, first
  in `main()`, puts `--locale` into `LANGUAGE`, parsing argv by hand.
- **`QTranslator::load()` succeeding is no existence test**: it strips suffixes until something loads
  (`qmapshack_pl` loads `qmapshack.qm`). Test `QFileInfo::exists()` on the exact name and pass the
  full `<prefix><locale>.qm`.

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
- **Log and error messages are for the reader, not the code.** Say what is wrong in the user's terms
  and hint how to fix it. No raw JSON, internal keys or class names as the message itself — that
  detail goes after it or behind `-d`. Report the first cause once, not every failure it triggers.

---

## Architecture: mouse/line editor

`src/qmapshack/mouse/line/`; the three editors are in `mouse/`.

```
IMouseEditLine          – owns the point list (SGisLine), undo/redo history,
│                          routing mode buttons, and the active ILineOp
├── CMouseEditTrk       – edits a track (CGisItemTrk)
├── CMouseEditRte       – edits a route (CGisItemRte)
└── CMouseEditArea      – edits an area overlay (CGisItemOvlArea)

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

- `mouseMove()` fires; `CLineOpAddPoint`/`CLineOpMovePoint` ignore it while `isRouting`.
- A right-click reaches `abortStep()` → `restoreFromHistory()`, which reallocates `points` and
  invalidates every saved index or pointer into it.
- A left-click can re-enter `leftClick()`.

**Any `ILineOp` that triggers routing must go through `runRoutingAndPin(coord)` and check its
return value.** It:

1. Pins `points[idxFocus].coord = coord` before routing.
2. Sets `isRouting = true` — subclasses test it in `leftClick()` to block re-entry.
3. Calls `slotTimeoutRouting()` → `finalizeOperation()` → `tryRouting()`.
4. Re-validates `idxFocus` against `points.size()` after the loop returns (an abort leaves both
   unknown).
5. Returns `false` if aborted — the caller must return immediately.

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

## Architecture: MTP devices

`CDeviceGarminMtp` works only through the abstract `IDeviceAccess`. The maintainer's rule is that
new device support goes into the OS-specific device watcher and an `IDeviceAccess` subclass only;
growing `IDeviceAccess` or touching `CDeviceGarminMtp`/`IDevice`/the workspace needs a real
blocker. `IDeviceAccess` is synchronous by design. `CDeviceAccessKMtp::waitForCopyOperation()`
keeps the GUI repainting through a transfer by spinning a nested `QEventLoop` inside the
synchronous call, so a backend with a worker thread gets a responsive GUI without an async API.
The project loader calls `listFilesOnStorage()` from a `CThread` and `readFileFromStorage()` on
the GUI thread (`BlockingQueuedConnection`). Each storage of a device is its own workspace device,
so a memory card shows next to the internal storage; a storage without `GarminDevice.xml` falls
back to the fixed paths in `CDeviceGarminMtp::setup()`.

**Nothing writes an item change back to a device.** So the workspace menus, on-map buttons and map
selection disable item changes on `isOnDevice()`, and a track converted from one goes to a workspace
project; only a whole project can be removed.

**Stopping an MTP load thread:** `IDevice::stopLoading()` cancels each load thread with a 100 ms
deadline (`CThread::cancel(receiver, deadline)`) and returns false while a call into the device is
on the GUI stack or a thread has not ended (`setProgress()` needs `mutexItems`); `removeDevice()`
and the Archive's collapse retry later. Code that enters a nested loop (KMtp transfer, LibMtp call,
dialog, `processEvents()`) and then calls into a device or its project holds an
`IDevice::CRunningCall` from before the loop. Only the destructors wait without a limit;
`CDeviceGarmin`/`CDeviceGarminArchive` still use the 5 s `cancel()`.

### macOS: `CDeviceAccessLibMtp`

libmtp is required on macOS. One worker thread per USB device (`Connection`) owns the libmtp
session and serves every storage's `CDeviceAccessLibMtp`; each call waits behind a nested loop.

- **Devices are found through libusb hotplug, never by `LIBMTP_Detect_Raw_Devices()`.** That scan
  opens every USB device missing from libmtp's device table and sends vendor requests to some of
  them. `CUsbWorker` registers a hotplug callback filtered on Garmin's vendor id (it also reports
  devices already connected), and `CDeviceAccessLibMtp::openDevice()` takes the flags from libmtp's
  table or probes that one device with `LIBMTP_Check_Specific_Device()`, on the worker thread. The
  GUI thread never calls libmtp. libusb's bus number and device address are what libmtp's raw device
  wants.
- **The watcher reconciles, it does not react.** Hotplug only edits `pluggedMtpDevices`;
  `slotUpdateMtp()` then removes what is open but no longer plugged and opens at most one plugged
  device per pass. Every open and removal runs local event loops that can re-enter it, so a nested
  call only sets `updateMtpAgain` and the outer call runs another pass; the timer is set once, at
  the end. A serial per arrival tells a device replugged at the same address from the earlier one.
- **A new device is opened 3 s after it appears.** A booting device can show up on USB briefly and
  leave again; the delay keeps such a device from being opened. Any failed open —
  not MTP yet, no storage yet, a failed listing, held by Garmin Express or OpenMTP — is retried
  2.5, 5, 10, 20 and 40 s later, then given up until the device is plugged in again. A device
  without `GarminDevice.xml` is dropped at once.
- **libmtp is not thread safe, so every libmtp call holds one process-wide mutex.** Consequence:
  destroying a `Connection` blocks the GUI until another device's running transfer ends.
- **A device must not be deleted while a call to it runs**, since the call waits in local event
  loops, its failure message boxes included. Each public call holds `CBusy` on its connection's
  counter, and the watcher postpones removing a device whose `isBusy()` is true.
- **A quit aborts every running transfer.** `aboutToQuit` comes before any event loop ends and sets
  the process-wide `quitting`; every connection's progress callback then aborts, and the object an
  aborted upload created is deleted. Process-wide because nested device calls wait on each other's
  libmtp lock, and the loader thread's loops do not end on a quit. `run()` posts the loop's quit
  before it releases the semaphore.
- **Transfers go straight through the caller's file descriptor**, as `CDeviceAccessKMtp` does. Nothing
  is read back.
- **An upload never deletes a file.** "Send to Devices" of a project the device already has goes
  through `IDevice::updateProject()`, which removes the device's copy first, so a name still taken
  belongs to another project: `simplifiedName()` drops every character outside `[A-Za-z0-9_]`, and
  "Tour 1" and "Tour-1" both become `Tour1.gpx`. The upload refuses it.
- **Object ids come from remembered folder listings.** A lookup walks the path down from the Garmin
  folder, taking ancestors from memory. Listing a folder, sending and removing fetch the folder
  concerned; reading reuses it. A lookup that fails
  forgets every listing and is walked once more from the device, and nothing below `retryLookup()`
  retries; a failed read forgets them too. Forgetting must reach the ancestors: a folder made anew
  keeps its stale id in its parent's listing.
  `LIBMTP_Get_Files_And_Folders()` returns no files for an empty folder and for a failure alike,
  so `entries()` tells them apart by the error stack.
- `LIBMTP_Open_Raw_Device_Uncached()` already calls `LIBMTP_Get_Storage()` and fails only on -1;
  1 means storage ids without properties and is a working device.
- Homebrew Qt ships D-Bus, so a macOS build also compiles the Linux D-Bus device sources.

---

## Architecture: routers

`IRouter` is the abstract base:

- **`CRouterRoutino`** — offline routing from Routino databases.
- **`CRouterBRouter`** — BRouter, either a local process (`CRouterBRouterLocal`) or the online HTTP
  API.

`CRouterSetup::self()` is a singleton owning the active router and exposing `calcRoute()`. It emits
`sigHasFastRouting(bool)` when the router capability changes; `IMouseEditLine` listens and
enables/disables the auto-routing button.

Routino and local BRouter support fast (on-the-fly) routing; online BRouter needs the whole route at
once via `calcRoute(const IGisItem::key_t&)`.

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

## Garmin label cache — `IGarminStrTbl`

`get()` is a non-virtual wrapper over a bounded LRU `QCache` (key `(type << 32) | offset`); a miss
calls the pure-virtual `decode()` of `CGarminStrTbl6/8/Utf8`. Keep the split so the cache lives in one
place. Only the draw thread touches it, so no lock.

---

## Mapsforge POI loading — `CPoiFilePOI`

A square counts as loaded once it has an entry in `loadedPoisByArea`, so read it only through `isLoaded()`/`poisInSquare()`:
`operator[]` inserts. Only a failed `prepare()` (lock, missing table) leaves squares unloaded, retried on the next draw; it stops
that draw's loading. A failed `exec()`/`next()` (corruption) keeps what was read: retrying costs every draw and cannot help.
A following `exec()` replaces `prepare()`'s error by "Parameter count mismatch", so check `prepare()` on its own.

---

## Surface databases — `CSurfaceDb`

Opened as `immutable=1` through a URI, so SQLite takes no lock: a built file never changes (the builder renames a finished
`.part` into place). The URI is percent-encoded and has no authority, which SQLite rejects.

---

## Help browser — `CHelpBrowser`

A `QWebEngineView` on its own off-the-record profile, whose `qthelp` handler serves `QHelpEngine::fileData()`.
`registerUrlScheme()` must run in both `main()`s before the `QApplication`. The destructor deletes the page
first: a profile deleted before its page is a Qt error. Theme colours are an injected `<style>`, rewritten on a palette change, never a reload.

---

## GIS item keys

`key.item` is the database identity (`items.keyqms`, unique): an import links an item whose key
exists instead of storing it again. It is the MD5 of the item's data, a geocache's id for a
waypoint with geocache data and no key yet. A clone takes a random key (`genCloneKey()`), since
every clone of one item has the same data.

---

## Architecture: tree item delegates

- `gis/CWksItemDelegate.{h,cpp}` — workspace tree
- `gis/CDBItemDelegate.{h,cpp}` — database tree
- `map/CMapItemDelegate.{h,cpp}` — map-item tree

Every `getRectangles*()` carves its row with `CRowBuilder` (`helpers/CRowBuilder.h`); padding is
`kCellPad`/`kInnerGap` there. `CWksItemDelegate` and `CDBItemDelegate` take
name-height buttons (`takeButton(fmName.height())`) with `fullStatusSlice()`, so the status line runs
under them; `CMapItemDelegate` takes full-height ones (`takeRight(row.height())`) with `statusSlice()`.

`CMapItemDelegate`'s `struct animations_t;` forward declaration is required: the type is defined
after `getAnimations()`.

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

`--color-scheme light|dark` is a normal option of both apps (`CUiTheme::pinColorSchemeOption()` from `main()`, after
`initLogHandler()` so a wrong value is logged; passed on to QMapTool). It sets the scheme hint and installs Fusion's palette only
where the palette does not follow; a doc run always installs Fusion's, so output cannot depend on the desktop.
Before the first window; not a scope, not undoable. The palette is what
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

Following a live scheme switch, `CQmsStyle` cues, canvas and printing, the surfaces that stay light on
purpose and how to test a switch: `src/common/theme/CLAUDE.md`.

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

`cmake/IconHygiene.cmake` hashes each working-tree `.svg` against `src/icons/svg.sha256`, so
`.gitattributes` pins them `eol=lf`; one CR makes every icon look edited. A blob committed with CRs
stays modified forever (`eol` normalises only on the way in); only a commit of LF blobs fixes it, and
if it blocks a fast-forward, `git reset --hard <remote>/<branch>`. `*.bat`/`*.cmd` (`eol=crlf`) have
the reverse trap: to rebase past one, put `<path> -text` in `.git/info/attributes`, rebase, remove it,
then `rm <path> && git checkout -- <path>`. `git ls-files --eol '*.bat'` lists offenders (`i/crlf`).

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

The `comment` column keeps its PNG paths: it serves full-text search, and its tooltip strips
`<img>`.

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
  presentation attribute; without it those icons render black.
- Qt ignores `markerUnits="strokeWidth"`, so `svghygiene` bakes markers into geometry. Set
  `stroke:none` wherever `stroke-width` is 0 **first**, or the shape becomes a filled block.
- `QIcon(":/x.svg")` needs both `imageformats/qsvg` and `iconengines/qsvgicon` deployed.
- Qt 6.10.0 breaks `currentColor` (QTBUG-141102); use 6.10.1 or newer.

Dark `ink` is `#9999ff`: legible on `paper` `#353535` at 4.5:1 and distinct from `lead` `#e0e0e0`
and `mark` `#66aaff`, which icons paint beside it. A "more vibrant blue" means a more saturated
navy, not azure.

### Waypoints are data, not chrome

`src/icons/waypoints/` follows the GPX `<sym>` vocabulary and its look is frozen, so it **stays PNG
on the canvas**; user icons are PNG/BMP (`CWptIconManager`) anyway. No dark theming: they sit on map
tiles.

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

`.qms`/DB store the symbol name and the colour; icons are re-derived on load, and `items.icon` is a
cache. Only the waypoint symbol is a frozen raster: the track/area PNGs are silhouette masks and can
become SVG rendered at size.

---

## Tile cache

`CDiskCache::cleanupRemovedMaps()` (from `CMapDraw::loadMapList()`) **deletes the cache directory of
every map the configuration does not know**, and `defaultCachePath()` ignores `--config`. A scripted
run must call `CMapDraw::setCacheRoot()` and `CGisListWks::setDatabasePath()` before `CMainWindow`,
or it prunes the user's tiles and empties their workspace.

`setCacheRoot()` pins the root: `Canvas/cachePath` in the configuration no longer applies. A doc run
takes `CShotFiles::tileCache()`: `doc/shots/_cache`, or `_cache/pages/<page>` when the page's fixture
has maps of its own, since each run deletes the caches of maps it does not load. A doc run without
`--config` is refused (`CShotEntry::prepare()`): it would write the user's settings.

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
smallest the main window can be. Each of these reads a *preferred* width as a *required* one:

- **A `QLabel` without `wordWrap` reports its whole unwrapped string as a minimum.** With `wordWrap`
  it reports its widest word. Give any label holding a sentence `wordWrap`, especially one whose
  text is set at runtime or translated.
- **`QSplitter` sizes a child through `qSmartMinSize()`**, which takes `qMax(sizeHint,
  minimumSizeHint)` unless the child's size policy carries a `ShrinkFlag`. `MinimumExpanding` and
  `Minimum` have none, so the child's preferred width becomes a hard floor. Use `Expanding` or
  `Preferred` for a splitter child that may shrink.
- **A `QTreeWidget` row is as high as the item's size hint, which does not follow the width.** A
  wrapped label in an item widget is clipped as the tree narrows; recompute the item size hints from
  `heightForWidth()` on resize, as `CDetailsTrk::updateFilterRowHeights()` does.
- **A combo box beside a field frame adds both widths**; stack it above instead.

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

`GDALAutoCreateWarpedVRT` warps onto an axis-aligned box, so a source not aligned with the target
SRS has uncovered corners:

- **Palette/gray (`CMapVRT`):** a source nodata value becomes src and dst nodata, and the
  constructor zeroes its alpha in the colortable. Without source nodata there is no transparency.
- **RGB(A) without alpha (`CMapVRT`):** a synthetic destination alpha band (`nDstAlphaBand`, as
  `gdalwarp -dstalpha`); `rasterBandCount` is re-read afterwards. Without it the corners are black.
- **`CDemVRT`:** not handled; uncovered samples read back zero-initialised, not `NOFLOAT`. **Open.**

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

## Subsystem notes elsewhere

Loaded when files there are touched; read them before working on the subsystem from outside.

- `src/qmapshack/shoot/CLAUDE.md` — documentation mode (`-DQMS_DOC_MODE=ON`, `doc/tools/shots.py`).
- `src/qmapshack/helpers/CLAUDE.md` — the overview-advisory system (VRT overviews, Fix, Combine).
- `src/common/theme/CLAUDE.md` — colour scheme details.

---

## Open work

Analysed designs live in `.notes/`, each wanting its own branch. De-freeze one by pointing at the
file.

- `QMS-1135-overview-restore-plan.md` — transactional rollback when a *Fix overviews* job is
  cancelled.
- `waypoint-icon-resolution-plan.md` — 32 → 96 px waypoint icons, gated on storing `icon_t::focus`
  relative.
- `QMS-1217-review-plan.md` — what is left on `QMS-1217`: live checks, open points, what was left
  deliberately. The design is `QMS-1217-documentation-images.md`.

### CMake — remaining

- Retire the platform blocks: the hardcoded `C:\...` cache defaults and the macOS
  `QT_DEV_PATH`/`ROUTINO_DEV_PATH`/… `FATAL_ERROR` gauntlet can move into `CMakePresets.json`, and
  `MacOSX/build-QMS.sh` and the msvc batch files can call `--preset`. Needs someone who can test
  both release paths.
- Shared `qms_common` library: eleven files under `src/common/` are compiled once per app. Blocker
  is `src/common/help/CHelp.cpp`'s `"helpers/CSettings.h"`, which resolves per-app; the two copies
  are byte-identical, so moving that header to `src/common/helpers/` unblocks it.
- Confirm on their own platforms that these removals were inert: macOS `LINK_FLAGS`, the
  `-framework` entries in `CMAKE_C_FLAGS` and the three framework include dirs; Windows
  qmt_map2jnx's `Win32/` include dir. `msvc_64/cmake/{FindGDAL,FindPROJ,FindJPEG}.cmake` can go if
  the gisinternals GDAL ships `GDALConfig.cmake`.

### `CDemVRT`/`IDem` rendering speed

- **Open:** batch `getElevationAt()`/`getSlopeAt()` point queries. Each is one small `RasterIO` call
  per point (e.g. per vertex of a track elevation profile) — a different path from `draw()`'s map
  rendering. Worth revisiting only if track-profile performance comes up.
- **Deliberately skipped:** caching `1/xscale`/`1/yscale` for `slopeOfWindowInterp()`;
  `xscale`/`yscale` have no setter to keep a reciprocal in sync.