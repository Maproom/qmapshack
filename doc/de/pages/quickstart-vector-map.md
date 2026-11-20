# Eine Garmin-Karte einschalten

Diese Seite zeigt, wie Sie eine Garmin-Karte zu QMapShack hinzufügen, einschalten und einstellen, was sie zeigt.

Bilder: *© OpenStreetMap contributors, created by BBBike*.

```{index} single: Karte; Garmin
```

```{index} single: Karte; Vektor
```

## Garmin-Karten

Eine Online-Karte besteht aus Bildern, die QMapShack lädt, während Sie sie ansehen. Eine Garmin-Karte ist dagegen
eine Vektorkarte: eine Datei mit Straßen, Wegen, Gebieten und Punkten, die QMapShack selbst zeichnet. Sie braucht
keine Internetverbindung.

Garmin-Karten sind `.img`-Dateien, und sie sind die einzigen Vektorkarten, die QMapShack liest. Das Beispiel auf dieser
Seite ist ein Ausschnitt aus OpenStreetMap-Daten, erstellt mit dem [BBBike extract service][BBBIKE]. Dort können Sie
auch einen für Ihre eigene Gegend erstellen.

Zwei Arten von `.img`-Dateien lassen sich nicht öffnen:

- Karten im neueren NT-Format von Garmin, das auch manche freie Karten nutzen. QMapShack meldet **Die Datei hat
    das NT Format.**
- Gesperrte Karten, wie Garmin sie verkauft. QMapShack meldet, dass die Datei verschlüsselte Daten enthält.

```{index} single: Karte; hinzufügen
```

## Die Karte hinzufügen

QMapShack zeigt jede Karte an, die direkt in einem seiner Kartenverzeichnisse liegt. Das Verzeichnis `Maps` in Ihrem
[Hauptverzeichnis](quickstart-main-window.md#das-hauptverzeichnis-einrichten) ist eines davon.

1. Kopieren Sie die `.img`-Datei in das Verzeichnis `Maps`.
2. ```{image} ../images/quickstart-vector-map/quickstart-vector-map-listed.png
    :align: right
    ```

    Nach einigen Sekunden erscheint die Karte in der Liste im Bereich **Karten**, grau wie jede ausgeschaltete
    Karte. Sie heißt wie ihre Datei, ohne den Dateityp und mit Leerzeichen statt Unterstrichen: `Tannheimer_Tal.img`
    steht in der Liste als **Tannheimer Tal**.
3. Um die Karte einzuschalten, klicken Sie auf den Knopf in ihrer Zeile. Die Zeile wird fett und zeigt
    **aktiv**.

    ```{image} ../images/quickstart-vector-map/quickstart-vector-map-map-on.png
    :class: below-right
    ```

```{index} single: Karte; Detailstufe
```

## Die Detailstufe einstellen

Eine Garmin-Karte enthält mehrere Detailstufen, und QMapShack wählt eine passend zum Zoom. Für jede Karte können
Sie einstellen, ob sie mehr oder weniger Details zeigt.

1. ```{image} ../images/quickstart-vector-map/quickstart-vector-map-settings.png
    :align: right
    ```

    Klicken Sie auf den Pfeil links vom Namen der Karte, um ihre Einstellungen zu öffnen.

- **Details** legt fest, wie viel die Karte zeigt: über 0 mehr als üblich bei derselben Zoomstufe, unter 0 weniger.
    0 ist die Voreinstellung. Im Bild unten steht **Details** auf 5.

    ```{image} ../images/quickstart-vector-map/quickstart-vector-map-detail-more.png
    :class: below-right
    ```

- Aktivieren oder deaktivieren Sie **Gebiete**, **Linien** und **Punkte**, um diese Teile der Karte ein- oder
    auszublenden. Ohne Gebiete zum Beispiel
    verschwinden die Wälder, und Wege treten hervor.

    ```{image} ../images/quickstart-vector-map/quickstart-vector-map-no-areas.png
    :class: below-right
    ```

```{index} single: Karte; TYP-Datei
```

## Das Aussehen der Karte: TYP-Dateien

Farben und Symbole einer Garmin-Karte kommen aus einer TYP-Datei. Die meisten Garmin-Karten bringen ihre eigene
mit, so auch diese.

- ```{image} ../images/quickstart-vector-map/quickstart-vector-map-typ-row.png
    :align: right
    ```

    Unten in den Einstellungen der Karte nennt **TYP Datei** eine externe TYP-Datei, sobald Sie eine laden. Ist
    das Feld leer, nutzt die Karte ihre eigene.
- Um eine andere zu laden, klicken Sie auf den Knopf mit dem Ordnersymbol und wählen eine `.typ`-Datei aus.
    Um wieder die eigene TYP-Datei der Karte zu nutzen, klicken Sie auf den Knopf mit dem Kreuzsymbol.
- Eine TYP-Datei ist für einen Kartenstil gemacht. Eine TYP-Datei für eine andere Karte kann dieser Karte
    falsche Farben und Symbole geben.

[BBBIKE]: https://extract.bbbike.org/
