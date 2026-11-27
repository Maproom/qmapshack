# Wegpunkte hinzufügen

Diese Seite zeigt, wie Sie einen Wegpunkt auf der Karte hinzufügen. Die Bilder auf dieser Seite nutzen die Karte aus
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

    Klicken Sie mit der rechten Maustaste auf die gewünschte Position in der Karte und wählen Sie **Wegpunkt hinzufügen**. Dasselbe Menü öffnet sich, wenn Sie die linke Maustaste auf der Karte gedrückt halten.
2. ```{image} ../images/quickstart-waypoints/quickstart-waypoints-project.png
    :align: right
    ```

    Wählen Sie das Projekt aus, dem Sie den Wegpunkt hinzufügen möchten, oder geben Sie unter **Neuer Projektname** einen Namen für ein neues Projekt ein.

    Wenn Sie ein neues Projekt anlegen, wählen Sie nach der Eingabe des Namens unter **Das neue Projekt wird erstellt als** das gewünschte Speicherformat aus:

    - **\*.qms** ist das eigene Dateiformat von QMapShack.
    - **\*.gpx** ist das .gpx Format, das auch von anderen Programmen und Geräten gelesen werden kann.

    QMapShack wählt automatisch ein Format aus. Prüfen Sie daher, ob die Vorauswahl Ihren Wünschen entspricht.

    Die dritte Möglichkeit, **Datenbank**, wird unter [Datenbanken][DB] erläutert.

    Klicken Sie auf **OK**.
3. Das Fenster mit den Details des Wegpunkts öffnet sich. Es zeigt Name, Symbol, Position, Höhe und Zeit des
    Wegpunkts.

    ```{image} ../images/quickstart-waypoints/quickstart-waypoints-details.png
    :class: below-right
    ```

    Die **Höhe** wird aus dem DEM ermittelt. Siehe [Höhendaten und Schummerung](quickstart-dem.md).
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

    Der Bereich **Arbeitsplatz** zeigt den Wegpunkt jetzt unter dem zugehörigen Projekt an.

Die im Bild markierte Speichern-Schaltfläche in der Projektzeile zeigt an, dass das Projekt Änderungen enthält, die noch nicht in der Datei gespeichert wurden. Klicken Sie darauf, um das Projekt zu speichern.

Beim ersten Speichern fragt QMapShack, wo das Projekt abgelegt werden soll. Der Dialog schlägt den Projektnamen als Dateinamen vor. Der Dialog öffnet sich in dem Verzeichnis, aus dem Sie zuletzt ein Projekt geladen oder in das Sie zuletzt eines gespeichert haben.

Wenn Sie das [Hauptverzeichnis][HAUPTVERZ] eingerichtet haben, öffnet QMapShack den Dialog standardmäßig in dessen Unterverzeichnis `GPX`.
In der Zeile des Wegpunkts ist dasselbe Symbol keine Schaltfläche, sondern eine Änderungsanzeige. Sie zeigt, dass sich der Wegpunkt seit dem letzten
Speichern des Projekts geändert hat.

Der erste Wegpunkt heißt `wpt`. Ein neuer Wegpunkt bekommt das zuletzt gewählte Symbol und den zuletzt vergebenen
Namen. Endet der Name mit einer Zahl, erhöht QMapShack diese automatisch: Auf `Hütte 3` folgt `Hütte 4`.


[DB]:        https://quickstart-database.md
[HAUPTVERZ]: https://quickstart-main-window.md#das-hauptverzeichnis-einrichten