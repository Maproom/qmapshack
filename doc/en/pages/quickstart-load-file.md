# Loading a GPX file

This page explains how to load a .gpx file into QMapShack, explore its tracks and waypoints, and locate them on the map.

Images: *© OpenStreetMap contributors*; *Datenquelle: Land Tirol - data.tirol.gv.at, CC BY 4.0*;
*`Example.gpx`: fuzzybear, CC BY-SA 4.0*.

```{index} single: GPX
```

```{index} single: File; load
```

## Loading the file

A `.gpx` file (GPS Exchange Format) stores tracks, waypoints, and routes. GPS devices, apps, and route-planning websites use this format to save and share them. The images on this page show `Example.gpx`, which contains a tour around the Aggenstein.

1. Click **File → Load GIS Data**, or press **Ctrl+L**.
2. Select the `.gpx` file and click **Open**. To load multiple files at once, hold down **Ctrl** while selecting each file.

The dialog opens in the folder you last loaded files from or saved files to. If you have set up your [home folder](quickstart-main-window.md#setting-up-the-home-folder), QMapShack opens the dialog in its `GPX` folder, a convenient place to store your files.

Instead of using the menu, you can simply drag files from your file manager onto the map.

QMapShack can also load its own `.qms` files, as well as files from other devices and applications in the following formats: `.fit`, `.tcx`, `.slf`, `.sml`, `.log`, and `.qlb`.

```{index} single: Project
```

```{index} single: Workspace
```

## The project in the workspace

QMapShack loads each file as a *project*, keeping all its tracks, routes, waypoints, and areas together. The **Workspace** panel lists the project by the name stored in the file, or by the file name if no name is stored. See [The main window](quickstart-main-window.md#the-window-on-first-start). All items in the project are immediately displayed on the map.

1. ```{image} ../images/quickstart-load-file/quickstart-load-file-items.png
    :align: right
    ```

    Click the arrow to the left of the project name to expand the project. Its tracks, routes, waypoints, and areas are listed below. Tracks, routes, and areas are marked with an icon indicating their type, while waypoints display their assigned symbols.
2. Hover the mouse pointer over an item's name (track, route, waypoint, or area) to display a tooltip with a summary of the item.

```{index} single: Item; find on the map
```

## Finding an item on the map

```{image} ../images/quickstart-load-file/quickstart-load-file-dclick.png
:align: right
```

Double-click an item in the **Workspace** panel to zoom the map to it.

For a track, QMapShack also highlights it on the map. A summary box in the bottom-right corner shows the track's length, total ascent, duration, speed, and other statistics. A small plot in the bottom-left corner displays its elevation profile.

```{image} ../images/quickstart-load-file/quickstart-load-file-zoomed.png
:class: below-right
```

```{index} single: Project; hide on the map
```

## Hiding a project on the map

1. ```{image} ../images/quickstart-load-file/quickstart-load-file-hide.png
    :align: right
    ```

    Click the eye button at the right end of the project's row. All items in the project disappear from the map, and the project and its contents are grayed out.
2. Click the eye button again to make the project visible on the map.

While a project is hidden, double-clicking one of its items has no effect on the map's zoom level.

```{index} single: Workspace; keep for the next start
```

## Saving the workspace

QMapShack saves the projects in the **Workspace** panel to a special file. They are restored the next time you start the application.

To do this, QMapShack keeps its own copy of each project, leaving the original file unchanged. Changes are written to the file only when you save the project.

QMapShack saves the workspace every five minutes and whenever you close the application. To change the save interval:

1. Click **Workspace → Setup Workspace**.
2. On the **General** tab, set the interval in minutes next to **Save workspace on exit, and every**. Set it to `0` to save the workspace only when you close QMapShack.
3. Click **OK**, confirm the message, and restart QMapShack. The new setting takes effect only after restarting the application.

If you clear the check box next to **Save workspace on exit, and every**, QMapShack stops saving the workspace. The next time you start the application, the workspace is restored to its last saved state.