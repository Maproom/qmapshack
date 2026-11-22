# Setting up routing

This page shows how to set up a router, so that QMapShack can draw a track along paths and roads for you.

```{index} single: Routing
```

```{index} single: Router
```

## What a router does

A *router* calculates the way along paths and roads from one point to the next. When you draw a track, you click
only the points the track should pass, and the router adds the way between them.

To add the way while you click, the router has to work *on the fly*: it answers at once, from routing data on your
computer. A router that asks a server on the internet can't do that. It would send a request for every click, and to
prevent abuse, servers block clients that send too many requests in a short time.

```{index} single: Routing; choose a router
```

## Choosing a router

QMapShack can use two routers, Routino and BRouter. Select one in the list at the top of the
**Routing** panel.

- **Routino (offline)** routes with data on your computer. You make that data from OpenStreetMap data with
    **Tool → Create Routino Database**. The steps on this page use Routino.
- **BRouter** is easier to set up. As **BRouter (online)** it asks the BRouter-Web server, so it can't route on the
    fly. Installed on your computer, it appears as **BRouter (offline)** and then routes on the fly.

```{index} single: BRouter; install
```

## Installing BRouter

```{image} ../images/quickstart-routing/quickstart-routing-brouter-setup.png
:align: right
```

BRouter runs on Java. Install Java first: the setup wizard doesn't continue without it.

A wizard installs BRouter for you. Select **BRouter (online)** in the list and click **Setup**. In the
**BRouter Setup** wizard, choose **local Installation** and follow its pages. The wizard downloads BRouter and the
routing data for the areas you select on its map into the `BRouter` folder of your
[home folder](quickstart-main-window.md#setting-up-the-home-folder).

When the wizard is done, the list shows **BRouter (offline)**.

```{index} single: Routino; set up
```

## Setting up Routino

Routino routes with a database that QMapShack builds from OpenStreetMap data. You need an extract of your area as a
`.pbf` file, for example from [Geofabrik][GEOFABRIK]. Routino can't route from one database into another, so the
extract must cover every area a route crosses. You can combine several `.pbf` files in one database.

On Linux, install your distribution's Routino package first: the tool needs its `planetsplitter` program.

1. Select **Routino (offline)** in the list at the top of the **Routing** panel.
2. Click **Tool → Create Routino Database**.
3. Click the folder button beside **Select source files** and select the `.pbf` files.
4. Select where the database goes in the **Target Path** list. It offers the folders registered for Routino, among
    them the `Routino` folder of your [home folder](quickstart-main-window.md#setting-up-the-home-folder). The folder
    button to the left of **Target Path** adds other folders.
5. Enter a name for the database in the **File Prefix** box.
6. Click **Start**. Building takes a while. When the tool says **!!! done !!!**, the database appears under
    **Database** in the **Routing** panel.
7. ```{image} ../images/quickstart-routing/quickstart-routing-routino-select.png
    :align: right
    ```

    Select the database under **Database**. Select how you travel under **Profile**, for example **Foot** or
    **Bicycle**.

A database built elsewhere is a set of four files: `<name>-nodes.mem`, `<name>-segments.mem`, `<name>-ways.mem` and
`<name>-relations.mem`. Copy them directly into the `Routino` folder, not into a subfolder. After a few seconds the
database appears under **Database**, named after its files with underscores as spaces: the files
`tannheimer_tal-*.mem` show as **tannheimer tal**.

[GEOFABRIK]: https://download.geofabrik.de/ "Geofabrik downloads"
