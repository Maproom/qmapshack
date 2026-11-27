# Das Hauptfenster

Diese Seite zeigt das QMapShack-Fenster, wie es beim ersten Start aussieht, und erläutert die Funktionen der einzelnen Bereiche.

```{index} single: Hauptfenster
```

## Das Fenster beim ersten Start

![](../images/quickstart-main-window/quickstart-main-window-first-start.png)

Die Karte nimmt die Mitte des Fensters ein, darüber befinden sich die Menüleiste und die Werkzeugleiste.

Beim ersten Start ist keine Karte aktiv, deshalb steht dort eine Begrüßungsseite. Sie schließt sich, sobald Sie
eine Karte aktivieren.

Die Bereiche links und rechts der Karte sind anfangs größtenteils leer. Die meisten zeigen einen Hinweis, wie Sie
sie füllen:

- **Karten**: Karten aktivieren und deaktivieren.
- **Dig. Höhenmodell (DEM)**: Höhendaten aktivieren und deaktivieren, zum Beispiel, um das Gelände auf der Karte zu
    schummern.
- **Sehenswürdigkeiten**: Sehenswürdigkeiten aus POI-Dateien anzeigen.
- **Arbeitsplatz**: die geöffneten Projekte mit den darin enthaltenen Tracks, Wegpunkten, Routen und Gebieten.
- **Datenbank**: Ihre Projekte in einer Datenbank speichern.
- **Streckenführung**: konfigurieren, wie QMapShack Routen berechnet.
- **Echtzeit**: Live-Positionen anzeigen, zum Beispiel von Schiffen oder Flugzeugen.

Diese Schnellstartanleitung behandelt **POI-Sammlungen** und **Echtzeit** nicht.

```{index} single: Bereich; verschieben
```

```{index} single: Bereich; stapeln
```

```{index} single: Bereich; Größe ändern
```

```{index} single: Bereich; schließen
```

## Das Fenster anordnen

Jeder Bereich hat am linken Rand eine Titelleiste mit seinem Namen.

- ```{image} ../images/quickstart-main-window/quickstart-main-window-panel-move.png
    :align: right
    ```

    Um einen Bereich zu verschieben, ziehen Sie ihn an seiner Titelleiste an die gewünschte Position. Sie können ihn links, rechts, über oder unter der Karte ablegen.

- ```{image} ../images/quickstart-main-window/quickstart-main-window-panel-stack.png
    :align: right
    ```

    Um Bereiche zu übereinander zu stapeln, legen Sie einen Bereich auf einem anderen ab. Über Reiter wechseln Sie dann zwischen ihnen.

- ```{image} ../images/quickstart-main-window/quickstart-main-window-panel-resize.png
    :align: right
    ```

    Um die Größe eines Bereichs zu ändern, ziehen Sie die Trennlinie zwischen zwei Bereichen oder zwischen einem Bereich und der Karte an die gewünschte Position.

- ```{image} ../images/quickstart-main-window/quickstart-main-window-panel-buttons.png
    :align: right
    ```

    Um einen Bereich zu schließen, den Sie nicht brauchen, klicken Sie auf die obere Schaltfläche seiner
    Titelleiste. Die Schaltfläche darunter löst den Bereich aus dem Fenster.

```{index} single: Bereich; wieder anzeigen
```

```{index} single: Vollbild
```

```{index} single: Fenster; umschalten
```

```{index} single: Bereich; alle ausblenden
```

## Einen Bereich wiederherstellen

- ```{image} ../images/quickstart-main-window/quickstart-main-window-menu-window.png
    :align: right
    ```

    Um einen geschlossenen Bereich wieder anzuzeigen, klicken Sie auf seinen Namen im Menü **Fenster**. Das Menü
    führt jeden Bereich und die Werkzeugleiste auf und hebt die sichtbaren hervor.

- Um alle Bereiche auf einmal auszublenden, klicken Sie auf **Fenster → Fenster umschalten** oder drücken Sie
    **Strg+D**. Das Menü bezeichnet die Bereiche als Fenster. Wiederholen Sie diesen Schritt, um die Bereiche/Fenster zurückzuholen.
- Um die Karte im Vollbildmodus anzuzeigen, wählen Sie **Fenster → Vollbild** oder drücken Sie **F11**. Das
    Vollbild blendet auch die Menüleiste aus. Verlassen Sie das Vollbild mit **F11**.

QMapShack merkt sich Ihre Anordnung für den nächsten Start.

```{index} single: Hauptverzeichnis
```

## Das Hauptverzeichnis einrichten

QMapShack speichert seine Daten in einem Verzeichnis, mit einem Unterverzeichnis für jede Art von Daten: `Maps`, `DEM`,
`POI`, `Routino` und `BRouter` (Daten für die Streckenführung), `Databases` und `GPX`. Die Begrüßungsseite richtet
es für Sie ein, und die anderen Seiten des Schnellstarts legen ihre Daten dort ab.

QMapShack speichert seine Daten in einem Verzeichnis ab, das für jede Datenart ein eigenes Unterverzeichnis enthält: `Maps`, `DEM`, `POI`, `Routino` und `BRouter` (Daten für die Streckenführung), `Databases` und `GPX`. Die Begrüßungsseite richtet diese Verzeichnisstruktur für Sie ein. Die übrigen Seiten des Schnellstarts speichern ihre Daten in den entsprechenden Unterverzeichnissen.

1. ```{image} ../images/quickstart-main-window/quickstart-main-window-give-it-a-home.png
    :align: right
    ```

    Scrollen Sie auf der Begrüßungsseite nach unten bis zu **Wählen Sie ein Hauptverzeichnis** und klicken Sie
    auf den Link **Pfad**.

2. Wählen Sie ein Verzeichnis aus oder legen Sie ein neues an, und bestätigen Sie.
3. QMapShack erstellt alle noch fehlenden Unterverzeichnisse und meldet sie für Karten, Höhendaten, POIs und die Streckenführung an. Anschließend zeigt die Begrüßungsseite das Verzeichnis unter **Aktuelles Hauptverzeichnis** an.