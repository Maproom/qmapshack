# Zooming and panning

This page explains how to navigate the map: zoom in and out, pan the map, and set zoom limits.


Images: *© OpenStreetMap contributors*.

```{index} single: Map; zoom
```

## Zooming with the mouse wheel

1. ```{image} ../images/quickstart-zoom-pan/quickstart-zoom-pan-wheel-before.png
    :align: right
    ```

    Move the mouse pointer over the place you want to examine more closely, in this example the village of Grän.
2. ```{image} ../images/quickstart-zoom-pan/quickstart-zoom-pan-wheel-in.png
    :align: right
    ```

    Roll the mouse wheel forward to zoom in and backward to zoom out. Each wheel step changes the zoom level by one. The map stays centered on the place under the mouse pointer. Here, the wheel was turned two steps forward, and Grän remains under the pointer.

If you prefer the opposite scroll direction, click **View → Flip Mouse Wheel**. QMapShack remembers this setting.

```{index} single: Map; move
```

## Panning the map

- ```{image} ../images/quickstart-zoom-pan/quickstart-zoom-pan-drag.png
    :align: right
    ```

    To pan the map, hold down the left mouse button and move the mouse. The map follows the pointer. Release the button when the area you want to see is in view. Here, you drag the map to the right, and Grän moves along with the pointer.

```{index} single: Map; keyboard
```

## Using the keyboard

Click the map once to give it keyboard focus. Then press:

| Key | What it does |
|---|---|
| **+** | Zoom in one step, centered on the middle of the map |
| **-** | Zoom out one step, centered on the middle of the map |
| **Up** | Move the map down by a quarter of the view to reveal the area above |
| **Down** | Move the map up by a quarter of the view to reveal the area below |
| **Left** | Move the map right by a quarter of the view to reveal the area on the left |
| **Right** | Move the map left by a quarter of the view to reveal the area on the right |

**+** and **-** work on both the main keyboard and the numeric keypad.

```{index} single: Map; scale
```

## Zoom steps and the scale

- ```{image} ../images/quickstart-zoom-pan/quickstart-zoom-pan-scale-bar.png
    :align: right
    ```
    
    The scale bar in the bottom-right corner of the map shows the actual distance represented by its length at the current zoom level. To hide it or show
    it again, click **View → Show Scale**.
- QMapShack zooms in fixed steps. To change the zoom steps, click **View → Setup Map View** and select an option under **Scales**.

    - **Logarithmic** uses steps through rounded scales, such as 1, 1.5, 2, 3, 5, 7, and 10 meters per pixel.
    - **Square (optimized for TMS and WMTS tiles)** doubles or halves the scale with each step. 
    
        Online maps consist of square images called tiles. These zoom steps allow QMapShack to display the tiles sharply. With **Logarithmic**, QMapShack has to resize the tiles, which makes the map look blurred.

        If you added the built-in online maps as described in [Enabling maps](quickstart-maps.md), QMapShack has already selected **Square**.

    ```{image} ../images/quickstart-zoom-pan/quickstart-zoom-pan-scales-setup.png
    :class: below-right
    ```

```{index} single: Map; zoom range
```

## Limiting a map to a zoom range

You can restrict a map to a specific zoom range. QMapShack displays the map only within that range. For example, you can make a detailed map appear only when you zoom in.

1. ```{image} ../images/quickstart-zoom-pan/quickstart-zoom-pan-range-open.png
    :align: right
    ```
    In the **Maps** panel, click the arrow to the left of the map name to expand its settings.
2. ```{image} ../images/quickstart-zoom-pan/quickstart-zoom-pan-range-buttons.png
    :align: right
    ```
    
    The bar below the opacity slider shows the current zoom level with an indicator. 
    
    Click the button to the left of the bar to set the current zoom level as the limit for zooming in. QMapShack will then stop displaying the map at closer zoom levels. The button to the right sets the limit for zooming out. Click either button again to remove its limit.
3. ```{image} ../images/quickstart-zoom-pan/quickstart-zoom-pan-range-set.png
    :align: right
    ```
    
    Click the right-hand button. The green section of the bar now shows the zoom levels at which QMapShack displays the map.
4. Zoom out one more step. The indicator moves outside the green range, and QMapShack stops displaying the map. Zoom in again, and the map reappears.

    ```{image} ../images/quickstart-zoom-pan/quickstart-zoom-pan-range-out.png
    :class: below-right
    ```

