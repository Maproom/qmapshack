# Databases

This page shows how to keep projects in a QMapShack database. The images use the example database `Example.db`.

```{index} single: Database; add
```

## Creating a database

A database is one file that holds your projects, sorted into folders. The **Database** panel lists your databases.

1. ```{image} ../images/quickstart-database/quickstart-database-add-menu.png
    :align: right
    ```

    Right-click an empty spot in the **Database** panel and click **Add Database**.

2. ```{image} ../images/quickstart-database/quickstart-database-add-dialog.png
    :align: right
    ```

    Enter `Example` as **Name** and leave **SQLite** selected.

3. Click the plus button marked in the image of step 2. Choose a folder, enter a file name and click **Save**.

    The dialog opens in the folder you last chose a database in. If you have set up the
    [home folder](quickstart-main-window.md#setting-up-the-home-folder), QMapShack points it to its `Databases` folder.

    If you already have a database file, click the folder button to the right of the plus button instead. Select the
    file and click **Open**.

4. ```{image} ../images/quickstart-database/quickstart-database-panel.png
    :align: right
    ```

    Click **OK**. The database appears in the **Database** panel.

```{index} single: Database; folder
```

## Folders

A database keeps its data in folders. The color of a folder shows its type:

- A **Group** folder (blue) holds other folders and helps to structure the data. You can't load it into the
    workspace. Put **Project** folders into a **Group** folder, not directly into the database.
- A **Project** folder (green) holds tracks, waypoints, routes and areas, like a project file. You can load it into
    the workspace.
- An **Other** folder (orange) works like a **Project** folder. Use it as you like. For example, put a trip's
    recording into a **Project** folder and the planning into an **Other** folder below it.

1. ```{image} ../images/quickstart-database/quickstart-database-folder-menu.png
    :align: right
    ```

    Right-click the **Example** database and click **Add Folder**.

2. ```{image} ../images/quickstart-database/quickstart-database-folder-dialog.png
    :align: right
    ```

    Enter `Tours` as **Folder name** and choose **Group**.

3. ```{image} ../images/quickstart-database/quickstart-database-folder-group.png
    :align: right
    ```

    Click **OK**. The group **Tours** appears below **Example**.

4. ```{image} ../images/quickstart-database/quickstart-database-folder-project.png
    :align: right
    ```

    Right-click **Tours** and click **Add Folder**. Enter `Alpine tour`, choose **Project** and click **OK**.

5. ```{image} ../images/quickstart-database/quickstart-database-folders.png
    :align: right
    ```

    Right-click **Alpine tour** and click **Add Folder**. Enter `Planning`, choose **Other** and click **OK**.

```{index} single: Database; import
```

## Importing files

You can put project files into a database without loading them into the workspace.

1. ```{image} ../images/quickstart-database/quickstart-database-import-menu.png
    :align: right
    ```

    Right-click the folder for the projects, in this example the group **Tours**, and click
    **Import from Files...**.

2. Select one or more files, for example `Vilsalpsee.gpx`, and click **Open**.

3. ```{image} ../images/quickstart-database/quickstart-database-imported.png
    :align: right
    ```

    QMapShack creates a **Project** folder for each file inside the folder you right-clicked. In this example,
    **Vilsalpsee** appears below **Tours**. The folder takes the name of the project in the file. QMapShack skips a
    project that is already in the database.

```{index} single: Database; export
```

## Exporting to GPX

You can export a database, or one of its folders, to GPX files, for example to share the data.

1. ```{image} ../images/quickstart-database/quickstart-database-export-menu.png
    :align: right
    ```

    Right-click the **Example** database and click **Export to GPX...**.

2. ```{image} ../images/quickstart-database/quickstart-database-export-dialog.png
    :align: right
    ```

    Click the folder button marked in the image and choose a folder on your computer. Check
    **GPX 1.1 without extensions** to leave out QMapShack's own data.

3. Click **Start**. QMapShack creates a folder for the database and for each group, and a GPX file for each
    **Project** and **Other** folder. The file name of an **Other** folder starts with the name of the folder above it.

4. When **Done!** appears, click **Close**.

    ```{image} ../images/quickstart-database/quickstart-database-exported.png
    :class: below-right
    ```

```{index} single: Database; workspace
```

## Working with the workspace

The database stores your data. You change it in the workspace: load a project, change it, save it back to the
database and unload it again.

1. ```{image} ../images/quickstart-database/quickstart-database-loaded-db.png
    :align: right
    ```

    Click the load button marked in the image, at the right end of the row of **Vilsalpsee**.

2. ```{image} ../images/quickstart-database/quickstart-database-loaded-wks.png
    :align: right
    ```

    The project appears in the **Workspace** panel as **Vilsalpsee @ Tours**: its name and the folder it is in.

3. Change the project as you would change a project file, for example [add a waypoint](quickstart-waypoints.md).
    Then click the save button on the project's row in the **Workspace** panel. QMapShack writes the changes to the
    database.

4. Click the load button in the **Database** panel again to unload the project. If the project has unsaved changes,
    QMapShack asks first: **Save** saves them, **No** discards them, **Abort** keeps the project loaded.

Removing an item from a project in the workspace leaves it in the database, even after saving. To delete it from a
folder, right-click it in the **Database** panel and click **Delete Item**. An item that is no longer in any folder
moves to **Lost & Found**. Deleting it there removes it permanently.
