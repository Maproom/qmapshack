# Zooming and panning

This page shows how to move around the map: zoom in and out, move the map, and limit the zoom range of a map.

Images: *© OpenStreetMap contributors*.

```{index} single: Map; zoom
```

## Zooming with the mouse wheel

- ```{image} ../images/quickstart-zoom-pan/wheel-before.png
    :align: right
    ```

    Point the mouse at the place you want to look at closer, here the village of Grän.

- ```{image} ../images/quickstart-zoom-pan/wheel-in.png
    :align: right
    ```

    Turn the mouse wheel forward to zoom in, backward to zoom out. Each step of the wheel is one zoom level. The
    place under the mouse pointer stays where it is: here two steps forward, and Grän is still under the pointer.

- If the direction feels wrong to you, click **View → Flip Mouse Wheel**. QMapShack keeps this setting for the
    next start.

```{index} single: Map; move
```

## Moving the map

- ```{image} ../images/quickstart-zoom-pan/drag.png
    :align: right
    ```

    Press the left mouse button on the map, hold it and move the mouse. The map moves with the pointer; release the
    button when the part you want to see is in view. Here the map is dragged to the right, and Grän moves along with
    the pointer.

```{index} single: Map; keyboard
```

## Using the keyboard

Click the map once, so that the keys go to it. Then press:

| Key | What it does |
|---|---|
| **+** | Zoom in one step, around the middle of the map |
| **−** | Zoom out one step, around the middle of the map |
| **Up** | Move the map down by a quarter of the window, to show what lies above |
| **Down** | Move the map up by a quarter of the window, to show what lies below |
| **Left** | Move the map right by a quarter of the window, to show what lies to the left |
| **Right** | Move the map left by a quarter of the window, to show what lies to the right |

**+** and **−** work on the main keys and on the number pad.

```{index} single: Map; scale
```

## Zoom steps and the scale

- ```{image} ../images/quickstart-zoom-pan/scale-bar.png
    :align: right
    ```

    The scale bar at the bottom right of the map shows how long a distance is at the current zoom. To hide it or show
    it again, click **View → Show Scale**.

- QMapShack zooms in fixed steps. To choose them, click **View → Setup Map View** and pick one under **Scales**.
    **Logarithmic** steps through round scales, such as 1, 1.5, 2, 3, 5, 7 and 10 meters per pixel.
    **Square** doubles or halves the scale with each step, as the tiles of online maps do, so they are drawn sharp.
    With **Logarithmic**, online maps are scaled and look blurred. If you got the built-in online maps as in
    [Turning on maps](quickstart-maps.md), QMapShack has already chosen **Square**.

    ```{image} ../images/quickstart-zoom-pan/scales-setup.png
    :class: below-right
    ```

```{index} single: Map; zoom range
```

## Limiting a map to a zoom range

A map can be limited to a range of zoom steps. Outside that range, QMapShack does not draw it. A detailed map, for
example, can then appear only when you zoom in.

- ```{image} ../images/quickstart-zoom-pan/range-open.png
    :align: right
    ```

    Click the arrow left of the map's name in the **Maps** panel. Its settings open below it.

- ```{image} ../images/quickstart-zoom-pan/range-buttons.png
    :align: right
    ```

    The bar below the opacity slider shows the current zoom as a small box. The button left of the bar takes the
    current zoom as the closest zoom the map is drawn at. The button right of the bar takes it as the farthest.

- ```{image} ../images/quickstart-zoom-pan/range-set.png
    :align: right
    ```

    Click the right button. The bar now shows in green the zoom steps at which the map is drawn.

- Zoom out one step further. The map is no longer drawn, and the bar between the map's name and its button on the row
    turns gray. Zoom in again, and the map comes back.

    ```{image} ../images/quickstart-zoom-pan/range-out.png
    :class: below-right
    ```

- To remove a limit, click its button again.
