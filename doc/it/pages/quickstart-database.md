# Database

Questa pagina mostra come conservare i progetti in un database di QMapShack. Le immagini usano il database di esempio
`Example.db`.

```{index} single: Database; aggiungere
```

## Creare un database

Un database è un unico file che contiene i tuoi progetti, ordinati in cartelle. Il riquadro **Database** elenca i tuoi
database.

1. ```{image} ../images/quickstart-database/quickstart-database-add-menu.png
    :align: right
    ```

    Fai clic con il tasto destro in un punto vuoto del riquadro **Database** e fai clic su **Aggiungi database**.

2. ```{image} ../images/quickstart-database/quickstart-database-add-dialog.png
    :align: right
    ```

    Inserisci `Example` come **Nome** e lascia selezionato **SQLite**.

3. Fai clic sul pulsante con il segno più, evidenziato nell'immagine del passo 2. Scegli una cartella, inserisci un nome
    di file e fai clic su **Salva**.

    La finestra di dialogo si apre nella cartella in cui hai scelto un database l'ultima volta. Se hai impostato la
    [cartella home](quickstart-main-window.md#impostare-la-cartella-home), si apre nella sottocartella `Databases`
    della cartella home.

    Se hai già un file di database, fai clic invece sul pulsante della cartella a destra del pulsante con il segno più.
    Seleziona il file e fai clic su **Apri**.

4. ```{image} ../images/quickstart-database/quickstart-database-panel.png
    :align: right
    ```

    Fai clic su **OK**. Il database compare nel riquadro **Database**.

```{index} single: Database; cartella
```

## Cartelle

Un database conserva i suoi dati in cartelle. Il colore di una cartella ne indica il tipo:

- Una cartella **Gruppo** (blu) contiene altre cartelle e aiuta a strutturare i dati. Non puoi caricarla nello spazio
    di lavoro. Metti le cartelle **Progetto** in una cartella **Gruppo**, non direttamente nel database.
- Una cartella **Progetto** (verde) contiene tracce, waypoint, itinerari e aree, come un file di progetto. Puoi
    caricarla nello spazio di lavoro.
- Una cartella **Altro** (arancione) funziona come una cartella **Progetto**. Usala come preferisci. Per esempio, metti
    la registrazione di un'escursione in una cartella **Progetto** e la pianificazione in una cartella **Altro** sotto
    di essa.

1. ```{image} ../images/quickstart-database/quickstart-database-folder-menu.png
    :align: right
    ```

    Fai clic con il tasto destro sul database **Example** e fai clic su **Aggiungi cartella**.

2. ```{image} ../images/quickstart-database/quickstart-database-folder-dialog.png
    :align: right
    ```

    Inserisci `Escursioni` come **Nome cartella** e scegli **Gruppo**.

3. ```{image} ../images/quickstart-database/quickstart-database-folder-group.png
    :align: right
    ```

    Fai clic su **OK**. Il gruppo **Escursioni** compare sotto **Example**.

4. ```{image} ../images/quickstart-database/quickstart-database-folder-project.png
    :align: right
    ```

    Fai clic con il tasto destro su **Escursioni** e fai clic su **Aggiungi cartella**. Inserisci
    `Escursione alpina`, scegli **Progetto** e fai clic su **OK**.

5. ```{image} ../images/quickstart-database/quickstart-database-folders.png
    :align: right
    ```

    Fai clic con il tasto destro su **Escursione alpina** e fai clic su **Aggiungi cartella**. Inserisci
    `Pianificazione`, scegli **Altro** e fai clic su **OK**.

```{index} single: Database; importare
```

## Importare file

Puoi mettere dei file di progetto in un database senza caricarli nello spazio di lavoro.

1. ```{image} ../images/quickstart-database/quickstart-database-import-menu.png
    :align: right
    ```

    Fai clic con il tasto destro sulla cartella per i progetti, in questo esempio il gruppo **Escursioni**, e fai clic
    su **Importa dai file...**.

2. Seleziona uno o più file, per esempio `Vilsalpsee.gpx`, e fai clic su **Apri**.

3. ```{image} ../images/quickstart-database/quickstart-database-imported.png
    :align: right
    ```

    QMapShack crea una cartella **Progetto** per ogni file all'interno della cartella su cui hai fatto clic con il tasto
    destro. In questo esempio, **Vilsalpsee** compare sotto **Escursioni**. La cartella prende il nome del progetto nel
    file. QMapShack salta i progetti che sono già nel database.

```{index} single: Database; esportare
```

## Esportare in GPX

Puoi esportare un database, o una delle sue cartelle, in file GPX, per esempio per condividere i dati.

1. ```{image} ../images/quickstart-database/quickstart-database-export-menu.png
    :align: right
    ```

    Fai clic con il tasto destro sul database **Example** e fai clic su **Esporta in GPX...**.

2. ```{image} ../images/quickstart-database/quickstart-database-export-dialog.png
    :align: right
    ```

    Fai clic sul pulsante della cartella evidenziato nell'immagine e scegli una cartella sul tuo computer. Seleziona
    **Usa GPX 1.1 (senza estensioni)** per escludere i dati propri di QMapShack.

3. Fai clic su **Avvia**. QMapShack crea una cartella per il database e per ogni gruppo, e un file GPX per ogni
    cartella **Progetto** e **Altro**. Il nome del file di una cartella **Altro** inizia con il nome della cartella
    superiore.

4. Quando compare **Fatto!**, fai clic su **Chiudi**.

    ```{image} ../images/quickstart-database/quickstart-database-exported.png
    :class: below-right
    ```

```{index} single: Database; spazio di lavoro
```

## Caricare progetti dal database e salvarli

Il database conserva i tuoi dati. Li modifichi nello spazio di lavoro: carica un progetto, modificalo, salvalo di nuovo
nel database e poi toglilo dallo spazio di lavoro.

1. ```{image} ../images/quickstart-database/quickstart-database-loaded-db.png
    :align: right
    ```

    Fai clic sul pulsante di caricamento evidenziato nell'immagine, all'estremità destra della riga di **Vilsalpsee**.

2. ```{image} ../images/quickstart-database/quickstart-database-loaded-wks.png
    :align: right
    ```

    Il progetto compare nel riquadro **Spazio di lavoro** come **Vilsalpsee @ Escursioni**: il suo nome e la cartella
    in cui si trova.

3. Modifica il progetto come modificheresti un file di progetto, per esempio
    [aggiungendo un waypoint](quickstart-waypoints.md). Poi fai clic sul pulsante di salvataggio nella riga del
    progetto nel riquadro **Spazio di lavoro**. QMapShack scrive le modifiche nel database.

4. Fai di nuovo clic sul pulsante di caricamento nel riquadro **Database** per togliere il progetto dallo spazio di
    lavoro. Se il progetto ha modifiche non salvate, QMapShack chiede prima: **Salva** le salva, **No** le scarta e
    **Interrompi** lascia il progetto caricato.

Se togli un elemento da un progetto nello spazio di lavoro, rimane nel database, anche dopo il salvataggio. Per
eliminarlo da una cartella, fai clic con il tasto destro su di esso nel riquadro **Database** e fai clic su
**Cancella voce**. Un elemento che non è più in nessuna cartella passa nella cartella **Oggetti smarriti**. Se lo
elimini lì, viene cancellato definitivamente.
