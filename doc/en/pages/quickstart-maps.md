# Enabling maps

This page explains how to add your first maps to QMapShack and display them.

Images: *© OpenStreetMap contributors*; *OpenTopoMap, CC-BY-SA*; *trails by waymarkedtrails.org*.

```{index} single: Map; online
```

## Getting the built-in maps

QMapShack comes with five online maps, including OpenStreetMap and OSM Topo. Online maps require an internet connection because QMapShack downloads map tiles as you view them.

1. Select **File → Setup Map Paths**. You can also access this option by right-clicking the list in the **Maps** panel.    
2. ```{image} ../images/quickstart-maps/quickstart-maps-map-path-setup.png
    :align: right
    ```

    Click **Help! I want maps! I don't want to read the documentation!**
3. QMapShack asks **Where do you want to store maps?** and suggests the `Maps` folder in your [home folder](quickstart-main-window.md#setting-up-the-home-folder) as the default location. Confirm it. The **Setup Map Paths** dialog then closes automatically.
4. ```{image} ../images/quickstart-maps/quickstart-maps-maps-panel.png
    :align: right
    ```

    The **Maps** panel lists the maps. A map is gray while it is inactive.

    QMapShack derives each map's name from its filename, removing the file extension and replacing underscores with spaces. For example, it displays `OSM_Topo.tms` as **OSM Topo**. The same naming convention applies to elevation data and POI files.


```{index} single: Map;activate
```

## Activating a map

1. ```{image} ../images/quickstart-maps/quickstart-maps-osm-button.png
    :align: right
    ```

    To activate a map, click the button in its row in the **Maps** panel. Click it again to deactivate the map.
2. Activate **OpenStreetMap**. Its name appears in bold, and its status changes to **active**. The map then replaces the welcome page.

    ```{image} ../images/quickstart-maps/quickstart-maps-osm-on.png
    :class: below-right
    ```

```{index} single: Map; stack
```

## Stacking maps

When multiple maps are active, they are layered in the order shown in the list. Each map covers the maps above it.

1. Activate **OSM Topo** as well. Since it appears below **OpenStreetMap** in the list, it is displayed on top and covers **OpenStreetMap**.

    ```{image} ../images/quickstart-maps/quickstart-maps-both-on.png
    :class: below-right
    ```
2. To change the order, drag a map up or down in the list. Alternatively, select it and press **Shift+Up** or **Shift+Down**. You can also use **Move Up** and **Move Down** in the right-click menu. Move **OSM Topo** up in the list, and **OpenStreetMap** will appear on top of it.

    ```{image} ../images/quickstart-maps/quickstart-maps-topo-up.png
    :class: below-right
    ```

```{index} single: Map; opacity
```

## Making a map transparent

1. To make the lower map visible through the upper one, click the arrow to the left of the upper map's name. This expands the map's settings below the row.
2. Drag the slider below the name to the left to reduce the map's opacity. Here, **OSM Topo** is set to 50% opacity, allowing **OpenStreetMap** to show through.

    ```{image} ../images/quickstart-maps/quickstart-maps-topo-half.png
    :class: below-right
    ```
