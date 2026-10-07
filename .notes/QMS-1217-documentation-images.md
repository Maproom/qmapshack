# QMS-1217 — documentation images

Issue: Maproom/qmapshack#1217. Related: discussion #1209 (documentation rework).

Goal: every image in the user documentation is build output, regenerable by one command.

Status: a throwaway demo on branch `QMS-1217_demo`. It renders a real page, a writer can use it,
and a scenario is recorded rather than registered. On `QMS-1217` the fixture data is committed and
`CShotFixture` loads it (#1249); `shots.py replay` and `unused` run the pages headless (#1250);
`shots.py take` opens the launcher and panel, `shots.py publish` exists (#1254); the state process takes
pictures with F9 and a region, records, and stores a recording only once it replays (#1257); F9 offers
the parts in the writer's words (#1255).

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
doc/shots/<page>.ini                the page's base
doc/shots/fixtures/default/         the fixture; fixtures/<page>/ holds what a page's differs in
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

**One base for the data, its own configuration per scenario.** Maps and DEM are shared and nothing
changes them; example data is one base project a page may add a file *on top of*. Data that needs to
*change* the base means the base is wrong - fix the base. Settings are not data: a scenario carries
the whole configuration it was recorded against, so it reproduces whatever the base becomes later.

**A scenario's `.ini` is a whole configuration, never a patch on the base.** A file holding a
difference would move whenever the base moved, and a picture already taken would silently stop
reproducing. It is written once, from the settings on screen when the recording starts, and the trial
that decides whether the recording is kept replays against it - there is no button that stores a
scenario's settings, a scenario is changed by recording it again. Storing a base changes what a page
opens on and what the next recording starts from, and nothing else.

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
  CShotPage         the JSON shot file: store, shootOne, run; the layout and view steps
  CShotAddress      widget addresses, workspace item paths, row paths and hits, each with its resolver
  CShotInput        what typing leaves in an input, and which inputs a replay cannot reach
  CShotStep         the steps a recording is made of, spelled in one place
  CShotApplication  the QApplication of a doc run: the frame around one user input being delivered
  CShotSynth        input through the window system, as a user's own: what replay performs steps with
  CShotRecorder     collects the steps the handlers make, in the order of the inputs that caused them
  IShotHandler      everything a recording knows about one class: its signal, its step, its replay
  CShotHandlers     the registry, keyed by QMetaObject; the handler of every class that is recorded
  CShotSelfTest     the recorder's own cases: real input in, steps compared, steps replayed
  CShotFixture      the example project
  CShotRunner       --shoot tasks; writes a JSON report beside the images
  CShotFiles        a page's files on disk: shot file edits, scenario configurations, picture paths
  CShotDocState     the launcher's handle on one state process: process, channel, follow-up command
  CShotsJob         one shots.py run with a single queued outcome
  CShotDocSelfTest  page rules and process handles, run by shots.py selftest
  CShotDocLauncher  the writer's session: the panel and every file operation but two
  CShotDocMode      the state process: F9, the shot dialogs, the channel, writing a recording and
                    the base configuration
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
  changed - and does every file operation but two: rename, delete, rebind, reset, publish and
  removing what no page references. It asks the base question and a recording's name; the state
  process writes the base configuration and the recording. Its main window is constructed and
  never shown; `CMainWindow::self()` is what initialises `IUnit`, `CWptIconManager`,
  `CGisWorkspace` and eight more singletons the panel's data goes through.
- The state process is disposable. It comes up in one scenario, replays it, and stays for as long
  as the writer works in it. Picking another kills it and starts another. **Nothing is ever taken
  back down** - that is the guarantee, and it is what `reset()` could not give: the application's
  state is not enumerable, so a list of things to put back is never complete.
- The state process owns nothing but the window it is pointed at. `Ctrl+Shift+F9` stays there.
- They talk over a `QLocalSocket`, `--doc-channel`. Launcher to state: `region`, `update`, `record`,
  `stop`, `name`, `discard`, `select`, `sync`. State to launcher: `status`, `ready`, `tagged`,
  `recording`, `recorded`, `recorded-pending`, `recorded-none`, `trial-failed`.
- Closing either window ends the session. The state process quits when its window closes, when its
  channel drops and when it cannot connect (`CShotDocMode::leave()`); the launcher quits when the state
  exits with 0 and when its panel is closed. A state that dies otherwise leaves the panel up.
- The launcher process must never render: no fixture, no replay, no `CShotWriter`. A picture is taken
  by the state process, or by a `shots.py` child it starts - *Take again* and *Take all again*.

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

A recording is **the state it started in and every change after it, as steps**. `CShotRecorder::start()`
takes `layout` and `view`; nothing is taken at `stop()`, where a change would be applied twice.

- **One clock.** `CShotApplication::notify()` numbers a frame around each spontaneous input event,
  closed when the delivery returns, nested loops included. Steps are sorted by frame: the
  application's slot runs before the recorder's connection, so what is done in a dialog a slot
  opened is recorded first and carries the higher number.
- **The input went to the control.** A step is kept only when the input being delivered went to the
  control that made it - for an action, to a widget it sits in. Everything else is the application's
  answer, which replaying the input repeats.
- **One handler per class.** `IShotHandler` holds a class's signals, the step each is, and the
  replay; `CShotHandlers` keys them by `QMetaObject`, nearest class above wins, and dispatches a
  step to the handler of what it resolves to.
- **Nothing lost silently.** A key press no handler made a step of is a `keypress` step; a press on
  a class nothing covers is reported once.

Writers cannot write code, so the recorder is not optional. Squish is not GPL, so it is not an
option either; what Squish does is what this does, in the size this project needs.

| Qt says, while the input goes to it | step | replayed as, then checked |
|---|---|---|
| `QAction::triggered` | `trigger` action [+ `widget` when the name is not unique] [+ `checked`] | a click on its entry when its menu is open, else `trigger()`; checked state |
| `QAbstractButton::clicked` | `click` [+ `checked`] | `click()`; checked state |
| `QGroupBox::clicked` | `click` + `checked` | Space on the group box; checked state |
| `QComboBox::activated` | `set currentIndex` | `setCurrentIndex()`, `activated`, `textActivated` |
| a line edit's, a spin box's or an editable combo's text edited, a completion picked | `key` final text [+ `enter`] | select all, Backspace, typed, Enter; the text |
| the focus leaving an edited input | `endedit` | the focus cleared if still there |
| `QSpinBox`/`QDoubleSpinBox` value signal (stepped, wheel) | `key` final text | as typing |
| `QDateTimeEdit::dateTimeChanged` | `set dateTime` | `setDateTime()`; the value |
| `QAbstractSlider::valueChanged` | `set value`; a scroll area's bar `set share` | `setValue()`; the value |
| an item view's scroll bars, wheel or drag | `scroll` + top `row` [+ `x` share] | `scrollTo(PositionAtTop)`; the top row |
| `QSplitter::splitterMoved` | `set sizes`, shares | `setSizes()` scaled; within a pixel |
| `QDockWidget` floated, docked, moved | `arrange`, `saveState()` | `restoreState()`; floating and area |
| `QTabBar::currentChanged` within the tab widget; `tabCloseRequested` | `set currentIndex`; `close` | `setCurrentIndex()`; `tabCloseRequested` |
| `QHeaderView::sectionClicked` | `click` + `section` | a click on the section; sort indicator |
| `QAbstractItemView::clicked`, `doubleClicked`, `expanded`, `collapsed` | `select`, `dclick`, `expand`, `collapse` + `row` + place in it | a click there / `setExpanded()`; current row |
| a delegate's `sigButtonPressed` | `click` or `dclick` + `row` + `button` [+ `mouse`] | a click where `buttonRect()` puts the button; the signal |
| a menu shown for a context menu request | `menu` + `row` or place | the pointer onto the place, a right click |
| a menu shown by a tool button, a menu bar entry or a submenu entry | `openmenu` + `widget` or `action` [+ `menu` name] | `showMenu()`; a click on the bar's or the open menu's entry; the menu shown |
| a key press no step came of | `keypress` + key, text, modifiers | the key into its widget |

**A surface is recorded as its input** (`CSurfaceHandler`) - a gesture another step comes in the middle of as `press`, `move` and `release` with pixel offsets from the press: `CCanvas` in degrees with the item under
the point as `hit`, `IPlot` by the x value it reads there (`xValueAt()`, the plot's own rule), else a
place in the widget, `CIconGrid` by the icon's name. A press to its release is one `click`, `drag` or
`dclick`, ordered at the press, with the release as a pixel offset - a drag moves the content - and
`held` by the clock when it was held past `CMouseAdapter::clickTimeout`. A hover is one `move`,
amended. A `wheel` keeps both deltas and the modifiers. Replay checks `hit` and that the point
reaches the surface. A row button of the three item delegates is a `click` with `row` and `button`,
reported by the delegate's `sigButtonPressed` and replayed as a click where `buttonRect()` puts it now.

**A menu entry is addressed by its action's `objectName`, never its text**, which is translated.
Every menu owner names its actions after the member they are assigned to; a menu built from data
takes a stable prefix plus an untranslated key: `actionActivity_<act20_e>`,
`actionColor_<GPX colour name>`, `actionWptIcon_<sym>`, `actionSearchWeb_<index>`,
`actionAddPoi_<POI file base name>_<id in the file>`, or `actionAddPoi_<name>_<lat>_<lon>` for a map's POI, which has no id. A new `addAction` needs the same.

Done: `CGisListWks` (52), `CGisListDB` (14), `CMouseNormal` (10), `CSearchLineEdit` (8),
`CGeoSearch` (7), `IPlot` (6), `CHelpBrowser` (3), `CGeoSearchWeb`, `CActivityTrk`, `CTableTrkInfo`,
`CHistoryListWidget`, `CWptIconManager` (3), `CPlotProfile`, `CTemplateWidget`, `CTextEditWidget`,
`IGisItem`.

**`CShotSelfTest` is what shows it works** (`shots.py selftest`): each case performs input through the
window system while recording, compares the steps, replays them and compares the state the replay
leaves with the one the recording left. The replay runs through `CShotReplay::perform()`, so a step that opens a
menu with `exec()` gets its pick delivered from inside it. The contract the vocabulary is held to - finite,
tested, reported where it ends, and every recording replayed before it is saved (#1257) - is in
`QMS-1251-recorder-signals-plan.md`.

**Not recorded:** a window closed by its title bar - Qt 6 closes a widget through `QWindow::close()`,
the same spontaneous close as the application's own; re-docking a floating dock through its window
frame; a main window separator dragged (reported); typing into an item view's cell editor.

**A recording never runs while a replay does**: replayed input is delivered like a user's own.

## 5. Replay

`CShotReplay::perform()` queues each step from the event loop before it runs and performs it through
`IShotHandler::replay()` of the class the step's target belongs to - the handler that recorded it. A
step that opens a modal dialog, a popup menu or a nested progress loop gets the steps after it
delivered from inside that loop.

- **The next step runs inside a running step only when that step waits in an `exec()`**:
  `QThread::loopLevel()` above its level at the start and a popup or modal up. A synthesized click on
  a menu bar delivers queued steps before it returns, at the same loop level with the menu up.
- **The picture is the replay's last step** (`whenReady`): a dialog a step opened is still inside its
  `exec()` there. Popups and modal dialogs are closed after it.
- **One scenario per process.** `shots.py` starts one process per scenario with its own `.ini`;
  `CShotPage::run()` refuses a matched scenario shot without `--shoot-scenario`. Within a process the
  same scenario is replayed once per shot.
- **`clear()` runs before every replay and after every scenario shot**: `CCanvas::abortMouse()` (the
  screen options), `resetMouse()` with `sendPostedEvents(DeferredDelete)`, the selection's map hint,
  and the mouse focus of every track a step's `hit` names.
- **The start state comes first**: the leading `layout` (`restoreState()`) and `view`, then
  `settleStable()` on the main window before the first step - during a redraw the DEM is locked by
  the draw thread.
- **The `tab` index and splitter states are applied with the leading `layout`, before the steps**:
  they were taken when the recording started, and a step may change them (Edit opens a details tab).
- **A context menu's shot carries the step that opens it** (`open`): the scenario stays free of
  open menus, and the replay runs `open` last and takes the picture inside the menu's `exec()`.
- **A canvas step waits until the map has finished drawing** (`CCanvasHandler::settle()`): an item's
  pixels are updated by the draw, so a `hit` or click after a zoom finds nothing until it is done.
- **A recorded click is a press and a release at the point**; the hover before it is a `move` step of
  its own.
- **A replay not finished after `kDeadlineMs` (60 s) ends the process** with exit code 1, after naming
  the step that has not returned.
- **A handler that sets instead of clicking reads the value back** (`CShotPage::driveProperty()`):
  `setProperty()` answers whether the property exists, not whether the value took.

## 6. The writer's loop

`shots.py take <page>` opens the launcher. `doc/WRITING.md` is to be the writer's guide; it is not
written yet.

**Two sections, one selection.** Scenarios are recorded, renamed and deleted at the top; the
pictures the page asks for are listed below, each row carrying the scenario it is taken in, in a
combo box of its own. A picture is taken in the scenario its own row names, and a name that has no
row yet is taken in the selected one.

**Ctrl+Shift+F9 is the only key** - photograph what the mouse points at. Everything else is a
button, because the mouse is busy pointing:

| Section | Button | Does |
|---|---|---|
| Scenarios | Record… | perform the state a picture needs; stop, name it, and it becomes the page's own once a trial replays it |
| | Rename… | another name; no picture is invalidated |
| | Delete | throw it away, and with it every picture taken in it |
| | Save config | store the arrangement, the size, the map and the settings on screen as `(base)`, which it asks first; a scenario keeps what it was recorded with |
| Pictures | Take again | `shots.py replay --only <id>` of the selected picture into `_work`: headless, the way a build renders it |
| | Region | drag a rectangle over the window in the picture's own scenario |
| | Revert | throw the work picture away and put the shot's entry back; the published one stays |
| | All | `shots.py replay` of the page into `_work`: every picture taken again, to compare and publish or revert |
| | Remove unused | delete the shots and pictures no page references |
| | Reload page, Publish | read the page again; `shots.py publish` |

F9 starts at the widget under the mouse and offers every step up to the whole window that a shot can
find again, each named the way the writer sees it - a dock's caption, a tab's text, a group box's
title, a window's title, the class name only when nothing else names it, and the address appended
only where two would read the same. Qt's own `qt_`-prefixed widgets are not offered. It shows what it
took to keep or throw away. A live part is written from the render made
at the key press; everything else - an exposed window above all - is a **fresh** instance through the
headless path. A class with no exposure prints the one line a developer has to add.

The panel lists what the page asks for against what exists: *taken*, *not taken*, *no image*, *not
used*, *not registered*, and *taken again* while a work picture waits. A page lists only its
own ids (`<page>/<name>`).

**Starting a state takes about seven seconds**, because it is a whole application. The panel says so
and refuses input while it happens; without that the writer clicks again and the clicks queue up
behind a process that is still coming up.

**The panel may refuse to close only by asking.** Over unpublished pictures it offers Publish / No /
Cancel, and it refuses while a publish runs. Refusing silently turned `qApp->quit()` - which
`QGuiApplication` answers with `closeAllWindows()` - into a quit that never happened.

## 7. shots.py

`doc/tools/shots.py`. Python because #1209 already requires every writer to have it and the three
platforms rule out a shell script.

| Command | Does |
|---|---|
| `take PAGE` | open the launcher so a writer can take the pictures a page asks for |
| `compose <page> [--scenario <name>] [--from <ini>] --out <ini>` | the one composer of a run's configuration; the launcher's only way in, hidden from `--help`. `--from` names the source file instead of resolving it from the page and scenario, which is how a trial replays against a parked recording's settings |
| `replay [--only GLOB]` | replay every shot and report the ones that no longer replay, one process per scenario; `GLOB` is an id glob - `test/*` one page, `test/menu-project` one picture |
| `unused [--delete]` | pictures and shot entries no page references any more |
| `publish` | copy the pictures a writer retook from `doc/images/_work/` into `doc/images/` and empty `_work/`; no render, no comparison |

There is no `list`, `inspect` or `explore`: they were developer probes for authoring exposures and
nothing in a build depends on them. With them the `--shoot-task` switch goes as well - `--shoot`
plus `--shoot-target` is the whole interface - and 243 of `CShotRunner.cpp`'s 370 lines.

It composes the run's configuration into a scratch copy - the file `--from` names, else the
scenario's own file, or the base when it has none - so a writer's session cannot drift what a build
renders. The launcher runs the same
`compose` before starting a state process, so the writer's session and the build cannot disagree.

**What the tool owns is injected per run, never stored**: `Canvas/{cachePath,mapPath,demPaths,poiPaths}` and
`Route/routino\paths` from the page's fixture, absolute and therefore uncommittable, a
`Database/Entries` pointing at a scratch copy of `database/Example.db`, and
`Database/saveOnExit=false` - without it a run saves its workspace and `CShotFixture` refuses the
next one. The cached tiles' modification times are refreshed before a run.

It pins `-style Fusion`, `--font-family DejaVu Sans`, `--font-size 10`, `--color-scheme light`,
`--locale en` with `LC_ALL`/`LANG`/`LANGUAGE`, `TZ=UTC` and Qt's whole scaling family unset, and with
`-v` prints which configuration file it read. `QT_QPA_PLATFORMTHEME=generic` is the application's own
pin (`CShotEntry::pinEnvironment()`), not set on Windows. What a run says about a picture
is the `shoot:` warnings on stderr - every one names its shot - and the exit code, which is the
failure count; the application writes no report.

`replay` refuses to start while another QMapShack runs: the leak guard compares the user's cache and
settings before and after a run, and a session of the writer's own writes there too. A running
QMapShack holds a lock on `<user data>/.QMapShack.lock` (`.lock` on macOS) - an `fcntl` write lock on
unix, an exclusive open on Windows - which a documentation run never takes; the tool tests that lock
and never creates the file. The tool needs Python 3.9 and nothing outside its standard library.

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

- **QTest's `QWidget` functions are no real input.** Built without `QTEST_QPA_MOUSE_HANDLING` they
  call `notify()` on the widget and skip the window's routing, and a move with no button down is only
  `QCursor::setPos()` (qtestmouse.h, Qt 6.10.2). Its `QWindow` functions go through
  `QWindowSystemInterface`; `CShotSynth` is built on those, for replay and for `CShotSelfTest`.
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
  position the writer last moved a state's window to (`placeWindow()`, `doc-panel.ini`).
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
- **Measured for #1254 (2026-09-17)**, `shots.py take test` on KDE/X11 and offscreen: the launcher's
  main window stays unmapped; a scenario pick replaces the state behind a window-modal busy box; the
  close question's Cancel keeps the panel, No ends the session; closing the state window, `kill -9`
  or SIGTERM on the launcher leaves no process within 1 s; `kill -9` on the state and a
  non-executable binary leave the panel up with a message; the panel's 520x820 came back on the next
  start; rename, delete, rebind and Remove unused edited `test.json` as intended (restored after);
  Take all again replayed 24 pictures with the session up; `replay` 24 and `selftest` 89/89 after
  the change. A workspace database shared by launcher and state logged `database is locked` and
  delayed the fixture from 1.6 s to 12 s. After the launcher was rebuilt on `CShotDocState`,
  `CShotsJob` and `CShotFiles`: `shots.py selftest` 89/89 and 30/30 documentation mode cases;
  `replay` 24 pictures byte-identical to the run before; the six panel checks on KDE/X11 again
  (scenario pick, follow-up, follow-up dropped after a failed start, rename and delete of the running
  scenario, close with Publish). A state process started without a launcher stayed up until killed;
  with `leave()` it ends after 2 s. A `QMessageBox` given `NoButton` after `show()` crashed on Escape
  (SIGSEGV, exit 139); before `show()` it has no button and ignores Escape and close.
- **Measured for #1257 (2026-09-17)**, `shots.py take test` in a nested `Xephyr` with `kwin_x11` on a
  scratch copy of `doc/`: F9 on the Workspace docker stored `widget: dockWorkspace` and a 350x174
  picture; a dragged region stored `rect` [300, 172, 601, 401] and a 601x401 picture; Throw away stored
  nothing; F9 on the About box went through its exposure, on `CDetailsWpt` it showed the `SHOT_EXPOSE`
  line; a recording (expand Example, select Track) was parked, replayed by a `--doc-trial track` state
  and stored with `track.ini`, the parked files gone; F9 in it stored `scenario: track`; F9 on a
  `(base)` shot there was refused; changing the units appended the drift warning (`Units/type` 1 vs 0)
  and Save config cleared it; Save config on `(base)` stored 49 settings and left out 9 paths; that
  base, stored on a 1700 px screen at 1200x876, came up 1200x872 on a 1280x900 one; launcher `kill -9`
  left no state after 1 s. `selftest` 89/89 and 39/39, `replay` 24 pictures. A `QVariant` drift
  compare reported `dem2/keysKnownDems` as changed on an untouched scenario (`QStringList` vs
  `QString`); `perform()` without steps closed the About box F9 was pressed on.
- **Measured for #1246 (2026-09-14):** a 1200x800 main window and its canvas render byte-identically
  twice in one process and across a cold-cache and a warm-cache process; an unreachable tile server
  is refused with no picture written; a local server failing one zoom level: refused there, accepted
  at another zoom, refused again back at the first without a new request; nothing under the user's
  data directories changed.
- **The `.qm` catalogs are unreachable in a build tree.** `prepareTranslator()` resolves a filesystem
  path that is empty there, while all nine files are already embedded under `:/locale`.

## 10. What the demo does not do

1. **The demo's fixture is synthesized in memory** (`CShotFixture`): a `CQmsProject` with one
   track, waypoint, route and area, at a fixed epoch, on the online `osm.tms`. Every change to that
   data is C++: a ticket, a PR and a rebuild before the writer can carry on.

   The branch commits the data instead (#1249), each directory with a `SOURCE.md` naming source,
   licence and the commands that made it:

   | `doc/shots/fixtures/default/` | What |
   |---|---|
   | `projects/Example.qms` | project "Example": Track, Waypoint, Route, Area |
   | `database/Example.db` | SQLite: group "Projects", project "Einstein" |
   | `maps/bev_km50.vrt` | BEV KM50-R, 10 × 10 km around Example, offline, drawn over `osm.tms` |
   | `maps/osm.tms` | OpenStreetMap, online |
   | `dem/tirol_dgm5m.vrt` | Land Tirol 5 m, both projects, nodata 0 beyond the border |
   | `poi/tannheimer_tal.poi` | OpenStreetMap POIs, mapsforge version 2, both projects |
   | `routino/tannheimer_tal-*.mem` | Routino database from OpenStreetMap, the DEM's extent |

   **OSM stays online under the offline map**, so a first run needs a network for its tiles and a
   run with a filled cache needs none. A run opens a copy of the database, never the committed file:
   opening one migrates it, and a migration asks with a message box a headless run cannot answer.
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
   own offscreen plugin there is unconfirmed and belongs to whoever next builds on Windows. So is the
   platform theme: `CShotEntry::pinEnvironment()` leaves `QT_QPA_PLATFORMTHEME` alone on Windows,
   because a Windows demo run with `generic` exited 3221225477 (discussion #1209). That the exclusion
   is enough is unconfirmed until a Windows run.
9. **The colour scheme is pinned, light.** `--color-scheme light|dark` (`CUiTheme::pinColorScheme`)
   replaces the application palette and sets the style hint. Two levers because neither is enough
   alone: `QPlatformTheme::requestColorScheme()` has an empty default implementation, so the style
   hint does nothing on X11, and `QStyle::standardPalette()` asks the platform theme for the very
   scheme being pinned away. `dark` is one constant in `shots.py` away.
10. **An unreachable network changes the right dock column.** Measured 2026-09-15: the test page from
    the same tile cache, once with the network and once behind an unreachable proxy, differs in
    `database` and `main-window` only - the database dock 35 px shorter, the routing dock taller. The
    copied workspace database is not the cause, and BRouter's version check is not sent at startup
    while Routino is the selected router. Which widget reacts is not found.
11. **BRouter has no fixture.** Routino has a database of the DEM's extent; BRouter needs data of its
    own before its dock and dialogs are documentation subjects.

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
(#1248), the ones that need the fixture (#1249), recorded scenarios (#1252, #1253) and a
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
| 1 | **Hermetic run** (#1245) | `--shoot`/`--doc` switch parsing under `QMS_DOC_MODE`, no `--shoot-task` - `--color-scheme` gated with the rest, which the demo leaves open - the bundled font, `CUiTheme::pinColorScheme()`, `CQmsStyle::pinThemeIndependentHints()`, `attachParentConsole()` as a platform override, cache root and workspace database redirection. `--config`, `--locale` and `QLocale::setDefault()` are already on `dev` and are no part of it | the application starts headless against a scratch configuration and touches nothing under `~/.QMapShack` |
| 2 | **Render path** (#1246) | `CShotWriter` (settle, settleStable, a window resized to its hint, dpr 1, PNG), `CShotContext`; tile completeness through `IMap::pendingTiles()`/`failedTiles()` and its `IMapOnline` / `CMapDraw` / `CCanvas` aggregation | one widget renders to a file, twice, byte-identically; a streaming map is never photographed half drawn |
| 3 | **Shot file and `shootOne()`** (#1247) | the JSON schema of §2, `addressOf()`/`resolve()` symmetric, `set`, `size`, `rect`, `view` as a centre plus a zoom level (`CCanvas::getPosFocus()`/`getZoomIndex()`), the failure counting | a page of plain widget shots renders; a renamed widget fails loudly |
| 4 | **Exposure catalog** (#1248) | `CShotRegistry`, `SHOT_EXPOSE`, the class checked against `staticMetaObject` with `Q_OBJECT` added where it was missing; values a dialog keeps a reference to owned by its factory; `live<T>()`. No `private`→`protected` form change is needed - the demo has none | a throwaway shot file with one entry per exposure renders all of them |
| 5 | **Fixture** (#1249) | `CShotFixture` loading the committed data of §10.1: `Example.qms` into the workspace with `project()`/`trk()`/`wpt()`/`rte()`/`area()` resolved from it; both maps on, `bev_km50` over `osm`; the DEM on with hillshading; the POI file; the Routino database; a copy of `Example.db`. Paths reach the run through `--config`. The 16 exposures that need a fixture item; `CPrintDialog` is not exposed | the 16 exposures build; the database dock lists Example from the copy, its expanded folders belong to a scenario; no writer's data is read; a first run needs a network for the OSM tiles, a run with a filled cache none |
| 6 | **shots.py** (#1250) | `compose`, `replay`, `unused`; the pinned command line and environment; one process per scenario; `compose` injects the fixture paths, `Route/routino\paths` and a scratch copy of `Example.db`; the leak guard inside `replay` - list `~/.QMapShack` and the user's settings before and after, a changed file fails the run; the cached tiles' modification times refreshed before a run, because `CDiskCache` deletes tiles older than `cacheExpiration` every 20 s and an offline run then has holes | `shots.py replay` replays every committed shot, and reports a run that wrote outside the scratch tree |
| 7 | **Recorder: vocabulary** (#1251) | the input frame (`CShotApplication::notify()`), `IShotHandler` and the handler per class, the step table of §4, `key`, the map, the plot and the icon grid as raw input; `stop()` returns JSON, writing it into the shot file is #1257 | `shots.py selftest` - `CShotSelfTest`, committed with the subsystem - records real input and compares every step, and replays what it records back into the same state |
| 8 | **Recorder: the painted row buttons** (#1252) | the buttons of all three item delegates - workspace, maps, database - which are painted into the row and are no widgets: a `sigButtonPressed(index, button)` and a `buttonRect()` in `CWksItemDelegate`, `CMapItemDelegate` and `CDBItemDelegate`, and the handler that records them and replays them as input; the menu-owner `objectName` audit. The canvas, `IPlot` and `CIconGrid` are done in #1251 | a case in `CShotSelfTest` per delegate; a recorded scenario that expands a database folder and toggles a row's check state replays |
| 9 | **Replay** (#1253) | `CShotReplay`: the queue that calls `IShotHandler::replay()` per step, `clear()` before and after, the start state and the `tab`-last rule, the deadline, a scenario's steps read out of the shot file, one scenario per process | a page with a scenario reproduces byte-identically, three times in one process |
| 10 | **Launcher, panel, channel** (#1254); also the minimal state process: `--doc-scenario`, the fixture, the replay, `ready`, `leave()` | `shots.py take` and `shots.py publish`; the session and every file operation but writing the base configuration and a recording, `childArguments()`, the panel's buttons and statuses, `setBusy`, `mayClose()`, `endSession()`, the `QLocalServer` named `qms-doc-<pid>` | the panel comes up, starts and replaces a state process, and asks before closing over unpublished pictures; `publish` puts the retaken pictures into `doc/images/` and empties `_work/` |
| 11 | **State process and F9** (#1257) | on top of #1254's minimal `CShotDocMode`: the `(base)` arrangement captured and replayed, the verbs beyond `select`/`sync`, one scenario held up, writing a recording into the shot file only after a state started with `--doc-trial` replays it without a failure, parked in `_cache` until then together with the settings snapshotted when the recording started, which that state is composed from, `Ctrl+Shift+F9`, the keep/throw preview, the region picker, `portableGeometry()`/`namesAPlace()`, `settingsDrift()` | a writer records a scenario and tags a picture without touching a file |
| 12 | **Writer-facing labels** (#1255) | `CShotDocMode::onScreenName()`: a dock's caption, a tab's text, a group box's title, a window's title, else the class name; `qt_`-prefixed internals not offered; the address only where two entries collide, because `chooseLivePart()` finds the part by its label. The stored address does not change | the step list reads in the writer's words |
| 13 | **Drop the menu split** (#1256) | delete `CGisListWks::buildMenuItemTrk()`, which the replay queue made pointless, and cover a stack-local menu with a shot instead | a shot of the track context menu replays; nothing calls a `buildMenuXxx()` |

**Naming, to settle with Oliver before the branch closes.** The switch prefixes name the driver, not
what a switch does: `--shoot-scenario` filters which shots a build takes, `--doc-scenario` is the
state a process comes up in and what marks it as the state process. The `shots.py` verbs (`take`,
`replay`) do not match the switches (`--doc`, `--shoot`) either. Review every switch, verb and class
name together, once nothing more is added.

**Review focus, per area.** What is worth a reviewer's attention is not spread evenly:

- **1, 5** - anything that writes outside the scratch tree. Both escapes so far were found by a
  crash, not by reading.
- **3, 7, 8** - `addressOf()` and `resolve()` must stay symmetric, and every new address kind needs
  both halves in the same commit.
- **8** - the delegates' press handling is unchanged apart from one `emit` beside each action; a
  replay goes through that same path as input, so there is no second way to press a button to review.
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
- Pictures of online map *services* as documentation subjects. The fixture's OSM map under the
  offline one is online, which is a compromise on this rule, not an exception to it.
- Retranslating a live `CMainWindow`.
- Making ffmpeg a build dependency.
- Icons. Inline icon glyphs in the documentation are `.svgt` renders, not screenshots - a small
  exporter, a separate ticket.
