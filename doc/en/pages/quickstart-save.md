# Saving your work

This page shows how to save your projects to a file or to a database.

```{index} single: Project; save
```

## Seeing unsaved changes

When you start QMapShack again, it restores every project in the **Workspace** panel. To do this, it keeps its own copy
of each project. The file a project was loaded from changes only when you save the project. A project you created has no
file until you save it.

1. ```{image} ../images/quickstart-save/quickstart-save-changed.png
    :align: right
    ```

    In the **Workspace** panel, find the project with unsaved changes: it shows a save button in its row,
    marked in the image. In this example, **My waypoints** has a new waypoint and **Example** has no changes.

2. Click the save button. If the project has never been saved, QMapShack first asks where to save it; see
    [Saving to a file](#saving-to-a-file). After saving, the save button disappears.

On an item's row, the same symbol is not a button but a change indicator. It shows that the item has changed since the
project was last saved.

## Saving to a file

1. Right-click the project in the **Workspace** panel and click **Save as...**.
2. In the dialog, choose the file type, enter a file name and click **Save**.

If the project already has a file, click **Save** in the same menu. QMapShack overwrites the file without asking.

The file types:

- **QMapShack Binary (\*.qms)** is QMapShack's own format. It keeps everything QMapShack stores about a project,
    including the list of changes, so you can undo them. Use it for your own projects.
- **GPS Exchange Format (\*.gpx \*.GPX)**: other programs and devices can read it too. It keeps the list of
    changes, but after loading the file again you can't undo them.

**Save as GPX 1.1 w/o ext...** in the project's menu writes a plain GPX 1.1 file without any of QMapShack's own data,
for programs that can't read QMapShack's extensions.

If you save a project as another file type, QMapShack writes a copy. The project keeps its own file and its save
button.

**File → Save All GIS Data** (**Ctrl+S**) saves every project in the workspace. For a project that has never been
saved, QMapShack asks where to save it.

When you close a project with unsaved changes, QMapShack asks whether to save it first.

```{index} single: Project; save into database
```

## Saving into a database

To save a project loaded from a database back into its folder, click its save button; see
[Working with the workspace](quickstart-database.md#working-with-the-workspace). To store a file in a database, copy its
project in the workspace as shown below, or import the file; see
[Importing files](quickstart-database.md#importing-files).

1. ```{image} ../images/quickstart-save/quickstart-save-copy-menu.png
    :align: right
    ```

    Right-click the project in the **Workspace** panel and click **Copy to...**.

2. ```{image} ../images/quickstart-save/quickstart-save-copy-dialog.png
    :align: right
    ```

    In the **Select a project...** dialog, enter a name for the new project, in this example `Lake tour`. Choose
    **Database** and click **OK**.

3. ```{image} ../images/quickstart-save/quickstart-save-copy-folder.png
    :align: right
    ```

    In the **Select Parent Folder...** dialog, click the arrow to the left of the database to expand it. Select the
    folder for the project, in this example the group **Projects**, and click **OK**.

4. In the **Database Folder...** dialog, leave **Project** selected and click **OK**.

5. ```{image} ../images/quickstart-save/quickstart-save-copied.png
    :align: right
    ```

    The copy appears in the **Workspace** panel as **Lake tour @ Projects**, with a save button.

6. ```{image} ../images/quickstart-save/quickstart-save-copied-db.png
    :align: right
    ```

    In the **Database** panel, the folder **Lake tour** appears too. It's still empty.

7. Click the save button of **Lake tour @ Projects** in the **Workspace** panel. QMapShack writes the items into the
    folder.

The **Select a project...** dialog from step 2 also opens for **Workspace → Add empty project** and when you add a new
item to a new project. Choose **Database** there to create the new project in a database, too.
