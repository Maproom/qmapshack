# Loading a GPX file

This page shows how to load a GPX file into QMapShack, look at its tracks and waypoints and find them on the map.

Images: *© OpenStreetMap contributors*; *Datenquelle: Land Tirol - data.tirol.gv.at, CC BY 4.0*;
*`Example.gpx`: fuzzybear, CC BY-SA 4.0*.

```{index} single: GPX
```

```{index} single: File; load
```

## Loading the file

A GPX file (GPS Exchange Format) holds tracks, waypoints and routes. GPS devices, apps and route-planning websites
use it to save and share them. The images on this page show `Example.gpx`, a tour around the Aggenstein.

1. Click **File → Load GIS Data**, or press **Ctrl+L**.
2. Select the GPX file and click **Open**. To load several files at once, hold **Ctrl** and click each file.

The dialog opens in the folder you last loaded from or saved to. If you have set up the
[home folder](quickstart-main-window.md#setting-up-the-home-folder), QMapShack points the dialog to its `GPX` folder,
a good place to keep your files.

Instead of using the menu, you can drag files from your file manager onto the map.

QMapShack also loads its own `.qms` files and files from other devices and programs: `.fit`, `.tcx`, `.slf`,
`.sml`, `.log` and `.qlb`.

```{index} single: Project
```

```{index} single: Workspace
```

## The project in the workspace

QMapShack loads a file as a *project*: the tracks, routes, waypoints and areas of one file, kept together. The
**Workspace** panel (see [The main window](quickstart-main-window.md#the-window-on-first-start)) lists it under the name
stored in the file, or under the file name if there is none. QMapShack shows everything in the project on the map
right away.

1. ```{image} ../images/quickstart-load-file/quickstart-load-file-items.png
    :align: right
    ```

    Click the arrow left of the project's name. The project's tracks, routes, waypoints and areas appear below it.
    Tracks, routes and areas have an icon for their kind, and a waypoint shows its symbol.

2. Hold the mouse pointer over the name of an item (a track, route, waypoint or area). A tooltip shows a summary of the
    item.

```{index} single: Item; find on the map
```

## Finding an item on the map

```{image} ../images/quickstart-load-file/quickstart-load-file-dclick.png
:align: right
```

Double-click an item in the **Workspace** panel. The map zooms to it.

For a track, QMapShack also highlights it on the map. A box at the bottom right shows a summary of the track: length,
ascent, time, speed and more. A small plot in the bottom left corner shows the track's elevation profile.

```{image} ../images/quickstart-load-file/quickstart-load-file-zoomed.png
:class: below-right
```

```{index} single: Project; hide on the map
```

## Hiding a project on the map

1. ```{image} ../images/quickstart-load-file/quickstart-load-file-hide.png
    :align: right
    ```

    Click the eye button at the right end of the project's row. Everything in the project disappears from the map,
    and the project and its items turn gray.

2. Click the eye button again to show the project.

While a project is hidden, double-clicking one of its items doesn't zoom the map.

```{index} single: Workspace; keep for the next start
```

## Keeping the workspace

QMapShack keeps the projects in the **Workspace** panel for the next start.

To do this, it stores its own copy of each project and leaves the file you loaded unchanged. QMapShack writes a change
to the file only when you save the project.

QMapShack saves the workspace every 5 minutes and when you close the program. To change how often:

1. Click **Workspace → Setup Workspace**.
2. On the **General** tab, set the minutes beside **save workspace on exit, and every**. If you set it to 0, QMapShack
    saves the workspace only when you close it.
3. Click **OK**, confirm the message and restart QMapShack. The new setting applies only after a restart.

If you clear the check box in front of **save workspace on exit, and every**, QMapShack stops saving the workspace. At
the next start, the workspace is as it was last saved.
