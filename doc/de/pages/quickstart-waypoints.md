# Wegpunkte hinzufügen

Diese Seite zeigt, wie Sie einen Wegpunkt auf der Karte hinzufügen. Die Bilder nutzen die Karte aus
[Karten einschalten](quickstart-maps.md), das DEM aus [Höhendaten und Schummerung](quickstart-dem.md) und
`Example.gpx` aus [Eine GPX-Datei laden](quickstart-load-file.md).

Bilder: *© OpenStreetMap contributors*; *Datenquelle: Land Tirol - data.tirol.gv.at, CC BY 4.0*;
*`Example.gpx`: fuzzybear, CC BY-SA 4.0*.

```{index} single: Wegpunkt; hinzufügen
```

## Einen Wegpunkt hinzufügen

1. ```{image} ../images/quickstart-waypoints/quickstart-waypoints-add-waypoint.png
    :align: right
    ```

    Klicken Sie mit der rechten Maustaste auf die Stelle der Karte, an der der Wegpunkt liegen soll, und klicken Sie auf
    **Wegpunkt hinzufügen**. Dasselbe Menü öffnet sich, wenn Sie die linke Maustaste einen Moment auf der Karte
    gedrückt halten.

2. ```{image} ../images/quickstart-waypoints/quickstart-waypoints-project.png
    :align: right
    ```

    Wählen Sie das Projekt aus, in das der Wegpunkt gehört, oder geben Sie unter **Neuer Projektname** einen Namen für
    ein neues Projekt ein.

    Bei einem neuen Projekt wählen Sie nach dem Namen unter **Das neue Projekt wird erstellt als** aus, wie QMapShack es
    speichert. **\*.qms** ist das eigene Dateiformat von QMapShack, **\*.gpx** eine GPX-Datei, die auch andere Programme
    und Geräte lesen. QMapShack wählt eines vor, achten Sie deshalb darauf, dass es das gewünschte ist. Die dritte
    Möglichkeit, **Datenbank**, erklärt [Datenbanken](quickstart-database.md).

    Klicken Sie auf **OK**.

3. Das Fenster mit den Details des Wegpunkts öffnet sich. Es zeigt Name, Symbol, Position, Höhe und Zeit des
    Wegpunkts.

    ```{image} ../images/quickstart-waypoints/quickstart-waypoints-details.png
    :class: below-right
    ```

    Die **Höhe** kommt aus dem DEM; siehe [Höhendaten und Schummerung](quickstart-dem.md).

4. Klicken Sie auf den Namen, geben Sie einen neuen ein und drücken Sie **Enter**.

    ```{image} ../images/quickstart-waypoints/quickstart-waypoints-named.png
    :class: below-right
    ```

5. ```{image} ../images/quickstart-waypoints/quickstart-waypoints-symbol.png
    :align: right
    ```

    Klicken Sie auf das Symbol links vom Namen, um ein anderes auszuwählen.

6. ```{image} ../images/quickstart-waypoints/quickstart-waypoints-closed.png
    :align: right
    ```

    Drücken Sie **Esc**, um das Fenster zu schließen. Der Wegpunkt erscheint mit seinem Namen auf der Karte.

7. ```{image} ../images/quickstart-waypoints/quickstart-waypoints-saved.png
    :align: right
    ```

    Der Bereich **Arbeitsplatz** listet den Wegpunkt jetzt unter seinem Projekt.

Der Speichern-Knopf in der Zeile des Projekts ist im Bild markiert. Er zeigt, dass das Projekt Änderungen hat,
die noch nicht in einer Datei gespeichert sind. Klicken Sie darauf, um das Projekt zu speichern.

Beim ersten Mal fragt QMapShack, wohin es das Projekt speichern soll. Der Dialog schlägt den Namen des Projekts vor und
öffnet sich im Verzeichnis, aus dem Sie zuletzt geladen oder in das Sie zuletzt gespeichert haben. Wenn Sie das
[Hauptverzeichnis](quickstart-main-window.md#das-hauptverzeichnis-einrichten) eingerichtet haben, stellt QMapShack den
Dialog auf dessen Verzeichnis `GPX`.

In der Zeile des Wegpunkts ist dasselbe Symbol kein Knopf, sondern eine Änderungsanzeige. Sie zeigt, dass sich der Wegpunkt seit dem letzten
Speichern des Projekts geändert hat.

Der erste Wegpunkt heißt `wpt`. Ein neuer Wegpunkt bekommt das zuletzt gewählte Symbol und den zuletzt vergebenen
Namen. Endet dieser auf eine Zahl, zählt QMapShack hoch: nach `Hütte 3` kommt `Hütte 4`.
