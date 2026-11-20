# Turning on maps

This page shows how to get your first maps into QMapShack and how to show them.

Images: *© OpenStreetMap contributors*; *OpenTopoMap, CC-BY-SA*; *trails by waymarkedtrails.org*.

```{index} single: Map; online
```

## Getting the built-in maps

QMapShack comes with five online maps, among them OpenStreetMap and OSM Topo. An online map needs an internet
connection: QMapShack loads the map while you look at it.

1. Click **File → Setup Map Paths**. The same entry is on the menu when you right-click the
    list in the **Maps** panel.
2. ```{image} ../images/quickstart-maps/quickstart-maps-map-path-setup.png
    :align: right
    ```

    Click **Help! I want maps! I don't want to read the documentation!**

3. QMapShack asks **Where do you want to store maps?** and already offers the `Maps` folder of your
    [home folder](quickstart-main-window.md#setting-up-the-home-folder).
    Confirm it. The **Setup Map Paths** dialog then closes by itself.
4. ```{image} ../images/quickstart-maps/quickstart-maps-maps-panel.png
    :align: right
    ```

    The **Maps** panel lists the maps. A map is gray while it is off.

    QMapShack names each map after its file, without the file type and with underscores as spaces: it lists
    `OSM_Topo.tms` as **OSM Topo**. It names elevation data and POI files the same way.

```{index} single: Map; turn on
```

## Turning on a map

1. ```{image} ../images/quickstart-maps/quickstart-maps-osm-button.png
    :align: right
    ```

    To turn a map on, click the button in its row in the **Maps** panel. Click it again to turn the map off.

2. Turn on **OpenStreetMap**. Its row turns bold and says **active**, and the map replaces the welcome page.

    ```{image} ../images/quickstart-maps/quickstart-maps-osm-on.png
    :class: below-right
    ```

```{index} single: Map; stack
```

## Stacking maps

When several maps are on, they lie on top of each other in the order of the list: a map covers the maps above it.

1. Turn on **OSM Topo** as well. It is below **OpenStreetMap** in the list, so **OSM Topo** covers
    **OpenStreetMap**.

    ```{image} ../images/quickstart-maps/quickstart-maps-both-on.png
    :class: below-right
    ```

2. To change the order, drag a map up or down in the list. Or select it and press **Shift+Up** or **Shift+Down**;
    the right-click menu has **Move Up** and **Move Down** as well. With **OSM Topo** moved up, **OpenStreetMap**
    covers it.

    ```{image} ../images/quickstart-maps/quickstart-maps-topo-up.png
    :class: below-right
    ```

```{index} single: Map; opacity
```

## Making a map see-through

1. To see the lower map through the upper one, click the arrow to the left of the upper map's name. Its settings open
    below the row.
2. Drag the slider under the name to the left to lower the map's opacity. Here **OSM Topo** is half see-through
    over **OpenStreetMap**.

    ```{image} ../images/quickstart-maps/quickstart-maps-topo-half.png
    :class: below-right
    ```
