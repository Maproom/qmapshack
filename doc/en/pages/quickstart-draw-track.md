# Drawing a track

This page shows you how to draw your own track using the router described in [Setting up routing](quickstart-routing.md).
The images also show 

- the map used in [Enabling on maps](quickstart-maps.md), 
- the DEM used in
[Elevation data and hillshading](quickstart-dem.md) 
- `Example.gpx` used in
[Loading a GPX file](quickstart-load-file.md).

Images: *© OpenStreetMap contributors*; *Datenquelle: Land Tirol - data.tirol.gv.at, CC BY 4.0*;
*`Example.gpx`: fuzzybear, CC BY-SA 4.0*.

```{index} single: Track; draw
```

## Starting a track

While you draw, a toolbar at the top of the map provides the drawing tools.  **Save as new** saves the track (see
[Saving the track](#saving-the-track)). **Abort** discards it.

```{image} ../images/quickstart-draw-track/quickstart-draw-track-edit-bar.png
:class: below-right
```

1. ```{image} ../images/quickstart-draw-track/quickstart-draw-track-add-track.png
    :align: right
    ```

    Right-click the map at the desired starting point and select **Add Track**. You can also open the same menu by pressing and holding the left mouse button on the map.
2. Move the mouse to the next point along the track and click to add it. Each click adds a point, which is connected to the previous one by a straight line. To draw straight lines, **O** must be active in the toolbar. If another letter is active, click **O** first.

    ```{image} ../images/quickstart-draw-track/quickstart-draw-track-drawing.png
    :class: below-right
    ```
3. When you've finished drawing the track, right-click to stop adding points. To continue drawing, move the mouse just beyond the first or last point until a red square appears, then click.

```{index} single: Track; auto-routing
```

## Drawing along paths

1. ```{image} ../images/quickstart-draw-track/quickstart-draw-track-mode-a.png
    :align: right
    ```

    While you add points, click **A** in the toolbar, or press the **A** key.
2. Move the mouse to the next point and pause briefly. The router calculates a route along paths and roads, and the track follows them. The route is calculated using the profile selected in the **Routing** panel. Click to add the routed section to the track.

    ```{image} ../images/quickstart-draw-track/quickstart-draw-track-routed.png
    :class: below-right
    ```

**A** requires a router that can calculate routes on the fly. See [What a router does](quickstart-routing.md#what-a-router-does). If no such router is available, the button is grayed out. The active mode when you save or abort a track remains active when you start drawing the next track.

```{index} single: Track; draw along a track
```

## Drawing along a track

1. ```{image} ../images/quickstart-draw-track/quickstart-draw-track-mode-t.png
    :align: right
    ```

    Right-click a loaded track at the point where you want your new track to start, for example, the track in `Example.gpx`. Select **Add Track**, then click **T** in the toolbar or press the **T** key.
2. Move the mouse along the loaded track to the next point. QMapShack highlights the track in yellow, and your new track follows it. This works only if both your last point and the mouse pointer are on the loaded track. Click to add the highlighted section to your track.


    ```{image} ../images/quickstart-draw-track/quickstart-draw-track-along-track.png
    :class: below-right
    ```

```{index} single: Track; straight lines
```

## Straight lines and vector maps

Click **O** or press the **O** key to switch back to drawing straight lines.

Click **V** or press the **V** key to follow the lines of a vector map, such as a Garmin map. You rarely need this mode because both the map and the routing data usually come from OpenStreetMap. In that case, **A** already follows the same paths.

```{index} single: Track; edit points
```

## Editing the track

```{image} ../images/quickstart-draw-track/quickstart-draw-track-point-tools.png
:align: right
```

Use the four buttons to the right of **Abort** to edit the line's points. From left to right, the buttons let you move, add, select a range of, and delete points.

- To move a point, click the first button or press **Ctrl+M**. Click the point you want to move, move the mouse to the new position of the point, and click again to place it.
- To add a point, click the second button or press **Ctrl++**. Click the line between two existing points. The new point attaches to the mouse pointer. Click again to place it.

    To add points at the beginning or end of the line, move the mouse just beyond the first or last point. When a red square appears to mark the point, click to add a new point. Each click adds another point. Right-click to stop adding points.
- To select a range of points, click the third button or press **Ctrl+R**. Then click the first and last points of the range. Two buttons appear: one deletes the selected points, while the other connects the two end points directly. The active mode in the toolbar determines how the points are reconnected.
- To delete a point, click the fourth button or press **Ctrl+-**, then click the point you want to delete.

When you move, add, or delete a point, QMapShack reconnects the adjacent points. The active mode in the toolbar determines how they are connected. Right-click or press **Esc** to cancel moving or adding a point.

```{image} ../images/quickstart-draw-track/quickstart-draw-track-undo-redo.png
:align: right
```

The arrow buttons at the right end of the toolbar undo and redo the last change.

```{index} single: Track; save
```

## Saving the track

1. ```{image} ../images/quickstart-draw-track/quickstart-draw-track-name.png
    :align: right
    ```

    Click **Save as new** in the toolbar. Enter a name for the track and click **OK**.
2. ```{image} ../images/quickstart-draw-track/quickstart-draw-track-project.png
    :align: right
    ```

    Select the project to which you want to add the track, or enter a name for a new project under **New project's name**.

    For a new project, enter a name and then choose how QMapShack should save it under **New project is created as**. 
    
    - **\*.qms** is QMapShack's own file format
    - **\*.gpx** is the .gpx format, which other programs and devices can also read. 
    
    QMapShack preselects a format, so make sure it's the one you want. The third option, **Database**, is explained in [Databases](quickstart-database.md).

    Click **OK**.
3. ```{image} ../images/quickstart-draw-track/quickstart-draw-track-saved.png
    :align: right
    ```

    The **Workspace** panel now lists the track under the selected project.

The save button in the project's row, highlighted in the image, indicates that the project has unsaved changes. Click it to save the project to a file.

The first time you save a project, QMapShack asks where to save it. The dialog suggests the project name as the file name and opens in the folder you last loaded a project from or saved one to. If you have set up the [home folder](quickstart-main-window.md#setting-up-the-home-folder), the dialog opens in its `GPX` subfolder.

In the track's row, the same symbol serves as a change indicator rather than a button. It shows that the track has been modified since the project was last saved.

```{index} single: Track; elevation
```

## Elevations from the DEM

While you draw, the **Track Metrics** box in the top-left corner of the map displays the track length and its total ascent and descent. Elevation data comes from the DEM. See [Elevation data and hillshading](quickstart-dem.md). Make sure the DEM is enabled before you start drawing, and draw in areas where elevation data is available.

When you save the track, QMapShack assigns elevations to its points based on the DEM. Points in areas without DEM data have no elevation. In this case, **Track Metrics** displays **Invalid elevations!** instead of the total ascent and descent.

```{image} ../images/quickstart-draw-track/quickstart-draw-track-metrics.png
:class: below-right
```
