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
- **Run it before reporting it.** The build is the user's, but verifying is not: run
  `build/bin/qmapshack` headlessly through `doc/tools/shots.py` (on this checkout: `chapter`,
  `build`) with `-o <scratch>` so nothing in the tree is touched, read the images it produced, and
  check the JSON and INI files a feature wrote. A claim that something works names the command that
  showed it. If nothing was run, say so and name the check that would settle it.
- **Syntax-check what you changed before saying it is done.** `.notes/syntax-check.sh <file.cpp>...`
  runs `c++ -fsyntax-only` with the flags the build uses: no object file, no linking, nothing
  written, and it catches the shadowing and stale-identifier mistakes that otherwise reach the user
  as a failed build. Not a substitute for the build — moc, AUTOUIC and linking are not covered. It
  reads the Qt prefix out of `build/CMakeCache.txt`, because checking against whatever Qt the system
  ships reports everything newer than that as missing — `QStyleHints::setColorScheme` and
  `QPalette::Accent` both came back as non-existent while it pointed at the distribution's Qt 6.4.
  It also fails when a Win32 SDK header precedes `<windows.h>`: no `#if defined(Q_OS_WIN32)` block
  is compiled here, so that break reaches the user's build unchecked.
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
- **The `.qm` catalogs are unreachable in a build tree.** `prepareTranslator()` resolves a filesystem
  path only (`<appdir>/../share/qmapshack/translations` on Linux), which exists after `install`, so an
  uninstalled run is always English. All nine catalogs *are* in the binary under `:/locale` — verify
  with `strings -a -e l build/bin/qmapshack | grep qmapshack_de.qm`. A `:/locale` fallback in
  `prepareTranslator()` is what would make an uninstalled run translatable.
- **A dialog's standard buttons come from the platform theme, not from a `.qm`.** Measured on
  X11/KDE with Qt 6.10.2: with `KDEPlasmaPlatformTheme6` loaded they read
  `&OK | &Schließen | &Abbrechen` whatever `--locale` says and whichever `qtbase_*.qm` is installed
  - `QPlatformTheme::standardButtonText()` answers out of the desktop's own translations, and it
  adds the mnemonics the generic theme leaves off. It follows `LANGUAGE`/`LC_MESSAGES`, and
  `QT_QPA_PLATFORMTHEME=` empty does not switch it off: Qt then detects `kde` from
  `XDG_CURRENT_DESKTOP`. `QT_QPA_PLATFORMTHEME=generic` does. **The offscreen platform loads no
  theme at all and ignores that variable**, so a headless run never has a theme and an on-screen one
  always does - which is what made a writer's session and the build it feeds disagree. `shots.py`
  pins `QT_QPA_PLATFORMTHEME=generic` and `LANGUAGE` for both, and the seven `test` images are
  byte-identical with and without that pin.
- **What is left of the theme is pinned in the application, because the environment cannot do it.**
  An unknown `QT_QPA_PLATFORMTHEME` does not suppress the desktop's own theme - Qt falls through to
  the integration's `themeNames()` and loads `kde` anyway - so a session cannot be told to have no
  theme. Measured instead (Qt 6.10.2, Fusion, generic theme against no theme): of 130 style hints,
  every class font, the palette and five standard icons, **exactly one differs**,
  `SH_DialogButtonBox_ButtonsHaveIcons`, plus `QIcon::themeName()` (`hicolor` against empty).
  `CQmsStyle::pinThemeIndependentHints()` answers that hint 0 and `CShotEntry::prepare()` clears the
  icon theme, for `--shoot` and `--doc` alike. Verified by rendering the same dialog into a QImage
  under both platforms: identical MD5 with the pin, different without it. Anything else that turns
  out to differ belongs in that one hint override, never in a per-symptom patch.
- **Qt's catalog must follow the application's, and `QTranslator::load()` cannot say whether it
  does** (QMS-#1242). Qt's catalogs live in `QLibraryInfo::TranslationsPath` and are always
  installed, the application's only after `install` and only for nine languages, so an ungated run
  mixes Qt's translated strings into an English window. `QMS-1217_demo` gates on
  `prepareTranslator()`'s return value, which is not a test: measured with the installed 1.21.1,
  `load("qmapshack_pl", dir)` drops the `_pl` and succeeds with `qmapshack.qm` — 33 bytes, the
  untranslated source catalog, installed whatever the language — so `qtbase_pl.qm` is loaded anyway.
  Only a build tree, where the directory does not exist, comes out English. The gate has to test
  that a file for the resolved language exists. Standard buttons are not what this is about - see
  above.
- **`QLocale::system()` follows `LANGUAGE`, not only `LANG`.** Measured: `LANGUAGE=de` with
  `LC_ALL=LANG=en_US.UTF-8` is `de_DE`. A run that must not depend on the desktop passes `--locale`;
  `shots.py` does.
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
- **`CMAKE_RUNTIME_OUTPUT_DIRECTORY` is set, the four per-config variables are deliberately not.**
  They are the only thing a multi-config generator consults for its `$<CONFIG>` suffix, so pinning
  them collapses every configuration into one `bin`, which leaves a `.pdb` from the previous
  configuration beside a mismatched `.exe` and makes the build report the other config's binary as
  up to date. `msvc_64/CopyFilesGis.bat` reads `bin\Release\` and wants the suffix. Single-config
  generators resolve the per-config variable to the same path as the plain one, so Linux and the
  macOS release path (`build-QMS.sh` uses `Unix Makefiles`; its Xcode branch is marked BROKEN and
  exits before bundling) are flat whatever the build type.
- **The project enables C as well as C++** (`LANGUAGES CXX C`). PROJ is a C library and its imported
  target can carry C compile features (Homebrew's exports `c_std_99`), which a CXX-only project fails
  to generate. `ConfigureChecks.cmake` still probes through the C++ compiler with the `_cxx` variants,
  as the code it guards is C++.
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

**Open, all platforms:** deleting a `CDeviceGarminMtp` while its load thread runs (an unplug during
the initial load) blocks the GUI for `CThread::cancel()`'s 5 s, because the thread waits on a
`BlockingQueuedConnection` into the GUI, and afterwards the thread uses the deleted device.

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
- **A new device is opened 3 s after it appears.** A booting Edge 1040 shows up on USB for about a
  millisecond and leaves again; the delay keeps such a device from being opened. Any failed open —
  not MTP yet, no storage yet, a failed listing, held by Garmin Express or OpenMTP — is retried
  2.5, 5, 10, 20 and 40 s later, then given up until the device is plugged in again. A device
  without `GarminDevice.xml` is dropped at once.
- **libmtp is not thread safe, so every libmtp call holds one process-wide mutex.** Consequence:
  destroying a `Connection` blocks the GUI until another device's running transfer ends.
- **A device must not be deleted while a call to it runs**, since the call waits in local event
  loops, its failure message boxes included. Each public call holds `CBusy` on its connection's
  counter, and the watcher postpones removing a device whose `isBusy()` is true.
- **A quit aborts every running transfer.** `QCoreApplication::exit()` emits `aboutToQuit` before it
  ends any event loop (Qt 6.8.0 and 6.11.2), and a connection made on the first `openDevice()` sets the
  process-wide `quitting` from it; libmtp's progress callback of every connection answers by aborting,
  and the object an aborted upload created is deleted. It must be process-wide: a device opened from
  inside another device's waiting call waits for the libmtp lock that call's worker holds, and the
  outer call cannot resume until the inner one returns. It also reaches calls waiting on the loader
  thread, whose loops a quit does not end. In `run()`, the worker posts the loop's quit before it
  releases the semaphore, so the loop and the captured locals outlive the worker's last use.
  Measured on the Edge with a 75 MB upload and a quit Apple event: the send stopped within 6 ms of
  `aboutToQuit`, and the app exited 2.4 s after the quit. The Edge 1040 drops an aborted object itself,
  so the cleanup delete fails there and nothing is left behind; that delete once took the device 4 s.
- **Transfers go straight through the caller's file descriptor**, as `CDeviceAccessKMtp` does. Nothing
  is read back.
- **An upload never deletes a file.** "Send to Devices" of a project the device already has goes
  through `IDevice::updateProject()`, which removes the device's copy first, so a name still taken
  belongs to another project: `simplifiedName()` drops every character outside `[A-Za-z0-9_]`, and
  "Tour 1" and "Tour-1" both become `Tour1.gpx`. The upload refuses it.
- **Object ids come from remembered folder listings.** A lookup walks the path down from the Garmin
  folder, taking ancestors from memory. Listing a folder, sending and removing fetch the folder
  concerned; reading reuses it, so the loader's 118 reads need no listing each. A lookup that fails
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

`CUiTheme::pinColorScheme(bool)` is the other way in: it replaces the application palette outright
so a run's output cannot depend on the desktop's scheme. Not a scope like `CForceLight`, not
undoable, and once only, before the first window — see the documentation-run notes under *Open
work*.

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

`CDiskCache::cleanupRemovedMaps()`, reached from `CMapDraw::loadMapList()`, **deletes the cache
directory of every map the current configuration does not know about**. `--config <fresh ini>` does
not isolate it — `defaultCachePath()` is `~/.QMapShack` regardless — so any headless or scripted run
must call `CMapDraw::setCacheRoot()` before the first map list load or it destroys the user's tile
cache.

`Canvas/cachePath` in the configuration wins over that setter, and always: `saveMapPath()` stores the
key on **every** exit, so only a configuration that has never been written falls back to the pinned
value. `setCacheRoot()` therefore protects startup alone — the stretch before `CMainWindow` reads the
configuration — and what the run then uses is whatever `--config` carries. A documentation run without
`--config` is refused for that reason (`CShotEntry::prepare()`), because it would also write the
user's settings from `~CMainWindow`, beside an open QMapShack that no longer stops a second
instance.

### The map list outlives its `CMapDraw`

`CMapDraw`'s constructor parents `mapList` to the canvas but disposes of it with
`connect(canvas, &CCanvas::destroyed, mapList, &CMapList::deleteLater)`, while `CMapDraw` itself is
a child of the canvas and dies with it. So between the canvas going away and the next event loop
turn, every `CMapItem` is alive with a dangling `CMapDraw* map`. Anything a `CMapItem` queues must
therefore watch its owner, not only itself — `loadConfig()`'s 100 ms deferred activation carries a
`QPointer<CMapDraw>` for exactly this. A canvas that lives less than that delay, such as the one
`CPrintDialog` builds, hits it.

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
- `doc-image-publish-plan.md` — a `shots.py publish` that commits only the pictures a recipe change
  actually moved, so a writer's run stops rewriting every PNG with their machine's rendering.

**The documentation subsystem is developer-only**, behind `-DQMS_DOC_MODE=ON`. A doc run needs the
source tree and a build tree configured with it.

**No source tests `QMS_DOC_MODE`** — `grep -rn QMS_DOC_MODE src/` finds `src/qmapshack/CMakeLists.txt`
and nothing else. The option picks which
implementation of a header is compiled: `shoot/CShotEntry.cpp` and `shoot/CShotOptions.cpp`, or
`shoot/CShotEntryStub.cpp` and `shoot/CShotOptionsStub.cpp`, which answer "no documentation run" and
define no switch. So a user's binary rejects `--shoot`, `--doc` and `--color-scheme` through
`QCommandLineParser`'s own error instead of accepting a switch that does nothing, and `main.cpp`,
`CCommandProcessor` and `CAppOpts` carry no preprocessor branch.

`CShotEntry` is all `main.cpp` knows: `isDocRun()` and `prepare()`. `prepare()` runs before
`CMainWindow` and is what makes a run harmless and machine-independent — the tile cache root, the
workspace database, `CUiTheme::pinColorScheme()`, `CQmsStyle::pinThemeIndependentHints()`, the icon
theme cleared, and the bundled `src/fonts/` families registered, which the offscreen platform on
Windows has none of. `CShotOptions` owns the eleven switches, defined and read in one place, and reaches everything as the
single `CAppOpts::doc` member. `--config` is mandatory for such a run; `prepare()` returns false
without it and `main()` exits.

The bundled fonts live in `shoot/fonts.qrc`, compiled only with the subsystem, so a user's binary
carries none of them.

A doc run also skips the splash screen and `CSingleInstanceProxy`, which would hand the arguments to
an already running QMapShack and exit.

`QMS-1217-documentation-images.md` is the one plan. §1-§8 are the design, §9 the evidence - measured
on this checkout, do not re-derive it and do not doubt it without a new measurement - §10 what the
demo does not do, §11 the sub-tickets the feature branch is built from. It replaces the three
layered plans that came before it.

**One branch, one commit per sub-ticket.** Everything from #1245 to #1257 is developed on `QMS-1217`,
which is based on `dev`; a sub-ticket gets a commit there, never a branch of its own. "Reviewable and
mergeable on its own" in §11 means the commit, not a branch — and a branch per sub-ticket does not
work anyway: its `CLAUDE.md` and `.notes` edits belong to text only the feature branch has.

**The subsystem is being built ticket by ticket**, #1245 to #1257 (#1247 is what creates the test
page). Landed: #1245, the hermetic run — the option, the switches, the two entry points, the
redirections and the pinning; nothing renders yet. Everything else described below is still the demo
on `QMS-1217_demo` and the facts it established, and a statement about it is a requirement, not a
description of code you will find.

What is on `QMS-1217_demo` is a **throwaway demo** of that design. The port names the writer's
session `shots.py take <page>` and the headless one `shots.py replay`; the demo calls them `doc` and
`chapter|build`. Everything but the exposure catalog is data:

```
doc/pages/<page>.md            what says a picture exists - the only source of shot names
doc/shots/<page>.json          the shots and the recorded scenarios
doc/shots/<page>/<name>.ini    one scenario's whole configuration
doc/shots/fixture/shots.ini       the base a page opens on
```

- **A scenario is recorded, not registered.** `CShotRecorder` diffs the application on each mouse
  release and stores meaning - a name path, a geographic point, a driven value - plus `layout` and
  `view` (the centre and the zoom level, never a rectangle: `zoomTo()` snaps and everything on the
  map moves) taken whole at Stop. `IShotRecipe` and `RecipesChapter.cpp` are gone; `SHOT_EXPOSE` stays.

### What the recorder costs the application - counted 2026-09-08

The question this answers is whether the framework's knowledge of particular widgets is finite.
Half of it is; the other half is not, and the unbounded half is the cheap-looking one.

**Finite: the click vocabulary.** A position means nothing in a widget that paints its own content, so four of them
are addressed in their own terms - `CCanvas` (a geographic point), `IPlot` (`xValueAt()` /
`pointOfXValue()`), `CIconGrid` (`iconAt()` / `rectOfIcon()`), `CWksItemDelegate` (`buttonAt()` /
`pressButton()`). The whole population is nine and every member can be named: six widgets override
both `paintEvent` and a mouse handler - the three above plus `CDateTimeEditor`, `CPhotoViewer` and
`CRouterBRouterTilesSelectArea` - and three delegates paint clickable areas, `CWksItemDelegate` plus
`CDBItemDelegate` and `CMapItemDelegate`, whose overview badge is clicked through `editorEvent()`.
Two of the five were covered on 2026-09-08 - `CMapItemDelegate` (`activate`, `overview`) and
`CDBItemDelegate` (`checkState`), each with the same `button_e` / `buttonName()` / `buttonAt()` /
`pressButton()` / `sigButtonPressed` shape the workspace delegate has. Their rows carry no type tag,
so a name path is the whole address and the step names its tree (`"tree": "maps"` / `"database"`;
absent is the workspace). Three are left - `CDateTimeEditor`, `CPhotoViewer`,
`CRouterBRouterTilesSelectArea` - and the list grows only when somebody writes a new
custom-painted widget.

**The fixture has no database**, so nothing in a documentation run has a `CDBItemDelegate` row: that
vocabulary cannot be exercised until `CShotFixture` builds one.

The hooks are small except one: `xValueAt` 6 lines, `pointOfXValue` 7, `iconAt` 4, `rectOfIcon` 11,
and `CWksItemDelegate::buttonAt` 50 + `pressButton` 73, which have no caller outside `shoot/`.

**Not finite: `objectName` on actions.** A menu entry is addressed by its action's `objectName`, so
the documentation work added 112 of those - 56 directly, 52 through `CGisListWks`' own
`addAction()`/`addSortAction()` and 4 through `CGeoSearch::addService()`, against 90 in the whole
application - and every menu entry written from now on needs one. Nothing fails when it is
forgotten: the recorder warns and drops the step, and the picture comes out of a state nobody asked
for. This is the tax that does not stop, and it is not bounded by the number of such widgets.

**For scale:** the documentation branch's app-side footprint, `shoot/` and the translations
excluded, is 55 files, +1306/-307 - and most of it is not vocabulary at all, but the bundled fonts a
headless run needs, the colour-scheme pinning, the command line options, and the cache and database
paths a run must set before it prunes the user's own.

`shoot/` itself knows eleven application classes, not four: the four above, `CMouseNormal` and
`CGisItemTrk` in `clear()`, `IWksItem` and `IGisProject` for tree paths, `CCanvas` again in
`CShotWriter` for the draw threads, and `CMapList` / `CMapDraw` / `CGrid` in the fixture and the
exposures.
- **The window's size has exactly one record: the shot's `size`.** `layout` carries `saveState()`
  and the tab, never `saveGeometry()`. `shootOne()` resizes the window before the scenario runs and
  `restoreState()` distributes the dock extents into it - that order is required, because those
  extents are pixels. Everything the scenario produces is measured against that size: where the map
  is centred, where an item's options are anchored, and what a stored rectangle frames. A shot the
  main window sizes - the window itself or anything inside it - must say how big it was, or it is a
  counted failure; an exposure is exempt. A `layout` that still carries a `geometry` warns and is
  ignored.
- **Documentation mode is two processes.** `qmapshack --doc <repo> --doc-page <ch>` is the
  launcher: it owns the panel, reads the shot file, and does every operation that is a file
  operation. It starts a second process with `--doc-scenario <name|->` for the state the writer
  works in, and throws it away when they pick another. Nothing is ever taken back down, which is
  what `reset()` could not do: the application's state is not enumerable, so a list of things to put
  back is never complete.
- **The launcher's main window is constructed and never shown.** `CMainWindow::self()` initialises
  `IUnit`, `CWptIconManager`, `CGisWorkspace` and eight more singletons the panel's data goes
  through. `CShotEntry::showsMainWindow()` is what main.cpp asks.
- **The supervisor owns the session, the state process owns nothing.** Which page, which
  scenario, which picture, what a retake changed - all of it lives in the process that is never
  photographed and never dies. Every dialog a panel button leads to opens there too: the base
  question, the recording's name, delete, rebind, and removing what no page references. The state process reports and obeys, and its
  only dialogs are the ones F9 opens about the widget the writer is pointing at.
- **Closing either window ends the session.** The state process quits when its application window
  closes; the supervisor quits when the state ends without it having asked for that (`killing`), and
  when its panel is closed. A state process whose channel drops quits too, so a killed supervisor
  leaves no orphan holding a window.
- **The state process re-applies `MainWindow/geometry` after the fixture is up.** What `CMainWindow`
  restores in its constructor is a size the docks then grow past, so a base stored at 1200x876 came
  back 120 pixels taller and a writer could not make a size stick. Applied again once the layout is
  populated, base -> scenario -> base returns the same window to the pixel.
- **What `saveGeometry()` records is machine-specific, so `portableGeometry()` takes it out.** The
  record carries the screen the window was on and that screen's width, and `restoreGeometry()`
  drops the whole record - size included - when the width it is replayed against differs by more
  than a quarter (Qt 6.10.2, measured: `factor < 0.8 || factor > 1.25` returns false and applies
  nothing). A width of 0 is what Qt 5.3 and earlier wrote and takes the branch that only rejects a
  window wider than one and a half screens, so a base stored on one machine still sizes the window
  on the next. The position stays: `QWidgetPrivate::checkRestoredGeometry()` moves a window that
  would land off screen back onto it, and shrinks one that does not fit. Both `storeBaseConfig()`
  and `storeScenarioConfig()` go through it; the three tracked `.ini` files were rewritten once.
- **Where the window sits is the writer's screen, not a stored position.** `saveGeometry()` records a
  position on the whole desktop, so on a multi-screen desk a state process landed wherever that
  pointed - away from the panel, and again on every scenario change. The launcher reads
  `panel->screen()->name()` at each start and hands it over as `--doc-screen`;
  `CShotDocMode::moveToWritersScreen()` centres the window there once the fixture is up, keeping its
  size. Measured on three screens: asked for `DVI-I-1-1` / `DVI-I-2-2` / `HDMI-1`, the frame came out
  at x=2320 / 400 / 4240, always 1119x769.
- **Starting a state takes about seven seconds**, because it is a whole application. The panel says
  so and refuses input while it happens (`setBusy`); without that the writer clicks again and the
  clicks queue up behind a process that is still coming up.
- **The panel's geometry is applied one event loop after it is shown.** A decorated window belongs
  to the window manager until it is mapped, and this desktop answered the constructor's 460x760 with
  1200x996. Once only, so a writer's own resizing survives.
- **The two talk over a `QLocalSocket`**, named `--doc-channel`. Launcher to state: `region`,
  `update`, `retake`, `record`, `stop`, `sync`. State to launcher: `status`, `ready`, `tagged`,
  `changed`, `recording`, `recorded`. `Ctrl+Shift+F9` stays in the state process - it owns the
  window the writer is pointing at.
- **`shots.py compose <page> [--scenario <name>] --out <ini>` is the one composer.** The launcher
  runs it before starting a state process, so the writer's session and the build cannot drift apart.
  A scenario's `.ini` is a whole configuration, never a patch on the base.
- **Never make the panel refuse to close.** It did, to keep the writer from losing it, and that
  turned `qApp->quit()` - which `QGuiApplication` answers with `closeAllWindows()` - into a quit that
  never happened. The panel is the launcher's only window; closing it ends the session.
- **A recording always starts from the base**, so a scenario is a whole state and never a difference
  from another scenario that is not stored with it.
- **`(base)` is a row, not a scenario.** A shot taken in it has no `scenario` key, and
  `--shoot-scenario -` is how a build asks for that group. Nothing stores it, so it cannot go stale.
- **A scenario's `.ini` is a whole configuration, never a patch**, and the base is a starting point,
  not a layer under it - otherwise restoring a base would move pictures already taken. Settings are
  read in constructors, so `shots.py` runs **one process per scenario**.
- **What the tool owns is injected per run, never stored**: `Canvas/{mapPath,demPaths,poiPaths}` from
  `doc/shots/fixture/`, and `Database/saveOnExit=false` - without it a run saves its workspace and
  the next one loads the demo project again.
- **Losing or changing a shot's scenario reduces the entry to its bare `id` and deletes the image**:
  a widget address and a rectangle frame something else in another state.
- **There is no `details` step**; `db342e266` dropped it together with the `CDetails*` exposures
  when input recording arrived. A track's and a project's `edit()` adds a page to the central tab
  widget and leaves it there, so the recorded double click or menu entry that opens it is the whole
  record. Every other `edit()` runs a modal `exec()`, and those belong to the exposure catalog.
- **The arrangement's `tab` index is applied after every other step**, because a page a step opens
  does not exist while `restoreState()` runs - and `QTabWidget` drops an index past its last page
  without a word.
- **`CShotPage::driveProperty()` is how a `set` reaches a widget.** `setProperty()` answers
  whether the property exists, never whether the value took, so the value is read back.
- **A menu entry is addressed by its action's `objectName`, never its text**, which is translated.
  Every menu owner names its actions after the member they are assigned to; a menu built from data
  takes a stable prefix plus an untranslated key (`actionActivity_<act20_e>`,
  `actionColor_<GPX colour name>`, `actionWptIcon_<sym>`). A new `addAction` needs the same.
- **A press is a step only when what it landed on acts on a click** (`pressIsStep()`): a button, a
  tab bar, a combo box, a header, or an item view row that exists. A splitter handle, a dock title,
  a scroll bar, a menu bar and the empty space under the last row are not. It is a whitelist, so a
  widget nobody has taught the recorder about records nothing - recoverable, where a wrong click is
  a picture of the wrong state.
- **A scenario is never performed on top of itself** (`CShotContext::liveScenario()`). Documentation
  mode holds one state up for the writer, and both shot paths - `CShotPage::shootOne()` and
  `takeRegion()` - used to perform it again to be sure the build starts where the writer does. A
  step is not idempotent, so the second pass ran the plot's click cycle from `eMouseClick2nd`: the
  range was cleared and the picture came out of a state nobody had seen. The state process records
  which scenario it is in; a shot of that one performs nothing and photographs what is there, and
  `replay()` with no steps clears nothing either. A build sets no live scenario and performs every
  one, because there the application has just been cleared.
- **A picture of the running application is taken before any question is asked.** `CWksItemDelegate`
  draws a project's device-sync, active-project and setup buttons at
  `IWksItem::getOpacityOfFocusBasedItems()`, which `holdUiFocus()` fades 0 -> 1 only while the
  option carries `State_HasFocus`, so a row has five buttons for the writer and two in a picture
  taken without focus. **Focus cannot be restored.** Measured on X11 with Qt 6.10.2: while another
  window is up the application has no focus at all - `hasFocus()` false, `focusWidget()` and
  `activeWindow()` null - `setFocus()` on a widget of an inactive window changes nothing, and
  whether the focus returns when that window closes is the window manager's decision and differed
  between two runs of the same test. The panel is a second process, so the application window is
  already inactive whenever the writer is using it. So `tag()` renders every part the writer could
  mean (`livePartsAt()`) at the key press and writes out the one they pick; the questions come
  after and cannot change what was taken. `CShotRegionPicker` takes `Qt::NoFocus` and reads Escape
  through an application filter for the same reason. A `select` step and a shot's own `select` do
  call `setFocus()` beside `setCurrentItem()` - that is what makes the headless build's rows look
  like the writer's.
- **`settle()` ends every running animation of finite length.** An animation runs on real elapsed
  time, so no number of event loop passes finishes one: the fade above takes 250 ms and a picture
  caught it at zero. An endless animation (`duration() == -1`) is a pulse with no end to put it at
  and is left alone.
- **A row button's signal cannot be a `Qt::UniqueConnection`.** The flag needs a pointer to a member
  function and refuses a lambda - "unique connections require a pointer to member function of a
  QObject subclass" - so the connect failed outright and no row button of any tree was ever
  recorded. `watchRowButtons()` disconnects the signal from itself first instead, which is what the
  flag was there for: `start()` can run more than once.
- **A scenario's `layout` carries every splitter's `saveState()` too**, keyed by the same address as
  everything else, and it is applied last with the tab - a details page's splitters do not exist
  while the arrangement is restored. `QMainWindow::saveState()` covers dockers and toolbars and
  nothing inside the central widget, and 32 classes write their own geometry in their destructor,
  which has not run when a scenario is stored. So the scenario says it itself and depends on no
  settings file. Without it a details page came back at default proportions, which also inflated
  its `minimumSizeHint()` and pushed the window 309 px wider than the scenario asked for.
- **A container input is driven through its own widget, never through what it holds** (`pressDrove()`).
  `isWithin()` is right for a control made of nothing but its own parts - a combo box owns the list
  that pops out of it - and wrong for the two inputs that hold foreign content: a `QTabWidget` is
  driven from its tab bar, a checkable `QGroupBox` from itself. A click in a plot switches the page
  the plot sits on, and because a press is recorded only when the diff found nothing, that `set`
  took the click's place and the first of two range clicks was lost.
- **A capture clears the press it answered.** `stop()` runs one last `captureChanges()` for what the
  final click produced too late for its own; with the press still standing, that recorded it a
  second time - two steps on one point, which replays as a range of no length: the screen option
  appears, the green band does not.
- **A widget with a vocabulary of its own is never recorded as a position**: the canvas has a
  geographic point, `IPlot` the x axis' own value (`xValueAt()`/`pointOfXValue()` - metres on a
  linear axis, seconds on a time one), `CIconGrid` the `<sym>` name
  (`iconAt()`/`rectOfIcon()`), and a workspace row's tool buttons the delegate's own names
  (`CWksItemDelegate::button_e`, `buttonAt()`, `pressButton()`). The row buttons are painted, not
  widgets, so the delegate's `sigButtonPressed` is the only thing that can report one; a replay
  reaches `pressButton()` directly rather than a point.
- **A recorded click is replayed as a click.** A geographic point reaches the canvas as a move, a
  press and a release, so whichever mouse delegate the scenario has put there answers it - normal,
  range, edit, ruler - and a mode nobody has taught the framework about replays like every other.
  `CMouseAdapter` counts a move only past `minimalMouseMovingDistance`, and `CMouseNormal::draw()`
  reads the item under the cursor out of the adapter's last position, so the point is approached
  from `kApproachPixels` away and the canvas repaints before the button goes down. The recorded
  `item` has to still be under the point, the rule `hit` follows everywhere else.
- **A replayed move is sent as an event (`moveMouseTo()`), never `QTest::mouseMove()`.** On a
  widget with no button down that one only calls `QCursor::setPos()` and leaves the move to the
  window system (qtestmouse.h, Qt 6.10.2). The offscreen platform answers it at once and X11 does
  not, so anything that needs the hover before the click - a track's screen options, and every
  scenario that goes through `toolRange` into range mode - worked in a headless build and not in
  the writer's own session.
- **A replay starts from nothing, not from what is on screen.** `CShotRecorder::replay()` calls
  `clear()` before its first step, because a step is not idempotent: a second click on a selected
  range takes the range away again. Documentation mode is where that bites - it holds a state up
  for the writer and `shootOne()` then builds the same scenario again to photograph it.
- **`clear()` puts the canvas' mouse delegate back** (`CCanvas::resetMouse()`, what a right button
  click does). Which delegate is there is state a scenario left behind, and it is
  `CMouseRangeTrk`'s destructor that returns the track to `eModeNormal` and lets go of its mouse
  focus - so the deferred delete has to be asked for
  (`sendPostedEvents(nullptr, QEvent::DeferredDelete)`). `processEvents()` does not deliver
  `DeferredDelete`; the event loop that posted it does, on its way out, and `settle()` never leaves
  one.
- **An `IScrOpt` overlay is a child widget of the canvas, not the map.** Only `CCanvas` itself is a
  geographic point; a press on `toolRange` or any other screen-option button is an ordinary
  addressed widget press, and recording it as a place on the map loses the button.
- **`IPlot` sets no `objectName`.** The per-instance tag the track compares its mouse-focus owner
  against is `IPlot::ownerTag`; making it the objectName too made a plot's address depend on how
  many plots were built before it. A plot placed by a `.ui` keeps its uic name; one built in code is
  addressed positionally (`framePlot/CPlotProfile#0`).
- **`--shoot`, `--doc` and their eight companions are parsed under `QMS_DOC_MODE` only**, so a user's
  binary rejects them instead of accepting a switch that does nothing. The values stay on `CAppOpts`,
  empty, so no reader needs a branch. `src/qmaptool/setup/` is a separate copy and never had them.
- **A Windows build owns no console.** `qt_add_executable(${APPLICATION_NAME} WIN32 ...)` links it for
  the GUI subsystem, so everything `CLogHandler` writes to `std::cout`/`std::cerr` is dropped and a
  `QCommandLineParser` error arrives as a message box. `IAppSetup::processArguments()` calls
  `AttachConsole(ATTACH_PARENT_PROCESS)` for a `--shoot`/`--doc` run and reopens both streams on
  `CONOUT$`, so the shell that started it gets the output and the state process inherits the handles.
  Anything a documentation run has to tell the writer is a dialog, never only a log line.
- **Never search `PATH` for the interpreter.** `shots.py` hands its own `sys.executable` over as
  `--doc-python`, and `CShotDocLauncher` runs `shots.py compose` with that. `python3` on Windows is
  normally the store's app execution alias, which opens the Microsoft Store and exits non-zero, and a
  session started through the `.py` file association need not have python on `PATH` at all.
- **A `QProcess` that fails to start emits no `finished()`.** `errorOccurred` with `FailedToStart` is
  the only report there is, so a launcher that waits for `finished()` alone waits behind its modal
  box forever. Put the box up before `start()`, not after: the failure is emitted from inside it.

A `--shoot` or `--doc` run must set `CMapDraw::setCacheRoot()` and `CGisListWks::setDatabasePath()`
before `CMainWindow` is constructed, or it prunes the user's tile cache and empties their
workspace. **Two mechanisms, both wanted:** that root protects the user's tiles during startup, and
the `Canvas/cachePath` `shots.py` injects into the scratch configuration is read by `CMainWindow`
afterwards and is the one the run then uses - one cache shared by the writer's session and the
replay, or the replay re-downloads every tile and draws an empty map without a network.

**`-platform` is plain `offscreen`, with nothing after it.** The plugin's `configfile=` parameter
pinned a 1920x1200 screen so `restoreGeometry()` could not clamp a window to the 800x600 default -
and a Windows run does not start with any parameter there at all. Measured 2026-09-02: the seven
`test` images are byte-identical without it, because every window a run photographs is resized by
`shootOne()` and `resize()` is not clamped to the screen. Do not put it back to fix a size; resize
the window instead. Everything else `shots.py` hands over is absolute.

**The offscreen platform has no font database on Windows.** It uses `QFreeTypeFontDatabase`, which
populates from `QLibraryInfo::LibrariesPath + "/fonts"` — a directory Qt no longer ships — so a
headless run there had no font at all and drew every glyph as an empty box. `src/fonts/` is
bundled in `resources.qrc` and registered by `IAppSetup::processArguments()` for `--shoot` and
`--doc` only. `--font-family` names a family; it cannot supply one.

**A picture is rendered at a device pixel ratio of 1, never `QWidget::grab()`.** `grab()` uses the
widget's own ratio, so a HiDPI writer accepted pictures twice the size the build produced. Qt 6
cannot put a real screen back to 1 — `CShotWriter` takes the ratio out of the paint device instead.
Exact for a dialog, a menu or a docker; a canvas is the exception, because `IDrawContext` sizes its
buffers by the widget's ratio, so a HiDPI writer's map lands downscaled — same size and layout,
resampled tiles.

**An opaque picture must not carry an alpha channel.** `grab()` gives one only to a widget that
does not paint its whole rect; rendering into an `ARGB32` image gives one to everything, which cost
a third of every PNG for pixels that were all opaque. `renderAtDpr1()` scans the result and
converts to `RGB32` when nothing used the alpha. Image weight is not cosmetic — every picture ships
inside the `.qch` that every user downloads.

**`(base)` is a state, so it has to be restorable.** `CShotDocMode` captures the arrangement and
the view once the fixture is up and replays that for the base row; selecting it used to fall into
`"This page has no scenario called %1"` with an empty `%1`. Nothing about the base is stored —
that is the point of it.

**A documentation run pins the colour scheme, it does not follow the desktop.**
`--color-scheme light|dark` → `CUiTheme::pinColorScheme()`, which sets *both* the style hint and
the application palette. Neither alone is enough: `QPlatformTheme::requestColorScheme()` has an
empty default implementation, so `QStyleHints::setColorScheme()` does nothing on X11, and
`QStyle::standardPalette()` reads the platform theme's scheme — the thing being pinned away. The
palette is the one that matters here, because `paletteIsDark()` reads it and `CUiTheme`,
`CQmsStyle` and the `.svgt` icon engine all follow from that.

**The documentation panel must not be `WindowStaysOnTopHint`.** It floated above the modal dialogs
the main window opens too, and one centred under it could be neither reached, closed nor moved out
from under — the application had to be killed. A parented `Qt::Tool` already floats above its own
window.

**Every comma in a `SHOT_EXPOSE` lambda must sit inside parentheses.** Only parens
protect a macro argument — a brace list (`x = {1, 2}`) or a template argument (`f<T, 1>()`) at
statement level splits the lambda into extra arguments and the error points at the closing `});`.
Put such a value in a helper function in the anonymous namespace and call it.

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
