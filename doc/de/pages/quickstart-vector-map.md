# Eine Garmin-Karte einschalten

Diese Seite zeigt, wie Sie eine Garmin-Karte zu QMapShack hinzufügen, einschalten und einstellen, was sie zeigt.

Bilder: *© OpenStreetMap contributors, created by BBBike*.

```{index} single: Karte; Garmin
```

```{index} single: Karte; Vektor
```

## Garmin-Karten

Eine Online-Karte besteht aus Bildern, die QMapShack lädt, während Sie sie ansehen. Eine Garmin-Karte ist dagegen
eine Vektorkarte: eine Datei mit Straßen, Wegen, Flächen und Punkten, die QMapShack selbst zeichnet. Sie braucht
keine Internetverbindung.

Garmin-Karten sind `.img`-Dateien, und sie sind die einzigen Vektorkarten, die QMapShack liest. Das Beispiel auf
dieser Seite ist ein Ausschnitt aus OpenStreetMap-Daten, erstellt vom BBBike extract service.

Zwei Arten von `.img`-Dateien lassen sich nicht öffnen:

- Karten im neueren NT-Format von Garmin, das auch manche freie Karten nutzen. QMapShack meldet **Die Datei hat
    das NT Format.**
- Gesperrte Karten, wie Garmin sie verkauft. QMapShack meldet, dass die Datei verschlüsselte Daten enthält.

```{index} single: Karte; hinzufügen
```

```{index} single: Karte; neu laden
```

## Die Karte hinzufügen

QMapShack zeigt jede Karte in seinen Kartenverzeichnissen an. Das Verzeichnis `Maps` in Ihrem
[Hauptverzeichnis](quickstart-main-window.md#das-hauptverzeichnis-einrichten) ist eines davon.

1. Kopieren Sie die `.img`-Datei in das Verzeichnis `Maps`.
2. ```{image} ../images/quickstart-vector-map/reload-menu.png
    :align: right
    ```

    Klicken Sie mit der rechten Maustaste in die Liste im Bereich **Karten** und klicken Sie auf **Karten erneut
    laden**. QMapShack überwacht das Verzeichnis nicht, deshalb erscheint eine neue Datei erst danach oder beim
    nächsten Start.
3. ```{image} ../images/quickstart-vector-map/listed.png
    :align: right
    ```

    Die Karte erscheint in der Liste, grau wie jede ausgeschaltete Karte. Sie heißt wie ihre Datei, ohne den
    Dateityp und mit Leerzeichen statt Unterstrichen: `Tannheimer_Tal.img` steht in der Liste als **Tannheimer
    Tal**.
4. Um die Karte einzuschalten, klicken Sie auf die Schaltfläche in ihrer Zeile. Die Zeile wird fett und zeigt
    **aktiv**.

    ```{image} ../images/quickstart-vector-map/map-on.png
    :class: below-right
    ```

```{index} single: Karte; Detailstufe
```

## Die Detailstufe einstellen

Eine Garmin-Karte enthält mehrere Detailstufen, und QMapShack wählt eine passend zum Zoom. Diese Wahl können Sie
für jede Karte verschieben.

- ```{image} ../images/quickstart-vector-map/settings.png
    :align: right
    ```

    Klicken Sie auf den Pfeil links vom Namen der Karte, um ihre Einstellungen zu öffnen.

- **Details** legt fest, wie viel die Karte zeigt: über 0 mehr als üblich beim selben Zoom, unter 0 weniger.
    0 ist die Voreinstellung. Unten steht der Wert auf 5.

    ```{image} ../images/quickstart-vector-map/detail-more.png
    :class: below-right
    ```

- **Gebiete**, **Linien** und **Punkte** schalten diese Teile der Karte ein und aus. Ohne Gebiete zum Beispiel
    verschwinden die Wälder, und Wege treten hervor.

    ```{image} ../images/quickstart-vector-map/no-areas.png
    :class: below-right
    ```

```{index} single: Karte; TYP-Datei
```

## Das Aussehen der Karte: TYP-Dateien

Farben und Symbole einer Garmin-Karte kommen aus einer TYP-Datei. Die meisten Garmin-Karten bringen ihre eigene
mit, so auch diese.

- ```{image} ../images/quickstart-vector-map/typ-row.png
    :align: right
    ```

    Unten in den Einstellungen der Karte nennt **TYP Datei:** eine externe TYP-Datei, sobald Sie eine laden. Ist
    das Feld leer, nutzt die Karte ihre eigene.
- Um eine andere zu laden, klicken Sie auf die Schaltfläche mit dem Ordner und wählen eine `.typ`-Datei aus.
    Die Schaltfläche mit dem Kreuz kehrt zur eigenen TYP-Datei der Karte zurück.
- Eine TYP-Datei ist für einen Kartenstil gemacht. Eine für eine andere Karte kann dieser Karte falsche Farben
    und Symbole geben.
