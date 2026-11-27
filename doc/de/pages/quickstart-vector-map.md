# Eine Garmin-Karte anzeigen

Diese Seite zeigt, wie Sie eine Garmin-Karte zu QMapShack hinzufügen, anzeigen und einstellen, was sie zeigt.

Bilder: *© OpenStreetMap contributors, created by BBBike*.

```{index} single: Karte; Garmin
```

```{index} single: Karte; Vektor
```

## Garmin-Karten

Eine Online-Karte besteht aus Bildern, die QMapShack während der Betrachtung der Karte lädt. Eine Garmin-Karte ist dagegen
eine Vektorkarte: eine Datei mit Straßen, Wegen, Gebieten und Punkten, die QMapShack selbst darstellt. Sie braucht
keine Internetverbindung.

Garmin-Karten sind .img Dateien. Dies sind die einzigen Vektorkarten, die QMapShack liest. 

Das Beispiel auf dieser
Seite ist ein Ausschnitt aus OpenStreetMap-Daten. Dieser wurde mit dem [BBBike extract service][BBBIKE] erstellt. Dort können Sie
auch einen Ausschnitt für Ihre eigene Gegend erstellen.

Zwei Arten von .img Dateien lassen sich nicht in QMapShack öffnen:

- Karten im neueren NT-Format von Garmin, das auch manche freie Karten nutzen. QMapShack meldet dann **Die Datei hat das NT Format.**
- Gesperrte Karten, wie Garmin sie vertreibt. QMapShack meldet dann, dass die Datei verschlüsselte Daten enthält.

```{index} single: Karte; hinzufügen
```

## Die Karte hinzufügen

QMapShack zeigt jede Karte an, die in einem seiner Kartenverzeichnisse liegt. Das Verzeichnis `Maps` in Ihrem
[Hauptverzeichnis][HAUPTVERZ] ist eines dieser Verzeichnisse.

1. Kopieren Sie die .img Datei in das Verzeichnis `Maps`.
2. ```{image} ../images/quickstart-vector-map/quickstart-vector-map-listed.png
    :align: right
    ```

    Nach einigen Sekunden erscheint die Karte in der Liste im Bereich **Karten** in grauer Schrift wie jede ausgeschaltete
    Karte. Sie heißt wie die zugehörige Datei, aber ohne den Dateityp und mit Leerzeichen statt Unterstrichen: `Tannheimer_Tal.img`
    steht in der Liste als **Tannheimer Tal**.
3. Um die Karte zu aktivieren, klicken Sie auf die Schaltfläche am Ende der Zeile. Die Zeile zeigt jetzt fett den Status **aktiv** an.

    ```{image} ../images/quickstart-vector-map/quickstart-vector-map-map-on.png
    :class: below-right
    ```

```{index} single: Karte; Detailstufe
```

## Die Detailstufe einstellen

Eine Garmin-Karte enthält mehrere Detailstufen. QMapShack wählt eine passend zum Zoom aus. Für jede Karte können
Sie einstellen, ob sie mehr oder weniger Details zeigt.

- ```{image} ../images/quickstart-vector-map/quickstart-vector-map-settings.png
    :align: right
    ```

    Klicken Sie auf den Pfeil links vom Namen der Karte, um deren Einstellungen zu öffnen.

- **Details** legt fest, wie viele Informationen auf der Karte angezeigt werden. Ein Wert größer als 0 zeigt bei gleicher Zoomstufe mehr Details als üblich an, ein Wert kleiner als 0 entsprechend weniger. Der Standardwert ist 0. In der Abbildung unten ist **Details** auf 5 eingestellt.

    ```{image} ../images/quickstart-vector-map/quickstart-vector-map-detail-more.png
    :class: below-right
    ```

- Aktivieren oder deaktivieren Sie **Gebiete**, **Linien** und **Punkte**, um diese Teile der Karte ein- oder
    auszublenden. Ohne Gebiete zum Beispiel
    verschwinden die Wälder, und die Wege treten hervor.

    ```{image} ../images/quickstart-vector-map/quickstart-vector-map-no-areas.png
    :class: below-right
    ```

```{index} single: Karte; TYP-Datei
```

## Das Aussehen der Karte: TYP-Dateien

Farben und Symbole einer Garmin-Karte werden in einer TYP Datei festgelegt. Die meisten Garmin-Karten enthalten eine eigene TYP-Datei. Bei der auf dieser Seite benutzten Karte ist das nicht anders.

- ```{image} ../images/quickstart-vector-map/quickstart-vector-map-typ-row.png
    :align: right
    ```
 
    Am unteren Ende der Karteneinstellungen zeigt das Feld **TYP Datei** den Namen einer externen TYP Datei an, sofern eine solche geladen wurde. Ist das Feld leer, verwendet die Karte ihre integrierte TYP Datei.
- Um eine andere TYP Datei zu laden, klicken Sie auf die Schaltfläche mit dem Ordnersymbol und wähle eine .typ Datei aus. Um wieder die integrierte TYP Datei der Karte zu verwenden, klicken Sie auf die Schaltfläche mit dem Kreuzsymbol.
- Jede TYP Datei ist für einen bestimmten Kartenstil ausgelegt. Die Verwendung einer TYP Datei, die für eine andere Karte bestimmt ist, kann zu falschen Farben und Symbolen führen.    

[BBBIKE]: https://extract.bbbike.org/
[HAUPTVERZ]: quickstart-main-window.md#das-hauptverzeichnis-einrichten