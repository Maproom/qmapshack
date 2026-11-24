# Quick start plan

Step 7 of `documentation-plan.md`: the quick start as the first real page. Done: all twelve pages, English and
German, are on `dev` (`doc/en/pages/`, `doc/de/pages/`). Decisions are recorded here only once Oliver has made them.

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
- GPS device: covered. The shot tool fakes one: a fixture part `device/` is added as a `CDeviceGarmin` (USB mass
  storage), see `src/qmapshack/shoot/CLAUDE.md`. Page 12 shows a GPSMAP 64s (mass storage, GPX; the 67 is MTP only).
- Topic list, in this order:
  1. The main window: canvas and the panels around it
  2. Turning on a map (BEV km50, OSM), stacking maps and opacity
  3. Turning on a vector map: detail level and TYP
  4. Zooming and panning
  5. Turning on a DEM and hillshading
  6. Loading a file (`Example.qms`)
  7. Setting up routing: Routino in the steps and images (fixture data); BRouter named as the easier option
     for the reader's own setup, with its installation wizard (Oliver: most users want BRouter)
  8. Drawing a track: auto-routing (**A**), along an existing track (**T**), straight (**0**); **V** one sentence
     only (map and routing data usually both come from OSM)
  9. Adding a waypoint; POIs are left out (page 1: the quick start does not need POI Collections)
  10. Databases: creating one, its folder structure, adding data directly, exporting, the intended use with the
      workspace (`quickstart-database.md`, before the saving page; Oliver)
  11. Saving: projects as files and in a database
  12. Sending to the device

  Installation is a link to the release page.
- Each page is written with Oliver in three steps: Claude summarises the page's goal (what it is about, what the
  reader knows afterwards), Oliver reviews and changes it; then the page is written section by section; screenshots
  and marks are decided per section on the way.
- Page layout (steps numbered, as the style guide says; other lists bullets): an action is a list item holding a right-aligned `{image}` of the detail it names, cropped to at most
  300 px wide, no wider than its content, and as low as its marks allow; `doc/_static/qms-layout.css` keeps each image beside its bullet, with a
  border and a line between items. An image too wide to sit
  beside its text goes below it with `:class: below-right`. Images in table cells are not allowed.
- Claude may write scenarios and shot entries by hand instead of recording them; a person must still be able to
  take, switch to and re-record them in the shot tool.
- One page per topic, linked in order from a quick start overview page, so each page can have its
  own base in the shot tool. Files in `doc/<lang>/pages/`: `quickstart.md` (overview), `quickstart-main-window.md`,
  `quickstart-maps.md`, `quickstart-vector-map.md`, `quickstart-zoom-pan.md`, `quickstart-dem.md`,
  `quickstart-load-file.md`, `quickstart-routing.md`, `quickstart-draw-track.md`, `quickstart-waypoints.md`,
  `quickstart-database.md`, `quickstart-save.md`, `quickstart-device.md`.


## Open

- Marks in images: `shot-marks-plan.md`.
- Device rows in scenarios: a replay expands the device by its name (`GPSMAP 64s (GARMIN)`, trial run), but recording
  writes no name for it (`CShotAddress::itemPathOf()`); a device project's menu in a replay is untested.
- Code bug, not fixed: **Save as...** with the QLB filter saves nothing and reports nothing (`IGisProject::saveAs()`
  offers `filedialogFilterQLB` but has no branch for it).
- Page 6 (en, de), section 5: "save the project" does not link to page 11 (`quickstart-save.md`) yet.
- Unchecked, read in code only:
  - Page 4: number-pad + and − zoom (Qt maps KP_Add to `Key_Plus`, measured; the canvas ignores the keypad flag);
    whether recording writes a scenario's leading `view` step (written by hand in `quickstart-zoom-pan.json`).
  - Page 11: the save button disappears after saving; Save as another type keeps it.
  - Page 10: the database dialog opens in the home folder's `Databases`; a new database shows in the panel after **OK**.
  - Page 12: sending and reading recordings in a run; the wiki's "device projects cannot be copied" against
    **Copy to...** in the device project's menu.

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

Future page (Oliver): where to find DEM data (and maps) and how to bring it into a usable format; the quick start
has no ready-made source for either. Link it from page 5 (and page 3) once it exists.

Future chapters (Oliver): one per GIS item (waypoint, track, route, area), outside the quick start; the quick start only
creates items. To cover there: a moved waypoint keeps its elevation (`setPosition()`), the on-map bar's eighth button
takes it from the DEM (seen in a replay). Projects: an active project (row button, `setUserFocus()`) takes new items
without the project dialog (`selectProject(false)`); left out of the quick start.

Future page (Oliver): troubleshooting, linked from the quick start once it exists. First entry: a broken workspace
is fixed by deleting `workspace.db` (`CGisListWks.cpp:109`, in `userDataPath()`: `~/.config/QLandkarte/` on Linux and
Windows, `AppLocalDataLocation` on macOS; Windows and macOS paths unchecked). Page 6 stays without it.

Better on concept pages (plan step 8): the `CARTO` folder layout (l. 84–157), building a VRT, DEM
online vs local, database folder colors (l. 453–457), map views (l. 301–359).

## Spanish chapters vs the default fixture

| Chapter | Fixture |
|---|---|
| Activate maps, stacking, opacity | BEV km50 (offline), OSM (online) |
| Activate a DEM, hillshading | Tirol DGM 5 m |
| Load a GPX file | `Example.qms` |
| Draw a track, automatic routing | Routino Tannheimer Tal |
| Waypoint (POI part left out) | none |
| Projects and databases, copy between them | `Example.qms`, `Example.db` |
| Zoom and pan | any |
| Vector map, TYP, **V** mode | `Tannheimer_Tal.img` (BBBike, OpenTopoMap style) |
| GPS device | none; to be faked |
| Installation | not a fixture topic |

## How a page is made

Agreed with Oliver while making pages 1–3; applies to any new page.

1. Goal first, as topics for the reader; Oliver reviews. Check each behaviour in code or in a trial shot before
   writing it; mark what is unchecked.
2. English text section by section, then the shots, then German: text, shot files copied from English, German
   images.
3. A page stands on its own (style guide rule): no "as on the previous page"; repeat the step or link.
4. Mark in the image what the text names (frame, disc, cursor, arrow); a menu entry is highlighted (`active`), never
   framed (Oliver); one image per list item, a wide image goes
   below its text (`:class: below-right`).
5. German labels from `src/qmapshack/locale/qmapshack_de.ts`; Qt's key names from `qtbase_de.qm` (Umschalt, Hoch,
   Herunter, Strg). Product and service names stay untranslated (BBBike extract service).
6. Each language gets its own crops: German words are longer. A shot's `"set": {"dockMaps.minimumWidth": 340}`
   widens the dock for the German image only; the German `.ini`s store the view name `Ansicht 1`.
7. Bases of pages 2 on store `Views\<key>\scales=1` (Square), as getting the built-in maps sets it
   (`CMapList::slotMapHonk()`, `CMapList.cpp:333`); zoom indices are then Square's: 17 levels, 0 the closest, 6 ≈ 10 m/px, 7 ≈ 19 m/px.
8. From page 6 on, a base copies page 6's `[MainWindow] state`: every panel on the left as two tab stacks
   (Workspace|Routing|Realtime over Maps|DEM|Database|POI, Realtime and POI closed, Maps shown), dock area at its
   minimum width (cut tab labels accepted), toolbar at the top. Pages 1–5 keep the default layout. The default
   fixture's DEM is on with hillshading (page 5 shows it): the page fixture's `dem` part is not `EMPTY`, the base
   carries the default `shots.ini`'s `dem2` keys (opacity 20).
9. Check: `check_pages.py`, `shots.py [--lang de] unused`, `generate_html.py`, the page in a browser or a headless
   Chrome screenshot (`google-chrome --headless=new --screenshot`).
10. Terms as on pages 8–11: "save button" / "Speichern-Knopf", "change indicator" / "Änderungsanzeige". MyST
    anchors keep umlauts (`#höhen-aus-dem-dem`).

## Page 7 goal — agreed

About: preparing a router so QMapShack can draw tracks along paths and roads (page 8).

The reader knows afterwards:

1. What a router does; drawing along while clicking needs on-the-fly routing: offline routers can, online ones
   can't (`IMouseEditLine.cpp:310–312`).
2. The Routing panel's list: **Routino (offline)**, **BRouter (online)**/**BRouter (offline)** (`CRouterSetup.cpp:34–35`,
   `CRouterBRouter.cpp:156–161`).
3. BRouter, the easier choice for one's own setup, one section without steps: **Setup** → **BRouter Setup** wizard,
   **BRouter-Web (online)** or **local Installation** with a Java executable; only local draws along
   (`CRouterBRouter.cpp:193–194`); the home folder presets its `BRouter` folder (`CMainWindow.cpp:699`).
4. Routino in steps with images: data into the home folder's `Routino` folder (registered, `CMainWindow.cpp:698`);
   **Routino (offline)**, **Database**, **Profile**; data made with **Tool → Create Routino Database**, one sentence.

Fixture: page 6's maps and DEM, the default Routino data, no project. Left out: routing a route item, BRouter
profiles/alternatives, **Mode**, **Language**, **Surface**. Unchecked: a copied database showing up without restart,
the panel with fixture data, the wizard in an image.

## Page 6 goal — agreed

About: loading a GPX file, seeing what is in it, finding it on the map. Example `Example.gpx`
(`doc/shots/fixtures/default/GPX/`).

The reader knows afterwards:

1. Loading: **File → Load GIS Data** (`Ctrl+L`, several files at once) or dragging files onto the map; the other
   formats in one sentence. The Welcome page link is left out: the page closes once a map is on.
2. The file becomes a project in the Workspace panel, its items under it (what hovering shows: unchecked).
3. Double-clicking an item zooms the map to it (`CGisListWks::slotItemDoubleClicked()`).
4. The project's checkbox shows and hides it (unchecked).
5. The workspace is kept for the next start (saved on exit and every 5 min, `CGisListWks.h:255`).

Fixture: page 2's online maps, OpenStreetMap on, no project. Left out: one project per key, editing items, the info
box, databases (page 10), saving (page 11), devices (page 12).

## Page 5 goal — agreed

About: elevation data (a DEM): what QMapShack does with it, adding the example DEM, turning it on, hillshading.

The reader knows afterwards:

1. A DEM holds the terrain's elevation. QMapShack gives an elevation for any point with it: under the mouse pointer
   the status bar shows **Ele.:** and the slope (`CMainWindow::slotMousePosition()`); it adds or replaces the
   elevation of tracks, waypoints, routes and areas (later pages, one sentence and a link); it shades the terrain.
2. QMapShack reads `.vrt` and `.wcs` (`CDemDraw::supportedFormats`); a GeoTIFF needs a VRT, **Tool → VRT Builder**,
   one sentence.
3. Adding one: the `DEM` folder of the home folder, or **File → Setup DEM Paths**.
4. Turning it on: the button on its row in the **DEM** panel.
5. Hillshading: the row's settings, **Hillshading** and its light angle slider, the opacity (the DEM draws over the
   map).
6. A DEM has a zoom range like a map's: link to page 4.

Fixture: page 2's online maps, OpenStreetMap on, plus the default fixture's `tirol_dgm5m.vrt`; images in Tirol (no
data on the German side). Left out: Slope Shading, the Slope tint, Elevation Shading and Limit, online DEMs (WCS),
building VRTs, overviews, track elevations (page 8).

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
open, adding one (copy into `Maps`, shows up after a few seconds; named after the file, `_` as space), turning it on, the
detail level and the Areas/Lines/Points switches, TYP files. Fixture `quickstart-vector-map`: page 2's online maps
plus `Tannheimer_Tal.img`. Left out: making maps, the **V** mode (page 8), zooming (page 4).

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

## Page 12 (device) goal — agreed

About: data onto the GPS device and recordings off it, with a Garmin connected as USB mass storage
(`quickstart-device.md`).

The reader knows afterwards:

1. Connecting: device support is on by default (`CGisListWks.cpp:261`), switched in **Workspace → Setup Workspace**,
   "Enable device support" (`ISetupWorkspace.ui:132`). A device shows in the workspace as a row of its own, model and
   folder name, with the projects found on it below. Prerequisites per the wiki DocGisDevices (not re-read): Linux
   UDisks2, macOS "Removable Volumes" permission. MTP devices and other makes: link to the wiki.
2. Sending: project menu **Send to Devices** (`CGisListWks.cpp:150`), enabled only while a device is connected
   (`CGisListWks.cpp:1267`); with more than one device the dialog `CSelDevices` asks which (`CGisListWks.cpp:1925`).
   Sending again replaces the device's copy (`IDevice.cpp:288–302`).
3. Reading recordings: the device's GPX files become projects below the device, named after the file with `_` as space
   (`CGpxProject.cpp:54`); items there cannot be changed (wiki; **Delete** disabled, `CGisListWks.cpp:1316`). To keep or
   change a recording, **Copy to...** puts it into a workspace project (`CGisListWks.cpp:168–169, 1029`), saved as on
   page 11.

Fixture `quickstart-device`: page 11's plus `device/`: a GPSMAP 64s (hand-written `GarminDevice.xml`, placeholder part
number) holding `Garmin/GPX/Track_2025-06-14 101530.gpx`, the Example track moved to that day (file name pattern
guessed). Left out (Oliver): automatic sync; also **Update Project on Device**, **Delete**, MTP, TwoNav, SD card, drag
and drop, the Archive. Unchecked: points 2–3 in a run; the wiki's "device projects cannot be copied" against
**Copy to...** in the device project's menu.

## Page 10 (databases) goal — agreed

About: keeping many projects in one place, in a QMapShack database (`quickstart-database.md`, before the saving page).

The reader knows afterwards:

1. Creating one: **Database** panel → **Add Database**, **Name**, **SQLite**, "Add new database." (`ISetupDatabase.ui`).
2. Folder structure: **Group** (not loadable, `CDBFolderGroup.cpp:22`), **Project**, **Other**; **Add Folder**.
3. Adding data directly: **Import from Files...** (`CGisListDB.cpp:957–994`), one **Project** folder per file.
4. Exporting: **Export to GPX...**, folders as directories, projects as GPX files (`CExportDatabaseThread.cpp:91–147`).
5. Use with the workspace: load button loads a project (`IDBFolder.cpp:329`), **Save** writes back; the intended use is
   discussed with Oliver when that section is written. Agreed to include, briefly (source: wiki `DocGisDatabase` and
   subpages, not yet checked in code): the database panel only loads, unloads and deletes, all editing is in the
   workspace and saved back; a folder's load button loads its items, not subfolders, and single items load on their own;
   an item is stored once and linked into folders, so an edit shows everywhere, **Clone** makes an independent copy;
   removing an item in the workspace does not delete it in the database; an item no folder links to goes to
   **Lost & Found**, deleted for good only there.

Fixture: page 9's plus the default `Example.db` (group "Projects", project "Einstein"). Left out: MySQL,
**Search Database**, **Copy Folder**/**Move Folder**, drag and drop. All points read in code only, none run yet.


Folder types (Oliver): **Project** and **Other** are the same to the program; by convention an **Other** folder is a
subfolder of a project, e.g. the trip's recorded data in the project (green folder) and the planned track in an
**Other** subfolder. Any other use is fine. On export an **Other** folder's file name is prefixed with its parent's
name (`CExportDatabaseThread.cpp:139–144`).

## Page 11 (saving) goal — agreed

About: keeping one's work, as a project file or in a database.

The reader knows afterwards:

1. The workspace is no storage: saving writes a project to a file or database. A changed project shows a save button
   on its row ("Item is changed and needs to be saved."), a click saves (`CWksItemDelegate.cpp:155, 763, 971`).
2. Files: project menu **Save** / **Save as...**; a never-saved project asks for a file name (`IGisProject.cpp:358–385`);
   **Close** asks to save a changed project (`IGisProject.cpp:189–207`). QMS keeps all data QMapShack uses and produces,
   e.g. the change history to revert changes; GPX only partly (history shown, no data to revert); **Save as GPX 1.1 w/o
   ext...** drops all extra data (Oliver). QMS for one's own projects, GPX to exchange; QLB, TCX and the Save/Save as basics left out.
3. Saving into a database, building on the database page before it. Ways a project gets into a database (Oliver):
   **Add empty project**, a project folder in the database loaded by its load button, **Copy to...**, a new item's project
   dialog. Drag and drop in the workspace left out.

Fixture: page 9's (online maps, Example.gpx, DEM on), page 6's layout, plus the default `Example.db`. Left out:
**Autom. Save**, drag and drop in the workspace, devices.

## Page 9 goal — agreed

About: adding a waypoint on the map.

The reader knows afterwards:

1. Adding: right-click the map → **Add Waypoint** (`CMouseNormal.cpp:393`); into the focused project, else a project
   dialog (`CGisWorkspace.cpp:262`); the details dialog opens at once (`CGisItemWpt::editInitial()`); the name counts
   up from the last (`getLastName()`), the last symbol is used again (`Waypoint/lastIcon`); elevation from the active
   DEM (`CGisItemWpt.cpp:70`).

Project dialog shown and worded as on page 8 (select a project or name a new one, **OK**); the active-project button
left out, as on page 8. Name counting only for a last name ending in digits (`CGisItemWpt.cpp:209–223`); Shots type a new
project, **My waypoints**, as page 8 does.

Fixture: page 8's (online maps, Example.gpx, DEM on), page 6's layout, no POI file. Left out: POIs, waypoints from the
elevation profile, radius circle, **Proj**, tags, bubble, geocaches. Unchecked: line 148 is the left-click path, the
project dialog, name counting in a run.

## Page 8 goal — agreed

About: drawing a track with the router from page 7.

The reader knows afterwards:

1. Starting: right-click the map → **Add Track** (`CMouseNormal.cpp:395`); left click drops points, right click
   stops (`IMouseEditLine.cpp:238`).
2. The routing modes: **A** auto-routing (needs an on-the-fly router, else disabled, `IMouseEditLine.cpp:327–338`),
   **T** along a loaded track (Example.gpx), **0** straight; **V** one sentence (map and routing data usually
   both OSM).
3. Saving: **Save as new** asks for a name and a project (`CMouseEditTrk.cpp:84`).
4. Elevations: a drawn line takes them from the active DEMs (`IMouseEditLine.cpp:397, 470`; `CDemDraw.cpp:331`);
   page 5's bullet on adding elevations links here.
