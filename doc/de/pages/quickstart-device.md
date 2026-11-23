# Ein GPS-Gerät nutzen

Diese Seite zeigt, wie Sie Ihre Tracks und Wegpunkte auf ein GPS-Gerät von Garmin bringen und wie Sie die Tracks
holen, die es aufgezeichnet hat.

```{index} single: Gerät; anschließen
```

## Das Gerät anschließen

Diese Seite nutzt ein Garmin-Gerät, das als USB-Massenspeicher angeschlossen ist und das Ihr Computer als Laufwerk
zeigt. Geräte, die über MTP (Media Transfer Protocol) angeschlossen sind, und Geräte anderer Hersteller behandelt
QMapShack genauso. Was Ihr Computer für ein Gerät braucht, steht auf der Wiki-Seite [GPS Devices][WIKI_DEVICES].

1. Schließen Sie das Gerät mit seinem USB-Kabel an Ihren Computer an.
2. ```{image} ../images/quickstart-device/quickstart-device-connected.png
    :align: right
    ```

    Das Gerät erscheint im Bereich **Arbeitsplatz** unter seinem Modellnamen. QMapShack listet jede Datei mit Tracks
    und Wegpunkten auf dem Gerät als Projekt darunter auf.

Erscheint das Gerät nicht, klicken Sie auf **Arbeitsplatz → Arbeitsplatz konfigurieren** und aktivieren Sie auf der
Registerkarte **Allgemein** die Option **Geräteunterstützung aktivieren**, falls sie deaktiviert ist. Sie ist
standardmäßig aktiviert.

```{index} single: Gerät; Projekt senden
```

## Ein Projekt an das Gerät senden

1. ```{image} ../images/quickstart-device/quickstart-device-send-menu.png
    :align: right
    ```

    Klicken Sie im Bereich **Arbeitsplatz** mit der rechten Maustaste auf das Projekt und dann auf
    **Ans Gerät senden**. Sie können den Eintrag nur anklicken, solange ein Gerät angeschlossen ist.

2. ```{image} ../images/quickstart-device/quickstart-device-sent.png
    :align: right
    ```

    Das Projekt erscheint unter dem Gerät. QMapShack speichert es auf dem Gerät als GPX-Datei mit dem Namen des
    Projekts, hier `Example.gpx`. Im Dateinamen lässt QMapShack alle Zeichen außer Buchstaben, Ziffern und `_` weg.

Sind mehrere Geräte angeschlossen, fragt QMapShack, an welche es das Projekt senden soll. Senden Sie das Projekt noch
einmal, ersetzt QMapShack seine Kopie auf dem Gerät.

```{index} single: Gerät; Aufzeichnung holen
```

## Eine Aufzeichnung vom Gerät holen

1. ```{image} ../images/quickstart-device/quickstart-device-recording.png
    :align: right
    ```

    Klicken Sie auf den Pfeil links neben dem Projekt mit Ihrer Aufzeichnung, hier **Track 2025-06-14 101530**, um
    seinen Track zu sehen.

2. ```{image} ../images/quickstart-device/quickstart-device-copy-menu.png
    :align: right
    ```

    Elemente auf dem Gerät können Sie nicht ändern. Kopieren Sie den Track deshalb in den Arbeitsplatz: Klicken Sie
    mit der rechten Maustaste auf den Track und dann auf **Kopieren nach...**.

3. ```{image} ../images/quickstart-device/quickstart-device-copy-dialog.png
    :align: right
    ```

    Wählen Sie im Dialog **Ein Projekt auswählen...** ein Projekt im Arbeitsplatz aus oder geben Sie einen Namen für
    ein neues ein, hier `Meine Aufzeichnungen`, und klicken Sie auf **OK**.

4. ```{image} ../images/quickstart-device/quickstart-device-copied.png
    :align: right
    ```

    Der Track liegt jetzt im Projekt im Arbeitsplatz, wo Sie ihn ändern können. Speichern Sie das Projekt, um ihn zu
    behalten; siehe [Ihre Arbeit speichern](quickstart-save.md).

[WIKI_DEVICES]: https://github.com/Maproom/qmapshack/wiki/DocGisDevices
