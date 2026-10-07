# Writing QMapShack documentation

You write a page and mark where the pictures go. You take each picture once, by pointing at it.
QMapShack takes them all again later without you.

## Setting up

You need Python 3 and a QMapShack you can build yourself. Build it the way you normally do on your
system ([Building from Source](../README.md#building-from-source)), and add one option to the
configure step:

```
-DQMS_DOC_MODE=ON
```

`shots.py` uses `build/bin/qmapshack`; pass `--binary path/to/qmapshack` for another one.

On Windows copy `platforms\qoffscreen.dll` from your Qt installation beside the `qwindows.dll` your
build uses.

## The loop

```
  ┌──────────────────────────────────────────────────────────┐
  │ [0]  doc/tools/shots.py take <page>                      │
  │        opens the panel, and QMapShack in (base)          │
  │        a page without a base asks which one to copy      │
  └────────────────────────────┬─────────────────────────────┘
                               ▼
  ┌──────────────────────────────────────────────────────────┐
  │ [1]  write the page                                      │
  │        ![](../images/<page>/name.png)  ->  one row       │
  │        saving the page updates the list                  │
  └────────────────────────────┬─────────────────────────────┘
                               ▼
  ┌──────────────────────────────────────────────────────────┐
  │ [2]  select the next picture marked "not taken"          │
  └────────────────────────────┬─────────────────────────────┘
                               ▼
  ┌──────────────────────────────────────────────────────────┐
  │      does it need something selected, loaded, opened?    │
  │        no   ->  a docker, the map, a setup dialog        │
  │        yes  ->  record a scenario, pick it for the row   │
  └────────────────────────────┬─────────────────────────────┘
                               ▼
  ┌──────────────────────────────────────────────────────────┐
  │ [3]  take it                                             │
  │        one widget      ->  point at it, Ctrl+Shift+F9    │
  │        a context menu  ->  open it, point, Ctrl+Shift+F9 │
  │        part of window  ->  Region, drag a rectangle      │
  └────────────────────────────┬─────────────────────────────┘
                               ▼
  ┌──────────────────────────────────────────────────────────┐
  │      is it right?                                        │
  │        yes ............  [2]  next picture               │
  │        wrong part .....  [3]  take it again              │
  │        wrong state ....  [3]  after fixing the scenario  │
  │        wrong page .....  [1]  rewrite it                 │
  └──────────────────────────────────────────────────────────┘

    when no row says "not taken" any more:

  ┌──────────────────────────────────────────────────────────┐
  │      All            QMapShack takes every picture        │
  │                     again by itself; check each one.     │
  │                     A published one is shown beside it.  │
  └────────────────────────────┬─────────────────────────────┘
                               ▼
  ┌──────────────────────────────────────────────────────────┐
  │      Publish all    moves the new pictures from          │
  │                     doc/images/_work/ to doc/images/,    │
  │                     where git sees them.                 │
  └────────────────────────────┬─────────────────────────────┘
                               ▼
  ┌──────────────────────────────────────────────────────────┐
  │      commit                                              │
  └──────────────────────────────────────────────────────────┘

    When QMapShack itself has changed, the pictures may be out of date:
    open the page and press All. Where a new picture no longer shows what
    the page describes, the app has changed too much for how it is taken:
    record its scenario again, or save the base again, and take it again.
    Then Publish all and commit.
```

## The panel

| | | |
|---|---|---|
| **Scenario** | | |
| ![](../src/icons/32x32/DocRecord.png) | Record | name a scenario and record it |
| ![](../src/icons/32x32/DocStop.png) | Stop | end the recording and store it |
| ![](../src/icons/32x32/DocRename.png) | Rename | rename the selected scenario |
| ![](../src/icons/32x32/DocDelete.png) | Delete | delete the selected scenario; every picture taken in it is deleted too and has to be taken again |
| ![](../src/icons/32x32/DocBase.png) | Base | write the running QMapShack's configuration into the page's base, or copy another page's base; see [The base](#the-base) (with `(base)` selected) |
| **Picture** | | |
| ![](../src/icons/32x32/DocRetake.png) | Retake | take the selected picture again as a build would |
| ![](../src/icons/32x32/DocRegion.png) | Region | drag a rectangle for it |
| ![](../src/icons/32x32/DocRevert.png) | Revert | throw away what you took and how; the project's picture stays |
| ![](../src/icons/32x32/DocPublish.png) | Publish | put it into the project |
| **Page** | | |
| ![](../src/icons/32x32/DocRetakeAll.png) | All | take every picture of the page again, as Retake does for one |
| ![](../src/icons/32x32/DocClean.png) | Clean | delete shots and pictures no page uses |
| ![](../src/icons/32x32/DocPublishAll.png) | Publ. All | put every picture of the page you took again into the project |

- Each button is labelled; its tooltip says more. A greyed-out one has nothing to do.
- Picking a scenario or a picture restarts QMapShack in that state (about seven seconds).
- Closing either window ends the session. Both come back where you left them.
- Ctrl+Shift+F9 is the only key.

## The base

The base is the page's basic configuration, `doc/shots/<page>.ini`: the QMapShack settings the page
opens with, such as dockers, window size, units and maps. Every page needs one; a new page asks
which page's base to copy. A picture is taken with it unless you give it a scenario, and every
recording starts from it. The panel lists it as `(base)`, above the scenarios.

With `(base)` selected, **Base** offers:

- **Save what is on screen as this page's base...** — writes the configuration of the running QMapShack, as it
  would write it on exit, into `doc/shots/<page>.ini`. Settings holding a path on this machine are left out.
- **Copy the base of another page...** — asks first; every picture taken in `(base)` changes.

## Scenarios

A picture needs one when it shows something that is not always there: a track's details, its
profile, its screen options.

**Record** asks for the name (the selected scenario's is offered; an existing one is replaced only
after asking), restarts QMapShack in the base and turns into **Stop**. Do what the state is, press
**Stop**. The recording is stored if it replays.

It stores what you did, never a screen position:

| You did | It stores |
|---|---|
| arranged the window, moved or zoomed the map | the arrangement, splitters, centre, zoom |
| set maps, elevation data, POIs, units, fonts | the scenario's own settings |
| selected, opened or expanded an item | the item, by name |
| clicked or rested the pointer on the map | the point and what was under it |
| clicked a button, tab, row or a row's button | that button, tab or row |
| picked a menu entry, opened a context menu | the entry by what it does, the menu on its item |
| clicked a graph or a grid icon | the x position, the icon |
| changed a control, typed, scrolled a list, pressed a key | the value, the text, the top row, the key |

Not stored: tooltips, closing a window by its title bar, docking a floating docker again by its window
frame, dragging a separator between dockers, typing into a table cell.

A replay stops and names the step when it cannot find what a step addresses.

**Rename** and **Delete** are for scenarios; deleting one means taking its pictures again. Changing
a picture's **Taken in** box asks first and moves QMapShack to the new scenario.

If you change settings after QMapShack started in a scenario, the panel warns after the picture: set
them back, or record the scenario again.

## Taking a picture

Point at it and press **Ctrl+Shift+F9**, then answer:

1. **Take a picture of** — which part: the list, the docker, the window.
2. **Which picture is this?** — the selected row comes first.
3. **Picture taken** — **Keep** or **Throw away**.

- **A context menu:** open it, rest the pointer on the entry to highlight, press Ctrl+Shift+F9.
  The picture remembers both.
- **A dialog in a scenario:** record the scenario up to opening it. In the session it is closed
  again; open it by hand and press Ctrl+Shift+F9.
- **Part of a window:** select the row, press **Region**, drag a rectangle (Escape cancels). Resizing
  the window later means dragging again.

## Taking again and publishing

After a take the panel shows the project's picture and yours side by side. Better: publish it.
Not: revert it.

Pictures never match byte for byte across machines; judge them by eye.

When a Retake or All fails, the console names the step.

Closing the panel with unpublished pictures asks whether to publish them.

## What the rows mean

| State | Meaning |
|---|---|
| taken | shot and picture exist |
| not taken | the page asks for it; take it |
| no image | the shot exists, the picture is gone; take it again |
| not registered | a picture exists but no shot took it |
| not used | a shot no page asks for; **Clean** deletes it |

", taken again" means a picture waits to be published.

## Picture names

`<page>/<subject>[-<variant>]`, lower case with hyphens: `track-details`,
`track-details-graphs`. A page cannot be called `default`.

## The example data

A project "Example" (track, waypoint, route, area), the BEV 1:50 000 map (offline) and
OpenStreetMap (online), Tirol elevation data, POIs and routing for the Tannheimer Tal, and a
database.

For other data, fill `doc/shots/fixtures/<page>/`. The panel creates it with one folder per part:
`projects/`, `maps/`, `dem/`, `poi/`, `routino/`, `database/`. A part holding something replaces the
default's; an empty one stays the default's. The panel header says which parts are the page's own.
Press **All** after a change.

## What is fixed

Light colours, DejaVu Sans 10, English, Fusion style, UTC, one image pixel per screen pixel.

## Files

```
doc/pages/<page>.md              your text
doc/shots/<page>.json            pictures and scenarios
doc/shots/<page>.ini             the page's base
doc/shots/<page>/<name>.ini      a scenario's settings
doc/shots/fixtures/<page>/       the page's own example data
doc/shots/fixtures/default/      the default example data
doc/images/<page>/*.png          the published pictures
doc/images/_work/                taken again, not published; not in git
doc/images/_check/               what `shots.py replay` renders when run by hand; not in git
```

## Not built yet

Animations, dark mode, other languages, recording tooltips.

OpenStreetMap needs a network on the first run.

A picture carries the attribution of the data it shows:

| Shows | Attribution |
|---|---|
| the OpenStreetMap map, POIs or routes | *© OpenStreetMap contributors* |
| the offline map bev_km50 | *Datenquelle: BEV – Bundesamt für Eich- und Vermessungswesen, CC BY 4.0* |
| elevation, hillshading or a profile | *Datenquelle: Land Tirol - data.tirol.gv.at, CC BY 4.0* |

Each part's `SOURCE.md` in `doc/shots/fixtures/default/` has the full terms.

If a dialog says **Cannot photograph this yet**, press **Copy** and put it into a ticket.
