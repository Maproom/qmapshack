# Höhendaten und Schummerung

Diese Seite zeigt, wie Sie Höhendaten zu QMapShack hinzufügen, sie einschalten und das Gelände damit schummern.

Bilder: *© OpenStreetMap contributors*; *Datenquelle: Land Tirol - data.tirol.gv.at, CC BY 4.0*.

```{index} single: DEM
```

```{index} single: Höhe; unter dem Mauszeiger
```

## Was Höhendaten leisten

Höhendaten, ein DEM (digitales Höhenmodell), enthalten die Höhe des Geländes. QMapShack nutzt sie auf drei Arten:

- Das DEM zeigt die Höhe jedes Punktes. Mit eingeschaltetem DEM zeigt die Statusleiste unten rechts bei **Höhe** und
    **Hangneigung** die Höhe und die Neigung am Mauszeiger.

    ```{image} ../images/quickstart-dem/quickstart-dem-status-bar.png
    :class: below-right
    ```

- Auf Wunsch fügt es Tracks und Wegpunkten eine Höhe hinzu oder ersetzt die vorhandene. Ein Track, den Sie
    zeichnen, bekommt seine Höhen daraus, wenn Sie ihn speichern; siehe
    [Einen Track zeichnen](quickstart-draw-track.md#höhen-aus-dem-dem). Routen bekommen ihre Höhen daraus, wenn
    QMapShack sie berechnet.
- Es schummert das Gelände auf der Karte, sodass Berge und Täler hervortreten.

```{index} single: DEM; hinzufügen
```

## Ein DEM hinzufügen

Höhendaten liegen meist als eine oder mehrere Bilddateien vor, zum Beispiel GeoTIFF. QMapShack öffnet sie nicht direkt:
Es liest eine `.vrt`-Datei (ein virtuelles Raster), die die Bilddateien auflistet und zu einem DEM zusammenfügt.

1. Kopieren Sie die Dateien in das Verzeichnis `DEM` Ihres
    [Hauptverzeichnisses](quickstart-main-window.md#das-hauptverzeichnis-einrichten): eine einzelne Datei direkt
    hinein, mehrere Dateien in ein eigenes Unterverzeichnis.
2. Klicken Sie auf **Werkzeug → VRT Builder**.
3. Klicken Sie auf den Knopf mit dem Ordner neben **Quelldateien auswählen** und wählen Sie die kopierten Dateien
    aus.
4. Klicken Sie auf den Knopf mit dem Ordner neben **Zieldatei auswählen**. Beim ersten Mal öffnet sich der Dialog im
    Verzeichnis `Maps`: Wechseln Sie in das Verzeichnis `DEM`, geben Sie einen Namen mit der Endung `.vrt` ein und
    speichern Sie.
5. Klicken Sie auf **Start**.
6. ```{image} ../images/quickstart-dem/quickstart-dem-listed.png
    :align: right
    ```

    Wenn der VRT Builder fertig ist, steht das DEM in der Liste, grau wie jedes ausgeschaltete DEM.

Eine anderswo erstellte `.vrt`-Datei kann ihre Dateien mit dem vollen Pfad nennen und findet sie nach dem Kopieren
dann möglicherweise nicht mehr. Wenn Sie sie im Verzeichnis `DEM` erstellen, passiert das nicht.

```{index} single: DEM; einschalten
```

## Das DEM einschalten

1. ```{image} ../images/quickstart-dem/quickstart-dem-dem-button.png
    :align: right
    ```

    Klicken Sie auf den Knopf in der Zeile des DEM im Bereich **Dig. Höhenmodell (DEM)**. Die Zeile wird fett und
    zeigt **aktiv**.

2. Die Karte sieht aus wie vorher: Das DEM schummert nichts, bis Sie die Schummerung einschalten. Aber wo das DEM
    Daten hat, zeigt die Statusleiste jetzt die Höhe und die Neigung am Mauszeiger.
    Wenn nicht, stimmt etwas mit dem DEM nicht.

```{index} single: DEM; Schummerung
```

```{index} single: Schummerung
```

## Das Gelände schummern

1. ```{image} ../images/quickstart-dem/quickstart-dem-hillshading.png
    :align: right
    ```

    Klicken Sie auf den Pfeil links vom Namen des DEM. Seine Einstellungen öffnen sich darunter. Aktivieren Sie
    **Schummerung**.

2. ```{image} ../images/quickstart-dem/quickstart-dem-shaded.png
    :align: right
    ```

    Das Gelände ist jetzt auf der Karte geschummert: Hänge, die vom Licht abgewandt sind, sind dunkler.

3. Der Schieberegler unter dem Namen des DEM stellt seine Deckkraft ein. QMapShack zeichnet das DEM über die Karte.
    Ziehen Sie den Regler deshalb nach rechts, wird die Schummerung stärker und die Karte dunkler. Der Schieberegler
    neben **Schummerung** ändert das Relief selbst: nach links wirken die Hänge steiler, nach rechts das Gelände
    flacher. Das Licht kommt immer aus Nordwesten.

    ```{image} ../images/quickstart-dem/quickstart-dem-stronger.png
    :class: below-right
    ```

Wie eine Karte lässt sich ein DEM auf bestimmte Zoomstufen begrenzen, mit den zwei Knöpfen neben dem Balken
unter dem Schieberegler. Siehe
[Eine Karte auf einen Zoombereich begrenzen](quickstart-zoom-pan.md#eine-karte-auf-einen-zoombereich-begrenzen).
