# Turning on a Garmin map

This page shows how to add a Garmin map to QMapShack, turn it on and adjust what it shows.

Images: *© OpenStreetMap contributors, created by BBBike*.

```{index} single: Map; Garmin
```

```{index} single: Map; vector
```

## Garmin maps

An online map is a set of images that QMapShack loads while you look at it. A Garmin map is a vector map
instead: a file of roads, paths, areas and points that QMapShack draws itself. It needs no internet
connection.

Garmin maps are `.img` files, and they are the only vector maps QMapShack reads. The example on this page is an
extract of OpenStreetMap data, made with the [BBBike extract service][BBBIKE]. You can make one for your own
area there.

QMapShack can't open two kinds of `.img` files:

- Maps in Garmin's newer NT format, which some free maps use as well. QMapShack reports **File is NT format.**
- Locked maps, as Garmin sells them. QMapShack reports that the file contains locked or encrypted data.

```{index} single: Map; add
```

## Adding the map

QMapShack lists every map that lies directly in one of its map folders. The `Maps` folder of your
[home folder](quickstart-main-window.md#setting-up-the-home-folder) is one of them.

1. Copy the `.img` file into the `Maps` folder.
2. ```{image} ../images/quickstart-vector-map/quickstart-vector-map-listed.png
    :align: right
    ```

    After a few seconds the map appears in the list in the **Maps** panel, gray like every map that is off. It is
    named after its file, without the file type and with underscores as spaces: QMapShack lists `Tannheimer_Tal.img`
    as **Tannheimer Tal**.
3. To turn the map on, click the button in its row. The row turns bold and says **active**.

    ```{image} ../images/quickstart-vector-map/quickstart-vector-map-map-on.png
    :class: below-right
    ```

```{index} single: Map; detail
```

## Adjusting the detail

A Garmin map holds several levels of detail, and QMapShack picks one to suit the zoom. You can shift that choice
for each map.

1. ```{image} ../images/quickstart-vector-map/quickstart-vector-map-settings.png
    :align: right
    ```

    Click the arrow to the left of the map's name to open its settings.

- **Details** sets how much the map shows: above 0 more than usual at the same zoom, below 0 less. 0 is the
    default. In the image below, **Details** is 5.

    ```{image} ../images/quickstart-vector-map/quickstart-vector-map-detail-more.png
    :class: below-right
    ```

- Check or clear **Areas**, **Lines** and **Points** to show or hide these parts of the map. Without areas, for
    example, the forests disappear and paths stand out.

    ```{image} ../images/quickstart-vector-map/quickstart-vector-map-no-areas.png
    :class: below-right
    ```

```{index} single: Map; TYP file
```

## The map's look: TYP files

The colors and symbols of a Garmin map come from a TYP file. Most Garmin maps include their own, as this one does.

- ```{image} ../images/quickstart-vector-map/quickstart-vector-map-typ-row.png
    :align: right
    ```

    At the bottom of the map's settings, **Type File** names an external TYP file once you load one. If the field is
    empty, the map uses its own.
- To load another one, click the button with the folder icon and select a `.typ` file. Click the button with the
    cross icon to use the map's own TYP file again.
- A TYP file is made for one map style. One made for another map can give this map the wrong colors and symbols.

[BBBIKE]: https://extract.bbbike.org/
