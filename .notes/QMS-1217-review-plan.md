# QMS-1217 — what is left

Findings of the review on the branch rebased onto `dev`. Done items are covered by `shots.py selftest`
or `test_shots.py`.

## Done

- Recording: a left or middle click no handler took is `mouseclick`/`mousedclick`; a drag on such a
  widget warns; a `Drop` warns. Item view `select`/`dclick` carry modifiers.
- Handlers name their verbs (`IShotHandler::verbs()`); any other verb fails.
- Replay: one replay per scenario for all its shots; `perform()` closes only what its steps opened;
  per-step deadline with exit code 124, failures capped at 123, the innermost step named.
- Launcher: an F9 picture waits in `_cache/pending/` until Keep; closing publishes the page's own
  pictures and asks also when the application window closes; All asks before replacing unpublished
  pictures; the suggested link is `../images/`; the state reports its scenario (`in`); messages for a
  missing Python or a failed compose.
- `shots.py`: broken shot files end with a sentence; `--only` with a page part reads only that page;
  `unused --delete` removes the work entry and writes the shot file atomically; scenario names checked.
- A failed fixture load takes no pictures.
- Self test: unexpected warnings fail a case; `expect` pins absolute states; new cases for label
  clicks and links, Ctrl selection, refused steps, a scenario's shots in one replay with a context
  menu shot; `checkCoverage()` for handlers and menu entry names.
- `CShotRunner` folded into `CShotEntry::run()`; `CShotEntry` is a namespace.

## Open

- Optional: pin fontconfig in `pinned_env()` like the platform theme. A run is byte-identical on one
  machine; swapping `FONTCONFIG_FILE` changed all 9 `test` pictures.

## Checked live (2026-11-01, Xephyr, 255d54af2 plus the working tree)

Take again; a failed Take again keeps the work picture; Base is disabled in a scenario; a double Stop
parks once; a trial that fails stores nothing and returns to `(base)`.
F9 in a scenario accepts a window its actions leave open and refuses one opened by hand. Stop is refused
while a dialog or menu is up. A scenario that no longer replays names the way out and its pictures.
The name dialog grows to its message. `QMS_DOC_MODE=OFF` builds and answers `--doc`/`--shoot` with
"Unknown option".

## Left, deliberately

- Drag and drop is not recorded: replaying it means driving `QDrag::exec()`, unmeasured offscreen.
- One handler over Qt's USER property: the value handlers differ in how they replay (typing vs value,
  share of range), so merging them changes replay semantics; not done.
- `SHOT_EXPOSE` as a registration function, double-click folding written once, `CShotContext`'s
  `begin()`/`shot()` pair, splitting `checkQtControls`: readability only, no defect.
- Reviewer suspicions not reproduced: floating dock moved after undocking, `inputReached()` through a
  dialog's parent, `CShotWarnings` across threads, launcher channel backlog, two sessions on one page.

## Live test setup

Never on the writer's desktop:

```sh
Xephyr :5 -screen 1700x1050 -ac -br -noreset &     # nested display
DISPLAY=:5 kwin_x11 &                              # without --replace
cp -a doc <scratch>/repo/                          # scratch copy, the checkout stays clean
cd <scratch>/repo && DISPLAY=:5 python3 doc/tools/shots.py \
    --binary ~/projects/qmapshack/build/bin/qmapshack -v take test > session.log 2>&1
```

Drive with `DISPLAY=:5 xdotool` (it types `-` as `/` there; never `windowclose`, which destroys the
window without a close event); capture a window with `import -display :5 -window <id>` (`xwd -root`
misses dialogs under kwin in Xephyr). Kill only the launcher by pid (`ps` args without
`--doc-scenario`). Close the Xephyr windows afterwards.
