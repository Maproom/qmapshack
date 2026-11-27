# Databases

This page shows how to keep projects in a QMapShack database. The images use the example database `Example.db`.

```{index} single: Database; add
```

## Creating a database

A database is a single file that stores your projects, organized into folders. The **Database** panel displays a list of your databases.

1. ```{image} ../images/quickstart-database/quickstart-database-add-menu.png
    :align: right
    ```

    Right-click an empty area in the **Database** panel and select **Add Database**.
2. ```{image} ../images/quickstart-database/quickstart-database-add-dialog.png
    :align: right
    ```

    Enter `Example` as **Name** and leave **SQLite** selected. 
3. Click the plus button highlighted in step 2. Choose a folder, enter a file name, and click **Save**.

    The dialog opens in the folder where you last saved a database. If you have set up the [home folder](quickstart-main-window.md#setting-up-the-home-folder), QMapShack opens the `Databases` folder within it.

    If you already have a database file, click the folder button to the right of the plus button instead. Select the file and click **Open**.    
    
    
4. ```{image} ../images/quickstart-database/quickstart-database-panel.png
    :align: right
    ```

    Click **OK**. The database appears in the **Database** panel.

```{index} single: Database; folder
```

## Folders

A database organizes its data into folders. The color of a folder shows its type:

- A **Group** folder (blue) contains other folders and helps structure your data. You can't load a **Group** folder into the workspace. Place **Project** folders inside a **Group** folder rather than directly in the database.
- A **Project** folder (green) contains tracks, waypoints, routes and areas much like a project file. You can load it into the workspace.
- An **Other** folder (orange) works like a **Project** folder, but you can use it for any purpose. For example, store a trip's recording in a **Project** folder and its planning in an **Other** folder nested inside it.

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

You can store project files into a database without loading them into the workspace.

1. ```{image} ../images/quickstart-database/quickstart-database-import-menu.png
    :align: right
    ```

    Right-click the folder for the projects, in this example, the group **Tours**, and click
    **Import from Files...**.

2. Select one or more files, for example `Vilsalpsee.gpx`, and click **Open**.

3. ```{image} ../images/quickstart-database/quickstart-database-imported.png
    :align: right
    ```

    QMapShack creates a **Project** folder for each file inside the folder you right-clicked. In this example, **Vilsalpsee** appears under **Tours**. The folder takes its name from the project in the file. QMapShack skips any project that is already in the database.

```{index} single: Database; export
```

## Exporting to GPX

You can export an entire database or individual folders as .gpx files, for example, to share your data with others.

1. ```{image} ../images/quickstart-database/quickstart-database-export-menu.png
    :align: right
    ```

    Right-click the **Example** database and click **Export to GPX...**.
2. ```{image} ../images/quickstart-database/quickstart-database-export-dialog.png
    :align: right
    ```

    Click the folder button highlighted in the image and select a folder on your computer. Check **GPX 1.1 without extensions** to exclude QMapShack-specific data.
3. Click **Start**. QMapShack creates a folder for the database and a separate folder for each group. It also creates a .gpx file for each **Project** and **Other** folder. The file name of an **Other** folder begins with the name of its parent folder.
4. When **Done!** appears, click **Close**.

    ```{image} ../images/quickstart-database/quickstart-database-exported.png
    :class: below-right
    ```

```{index} single: Database; workspace
```

## Working with the workspace

The database stores your data, but you edit it in the workspace. Load a project, make your changes, save it back to the database, and then unload it from the workspace.

1. ```{image} ../images/quickstart-database/quickstart-database-loaded-db.png
    :align: right
    ```

    Click the load button highlighted in the image at the right end of the **Vilsalpsee** row.

2. ```{image} ../images/quickstart-database/quickstart-database-loaded-wks.png
    :align: right
    ```

    The project appears in the **Workspace** panel as **Vilsalpsee @ Tours**, showing its name and parent folder.

3. Edit the project just as you would edit a project file, for example, by [adding a waypoint](quickstart-waypoints.md). When you're done, click the save button in the project's row in the **Workspace** panel. QMapShack saves your changes to the database.
4. Click the load button in the **Database** panel again to unload the project. If the project has unsaved changes, QMapShack asks what you want to do:

    - **Save** saves the changes before unloading the project.
    - **No** discards the changes and unloads the project.
    - **Abort** cancels the operation and leaves the project loaded.
    
Removing an item from a project in the workspace does not delete it from the database, even after saving. To delete an item from a folder, right-click it in the **Database** panel and select **Delete Item**. If an item is no longer in any folder, QMapShack moves it to **Lost & Found**. Deleting it from there removes it permanently.