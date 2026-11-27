# Höhendaten und Schummerung

Diese Seite beschreibt, wie Sie Höhendaten zu QMapShack hinzufügen, diese aktivieren und das Gelände damit schummern.

Bilder: *© OpenStreetMap contributors*; *Datenquelle: Land Tirol - data.tirol.gv.at, CC BY 4.0*.

```{index} single: DEM
```

```{index} single: Höhe; unter dem Mauszeiger
```

## Was Höhendaten leisten

Höhendaten, ein DEM (digitales Höhenmodell), enthalten die Höhe des Geländes. QMapShack nutzt sie auf drei Arten:

- Das DEM enthält die Höheninformationen für jeden Geländepunkt. Bei aktiviertem DEM zeigt die Statusleiste unten rechts unter **Höhe** und **Hangneigung** die entsprechenden Werte an der Position des Mauszeigers an.

    ```{image} ../images/quickstart-dem/quickstart-dem-status-bar.png
    :class: below-right
    ```
- Auf Wunsch fügt es zu Tracks und Wegpunkten Höheninformationen hinzu oder ersetzt die bereits vorhandenen. Ein von Ihnen gezeichneter Track erhält seine Höhenwerte aus dem DEM beim Speichern. Siehe [Einen Track zeichnen](quickstart-draw-track.md#höhen-aus-dem-dem). Routen erhalten ihre Höhenwerte aus dem DEM während QMapShack sie berechnet.
- Sie schummern das Gelände auf der Karte, sodass Berge und Täler hervortreten.

```{index} single: DEM; hinzufügen
```

## Ein DEM hinzufügen

Höhendaten liegen meist als eine oder mehrere Bilddateien vor, beispielsweise im GeoTIFF-Format. QMapShack kann diese Dateien nicht direkt öffnen. Stattdessen liest es eine .vrt Datei (ein virtuelles Raster), die auf die Bilddateien verweist und sie zu einem einzigen DEM zusammenfasst.

1. Kopieren Sie die Dateien in das Unterverzeichnis `DEM` Ihres [Hauptverzeichnisses](quickstart-main-window.md#das-hauptverzeichnis-einrichten). Legen Sie eine einzelne Datei direkt dort ab. Mehrere Dateien speichern Sie gemeinsam in einem eigenen Unterverzeichnis.
2. Klicken Sie auf **Werkzeug → VRT Builder**.
3. Klicken Sie auf die Ordnerschaltfläche neben **Quelldateien auswählen** und wählen Sie die zuvor kopierten Dateien aus.
4. Klicken Sie auf die Ordnerschaltfläche neben **Zieldatei auswählen**. Beim ersten Mal öffnet sich der Dialog im Verzeichnis `Maps`. Wechseln Sie in das Verzeichnis `DEM`, geben Sie einen Dateinamen mit der Endung `.vrt` ein und klicken Sie auf **Speichern**.
5. Klicken Sie auf **Start**.
6. ```{image} ../images/quickstart-dem/quickstart-dem-listed.png
    :align: right
    ```

    Sobald der VRT Builder fertig ist, erscheint das DEM in der Liste. Es wird grau dargestellt, wie jedes andere inaktive DEM.

Eine an anderer Stelle erstellte .vrt Datei kann die Quelldateien über ihre absoluten Pfade referenzieren. Werden die Dateien anschließend kopiert oder verschoben, stimmen diese Pfade möglicherweise nicht mehr. Wenn Sie die .vrt Datei direkt im Verzeichnis `DEM` erstellen, vermeiden Sie dieses Problem.

```{index} single: DEM; einschalten
```

## Das DEM einschalten

1. ```{image} ../images/quickstart-dem/quickstart-dem-dem-button.png
    :align: right
    ```

    Klicken Sie auf die Schaltfläche in der Zeile des DEM im Bereich **Dig. Höhenmodell (DEM)**. Die Zeile wird daraufhin fett dargestellt und mit **aktiv** gekennzeichnet.
2. Die Karte sieht zunächst unverändert aus. Das DEM schattiert das Gelände erst, wenn Sie die Schummerung einschalten. Wo das DEM Daten enthält, zeigt die Statusleiste jetzt die Höhe und die Hangneigung an der Position des Mauszeigers an. Werden diese Werte nicht angezeigt, stimmt möglicherweise etwas mit dem DEM nicht.

```{index} single: DEM; Schummerung
```

```{index} single: Schummerung
```

## Das Gelände schummern

1. ```{image} ../images/quickstart-dem/quickstart-dem-hillshading.png
    :align: right
    ```

    Klicken Sie auf den Pfeil links neben dem Namen des DEM, um dessen Einstellungen auszuklappen. Aktivieren Sie anschließend **Schummerung**.
2. ```{image} ../images/quickstart-dem/quickstart-dem-shaded.png
    :align: right
    ```

    Das Gelände wird jetzt auf der Karte schattiert dargestellt. Hänge, die vom Licht abgewandt sind, erscheinen dunkler.
3. Der Schieberegler unter dem Namen des DEM steuert dessen Deckkraft. QMapShack legt das DEM als Ebene über die Karte. Wenn Sie den Regler nach rechts schieben, wird die Schummerung stärker und die Karte dunkler.

    Der Schieberegler neben **Schummerung** steuert die Stärke des Reliefs. Schieben Sie ihn nach links, erscheinen die Hänge steiler, nach rechts wirkt das Gelände flacher. Die Lichtquelle kommt immer aus Nordwesten.

    ```{image} ../images/quickstart-dem/quickstart-dem-stronger.png
    :class: below-right
    ```

Wie bei Karten können Sie auch bei einem DEM den Zoombereich mit den beiden Schaltflächen neben dem Balken unterhalb des Schiebereglers einschränken. Siehe [Eine Karte auf einen Zoombereich begrenzen](quickstart-zoom-pan.md#eine-karte-auf-einen-zoombereich-begrenzen).