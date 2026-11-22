# Adding waypoints

This page shows how to add a waypoint to the map. The images use the map from [Turning on maps](quickstart-maps.md),
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

    Right-click the map where the waypoint should go and click **Add Waypoint**. Holding the left mouse button down
    on the map for a moment opens the same menu.

2. ```{image} ../images/quickstart-waypoints/quickstart-waypoints-project.png
    :align: right
    ```

    Select the project the waypoint goes into, or enter a name for a new project under **New project's name**.

    For a new project, after you enter the name, under **New project is created as**, choose how QMapShack saves it.
    **\*.qms** is QMapShack's own file format, **\*.gpx** a GPX file that other programs and devices read too.
    QMapShack preselects one, so make sure it's the one you want. The third choice, **Database**, is explained in
    [Databases](quickstart-database.md).

    Click **OK**.

3. The waypoint's details window opens. It shows the waypoint's name, symbol, position, elevation and time.

    ```{image} ../images/quickstart-waypoints/quickstart-waypoints-details.png
    :class: below-right
    ```

    The **Elevation** comes from the DEM; see [Elevation data and hillshading](quickstart-dem.md).

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

    Press **Esc** to close the window. The waypoint now shows on the map with its name.

7. ```{image} ../images/quickstart-waypoints/quickstart-waypoints-saved.png
    :align: right
    ```

    The **Workspace** panel now lists the waypoint under its project.

The save button on the project's row, marked in the image, means the project has changes that aren't saved
to a file. Click it to save the project.

The first time you save the project, QMapShack asks where to save it. The dialog suggests the
project's name as the file name and opens in the folder you last loaded from or saved to. If you have set up the
[home folder](quickstart-main-window.md#setting-up-the-home-folder), QMapShack points the dialog to its `GPX` folder.

On the waypoint's row, the same symbol is not a button but a change indicator. It shows that the waypoint has changed since the project
was last saved.

The first waypoint is called `wpt`. A new waypoint gets the symbol you chose last and the name you gave last. If that
name ends in a number, QMapShack counts it up: after `Hut 3` comes `Hut 4`.
