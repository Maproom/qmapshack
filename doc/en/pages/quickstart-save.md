# Saving your work

This page explains how to save your projects to a file or database.

```{index} single: Project; save
```

## Recognizing unsaved changes

When you start QMapShack again, it restores all projects in the **Workspace** panel. To do this, QMapShack keeps its own copy of each project.

The original file from which you loaded a project is not modified until you save the project. A newly created project has no associated file until you save it.

1. ```{image} ../images/quickstart-save/quickstart-save-changed.png
    :align: right
    ```

    In the **Workspace** panel, locate the project with unsaved changes. Its row displays a save button, highlighted in the image. In this example, **My waypoints** contains a newly added waypoint, while **Example** has no unsaved changes.
2. Click the save button. If the project has never been saved before, QMapShack first asks you where to save it. See [Saving to a file](#saving-to-a-file). Once the project is saved, the save button disappears.

In an item's row, the same symbol serves as a change indicator rather than a button. It indicates that the item has been modified since the project was last saved.

## Saving to a file

1. Right-click the project in the **Workspace** panel and click **Save as...**.
2. In the dialog, select the file type, enter a file name, and click **Save**.

If the project already has an associated file, select **Save** from the same menu. QMapShack overwrites the existing file without prompting for confirmation.

The following file types are supported:

- **QMapShack Binary .qms** is QMapShack's native file format. It preserves all project data stored by QMapShack, including the change history, so you can undo your changes. Use this format for your own projects.
- **GPS Exchange Format .gpx** is supported by many other programs and GPS devices. It preserves the list of changes, but you cannot undo them after loading the file again.

**Save as GPX 1.1 w/o ext...** in the project menu saves a standard GPX 1.1 file without any QMapShack-specific data. Use this option for programs that do not support QMapShack's extensions.

When you save a project in a different file format, QMapShack creates a copy and leaves the project's original file unchanged. The project remains associated with its original file, and its save button remains visible.

**File → Save All GIS Data** (**Ctrl+S**) saves all projects in the workspace. If a project has never been saved before, QMapShack prompts you to choose where to save it.

When you close a project with unsaved changes, QMapShack asks whether you want to save it first.

```{index} single: Project; save into database
```

## Saving to a database

To save a project loaded from a database back to its folder, click its save button. See [Working with the workspace](quickstart-database.md#working-with-the-workspace).

To store a file in a database, either copy its project in the workspace as described below or import the file. 
See [Importing files](quickstart-database.md#importing-files).

1. ```{image} ../images/quickstart-save/quickstart-save-copy-menu.png
    :align: right
    ```

    Right-click the project in the **Workspace** panel and select **Copy to...**.
2. ```{image} ../images/quickstart-save/quickstart-save-copy-dialog.png
    :align: right
    ```

    In the **Select a project...** dialog, enter a name for the new project (in this example, `Lake tour`). Select **Database** as the destination and click **OK**.
3. ```{image} ../images/quickstart-save/quickstart-save-copy-folder.png
    :align: right
    ```

    In the **Select Parent Folder...** dialog, click the arrow to the left of the database to expand its contents. Select the folder where you want to store the project (in this example, the **Projects** group) and click **OK**.
4. In the **Database Folder...** dialog, leave **Project** selected and click **OK**.
5. ```{image} ../images/quickstart-save/quickstart-save-copied.png
    :align: right
    ```

    The copy appears in the **Workspace** panel as **Lake tour @ Projects**, with a save button next to it.
6. ```{image} ../images/quickstart-save/quickstart-save-copied-db.png
    :align: right
    ```

    The **Lake tour** folder also appears in the **Database** panel, but it is still empty.
7. In the **Workspace** panel, click the save button next to **Lake tour @ Projects**. QMapShack then saves the project's items in the database folder.

The **Select a project...** dialog described in step 2 also appears when you choose **Workspace → Add empty project** or add a new item to a new project. Select **Database** in this dialog to create the new project directly in a database, too.