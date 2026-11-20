# Karten einschalten

Diese Seite zeigt, wie Sie Ihre ersten Karten in QMapShack bekommen und wie Sie sie anzeigen.

Bilder: *© OpenStreetMap contributors*; *OpenTopoMap, CC-BY-SA*; *trails by waymarkedtrails.org*.

```{index} single: Karte; online
```

## Die eingebauten Karten holen

QMapShack bringt fünf Online-Karten mit, darunter OpenStreetMap und OSM Topo. Eine Online-Karte braucht eine
Internetverbindung: QMapShack lädt die Karte, während Sie sie ansehen.

1. Klicken Sie auf **Datei → Kartenverzeichnisse angeben**. Denselben Eintrag finden Sie, wenn Sie mit der
    rechten Maustaste in die Liste im Bereich **Karten** klicken.
2. ```{image} ../images/quickstart-maps/map-path-setup.png
    :align: right
    ```

    Klicken Sie auf **Hilfe! Ich will Karten! Keine Lust die Anleitung zu lesen!**

3. QMapShack fragt **Wo wollen Sie die Karten speichern?** und schlägt schon das Verzeichnis `Maps` in Ihrem
    [Hauptverzeichnis](quickstart-main-window.md#das-hauptverzeichnis-einrichten) vor. Bestätigen Sie es. Der
    Dialog schließt sich von selbst.
4. ```{image} ../images/quickstart-maps/maps-panel.png
    :align: right
    ```

    Der Bereich **Karten** führt die Karten auf. Sie sind grau, bis Sie eine einschalten.

    Jede Karte heißt wie ihre Datei, ohne den Dateityp und mit Leerzeichen statt Unterstrichen: `OSM_Topo.tms`
    steht in der Liste als **OSM Topo**. Höhendaten und POI-Dateien werden genauso benannt.

```{index} single: Karte; einschalten
```

## Eine Karte einschalten

- ```{image} ../images/quickstart-maps/osm-button.png
    :align: right
    ```

    Um eine Karte einzuschalten, klicken Sie auf die Schaltfläche in ihrer Zeile im Bereich **Karten**. Ein weiterer
    Klick schaltet die Karte wieder aus.

- Schalten Sie **OpenStreetMap** ein. Die Zeile der Karte wird fett und zeigt **aktiv**, und die Karte ersetzt die
    Begrüßungsseite.

    ```{image} ../images/quickstart-maps/osm-on.png
    :class: below-right
    ```

```{index} single: Karte; stapeln
```

## Karten stapeln

Wenn mehrere Karten eingeschaltet sind, liegen sie in der Reihenfolge der Liste übereinander: Eine Karte verdeckt
die Karten über ihr.

- Schalten Sie auch **OSM Topo** ein. Die Karte steht in der Liste unter **OpenStreetMap** und verdeckt diese
    deshalb.

    ```{image} ../images/quickstart-maps/both-on.png
    :class: below-right
    ```

- Um die Reihenfolge zu ändern, ziehen Sie eine Karte in der Liste nach oben oder unten. Oder wählen Sie sie aus
    und drücken **Umschalt+Hoch** oder **Umschalt+Herunter**; das Kontextmenü hat dafür auch
    **Nach oben verschieben** und **Nach unten verschieben**. Steht **OSM Topo** weiter oben,
    verdeckt **OpenStreetMap** sie.

    ```{image} ../images/quickstart-maps/topo-up.png
    :class: below-right
    ```

```{index} single: Karte; Deckkraft
```

## Eine Karte durchscheinend machen

- Um die untere Karte durch die obere zu sehen, klicken Sie auf den Pfeil links vom Namen der oberen Karte. Ihre
    Einstellungen öffnen sich unter der Zeile. Ziehen Sie den Schieberegler unter dem Namen nach links, um die Deckkraft
    der Karte zu senken. Hier ist **OSM Topo** halb durchscheinend über **OpenStreetMap**.

    ```{image} ../images/quickstart-maps/topo-half.png
    :class: below-right
    ```
