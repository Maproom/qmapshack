# Elevation data and hillshading

This page shows how to add elevation data to QMapShack, enable it, and shade the terrain with it.

Images: *© OpenStreetMap contributors*; *Datenquelle: Land Tirol - data.tirol.gv.at, CC BY 4.0*.

```{index} single: DEM
```

```{index} single: Elevation; under the mouse pointer
```

## What elevation data does

Elevation data, also known as a Digital Elevation Model (DEM), represents the height of the terrain. QMapShack uses it in three ways:

- It shows the elevation of any point. With a DEM enabled, the status bar at the bottom right shows the elevation
    (**Ele.**) and slope (**Slope**) at the mouse pointer.

    ```{image} ../images/quickstart-dem/quickstart-dem-status-bar.png
    :class: below-right
    ```
- On request, it adds elevation data to tracks and waypoints, or replaces their existing elevation data. When you save a track you have drawn, its elevation data is obtained from the DEM. See [Drawing a track](quickstart-draw-track.md#elevations-from-the-dem). Routes obtain their elevation data from the DEM when QMapShack calculates them.
- It adds shaded relief to the map, making mountains and valleys stand out.

```{index} single: DEM; add
```

## Adding a DEM

Elevation data is usually provided as one or more image files, such as GeoTIFF files. QMapShack cannot open these files directly. Instead, it reads a .vrt file (a virtual raster), which references the image files and combines them into a single DEM.

1. Copy the files into the `DEM` folder in your [home folder](quickstart-main-window.md#setting-up-the-home-folder). Place a single file directly in the `DEM` folder. If you have several files, put them together in a separate subfolder.
2. Click **Tool → VRT Builder**.
3. Click the folder button next to **Select source files** and choose the files you copied.
4. Click the folder button next to **Target Filename**. The first time you do this, the dialog opens in the `Maps` folder. Navigate to the `DEM` folder, enter a file name ending in .vrt, and click **Save**.
5. Click **Start**.
6. ```{image} ../images/quickstart-dem/quickstart-dem-listed.png
    :align: right
    ```

    When the VRT Builder finishes, the DEM appears in the list, grayed out, like all inactive DEMs.

A .vrt file created elsewhere may reference its source files using absolute paths, which can become invalid if the files are moved. Creating the .vrt file in the `DEM` folder avoids this problem.

```{index} single: DEM; enable
```

## Enabling the DEM

1. ```{image} ../images/quickstart-dem/quickstart-dem-dem-button.png
    :align: right
    ```

    Click the button in the DEM's row in the **Dig. Elev. Model (DEM)** panel. The row is displayed in bold and marked **active**.
2. The map looks unchanged. The DEM does not shade the terrain until you enable hillshading. However, wherever the DEM contains data, the status bar now displays the elevation and slope at the mouse pointer. If these values are not displayed, there may be a problem with the DEM.

```{index} single: DEM; hillshading
```

```{index} single: Hillshading
```

## Shading the terrain

1. ```{image} ../images/quickstart-dem/quickstart-dem-hillshading.png
    :align: right
    ```

    Click the arrow to the left of the DEM's name to expand its settings. Then check **Hillshading**.
2. ```{image} ../images/quickstart-dem/quickstart-dem-shaded.png
    :align: right
    ```

    The terrain is now shaded on the map, with slopes facing away from the light appearing darker.
3. The slider below the DEM's name controls the opacity. QMapShack draws the DEM as an overlay on the map. Moving the slider to the right makes the shading more pronounced and the map darker.

    The slider next to **Hillshading** controls the strength of the relief shading. Moving it to the left makes slopes appear steeper, while moving it to the right makes the terrain look flatter. The light source always comes from the northwest.

    ```{image} ../images/quickstart-dem/quickstart-dem-stronger.png
    :class: below-right
    ```

As with maps, you can restrict a DEM to a specific zoom range using the two buttons beside the bar below the slider. See [Limiting a map to a zoom range](quickstart-zoom-pan.md#limiting-a-map-to-a-zoom-range).