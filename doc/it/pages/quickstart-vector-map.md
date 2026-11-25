# Attivare una mappa Garmin

Questa pagina mostra come aggiungere una mappa Garmin a QMapShack, attivarla e regolare ciò che mostra.

Immagini: *© OpenStreetMap contributors, created by BBBike*.

```{index} single: Mappa; Garmin
```

```{index} single: Mappa; vettoriale
```

## Mappe Garmin

Una mappa online è un insieme di immagini che QMapShack carica mentre la guardi. Una mappa Garmin, invece, è una
mappa vettoriale: un file di strade, sentieri, aree e punti che QMapShack disegna da sé. Non ha bisogno di una
connessione a internet.

Le mappe Garmin sono file `.img`, e sono le uniche mappe vettoriali che QMapShack legge. L'esempio di questa pagina è
un estratto di dati OpenStreetMap, creato con il [servizio di estrazione di BBBike][BBBIKE]. Lì puoi crearne uno per
la tua zona.

QMapShack non può aprire due tipi di file `.img`:

- Mappe nel più recente formato NT di Garmin, usato anche da alcune mappe libere. QMapShack segnala
    **Il file è in formato NT.**
- Mappe bloccate, come quelle vendute da Garmin. QMapShack segnala che il file contiene dati bloccati o cifrati.

```{index} single: Mappa; aggiungere
```

## Aggiungere la mappa

QMapShack elenca ogni mappa che si trova direttamente in una delle cartelle mappe registrate. La cartella `Maps`
della tua [cartella home](quickstart-main-window.md#impostare-la-cartella-home) è una di queste.

1. Copia il file `.img` nella cartella `Maps`.
2. ```{image} ../images/quickstart-vector-map/quickstart-vector-map-listed.png
    :align: right
    ```

    Dopo qualche secondo la mappa compare nell'elenco del riquadro **Mappe**, grigia come ogni mappa disattivata. Ha
    il nome del suo file, senza l'estensione e con spazi al posto dei trattini bassi: QMapShack elenca
    `Tannheimer_Tal.img` come **Tannheimer Tal**.
3. Per attivare la mappa, fai clic sul pulsante nella sua riga. La riga passa in grassetto e riporta **in uso**.

    ```{image} ../images/quickstart-vector-map/quickstart-vector-map-map-on.png
    :class: below-right
    ```

```{index} single: Mappa; dettaglio
```

## Regolare il dettaglio

Una mappa Garmin contiene diversi livelli di dettaglio, e QMapShack ne sceglie uno adatto allo zoom. Puoi modificare
questa scelta per ogni mappa.

1. ```{image} ../images/quickstart-vector-map/quickstart-vector-map-settings.png
    :align: right
    ```

    Fai clic sulla freccia a sinistra del nome della mappa per aprire le sue impostazioni.

- **Dettaglio** stabilisce quanti dettagli mostra la mappa: con valori sopra 0 ne mostra più del solito allo stesso
    zoom, sotto 0 meno. 0 è il valore predefinito. Nell'immagine qui sotto, **Dettaglio** è 5.

    ```{image} ../images/quickstart-vector-map/quickstart-vector-map-detail-more.png
    :class: below-right
    ```

- Attiva o disattiva **Aree**, **Linee** e **Punti** per mostrare o nascondere queste parti della mappa. Senza aree,
    per esempio, i boschi scompaiono e i sentieri risaltano.

    ```{image} ../images/quickstart-vector-map/quickstart-vector-map-no-areas.png
    :class: below-right
    ```

```{index} single: Mappa; file TYP
```

## L'aspetto della mappa: file TYP

I colori e i simboli di una mappa Garmin vengono da un file TYP. La maggior parte delle mappe Garmin include il
proprio, come questa.

- ```{image} ../images/quickstart-vector-map/quickstart-vector-map-typ-row.png
    :align: right
    ```

    In fondo alle impostazioni della mappa, **File TYP** indica un file TYP esterno, una volta che ne carichi uno.
    Se il campo è vuoto, la mappa usa il proprio.
- Per caricarne un altro, fai clic sul pulsante con l'icona della cartella e seleziona un file `.typ`. Fai clic sul
    pulsante con l'icona della croce per tornare a usare il file TYP della mappa.
- Un file TYP è fatto per uno stile di mappa. Uno fatto per un'altra mappa può dare a questa mappa colori e simboli
    sbagliati.

[BBBIKE]: https://extract.bbbike.org/
