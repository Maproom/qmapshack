# Using a GPS device

This page shows how to put your tracks and waypoints on a Garmin GPS device and how to get the tracks it recorded.

```{index} single: Device; connect
```

## Connecting the device

This page describes how to use in QMapShack a Garmin device connected as USB mass storage, which your computer recognizes as a drive. QMapShack handles devices that connect via MTP (Media Transfer Protocol), as well as devices from other manufacturers, in a similar way. For information about the requirements your computer must meet to work with a particular device, see the wiki page [GPS Devices][WIKI_DEVICES].


1. Connect the device to your computer with its USB cable.
2. ```{image} ../images/quickstart-device/quickstart-device-connected.png
    :align: right
    ```

    The device appears in the **Workspace** panel under its model name. QMapShack lists each track and waypoint file on the device as a separate project beneath it.

If the device does not appear, click **Workspace → Setup Workspace** and make sure **Enable device support** is checked
on the **General** tab. This option is enabled by default.

```{index} single: Device; send project
```

## Sending a project to the device

1. ```{image} ../images/quickstart-device/quickstart-device-send-menu.png
    :align: right
    ```

    Right-click the project in the **Workspace** panel and click **Send to Devices**. You can only click it when a device is connected.

2. ```{image} ../images/quickstart-device/quickstart-device-sent.png
    :align: right
    ```

    The project appears below the device. QMapShack stores it on the device as a .gpx file named after the project, in
    this example `Example.gpx`. When creating the file name, QMapShack removes all characters except letters, digits, and underscores.

If more than one device is connected, QMapShack asks which devices you want to send the project to. Sending the project again overwrites the existing copy on the device.

```{index} single: Device; get recording
```

## Getting a recording from the device

1. ```{image} ../images/quickstart-device/quickstart-device-recording.png
    :align: right
    ```
   
    Click the arrow to the left of the project containing your recording (in this example, **Track 2025-06-14 101530**) to expand it and display the track.

2. ```{image} ../images/quickstart-device/quickstart-device-copy-menu.png
    :align: right
    ```

    You cannot modify items directly on the device, so first copy the track to the workspace. To do this, right-click the track and select **Copy to...**.

3. ```{image} ../images/quickstart-device/quickstart-device-copy-dialog.png
    :align: right
    ```

    In the **Select a project...** dialog, select a project in the workspace or enter a name for a new project (in this
    example `My recordings`). Then click **OK**.

4. ```{image} ../images/quickstart-device/quickstart-device-copied.png
    :align: right
    ```

    The track is now in the project in the workspace, where you can edit it. Save the project to keep your changes. See
    [Saving your work](quickstart-save.md).

[WIKI_DEVICES]: https://github.com/Maproom/qmapshack/wiki/DocGisDevices
