# Zoomen und Verschieben

Diese Seite zeigt, wie Sie sich auf der Karte bewegen: hinein- und herauszoomen, die Karte verschieben und den
Zoombereich einer Karte begrenzen.

Bilder: *© OpenStreetMap contributors*.

```{index} single: Karte; zoomen
```

## Mit dem Mausrad zoomen

1. ```{image} ../images/quickstart-zoom-pan/quickstart-zoom-pan-wheel-before.png
    :align: right
    ```

    Zeigen Sie mit der Maus auf den Ort, den Sie genauer ansehen wollen, hier das Dorf Grän.

2. ```{image} ../images/quickstart-zoom-pan/quickstart-zoom-pan-wheel-in.png
    :align: right
    ```

    Drehen Sie das Mausrad nach vorn, um hineinzuzoomen, und nach hinten, um herauszuzoomen. Jede Raste des Mausrads
    ist eine Zoomstufe. Der Ort unter dem Mauszeiger bleibt, wo er ist: hier zwei Rasten nach vorn, und Grän liegt
    noch unter dem Zeiger.

Wenn Ihnen die Richtung verkehrt vorkommt, klicken Sie auf **Ansicht → Mausrad umdrehen**. QMapShack merkt sich
diese Einstellung für den nächsten Start.

```{index} single: Karte; verschieben
```

## Die Karte verschieben

- ```{image} ../images/quickstart-zoom-pan/quickstart-zoom-pan-drag.png
    :align: right
    ```

    Ziehen Sie die Karte mit der linken Maustaste: Halten Sie die Taste gedrückt und bewegen Sie die Maus. Die Karte
    bewegt sich mit dem Zeiger; lassen Sie die Taste los, wenn der gewünschte Ausschnitt zu sehen ist. Hier ziehen Sie
    die Karte nach rechts, und Grän wandert mit dem Zeiger mit.

```{index} single: Karte; Tastatur
```

## Mit der Tastatur

Klicken Sie einmal auf die Karte, damit sie die Tastendrücke erhält. Drücken Sie dann:

| Taste | Wirkung |
|---|---|
| **+** | Eine Stufe hineinzoomen; die Kartenmitte bleibt stehen |
| **-** | Eine Stufe herauszoomen; die Kartenmitte bleibt stehen |
| **Hoch** | Die Karte um ein Viertel der Kartenansicht nach unten schieben, um zu zeigen, was darüber liegt |
| **Herunter** | Die Karte um ein Viertel der Kartenansicht nach oben schieben, um zu zeigen, was darunter liegt |
| **Links** | Die Karte um ein Viertel der Kartenansicht nach rechts schieben, um zu zeigen, was links davon liegt |
| **Rechts** | Die Karte um ein Viertel der Kartenansicht nach links schieben, um zu zeigen, was rechts davon liegt |

**+** und **-** funktionieren auf der Haupttastatur und auf dem Ziffernblock.

```{index} single: Karte; Maßstab
```

## Zoomstufen und Maßstab

- ```{image} ../images/quickstart-zoom-pan/quickstart-zoom-pan-scale-bar.png
    :align: right
    ```

    Der Maßstab unten rechts auf der Karte zeigt, wie lang eine Strecke bei der aktuellen Zoomstufe ist. Um ihn aus-
    oder wieder einzublenden, klicken Sie auf **Ansicht → Maßstab**.

- QMapShack zoomt in festen Stufen. Um sie zu wählen, klicken Sie auf **Ansicht → Kartenansicht einstellen** und wählen
    unter **Skalierung** eine aus. **Logarithmisch** durchläuft runde Maßstäbe, etwa 1, 1,5, 2, 3, 5, 7 und 10 Meter pro
    Pixel. **Quadratisch (optimiert für TMS und WMTS Kacheln)** verdoppelt oder halbiert den Maßstab mit jeder Stufe.
    Online-Karten bestehen aus quadratischen Bildern, den Kacheln, und mit diesen Stufen zeichnet QMapShack sie scharf.
    Mit **Logarithmisch** skaliert QMapShack Online-Karten, und sie wirken unscharf. Wenn Sie die eingebauten
    Online-Karten wie unter [Karten einschalten](quickstart-maps.md) beschrieben eingerichtet haben, hat QMapShack
    schon **Quadratisch** gewählt.

    ```{image} ../images/quickstart-zoom-pan/quickstart-zoom-pan-scales-setup.png
    :class: below-right
    ```

```{index} single: Karte; Zoombereich
```

## Eine Karte auf einen Zoombereich begrenzen

Sie können eine Karte auf bestimmte Zoomstufen begrenzen. Außerhalb davon zeichnet QMapShack sie nicht. Eine
detaillierte Karte erscheint dann zum Beispiel erst, wenn Sie hineinzoomen.

1. ```{image} ../images/quickstart-zoom-pan/quickstart-zoom-pan-range-open.png
    :align: right
    ```

    Klicken Sie im Bereich **Karten** auf den Pfeil links vom Namen der Karte. Ihre Einstellungen öffnen sich darunter.

2. ```{image} ../images/quickstart-zoom-pan/quickstart-zoom-pan-range-buttons.png
    :align: right
    ```

    Der Balken unter dem Schieberegler für die Deckkraft zeigt die aktuelle Zoomstufe mit einer Markierung. Der
    Knopf links vom Balken macht die aktuelle Zoomstufe zur Grenze fürs Hineinzoomen: Weiter hinein zeichnet
    QMapShack die Karte nicht mehr. Der Knopf rechts davon tut dasselbe fürs Herauszoomen. Klicken Sie erneut auf
    einen Knopf, um seine Grenze aufzuheben.

3. ```{image} ../images/quickstart-zoom-pan/quickstart-zoom-pan-range-set.png
    :align: right
    ```

    Klicken Sie auf den rechten Knopf. Der Balken zeigt jetzt grün die Zoomstufen, bei denen QMapShack die Karte
    zeichnet.

4. Zoomen Sie eine Stufe weiter heraus. Die Markierung auf dem Balken verlässt den grünen Abschnitt, und QMapShack
    zeichnet die Karte nicht mehr. Zoomen Sie wieder hinein, und die Karte ist wieder da.

    ```{image} ../images/quickstart-zoom-pan/quickstart-zoom-pan-range-out.png
    :class: below-right
    ```

Um eine Grenze aufzuheben, klicken Sie noch einmal auf ihren Knopf.
