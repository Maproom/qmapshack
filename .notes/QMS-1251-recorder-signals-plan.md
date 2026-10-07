# QMS-1251 — the recorder

#1251 and #1252 are committed. The design is `QMS-1217-documentation-images.md` §4; the facts it rests on
are the recorder bullets in `CLAUDE.md`. The queue that performs a scenario is #1253 (`CShotReplay`).

## The contract

Writers record on their own machines and every machine renders a few pixels differently, so steps
carry meaning - an action's name, a row's name path, a value, degrees, an x value - and not input
positions. That makes the vocabulary finite and grown by maintenance, like Squish's; no design that
records on one machine and replays on another gets rid of that. What keeps it from a review loop:

1. **The supported vocabulary is the handlers**, each backed by self-test cases that record through
   the window system, replay, and compare the state.
2. **Nothing unsupported goes unnoticed while recording**: a click nothing covers is reported, a key
   press nothing made a step of is recorded as itself.
3. **A recording is saved only if it replays** - every step's replay check passes in a state started with
   `--doc-trial`, the recording parked in `_cache` until then - #1257. A
   handler bug then shows as "record this differently" when it happens, not as a wrong picture later.
4. **A finding is a local handler fix with a case that fails first** - never a design change.

## Why the first two designs failed

Both took a press and inferred what it meant - a state diff credited to the press, and a whitelist
when the diff was empty. Every Qt delivery detail became a patch on that inference. The third design
records what Qt's controls say, kept only when the input went to the control that said it, and a
surface's input as input. Do not bring back a rule about what a press might have meant: a symptom
that seems to need one means a class has no handler.

## How it was verified, and what that cost

The first self test drove the application with QTest's `QWidget` functions, which are no real input
(see `CLAUDE.md`). It passed cases that fail on the real path and hid bugs a review then found. Every
case now goes through `CShotSynth`, and each replays what it recorded and compares the state. Qt
behaviour a handler depends on is read from Qt's source or measured before it is coded - the context
menu's receiver (`qt_last_mouse_receiver`) was first guessed, coded, and wrong.

Throwaway measurements of Qt alone: `.notes/QMS-1251-notify-frame-probe.cpp` (the frame, through
QTest's widget path - its conclusions about signals hold, its claim of real delivery does not).

## Not covered, and why

- A window closed by its title bar: indistinguishable from the application closing it.
- Re-docking a floating dock through its window frame: not Qt input headless, not verified on X11.
- A main window separator dragged: no signal; reported as a click nothing covers.
- Typing into an item view's cell editor: the editor is gone when the edit ends.
- A pinch or touchpad gesture: reported. The touchpad's `NativeGesture` is not measured headless.
