# QMS-1217 — documentation images

Issue: Maproom/qmapshack#1217. Related: discussion #1209 (documentation rework).

Goal: every image in the user documentation is build output, regenerable by one command.

Status: a throwaway demo on branch `QMS-1217_demo`. It renders a real page, a writer can use it,
and a scenario is recorded rather than registered. The fixture is not built.

This file replaces `QMS-1217-screenshot-framework-plan.md`,
`QMS-1217-doc-mode-two-process-plan.md` and `shot-input-replay-plan.md`. They were three layers of
one plan, each superseding a section of the one before, and the layers had begun to contradict each
other and the code.

**How to read it.** §1-§7 are the design as it now stands - what the demo showed works. §8 are the
rules that fall out of it. §9 is evidence, measured on this checkout; do not re-derive it and do not
doubt it without a new measurement. §10 is what the demo does not do. §11 is the feature branch: the
demo is thrown away and built again as sub-tickets, each one reviewable on its own. §12 is out of
scope.

---

## 1. The units

A **page** is one `.md`, one JSON shot file, one directory of images:

```
doc/pages/<page>.md              the page, MyST, references ../images/<page>/<name>.png
doc/shots/<page>.json            the pictures and the scenarios, written by the application
doc/shots/<page>/<scenario>.ini  one scenario's whole configuration
doc/shots/fixture/shots.ini         the base every page opens on
doc/images/<page>/               the pictures
doc/shots/_cache/                   per-run tile cache and workspace database, git-ignored
```

The page is the unit because the application a user sees is not the application at first start:
dockers get rearranged, a writer tunes the layout to make a picture focused, and a setup page's
configuration changes *during* the page.

Inside a page the unit is the **scenario**: a picture is taken in one, and a scenario owns the
arrangement, the map and the settings it is taken with. Its pictures are a sequence of states, not
independent stills.

**`(base)` is a row, not a scenario.** It heads the scenario list and every *Taken in* box, is
stored nowhere, and a picture taken in it simply has no `scenario` key. A per-page copy of the
start state can go stale; the start state cannot. `--shoot-scenario -` is how a build asks for that
group, and `-` is refused as a scenario name.

**One base, a scenario may only add.** Maps and DEM are shared and nothing changes them; example
data is one base project a page may add a file *on top of*. Data that needs to *change* the base
means the base is wrong - fix the base. Storing a scenario's setup is an explicit button, never
automatic, or the setups drift with every session.

**A scenario's `.ini` is a whole configuration, never a patch on the base.** A file holding a
difference would move whenever the base moved, and a picture already taken would silently stop
reproducing. Storing a base changes what a page opens on and what the next recording starts
from, and nothing else.

## 2. A shot is data

`doc/shots/<page>.json`, written by documentation mode through `CShotPage::store()`:

```json
{
  "shots": [
    { "id": "test/menu-setup-workspace", "widget": "menuProject" },
    { "id": "test/workspace-setup-database", "exposure": "SetupWorkspace",
      "set": { "tabWidget.currentIndex": 2 }, "size": [620, 471] },
    { "id": "test/track-scropt", "scenario": "track-screen-option", "widget": "",
      "size": [1666, 741], "rect": [552, 144, 600, 266] }
  ],
  "scenarios": {
    "track-range": [
      { "do": "layout", "state": "…", "tab": 0 },
      { "do": "view", "lat": 47.5, "lon": 11.0099, "zoom": 11 },
      { "do": "select", "item": "Shoot Demo/trk:Demo Track" },
      { "do": "click", "item": "Shoot Demo/trk:Demo Track", "lat": 47.5048, "lon": 11.0085 },
      { "do": "click", "widget": "toolRange", "hit": "toolRange", "at": [0.54, 0.56] },
      { "do": "click", "lat": 47.4990, "lon": 10.9930 }
    ]
  }
}
```

| Key | Means |
|---|---|
| `id` | file stem, `<page>/<name>`; also what the page references |
| `widget` | address in the running application; empty is whatever is on top, else the main window |
| `window` | the class a scenario put on top, so a scenario that stops opening it fails loudly |
| `exposure` | build a fresh instance from the exposure catalog instead |
| `scenario` | the recording this picture is taken in; absent means `(base)` |
| `set` | properties to drive, `child.property` or a bare property of the shot's own widget |
| `size` | explicit render size - the window's, whatever part of it is photographed |
| `rect` | the part of the result that is kept |
| `note` | JSON has no comments |

JSON, not YAML: `QJsonDocument` is in Qt and `json` is in the Python standard library, and
`QJsonObject` sorts its keys, so a machine-written file diffs stably.

**The `.md` is what says a picture exists.** The shot file only says how it is taken. F9 offers
the names the page asks for and has not got, and only those.

**The window's size has exactly one record: the shot's `size`.** `layout` carries `saveState()` and
the central tab index, never `saveGeometry()`. `shootOne()` resizes the window *before* the scenario
runs and `restoreState()` distributes the dock extents into it - that order is required, because
those extents are pixels. Everything the scenario produces is measured against that size: where the
map is centred, where an item's options are anchored, what a stored rectangle frames. A shot the
main window sizes and that says nothing about how big it was is a counted failure; an exposure is
exempt, being built free of the layout.

**A rectangle is not a kind of shot**, it is what is kept of one. Whatever produced the picture -
a widget, an exposure, a scenario - the rectangle is cut out of the result, which is what makes a
region of something dynamic possible at all.

**Losing or changing a shot's scenario reduces the entry to its bare `id` and deletes the image.**
A widget address and a rectangle frame something else in another state.

## 3. Architecture

```
src/qmapshack/shoot/
  CShotRegistry     the exposed widget classes; SHOT_EXPOSE
  CShotContext      the live application and the fixture by role; shot() and frame()
  CShotWriter       settle, resize, grab, PNG
  CShotPage         the JSON shot file: addressing, item paths, store, shootOne, run
  CShotRecorder     records what the writer performs, and replays it
  CShotFixture      the example project
  CShotRunner       --shoot tasks; writes a JSON report beside the images
  CShotDocLauncher  the writer's session: the panel and every file operation
  CShotDocMode      the state process: F9, the shot dialogs, the channel
  CShotDocPanel     the writer's panel
```

**Developer-only.** `shoot/` is compiled and `Qt6::Test` linked only under `-DQMS_DOC_MODE=ON`, and
`main.cpp` compiles its two call sites out with the same define, so `--shoot` and `--doc` are inert
in a user's binary. The switches are parsed under the same define, so a user's binary rejects them
as unknown instead of accepting a switch that does nothing; the values stay on `CAppOpts`, empty, so
no reader of them needs a branch.

**Two drivers, one render path.** Documentation mode (`--doc`) and the headless run (`--shoot`) load
the same configuration and the same fixture and go through the same `CShotPage::shootOne()` and
`CShotWriter`, so the picture a writer accepts is what a later replay reproduces.

**Documentation mode is two processes.**

```
qmapshack --doc <repo> --doc-page <ch>                 the launcher: the panel, nothing else
    └── qmapshack --doc … --doc-scenario <name|-> …       the state: one scenario, thrown away
```

- The launcher owns the session - which page, which scenario, which picture, what a retake
  changed - and does every operation that is a file operation: the base question, the recording's
  name, delete, rebind, and removing what no page references. Its main window is constructed and
  never shown; `CMainWindow::self()` is what initialises `IUnit`, `CWptIconManager`,
  `CGisWorkspace` and eight more singletons the panel's data goes through.
- The state process is disposable. It comes up in one scenario, replays it, and stays for as long
  as the writer works in it. Picking another kills it and starts another. **Nothing is ever taken
  back down** - that is the guarantee, and it is what `reset()` could not give: the application's
  state is not enumerable, so a list of things to put back is never complete.
- The state process owns nothing but the window it is pointed at. `Ctrl+Shift+F9` stays there.
- They talk over a `QLocalSocket`, `--doc-channel`. Launcher to state: `region`, `update`, `retake`,
  `record`, `stop`, `sync`. State to launcher: `status`, `ready`, `tagged`, `changed`, `recording`,
  `recorded`.
- Closing either window ends the session. The state process quits when its window closes; the
  launcher quits when the state ends without it having asked (`killing`) and when its panel is
  closed. A state process whose channel drops quits too, so a killed launcher leaves no orphan.
- The launcher must never render: no fixture, no replay, no `CShotWriter`. A picture is always taken
  by the state process.

**The exposure catalog is the only C++ that grows.** `CShotExposures.cpp`, 52 entries, because a
constructor's arguments cannot be data. Only 9 of the 61 dialogs take nothing but a parent; the rest
want a fixture item, a singleton alive inside `CMainWindow`, or a result the dialog writes back
through a reference - the last kind gets a static of its own factory, set again for every build, so
no shot sees a value another one left behind. An entry is one line and is paid once per class, never
per image. The built widget is checked against `TYPE::staticMetaObject`, so every exposed class
declares `Q_OBJECT`; `SHOT_EXPOSE` does not compile for one that does not.

All 52 entries are `QDialog` subclasses; 52 of the tree's 61 are exposed. The nine that are not:
`CDetailsGeoCache`, `CDetailsOvlArea`, `CDetailsRte` and `CDetailsWpt`, which a scenario reaches
through `dclick`/`trigger` and which therefore need no entry; `CExportDatabase` and
`CSearchDatabase`, which need a live `QSqlDatabase` with content; `CRangeToolSetup`, which exists
only while the range mouse mode is active; and `CTemplateWidget` and `CShotDocPanel`, which are not
user-facing.

There are no scenario recipes. `IShotRecipe` and `RecipesChapter.cpp` are gone: a recipe was written
per *picture*, so the catalog grew with the documentation and had to be read to be used. A recorder
is asked for per *interaction kind*, and that list is §10.4's table.

A rename is caught by `shots.py replay` failing loudly, not by the compiler - `"dockWorkspace"` and
`"Shoot Demo/trk:Demo Track"` are strings either way, so keeping shots in C++ would protect nothing.

## 4. Recording

`CShotRecorder` watches through one event filter. What is stored is **addressed input**, not a state
diff: a click is a click whatever it lands on, so the vocabulary is indexed by Qt and not by
QMapShack's features. The adapters that keep a recording readable are a list of Qt classes and do
not grow with the application.

Writers cannot write code, so the recorder is not optional. Squish is not GPL, so it is not an
option either.

| action | payload | replays as |
|---|---|---|
| `layout` | `saveState()` + the central tab index | `restoreState()`, tab last |
| `view` | centre in degrees + zoom index | `zoom(index)` + `moveMap()` |
| `select`, `expand` | item name path | the workspace list |
| `set` | widget address, property, value | `driveProperty()` |
| `click` | widget address + `at` fraction + `hit`, or a vocabulary of its own | `QTest::mouseClick` |
| `dclick` | item path, or widget address + position | plain click, then `QTest::mouseDClick` |
| `trigger` | action objectName | `QAction::trigger()` |
| `menu` | widget address + item path or position | `QContextMenuEvent` to the viewport |
| `key` | widget address + key sequence | `QTest::keyClick`/`keyClicks` |

`layout` and `view` are taken whole when the writer stops, and put in front. That is the replay
order too: the arrangement decides how big the canvas is, the view decides what it looks at, and
only then does a geographic point mean the pixel the writer clicked.

**Address, never a global pixel.** A position is only ever stored relative to an addressed widget,
and only for widgets with no vocabulary of their own.

**A position is never stored alone.** It carries what it hit, and replay verifies the hit before
acting; a miss is a counted failure, not a picture of the wrong state. A widget metric moving 2 px
between Linux and Windows is measured, and on a 17 px delegate button that is enough to miss.

**A widget that names what it was hit on is recorded by that name.**

| widget | recorded as |
|---|---|
| canvas | `click` with lat/lon, plus the item path when exactly one thing is under it |
| `IPlot` | `click` with `x`, the axis' own value - metres on a linear axis, seconds on a time one |
| `CIconGrid` | `click` with `icon`, the `<sym>` name |
| workspace row buttons | `click` with `item` + `button`, the delegate's own button names |

The row buttons are painted into the row and carry no widget, so the press never reaches the filter
as anything but a point in the viewport; before the adapter such a click was recorded as the
selection it also produced and the toggle was silently lost. A replay reaches `pressButton()`
directly rather than a point.

**An `IScrOpt` overlay is a child widget of the canvas, not the map.** Only `CCanvas` itself is a
geographic point; a press on `toolRange` or any other screen-option button is an ordinary addressed
widget press.

**A press is a step only when what it landed on acts on a click** (`pressIsStep()`): a button, a tab
bar, a combo box, a header, or an item view row that exists. A splitter handle, a dock title, a
scroll bar, a menu bar, a label and the empty space under the last row are not. It is a whitelist,
so a widget nobody has taught the recorder about records nothing - recoverable, where a wrong click
is a picture of the wrong state.

**A menu entry is addressed by its action's `objectName`, never its text**, which is translated.
Every menu owner names its actions after the member they are assigned to; a menu built from data
takes a stable prefix plus an untranslated key: `actionActivity_<act20_e>`,
`actionColor_<GPX colour name>`, `actionWptIcon_<sym>`, `actionSearchWeb_<index>`,
`actionAddPoi_<POI name>`. A new `addAction` needs the same.

Done: `CGisListWks` (52), `CGisListDB` (14), `CMouseNormal` (10), `CSearchLineEdit` (8),
`CGeoSearch` (7), `IPlot` (6), `CGeoSearchWeb`, `CActivityTrk`, `CTableTrkInfo`,
`CHistoryListWidget`, `CWptIconManager` (3), `CPlotProfile`, `CTemplateWidget`, `CTextEditWidget`,
`IGisItem`.

**A recording always starts from the base**, so a scenario is a whole state and never a difference
from another scenario that is not stored with it.

**Where a recording stops:**

- A hover, a tooltip, a half-finished drag.
- A modal dialog opened during a recording: its `exec()` is a nested event loop a headless run
  cannot get out of, so such a widget is photographed through the exposure catalog instead.
- Whether a recording should be re-recordable in place. Today a changed scenario is a new recording
  under the same name.

## 5. Replay

`CShotRecorder::replay()` is a queue: each step schedules the next from the event loop instead of a
`for` loop calling them in sequence. That is what lets a step enter a modal dialog, a popup menu or
a nested progress loop while the steps after it still run, and it is what made the exposure catalog
unnecessary for anything reachable through the UI.

- **The picture is taken as the scenario's last step, never after it.** A step that opened a modal
  dialog is still inside its `exec()` there; by the time `replay()` returns the dialog is gone and
  the picture would be of the window behind it.
- **A replay starts from nothing, not from what is on screen.** `replay()` calls `clear()` before
  its first step. A step is not idempotent - a second click on a selected range takes the range
  away - and documentation mode replays a scenario it is already holding: once for a picture, twice
  more for a rectangle.
- **`clear()` takes back what a replay leaves**: the screen options, the canvas' mouse delegate
  (`CCanvas::resetMouse()`, what a right button click does), a track's mouse focus, and the hint a
  selection puts on the map. It runs before every replay, and after a shot unless the context is
  holding the state up for a writer.
- **A recorded click is replayed as a click.** A geographic point reaches the canvas as a move, a
  press and a release, so whichever mouse delegate the scenario put there answers it - normal,
  range, edit, ruler - and a mode nobody has taught the framework about replays like every other.
  The point is approached from `kApproachPixels` away and the canvas repaints before the button goes
  down, because the hover an overlay needs is computed while the canvas paints.
- **The arrangement's `tab` index is applied after every other step**, because a page a step adds
  does not exist while `restoreState()` runs, and `QTabWidget` drops an index past its last page
  without a word.
- **Every step waits for a condition**, never for a duration: `CShotWriter::settle()`,
  `settleStable()`, `CCanvas::waitForDrawContexts()`.
- **`driveProperty()` reads the value back.** `setProperty()` answers whether the property exists,
  never whether the value took.

## 6. The writer's loop

`shots.py take <page>` opens the launcher. `doc/WRITING.md` is the writer's guide.

**Two sections, one selection.** Scenarios are recorded, renamed and deleted at the top; the
pictures the page asks for are listed below, each row carrying the scenario it is taken in, in a
combo box of its own. A picture is taken in the scenario its own row names, and a name that has no
row yet is taken in the selected one.

**Ctrl+Shift+F9 is the only key** - photograph what the mouse points at. Everything else is a
button, because the mouse is busy pointing:

| Section | Button | Does |
|---|---|---|
| Scenarios | Record… | perform the state a picture needs; stop and it becomes the page's own |
| | Rename… | another name; no picture is invalidated |
| | Delete | throw it away, and with it every picture taken in it |
| | Save config | store the arrangement, the size, the map and the settings on screen into the selected scenario; on `(base)` it asks first |
| Pictures | Take a region… | drag a rectangle over the window in the picture's own scenario |
| | Take all again | take every picture of the page again, here, and report which came out different |
| | Remove unused | delete the pictures no page references |

F9 starts at the widget under the mouse and offers every step up to the whole window that a shot can
find again, renders a **fresh** instance through the headless path, and shows it to keep or throw
away. A class with no exposure prints the one line a developer has to add.

The panel lists what the page asks for against what exists: *taken*, *missing*, *no image*, *not
used*. A retake reports which pictures came out different, which is the only signal that a replay no
longer reproduces what the writer accepted.

**Starting a state takes about seven seconds**, because it is a whole application. The panel says so
and refuses input while it happens; without that the writer clicks again and the clicks queue up
behind a process that is still coming up.

**Never make the panel refuse to close.** It did, to keep the writer from losing it, and that turned
`qApp->quit()` - which `QGuiApplication` answers with `closeAllWindows()` - into a quit that never
happened.

## 7. shots.py

`doc/tools/shots.py`. Python because #1209 already requires every writer to have it and the three
platforms rule out a shell script.

| Command | Does |
|---|---|
| `take PAGE` | open the launcher so a writer can take the pictures a page asks for |
| `compose <page> [--scenario <name>] --out <ini>` | the one composer of a run's configuration; the launcher's only way in, hidden from `--help` |
| `replay [--only GLOB]` | replay every shot and report the ones that no longer replay, one process per scenario; `GLOB` is an id glob - `test/*` one page, `test/menu-project` one picture |
| `unused [--delete]` | pictures and shot entries no page references any more |

There is no `list`, `inspect` or `explore`: they were developer probes for authoring exposures and
nothing in a build depends on them. With them the `--shoot-task` switch goes as well - `--shoot`
plus `--shoot-target` is the whole interface - and 243 of `CShotRunner.cpp`'s 370 lines.

It composes the run's configuration into a scratch copy - the scenario's own file, or the base when
it has none - so a writer's session cannot drift what a build renders. The launcher runs the same
`compose` before starting a state process, so the writer's session and the build cannot disagree.

**What the tool owns is injected per run, never stored**: `Canvas/{mapPath,demPaths,poiPaths}` from
`doc/shots/fixture/`, absolute and therefore uncommittable, and `Database/saveOnExit=false` -
without it a run saves its workspace and the next one loads the demo project twice.

It pins `-style Fusion`, `--font-family DejaVu Sans`, `--font-size 10`, `--color-scheme light`,
`--locale en` with `LC_ALL`/`LANG`, `-platform-theme generic`, `TZ=UTC` and an unset
`QT_SCALE_FACTOR`, and prints which configuration file it read. Every task writes a JSON report
beside the images, so nothing parses stdout.

`diff` and `update` are deliberately absent: they mean nothing until the output is byte-stable.

## 8. Rules that hold

- **An exposure never calls `exec()`.** Construct, polish, size, render. That one rule is what makes
  menus, message boxes and modal dialogs tractable at all.
- **Drive inputs, never outputs.** Set `comboColorSource->setCurrentIndex(2)` and let the
  application's own signal chain produce the state. Setting a visibility directly can produce a
  state no user can reach and bakes today's logic into the shot; driving the input means that when
  the logic changes the shot does not become wrong, it renders differently.
- **Never reload the configuration in a running application.** 168 `SETTINGS` uses; the survey of
  2026-08-25 put 40 of them in constructors, 27 in destructors, 26 in slots and 9 in
  `loadSettings()`, and the shape has not changed. The dock layout is
  `restoreGeometry`/`restoreState` in the `CMainWindow` constructor. **One process per distinct
  configuration state** instead: startup is 1.3 s, so ~18 s for a ten-picture page and ~20 min for
  690, trivially parallel.
- **A window-sized picture must carry an explicit `size`.** Never render at the size the window
  happens to have; widget metrics differ per platform.
- **The run must stay hermetic.** Two paths escape `--config` and both were found by stepping on
  them: `CDiskCache::cleanupRemovedMaps()` deletes the cache directory of every map the current
  configuration does not know, so `CMapDraw::setCacheRoot()` is called before anything reads the map
  list; and `CGisListWks` opened the real `userDataPath()/workspace.db` and emptied it on exit, so
  `CGisListWks::setDatabasePath()` points it at `_cache/<page>-workspace.db`. Both must be set
  before `CMainWindow` is constructed. The count of hardcoded `~/.QMapShack` paths is unknown; each
  is found by a crash.
- **The platform argument carries nothing.** It pinned a screen through the offscreen plugin's
  `configfile=`, which a Windows run does not accept in any spelling - the reason every task but
  `doc` failed to start there.
- **Output naming leaves room for Sphinx's.** `figure_language_filename` defaults to
  `'{root}.{language}{ext}'`, so `CShotWriter` writes `<id>.png` for English and `<id>.<lang>.png`
  otherwise. English is all there is; translated pictures are a future topic, not part of this.
- **The pictures are repository content, not build output.** A checked-out branch has to render its
  documentation with no command run first, so every picture is committed and a PR carries the binary
  churn of the ones a change moved. Only `_cache/`, `_preview/` and the JSON reports are ignored.
- **Images ship inside the `.qch`**, so total image weight lands in what every user downloads. An
  argument for PNG discipline, against gratuitous full-window shots, and for keeping the fixture map
  small.

## 9. Measured

On this checkout, with a throwaway probe that drove the running application through its own UI, on
Linux and Windows, 13 of 13 checks passing on both. The probe is deleted; this section is what it
was for.

### How every number here was counted

Re-run these before trusting a number; they went stale once already. Last run 2026-09-07, from the
repository root.

```sh
grep -c "^SHOT_EXPOSE" src/qmapshack/shoot/CShotExposures.cpp             # 52 exposures
grep -rho "class [A-Za-z_]* *: *public *QDialog" src/qmapshack --include=*.h | wc -l   # 61 dialogs
grep -rho "\bSETTINGS\b" src/qmapshack --include=*.cpp | wc -l           # 168 SETTINGS uses
grep -rl "CMainWindow::self()" src/qmapshack --include=*.cpp --include=*.h | wc -l     # 71 files
grep -rho "QMessageBox::\(warning\|information\|critical\|question\)" \
     src/qmapshack --include=*.cpp | wc -l                                # 151 message box sites
grep -rn "public IScrOpt" src/qmapshack --include=*.h | wc -l             # 13 canvas overlays
grep -rn "QMenu menu" src/qmapshack --include=*.cpp | wc -l               # 9 stack-local menus,
                                                                          # one already split
```

`grep -c` counts *lines*, `grep -o | wc -l` counts *occurrences*; the two disagree wherever a line
carries the token twice, which is why each command above is written the way it is. The panel row of
§10.4 (40 of 57) has no command - it was a hand survey and is an estimate.

### The framework

- **`-platform offscreen` runs the whole application.** Map, DEM and POI paths load, canvases are
  created, draw threads start. No display, no window manager, no Xvfb.
- **Transient widgets render unshown.** A `QMenu` that was never popped up grabs complete with
  labels, separators, check marks and submenu arrows, and without a drop shadow - which is what
  documentation wants. `popup()` works too but logs offscreen warnings. Tooltips work only by
  fishing the private `QTipLabel` out of `topLevelWidgets()`. Static convenience dialogs
  (`QFileDialog::getOpenFileName`, `QMessageBox::critical`) block and cannot be shot; a constructed
  instance can.
- **`--config <file>` makes the settings hermetic**: `CSettings` switches to that file and the
  destructor's write-back lands in the scratch copy.
- **State installs through existing public API**: `CMapDraw::setupMapPath()`,
  `CDemDraw::setupDemPath()`, `CPoiDraw::setupPoiPath()`, `CGisWorkspace::loadGisProject()`,
  `CCanvas::loadConfig()` fed from a `.view` INI in the exact format *File ▸ Store View* writes.
- **A dialog inherits its form privately.** `private Ui::IX` → `protected Ui::IX` is the whole cost
  of reaching a widget from a subclass, one keyword, and only on the classes that need it.
- **A live `CMainWindow` is the fixture, not a workaround.** 71 files call `CMainWindow::self()`,
  and its constructor initialises `IUnit`, `CSearch`, `IPoiFile`, `IGisItem`, `CWptIconManager`,
  `CActivityTrk`, `CGisWorkspace`, `CGisDatabase`, `CToolBarConfig`, `CShortcutConfig` and
  `CGeoSearchConfig`.
- **Cost, on the demo:** framework ~3000 lines, `shots.py` ~400, the exposure catalog one line per
  class as predicted, changes to existing classes one keyword or one accessor each plus the menu
  split. One picture 1.3 s, fourteen 1.7-2.0 s cold, ~33 ms marginal - the cost is startup, not
  rendering.
- **`sizeHint`-driven pictures match across platforms exactly** (menu 234x372, dialog 550x473). A
  picture sized by whatever the window grew to does not: 1660x741 on Linux, 1482x741 on Windows.
- **`--dpr 2` changes nothing but text antialiasing.** Same image sizes, same layout, same icons.
- **Two runs of `shots.py replay --only test/*` are byte-identical** to each other and to the committed
  images (2026-09-02), so on one machine a picture is a function of the configuration alone. Across
  machines it is still unmeasured.

### Qt behaviour that cost a day each

- **`QTest::mouseMove()` on a widget with no button down only calls `QCursor::setPos()`** and leaves
  the move to the window system (qtestmouse.h, Qt 6.10.2). The offscreen platform answers that at
  once and X11 does not, so a headless build saw the hover before the click and the writer's own
  session never did. A replayed move is sent as a `QMouseEvent` (`moveMouseTo()`).
- **`processEvents()` does not deliver `DeferredDelete`** - the event loop that posted it does, on
  its way out, and `settle()` never leaves one. Anything whose destructor is the state change has to
  ask: `sendPostedEvents(nullptr, QEvent::DeferredDelete)`. `CCanvas::resetMouse()` is such a case;
  `~CMouseRangeTrk` is what returns the track to `eModeNormal` and lets go of its mouse focus.
- **`restoreGeometry()` drops the whole record - size included - when the screen width it is
  replayed against differs by more than a quarter** (Qt 6.10.2, measured: `factor < 0.8 || factor >
  1.25` returns false and applies nothing). A width of 0 is what Qt 5.3 and earlier wrote and takes
  the branch that only rejects a window wider than one and a half screens, so `portableGeometry()`
  writes that instead and a base stored on one machine still sizes the window on the next.
  `QWidgetPrivate::checkRestoredGeometry()` then moves a window that would land off screen back onto
  it.
- **What `CMainWindow` restores in its constructor is a size the docks then grow past.**
  `MainWindow/geometry` has to be applied again once the layout is populated, or a base stored at
  1200x876 comes back 120 pixels taller and a writer cannot make a size stick.
- **On Windows the offscreen platform's font database is `QFreeTypeFontDatabase`**, which populates
  itself from `QLibraryInfo::LibrariesPath + "/fonts"`, a directory Qt no longer ships. A headless
  Windows run had no font at all and rendered every glyph as an empty box while the same picture in
  `--doc` came out right. `--font-family` only names a family; it cannot supply one, so
  `src/fonts/DejaVuSans{,-Bold}.ttf` are in `resources.qrc` and registered for a `--shoot`/`--doc`
  run.
- **The desktop's platform theme answers `QPlatformTheme::standardButtonText()` out of its own
  translations**, so a KDE session puts "Abbrechen" on a dialog whatever `--locale` says, and adds
  the mnemonics the generic theme leaves off. `-platform-theme generic` is the pin.
- **`QApplication::activePopupWidget()` needs window activation**, which an offscreen run has none
  of. A visible `QMenu` among `topLevelWidgets()` is the fact underneath it.
- **`QContextMenuEvent` must be sent to the viewport.** Sent to the scroll area,
  `customContextMenuRequested` does not fire even with `Qt::CustomContextMenu` set.
- **`QAbstractItemView` drops a double click whose index does not match one a press recorded.** A
  plain click has to precede it or nothing is emitted.
- **`QTest::mouseClick` fires `clicked()` synchronously** in a normal application, offscreen.
  `QAccessibleActionInterface::doAction("Press")` does not - `animateClick()` is behind it.
- **A step scheduled on the event loop runs *inside* `exec()`.** A modal dialog is addressable,
  renderable and closable from there.
- **A collapsed tree row's children are not in the accessibility tree**, so `QAccessible` alone
  cannot address the workspace. `itemPathOf()`/`resolveItemPath()` stay.
- **`saveGeometry()` records the screen the window was on and where it sat on the whole desktop**,
  so on a multi-screen desk a state process landed away from the panel. Where the window sits is the
  writer's screen (`--doc-screen`, `panel->screen()->name()`), not a stored position. Measured on
  three screens: asked for `DVI-I-1-1` / `DVI-I-2-2` / `HDMI-1`, the frame came out at x=2320 / 400
  / 4240, always 1119x769.
- **`processEvents(flags, ms)` does not wait.** It strips `WaitForMoreEvents` and returns as soon as
  nothing is pending (qcoreapplication.cpp, Qt 6.10.2), so a loop built on it spins. Waiting for
  something that arrives over the network is a `QEventLoop` quit by a timer.
- **A decorated window belongs to the window manager until it is mapped.** The panel's geometry has
  to be applied one event loop after it is shown - this desktop answered the constructor's 460x760
  with 1200x996. Once only, so a writer's own resizing survives.

### QMapShack behaviour

- **`IPlot` sets no `objectName`.** The per-instance tag the track compares its mouse-focus owner
  against is `IPlot::ownerTag`; making it the objectName too made a plot's address depend on how
  many plots were built before it.
- **The view is the exact view**, `getPosFocus()` and `getZoomIndex()`. A visible *rectangle* is not
  enough: `zoomTo()` refits it to the canvas aspect and snaps it to a level, so the centre and the
  scale both drift and a stored crop frames the wrong thing.
- **The view is only the view.** Which maps, DEM, POI and grid are on belongs to the settings.
  `CMapDraw::loadMapList()` clears and rebuilds the map list and prunes the tile cache with the draw
  threads live, so a whole `CCanvas::saveConfig()` is not something a *running* canvas takes;
  `zoomTo()` is.
- **A click near an item is not a click on it.** `isCloseTo()` answers within 20 px and the range
  delegate takes one 200 px away (`MIN_DIST_FOCUS`), so a recorded point with nothing single under
  it is a step all the same and the point is all of it.
- **`CMouseAdapter` counts a move only past `minimalMouseMovingDistance`** (4 px) and drops a click
  held longer than `clickTimeout` (400 ms).
- **`settleStable()` is stable on an empty map.** It waits for two identical renders, and a blank
  canvas is identical to itself, so a build with a cold cache and no network produces blank maps
  quickly and quietly. What it catches is a map still arriving, not one that never does. A check
  that the map area is not one flat colour would close it.
- **No pending tiles is not a drawn map.** `CMapTMS::draw()` clears its queue when a redraw starts
  and the canvas paints the previous buffer until the draw thread swaps it, so completeness is
  `CCanvas::isDrawComplete()`: no redraw outstanding, no context running, then no pending tile.
  In that order - `CMapDraw::drawt()` holds `CMapItem::mutexActiveMaps` for its whole run.
- **A hidden canvas never completes.** `paintEvent()` returns before it clears `needsRedraw`, so
  it renders blank. A canvas not `isVisibleTo(w)` is not in the picture and is skipped; one that is
  but is not `isVisible()` (a `w` never shown) refuses the picture.
- **Completeness is asked before the render.** Checked after it, a draw finishing during the render
  is vouched for: with draws slowed to 1.5 s and the load indicator GIFs stopped, 3 of 3 accepted
  pictures carried the load indicator over the finished map (its hide is queued behind `finished`);
  checked before, 0 of 3. The animated GIF masks this at normal speed - 12 frames of 100 ms make two
  renders during a draw differ. A resize refused by a running draw is part of the check too
  (`drawContextViewportIsCurrent()`): with the window alternating 1200x800 / 1000x700 under the same
  slowed draws, 9 of 9 shots after a shrink were wrong without it, 0 of 18 with it.
- **A requested size is not the size a main window picture gets.** 1000x700 came out 1000x753, the
  window's minimum winning silently, so `shootOne()` (#1247) has to compare the result with `size` and
  count a mismatch. And the same size differs by resize history: 1200x800 reached again after
  1000x753 lays the right dock column out 1 px higher than the run's first 1200x800 (2026-09-14).
  A configuration with no `MainWindow/geometry` maximizes the window 500 ms after the constructor
  (`showMaximized`); `CShotRunner` waits 1000 ms before the first shot.
- **Measured for the hidden-canvas refusal (2026-09-14)**, through a temporary hook in `main.cpp`: a
  main window never shown was accepted as a 1200x800 picture whose canvas never painted with the
  old visibility filter, and is refused with the `isVisibleTo()` one; shown with the canvas in front
  and with a page added through `addWidgetToTab()` in front, both are accepted either way.
- **Measured for #1247 (2026-09-14):** `doc/shots/test.json`, four widget shots, renders
  byte-identically on two fresh configurations and after a clean rebuild; a page of twelve defective
  shots - renamed widget, unknown key, undeclared property, missing child, a size below the window's
  minimum, a rect outside the picture, no size, an exposure, a scenario, a duplicate id, the wrong
  window, a malformed size - exits 12 and writes nothing; 332 widgets of the main window address and
  resolve back; `applyView()` reads back exactly and refuses zoom 9999; nothing under `~/.QMapShack`
  or `~/.config/QLandkarte` changed.
  Only a window is resized: `test/workspace-filter` resized to its hint came out 355x240 where the
  window has the dock 355x125, and the next main window picture still showed it over the Database
  dock; resized only as a window, 355x125 and no trace. A `set` is put back after its shot: the main
  window before and after `test/workspace-filter` is byte-identical. A `size` on a widget another
  window lays out is refused - read from the code, not run.
- **Measured for #1248 (2026-09-14):** 35 exposures - the 52 of the demo less the 17 that need a
  fixture item - render, and two runs are byte-identical; a shot naming an unknown exposure fails and
  lists the 35 names sorted; `exposure` with `widget` fails; a second registration of `About` fails
  the run while the first still builds; an entry declaring `CUnitsSetup` and building `CAbout` is
  refused. On this branch there are 60 `QDialog` subclasses, not 61 (the demo's 61st is
  `CShotDocPanel`), and 5 exposed dialogs have no `Q_OBJECT`, not 3: `CAbout`,
  `CGeoSearchConfigDialog`, `CResolveDatabaseConflict`, `CTimeZoneSetup`, `CUnitsSetup`. Machine
  dependent content: `CAbout`'s library versions, `CWptIconDialog`'s icon path under the home
  directory, `CSetupDatabase`'s QMYSQL availability, `CPhotoViewer`'s `showMaximized()`.
  `CAbout`, `CUnitsSetup`, `CTimeZoneSetup`, `CGeoSearchConfigDialog` and `CResolveDatabaseConflict`
  got `Q_OBJECT`, so the registry checks `staticMetaObject`, not `typeid`; an entry whose class has
  none fails to compile. Each factory owns the values its dialog keeps a reference to: shared per
  type, `MapPathSetup` showed the folder name `SetupFolder` set; the 35 in reverse order are
  byte-identical to the 35 in order.
- **A failed tile stays a hole until its `CDiskCache` is replaced.** An error and an undecodable
  reply both store a null image, which the cache answers with a transparent dummy and never requests
  again, so a per-reply failure count goes stale and a reset per draw would miss it. `restore()`
  reports the hole and each `draw()` counts what it painted: `failedTiles()` is the last buffer's.
  The tile size a HiDPI source teaches (`tileSizePx`, `tileScale`) is learned from real tiles only:
  the 256 px dummy on a 512 px source flipped it on every draw and redrew forever. Measured on a local
  512 px TMS server missing one tile per zoom level: 20 s timeout before, refused with 1 hole in 0.5 s
  after, identical requests (2026-09-14). WMTS not run.
  A cache file that does not load is the same hole: stored as the dummy, it is counted on every draw
  (measured 2026-09-14: one garbage tile file, 4 of 4 shots refused; before, 4 of 4 accepted).
- **A main window destroyed while shown crashes.** `~QWidget` closes it, the docks emit
  `visibilityChanged` into `CMainWindow::slotDockVisibilityChanged()`, whose `docks` member is
  already gone (gdb, 2026-09-14). A run that leaves `exec()` calls `close()` on the window first.
- **Measured for #1246 (2026-09-14):** a 1200x800 main window and its canvas render byte-identically
  twice in one process and across a cold-cache and a warm-cache process; an unreachable tile server
  is refused with no picture written; a local server failing one zoom level: refused there, accepted
  at another zoom, refused again back at the first without a new request; nothing under the user's
  data directories changed.
- **The `.qm` catalogs are unreachable in a build tree.** `prepareTranslator()` resolves a filesystem
  path that is empty there, while all nine files are already embedded under `:/locale`.

## 10. What the demo does not do

1. **The fixture is half built.** The GIS data is synthesized in memory (`CShotFixture`): a
   `CQmsProject` with one track, waypoint, route and area, at a fixed epoch. Committed is only the
   map, `doc/shots/fixture/maps/osm.tms`, activated by `CShotFixture::build()` because a canvas with
   no active map covers itself with the welcome help. A DEM is a matter of putting files in
   `doc/shots/fixture/dem/`, not of touching code.

   Missing: a committed example project, so a page can add its own data. **This is what puts a
   developer in the writer's loop** - today every change to the example data is C++: a ticket, a PR
   and a rebuild before the writer can carry on.

   **The map is online, and that is a compromise**, not the offline raster #1209 asked for: a run
   needs a network the first time and byte-stability now also depends on the tile server. An offline
   extract of the fixture area replaces it without touching a line of code.
2. **Cross-machine determinism is unproven and deliberately not pursued.** One machine is
   byte-stable, and the two halves that failed in the field - the font and the device pixel ratio -
   are fixed. Nothing has compared two machines and nothing needs to: see the non-goal below.
3. **One language.** English, pinned rather than inherited. Translated pictures are out of scope
   for this feature: they would need a `:/locale` fallback in `prepareTranslator()` (the `.qm`
   files are unreachable in a build tree) and one process per language. A future topic, and it
   depends on whether the documentation is translated at all - there is no Sphinx project in this
   repository.
4. **Five mechanisms, each covering a whole bucket:**

   | Subject | Count | State |
   |---|---|---|
   | Dialogs | 61 | 52 exposed; 4 `CDetails*` reached by `dclick`/`trigger` instead, 5 cannot be built - see §3 |
   | `QWidget` panels with a designed form | 40 of 57 | mostly works |
   | Menus named in a `.ui`, or a member built in code | 6 + 24 | works, live address |
   | Menus built as a stack local ending in `exec()` | 8 | works - measured 2026-09-11: the track menu was photographed at its own 234x372 through a `menu` step plus `widget: ""` / `window: "QMenu"`, because the queue runs the steps inside `exec()` and `topmost()` returns the active popup. The `buildMenuXxx(QMenu&)` split is not needed |
   | `QMessageBox::warning/information/critical/question` | 151 sites, 62 files | **no** - no object to expose or address |
   | Canvas overlays (`IScrOpt` subclasses) | 13 | reached by recording a click: the 4 item overlays directly, the rest through the button that turns their mouse mode on |
   | Progress dialogs mid-operation (`PROGRESS_SETUP`) | 18 | **no** - exposable, the state is not |
   | WebEngine content (`CWebPage`, `QWebEngineView`) | 10 + 4 | **no** - `grab()` does not render it |

   Recounted 2026-09-07 with the commands in §9; the panel row is still the survey of 2026-08-25
   and has no command, so treat it as an estimate. `doc/pages/test.md` is the worked example: eight
   pictures, five in `(base)` and three in recorded scenarios, one of them a region.
5. **Animations.** Stills only. Capture is easy - start/stop bracketing frames through the existing
   `CShotWriter` numbering, stitched by an `ffmpeg` step that must resolve through
   `IAppSetup::getPlatformInstance()->findExecutable()` rather than becoming a dependency. Replay is
   the unproven half: a canvas interaction drives `IMouseEditLine` and its `ILineOp` states, the
   most re-entrancy-sensitive code in the project. Keep `CShotContext` frame-sequence-shaped so
   waiting costs nothing.
6. **The writer's half is untested end to end.** There is no Sphinx project in this repo. The pages
   under `doc/pages/` are MyST by shape only; `figure_language_filename` is a claim from the
   documentation, not a result.
7. **The writer is shown addresses, not names.** `chooseLivePart()` labels every step
   `windowTitle-or-className (address)` and `addressOf()` hands back Qt's own object names verbatim,
   so the choice reads `qt_scrollarea_vcontainer` against `CTableTrk`. The address is the stored
   identity and stays; the *label* wants what the writer sees - a docker's caption, a tab's text, a
   group box's title - and a `qt_`-prefixed internal should not be offered as a step at all.
8. **`-platform offscreen` does not deploy on Windows**, and is not meant to: `msvc_64/copyfiles.bat:75`
   and `CopyFilesGis.bat:57` copy `qwindows.dll` alone, and a packaged build has `QMS_DOC_MODE` off
   and rejects `--shoot` anyway. A Windows doc run is a developer's build tree; that it finds Qt's
   own offscreen plugin there is unconfirmed and belongs to whoever next builds on Windows.
9. **The colour scheme is pinned, light.** `--color-scheme light|dark` (`pinColorScheme()` in `CShotEntry.cpp`)
   replaces the application palette and sets the style hint. Two levers because neither is enough
   alone: `QPlatformTheme::requestColorScheme()` has an empty default implementation, so the style
   hint does nothing on X11, and `QStyle::standardPalette()` asks the platform theme for the very
   scheme being pinned away. `dark` is one constant in `shots.py` away.

**Open, and to decide before the naming is fixed:** whether the documentation carries a dark set at
all and how the scheme enters the file name; whether a picture of a message box is in scope; whether
a picture can be deliberately scaled down on the way out - asked for by a writer whose pictures were
too large.

**Cross-machine byte equality is a non-goal.** A picture reaches `doc/images` only through
`publish`, after a person looked at it, so nothing ever compares two machines' bytes: measuring
whether they agree answers nothing, and `diff`/`update` have no use. What has to hold everywhere is
that a replay *works*, and `shots.py replay` is what says so - run by hand. Windows packaging is off
the list for the same kind of reason: a packaged build has `QMS_DOC_MODE` off and rejects `--shoot`,
so deploying `qoffscreen.dll` beside it would serve nobody.

**Not part of this feature, all future topics:** a CI job, translated pictures, and animations.

## 11. The feature branch

The demo is a throwaway. It answered the design questions; it is not the code that ships. The branch
builds the same thing again as sub-tickets, each one reviewable and mergeable on its own, in this
order - every step leaves the tree working and every step has something that can be run to see it.

Cross-cutting requirements for every sub-ticket: the code compiles out under `QMS_DOC_MODE=OFF`; no
new dependency; nothing reaches outside `--config`; every failure is counted and printed with the
shot id and the offending name, never silently rendered.

**The test page is the harness.** #1247 creates `doc/pages/test.md` and `doc/shots/test.json` with
widget shots, because nothing in it can be checked without them; it grows with exposure shots
(#1248), the ones that need the fixture (#1249), recorded scenarios (#1251, #1252, #1253) and a
context menu (#1256). After every sub-ticket, `shots.py replay --only test/*` still produces the
pictures it could before that ticket, and the last one reproduces all of them.

**What is git-ignored, and by whom:** `doc/shots/_cache/` and `*.ttf binary` with #1245, which is
where the redirected tile cache, the scratch workspace database and the bundled fonts arrive;
`doc/images/_check/` with #1250, where `replay` renders; `doc/images/_work/` with #1254, where the
writer's own pictures wait. The demo's `doc/images/_preview/` and `doc/images/*.json` are not
ported - only `inspect` and `explore` wrote those - and neither is `doc/images/_cache/`, which only
appears if something renders straight into `doc/images`. That is
the acceptance test, not a diff against the demo: the port is reviewed as new code, so what has to
match is the behaviour and the facts in §9, never the demo's lines.

**No `#ifdef QMS_DOC_MODE` in the sources**, which is where the demo puts it (`CShotEntry.h`,
`CCommandProcessor.cpp`). The build option selects which file is compiled, and a build without it
gets its answers from a stub source against the same header. If nothing tests the macro, the
`target_compile_definitions` goes too.

**Three of the demo's changes are not documentation work and land before the port**, each against a
ticket of its own, because none of them needs the framework to be reviewable:

1. **#1242, bug: Qt's `.qm` is loaded for a language QMapShack has none for**, so Qt's strings are
   translated in an otherwise English window. The gate cannot be `QTranslator::load()`'s return
   value, which is what the demo uses: `load("qmapshack_pl", dir)` drops the `_pl` and succeeds with
   the installed, untranslated `qmapshack.qm`, so on an installed tree the demo's version does not
   close it - only in a build tree, where the translation directory does not exist at all. It has to
   test whether a file for the resolved language exists.
2. **#1243: the Win32 include block** is unguarded against clang-format, which sorts `windows.h`
   last and breaks the MSVC build (measured on this checkout). `// clang-format off` around the
   block, as `CMainWindow.cpp` does, needs no Windows build to verify; the demo's `windows.h` alone
   does.
3. **#1244: the man pages** document 3 of the 10 options, `qmapshack.1` has a broken roff escape in
   the SYNOPSIS and a stray `.TP`. The `--shoot`/`--doc` switches and `--color-scheme` belong in a
   section of their own, not under OPTIONS - they exist only in a `QMS_DOC_MODE` build.

| # | Sub-ticket | Scope | Done when |
|---|---|---|---|
| 1 | **Hermetic run** (#1245) | `--shoot`/`--doc` switch parsing under `QMS_DOC_MODE`, no `--shoot-task` - `--color-scheme` gated with the rest, which the demo leaves open - the bundled font, `pinColorScheme()`, `CQmsStyle::pinThemeIndependentHints()`, the console attach in `pinEnvironment()`, cache root and workspace database redirection. `--config`, `--locale` and `QLocale::setDefault()` are already on `dev` and are no part of it | the application starts headless against a scratch configuration and touches nothing under `~/.QMapShack` |
| 2 | **Render path** (#1246) | `CShotWriter` (settle, settleStable, a window resized to its hint, dpr 1, PNG), `CShotContext`; tile completeness through `IMap::pendingTiles()`/`failedTiles()` and its `IMapOnline` / `CMapDraw` / `CCanvas` aggregation | one widget renders to a file, twice, byte-identically; a streaming map is never photographed half drawn |
| 3 | **Shot file and `shootOne()`** (#1247) | the JSON schema of §2, `addressOf()`/`resolve()` symmetric, `set`, `size`, `rect`, `view` as a centre plus a zoom level (`CCanvas::getPosFocus()`/`getZoomIndex()`), the failure counting | a page of plain widget shots renders; a renamed widget fails loudly |
| 4 | **Exposure catalog** (#1248) | `CShotRegistry`, `SHOT_EXPOSE`, the class checked against `staticMetaObject` with `Q_OBJECT` added where it was missing; values a dialog keeps a reference to owned by its factory; `live<T>()`. No `private`→`protected` form change is needed - the demo has none | a throwaway shot file with one entry per exposure renders all of them |
| 5 | **Fixture** (#1249) | `CShotFixture` plus a **committed** example project and an **offline** map extract; DEM and POI directories; a database, without which `CDBItemDelegate`'s vocabulary cannot be recorded | a run needs no network and no writer's data |
| 6 | **shots.py** (#1250) | `compose`, `take`, `replay`, `unused`; the pinned command line and environment; one process per scenario; the leak guard - snapshot `~/.QMapShack` and the user's `workspace.db`, run, diff - which #1245 tested with a throwaway script | `shots.py replay` replays every committed shot, and reports a run that wrote outside the scratch tree |
| 7 | **Recorder: vocabulary** (#1251) | the event filter, `pressIsStep()`, the step table of §4, `driveProperty()` | a recording of a menu, a control and a row is stored and reads as meaning |
| 8 | **Recorder: adapters** (#1252) | canvas, `IPlot`, `CIconGrid`, and the painted row buttons of all three item delegates - workspace, maps, database; the menu-owner `objectName` audit | each adapter verified end to end against a throwaway page |
| 9 | **Replay** (#1253) | the queue, `clear()` before and after (`CMouseNormal::clearScreenOption()`, `CCanvas::resetMouse()` and the `DeferredDelete` it needs, public `waitForDrawContexts()`), the click path, hit verification, the `tab`-last rule | a page with a scenario reproduces byte-identically, three times in one process |
| 10 | **Launcher, panel, channel** (#1254) | the session and every file operation, `childArguments()`, the panel's buttons and statuses, `setBusy`, `mayClose()`, `endSession()`, the `QLocalServer` named `qms-doc-<pid>` | the panel comes up, starts and replaces a state process, and asks before closing over unpublished pictures |
| 11 | **State process and F9** (#1257) | one scenario held up, `Ctrl+Shift+F9`, the keep/throw preview, the region picker, `portableGeometry()`/`namesAPlace()`, `settingsDrift()` | a writer records a scenario and tags a picture without touching a file |
| 12 | **Writer-facing labels** (#1255) | names, not addresses, in `chooseLivePart()`; `qt_`-prefixed internals not offered | the step list reads in the writer's words |
| 13 | **Drop the menu split** (#1256) | delete `CGisListWks::buildMenuItemTrk()`, which the replay queue made pointless, and cover a stack-local menu with a shot instead | a shot of the track context menu replays; nothing calls a `buildMenuXxx()` |

**Review focus, per area.** What is worth a reviewer's attention is not spread evenly:

- **1, 5** - anything that writes outside the scratch tree. Both escapes so far were found by a
  crash, not by reading.
- **3, 7, 8** - `addressOf()` and `resolve()` must stay symmetric, and every new address kind needs
  both halves in the same commit.
- **8** - the two places where the demo rewrites shipped behaviour rather than adding to it, both to
  be reviewed line by line and not taken over: extracting `pressButton()` out of the three delegates
  inverted `eSave`'s nesting, turned `eActiveProject`'s focus test into a parameter, let a null item
  on the activate button fall through to `QStyledItemDelegate::editorEvent()`, and made a database
  row's double click report the check-state button; and `IPlot` lost its `objectName` outright, so a
  plot built in code is addressed positionally - keeping the name beside `ownerTag` is the
  alternative, and nothing has checked what else looks a plot up by name.
- **3, 9** - the demo reaches the workspace tree through a non-const `CGisWorkspace::getWksList()`,
  which hands every caller write access to serve one. A narrower accessor is preferred.
- **9** - ordering and idempotence. The bugs here do not look like bugs: they look like a picture of
  a slightly different state.
- **10, 11** - lifetime. Two processes, a socket, a window that may be gone; every path that can leave
  an orphan or a veto on quit.

**What a sub-ticket must carry:** the page it was verified against, the images it changed, and a
line in the repo's `CLAUDE.md` for any behaviour a reader could not have guessed. The demo's own
lessons are §9 - a fact that cost a day belongs there and not in a commit message nobody reads
again.

## 12. Non-goals

- The documentation toolchain, page structure and hosting (#1209), beyond the constraints Sphinx
  imposes here.
- Pictures of online map *services* as documentation subjects. The fixture map is itself online
  today, which is a compromise on this rule, not an exception to it.
- Retranslating a live `CMainWindow`.
- Making ffmpeg a build dependency.
- Icons. Inline icon glyphs in the documentation are `.svgt` renders, not screenshots - a small
  exporter, a separate ticket.
