# The main window

This page shows the QMapShack window as you see it on first start, and what each part of it is for.

```{index} single: Main window
```

## The window on first start

![](../images/quickstart-main-window/quickstart-main-window-first-start.png)

The map takes up the middle of the window, with the menu bar and the toolbar above it.

On first start, no map is active, so a welcome page appears instead. It closes once you turn on a map.

The panels left and right of the map are mostly empty at first; many show a hint on how to fill them:

- **Maps**: turn maps on and off.
- **Dig. Elev. Model (DEM)**: turn elevation data on and off, for example to shade the terrain on the map.
- **POI Collections**: show points of interest from POI files.
- **Workspace**: the projects you have open, with their tracks, waypoints, routes and areas.
- **Database**: keep your projects in a database.
- **Routing**: set up how QMapShack calculates routes.
- **Realtime**: show live positions, for example of ships or aircraft.

The quick start doesn't need **POI Collections** and **Realtime**.

```{index} single: Panel; move
```

```{index} single: Panel; stack
```

```{index} single: Panel; resize
```

```{index} single: Panel; close
```

## Arranging the window

Each panel has a title bar with its name along its left edge.

- ```{image} ../images/quickstart-main-window/quickstart-main-window-panel-move.png
    :align: right
    ```

    To move a panel, drag it by its title bar. You can drop it left, right, above or below the map.

- ```{image} ../images/quickstart-main-window/quickstart-main-window-panel-stack.png
    :align: right
    ```

    To stack panels, drop one panel onto another. Then use the tabs to switch between them.

- ```{image} ../images/quickstart-main-window/quickstart-main-window-panel-resize.png
    :align: right
    ```

    To resize a panel, drag the border between two panels, or between a panel and the map.

- ```{image} ../images/quickstart-main-window/quickstart-main-window-panel-buttons.png
    :align: right
    ```

    To close a panel you don't need, click the top button of its title bar. The button below it detaches the panel from the window.

```{index} single: Panel; show again
```

```{index} single: Full screen
```

```{index} single: Dock; toggle
```

```{index} single: Panel; hide all
```

## Getting a panel back

- ```{image} ../images/quickstart-main-window/quickstart-main-window-menu-window.png
    :align: right
    ```

    To show a closed panel again, click its name in the **Window** menu. The menu lists every panel and
    the toolbar, and highlights the ones on screen.

- To hide all panels at once, click **Window → Toggle Docks** or press **Ctrl+D**; the menu calls panels docks. Do the same again to bring them back.
- To give the whole screen to the map, click **Window → Fullscreen** or press **F11**. Full screen also hides the menu bar, so press **F11** to leave it.

QMapShack keeps your arrangement for the next start.

```{index} single: Home folder
```

## Setting up the home folder

QMapShack keeps its data in one folder, with a subfolder for each kind of data: `Maps`, `DEM`, `POI`, `Routino` and
`BRouter` (routing data), `Databases` and `GPX`. The welcome page sets this up for you, and the other quick start
pages put their data there.

1. ```{image} ../images/quickstart-main-window/quickstart-main-window-give-it-a-home.png
    :align: right
    ```

    Scroll the welcome page down to **Give it a home** and click the **path** link.

2. Select a folder, or create a new one, and confirm.
3. QMapShack creates the subfolders that are missing and registers them for maps, elevation data, POIs and
    routing. The welcome page then shows the folder under **Current home**.
