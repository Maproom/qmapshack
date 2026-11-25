# Attivare le mappe

Questa pagina mostra come portare le tue prime mappe in QMapShack e come visualizzarle.

Immagini: *© OpenStreetMap contributors*; *OpenTopoMap, CC-BY-SA*; *trails by waymarkedtrails.org*.

```{index} single: Mappa; online
```

## Ottenere le mappe incluse

QMapShack include cinque mappe online, tra cui OpenStreetMap e OSM Topo. Una mappa online ha bisogno di una connessione
a internet: QMapShack carica la mappa mentre la guardi.

1. Fai clic su **File → Percorso mappe**. La stessa voce si trova nel menu che si apre facendo clic con il tasto
    destro sull'elenco nel riquadro **Mappe**.
2. ```{image} ../images/quickstart-maps/quickstart-maps-map-path-setup.png
    :align: right
    ```

    Fai clic su **Voglio le mappe! Non ho voglia di leggere la documentazione!**

3. QMapShack chiede **Dove vuoi salvare le mappe?** e ti propone già la cartella `Maps` della tua
    [cartella home](quickstart-main-window.md#impostare-la-cartella-home).
    Confermala. La finestra di dialogo **Cartella mappe** si chiude da sola.
4. ```{image} ../images/quickstart-maps/quickstart-maps-maps-panel.png
    :align: right
    ```

    Il riquadro **Mappe** elenca le mappe. Una mappa è grigia finché è disattivata.

    QMapShack dà a ogni mappa il nome del suo file, senza l'estensione e con spazi al posto dei trattini bassi:
    `OSM_Topo.tms` compare come **OSM Topo**. Allo stesso modo dà il nome ai dati altimetrici e ai file POI.

```{index} single: Mappa; attivare
```

## Attivare una mappa

1. ```{image} ../images/quickstart-maps/quickstart-maps-osm-button.png
    :align: right
    ```

    Per attivare una mappa, fai clic sul pulsante nella sua riga del riquadro **Mappe**. Fai di nuovo clic per
    disattivarla.

2. Attiva **OpenStreetMap**. La sua riga passa in grassetto e riporta **in uso**, e la mappa prende il posto della
    pagina di benvenuto.

    ```{image} ../images/quickstart-maps/quickstart-maps-osm-on.png
    :class: below-right
    ```

```{index} single: Mappa; sovrapporre
```

## Sovrapporre le mappe

Quando sono attive più mappe, si sovrappongono nell'ordine dell'elenco: una mappa copre le mappe che le stanno sopra.

1. Attiva anche **OSM Topo**. Nell'elenco si trova sotto **OpenStreetMap**, quindi **OSM Topo** copre
    **OpenStreetMap**.

    ```{image} ../images/quickstart-maps/quickstart-maps-both-on.png
    :class: below-right
    ```

2. Per cambiare l'ordine, trascina una mappa in su o in giù nell'elenco. Oppure selezionala e premi **Shift+Su** o
    **Shift+Giù** (i tasti freccia); nel menu contestuale trovi anche **Muovi su** e **Muovi giù**. Con
    **OSM Topo** spostata più in alto, **OpenStreetMap** la copre.

    ```{image} ../images/quickstart-maps/quickstart-maps-topo-up.png
    :class: below-right
    ```

```{index} single: Mappa; opacità
```

## Rendere trasparente una mappa

1. Per vedere la mappa inferiore attraverso quella superiore, fai clic sulla freccia a sinistra del nome della mappa
    superiore. Le sue impostazioni si aprono sotto la riga.
2. Trascina verso sinistra il cursore sotto il nome per ridurre l'opacità della mappa. Qui **OSM Topo** è
    semitrasparente sopra **OpenStreetMap**.

    ```{image} ../images/quickstart-maps/quickstart-maps-topo-half.png
    :class: below-right
    ```
