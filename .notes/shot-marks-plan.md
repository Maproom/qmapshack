# Shot marks plan

QMS-1319 (Maproom/qmapshack#1319): marks on controls in documentation images that survive retaking.
Branch `QMS-1319`.

## How to work on this

One open point at a time, in the order of "Open". A decision moves to "Decided" in Oliver's words, a
point it raises is appended to "Open". Stop anywhere; "Next" says where to go on. Done when "Open" is
empty and the walk-through has run once more without new points.

Technical points (JSON form, how a target is found or checked) Claude decides and records here;
only what the writer sees or does goes to Oliver.

## Built (commit 0d134e903)

- Shot key `mark`: a list of widget addresses (frame) or objects
  `{widget, part: handle|branch, row, shape: disc|frame}`, resolved below the shot's widget
  (`CShotPage.cpp`, `CShotAddress::resolve()`).
- Painted on the rendered image in `CShotContext::emitImage()` before the `rect` crop; positions from
  the live widgets (`markArea()`).
- A missing or hidden target fails the shot.
- Tested headless: frames, knob, tree arrow, a map row's opacity knob.

## Decided

- Marks are placed in the running QMapShack by pointing, not on the finished image.
- The writer first picks a marker in the panel, opens a menu or dialog in QMapShack if needed, points
  at the control and presses a key; the marker goes to the pointer. A choice is asked only when the
  target is unclear (widget or parent, slider or knob, row or arrow); see "which target" below.
- The shot carries its marks; the image is then taken with Ctrl+Shift+F9 or Region.
- Targets and shapes are those of the Spanish wiki quick start (see the ticket): widgets, slider knob,
  tree arrow, menu entry, toolbar action, dock title bar, main window splitters; disc, frame, arrow,
  cursor (pointer, hand, resize).
- Splitters matter: many users do not recognise them.
- The panel lists the selected row's marks, one line each; a button of its own removes the selected
  one (not the scenario Delete). Moving is deleting and placing again: a mark names a control, not a
  position.
- Preview: a toggle "Show marks" in the panel's Image group, on by default. On, QMapShack shows the
  selected row's marks and updates them whenever its mark list changes; they are hidden while the
  image is taken, while recording and while Region drags. Off, the marks are still stored and still go into the image.
- Marks live and die with the shot entry in `doc/en/shots/<page>.json`: Retake and All keep them and draw
  them again, Revert resets them with the entry, Publish only copies the image. A shot removed and
  defined again needs its marks again.
- The mark key is Ctrl+Shift+F10, caught by the same application-wide filter as Ctrl+Shift+F9
  (`CShotDocMode::eventFilter()`). Desktop shortcuts on it are unchecked.
- One fixed red, not themed; every shape gets a thin light outline. Images are light only
  (`--color-scheme light` pinned, `DESIGN.md`; dark mode "Not built yet", `WRITING_DOCS.md`): the
  dark check waits for dark images.
- Arrow: pick "arrow", press Ctrl+Shift+F10, first click on the target widget (the head; if unclear,
  the "which target" choice follows and its confirming click is not the tail), second click on a free
  point (the tail), stored as an offset from the target's centre.
  Always a straight line, its direction snapped to a multiple of 45°. A tail that falls outside the
  image after a layout change fails the shot.
- A mark not completely inside the shot's `rect` fails the shot, partly inside too. Without the check
  it is cut off silently (replay of `test/toolbar` with `mark` ["dockWorkspace"] outside its `rect`).
- Marks in an open menu work: a `qApp` event filter sees Ctrl+Shift+F10 while the menu holds its
  grab, `QMenu::actionAt()` at the pointer gives the entry, arrow mode swallows both clicks and the
  menu stays open. Tested on Xephyr with xdotool; KDE's window manager and global shortcuts untested.
- Dock title bar and main window splitters are found with public API (standalone Qt 6 probe on
  Xephyr, scratch `sepprobe`): the title bar is the dock's rect minus `widget()->geometry()`
  (QMapShack's docks have `DockWidgetFeatureMask` in `IMainWindow.ui`, vertical title bar on the
  left); a separator by walking outward from a widget's edge, across its own tab bar, until
  `QMainWindow::isSeparator()` holds, then along it. Not yet tried in QMapShack itself.
- A splitter mark names the two widgets it divides (e.g. `dockWorkspace` | the central widget), not
  a dock and a side: the shot fails as soon as they are no longer neighbours.
- A splitter mark next to a tab group: the named dock stands for its whole tab group, so the mark
  holds whichever tab is in front (the separator stays put, scratch `tabprobe`); it fails only when
  the groups are no longer neighbours.
- The JSON form of a mark is the tool's business: the writer only points, the tool reads and writes
  it. Dock title bar: `{widget: "<dock>", part: "title"}`.
- A mark not completely inside the image fails the shot; this also catches a dock behind another
  dock's tab, which sits at negative coordinates while `isVisibleTo()` stays true (scratch `tabprobe`).
- Walk-through of `doc/WRITING_DOCS.md` (point 10), technical answers: Escape cancels a mark half
  placed (arrow between its clicks); Delete of a scenario drops its shots' marks, as it already keeps
  only `id` (`CShotFiles::deleteScenario()`, "A widget address and a rectangle frame something else in
  another state"); Clean drops them with the shot; Rename keeps them; a recorded-again scenario keeps
  them; Base keeps them and a replay fails a mark that no longer resolves. Ctrl+Shift+F9 or Region
  with a mark outside the chosen part refuses at once and names the mark, not only at the next Retake.
- Failures (point 11), each fails the shot and the Retake/All dialog names the mark: target gone
  (address resolves to nothing); hidden (`isVisibleTo()`), behind a dock tab or otherwise outside the
  image; outside `rect`; arrow tail outside the image; splitter neighbours no longer adjacent; menu
  entry gone. Address no longer unique: `CShotAddress::resolve()` takes a plain name with
  `findChild()`, which returns whichever widget was created first (`CShotAddress.cpp`, comment in
  `addressOf()`), so a mark would land silently on the wrong one; a mark's plain name must match
  exactly one widget below its own root (`root`, Fourth review), or the shot fails. Same for a row name.
- A shot moved to another scenario ("Taken in") loses its marks with its address and `rect`; they
  are placed again in the new scenario.
- Marks placed on a row not taken yet wait for the first Ctrl+Shift+F9; if "Which image is this?"
  gets another row, they go with the image to that row.
- "Throw away" after a take keeps the marks placed for it: on a row not taken yet they go back to the
  pending marks; a thrown-away image does not clear "marks changed".
- Marks changed on a taken row: an image waiting in `doc/en/images/_work/` is deleted, a published one
  in `doc/en/images/<page>/` stays until a new take is published; the row says the marks changed and
  the image must be taken again, until it is.
- Ctrl+Shift+F10 while recording a scenario is ignored; the panel says "Marks are placed after Stop,
  on an image."
- Review (point 12, fresh agent on this file, `WRITING_DOCS.md`, `DESIGN.md`), technical answers:
  - A live take (Ctrl+Shift+F9) renders through `CShotWriter::renderAll()` (`CShotDocMode.cpp:851`,
    `DESIGN.md` "One configuration, one fixture"), not `emitImage()`: marks must be painted there too,
    by the same code.
  - Targets outside the shot's widget (the central widget as a splitter neighbour, an entry of an
    open menu) resolve from the mark's own root (see `root` below), not below the shot's widget.
  - "Inside" means the drawn mark, outline included.
  - Taking an image again with another part keeps the marks; each resolves again and fails the shot
    if it is not in the new part.
  - Ctrl+Shift+F10 with no row selected or on a "not registered" row is refused with a line in the
    panel. An exposure shot takes marks: a dialog taken live in `(base)` becomes one
    (`CShotDocMode.cpp:880-884`), and its marks resolve below the instance built at replay.
  - Escape in arrow mode cancels the arrow only; the filter swallows it, the menu stays open.
  - Geometry (head on the target's edge, length kept after the 45° snap, sizes, line widths, red,
    drawing order, a target marked twice allowed) is set when building and checked by eye on a replay.
  - A take writes `mark` with the shot (state process); every other change of marks (panel remove,
    Delete, Taken in, Revert, Clean) is a launcher file operation (`DESIGN.md`: the launcher "does
    every other file operation"). New channel messages for picking a marker, placing, removing and
    the preview toggle. `shots.py` keeps shot entries whole when it filters them (`shots.py:621`).
  - `WRITING_DOCS.md` and `DESIGN.md` are updated with the build.
- A mark is deleted with its own button next to the mark list, not the scenario Delete.
- Marks sent by "Which image is this?" to a row with marks of its own replace them.
- Cursor marks: each cursor (pointer, hand, resize) is an entry in the panel's marker list, placed
  like the others; a resize cursor's direction follows the target (a separator between left and right
  gets a horizontal one); the cursor's tip sits on the target's centre.
- The "marks changed" row state survives closing the panel; any new image of the row clears it, no
  matter how it is taken (Ctrl+Shift+F9, Region, Retake, All). Retake and All draw the current marks:
  `CShotDocLauncher::retakeShot()`/`retakePage()` run `shots.py replay`, which reaches
  `ctx.begin(id, rect, marks)` and `ctx.shot()` (`CShotPage.cpp:445-446`), and `shot()` is
  `emitImage()` (`CShotContext.cpp:143`); no other `ctx.shot` call in `shoot/`. Revert clears it too:
  it brings back the entry, marks included, that the published image was taken with.
- Second review (fresh agent, same three files), technical answers:
  - While placing, a mark is held by address from its root (see JSON forms); the take refuses one
    outside the chosen part.
  - Placing on a widget whose plain name is not unique: the address falls back to the class-index
    path, as `addressOf()` already does; uniqueness is checked below the mark's `root`. A menu entry or toolbar action without `objectName` is refused with
    a line in the panel (`DESIGN.md`: menu entries are addressed by `objectName` only).
  - Ctrl+Shift+F10 while Region drags or a take's dialogs are open is ignored.
  - Revert on a row taken but never published, after "marks changed": the row is "not taken", its
    marks go with the entry.
  - Every target and shape combination is allowed, except a resize cursor on a target that is not a
    splitter; a disc or cursor sits on the target's centre, a frame around its area, an arrow head on
    its edge.
  - Cursor artwork is bundled with the doc build (like its fonts), never the platform theme.
  - A mark's readable name in the panel and in failure dialogs: shape and target, e.g. "frame:
    Opacity slider (knob)", from the target's text, tooltip or class.
  - Marks resolve after `set`, `open` and `active`, right before the image.
  - JSON forms (go into `DESIGN.md` beside `mark` with the build):
    `{widget, part: "action", action: "<objectName>"}` for a menu entry or toolbar action;
    `{part: "splitter", between: ["<address>", "<address>"]}`;
    `{..., shape: "arrow", tail: [dx, dy]}`, the tail in image pixels from the target's centre;
    `{..., shape: "cursor", cursor: "pointer|hand|resize"}`. Addresses are from the mark's own `root`
    (Fourth review), never relative to the part, so a new part needs no rebase.
- Each line of the panel's mark list shows a status: shown, not on screen (target exists, closed or
  hidden now), not found (broken).
- Closing with rows that say "marks changed" warns too ("3 images need taking again because their
  marks changed. Close anyway?"), beside the question about unpublished images.
- Third review (fresh agent, same three files), technical answers:
  - Marks on a row not taken yet are kept by the launcher in `doc/shots/_cache/` per page, so a
    restart of QMapShack (picking a row or scenario) or closing the panel keeps them; the first take
    moves them into the new entry.
  - Placing or removing a mark: the state reports it over the channel, the launcher writes the shot
    file. Before the first change after a take, the launcher saves the entry as it was to
    `_work/<id>.shot.json`, as a take does, and deletes only the `_work` image: Revert then brings back
    the marks the image was taken with (`CShotFiles::revertShot()` restores that file).
  - "Marks changed" is stored by the launcher in `doc/shots/_cache/` per page, beside the pending
    marks, so it does not depend on what `_work/` holds.
  - The confirmations of Taken in and scenario Delete say that marks go too.
  - The preview is hidden while recording and while Region drags.
  - The "which target" list is not a popup over QMapShack: a popup would take an open menu's grab.
    See "which target" at the end of this section.
  - Pointing at a separator: the widgets on both sides of the pointer are the pair; a tab group is
    named by its front dock. `QSplitter` handles inside dockers count as splitters too.
  - All shapes get the thin light outline; a cursor's bounds are its artwork's.
  - Channel: state to launcher `mark-placed`, `mark-status` (per line: shown, not on screen, not
    found); launcher to state `marker`, `marks` (the selected row's list), `marks-shown`.
  - `shots.py selftest` gets cases for placing, refusals, the menu path and each failure dialog.
- The picked marker stays picked after placing, for the next mark; Ctrl+Shift+F10 with none picked
  says "Pick a marker first." in the panel.
- Fourth review (fresh agent, same three files), technical answers:
  - The Revert backup `_work/<id>.shot.json` is written only if none exists: on a row taken again and
    not published it already holds the entry from before that take, so Revert brings back the
    published marks. `publish` keeps a `<id>.shot.json` whose image is not in `_work/`.
  - Throw away removes only what that take wrote.
  - Each mark records its root: `root: "main" | "menu" | "window" | "dock:<address>"` (main window, the
    open menu, the window a scenario leaves open or the exposure's dialog, a floating docker).
  - No shot file uses `mark` yet (`grep -l '"mark"' doc/en/shots/*.json` finds none): no old form to
    convert; the built form (addresses below the shot's widget) is replaced.
  - Pending marks of a row not taken yet: dropped when its Taken in changes (as for a shot); Clean
    drops those of rows no page references.
  - A menu-entry mark on a row whose shot opens no menu is allowed; it shows "not on screen" until a
    take inside the menu stores `open`.
  - The picked marker is unpicked by clicking it again in the panel.
  - The readable name is stored with the mark when it is placed, so "not found" still has one.
  - Arrow: Ctrl+Shift+F10 only starts arrow mode; the head is the first click. `tail` is stored
    snapped to 45°.
  - Cursor artwork goes into `shoot/doc.qrc` beside the fonts and `Doc*` icons (`shoot/CLAUDE.md`),
    each with its hotspot in the code.
- The "which target" choice: Ctrl+Shift+F10 again steps through the candidates (preview shows the
  current one, the panel lists them); a click in QMapShack or Return confirms. Tested on Xephyr
  (scratch `choosetest`, two processes, xdotool): the filter sees each step and the Return and click
  while the menu is open, swallows them, nothing triggers, the menu stays open; a click into the other
  process's window closes the menu (`MENU closed`), so the choice must not need the panel.
- Fifth review (fresh agent, same three files), technical answers:
  - Ctrl+Shift+F10 reaches the process with the keyboard focus. After picking a marker the panel
    (launcher) has it, and `CShotDocPanel`/`CShotDocLauncher` do not forward Ctrl+Shift+F9 today
    (`grep` finds no forwarding): the launcher catches Ctrl+Shift+F10 too and sends the pointer's
    global position to the state, so no click into QMapShack is needed. Tested on Xephyr with KWin
    (own D-Bus session) and two processes (scratch `keytest`): with the panel focused the panel got
    the key and forwarded `225,198`, and the app found `theButton` under it; with the app focused the
    app got the key itself; no click triggered the button. Ctrl+Shift+F9 has the same gap today:
    real tool on Xephyr with KWin, panel focused (`xdotool getwindowfocus`: "Documentation mode"),
    pointer on the workspace list, key pressed: no dialog, `take.log` shows the key events at
    `CShotDocPanelClassWindow`; after a click into QMapShack the same key opens "Take an image of". Fixed in QMS-1319 (Oliver): the
    launcher forwards Ctrl+Shift+F9 like Ctrl+Shift+F10.
  - Row display: "marks changed" is shown as its own state ("marks changed, take it again") and wins
    over ", taken again" and over "no image".
  - "Which image is this?" on a taken row: the marks on screen go with the image to the chosen row
    (replacing its own); the selected row keeps its marks.
  - Escape during the "which target" choice cancels the choice only (swallowed, a menu stays open); a
    click anywhere in QMapShack confirms the current candidate (a click into the panel would close an
    open menu, see "which target").
  - A restart of QMapShack (row or scenario picked) drops a half-placed arrow and a pending choice; the
    picked marker and "Show marks" are panel state and stay.
  - A row without a name (`DESIGN.md`) is refused as a target with a line in the panel.
  - Mark status adds "fails: <reason>" (outside the part, not unique) beside shown, not on screen and
    not found, so the writer sees it before the take.
  - A mark with `root: "dock:..."` on a docker docked at replay is "not found" and fails the shot; the
    shot's `layout` restores the floating state.
  - A "not used" row takes marks like any shot; Clean deletes them with it.
  - An orphan Revert backup in `_work/` is removed by Revert, by Publish of a new take of that row, or
    by Clean. `DESIGN.md`'s `publish` line ("empty `_work/`") is updated with the build.
  - Pending marks follow their row's scenario: scenario Delete drops them, Rename keeps them.
  - Mark schema, all keys: `root` ("main" | "menu" | "window" | "dock:<address>"), `widget`
    (address below `root`), `part` ("" | "handle" | "branch" | "title" | "action" | "splitter"), `row`
    (branch), `action` (objectName), `between` (two addresses, splitter), `shape` ("frame" | "disc" |
    "arrow" | "cursor"), `tail` ([dx, dy], arrow, snapped), `cursor` ("pointer" | "hand" | "resize"),
    `name` (readable name). Pending marks and "marks changed": `doc/shots/_cache/<page>-marks.json`,
    `{pending: {<id>: [mark...]}, changed: [<id>...]}`, launcher-owned.

## Open


## Next

Built in stages on `QMS-1319` (not pushed): 6ee6ecb66 drawing, 2615b2118 placing, 235b319a6 pending marks and
"marks changed". Uncommitted on 2026-11-18 (Oliver: "not yet"):
- Look: one red; disc with a hole for its target; pointer, hand, resize on a red halo; frame around the target with a
  see-through band; marker icons on the panel buttons from `CShotMarks::icon()`.
- A part whose marks reach past it is cut from its window with room for them (Oliver chose this over Region);
  a mark whose target lies outside the part fails the shot ("which is not in the image"); where the window ends
  first, the frame runs along its edge (Claude's choice, not confirmed).
- Mark status "not in this image" in the panel (warn, never refuse).
- Docs (stage 4): `doc/WRITING_DOCS.md` "Marks" section rewritten several times with Oliver, last version accepted
  ("ok better"), then "not in this image" added; `src/qmapshack/shoot/DESIGN.md` mark keys, channel, publish.
- 2026-11-19, Oliver's glitches: a docker's tab and a button a list's delegate paints into a row can be marked
  (`part: "tab"`, `part: "button"` with `row` and `button`). A dock tab bar's `tabData()` holds its docker
  (Qt 6.10.2, scratch `tabprobe`); row buttons through `CShotHandlers::rowButtonRect()`/`rowButtonAt()`, all three
  delegates. Self test points at both through `CShotMarker::markAtPointer()`, moving the main window onto the 800x800
  offscreen screen first. Untested by hand on `:5`.
- A disc on a target too big for it (its hole cut the disc apart on a tab) is refused, when pointing and when
  drawn (Oliver: "Frame instead"). `doc/en/shots/test.json` reverted to HEAD on his request.
- The marker's refusals show as a tooltip at the pointer too (`CShotMarker::refuse()`), not only in the panel's
  status line; prompts while choosing stay in the status line.
- 2026-11-19, Oliver's panel points: hints at the pointer wrap at 40 characters (`CShotMarker::hint()`);
  Ctrl+Shift+F10 without an image, while recording or on an unregistered row refuses there too; a mark placed outside
  the image says so there (kept). Panel: framed groups Scenarios, Images, and "Selected image: <id>" with markers, a
  Mark | Status table, and the preview; Show (eye) and Remove as tool buttons in the markers' row after a separator;
  that group is disabled without a selected image (a picked marker is put away). Marker buttons share the other tool
  buttons' size, icons drawn at the shown size. Checked by rendering the panel offscreen (temporary hook, removed);
  untested on a desktop. Separator lines in both rows.
- Settings drift (Oliver moved an opacity knob, Retake replayed without it): the drift warning after Ctrl+Shift+F9
  shows at the pointer; Retake and All ask the state (`drift`) and offer Save as base, then retake / Retake anyway /
  Cancel. Built, self test passes; the dialog flow is untested.
- Independent review 2026-11-19, fixed: tree arrow placed as a square (a tall row refused the disc); `discFits` caps
  the thickness at `kSpot`; marks on a window a scenario opens: placed on the window pointed at or open
  (`CShotMarker::windowShot()`) and judged against the shot's `window`; `drift-unknown` while the state is busy;
  a pending drift question dropped when the state ends; one `CShotMarks::framed()` for build and live take (a
  floating docker's own image); a failed take names its reason at the pointer; one `withRowDelegate()`; docker tab
  lookup in `CShotMarks::dockTab()`/`dockOfTab()`.
- Self test cases for all of it; last run 138 of 138 and 57 of 57.
- `doc/en/shots/test.json` holds Oliver's own test marks: keep, never commit.

To discuss first: Oliver's replay of `test/menu-project` failed with "marks the slider in Workspace, which is not in
the image" while the row's image was `menuProject`; after he took it again on `IGisWorkspace` it replays. How marks
and the image's part should relate (warn only, grow the image, fail) needs his decision; the frame along the window
edge looked strange to him.

Known gaps: no preview inside an open menu; marks placed before the naming fix keep class names; untested by hand:
Ctrl+Shift+F10 while recording, a floating docker, Revert/Taken in/scenario Delete/Clean with marks.

Testing: Xephyr `:5` with KWin in its own D-Bus session (`dbus-run-session -- kwin_x11`), xdotool there; a throwaway
page `markprobe` removed afterwards. Stop helpers by PID only (`pkill -x kwin_x11` once killed Oliver's KWin).
