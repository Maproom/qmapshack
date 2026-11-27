# Eine GPX Datei laden

Diese Seite beschreibt, wie Sie eine .gpx Datei in QMapShack laden, ihre Tracks und Wegpunkte ansehen und sie auf der Karte
finden.

Bilder: *© OpenStreetMap contributors*; *Datenquelle: Land Tirol - data.tirol.gv.at, CC BY 4.0*;
*`Example.gpx`: fuzzybear, CC BY-SA 4.0*.

```{index} single: GPX
```

```{index} single: Datei; laden
```

## Die Datei laden

Eine `.gpx`-Datei (GPS Exchange Format) enthält Tracks, Wegpunkte und Routen. GPS-Geräte, Apps und Tourenplanungs-Webseiten verwenden dieses Format, um solche Daten zu speichern und auszutauschen. Die Abbildungen auf dieser Seite zeigen die Datei `Example.gpx` mit einer Tour rund um den Aggenstein.

1. Klicken Sie auf **Datei → GIS Daten laden** oder drücken Sie **Strg+L**.
2. Wählen Sie die .gpx Datei aus und klicken Sie auf **Öffnen**. Um mehrere Dateien gleichzeitig zu laden, halten Sie die Taste **Strg** gedrückt und klicken Sie nacheinander auf die gewünschten Dateien.

Der Dialog öffnet sich in dem Verzeichnis, aus dem Sie zuletzt eine Datei geladen oder in dem Sie zuletzt eine Datei gespeichert haben. Wenn Sie das [Hauptverzeichnis](quickstart-main-window.md#das-hauptverzeichnis-einrichten) eingerichtet haben, öffnet QMapShack den Dialog direkt im Unterverzeichnis `GPX`. Dort können Sie Ihre .gpx Dateien übersichtlich ablegen.

Alternativ zum Menü können Sie Dateien einfach aus Ihrem Dateimanager auf die Karte ziehen.

QMapShack kann auch eigene .qms Dateien sowie Dateien anderer Geräte und Programme in den Formaten .fit, .tcx, .slf, .sml, .log und .qlb laden.

```{index} single: Projekt
```

```{index} single: Arbeitsplatz
```

## Das Projekt im Arbeitsplatz

QMapShack lädt jede Datei als *Projekt*. Darin werden alle Tracks, Routen, Wegpunkte und Gebiete der Datei zusammengefasst. Der Bereich **Arbeitsplatz** (siehe [Das Hauptfenster](quickstart-main-window.md#das-fenster-beim-ersten-start)) zeigt das Projekt unter dem in der Datei gespeicherten Namen an. Ist dort kein Name hinterlegt, wird stattdessen der Dateiname verwendet. 

Alle Elemente des Projekts werden sofort auf der Karte angezeigt.

1. ```{image} ../images/quickstart-load-file/quickstart-load-file-items.png
    :align: right
    ```

    Klicken Sie auf den Pfeil links neben dem Projektnamen, um das Projekt aufzuklappen. Darunter erscheinen seine Tracks, Routen, Wegpunkte und Gebiete. Tracks, Routen und Gebiete sind jeweils durch ein Symbol gekennzeichnet, während Wegpunkte ihr zugewiesenes Symbol anzeigen.
2. Bewegen Sie den Mauszeiger über den Namen eines Elements (Track, Route, Wegpunkt oder Gebiet). Daraufhin erscheint ein Tooltip mit einer kurzen Zusammenfassung des Elements.

```{index} single: Element; auf der Karte finden
```

## Ein Element auf der Karte finden

```{image} ../images/quickstart-load-file/quickstart-load-file-dclick.png
:align: right
```

Doppelklicken Sie im Bereich **Arbeitsplatz** auf ein Element, um die Karte darauf zu zoomen.

Bei einem Track hebt QMapShack diesen zusätzlich auf der Karte hervor. Ein Kasten unten rechts zeigt eine Zusammenfassung mit Angaben zu Länge, Höhengewinn, Dauer, Geschwindigkeit und weiteren Werten. Ein kleines Diagramm unten links stellt das Höhenprofil des Tracks dar.

```{image} ../images/quickstart-load-file/quickstart-load-file-zoomed.png
:class: below-right
```

```{index} single: Projekt; auf der Karte ausblenden
```

## Ein Projekt auf der Karte ausblenden

1. ```{image} ../images/quickstart-load-file/quickstart-load-file-hide.png
    :align: right
    ```

    Klicken Sie auf die Augenschaltfläche rechts in der Projektzeile. Alle Elemente des Projekts werden daraufhin auf der Karte ausgeblendet. Das Projekt und seine Elemente erscheinen nun grau.
2. Klicken Sie erneut auf die Augenschaltfläche, um das Projekt wieder auf der Karte einzublenden.

Solange ein Projekt ausgeblendet ist, hat ein Doppelklick auf eines seiner Elemente keine Auswirkung auf den Kartenausschnitt.

```{index} single: Arbeitsplatz; für den nächsten Start behalten
```

## Den Arbeitsplatz behalten

QMapShack speichert die Projekte im Bereich **Arbeitsplatz**, so dass sie beim nächsten Programmstart wieder zur Verfügung stehen.

Dazu legt QMapShack eine eigene Kopie jedes Projekts an und lässt die ursprünglich geladene Datei unverändert. 

Änderungen werden erst dann in die Datei übernommen, wenn Sie das Projekt speichern.

QMapShack speichert den Arbeitsplatz alle fünf Minuten sowie beim Beenden des Programms. So ändern Sie das Speicherintervall:

1. Klicken Sie auf **Arbeitsplatz → Arbeitsplatz konfigurieren**.
2. Legen Sie im Reiter **Allgemein** neben **Arbeitsplatz beim Beenden speichern, und alle** das gewünschte Speicherintervall in Minuten fest. Bei einem Wert von `0` speichert QMapShack den Arbeitsplatz nur beim Beenden des Programms.
3. Klicken Sie auf **OK**, bestätigen Sie die Meldung und starten Sie QMapShack neu. Die neue Einstellung wird erst nach einem Neustart wirksam.

Wenn Sie das Kontrollkästchen vor **Arbeitsplatz beim Beenden speichern, und alle** deaktivieren, speichert QMapShack den Arbeitsplatz nicht mehr automatisch. Beim nächsten Programmstart wird der zuletzt gespeicherte Zustand des Arbeitsplatzes wiederhergestellt.