# Setting up routing

This page explains how to configure a router so that QMapShack can automatically create tracks along paths and roads.

```{index} single: Routing
```

```{index} single: Router
```

## What a router does

A *router* calculates routes along paths and roads between two points. When drawing a track, you only need to click the points you want to pass through. The router automatically calculates the route between them.

To add the route as you click, the router must work *on the fly*. It needs to respond immediately using routing data stored on your computer.

An online router that relies on an internet server cannot do this. It would have to send a request for every click. To prevent abuse, servers block clients that send too many requests in a short period of time.

```{index} single: Routing; choose a router
```

## Choosing a router

QMapShack supports two routers: Routino and BRouter. Select the one you want to use from the list at the top of the **Routing** panel.

- **Routino (offline)** calculates routes using data stored on your computer. You create this data from OpenStreetMap data using **Tool → Create Routino Database**. The instructions on this page describe how to set up and use Routino.
- **BRouter** is easier to set up. In **BRouter (online)** mode, it sends requests to the BRouter-Web server, so it cannot calculate routes on the fly. When installed on your computer, it appears as **BRouter (offline)** and can calculate routes on the fly.

```{index} single: BRouter; install
```

## Installing BRouter

```{image} ../images/quickstart-routing/quickstart-routing-brouter-setup.png
:align: right
```

BRouter requires Java. Install Java before proceeding. Otherwise, the setup wizard cannot continue.

A setup wizard guides you through installing BRouter. Select **BRouter (online)** from the list and click **Setup**.

In the **BRouter Setup** wizard, choose **Local Installation** and follow the instructions on each page. The wizard downloads BRouter and the routing data for the areas you select on the map. It saves them in the `BRouter` folder inside your [home folder](quickstart-main-window.md#setting-up-the-home-folder).

When the setup is complete, the list shows **BRouter (offline)**.

```{index} single: Routino; set up
```

## Setting up Routino

Routino calculates routes using a database that QMapShack builds from OpenStreetMap data. You need an extract of your area in `.pbf` format, available, for example, from [Geofabrik][GEOFABRIK].

Routino cannot calculate routes across separate databases. Therefore, the extract must cover all areas that a route passes through.

You can also combine several `.pbf` files into a single database.

On Linux, first install your distribution's Routino package. It provides the required `planetsplitter` tool. On Windows, `planetsplitter` is included in the QMapShack installation.

1. At the top of the **Routing** panel, select **Routino (offline)** from the list.
2. Click **Tool → Create Routino Database**.
3. Click the folder button next to **Select source files** and choose the desired `.pbf` files.
4. In the **Target Path** list, select the folder where you want to store the database. The list contains all folders registered with Routino, including the `Routino` folder in your [home folder](quickstart-main-window.md#setting-up-the-home-folder).

    Click the folder button to the left of **Target Path** to add more folders.
5. Enter a name for the database in the **File Prefix** field.
6. Click **Start** to build the database. This may take a while.

    When the tool reports **!!! done !!!**, the database appears under **Database** in the **Routing** panel.
7. ```{image} ../images/quickstart-routing/quickstart-routing-routino-select.png
    :align: right
    ```

    Select the database under **Database**. Then choose your mode of travel under **Profile**, for example **Foot** or **Bicycle**.

A database created elsewhere consists of four files: `<name>-nodes.mem`, `<name>-segments.mem`, `<name>-ways.mem`, and `<name>-relations.mem`.

Copy these files directly into the `Routino` folder, not into a subfolder. After a few seconds, the database appears under **Database**.

Its name is derived from the filenames, with underscores replaced by spaces. For example, the files `tannheimer_tal-*.mem` appear in the list as **tannheimer tal**.


[GEOFABRIK]: https://download.geofabrik.de/ "Geofabrik downloads"
