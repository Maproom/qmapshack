# Drawing a track

This page shows how to draw a track of your own, with the router from [Setting up routing](quickstart-routing.md).
The images also use the map from [Turning on maps](quickstart-maps.md), the DEM from
[Elevation data and hillshading](quickstart-dem.md) and `Example.gpx` from
[Loading a GPX file](quickstart-load-file.md).

Images: *© OpenStreetMap contributors*; *Datenquelle: Land Tirol - data.tirol.gv.at, CC BY 4.0*;
*`Example.gpx`: fuzzybear, CC BY-SA 4.0*.

```{index} single: Track; draw
```

## Starting a track

While you draw, a bar at the top of the map holds the drawing tools. **Save as new** saves the track (see
[Saving the track](#saving-the-track)); **Abort** throws it away.

```{image} ../images/quickstart-draw-track/quickstart-draw-track-edit-bar.png
:class: below-right
```

1. ```{image} ../images/quickstart-draw-track/quickstart-draw-track-add-track.png
    :align: right
    ```

    Right-click the map where the track should start and click **Add Track**. Holding the left mouse button down
    on the map for a moment opens the same menu.

2. Move the mouse to where the track goes next and click. Each click adds a point, and a straight line joins it to
    the point before. For straight lines, **O** must be active in the bar; if another letter is active, click **O**
    first.

    ```{image} ../images/quickstart-draw-track/quickstart-draw-track-drawing.png
    :class: below-right
    ```

3. When the track is done, right-click to stop adding points. To continue drawing, move the mouse just past the first
    or last point until a red square marks it, and click.

```{index} single: Track; auto-routing
```

## Drawing along paths

1. ```{image} ../images/quickstart-draw-track/quickstart-draw-track-mode-a.png
    :align: right
    ```

    While you add points, click **A** in the bar, or press the **A** key.

2. Move the mouse to the next point and hold it still for a moment. The router finds the way along paths and roads,
    and the line follows it. It routes for the profile selected in the **Routing** panel. Click to add that part to
    the track.

    ```{image} ../images/quickstart-draw-track/quickstart-draw-track-routed.png
    :class: below-right
    ```

**A** needs a router that routes on the fly; see
[What a router does](quickstart-routing.md#what-a-router-does). Without one, its button is gray. The letter that is
active when you save or abort the track is active again when you start the next one.

```{index} single: Track; draw along a track
```

## Drawing along a track

1. ```{image} ../images/quickstart-draw-track/quickstart-draw-track-mode-t.png
    :align: right
    ```

    Right-click a loaded track where your track should start, for example the one in `Example.gpx`, and click
    **Add Track**. Then click **T** in the bar, or press the **T** key.

2. Move the mouse along that track to the next point. QMapShack shows that track in yellow, and your line follows it.
    This works only when your last point and the mouse pointer both lie on that track. Click to add that part to the
    track.

    ```{image} ../images/quickstart-draw-track/quickstart-draw-track-along-track.png
    :class: below-right
    ```

```{index} single: Track; straight lines
```

## Straight lines and vector maps

Click **O**, or press the **O** key, to draw straight lines again. **V**, or the **V** key, follows the lines of a
[vector map](quickstart-vector-map.md), such as a Garmin map. You rarely need it: the map and the routing data usually
both come from OpenStreetMap, so **A** already follows the same paths.

```{index} single: Track; edit points
```

## Correcting the track

```{image} ../images/quickstart-draw-track/quickstart-draw-track-point-tools.png
:align: right
```

Use the four buttons to the right of **Abort** to change the points of the line. From left to right, they move, add,
select a range of and delete points.

- To move a point, click the first button or press **Ctrl+M**. Click the point, move the mouse and click again to
    drop it.
- To add a point, click the second button or press **Ctrl++**. Click the line between two points: the new point
    sticks to the mouse. Click again to drop it. To add points at the start or end, move the mouse just past the first
    or last point. When a red square marks that point, click. Each click adds a point there; right-click to stop.
- To select a range, click the third button or press **Ctrl+R**, then click the first and the last point of the
    range. Two buttons appear: one deletes the points in between, the other reconnects the two points. The active
    letter in the bar decides how.
- To delete a point, click the fourth button or press **Ctrl+-**, then click the point.

When you move, add or delete a point, QMapShack joins the points next to it again. The active letter in
the bar decides how. Right-click or press **Esc** to cancel moving or adding a point.

```{image} ../images/quickstart-draw-track/quickstart-draw-track-undo-redo.png
:align: right
```

The arrow buttons at the right end of the bar undo and redo the last change.

```{index} single: Track; save
```

## Saving the track

1. ```{image} ../images/quickstart-draw-track/quickstart-draw-track-name.png
    :align: right
    ```

    Click **Save as new** in the bar. Enter a name for the track and click **OK**.

2. ```{image} ../images/quickstart-draw-track/quickstart-draw-track-project.png
    :align: right
    ```

    Select the project the track goes into, or enter a name for a new project under **New project's name**.

    For a new project, after you enter the name, under **New project is created as**, choose how QMapShack saves it.
    **\*.qms** is QMapShack's own file format, **\*.gpx** a GPX file that other programs and devices read too.
    QMapShack preselects one, so make sure it's the one you want. The third choice, **Database**, is explained in
    [Databases](quickstart-database.md).

    Click **OK**.

3. ```{image} ../images/quickstart-draw-track/quickstart-draw-track-saved.png
    :align: right
    ```

    The **Workspace** panel now lists the track under its project.

The save button on the project's row, marked in the image, means the project has changes that aren't saved
to a file. Click it to save the project.

The first time you save the project, QMapShack asks where to save it. The dialog suggests the
project's name as the file name and opens in the folder you last loaded from or saved to. If you have set up the
[home folder](quickstart-main-window.md#setting-up-the-home-folder), QMapShack points the dialog to its `GPX` folder.

On the track's row, the same symbol is not a button but a change indicator. It shows that the track has changed since the project was
last saved.

```{index} single: Track; elevation
```

## Elevations from the DEM

While you draw, the **Track Metrics** box at the top left of the map shows the track's length and how far it climbs
and descends. The elevations come from the DEM; see [Elevation data and hillshading](quickstart-dem.md). So turn the
DEM on before you draw, and draw where it has data.

When you save the track, QMapShack gives its points their elevations. Where the DEM has no data, a point gets none,
and **Track Metrics** shows **Invalid elevations!** instead of the ascent and descent.

```{image} ../images/quickstart-draw-track/quickstart-draw-track-metrics.png
:class: below-right
```
