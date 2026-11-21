# Quick start plan

Step 7 of `documentation-plan.md`: the quick start as the first real page. Developed step by step
with Oliver; decisions are recorded here only once he has made them.

## Goal

A new English quick start, starting from the Spanish wiki guide
(`~/projects/QMapShack_Wiki/DocQuickStartSpanish.md`), the best of the language versions. Not a
translation: a better guide. It follows `doc/STYLE_GUIDE.md` and `doc/WRITING_DOCS.md`, and its
images are taken with the shot tool from fixture data.

## Decided

- Packaging is not part of the documentation work.
- The example data is the default fixture, `doc/shots/fixtures/default/` (Tannheimer Tal).
- A small vector map fixture is added: Garmin IMG, BBBike custom extract (extract.bbbike.org), style
  Garmin OpenTopoMap UTF-8, bbox about 10.45,47.47 → 10.64,47.57 (extent of the fixture's DEM), in the default fixture as
  `maps/Tannheimer_Tal.img`.
- Vector map `Tannheimer_Tal.img`: attributed as OpenStreetMap data, created by BBBike (Oliver); the style's own
  licence is not pursued.
- GPS device: covered; when that part comes, try to fake a device for the shot tool.
- Topic list, in this order:
  1. The main window: canvas and the panels around it
  2. Turning on a map (BEV km50, OSM), stacking maps and opacity
  3. Turning on a vector map: detail level and TYP
  4. Zooming and panning
  5. Turning on a DEM and hillshading
  6. Loading a file (`Example.qms`)
  7. Drawing a track with automatic routing, including the **V** mode on the vector map
  8. Adding a waypoint, including one taken from a POI
  9. Saving: projects vs databases
  10. Sending to the device

  Installation is a link to the release page.
- Each page is written with Oliver in three steps: Claude summarises the page's goal (what it is about, what the
  reader knows afterwards), Oliver reviews and changes it; then the page is written section by section; screenshots
  and marks are decided per section on the way.
- Page layout: an action is a bullet holding a right-aligned `{image}` of the detail it names, cropped to at most
  300 px wide, no wider than its content, and as low as its marks allow; `doc/_static/qms-layout.css` keeps each image beside its bullet, with a
  border and a line between items. An image too wide to sit
  beside its text goes below it with `:class: below-right`. Images in table cells are not allowed.
- Claude may write scenarios and shot entries by hand instead of recording them; a person must still be able to
  take, switch to and re-record them in the shot tool.
- One page per topic, linked in order from a quick start overview page, so each page can have its
  own base in the shot tool. Working file names in `doc/en/pages/`, may change with the content:
  `quickstart.md` (overview), `quickstart-main-window.md`, `quickstart-maps.md`,
  `quickstart-vector-map.md`, `quickstart-zoom-pan.md`, `quickstart-dem.md`,
  `quickstart-load-file.md`, `quickstart-draw-track.md`, `quickstart-waypoints.md`,
  `quickstart-save.md`, `quickstart-device.md`.

## Open

- Marking elements in images. Prototype on branch `QMS-1319` (Maproom/qmapshack#1319): shot key `mark`, a widget address
  (frame) or `{widget, part: handle|branch, row, shape: disc|frame}`, drawn in
  `CShotContext::emitImage()`; a hidden or missing target fails the shot. Tested headless on the test
  page incl. a map row's opacity knob. Open: interactive placement in the panel, docs (`DESIGN.md`,
  `WRITING_DOCS.md`), a self-test case, menu entries/toolbar actions, main window splitters, cursors.
- GPS device: whether the shot tool can show a faked device in the workspace is unchecked.

## Spanish guide — analysis

Strengths:

- One example runs through: load `Sallent.gpx`, draw the Bachimaña circuit with routing, add
  waypoints, save as project, send to the device.
- Explains why: VRT, database vs GPX (l. 426–428), a GPX file is a container (l. 519), online vs
  local DEM (l. 333–339), which drawing mode to start with (l. 395).
- Pitfalls: TYP fits only its map (l. 293), no Garmin NT (l. 126), device items not editable
  (l. 540).

Dated or unfit:

- Images from 1.10.0; installer text for 1.14.0 with MSVC 2013 runtime (l. 162–165); Windows only.
- Mega/Dropbox/forum links, Spain-only data; routing data downloaded as `Routino_IBERIA_202012`
  (l. 69) but selected as `Iberia_201902` (l. 259).
- Against the style guide: "QMS", "we", long sentences, UI labels in backticks, menu paths with `-`.
- UI labels not yet compared with the current build.

Better on concept pages (plan step 8): the `CARTO` folder layout (l. 84–157), building a VRT, DEM
online vs local, database folder colors (l. 453–457), map views (l. 301–359).

## Spanish chapters vs the default fixture

| Chapter | Fixture |
|---|---|
| Activate maps, stacking, opacity | BEV km50 (offline), OSM (online) |
| Activate a DEM, hillshading | Tirol DGM 5 m |
| Load a GPX file | `Example.qms` |
| Draw a track, automatic routing | Routino Tannheimer Tal |
| Waypoint, also from a POI | `tannheimer_tal.poi` |
| Projects and databases, copy between them | `Example.qms`, `Example.db` |
| Zoom and pan | any |
| Vector map, TYP, **V** mode | `Tannheimer_Tal.img` (BBBike, OpenTopoMap style) |
| GPS device | none; to be faked |
| Installation | not a fixture topic |

## Next

State: pages 1–3 are written in English and German, with images in both languages, and pushed
(`kiozen/QMS-1322`); their bases now use Square scales and their map images were retaken (uncommitted). Page 4,
zooming and panning (`quickstart-zoom-pan.md`, uncommitted): English and German written, all five sections, images
in both languages (German crops need no wider dock), linked from both `quickstart.md`: wheel (`wheel-before`/`wheel-in`, zoom 7 → 5), dragging (`drag`), keyboard (table, no image; + zooms
around the view centre, Up moves the view a quarter of the map's height north, both from trial shots), zoom steps and
scale (`scale-bar`, `scales-setup`), a map's zoom range (`range-open`, `range-buttons`, `range-set`, `range-out`; out
of range the map is not drawn and its row bar turns gray, back in range it is drawn again, from trial shots). Next:
Oliver reviews page 4; then page 5, the DEM and hillshading: goal first.

Page 4, still unchecked: number-pad + and − zooming (Qt maps KP_Add to `Key_Plus`, measured; the canvas ignores the
keypad flag, read only); whether recording writes the scenario's leading `view` step (written by hand in
`quickstart-zoom-pan.json`).

Shot tool changes made for page 4 (uncommitted, belong to the shoot commit): mark part `point` (`lat`/`lon`, a place
on the map) offered first by Ctrl+Shift+F10 on the map; marker buttons with their own tooltips; panel tooltips rich
text through `tipOf()` and shown while the panel is inactive (`WA_AlwaysShowToolTips`). Self-test 148/148 and 60/60.

How a page is made (agreed with Oliver while making pages 1–3):

1. Goal first, as topics for the reader; Oliver reviews. Check each behaviour in code or in a trial shot before
   writing it; mark what is unchecked.
2. English text section by section, then the shots, then German: text, shot files copied from English, German
   images.
3. A page stands on its own (style guide rule): no "as on the previous page"; repeat the step or link.
4. Mark in the image what the text names (frame, disc, cursor, arrow); one image per list item, a wide image goes
   below its text (`:class: below-right`).
5. German labels from `src/qmapshack/locale/qmapshack_de.ts`; Qt's key names from `qtbase_de.qm` (Umschalt, Hoch,
   Herunter, Strg). Product and service names stay untranslated (BBBike extract service).
6. Each language gets its own crops: German words are longer. A shot's `"set": {"dockMaps.minimumWidth": 340}`
   widens the dock for the German image only; the German `.ini`s store the view name `Ansicht 1`.
7. Bases of pages 2 on store `Views\<key>\scales=1` (Square), as getting the built-in maps sets it
   (`CMapList::slotMapHonk()`, `CMapList.cpp:333`); zoom indices are then Square's: 17 levels, 0 the closest, 6 ≈ 10 m/px, 7 ≈ 19 m/px.
8. Check: `check_pages.py`, `shots.py [--lang de] unused`, `generate_html.py`, the page in a browser or a headless
   Chrome screenshot (`google-chrome --headless=new --screenshot`).

Commits on `QMS-1322`, ten groups in this order: the move to `doc/en`; shoot (C++ in `src/qmapshack/shoot/` and
`doc/tools/shots.py`, `test_shots.py`; reviewed by an agent, not Oliver); other C++ (Oliver reviews); doc build
(`conf.py`, `_static`, `.gitignore`, `doc/locales`, `check_pages.py`, `generate_html.py`); the default fixture's
vector map; one commit per page with all its languages, shot files, images and fixture; start page and overview;
plans, guides, notes and changelog; generated `doc/html` last. A change goes in with `git commit --fixup=<commit>
-- <paths>` (only those paths: a staged rename otherwise rides along), then
`GIT_SEQUENCE_EDITOR=: git rebase -i --autosquash <base>`; check `git diff --stat <old tip> HEAD` is empty before
`git push --force-with-lease`. A new page is a new commit moved after the previous page. Ask Oliver before
committing and before pushing.

The bar under the opacity slider in a map's settings is no slider: `CScaleWidget` indicates the current zoom. The
button left of it takes the current zoom as the map's maximum zoom-in, the right one as its maximum zoom-out
(tooltips in `IMapPropSetup.ui`); outside that range the map is not drawn. That belongs on page 4. Dragging a map to
reorder works (checked by Oliver).

## Page 4 goal — agreed

About: moving around the map — zooming in and out, panning, and the zoom range a map is drawn in.

The reader knows afterwards:

1. Zoom with the mouse wheel; the point under the cursor stays put. View ▸ Flip Mouse Wheel reverses the direction
   and is kept for the next start.
2. Pan by dragging the map with the left mouse button.
3. Keyboard, after clicking the map: + and − zoom (number pad unchecked), the arrow keys move the map by a quarter of
   the window.
4. Zoom works in steps; the scale at the bottom of the map shows the current one (View ▸ Show Scale, on by default).
   Setup Map View ▸ Scales picks between Logarithmic and Square; Square suits tiled online maps.
5. A map can be limited to a zoom range: the two buttons beside the zoom bar in the map's properties take the current
   zoom as the maximum zoom-in or zoom-out; outside that range the map is not drawn (unchecked).

Fixture `quickstart-zoom-pan`: page 2's online maps, OpenStreetMap on. Left out: double-clicking a workspace item to
zoom to it (page 6), map views and linked views.

## Page 3 goal — agreed

Garmin vector maps, the only vector format QMapShack reads: what they are, the NT and locked maps it can't
open, adding one (copy into `Maps`, **Reload Maps**; named after the file, `_` as space), turning it on, the
detail level and the Areas/Lines/Points switches, TYP files. Fixture `quickstart-vector-map`: page 2's online maps
plus `Tannheimer_Tal.img`. Left out: making maps, the **V** mode (page 7), zooming (page 4).

## Page 2 goal — agreed

About: the first maps in QMapShack — the built-in online maps, copied into the Maps folder from the welcome
page; OpenStreetMap turned on, a second map stacked with it and made see-through.

The reader knows afterwards:

1. How to get the built-in maps: "I want some online maps" on the welcome page, then select the Maps folder.
2. How to turn a map on and off: the button on its row in the Maps panel. First OpenStreetMap.
3. How two maps stack: the list order decides which one is on top; Move Up/Down or Shift+arrow keys.
4. How to make the upper map see-through: the opacity slider in the map's properties.

Second map: OSM_Topo (OpenTopoMap, CC-BY-SA); it shows map content at zoom indices 10, 12 and 14. Links to
other maps, if any, only under related pages. Fixture: the five built-in map definitions; `projects`, `dem`,
`poi`, `routino`, `database` marked `EMPTY`. A map turned on in a base needs its whole key set, not only
`isActive`.

Left out: offline maps and other map sources, vector maps (page 3), zooming and panning (page 4), DEMs
(page 5), map views, cache setup.

## Page 1 goal — agreed

About: the window on first start — the Welcome page in the middle, the toolbar, and the seven panels around it, each
empty with a "?" hint: left Maps, DEM, POI Collections; right Workspace, Database, Routing, Realtime.

The reader knows afterwards:

1. What each panel is for, one line each, so later pages can say "in the Maps panel". Realtime and POI Collections:
   not needed for the quick start.
2. How to arrange the window: move a panel by its title bar, dock it on any side, stack panels as tabs, resize at the
   splitters, close the ones not needed.
3. How to get a panel back: Window menu, one entry per panel and the toolbar.
4. `Ctrl+D` hides/shows all panels at once, `F11` full screen.
5. The layout is kept for the next start.
6. The home folder: *Give it a home* on the Welcome page creates `Maps`, `DEM`, `POI`, `Routino`, `BRouter`,
   `Databases`, `GPX` in a chosen folder and registers them; pages 2–6 put their data there. The Welcome page link is
   its only entry point (`CMainWindow::setupHomePath()`), and the page says it closes once a map is activated, so this
   comes before page 2.

Images start from a real first start: the page's fixture marks every part `EMPTY`.

Left out: map views/tabs (View menu); toolbar setup.
