# Turning on maps

This page shows how to get your first maps into QMapShack and how to show them.

Images: *© OpenStreetMap contributors*; *OpenTopoMap, CC-BY-SA*; *trails by waymarkedtrails.org*.

```{index} single: Map; online
```

## Getting the built-in maps

QMapShack comes with five online maps, among them OpenStreetMap and OSM Topo. An online map needs an internet
connection: QMapShack loads the map while you look at it.

1. Click **File → Setup Map Paths**. You find the same entry when you right-click the list in
    the **Maps** panel.
2. ```{image} ../images/quickstart-maps/map-path-setup.png
    :align: right
    ```

    Click **Help! I want maps! I don't want to read the documentation!**

3. QMapShack asks **Where do you want to store maps?** and already offers the `Maps` folder of your
    [home folder](quickstart-main-window.md#setting-up-the-home-folder).
    Confirm it. The dialog closes by itself.
4. ```{image} ../images/quickstart-maps/maps-panel.png
    :align: right
    ```

    The **Maps** panel lists the maps. They are gray until you turn one on.

    Each map is named after its file, without the file type and with underscores as spaces: `OSM_Topo.tms` is
    listed as **OSM Topo**. Elevation data and POI files are named the same way.

```{index} single: Map; turn on
```

## Turning a map on

- ```{image} ../images/quickstart-maps/osm-button.png
    :align: right
    ```

    To turn a map on, click the button on its row in the **Maps** panel. Click it again to turn the map off.

- Turn on **OpenStreetMap**. Its row turns bold and says **active**, and the map replaces the welcome page.

    ```{image} ../images/quickstart-maps/osm-on.png
    :class: below-right
    ```

```{index} single: Map; stack
```

## Stacking maps

When several maps are on, they lie on top of each other in the order of the list: a map covers the maps above it.

- Turn on **OSM Topo** as well. It is below **OpenStreetMap** in the list, so it covers it.

    ```{image} ../images/quickstart-maps/both-on.png
    :class: below-right
    ```

- To change the order, drag a map up or down in the list. Or select it and press **Shift+Up** or **Shift+Down**;
    the right-click menu has **Move Up** and **Move Down** as well. With **OSM Topo** moved up, **OpenStreetMap**
    covers it.

    ```{image} ../images/quickstart-maps/topo-up.png
    :class: below-right
    ```

```{index} single: Map; opacity
```

## Making a map see-through

- To see the lower map through the upper one, click the arrow left of the upper map's name. Its settings open
    below the row. Drag the slider under the name to the left to lower the map's opacity. Here **OSM Topo** is
    half see-through over **OpenStreetMap**.

    ```{image} ../images/quickstart-maps/topo-half.png
    :class: below-right
    ```
