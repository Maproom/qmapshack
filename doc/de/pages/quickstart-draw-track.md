# Einen Track zeichnen

Diese Seite zeigt, wie Sie mit dem Router aus [Routing einrichten](quickstart-routing.md) einen eigenen Track zeichnen.
Die Bilder nutzen außerdem die Karte aus [Karten einschalten](quickstart-maps.md), das DEM aus
[Höhendaten und Schummerung](quickstart-dem.md) und `Example.gpx` aus [Eine GPX-Datei laden](quickstart-load-file.md).

Bilder: *© OpenStreetMap contributors*; *Datenquelle: Land Tirol - data.tirol.gv.at, CC BY 4.0*;
*`Example.gpx`: fuzzybear, CC BY-SA 4.0*.

```{index} single: Track; zeichnen
```

## Einen Track beginnen

Während Sie zeichnen, enthält eine Leiste oben auf der Karte die Werkzeuge zum Zeichnen. **Als neu speichern**
speichert den Track, siehe [Den Track speichern](#den-track-speichern); **Abbrechen** verwirft ihn.

```{image} ../images/quickstart-draw-track/quickstart-draw-track-edit-bar.png
:class: below-right
```

1. ```{image} ../images/quickstart-draw-track/quickstart-draw-track-add-track.png
    :align: right
    ```

    Klicken Sie mit der rechten Maustaste auf die Karte, wo der Track beginnen soll, und klicken Sie auf
    **Track hinzufügen**. Dasselbe Menü öffnet sich, wenn Sie die linke Maustaste einen Moment auf der Karte gedrückt
    halten.

2. Bewegen Sie die Maus dorthin, wo der Track weitergeht, und klicken Sie. Jeder Klick fügt einen Punkt hinzu, und
    eine gerade Linie verbindet ihn mit dem Punkt davor. Für gerade Linien muss in der Leiste **O** aktiv sein; ist
    ein anderer Buchstabe aktiv, klicken Sie zuerst auf **O**.

    ```{image} ../images/quickstart-draw-track/quickstart-draw-track-drawing.png
    :class: below-right
    ```

3. Wenn der Track fertig ist, klicken Sie mit der rechten Maustaste, um keine Punkte mehr hinzuzufügen. Um
    weiterzuzeichnen, bewegen Sie die Maus knapp über den ersten oder letzten Punkt hinaus, bis ein rotes Quadrat ihn
    markiert, und klicken Sie.

```{index} single: Track; Auto-Routing
```

## Entlang von Wegen zeichnen

1. ```{image} ../images/quickstart-draw-track/quickstart-draw-track-mode-a.png
    :align: right
    ```

    Während Sie Punkte hinzufügen, klicken Sie in der Leiste auf **A** oder drücken Sie die Taste **A**.

2. Bewegen Sie die Maus zum nächsten Punkt und halten Sie sie einen Moment still. Der Router findet die Strecke
    entlang von Wegen und Straßen, und die Linie folgt ihr. Er berechnet sie für das Profil, das im Bereich
    **Streckenführung** ausgewählt ist. Klicken Sie, um diesen Teil zum Track hinzuzufügen.

    ```{image} ../images/quickstart-draw-track/quickstart-draw-track-routed.png
    :class: below-right
    ```

**A** funktioniert nur mit einem Router, der sofort antwortet; siehe
[Was ein Router macht](quickstart-routing.md#was-ein-router-macht). Ohne einen solchen Router ist der Knopf grau.
Der Buchstabe, der beim Speichern oder Abbrechen des Tracks aktiv ist, ist beim nächsten Track wieder aktiv.

```{index} single: Track; entlang eines Tracks zeichnen
```

## Entlang eines Tracks zeichnen

1. ```{image} ../images/quickstart-draw-track/quickstart-draw-track-mode-t.png
    :align: right
    ```

    Klicken Sie mit der rechten Maustaste auf einen geladenen Track, wo Ihr Track beginnen soll, zum Beispiel auf den
    in `Example.gpx`, und klicken Sie auf **Track hinzufügen**. Klicken Sie dann in der Leiste auf **T** oder drücken
    Sie die Taste **T**.

2. Bewegen Sie die Maus entlang dieses Tracks zum nächsten Punkt. QMapShack zeigt diesen Track gelb, und Ihre Linie
    folgt ihm. Das klappt nur, wenn Ihr letzter Punkt und der Mauszeiger beide auf diesem Track liegen. Klicken Sie,
    um diesen Teil zum Track hinzuzufügen.

    ```{image} ../images/quickstart-draw-track/quickstart-draw-track-along-track.png
    :class: below-right
    ```

```{index} single: Track; gerade Linien
```

## Gerade Linien und Vektorkarten

Klicken Sie auf **O** oder drücken Sie die Taste **O**, um wieder gerade Linien zu zeichnen. **V** oder die Taste
**V** folgt den Linien einer [Vektorkarte](quickstart-vector-map.md), etwa einer Garmin-Karte. Das brauchen Sie selten:
Karte und Routingdaten stammen meist beide aus OpenStreetMap, deshalb folgt **A** schon denselben Wegen.

```{index} single: Track; Punkte bearbeiten
```

## Den Track korrigieren

```{image} ../images/quickstart-draw-track/quickstart-draw-track-point-tools.png
:align: right
```

Mit den vier Knöpfen rechts von **Abbrechen** ändern Sie die Punkte der Linie. Von links nach rechts verschieben
sie Punkte, fügen Punkte hinzu, wählen einen Punktebereich und löschen Punkte.

- Um einen Punkt zu verschieben, klicken Sie auf den ersten Knopf oder drücken Sie **Strg+M**. Klicken Sie auf den
    Punkt, bewegen Sie die Maus und klicken Sie erneut, um ihn abzusetzen.
- Um einen Punkt hinzuzufügen, klicken Sie auf den zweiten Knopf oder drücken Sie **Strg++**. Klicken Sie auf die
    Linie zwischen zwei Punkten: Der neue Punkt hängt an der Maus. Klicken Sie erneut, um ihn abzusetzen. Für Punkte am
    Anfang oder Ende bewegen Sie die Maus knapp über den ersten oder letzten Punkt hinaus. Wenn ein rotes Quadrat
    diesen Punkt markiert, klicken Sie. Jeder Klick fügt dort einen Punkt hinzu; die rechte Maustaste beendet das.
- Um einen Punktebereich zu wählen, klicken Sie auf den dritten Knopf oder drücken Sie **Strg+R**. Klicken Sie dann auf
    den ersten und den letzten Punkt des Punktebereichs. Zwei Knöpfe erscheinen: Einer löscht die Punkte dazwischen, der
    andere verbindet die beiden Punkte neu. Wie, gibt der aktive Buchstabe in der Leiste vor.
- Um einen Punkt zu löschen, klicken Sie auf den vierten Knopf oder drücken Sie **Strg+-** und klicken Sie dann auf
    den Punkt.

Wenn Sie einen Punkt verschieben, hinzufügen oder löschen, verbindet QMapShack die Punkte daneben neu. Wie, gibt
der aktive Buchstabe in der Leiste vor. Um das Verschieben oder Hinzufügen eines Punkts
abzubrechen, klicken Sie mit der rechten Maustaste oder drücken Sie **Esc**.

```{image} ../images/quickstart-draw-track/quickstart-draw-track-undo-redo.png
:align: right
```

Die Pfeilknöpfe am rechten Ende der Leiste machen die letzte Änderung rückgängig und stellen sie wieder her.

```{index} single: Track; speichern
```

## Den Track speichern

1. ```{image} ../images/quickstart-draw-track/quickstart-draw-track-name.png
    :align: right
    ```

    Klicken Sie in der Leiste auf **Als neu speichern**. Geben Sie einen Namen für den Track ein und klicken Sie auf
    **OK**.

2. ```{image} ../images/quickstart-draw-track/quickstart-draw-track-project.png
    :align: right
    ```

    Wählen Sie das Projekt aus, in das der Track gehört, oder geben Sie unter **Neuer Projektname** einen Namen für ein
    neues Projekt ein.

    Bei einem neuen Projekt wählen Sie nach dem Namen unter **Das neue Projekt wird erstellt als** aus, wie QMapShack
    es speichert. **\*.qms** ist das eigene Dateiformat von
    QMapShack, **\*.gpx** eine GPX-Datei, die auch andere Programme und Geräte lesen. QMapShack wählt eines vor,
    achten Sie deshalb darauf, dass es das gewünschte ist.
    Die dritte Möglichkeit, **Datenbank**, erklärt [Datenbanken](quickstart-database.md).

    Klicken Sie auf **OK**.

3. ```{image} ../images/quickstart-draw-track/quickstart-draw-track-saved.png
    :align: right
    ```

    Der Bereich **Arbeitsplatz** listet den Track jetzt unter seinem Projekt.

Der Speichern-Knopf in der Zeile des Projekts ist im Bild markiert. Er zeigt, dass das Projekt Änderungen hat,
die noch nicht in einer Datei gespeichert sind. Klicken Sie darauf, um das Projekt zu speichern.

Beim ersten Mal fragt QMapShack, wohin es das Projekt speichern soll. Der Dialog schlägt den Namen des Projekts vor und
öffnet sich im Verzeichnis, aus dem Sie zuletzt geladen oder in das Sie zuletzt gespeichert haben. Wenn Sie das
[Hauptverzeichnis](quickstart-main-window.md#das-hauptverzeichnis-einrichten) eingerichtet haben, stellt QMapShack den
Dialog auf dessen Verzeichnis `GPX`.

In der Zeile des Tracks ist dasselbe Symbol kein Knopf, sondern eine Änderungsanzeige. Sie zeigt, dass sich der Track seit dem letzten Speichern
des Projekts geändert hat.

```{index} single: Track; Höhe
```

## Höhen aus dem DEM

Während Sie zeichnen, zeigt der Kasten **Trackmaße** oben links auf der Karte die Länge des Tracks und wie viel er
ansteigt und abfällt. Die Höhen kommen aus dem DEM; siehe [Höhendaten und Schummerung](quickstart-dem.md). Schalten
Sie das DEM also ein, bevor Sie zeichnen, und zeichnen Sie dort, wo es Daten hat.

Wenn Sie den Track speichern, gibt QMapShack seinen Punkten ihre Höhen. Wo das DEM keine Daten hat, bekommt ein Punkt
keine, und **Trackmaße** zeigt **Ungültige Höhenwerte!** statt Anstieg und Abstieg.

```{image} ../images/quickstart-draw-track/quickstart-draw-track-metrics.png
:class: below-right
```
