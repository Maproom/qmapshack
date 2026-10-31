# Documentation mode (QMS-1217)

Subsystem notes; the project rules are in the root `CLAUDE.md`. `doc/tools/shots.py` is part of it.

`.notes/QMS-1217-documentation-images.md` is the one plan: §1-§8 design, §9 evidence (measured, do
not re-derive), §10 what the demo does not do, §11 the sub-tickets.

**One branch, one commit per sub-ticket**: each sub-ticket is a commit on `QMS-1217` (based on `dev`),
never a branch of its own. A fix goes into the commit that introduced the code.

The file layout is plan §1.

The writer's guide is `doc/WRITING.md`: short, facts only, updated with every change a writer sees.

## Build and entry

- **Developer-only, behind `-DQMS_DOC_MODE=ON`.** No source tests the option: CMake compiles
  `shoot/CShotEntry.cpp`/`CShotOptions.cpp` or the `*Stub.cpp` pair, and `shoot/doc.qrc` (fonts and `Doc*` icons) only
  with it. A user's binary rejects `--shoot`, `--doc`, `--color-scheme` and companions through
  `QCommandLineParser`; `main.cpp`, `CCommandProcessor` and `CAppOpts` carry no `#if`.
  `src/qmaptool/setup/` never had the switches.
- **`CShotEntry` is all `main.cpp` knows**: `pinEnvironment()`, `createApplication()`, `isDocRun()`,
  `prepare()`, `run()`. `CShotOptions` defines and reads every switch; they reach the app as
  `CAppOpts::doc`, empty without the subsystem.
- **`prepare()` runs before `CMainWindow`** and sets the cache root, the workspace database
  (`CGisListWks::setDatabasePath()`), the colour scheme (`pinColorScheme()`),
  `CQmsStyle::pinThemeIndependentHints()` and registers the bundled DejaVu
  fonts (offscreen on Windows has no font database). It fails without `--config`, and `main()`
  exits 1.
- **A doc run skips the splash and `CSingleInstanceProxy`**, which would hand the arguments to a
  running QMapShack and exit.
- **`pinEnvironment()` sets `QT_QPA_PLATFORMTHEME=generic` except on Windows**, where a demo run
  with it crashed (access violation) and there is no desktop theme to keep out. macOS unmeasured.
- **A Windows build owns no console** (GUI subsystem). `pinEnvironment()`, first thing in `main()`,
  attaches to the parent's console for `--shoot`/`--doc` and reopens a stream on `CONOUT$` only if its
  handle is not already a pipe or file. Untested on Windows. Anything the writer must
  see is a dialog, never only a log line.

## Rendering (`CShotWriter`)

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
- **`processEvents(flags, ms)` does not wait**; wait with `CShotWait::wait()`/`waitFor()`, a `QEventLoop` quit by a
  `QTimer` without user input.
- **A main window picture depends on its resize history**: a window's minimum silently wins, and
  returning to a size can shift the right dock column by 1 px. Two runs on fresh configurations are
  byte-identical.
- **No `MainWindow/geometry` maximizes the window 500 ms after construction**; `CShotEntry::run()`
  waits 1000 ms before the first shot.
- **`-platform offscreen` takes no parameters**; resize the window instead of pinning a screen size.
- **A picture shows nothing private to the writer.** `CShotPrivacy`, held only around `QWidget::render()` in `renderAtDpr1()`, rewrites
  fixture, scratch, cache and home paths in standard widgets' text to `/home/user/...` and puts it back after;
  `pinEnvironment()` pins `USER`/`USERNAME` to `user`. Text a delegate paints or a model view holds is not covered.
- **Close the main window before leaving `exec()`**: destroying it while shown hides the docks after
  `CMainWindow::docks` is gone (SIGSEGV on exit).

## Shot files and exposures (`CShotPage`, `CShotRegistry`)

- **A window's size has one record, the shot's `size`**, applied to the main window or a window
  only; a shot the main window sizes must give one (exposures exempt). `layout` holds `saveState()`,
  the tab and every splitter's state, never `saveGeometry()`. `shootOne()` resizes before
  `restoreState()`, because dock extents are pixels.
- **A scenario shot must have a `size`: the main window's**, applied before the replay. A window a
  step opened is rendered at its own size hint.
- **One scenario per process, replayed once.** `CShotPage::run()` refuses a scenario shot `--only`
  matches unless `--shoot-scenario` is given; `shots.py` starts one process per scenario with its own
  `.ini`. Every shot of the scenario is taken inside that one replay (`shootScenario()`), a shot with
  `open` inside its menu's `exec()`; a replay on top of the scenario would be another state. A shot of
  another `size` than the first is skipped there and gets a process of its own: `shots.py` runs it
  alone with `--only <id>`.
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
  are updated by the draw, so a `hit` or click right after a wheel zoom finds nothing.
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

## Fixture (`doc/shots/fixtures/`, `CShotFixture`)

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
  (`compose` writes the page fixture's first `*.qms` or `*.gpx`), waits for
  `CGisListWks::isWorkspaceLoaded()` (the restore runs ~1100 ms after `CMainWindow` and does not
  check duplicates) and `IGisProject::isLoading()`, refuses a workspace already holding the project,
  and hands the first trk/wpt/rte/area to `CShotContext`.
- **`shots.py compose`** adds to the base the absolute `mapPath`,
  `demPaths`, `poiPaths`, `Route/routino\paths`, `Shoot/fixtureProject` and a `Database/Entries` on
  a scratch copy of the fixture's first `database/*.db`, plus `Database/saveOnExit=false`. A scenario `.ini` is a whole
  configuration, never a patch; settings are read in constructors, so one process per scenario.
- **A map or DEM activates from the configuration only with more than two keys**
  (`noShadowConfig()`): store `opacity`, `minScale`, `maxScale` with `isActive`.
- **A map's or DEM's configuration key is the MD5 of the file's first 4096 bytes** (`CMapItem`,
  `CDemItem`). A CRLF checkout of a `.vrt`/`.tms` (Windows `core.autocrlf`) changes the key, the
  configuration names nothing, and no map is active: the canvas' `labelHelp` then covers the map and
  takes its input. `.gitattributes` pins the fixtures' `.vrt`/`.tms`/`.wmts` to LF; a checkout made
  before that keeps its CRLF until the files are deleted and checked out again.
- **The DEM draws over maps and POIs**; the base uses opacity 20.
- **Tile files expire during a run** (`cacheExpiration`, 20 s timer) and come back as holes
  offline; refresh modification times on an old cache first.
- **Network reachability changes the right dock column** (database vs routing dock height), cause
  unknown.
- **No configuration sets checked POI categories** (`CPoiFilePOI::categoryActivated`) or the
  database tree's expanded folders; a scenario records them.

## `doc/tools/shots.py`

- **Starts every headless run**: `replay`, `selftest`, `unused`, hidden `compose`. Python 3.9,
  standard library only. One process per page and scenario group into `doc/images/_check` (or
  `-o`), pinned command line and environment (`QT_QPA_PLATFORMTHEME=generic`, `LANGUAGE`,
  `--locale`), scratch working directory, killed after 600 s.
- **A run's result** is its `shoot:` warnings on stderr, its exit code (failure count, capped at
  123; 124 is a step that never returned, `CShotReplay::kDeadlineExitCode`) and whether
  `<id>.png` came out in its render folder (`doc/shots/_cache/render-*`); only one that did replaces
  `<out>/<id>.png`. `-v` adds `-d`: `qDebug` lines, including one per input frame.
- **`selftest`** (`--shoot-selftest`, `CShotSelfTest`) takes no pictures: it drives the app through
  `CShotSynth`, compares the recorded steps, replays them and compares the state. Exit code = failed
  cases. A case goes into its group's `check…(CHarness&)`; a `case_t` field off its default is
  named (`.closesMenus = true`). A `shoot:` warning a case does not name in `warns` fails it; `expect`
  pins the state the input must give, since a replay that equals a recording that did nothing passes.
  `checkCoverage()` runs last: every handler replayed in some case, every main window menu entry
  named.
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

## Recorder (`CShotRecorder`, `CShotApplication`, `IShotHandler`, `CShotHandlers`)

- **A recording is its start state (`layout`, `view`) plus every change as a step**, always from the
  base; nothing is taken at `stop()`.
- **Steps are ordered by input frame**: `CShotApplication::notify()` numbers a frame around each
  spontaneous input, so a step inside a dialog a slot opened sorts after the input that opened it.
- **A step is kept only when the input went to the control that made it** (`inputReached()`); a
  signal fired while input went elsewhere is the application's answer, which the replay repeats.
- **One handler per class**, keyed by `QMetaObject`, nearest base wins. Connect once per object
  (`CShotRecorder::watched`): `Qt::UniqueConnection` refuses a lambda. A handler names the verbs it
  replays (`verbs()`); `CShotHandlers::replay()` refuses any other, which a handler would otherwise
  perform as its own.
- **Record the intent, replay the input, check the outcome**: a combo is `set currentIndex` with the
  entry's `text`, replayed by the text with `activated`; typing is `key` with the final text; a moved
  dock is `arrange` with `saveState()` (`dockWidgetArea()` lies while floating).
- **A surface is recorded in its own units, never a position**: canvas (geographic point plus item
  hit), `IPlot` (x value), `CIconGrid` (icon name), row buttons (delegate name). Press to release is
  one `click`, `drag` or `dclick`; something happening mid-gesture splits it into `press`, `move`, …,
  `release`. Not covered: `CDateTimeEditor`, `CPhotoViewer`, `CRouterBRouterTilesSelectArea`.
- **`IPlot` sets no `objectName`**: a code-built plot is addressed positionally
  (`framePlot/CPlotProfile#0`).
- **A key press no handler took is `keypress`**, held until the next frame: a matched `Shortcut`
  arrives after its key press. Editing ended by focus loss is `endedit`.
- **A left or middle click no handler took is `mouseclick`** (`mousedclick` for a double click) at a
  fraction of the widget the press went to (`onMouseFrame()`): held from the press to the release and
  dropped if any step was made meanwhile, so a viewport or a surface's child stays its owner's. Named
  only then: a completer's viewport cannot be named and needs no name. Moved past the drag distance it
  is no step and warns. The right button is the context menu request, a `menu` step once one answers.
- **An item view's `select` and `dclick` carry the modifiers**: Ctrl and Shift extend a selection.
- **A menu entry is addressed by its action's `objectName`, never its text**, a menu by its own
  `objectName`. Every new entry needs one: without it the recorder warns and drops the step.
  Data-built menus use a prefix plus an untranslated key (`actionWptIcon_<sym>`); a POI entry uses
  the file's base name plus 8 characters of `CPoiFileItem::key`.
- **A menu shown with `exec()` must be deleted when it returns**, or its leftovers shift addresses
  to `QMenu#n`.
- **A pick in an open menu replays as a click on the entry, not `trigger()`**: callers read
  `exec()`'s return, and a disabled action ignores `trigger()`. `QMenu` ignores a press unless the
  pointer moved, so `CShotSynth::arrive()` moves 8 times.
- **A trigger whose slot runs `exec()` is recorded only when that dialog closes**; a recording
  stopped with it open lacks the step.
- **A completer popup has no parent**: its input counts for the line edit.
- **A mouse context menu goes to `qt_last_mouse_receiver`**, and Qt drops a move to the current
  position, so replay moves the pointer there first. The Menu key's context menu comes from the
  platform plugin, none offscreen.
- **The wheel over a list goes to its viewport**: an item view's scroll is `scroll` (top row).
- **Replay and `CShotSelfTest` use `CShotSynth`, never QTest's `QWidget` functions**, which bypass
  window routing (grabs, popups, double-click synthesis, focus). Replay refuses a point another
  widget would get (`CShotSynth::missed()`).
- **`QTest::wheelEvent()` hands logical positions on as native ones**, so on a scaled screen the
  wheel lands elsewhere; `CShotSynth::wheel()` scales them itself. `shots.py` drops every scaling
  variable, so a scaled check sets one by hand.
- **Qt delivers a double click's second press only as `MouseButtonDblClick`**: an item view emits
  `clicked` then `doubleClicked`; the handler folds them into one `dclick`.
- **Row buttons** are a `click` with `row` and `button` from `sigButtonPressed`. A button is named by
  its `Q_ENUM` enumerator without the `e`, so renaming an enumerator renames it in every recording.
  Database, map and DEM rows are named by what they show (`CShotAddress::namePathOf()`); workspace
  device and geo search rows have no name, and their buttons are dropped with a warning.
- **Not recorded**: a window closed by its title bar, re-docking through a window frame, a main
  window separator drag, typing into a cell editor, touch, drag and drop (a `Drop` warns).
- **A replay starts from nothing**: `CShotReplay::replay()` calls `clear()` first, because steps are
  not idempotent. `clear()` must `sendPostedEvents(nullptr, QEvent::DeferredDelete)`:
  `CMouseRangeTrk`'s destructor resets the track, and `processEvents()` does not deliver
  `DeferredDelete`.
- **Steps start only once the start state's map is drawn** (`settleStable()`): during a redraw
  `CDemDraw::getElevationAt()` answers `NOFLOAT`.
- **The picture is the replay's last step**: a dialog a step opened is still inside its `exec()`.
  The popups and modal dialogs the steps opened are closed after it, or the loops below cannot
  return; what was up before `perform()` stays (`closeAbove()`), so a shot's `open` menu closes and
  the scenario's dialog does not.
- **The next step may run inside a step only when that step waits in an `exec()`**: the loop level
  is above the step's start and a popup or modal is up that was not before. A popup alone is not
  enough: `CShotSynth::mouse()` on a menu bar delivers queued steps at the same loop level.
- **The deadline ends the process** (`std::_Exit(kDeadlineExitCode)`): a step that never returns
  keeps every loop below it from returning. It is restarted as each step starts and before the
  picture, and `keepAlive()` restarts every running one per shot; the report names the innermost
  step.
- **`settle()` ends every finite running animation**; animations run on wall time.

## Launcher, panel, channel (`CShotDocLauncher`, `CShotDocPanel`, `CShotDocState`, `CShotsJob`, `CShotFiles`, `CShotDocMode`)

**No flags, only objects that die with what they describe.** A flag that outlives the start or run
it belongs to is the bug this subsystem invites:

- **`CShotDocState`** is one state process: `QProcess`, channel socket and the one follow-up command
  sent after its first `ready`. Held in a `QPointer`, replaced with `stop()` + `deleteLater()`; every
  outcome is emitted queued and dropped after `stop()`.
- **`CShotsJob`** is one `shots.py` run: `finished(bool, error)` exactly once, always queued, also
  for a failed start, then it deletes itself. `error` is the program's standard error, where
  `shots.py` writes why a replay failed; the launcher shows it in a dialog. Standard output is
  forwarded to the console.
- **`CShotFiles`** is one page on disk and owns every file rule: names, rename with rollback (the
  scenario's `.ini` moves too), pictures deleted only after the shot file is committed
  (`QSaveFile`). The launcher only asks the question.
- **Two processes.** `shots.py take <page>` starts the launcher (`--doc <repo> --doc-page <page>
  --doc-python`), which owns the panel and the files and never renders. Each pick starts a new state
  process with `--doc-scenario <name|->`; nothing is reset in place. The two need separate workspace
  databases: a shared SQLite file locks.
- **Channel** `QLocalServer` `qms-doc-<pid>`, one line per message, every report handled queued.
  Launcher → state: `region`, `update`, `record <name>`, `stop`, `select`.
  State → launcher: `status`, `in <scenario|->`, `ready`, `tagged`, `recording`, `recorded`,
  `recorded-none`, `trial-failed`, `setup-failed` (the fixture did not load; the launcher stops that state).
  `in` comes before each `ready`: a scenario the state could not
  enter leaves it in the base, and the launcher's `CShotDocState::scenario()` follows it. Every `recording` ends in `recorded` or `recorded-none`. Before its `ready` a state
  answers every verb but `select` with a `status` line. Without a listening channel no state is
  started.
- **Lifetime:** state killed → the panel stays and says so; state window closed → the session ends
  once `mayClose()` agrees, which asks about the page's unpublished pictures as closing the panel does;
  launcher killed → the state quits on `disconnected`. SIGTERM only closes the main window
  (`CAppSetupLinux::closeOnSIGTERM`), so the launcher and `CShotDocMode` filter `QEvent::Close` on
  it. The state ends through `CShotDocMode::leave()`, which falls back to `std::_Exit(0)` after 2 s.
- **A `QProcess` that fails to start emits only `errorOccurred`**, and a failed connect emits no
  `disconnected`.
- **The launcher's main window is never shown** (`CShotEntry::showsMainWindow()`, plus
  `WA_DontShowOnScreen` against `CMainWindow`'s own delayed `showMaximized`).
- **Every `shots.py` run uses `--doc-python`**, never `python3` from `PATH` (a Store alias on
  Windows). Retake and All replay into `_work`, written through `QSaveFile`; publish moves from there
  with `os.replace`.
- **Revert puts the shot's entry back too**: the entry from before the first take is kept as
  `_work/<id>.shot.json` (`{}` for a new shot). A revert entry that does not parse reverts nothing.
- **A shot file with `shots` not a list or `scenarios` not an object is refused** like one that is
  no JSON: read as empty, the next write would drop it.
- **Deleting a scenario or rebinding a shot reduces the shot to its `id` and deletes its pictures.**
- **A shot id or scenario name that leaves its directory has no path** (`CShotFiles::staysInside()`);
  `shots.py`'s `stays_inside()` is the same rule — change both.
- **Scenario names are file names and arguments**: no `/ \ : * ? " < > |`, no leading `-`, no spaces
  or dots at the ends, not `-` or `(base)`, no Windows device name, none differing from another only
  in case.
- **A page is a file directly in `doc/pages`** and lists and rebinds only its own ids
  (`<page>/<name>`).
- **Picture references need `QRegularExpression::UseUnicodePropertiesOption`**: without it Qt's `\w`
  is ASCII.
- **A `QMessageBox` meant to have no button gets `setStandardButtons(NoButton)` before its first
  `show()`**: otherwise it adds an OK button as its escape button, and removing it later crashes on
  Escape.
- **A nested `QEventLoop` does not deliver a `DeferredDelete` posted outside it**; a test checking a
  `deleteLater()` sends it with `sendPostedEvents(nullptr, QEvent::DeferredDelete)`.
- **Placement** of panel and window is kept in `doc/shots/_cache/doc-panel.ini` and moved fully onto
  a screen by `CShotFiles::onScreen()`.
- **Tests:** `shots.py selftest` runs `CShotDocSelfTest` too (page rules on a temp checkout, job and
  state handles against real processes); `python3 -m unittest doc/tools/test_shots.py` covers
  `publish`, `unused --delete` and page names.
- **Driving the panel with xdotool on KDE**: `windowactivate` does not raise it; check the window
  under the pointer before every click.

## The state process (`CShotDocMode`)

- **Every writer works at one window size** (`kWindowSize`, 1200x800, `setFixedSize()` after the
  geometry): recording, trial and pictures happen at it, so a scenario's map positions land on every
  machine. `windowProblem()` refuses recording and pictures when the screen has no room for it or the
  window is not at it, and says so in the writer's words.
- **A replay failure names the action and why, in the writer's words**: `fail(why, step)` writes
  "Action N of M in the recording, '<step described>', did not replay: <why>." No class names.
- **The state re-applies `MainWindow/geometry` after the fixture is up**, and
  `portableGeometry()` zeroes the screen and its width in stored geometry so `restoreGeometry()` does
  not drop it on another screen size (>25 % difference). `placeWindow()` moves it to
  where the writer left the last state's window. `CShotDocSelfTest` fails when a Qt writes another geometry version.
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
- **F9 in a scenario takes only a window its actions leave open** (`scenarioWindows`, noted after
  the setup and after a trial): the scenario's replay closes the modal dialogs it opened, so one on
  screen was opened by hand and a build could not take it.
- **F9 refuses a picture whose shot names another scenario** than the running one; its row starts
  that scenario.
- **Stop is refused while a popup or modal dialog is up** (`CShotReplay::blocker()`): what opened it
  is recorded only once it closes, so the recording would lack it.
- **A recording is stored only once it replays.** `record <name>` snapshots the settings into
  `_cache/<page>-trial.ini` before the recorder runs, `stop` parks the steps beside it in
  `<page>-trial.json`; the launcher starts a state composed `--from` that snapshot with
  `--doc-trial <name>`, which replays them, stores them with `CShotFiles::storeScenario()` and holds
  the scenario - or reports `trial-failed` with the first `shoot:` warning, stores nothing and the
  launcher returns to the base.
- **Save config writes only `(base)`**, through `CMainWindow::saveConfig()` (what `~CMainWindow`
  writes): a scenario keeps its recorded configuration. `compose --from <ini>` names the source file.
- **Take again is headless**: the launcher runs `shots.py -o doc/images/_work replay --only <id>`,
  what a build does; a failure keeps the work picture.
- **A picture is written from what F9 rendered only for a live part.** An exposed dialog parented to
  the main window is in that render too, and its shot says `exposure`, which the build renders from.
- **A picture just taken waits in `doc/shots/_cache/pending/` until Keep** (`keepShot()` points the
  writer there): a state that ends with the question up leaves `_work` as it was. Keep moves it into
  `_work`, then stores the entry; a failed store puts the replaced work picture back.
- **Publishing on close is the page's alone** (`page + "/*"`), as Publ. All is; another page's work
  pictures wait for that page's session.
- **Settings drift is what changed since the state was set up** (`snapshotSetup()` after
  `slotSetUp()`, a trial and base Save config), compared as INI text: `QSettings` reads a one-entry
  list back as `QString` from another process' file and as `QStringList` from its own write.
- **`obey()` refuses launcher commands with a `status` while F9 or a region is in progress** (`busy`).
