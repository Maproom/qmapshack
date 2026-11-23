# Documentation mode — design

Every image in the user documentation is taken from QMapShack itself and can be taken again by one
command. Working rules and details per class: `CLAUDE.md` in this directory.

## Files

A **page** is one Markdown file, one shot file and one directory of images:

```
doc/en/pages/<page>.md              the page; references ../images/<page>/<name>.png
doc/en/shots/<page>.json            how each image is taken, and the page's scenarios
doc/en/shots/<page>.ini             the page's base configuration
doc/en/shots/<page>/<scenario>.ini  a scenario's whole configuration
doc/shots/fixtures/default/         the example data; fixtures/<page>/ holds what a page's differs in
doc/en/images/<page>/<name>.png     the published images
doc/en/images/_work/                images taken again and their <id>.shot.json, not published (git-ignored)
doc/en/images/_check/               what shots.py replay renders (git-ignored)
doc/shots/_cache/                   tile caches, workspace databases, images waiting for Keep, parked
                                    recordings, doc-panel.ini, <page>-marks.json, render-* run directories
                                    (git-ignored)
```

- **The page says which images exist**; the shot file says how each is taken.
- **The images are repository content.** A checkout shows its documentation with no command run.
- **A page's fixture holds only what differs.** A part it has (`projects/`, `maps/`, `dem/`,
  `poi/`, `routino/`, `database/`, `device/`) replaces the default's whole; a part it lacks, or one holding only
  `SOURCE.md`, `README.md` or dotfiles, is the default's (`fixture_part()`, `CShotFiles::ownFixtureParts()`).
  A part holding a file `EMPTY` is left out, as on a first start.

## Base and scenarios

- **`(base)` is the page's start state**, `doc/en/shots/<page>.ini`. An image taken in it has no
  `scenario` key; `--shoot-scenario -` selects those images, and `-` is no valid scenario name.
- **A scenario is a recording on top of the base**: its steps, and the whole configuration on
  screen when the recording started. Never a difference to the base, so a later change of the base
  does not change an image already taken in a scenario.
- **A recording is stored only after a state started with `--doc-trial` replays it.** A scenario is
  changed by recording it again.
- **A page's base is copied once**, from the default fixture's `shots.ini` or another page's base.
  The panel's Base menu writes it ("Save what is on screen as this page's base...") or replaces it
  ("Copy the base of another page...").

## Shot file

`doc/en/shots/<page>.json`, written by `CShotFiles::writeShotFile()` and by `shots.py`:

```json
{
  "shots": [
    { "id": "test/test-menu-project", "widget": "menuProject" },
    { "id": "test/test-workspace-filter", "set": { "lineFilter.text": "track" },
      "size": [1200, 800], "widget": "dockWorkspace" }
  ],
  "scenarios": {
    "track-screen-option": [ { "do": "layout", "…": "…" }, { "do": "select", "…": "…" } ]
  }
}
```

| Key | Means |
|---|---|
| `id` | `<page>/<name>`, the image's file stem and what the page references |
| `widget` | address of the part to take; empty is the window on top, else the main window |
| `window` | the class a scenario leaves on top; a scenario that no longer opens it fails |
| `exposure` | build a fresh instance from the exposure catalog instead |
| `scenario` | the recording the image is taken in; absent is `(base)` |
| `set` | properties to drive before the image, `child.property` or a property of the shot's widget |
| `size` | the window's size, whatever part of it is taken; in a scenario the main window's, and a window the steps open renders at its own size |
| `rect` | the part of the result that is kept |
| `open` | the step that opens a context menu; it runs last and the image is taken inside the menu |
| `active` | the menu entry highlighted in that menu |
| `note` | free text |
| `mark` | marks drawn on the image; see below |

- **A window-sized image has an explicit `size`**, applied before the scenario runs. A shot sized by
  the main window without `size` fails.
- **`rect` is cut out of whatever produced the image**: a widget, an exposure or a scenario.
- **A shot moved to another scenario keeps only `id` and `scenario`**, losing its scenario keeps only
  `id`; either way its images are deleted.
- **A mark** (`CShotMarks`) is placed after the render from the live widgets and drawn in one red, each with a
  thin light outline; one not completely inside the image or `rect`, or in another window, fails the shot.

  | Key | |
  |---|---|
  | `root` | `main`, `menu` (the open menu), `window` (the window a scenario leaves open, or the exposure), `dock:<address>` (a floating docker) |
  | `widget` | address below `root`; a plain name must be unique there; a mark is always an object |
  | `part` | empty for the whole widget, `handle` (a slider's knob), `branch` (a tree row's arrow, with `row`), `button` (a button a list's delegate paints into a row, with `row` and `button`), `tab` (the tab of a docked docker or of a tab widget's page), `title` (a docker's title bar), `action` (a menu entry or toolbar action, with `action`, its objectName), `splitter` (with `between`), `point` (a place on the map, with `lat` and `lon`) |
  | `row` | a tree row as `CShotAddress::rowPathOf()`: row numbers from the top level down and the column, `0/2:0` |
  | `button` | a row's button as a `click` step names it: `visible`, `save`, `checkState`, `activate`, … |
  | `between` | the two addresses a separator divides; a docker stands for its tab group |
  | `lat`, `lon` | a `point`'s place in degrees; a 1 px target, so a cursor's hotspot or a disc's centre sits on it; not on the map now is *not on screen* |
  | `shape` | `frame`, `disc`, `arrow` (with `tail`), `cursor` (with `cursor`); empty is a disc on a knob, an arrow, a separator or a point, else a frame; a disc fails on a target that fits neither inside it nor is at most 16 px thick (`CShotMarks::discFits()`) |
  | `tail` | `[dx, dy]`, an arrow's start from its target's centre in image pixels, a multiple of 45° |
  | `cursor` | `pointer`, `hand`, `resize`; resize only on a separator |
  | `name` | what the panel and failures call it |

- **Marks a shot file cannot hold** are the launcher's, in `_cache/<page>-marks.json`: those placed on an image not
  taken yet (`pending`, moved into its entry by the first take) and the ids whose marks changed since their image
  (`changed`). A mark change on a taken image deletes its `_work` image.
- **A view step is the canvas' centre and zoom level** (`getPosFocus()`, `getZoomIndex()`), never a
  rectangle.

## Architecture

```
src/qmapshack/shoot/
  CShotEntry, CShotOptions    the command line switches and the start of a --shoot or --doc run
  CShotRegistry               the exposure catalog; SHOT_EXPOSE
  CShotContext                the live application and the fixture by role; begin() and shot()
  CShotWriter                 settle, resize, render at dpr 1, PNG
  CShotPage                   a shot file's shots: shootOne(), run(); the layout and view steps
  CShotAddress                widget addresses, item paths, row paths, each with its resolver
  CShotStep, CShotInput       the step vocabulary; what typing leaves in an input
  CShotApplication            the QApplication of a run: one frame around each user input
  CShotRecorder               collects the steps the handlers make, in input order
  IShotHandler, CShotHandlers one handler per recorded class: its signals, its step, its replay
  CShotReplay                 performs a scenario's steps from the event loop
  CShotSynth                  input through QWindowSystemInterface, as a user's own
  CShotFixture                loads the example data
  CShotWait                   waiting with the event loop running and user input held back
  CShotWarnings               collects the warnings given during its lifetime, on any thread
  CShotPrivacy                keeps the writer's private data out of an image
  CShotFiles                  a page's files: shot file edits, scenario configurations, image paths
  CShotDocLauncher            the writer's session and the file operations the state does not do
  CShotDocPanel               the writer's panel
  CShotDocState, CShotsJob    the launcher's handle on a state process; one shots.py run
  CShotDocMode                the state process: Ctrl+Shift+F9, regions, recording, the channel
  CShotSelfTest, CShotDocSelfTest   the cases shots.py selftest runs
```

- **`QMS_DOC_MODE=OFF` builds stubs** (`CShotEntryStub.cpp`, `CShotOptionsStub.cpp`): `main.cpp` calls
  `CShotEntry` either way, the stub answers "no documentation run", and its parser knows no
  `--shoot`/`--doc` switches, so a user's binary rejects them as unknown. No `#ifdef QMS_DOC_MODE`
  in the sources.
- **One configuration, one fixture.** The headless run (`--shoot`) and documentation mode (`--doc`)
  compose the same configuration and load the same fixture. Both render through `CShotWriter`; a
  live part taken with Ctrl+Shift+F9 is written from `CShotWriter::renderAll()` at the key press,
  everything else through `CShotPage::shootOne()`.
- **The exposure catalog is the only C++ that grows**: `CShotExposures.cpp`, one `SHOT_EXPOSE` line
  per dialog class, checked against `TYPE::staticMetaObject`, so an exposed class declares
  `Q_OBJECT`. A value a dialog keeps by reference is a static of its factory.
  Not exposed: `CDetails*` (reached by a scenario), `CExportDatabase`, `CSearchDatabase`,
  `CRangeToolSetup`, `CTemplateWidget`, `CPrintDialog`, `CRouterBRouterSetupWizard`.
- **A rename is caught by `shots.py replay` failing**, not by the compiler: addresses are strings.

## Two processes

```
qmapshack --doc <repo> --doc-page <page> …          the launcher: the panel only
  └── qmapshack --doc … --doc-scenario <name|-> …     the state: one scenario, thrown away
```

- **The state writes what it takes or records**: images, shot entries, the base, recordings and
  `doc-panel.ini`. The launcher owns the session and does every other file operation. It never renders; an image is taken by the state or by a `shots.py` child
  (Retake, All). Its main window is constructed and never shown: `CMainWindow` initialises the
  singletons the panel uses.
- **The state process is disposable.** It comes up in one scenario and stays while the writer works
  in it; another scenario is another process. No scenario is undone in place.
- **They talk over a `QLocalSocket`** (`--doc-channel`). Launcher to state: `select`, `region`,
  `update`, `record`, `stop`, `tag`, `mark`, `marker`, `marks`, `marks-shown`, `mark-confirm`, `mark-cancel`,
  `drift`. State to launcher: `status`, `ready`, `in`, `tagged <id>[\t<row whose marks it carries>]`, `recording`,
  `recorded`, `recorded-none`, `trial-failed`, `setup-failed`, `mark-placed <id>\t<mark>`, `mark-status`, `marking`,
  and `drift <scenario>\t<settings>`, or
  `drift-unknown <why>` while the state cannot tell. `drift` is always answered: the launcher asks before Retake and
  All, and offers to save the base when settings differ.
- **Ctrl+Shift+F9 and F10 pressed in the panel** reach the launcher; it passes them on as `tag` and `mark`, and Return
  and Escape as `mark-confirm` and `mark-cancel` while the state chooses a mark's target.
- **Closing either window ends the session.** The state quits when its window closes, its channel
  drops or it cannot connect (`CShotDocMode::leave()`). The launcher ends when the state exits with 0
  or the panel is closed, both through `mayClose()`, which asks first over images whose marks changed and over
  unpublished images. A
  state that dies otherwise leaves the panel up.

## Recording

- **A recording is its start state and every change after it, as steps.** `CShotRecorder::start()`
  takes `layout` and `view`; `stop()` takes nothing.
- **A step is kept only when the input being delivered went to the control that made it**; the rest
  is the application's answer, which replaying the input repeats. `CShotApplication::notify()`
  numbers a frame around each input and steps are ordered by it.
- **One handler per class**, keyed by `QMetaObject`, nearest class above wins.
- **A menu entry is addressed by its action's `objectName`**, never its text. A menu built from data
  uses a stable prefix plus an untranslated key (`actionWptIcon_<sym>`, …).
- **Input nothing covers is reported while recording**: drag and drop, dragging on a widget without
  a handler, touch input and touchpad gestures, a menu or action without an `objectName`, a menu more
  than one widget opens, a row without a name, typing into a cell editor.
- **A close opens no frame**, so a window closed by its title bar leaves no step: Qt 6 closes through
  the platform, and the application's close and the title bar's are the same event.

## Replay

- **`CShotReplay::perform()` queues each step from the event loop** and performs it through the
  handler of the step's target. A step that waits in an `exec()` gets the following steps delivered
  inside it.
- **The image is the replay's last step**; popups and modal dialogs are closed after it.
- **The start state first**: the leading `layout` and `view`, then `settleStable()` before the first
  step.
- **One scenario per process.** A process replays a scenario once for all its shots of one size;
  another size is another process.
- **A step or image not done within `kDeadlineMs` (60 s) ends the process with exit code 124**,
  naming the step if one was running.

## shots.py

`doc/tools/shots.py`, Python 3.9, standard library only. Options before the command: `--binary`
(the QMapShack to run), `-o/--out` (where images go), `-v`.

| Command | Does |
|---|---|
| `take PAGE` | open the launcher on a page |
| `replay [--only GLOB]` | replay every shot into `doc/en/images/_check/`, one process per scenario; report what no longer comes out |
| `publish [--only GLOB] [--report FILE]` | move `doc/en/images/_work/`'s images into `doc/en/images/` and delete their `<id>.shot.json`; one without an image stays |
| `unused [--delete]` | shots and images no page references |
| `selftest [--page PAGE]` | the recorder's and documentation mode's own cases |
| `compose` | the one composer of a run's configuration; used by the launcher, hidden from `--help` |

- **A run's configuration is a scratch copy** of the scenario's file, else the base. The fixture paths
  (`Canvas/mapPath`, `Canvas/demPaths`, `Canvas/poiPaths`, `Route/routino\paths`,
  `Shoot/fixtureProject`) and the path of a scratch copy of the fixture's database are injected per
  run and never stored; the database entry's name and type and `Database/saveOnExit=false` are
  stored with a base.
- **The run is pinned**: `-style Fusion`, DejaVu Sans 10, `--color-scheme light`, `--locale en` with
  `LC_ALL`/`LANG`/`LANGUAGE`, `TZ=UTC`, Qt's scaling variables unset. `QT_QPA_PLATFORMTHEME=generic`
  is set by `CShotEntry::pinEnvironment()`, not on Windows.
- **`replay` refuses while another QMapShack runs**: its leak guard compares the user's cache and
  data directories before and after, and tests the running instance's lock file without creating it.
- **A run reports through `shoot:` warnings** on stderr and the exit code.

## Rules

- **An exposure never calls `exec()`**: construct, polish, size, render.
- **Drive inputs, never outputs**: set a control and let the application's signals produce the state.
  A shot's `set` reads the property back (`CShotPage::driveProperty()`); a handler checks its own
  replay.
- **Never reload the configuration in a running application**; one process per configuration.
- **The run stays inside `--config`**: `CMapDraw::setCacheRoot()` and `CGisListWks::setDatabasePath()`
  are set before `CMainWindow` is constructed.
- **Images are `<id>.png`.** Names per language are not implemented.

## Not supported

- A message box from the static `QMessageBox` functions has no exposure: the function builds and runs
  it inside the call.
- `CProgressDialog` is exposed only as a fresh instance (`ProgressDialog`), not in the state of a
  running operation.
- WebEngine content (`CWebPage`, `QWebEngineView`) is not shown in a render (not verified).
- Other languages than English (`--locale en`), a dark image set (`--color-scheme light`), animations
  (`CShotWriter` writes one PNG per image).
- `QMS_DOC_MODE` is OFF by default; a Windows run uses a developer's build tree.
- Images of online map services as documentation subjects. The fixture's maps include the online
  `osm.tms` beside the offline `bev_km50`.
