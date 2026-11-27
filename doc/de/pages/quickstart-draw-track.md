# Einen Track zeichnen

Diese Seite zeigt, wie Sie mit dem Router aus [Routing einrichten](quickstart-routing.md) einen eigenen Track zeichnen.
Die Bilder nutzen 

- die Karte aus [Karten einschalten](quickstart-maps.md), 
- das DEM aus
[Höhendaten und Schummerung](quickstart-dem.md) 
- `Example.gpx` aus [Eine GPX-Datei laden](quickstart-load-file.md).

Bilder: *© OpenStreetMap contributors*; *Datenquelle: Land Tirol - data.tirol.gv.at, CC BY 4.0*;
*`Example.gpx`: fuzzybear, CC BY-SA 4.0*.

```{index} single: Track; zeichnen
```

## Einen Track beginnen

Während Sie zeichnen, zeigt eine Leiste oben auf der Karte die Zeichenwerkzeuge an. Mit **Als neu speichern** speichern Sie den Track, siehe [Den Track speichern](#den-track-speichern). **Abbrechen** verwirft den Track.

```{image} ../images/quickstart-draw-track/quickstart-draw-track-edit-bar.png
:class: below-right
```

1. ```{image} ../images/quickstart-draw-track/quickstart-draw-track-add-track.png
    :align: right
    ```

    Klicken Sie mit der rechten Maustaste auf die gewünschte Startposition in der Karte und wählen Sie **Track hinzufügen**. Dasselbe Menü öffnet sich, wenn Sie die linke Maustaste  auf der Karte gedrückt halten.
2. Bewegen Sie die Maus zum nächsten Punkt des Tracks und klicken Sie. Jeder Klick fügt einen Punkt hinzu, der durch eine gerade Linie mit dem vorherigen Punkt verbunden wird. Damit QMapShack gerade Linien zeichnet, muss in der Leiste **O** aktiv sein. Ist ein anderer Buchstabe aktiv, klicken Sie zuerst auf **O**.

    ```{image} ../images/quickstart-draw-track/quickstart-draw-track-drawing.png
    :class: below-right
    ```
3. Wenn Sie den Track fertig gezeichnet haben, klicken Sie mit der rechten Maustaste, um das Hinzufügen von Punkten zu beenden. Um weiterzuzeichnen, bewegen Sie die Maus knapp über den ersten oder letzten Punkt hinaus. Sobald ein rotes Quadrat erscheint, klicken Sie.

```{index} single: Track; Auto-Routing
```

## Entlang von Wegen zeichnen

1. ```{image} ../images/quickstart-draw-track/quickstart-draw-track-mode-a.png
    :align: right
    ```

    Während Sie Punkte hinzufügen, klicken Sie in der Leiste auf **A** oder drücken Sie die Taste **A**.
2. Bewegen Sie die Maus zum nächsten Punkt und halten Sie sie kurz still. Der Router berechnet eine Strecke entlang von Wegen und Straßen, der der Track folgt. Dabei verwendet er das Profil, das im Bereich **Streckenführung** ausgewählt ist. Klicken Sie, um den berechneten Abschnitt zum Track hinzuzufügen.

    ```{image} ../images/quickstart-draw-track/quickstart-draw-track-routed.png
    :class: below-right
    ```

    Dies funktioniert nur mit einem Router, der Strecken sofort berechnen kann. Siehe [Was ein Router macht](quickstart-routing.md#was-ein-router-macht). Ohne einen solchen Router ist die Schaltfläche ausgegraut. Der Modus, der beim Speichern oder Abbrechen eines Tracks aktiv ist, bleibt auch beim nächsten Track eingestellt.

```{index} single: Track; entlang eines Tracks zeichnen
```

## Entlang eines Tracks zeichnen

1. ```{image} ../images/quickstart-draw-track/quickstart-draw-track-mode-t.png
    :align: right
    ```

    Klicken Sie mit der rechten Maustaste auf den geladenen Track, an dem Ihr neuer Track beginnen soll, zum Beispiel auf den Track in `Example.gpx`. Wählen Sie danach **Track hinzufügen**. Klicken Sie anschließend in der Leiste auf **T** oder drücken Sie die Taste **T**.
2. Bewegen Sie die Maus entlang des geladenen Tracks zum nächsten Punkt. QMapShack hebt den Track gelb hervor. Ihr neuer Track folgt seinem Verlauf. Das funktioniert nur, wenn sich sowohl Ihr letzter Punkt als auch der Mauszeiger auf dem geladenen Track befinden. Klicken Sie, um den hervorgehobenen Abschnitt zu Ihrem Track hinzuzufügen.

    ```{image} ../images/quickstart-draw-track/quickstart-draw-track-along-track.png
    :class: below-right
    ```

```{index} single: Track; gerade Linien
```

## Gerade Linien und Vektorkarten

Klicken Sie auf **O** oder drücken Sie die Taste **O**, um wieder gerade Linien zu zeichnen. 

Mit **V** oder der Taste **V** folgen Sie den Strecken einer [Vektorkarte](quickstart-vector-map.md), beispielsweise einer Garmin-Karte. Diesen Modus brauchen Sie nur selten: Da Karte und Routingdaten meist beide aus OpenStreetMap stammen, folgt **A** bereits denselben Strecken.

```{index} single: Track; Punkte bearbeiten
```

## Den Track bearbeiten

```{image} ../images/quickstart-draw-track/quickstart-draw-track-point-tools.png
:align: right
```

Mit den vier Schaltflächen rechts neben **Abbrechen** bearbeiten Sie die Trackpunkte. Von links nach rechts: Punkte verschieben, Punkte hinzufügen, einen Bereich von Punkten auswählen und Punkte löschen.

- Um einen Punkt zu verschieben, klicken Sie auf die erste Schaltfläche oder drücken Sie **Strg+M**. Klicken Sie auf den gewünschten Punkt, bewegen Sie ihn mit der Maus an die neue Position und klicken Sie erneut, um ihn dort abzusetzen.
- Um einen Punkt hinzuzufügen, klicken Sie auf die zweite Schaltfläche oder drücken Sie **Strg++**. Klicken Sie auf die Linie zwischen zwei vorhandenen Punkten. Der neue Punkt hängt nun an der Maus. Klicken Sie erneut, um ihn an der gewünschten Position abzusetzen. Um am Anfang oder Ende der Linie einen Punkt hinzuzufügen, bewegen Sie die Maus knapp über den ersten oder letzten Punkt hinaus. Sobald ein rotes Quadrat erscheint, klicken Sie. Jeder weitere Klick fügt einen zusätzlichen Punkt hinzu. Klicken Sie mit der rechten Maustaste, um das Hinzufügen zu beenden.
- Um einen Punktebereich auszuwählen, klicken Sie auf die dritte Schaltfläche oder drücken Sie **Strg+R**. Klicken Sie anschließend auf den ersten und den letzten Punkt des gewünschten Bereichs. Daraufhin erscheinen zwei Schaltflächen: Mit der einen löschen Sie die ausgewählten Punkte, mit der anderen verbinden Sie die beiden Endpunkte direkt miteinander. Wie die Verbindung verläuft, bestimmt der aktive Modus in der Leiste.
- Um einen Punkt zu löschen, klicken Sie auf die vierte Schaltfläche oder drücken Sie **Strg+-**. Klicken Sie anschließend auf den Punkt, den Sie löschen möchten.

Wenn Sie einen Punkt verschieben, hinzufügen oder löschen, verbindet QMapShack die benachbarten Punkte neu. Wie die Verbindung verläuft, bestimmt der aktive Modus in der Werkzeugleiste. Um das Verschieben oder Hinzufügen eines Punkts abzubrechen, klicken Sie mit der rechten Maustaste oder drücken Sie **Esc**.

```{image} ../images/quickstart-draw-track/quickstart-draw-track-undo-redo.png
:align: right
```

Mit den Pfeilschaltflächen am rechten Ende der Werkzeugleiste können Sie die letzte Änderung rückgängig machen oder wiederherstellen.

```{index} single: Track; speichern
```

## Den Track speichern

1. ```{image} ../images/quickstart-draw-track/quickstart-draw-track-name.png
    :align: right
    ```

    Klicken Sie in der Werkzeugleiste auf **Als neu speichern**. Geben Sie einen Namen für den Track ein und klicken Sie auf
    **OK**.
2. ```{image} ../images/quickstart-draw-track/quickstart-draw-track-project.png
    :align: right
    ```

    Wählen Sie das Projekt aus, zu dem Sie den Track hinzufügen möchten, oder geben Sie unter **Neuer Projektname** einen Namen für ein neues Projekt ein.

    Bei einem neuen Projekt geben Sie zunächst einen Namen ein und wählen dann unter **Das neue Projekt wird erstellt als** das gewünschte Speicherformat aus. 
    
    - **\*.qms** ist das Dateiformat von QMapShack, 
    - **\*.gpx** ist das .gpx Format, das auch andere Programme und Geräte lesen können. 
    
    QMapShack wählt automatisch ein Format aus. Prüfen Sie daher, ob die Vorauswahl Ihren Wünschen entspricht. Die dritte Möglichkeit, **Datenbank**, wird unter [Datenbanken](quickstart-database.md) erläutert.
    
    Klicken Sie auf **OK**.
3. ```{image} ../images/quickstart-draw-track/quickstart-draw-track-saved.png
    :align: right
    ```

    Der Bereich **Arbeitsplatz** zeigt den Track jetzt unter dem zugehörigen Projekt an.

Die im Bild markierte Speichern-Schaltfläche in der Projektzeile zeigt an, dass das Projekt Änderungen enthält, die noch nicht in der Datei gespeichert wurden. Klicken Sie darauf, um das Projekt zu speichern.

Beim ersten Speichern fragt QMapShack, wo das Projekt abgelegt werden soll. Der Dialog schlägt den Projektnamen als Dateinamen vor. Er öffnet sich in dem Verzeichnis, aus dem Sie zuletzt ein Projekt geladen oder in das Sie zuletzt eines gespeichert haben. Wenn Sie das [Hauptverzeichnis](quickstart-main-window.md#das-hauptverzeichnis-einrichten) eingerichtet haben, öffnet QMapShack den Dialog standardmäßig in dessen Unterverzeichnis `GPX`.

In der Trackzeile dient dasselbe Symbol nicht als Schaltfläche, sondern als Änderungsanzeige. Es zeigt an, dass der Track seit dem letzten Speichern des Projekts geändert wurde.

```{index} single: Track; Höhe
```

## Höhen aus dem DEM

Während Sie zeichnen, zeigt der Kasten **Trackmaße** oben links auf der Karte die Länge des Tracks sowie seinen gesamten Auf- und Abstieg an. Die Höhen stammen aus dem DEM. Siehe [Höhendaten und Schummerung](quickstart-dem.md). Schalten Sie das DEM daher ein, bevor Sie mit dem Zeichnen beginnen, und zeichnen Sie nur in Bereichen, für die Höhendaten verfügbar sind.

Beim Speichern des Tracks weist QMapShack seinen Punkten die entsprechenden Höhen aus dem DEM zu. Wo keine DEM-Daten verfügbar sind, bleibt die Höhe eines Punkts leer. In diesem Fall zeigt **Trackmaße** statt des Auf- und Abstiegs den Hinweis **Ungültige Höhenwerte!** an.

```{image} ../images/quickstart-draw-track/quickstart-draw-track-metrics.png
:class: below-right
```
