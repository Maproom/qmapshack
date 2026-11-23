# Using a GPS device

This page shows how to put your tracks and waypoints on a Garmin GPS device and how to get the tracks it recorded.

```{index} single: Device; connect
```

## Connecting the device

This page uses a Garmin device connected as USB mass storage, which your computer shows as a drive. QMapShack handles
devices that connect over MTP (Media Transfer Protocol), and devices of other makes, the same way. What your computer
needs for a device is on the wiki page [GPS Devices][WIKI_DEVICES].

1. Connect the device to your computer with its USB cable.
2. ```{image} ../images/quickstart-device/quickstart-device-connected.png
    :align: right
    ```

    The device appears in the **Workspace** panel under its model name. Below it, QMapShack lists each track and
    waypoint file on the device as a project.

If the device does not appear, click **Workspace → Setup Workspace** and make sure **Enable device support** is checked
on the **General** tab. It's checked by default.

```{index} single: Device; send project
```

## Sending a project to the device

1. ```{image} ../images/quickstart-device/quickstart-device-send-menu.png
    :align: right
    ```

    Right-click the project in the **Workspace** panel and click **Send to Devices**. You can click it only while a
    device is connected.

2. ```{image} ../images/quickstart-device/quickstart-device-sent.png
    :align: right
    ```

    The project appears below the device. QMapShack stores it on the device as a GPX file named after the project, in
    this example `Example.gpx`. In the file name, QMapShack leaves out every character except letters, digits and `_`.

If more than one device is connected, QMapShack asks which ones to send the project to. Sending the project again
replaces its copy on the device.

```{index} single: Device; get recording
```

## Getting a recording from the device

1. ```{image} ../images/quickstart-device/quickstart-device-recording.png
    :align: right
    ```

    Click the arrow to the left of the project with your recording, in this example **Track 2025-06-14 101530**, to
    see its track.

2. ```{image} ../images/quickstart-device/quickstart-device-copy-menu.png
    :align: right
    ```

    You can't change items on the device, so copy the track into the workspace: right-click the track and click
    **Copy to...**.

3. ```{image} ../images/quickstart-device/quickstart-device-copy-dialog.png
    :align: right
    ```

    In the **Select a project...** dialog, select a project in the workspace or enter a name for a new one, in this
    example `My recordings`, and click **OK**.

4. ```{image} ../images/quickstart-device/quickstart-device-copied.png
    :align: right
    ```

    The track is now in the project in the workspace, where you can change it. Save the project to keep it; see
    [Saving your work](quickstart-save.md).

[WIKI_DEVICES]: https://github.com/Maproom/qmapshack/wiki/DocGisDevices
