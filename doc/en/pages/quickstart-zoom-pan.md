# Zooming and panning

This page shows how to move around the map: zoom in and out, move the map, and limit the zoom range of a map.

Images: *© OpenStreetMap contributors*.

```{index} single: Map; zoom
```

## Zooming with the mouse wheel

1. ```{image} ../images/quickstart-zoom-pan/quickstart-zoom-pan-wheel-before.png
    :align: right
    ```

    Point the mouse at the place you want to look at more closely, here the village of Grän.

2. ```{image} ../images/quickstart-zoom-pan/quickstart-zoom-pan-wheel-in.png
    :align: right
    ```

    Turn the mouse wheel forward to zoom in, backward to zoom out. Each step of the wheel is one zoom step. The place
    under the mouse pointer stays where it is. Here the wheel turned two steps forward, and Grän is still under the
    pointer.

If the direction feels wrong to you, click **View → Flip Mouse Wheel**. QMapShack remembers this
setting.

```{index} single: Map; move
```

## Moving the map

- ```{image} ../images/quickstart-zoom-pan/quickstart-zoom-pan-drag.png
    :align: right
    ```

    Drag the map with the left mouse button: hold the button down and move the mouse. The map moves with the pointer;
    release the button when the part you want to see is in view. Here you drag the map to the right, and Grän moves
    along with the pointer.

```{index} single: Map; keyboard
```

## Using the keyboard

Click the map once so that it receives the key presses. Then press:

| Key | What it does |
|---|---|
| **+** | Zoom in one step, around the middle of the map |
| **-** | Zoom out one step, around the middle of the map |
| **Up** | Move the map down by a quarter of the view, to show what lies above |
| **Down** | Move the map up by a quarter of the view, to show what lies below |
| **Left** | Move the map right by a quarter of the map, to show what lies to the left |
| **Right** | Move the map left by a quarter of the map, to show what lies to the right |

**+** and **-** work on the main keys and on the number pad.

```{index} single: Map; scale
```

## Zoom steps and the scale

- ```{image} ../images/quickstart-zoom-pan/quickstart-zoom-pan-scale-bar.png
    :align: right
    ```

    The scale bar at the bottom right of the map shows how long a distance is at the current zoom. To hide it or show
    it again, click **View → Show Scale**.

- QMapShack zooms in fixed steps. To change them, click **View → Setup Map View** and choose an option under **Scales**.
    **Logarithmic** steps through round scales, such as 1, 1.5, 2, 3, 5, 7 and 10 meters per pixel.
    **Square (optimized for TMS and WMTS tiles)** doubles or halves the scale with each step. Online maps are made
    of square images, called tiles, and with these steps QMapShack draws them sharp. With **Logarithmic**,
    QMapShack scales online maps, and they look blurred. If you added the built-in online maps as described in
    [Turning on maps](quickstart-maps.md), QMapShack has already chosen **Square**.

    ```{image} ../images/quickstart-zoom-pan/quickstart-zoom-pan-scales-setup.png
    :class: below-right
    ```

```{index} single: Map; zoom range
```

## Limiting a map to a zoom range

You can limit a map to a range of zoom steps. Outside that range, QMapShack does not draw it. A detailed map, for
example, can then appear only when you zoom in.

1. ```{image} ../images/quickstart-zoom-pan/quickstart-zoom-pan-range-open.png
    :align: right
    ```

    Click the arrow to the left of the map's name in the **Maps** panel. Its settings open below it.

2. ```{image} ../images/quickstart-zoom-pan/quickstart-zoom-pan-range-buttons.png
    :align: right
    ```

    The bar below the opacity slider shows the current zoom with an indicator. The button to the left of the bar
    makes the current zoom the limit for zooming in: closer in, QMapShack no longer draws the map. The button to the
    right of the bar does the same for zooming out. Click a button again to remove its limit.

3. ```{image} ../images/quickstart-zoom-pan/quickstart-zoom-pan-range-set.png
    :align: right
    ```

    Click the right button. Green on the bar now marks the zoom steps at which QMapShack draws the map.

4. Zoom out one step further. The indicator on the bar leaves the green range, and QMapShack no longer draws the
    map. Zoom in again, and the map comes back.

    ```{image} ../images/quickstart-zoom-pan/quickstart-zoom-pan-range-out.png
    :class: below-right
    ```

To remove a limit, click its button again.
