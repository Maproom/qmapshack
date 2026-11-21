# Zoomen und Verschieben

Diese Seite zeigt, wie Sie sich auf der Karte bewegen: hinein- und herauszoomen, die Karte verschieben und den
Zoombereich einer Karte begrenzen.

Bilder: *© OpenStreetMap contributors*.

```{index} single: Karte; zoomen
```

## Mit dem Mausrad zoomen

- ```{image} ../images/quickstart-zoom-pan/wheel-before.png
    :align: right
    ```

    Zeigen Sie mit der Maus auf den Ort, den Sie genauer ansehen wollen, hier das Dorf Grän.

- ```{image} ../images/quickstart-zoom-pan/wheel-in.png
    :align: right
    ```

    Drehen Sie das Mausrad nach vorn, um hineinzuzoomen, und nach hinten, um herauszuzoomen. Jede Raste des Mausrads
    ist eine Zoomstufe. Der Ort unter dem Mauszeiger bleibt, wo er ist: hier zwei Rasten nach vorn, und Grän liegt
    noch unter dem Zeiger.

- Wenn Ihnen die Richtung verkehrt vorkommt, klicken Sie auf **Ansicht → Mausrad umdrehen**. QMapShack merkt sich
    diese Einstellung für den nächsten Start.

```{index} single: Karte; verschieben
```

## Die Karte verschieben

- ```{image} ../images/quickstart-zoom-pan/drag.png
    :align: right
    ```

    Drücken Sie die linke Maustaste auf der Karte, halten Sie sie und bewegen Sie die Maus. Die Karte bewegt sich mit
    dem Zeiger; lassen Sie die Taste los, wenn der gewünschte Ausschnitt zu sehen ist. Hier wird die Karte nach rechts
    gezogen, und Grän wandert mit dem Zeiger mit.

```{index} single: Karte; Tastatur
```

## Mit der Tastatur

Klicken Sie einmal auf die Karte, damit die Tasten bei ihr ankommen. Drücken Sie dann:

| Taste | Wirkung |
|---|---|
| **+** | Eine Stufe hineinzoomen, um die Mitte der Karte |
| **−** | Eine Stufe herauszoomen, um die Mitte der Karte |
| **Hoch** | Die Karte um ein Viertel des Fensters nach unten schieben, um zu zeigen, was darüber liegt |
| **Herunter** | Die Karte um ein Viertel des Fensters nach oben schieben, um zu zeigen, was darunter liegt |
| **Links** | Die Karte um ein Viertel des Fensters nach rechts schieben, um zu zeigen, was links davon liegt |
| **Rechts** | Die Karte um ein Viertel des Fensters nach links schieben, um zu zeigen, was rechts davon liegt |

**+** und **−** wirken auf den Haupttasten und auf dem Zahlenblock.

```{index} single: Karte; Maßstab
```

## Zoomstufen und Maßstab

- ```{image} ../images/quickstart-zoom-pan/scale-bar.png
    :align: right
    ```

    Der Maßstab unten rechts auf der Karte zeigt, wie lang eine Strecke bei der aktuellen Zoomstufe ist. Um ihn aus-
    oder wieder einzublenden, klicken Sie auf **Ansicht → Maßstab**.

- QMapShack zoomt in festen Stufen. Um sie zu wählen, klicken Sie auf **Ansicht → Kartenansicht einstellen** und
    wählen unter **Skalierung** eine aus. **Logarithmisch** geht durch runde Maßstäbe, etwa 1, 1,5, 2, 3, 5, 7 und
    10 Meter pro Pixel. **Quadratisch** verdoppelt oder halbiert den Maßstab mit jeder Stufe, wie es die Kacheln von
    Online-Karten tun; so werden sie scharf gezeichnet. Mit **Logarithmisch** werden Online-Karten skaliert und wirken
    unscharf. Wenn Sie die eingebauten Online-Karten wie unter [Karten einschalten](quickstart-maps.md) geholt haben,
    hat QMapShack schon **Quadratisch** gewählt.

    ```{image} ../images/quickstart-zoom-pan/scales-setup.png
    :class: below-right
    ```

```{index} single: Karte; Zoombereich
```

## Eine Karte auf einen Zoombereich begrenzen

Eine Karte lässt sich auf einen Bereich von Zoomstufen begrenzen. Außerhalb davon zeichnet QMapShack sie nicht. Eine
detaillierte Karte erscheint dann zum Beispiel erst, wenn Sie hineinzoomen.

- ```{image} ../images/quickstart-zoom-pan/range-open.png
    :align: right
    ```

    Klicken Sie im Bereich **Karten** auf den Pfeil links vom Namen der Karte. Ihre Einstellungen öffnen sich darunter.

- ```{image} ../images/quickstart-zoom-pan/range-buttons.png
    :align: right
    ```

    Der Balken unter dem Schieberegler für die Deckkraft zeigt die aktuelle Zoomstufe als kleines Kästchen. Mit der
    Schaltfläche links vom Balken wird die Karte nicht mehr gezeichnet, wenn Sie weiter hineinzoomen als jetzt. Mit der
    Schaltfläche rechts davon wird sie nicht mehr gezeichnet, wenn Sie weiter herauszoomen als jetzt.

- ```{image} ../images/quickstart-zoom-pan/range-set.png
    :align: right
    ```

    Klicken Sie auf die rechte Schaltfläche. Der Balken zeigt jetzt grün die Zoomstufen, in denen die Karte gezeichnet
    wird.

- Zoomen Sie eine Stufe weiter heraus. Die Karte wird nicht mehr gezeichnet, und der Balken zwischen dem Namen der
    Karte und ihrer Schaltfläche in der Zeile wird grau. Zoomen Sie wieder hinein, und die Karte ist wieder da.

    ```{image} ../images/quickstart-zoom-pan/range-out.png
    :class: below-right
    ```

- Um eine Grenze aufzuheben, klicken Sie ihre Schaltfläche noch einmal an.
