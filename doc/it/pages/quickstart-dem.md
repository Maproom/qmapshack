# Dati altimetrici e ombreggiatura

Questa pagina mostra come aggiungere dati altimetrici a QMapShack, attivarli e ombreggiare il terreno con essi.

Immagini: *© OpenStreetMap contributors*; *Datenquelle: Land Tirol - data.tirol.gv.at, CC BY 4.0*.

```{index} single: DEM
```

```{index} single: Quota; sotto il puntatore del mouse
```

## A cosa servono i dati altimetrici

I dati altimetrici, un DEM (modello digitale di elevazione), contengono la quota del terreno. QMapShack li usa in tre
modi:

- Mostra la quota di qualsiasi punto. Con un DEM attivo, la barra di stato in basso a destra mostra **Quota** e
    **Pendenza** del punto sotto il puntatore del mouse.

    ```{image} ../images/quickstart-dem/quickstart-dem-status-bar.png
    :class: below-right
    ```

- Su richiesta, aggiunge una quota a tracce e waypoint, o sostituisce quella che hanno. Una traccia che disegni
    prende le sue quote dal DEM quando la salvi; vedi
    [Disegnare una traccia](quickstart-draw-track.md#quote-dal-dem). Gli itinerari ricevono le quote dal DEM quando
    QMapShack li calcola.
- Ombreggia il terreno sulla mappa, così che montagne e valli risaltino.

```{index} single: DEM; aggiungere
```

## Aggiungere un DEM

I dati altimetrici di solito arrivano come uno o più file immagine, per esempio GeoTIFF. QMapShack non li apre
direttamente: legge un file `.vrt` (un raster virtuale), che elenca i file immagine e li unisce in un unico DEM.

1. Copia i file nella cartella `DEM` della tua
    [cartella home](quickstart-main-window.md#impostare-la-cartella-home): un singolo file direttamente
    al suo interno, più file in una sottocartella propria.
2. Fai clic su **Strumenti → Costruttore di VRT**.
3. Fai clic sul pulsante con la cartella accanto a **Seleziona i file di origine** e seleziona i file che hai copiato.
4. Fai clic sul pulsante con la cartella accanto a **Nome del file di destinazione**. La prima volta, la finestra di
    dialogo si apre nella cartella `Maps`: vai nella cartella `DEM`, inserisci un nome che termina con `.vrt` e salva.
5. Fai clic su **Avvia**.
6. ```{image} ../images/quickstart-dem/quickstart-dem-listed.png
    :align: right
    ```

    Quando il Costruttore di VRT ha finito, il DEM compare nell'elenco, grigio come ogni DEM disattivato.

Un `.vrt` creato altrove può indicare i suoi file con il percorso completo, e quindi potrebbe non trovarli dopo una
copia. Crearlo nella cartella `DEM` evita questo problema.

```{index} single: DEM; attivare
```

## Attivare il DEM

1. ```{image} ../images/quickstart-dem/quickstart-dem-dem-button.png
    :align: right
    ```

    Fai clic sul pulsante nella riga del DEM nel riquadro **DEM**. La riga passa in grassetto e
    riporta **in uso**.

2. La mappa appare come prima: il DEM non ombreggia nulla finché non attivi l'ombreggiatura. Ma dove il DEM ha
    dati, la barra di stato ora mostra la quota e la pendenza sotto il puntatore del mouse. Se non lo fa, c'è
    qualcosa che non va nel DEM.

```{index} single: DEM; ombreggiatura
```

```{index} single: Ombreggiatura
```

## Ombreggiare il terreno

1. ```{image} ../images/quickstart-dem/quickstart-dem-hillshading.png
    :align: right
    ```

    Fai clic sulla freccia a sinistra del nome del DEM. Le sue impostazioni si aprono sotto. Spunta
    **Ombreggiatura**.

2. ```{image} ../images/quickstart-dem/quickstart-dem-shaded.png
    :align: right
    ```

    Ora il terreno è ombreggiato sulla mappa: i versanti rivolti dalla parte opposta alla luce sono più scuri.

3. Il cursore sotto il nome del DEM ne regola l'opacità. QMapShack disegna il DEM sopra la mappa: se trascini il
    cursore verso destra, l'ombreggiatura diventa più intensa e la mappa più scura. Il cursore accanto a
    **Ombreggiatura** cambia il rilievo stesso: verso sinistra i versanti sembrano più ripidi, verso destra il terreno
    sembra più piatto. La luce viene sempre da nord-ovest.

    ```{image} ../images/quickstart-dem/quickstart-dem-stronger.png
    :class: below-right
    ```

Come per una mappa, puoi limitare un DEM a un intervallo di livelli di zoom con i due pulsanti accanto alla barra sotto
il cursore. Vedi
[Limitare una mappa a un intervallo di zoom](quickstart-zoom-pan.md#limitare-una-mappa-a-un-intervallo-di-zoom).
