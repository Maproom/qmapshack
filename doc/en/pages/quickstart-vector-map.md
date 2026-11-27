# Enabling a Garmin map

This page shows how to add a Garmin map to QMapShack, enable it on and adjust what it shows.

Images: *© OpenStreetMap contributors, created by BBBike*.

```{index} single: Map; Garmin
```

```{index} single: Map; vector
```

## Garmin maps

An online map is a collection of images that QMapShack loads as you browse the map. A Garmin map, by contrast, is an offline vector map: a file containing roads, paths, areas and points that QMapShack renders itself. No internet connection is required.

Garmin maps are stored in .img files, the only vector map format supported by QMapShack. The example on this page is an extract of OpenStreetMap data, created using the [BBBike extract service][BBBIKE]. You can use this service to create an extract for your own area.

QMapShack cannot open two types of .img files:

- Maps in Garmin's newer NT format, which is also used by some free maps. QMapShack displays the error message **File is NT format.**
- Locked maps, such as those sold by Garmin. QMapShack reports that the file contains locked or encrypted data.

```{index} single: Map; add
```

## Adding the map

QMapShack lists all maps stored directly in one of its map folders. One of these folders is the `Maps` folder in your [home folder][HOMEFOLDER].

1. Copy the .img file into the `Maps` folder.
2. ```{image} ../images/quickstart-vector-map/quickstart-vector-map-listed.png
    :align: right
    ```
    After a few seconds, the map appears in the **Maps** panel. Like all inactive maps, it is shown in gray. Its name is derived from the filename: the file extension is removed and underscores are replaced with spaces. For example, QMapShack displays `Tannheimer_Tal.img` as **Tannheimer Tal**.
3. To activate a map, click the button in its row. Its name then appears in bold, and its status changes to **active**.

    ```{image} ../images/quickstart-vector-map/quickstart-vector-map-map-on.png
    :class: below-right
    ```

```{index} single: Map; detail
```

## Adjusting the details

A Garmin map contains several levels of detail, and QMapShack automatically selects the appropriate level based on the current zoom level. You can adjust this selection individually for each map.

- ```{image} ../images/quickstart-vector-map/quickstart-vector-map-settings.png
    :align: right
    ```

    Click the arrow to the left of the map's name to open its settings.


- **Details** controls how much information is displayed on the map. A value above 0 displays more detail than usual at the same zoom level, while a value below 0 displays less. The default value is 0. In the image below, **Details** is set to 5.


    ```{image} ../images/quickstart-vector-map/quickstart-vector-map-detail-more.png
    :class: below-right
    ```

- Select or clear **Areas**, **Lines**, and **Points** to show or hide the corresponding map elements. For example, hiding areas makes forests disappear, allowing paths to stand out more clearly.

    ```{image} ../images/quickstart-vector-map/quickstart-vector-map-no-areas.png
    :class: below-right
    ```

```{index} single: Map; TYP file
```

## The map's appearance: TYP files

A Garmin map's colors and symbols are defined by a TYP file. Most Garmin maps include their own TYP file, and this map is no exception.

- ```{image} ../images/quickstart-vector-map/quickstart-vector-map-typ-row.png
    :align: right
    ```    
    
    At the bottom of the map's settings, **Type File** shows the name of an external TYP file if one has been loaded. If the field is empty, the map uses its built-in TYP file.
- To load a different TYP file, click the folder icon and select a .typ file. To switch back to the map's built-in TYP file, click the button with the cross icon.
- Each TYP file is designed for a particular map style. Using a TYP file designed for a different map may result in incorrect colors and symbols.


[BBBIKE]: https://extract.bbbike.org/
[HOMEFOLDER]: quickstart-main-window.md#setting-up-the-home-folder