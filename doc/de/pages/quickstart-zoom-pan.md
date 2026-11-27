# Zoomen und Verschieben

Diese Seite zeigt, wie Sie auf der Karte navigieren: hinein- und herauszoomen, die Karte verschieben und den
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

    Drehen Sie das Mausrad nach vorn, um hineinzuzoomen, und nach hinten, um herauszuzoomen. Jede Mausradrastung entspricht einer Zoomstufe. Der Ort unter dem Mauszeiger bleibt dabei an derselben Stelle. Im Beispiel wurde das Mausrad zwei Rastungen nach vorn gedreht, Grän liegt weiterhin unter dem Mauszeiger.

Wenn Ihnen die Richtung verkehrt vorkommt, klicken Sie auf **Ansicht → Mausrad umdrehen**. QMapShack merkt sich
diese Einstellung für den nächsten Start.

```{index} single: Karte; verschieben
```

## Die Karte verschieben

- ```{image} ../images/quickstart-zoom-pan/quickstart-zoom-pan-drag.png
    :align: right
    ```

    Ziehen Sie die Karte mit der linken Maustaste: Halten Sie die Taste gedrückt und bewegen Sie die Maus. Die Karte
    bewegt sich mit dem Zeiger. Lassen Sie die Taste los, wenn der gewünschte Ausschnitt zu sehen ist. Hier ziehen Sie
    die Karte nach rechts, und Grän wandert mit dem Zeiger mit.

```{index} single: Karte; Tastatur
```

## Mit der Tastatur

Klicken Sie einmal auf die Karte, damit sie den Tastaturfokus erhält. Drücken Sie dann:

| Taste | Wirkung |
|---|---|
| **+** | Eine Stufe hineinzoomen, wobei die Kartenmitte an derselben Stelle bleibt |
| **-** | Eine Stufe herauszoomen, wobei die Kartenmitte an derselben Stelle bleibt |
| **Hoch** | Die Karte um ein Viertel der Ansicht nach unten verschieben, um den Bereich darüber zu zeigen |
| **Herunter** | Die Karte um ein Viertel der Ansicht nach oben verschieben, um den Bereich darunter zu zeigen |
| **Links** | Die Karte um ein Viertel der Ansicht nach rechts verschieben, um den Bereich links davon zu zeigen |
| **Rechts** | Die Karte um ein Viertel der Ansicht nach links verschieben, um den Bereich rechts davon zu zeigen. |

**+** und **-** funktionieren auf der Haupttastatur und auf dem Ziffernblock.

```{index} single: Karte; Maßstab
```

## Zoomstufen und Maßstab

- ```{image} ../images/quickstart-zoom-pan/quickstart-zoom-pan-scale-bar.png
    :align: right
    ```

    Der Maßstabsbalken unten rechts auf der Karte zeigt, welcher tatsächlichen Entfernung seine Länge bei der aktuellen Zoomstufe entspricht.

    Um ihn aus- oder wieder einzublenden, klicken Sie auf **Ansicht → Maßstab**.
- QMapShack zoomt in festen Stufen. Um die Zoomstufen festzulegen, klicken Sie auf **Ansicht → Kartenansicht einstellen** und wählen unter **Skalierung** eine Option aus.

    - **Logarithmisch** verwendet gerundete Maßstäbe, etwa 1, 1.5, 2, 3, 5, 7 und 10 Meter pro Pixel.
    - **Quadratisch (optimiert für TMS und WMTS Kacheln)** verdoppelt oder halbiert den Maßstab mit jeder Zoomstufe. 
    
        Online-Karten bestehen aus quadratischen Bildern, den sogenannten Kacheln. Bei diesen Zoomstufen kann QMapShack die Kacheln unverändert und damit scharf darstellen. Bei **Logarithmisch** muss QMapShack die Kacheln skalieren, wodurch die Karte unscharf wirkt.

        Wenn Sie die eingebauten Online-Karten, wie unter [Karten einschalten](quickstart-maps.md) beschrieben, eingerichtet haben, hat QMapShack bereits **Quadratisch** ausgewählt.
        
    ```{image} ../images/quickstart-zoom-pan/quickstart-zoom-pan-scales-setup.png
    :class: below-right
    ```

```{index} single: Karte; Zoombereich
```

## Eine Karte auf einen Zoombereich begrenzen

Sie können festlegen, in welchem Zoomstufenbereich eine Karte angezeigt wird. Außerhalb dieses Bereichs blendet QMapShack die Karte aus. So können Sie beispielsweise erreichen, dass eine detaillierte Karte erst beim Hineinzoomen erscheint.

1. ```{image} ../images/quickstart-zoom-pan/quickstart-zoom-pan-range-open.png
    :align: right
    ```

    Klicken Sie im Bereich **Karten** auf den Pfeil links neben dem Kartennamen, um die zugehörigen Einstellungen aufzuklappen.
2. ```{image} ../images/quickstart-zoom-pan/quickstart-zoom-pan-range-buttons.png
    :align: right
    ```

    Der Balken unter dem Schieberegler für die Deckkraft zeigt die aktuelle Zoomstufe durch eine Markierung an. 
    
    Mit der linken Schaltfläche legen Sie die aktuelle Zoomstufe als Grenze fürs Hineinzoomen fest. Bei stärkerem Zoom blendet QMapShack die Karte aus. Mit der rechten Schaltfläche legen Sie entsprechend die Grenze fürs Herauszoomen fest. Klicken Sie erneut auf eine Schaltfläche, um die jeweilige Grenze aufzuheben.
3. ```{image} ../images/quickstart-zoom-pan/quickstart-zoom-pan-range-set.png
    :align: right
    ```

    Klicken Sie auf die rechte Schaltfläche. Der grüne Bereich des Balkens zeigt nun die Zoomstufen an, bei denen QMapShack die Karte darstellt.
4. Zoomen Sie eine weitere Stufe heraus. Die Markierung auf dem Balken verlässt den grünen Bereich, und QMapShack blendet die Karte aus. Wenn Sie wieder hineinzoomen, wird die Karte erneut angezeigt.

    ```{image} ../images/quickstart-zoom-pan/quickstart-zoom-pan-range-out.png
    :class: below-right
    ```

