# Writing QMapShack documentation

You write a page and mark where the images go. You take each image once, by pointing at it.
QMapShack takes them all again later without you. [CREATING_DOCS.md](CREATING_DOCS.md)
describes how to generate the pages.

## Setting up

You need Python 3.9 or newer and a QMapShack you can build yourself. Build it the way you normally do
on your system ([Building from Source][BUILD]), and add one option to the configure step:

```
-DQMS_DOC_MODE=ON
```

This also needs Qt's Test module and, from Qt 6.9 on, its GuiPrivate package.

Run `shots.py` from the top folder of your checkout: `python3 doc/tools/shots.py <command>`, on
Windows `py doc\tools\shots.py <command>`. It uses `build/bin/qmapshack`; pass
`--binary path/to/qmapshack` for another one. By default it works on the English pages in
`doc/en/`. With `--lang de`, `--lang es` or `--lang it` before the command, it works on `doc/de/`, `doc/es/` or
`doc/it/` and takes the images with a German, Spanish or Italian QMapShack.

With bash, Tab completes commands, page names and shot ids: `doc/tools/shots.py take quickstart-d` and Tab. The
first run sets it up for new terminals; it needs the bash-completion package, as git's completion does.

On Windows copy `platforms\qoffscreen.dll` from your Qt installation beside the `qwindows.dll` your
build uses.

## The loop

```
  ┌──────────────────────────────────────────────────────────┐
  │ [0]  python3 doc/tools/shots.py take <page>              │
  │        opens the panel, and QMapShack in (base)          │
  │        a page without a base asks which one to copy      │
  └────────────────────────────┬─────────────────────────────┘
                               ▼
  ┌──────────────────────────────────────────────────────────┐
  │ [1]  write the page                                      │
  │        ![](../images/<page>/<page>-name.png) -> one row  │
  │        saving the page updates the list                  │
  └────────────────────────────┬─────────────────────────────┘
                               ▼
  ┌──────────────────────────────────────────────────────────┐
  │ [2]  select the next image marked "not taken"            │
  └────────────────────────────┬─────────────────────────────┘
                               ▼
  ┌──────────────────────────────────────────────────────────┐
  │      does it need something selected or opened?          │
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
  │        yes ............  [2]  next image                 │
  │        wrong part .....  [3]  take it again              │
  │        wrong state ....  [3]  after fixing the scenario  │
  │        wrong page .....  [1]  rewrite it                 │
  └──────────────────────────────────────────────────────────┘

    when no row says "not taken" anymore:

  ┌──────────────────────────────────────────────────────────┐
  │      All            QMapShack takes every image          │
  │                     again by itself; check each one.     │
  │                     A published one is shown beside it.  │
  └────────────────────────────┬─────────────────────────────┘
                               ▼
  ┌──────────────────────────────────────────────────────────┐
  │      Publ. All      moves the new images from            │
  │                     doc/en/images/_work/ to              │
  │                     doc/en/images/, where git sees them. │
  └────────────────────────────┬─────────────────────────────┘
                               ▼
  ┌──────────────────────────────────────────────────────────┐
  │      generate the pages and commit (CREATING_DOCS.md)    │
  └──────────────────────────────────────────────────────────┘
```

When QMapShack itself has changed, the images may be out of date: open the page and click **All**.
Where a new image no longer shows what the page describes, record its scenario again, or save the
base again, and take it again. Then **Publ. All**, generate and commit.

## The panel

| | | |
|---|---|---|
| **Scenarios** | | |
| ![](../src/icons/32x32/DocRecord.png) | Record | name a scenario and record it |
| ![](../src/icons/32x32/DocStop.png) | Stop | end the recording and store it |
| ![](../src/icons/32x32/DocRename.png) | Rename | rename the selected scenario |
| ![](../src/icons/32x32/DocDelete.png) | Delete | delete the selected scenario; every image taken in it is deleted too and has to be taken again |
| ![](../src/icons/32x32/DocBase.png) | Base | write the running QMapShack's configuration into the page's base, or copy another page's base; see [Base and scenario](#base-and-scenario) |
| **Images**: the selected one | | |
| ![](../src/icons/32x32/DocRetake.png) | Retake | take the selected image again as a build would |
| ![](../src/icons/32x32/DocRegion.png) | Region | drag a rectangle for it |
| ![](../src/icons/32x32/DocRevert.png) | Revert | throw away what you took and how; the project's image stays |
| ![](../src/icons/32x32/DocPublish.png) | Publish | put it into the project |
| **Images**: the whole page | | |
| ![](../src/icons/32x32/DocRetakeAll.png) | All | take every image of the page again, as Retake does for one; asks first when images you took are not published |
| ![](../src/icons/32x32/DocClean.png) | Clean | delete shots and images no page uses |
| ![](../src/icons/32x32/DocPublishAll.png) | Publ. All | put every image of the page you took again into the project |
| **Marks** | | |
| | Frame, Disc, Arrow, Pointer, Hand, Resize | the marker **Ctrl+Shift+F10** places; click it again to put it away |
| ![](../src/icons/32x32/ShowAll.png) | Show | show the selected image's marks in QMapShack; they go into the image either way |
| ![](../src/icons/32x32/DocDelete.png) | Remove | take the selected mark off the image |

- Each button is labeled; its tooltip says more. An unavailable one has nothing to do.
- The bottom group belongs to the selected image: its marks and its preview. It is unavailable until you select
  an image.
- Picking a scenario or an image restarts QMapShack in that state.
- Closing either window ends the session. Both come back where you left them.
- **Ctrl+Shift+F9** takes an image, **Ctrl+Shift+F10** places a mark; both work while the panel or one of its
  dialogs has the keyboard. **Esc** cancels **Region**.

## Base and scenario

The *base* is QMapShack as the page starts it: dockers, maps, elevation data, POIs, units, map
position, stored in `doc/en/shots/<page>.ini`. Set it up by hand and save it with **Base**; nothing in
it is recorded. Every page needs one; a new page asks which page's base to copy. An image is taken
in it unless you give it a scenario, and every recording starts from it. The panel lists it as
`(base)`, above the scenarios.

A *scenario* is a short recording on top of the base, for what a restart loses: a selected item,
an open window, a range on a track, screen options. Other maps or elevation data need another base,
not a scenario.

With `(base)` selected, **Base** offers:

- **Save what is on screen as this page's base...** — writes the configuration of the running QMapShack, as it
  would write it on exit, into `doc/en/shots/<page>.ini`. Settings holding a path on this machine, and account
  details such as API keys and passwords, are left out.
- **Copy the base of another page...** — asks first; every image taken in `(base)` changes.

## Scenarios

**Record** asks for the name (the selected scenario's is offered; an existing one is replaced only
after asking), restarts QMapShack in the base and turns into **Stop**. Set up the state the image needs,
close every dialog and menu, then click **Stop**. The recording is stored if it replays.

It stores what you did, never a screen position:

| You did | It stores |
|---|---|
| arranged the window, moved or zoomed the map | the arrangement, splitters, center, zoom |
| selected, opened or expanded an item | the item, by name |
| clicked or rested the pointer on the map | the point and what was under it |
| clicked a button, tab, row or a row's button | that button, tab or row |
| picked a menu entry, opened a context menu | the entry by what it does, the menu on its item |
| clicked a graph or a grid icon | the x position, the icon |
| changed a control, typed, scrolled a list, pressed a key | the value, the text, the top row, the key |
| clicked anything else, such as a link in a label | the place on it |

Not stored: tooltips, closing a window by its title bar, docking a floating docker again by its window
frame, dragging a separator between dockers, typing into a table cell, drag and drop, touchpad
gestures. Ctrl and Shift on a list row are stored.

A replay stops and names the step when it cannot find what a step addresses. Record that scenario
again under the same name: its images stay with it; take them again.

**Rename** and **Delete** are for scenarios; deleting one means taking its images again. Changing
an image's **Taken in** box asks first and moves QMapShack to the new scenario.

If you change settings after QMapShack started in a scenario, a map's opacity for example, a replay will not show
them: Retake and All start from the base and replay the scenario. QMapShack says so next to the pointer after
**Ctrl+Shift+F9**. **All** asks first, and so does **Retake** on an image of the scenario QMapShack is in. In
(base), **Save as base, then retake** keeps them; in a scenario, record it again with them.

## Taking an image

Point at it and press **Ctrl+Shift+F9**, then answer:

1. **Take an image of** — which part: the list, the docker, the window.
2. **Which image is this?** — the selected row comes first.
3. **Image taken** — **Keep** or **Throw away**.

- **A context menu:** open it, rest the pointer on the entry to highlight, press **Ctrl+Shift+F9**.
  The image remembers both.
- **A window in a scenario:** only one the scenario's recording leaves open can be taken. A dialog
  that waits for an answer cannot stay open at **Stop**; take it in `(base)` instead.
- **Part of a window:** select the row, click **Region**, drag a rectangle (**Esc** cancels).

## Marks

A mark shows the reader where to look in an image: a red rectangle around a button or list, a red spot on a slider's
knob, a red arrow pointing at something, or a red mouse pointer where to click or drag. It belongs to the image and
is drawn again whenever the image is taken again.

A highlighted menu entry needs no mark: the highlight already shows it.

### Placing a mark

1. Select the image's row.
2. In the panel, under **Marks**, click the button of the marker you want: Frame, Disc, Arrow, Pointer, Hand or
   Resize.
3. In QMapShack, point at what you want to mark and press **Ctrl+Shift+F10**.

If it is in a menu or dialog, open that first.

On the map, QMapShack first offers the place under the pointer. That mark stays on the place, not on the window
position: zoom or pan and it moves with the map.

When nothing can be marked there, or the marker does not fit, QMapShack says why next to the pointer.

### The markers

| Button | Draws | On | |
|---|---|---|---|
| Frame | a red rectangle around it | a button, a list, a field, a docker, a tab, a button in a list's row | |
| Disc | a red spot on it | a small part: a slider's knob, the arrow that opens a list entry, a place on the map | refused on anything bigger, such as a tab or a field: use Frame |
| Arrow | a red arrow pointing at it | anything | then click where the arrow starts; it runs in steps of 45° |
| Pointer | a red mouse pointer | where to click, a place on the map | |
| Hand | a red pointing hand | what to drag | |
| Resize | a red double arrow | the separator between two dockers, to show it can be dragged | |

### When QMapShack is not sure

It outlines one candidate in blue, for example the list or the docker around it. **Ctrl+Shift+F10** outlines the next, a
click or **Enter** takes it, **Esc** cancels.

### The list of marks

The table below the markers lists the marks of the selected image, each with its status: *shown*, *not on screen* (in
a closed menu or under a collapsed row, for example), *not in this image* (outside what the image shows; take it again
with what it should show), *not found*, or *fails*, and why. A mark placed outside the image is kept; QMapShack says so
next to the pointer. **Remove** takes the selected one off. To move a mark,
remove it and place it again; it stays on what it marks when the layout changes.

### Marks and the image

- Marks placed before the image is taken go into it with **Ctrl+Shift+F9**.
- If you change the marks of an image you already took again, that image is thrown away. The row says *marks
  changed, take it again* until you take it again or revert it.
- An image moved to another scenario, or whose scenario is deleted, loses its marks.

## Taking again and publishing

After a take the panel shows the project's image and yours side by side. If yours is better, publish it;
if not, revert it.

Images never match byte for byte across machines; judge them by eye.

When a Retake or All fails, a dialog says which image and why.

Taking an image again replays it: QMapShack performs its scenario's recorded steps without you,
headless, and photographs the result. `shots.py replay` does that for every page, to check that every
image still comes out. It writes into `doc/en/images/_check/`. Nothing is published from there, and the
folder can be deleted.

Closing the panel or the QMapShack window with unpublished images of this page asks whether to
publish them, and with images whose marks changed whether to close anyway. An image you take shows up in the
panel only once you press Keep.

## What the rows mean

A *shot* is how an image is taken: its scenario and the part it shows, stored in
`doc/en/shots/<page>.json`.

| State | Meaning |
|---|---|
| taken | shot and image exist |
| not taken | the page asks for it; take it |
| no image | the shot exists, the image is gone; take it again |
| not registered | an image exists but no shot took it |
| not used | a shot no page asks for; **Clean** deletes it |
| marks changed, take it again | its marks changed since the image was taken |

A state followed by ", taken again" means a new image waits to be published.

## Image names

`<page>/<page>-<subject>[-<variant>]`, lower case with hyphens: `track-filters/track-filters-details`,
`track-filters/track-filters-details-graphs`. A page cannot be called `default`.

## The example data

A project "Example" (track, waypoint, route, area), the BEV 1:50 000 map (offline) and
OpenStreetMap (online), Tirol elevation data, POIs and routing for the Tannheimer Tal, and a
database.

For other data, fill `doc/shots/fixtures/<page>/`. The panel creates it with one folder per part:
`projects/`, `maps/`, `dem/`, `poi/`, `routino/`, `database/`, `device/`. A part holding something replaces the
default's; an empty one uses the default's. A file `EMPTY` in a part leaves the page without it, as on a
first start. The panel header says which parts are the page's own.
`projects/` loads one `.qms` or `.gpx` file, the first by name; a dialog about a track, waypoint,
route or area can only be photographed if the project has one.
`device/` is shown in the workspace as a Garmin connected as USB mass storage and needs
`Garmin/GarminDevice.xml`; each run gets a fresh copy. The default fixture has none.
Click **All** after a change.

OpenStreetMap needs a network on the first run.

A page names the attribution of the data its images show, in a line of text below its introduction:

| Shows | Attribution |
|---|---|
| the OpenStreetMap map, POIs or routes | *© OpenStreetMap contributors* |
| the offline map bev_km50 | *Datenquelle: BEV – Bundesamt für Eich- und Vermessungswesen, CC BY 4.0* |
| elevation, hillshading or a profile | *Datenquelle: Land Tirol - data.tirol.gv.at, CC BY 4.0* |
| the online map OSM Topo | *OpenTopoMap, CC-BY-SA*; its trails overlay *trails by waymarkedtrails.org* |
| the vector map Tannheimer Tal | *© OpenStreetMap contributors, created by BBBike* |

Each part's `SOURCE.md` in `doc/shots/fixtures/default/` has the full terms.

## What is fixed

Light colors, DejaVu Sans 10, the page's language, Fusion style, UTC, one image pixel per screen pixel.

The QMapShack window is 1200 × 800 and cannot be resized, so a scenario replays the same on every
machine. A screen without room for it, display scaling included, cannot record or take images;
QMapShack says so.

## Files

```
doc/en/pages/<page>.md              your text
doc/en/shots/<page>.json            images and scenarios
doc/en/shots/<page>.ini             the page's base
doc/en/shots/<page>/<name>.ini      a scenario's settings
doc/shots/fixtures/<page>/          the page's own example data
doc/shots/fixtures/default/         the default example data
doc/en/images/<page>/*.png          the published images
doc/en/images/_work/                taken again, not published; not in git
doc/en/images/_check/               what `shots.py replay` renders when run by hand; not in git
```

## Not built yet

Animations, dark mode, languages other than English, German, Spanish and Italian, recording tooltips.

If a dialog says **Cannot photograph this yet**, click **Copy** and put it into a ticket.

[BUILD]: https://github.com/Maproom/qmapshack/blob/dev/README.md#building-from-source "Building from Source"
