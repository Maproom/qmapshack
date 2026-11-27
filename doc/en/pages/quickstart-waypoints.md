# Adding waypoints

This page shows how to add a waypoint to a map. The images on this page use the map from [Enabling maps](quickstart-maps.md),
the DEM from [Elevation data and hillshading](quickstart-dem.md) and `Example.gpx` from
[Loading a GPX file](quickstart-load-file.md).

Images: *© OpenStreetMap contributors*; *Datenquelle: Land Tirol - data.tirol.gv.at, CC BY 4.0*;
*`Example.gpx`: fuzzybear, CC BY-SA 4.0*.

```{index} single: Waypoint; add
```

## Adding a waypoint

1. ```{image} ../images/quickstart-waypoints/quickstart-waypoints-add-waypoint.png
    :align: right
    ```

    Right-click the map at the desired waypoint location and select **Add Waypoint**. You can also open the same menu by pressing and holding the left mouse button on the map.
2. ```{image} ../images/quickstart-waypoints/quickstart-waypoints-project.png
    :align: right
    ```

    Select the project you want to add the waypoint to, or enter a name under **New project's name** to create a new project.
   
    For a new project, enter its name and choose how QMapShack should save it under **New project is created as**:

    - **\*.qms** is QMapShack's own file format.
    - **\*.gpx** is a .gpx file format that other programs and devices can also read.

    QMapShack preselects one of these formats, so make sure it matches your needs.

    The third option, **Database**, is explained in [Databases](quickstart-database.md).

    Click **OK**.
3. The waypoint details window opens, showing the waypoint's name, symbol, position, elevation, and time.

    ```{image} ../images/quickstart-waypoints/quickstart-waypoints-details.png
    :class: below-right
    ```

    The Elevation value is obtained from the DEM. See [Elevation data and hillshading](quickstart-dem.md).
4. Click the name, type a new one and press **Enter**.

    ```{image} ../images/quickstart-waypoints/quickstart-waypoints-named.png
    :class: below-right
    ```
5. ```{image} ../images/quickstart-waypoints/quickstart-waypoints-symbol.png
    :align: right
    ```

    Click the symbol left of the name to choose another one.
6. ```{image} ../images/quickstart-waypoints/quickstart-waypoints-closed.png
    :align: right
    ```

    Press **Esc** to close the window. The waypoint now appears on the map, with its name displayed next to it.
7. ```{image} ../images/quickstart-waypoints/quickstart-waypoints-saved.png
    :align: right
    ```

    The **Workspace** panel now shows the waypoint listed under its project.

The save button in the project's row, highlighted in the image, indicates that the project has unsaved changes. Click it to save the project to its file.

The first time you save a project, QMapShack asks where to save it. The dialog suggests the project name as the file name and opens in the folder you last loaded a project from or saved one to. If you have set up the [home folder](quickstart-main-window.md#setting-up-the-home-folder), QMapShack opens the dialog in its `GPX` subfolder.

In the waypoint's row, the same symbol is not a button but serves as a change indicator. It shows that the waypoint has been modified since the project was last saved.

The first waypoint is named `wpt`. Each new waypoint uses the symbol and name you last selected. If the name ends with a number, QMapShack automatically increments it. For example, after `Hut 3` comes `Hut 4`.