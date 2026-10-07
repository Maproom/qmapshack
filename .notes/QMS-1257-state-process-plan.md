# QMS-1257 — the state process and F9

Ticket: Maproom/qmapshack#1257 (body updated 2026-09-17). Branch `QMS-1217`, uncommitted; commit as
`[QMS-1257]` once the open points below are done. #1255 (labels) and #1256 (menu split) follow.

## Built and verified (2026-09-17)

`CShotDocMode` ported from the demo onto `CShotFiles`/`CShotPage`/`CShotReplay`: F9 with part and id
questions, keep/throw preview (a thrown-away retake restores the previous work picture), unexposed
class shows the `SHOT_EXPOSE` line, region picker, recording, parking and trial, base Save config,
`portableGeometry()`, drift warning, `placeWindow()`, geometry re-applied after the fixture.
Also: `--doc-trial`, `CShotContext::liveScenario()`, `CShotRegistry::exposureOf()`,
`CShotFiles::{parkRecording,parkedRecording,dropParked,storeScenario,storeShot}`, launcher
`tryRecording()` and `trial-failed`, `CShotReplay::perform()` closes windows only after steps.

Verified: `shots.py selftest` 89/89 + 39/39 (9 new doc cases), `replay` 24 pictures, and the live
checks in plan §9 "Measured for #1257". Not tested: a failing trial (`trial-failed`), Take again,
a `QMS_DOC_MODE=OFF` build.

## Implemented 2026-09-18, not yet driven by hand

All six decided changes are in the tree. `selftest` 89/89 + 39/39 and `replay` 24 pictures pass with
them; nothing below was exercised in a live session.

1. **Save config only for `(base)`.** A configuration stored after a scenario ran holds its steps'
   results, which a replay would run again on top. `CShotDocMode::updateScenario()` refuses a
   scenario and the launcher's `storeConfig()` says so without sending `update`. What the destructor
   wrote is `CMainWindow::saveConfig()` now; `~CMainWindow` calls it and then collects
   `allOtherTabs`/`allViews` in a loop of its own. `storeSettings()` calls it and overwrites
   `MainWindow/geometry` with `portableGeometry()`. Side effect: `toolBarConfig->saveSettings()` and
   `geoSearchConfig->save()` run before the tab deletions instead of after; `ToolBar/*` and
   `Search/*` do not meet `Canvas/Profile/<canvas>/geometry`, which is all a deleted view writes.
2. **A scenario's `.ini` is the configuration that stood when its recording started.**
   `startRecording()` writes `storeSettings(files.trialConfig(), ...)` before `recorder->start()`;
   `parkRecording()` only checks the file is still there. `recorded-none` and `discard` drop it,
   `discard` only while nothing records. `tryRecording()` composes `--from` it, which `compose` and
   `composeConfig()` learned.
3. **Drift is measured against a snapshot taken when the state is set up.** `snapshotSetup()` writes
   into the `setUpWith` temporary directory at the end of `slotSetUp()`, after `runTrial()` succeeds
   and after base Save config; `settingsDrift()` takes no argument any more.
4. **`record` only starts, `stop` only stops** - `toggleRecording()` split in two.
5. **Take again is `shots.py -o doc/images/_work replay --only <id>`** from the launcher. The
   `retake` verb and `CShotDocMode::retakeShot()` are gone. Note: replay deletes `<out>/<id>.png`
   before running, so a failed retake loses the previous work picture.
6. **`namesAPlace()` drops every absolute path**, no prefix match, no path argument.

## Open, from reading the code

- **A scenario's F9 on a dialog is not checked against the scenario's steps.** The shot stores only
  `window: <class>`; `CShotReplay::perform()` closes blocking windows after the steps, so a dialog on
  screen in a scenario state was opened by hand and a class that its steps never open would surface
  at the next replay. Inferred, not tested.
- **A quoted list entry is not seen as a path.** `namesAPlace()` matches `QDir::isAbsolutePath`,
  which a value like `"/home/..."` fails. No value in `doc/shots/fixture/shots.ini` is quoted;
  whether `QSettings` can hand one back that way is unverified.

## Proposed, awaiting a yes

- `.gitattributes`: `doc/shots/fixture/** -text`. Map/DEM/POI settings are keyed by the MD5 of the
  file's first 4096 bytes (`CMapItem.cpp:61-63`, `CDemItem.cpp:57-63`, `CPoiDraw.cpp:200-209`);
  `.vrt`/`.tms` have no eol attribute, so a CRLF checkout would change the keys (unverified, no
  Windows here).

## Still to do

Left: the live checks of the setup below - a failing trial, Take again, Save config in a scenario,
a double Stop - and a `QMS_DOC_MODE=OFF` build. Then commit as `[QMS-1257]`.

## Live test setup

Never on the writer's desktop:

```sh
Xephyr :5 -screen 1700x1050 -ac -br -noreset &     # nested display
DISPLAY=:5 kwin_x11 &                              # without --replace
cp -a doc <scratch>/repo/                          # scratch copy, the checkout stays clean
cd <scratch>/repo && DISPLAY=:5 python3 doc/tools/shots.py \
    --binary ~/projects/qmapshack/build/bin/qmapshack -v take test > session.log 2>&1
```

Drive with `DISPLAY=:5 xdotool`; capture a window with `import -display :5 -window <id>` (`xwd -root`
misses dialogs under kwin in Xephyr). Kill only the launcher by pid (`ps` args without
`--doc-scenario`; `shots.py` also has the binary path in its arguments). Close the Xephyr windows
afterwards.
