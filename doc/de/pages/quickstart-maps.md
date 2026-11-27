# Karten einschalten

Diese Seite zeigt, wie Sie Ihre ersten Karten in QMapShack einrichten und wie Sie sie anzeigen.

Bilder: *© OpenStreetMap contributors*; *OpenTopoMap, CC-BY-SA*; *trails by waymarkedtrails.org*.

```{index} single: Karte; online
```

## Die eingebauten Karten einrichten

QMapShack bringt fünf Online-Karten mit, darunter OpenStreetMap und OSM Topo. Eine Online-Karte braucht eine
Internetverbindung. QMapShack lädt die Karte, während Sie sie ansehen.

1. Klicken Sie auf **Datei → Kartenverzeichnisse angeben**. Denselben Eintrag finden Sie, wenn Sie mit der
    rechten Maustaste in die Liste im Bereich **Karten** klicken.
2. ```{image} ../images/quickstart-maps/quickstart-maps-map-path-setup.png
    :align: right
    ```

    Klicken Sie auf **Hilfe! Ich will Karten! Keine Lust die Anleitung zu lesen!**
3. QMapShack fragt **Wo wollen Sie die Karten speichern?** und schlägt das Verzeichnis `Maps` in Ihrem
    [Hauptverzeichnis](quickstart-main-window.md#das-hauptverzeichnis-einrichten) vor. Bestätigen Sie es. Der
    Dialog **Kartenverzeichnisse angeben** schließt sich dann automatisch.
4. ```{image} ../images/quickstart-maps/quickstart-maps-maps-panel.png
    :align: right
    ```

    Im Bereich **Karten** werden alle verfügbaren Karten aufgelistet. Nicht aktivierte Karten sind grau dargestellt.

    Jede Karte erhält den Namen ihrer Datei, wobei die Dateiendung weggelassen und Unterstriche durch Leerzeichen ersetzt werden. So erscheint `OSM_Topo.tms` in der Liste als **OSM Topo**. Höhendaten und POI-Dateien benennt QMapShack nach demselben Schema.

```{index} single: Karte; einschalten
```

## Eine Karte einschalten

1. ```{image} ../images/quickstart-maps/quickstart-maps-osm-button.png
    :align: right
    ```

    Um eine Karte zu aktivieren, klicken Sie auf die Schaltfläche rechts neben ihrem Namen im Bereich **Karten**. Ein erneuter Klick deaktiviert die Karte wieder.

2. Aktivieren Sie **OpenStreetMap**. Der Name der Karte wird nun fett dargestellt und ihr Status wechselt zu aktiv. Die Karte ersetzt daraufhin die Begrüßungsseite.

    ```{image} ../images/quickstart-maps/quickstart-maps-osm-on.png
    :class: below-right
    ```

```{index} single: Karte; stapeln
```

## Karten stapeln

Wenn mehrere Karten aktiviert sind, werden sie in der Reihenfolge ihrer Einträge übereinandergelegt. Dabei verdeckt jede Karte die Karten, die in der Liste oberhalb von ihr stehen.

1. Aktivieren Sie auch **OSM Topo**. Die Karte steht in der Liste unter **OpenStreetMap** und verdeckt diese
    deshalb.

    ```{image} ../images/quickstart-maps/quickstart-maps-both-on.png
    :class: below-right
    ```
2. Um die Reihenfolge zu ändern, ziehen Sie eine Karte in der Liste nach oben oder unten. Alternativ können Sie die Karte auswählen und **Umschalt+Pfeil nach oben** oder **Umschalt+Pfeil nach unten** drücken. Im Kontextmenü stehen dafür die Einträge **Nach oben verschieben** und **Nach unten verschieben** ebenfalls zur Verfügung. Verschieben Sie **OSM Topo** weiter nach oben, wird die Karte von **OpenStreetMap** überlagert.

    ```{image} ../images/quickstart-maps/quickstart-maps-topo-up.png
    :class: below-right
    ```

```{index} single: Karte; Deckkraft
```

## Eine Karte transparent machen

1. Um die untere Karte durch die obere hindurch sichtbar zu machen, klicken Sie auf den Pfeil links neben dem Namen der oberen Karte. Dadurch werden die Einstellungen unterhalb der Zeile eingeblendet.
2. Ziehen Sie den Schieberegler unter dem Namen nach links, um die Deckkraft der Karte zu senken. Hier ist
    **OSM Topo** halb durchscheinend über **OpenStreetMap**.

    ```{image} ../images/quickstart-maps/quickstart-maps-topo-half.png
    :class: below-right
    ```
