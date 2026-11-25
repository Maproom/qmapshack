# Salvare il lavoro

Questa pagina mostra come salvare i tuoi progetti in un file o in un database.

```{index} single: Progetto; salvare
```

## Vedere le modifiche non salvate

Al riavvio, QMapShack ripristina tutti i progetti del riquadro **Spazio di lavoro**. Per farlo tiene una propria
copia di ogni progetto. Il file da cui è stato caricato un progetto cambia solo quando salvi il progetto. Un progetto
che hai creato tu non ha un file finché non lo salvi.

1. ```{image} ../images/quickstart-save/quickstart-save-changed.png
    :align: right
    ```

    Nel riquadro **Spazio di lavoro**, cerca il progetto con modifiche non salvate: mostra un pulsante di salvataggio
    nella sua riga, evidenziato nell'immagine. In questo esempio, **I miei waypoint** ha un nuovo waypoint ed
    **Example** non ha modifiche.

2. Fai clic sul pulsante di salvataggio. Se il progetto non è mai stato salvato, QMapShack ti chiede prima dove
    salvarlo; vedi [Salvare in un file](#salvare-in-un-file). Dopo il salvataggio, il pulsante scompare.

Nella riga di un elemento, lo stesso simbolo non è un pulsante ma un indicatore di modifica. Mostra che l'elemento è
cambiato dall'ultimo salvataggio del progetto.

## Salvare in un file

1. Fai clic con il tasto destro sul progetto nel riquadro **Spazio di lavoro** e fai clic su **Salva come...**.
2. Nella finestra di dialogo, scegli il tipo di file, inserisci un nome di file e fai clic su **Salva**.

Se il progetto ha già un file, fai clic su **Salva** nello stesso menu. QMapShack sovrascrive il file senza chiedere.

I tipi di file:

- **QMapShack Binary (\*.qms)** è il formato proprio di QMapShack. Conserva tutto ciò che QMapShack sa di un progetto,
    compreso l'elenco delle modifiche, così puoi annullarle. Usalo per i tuoi progetti.
- **GPS Exchange Format (\*.gpx \*.GPX)**: lo leggono anche altri programmi e dispositivi. Conserva l'elenco delle
    modifiche, ma dopo aver ricaricato il file non puoi più annullarle.

**Salva come GPX 1.1 senza estensioni...** nel menu del progetto scrive un semplice file GPX 1.1 senza i dati propri di
QMapShack. Serve per i programmi che non leggono le estensioni di QMapShack.

Se salvi un progetto in un altro formato, QMapShack ne scrive una copia. Il progetto mantiene il proprio file e il
suo pulsante di salvataggio.

**File → Salva tutti i dati GIS** (**Ctrl+S**) salva tutti i progetti dello spazio di lavoro. Per un progetto mai
salvato, QMapShack chiede dove salvarlo.

Quando chiudi un progetto con modifiche non salvate, QMapShack chiede se vuoi salvarlo prima.

```{index} single: Progetto; salvare in un database
```

## Salvare in un database

Per salvare nella sua cartella un progetto caricato da un database, fai clic sul suo pulsante di salvataggio; vedi
[Caricare progetti dal database e salvarli](quickstart-database.md#caricare-progetti-dal-database-e-salvarli). Per
salvare un file in un database, copia nel database il suo progetto dallo spazio di lavoro, come mostrato sotto. Puoi
anche importare il file; vedi [Importare file](quickstart-database.md#importare-file).

1. ```{image} ../images/quickstart-save/quickstart-save-copy-menu.png
    :align: right
    ```

    Fai clic con il tasto destro sul progetto nel riquadro **Spazio di lavoro** e fai clic su **Copia in...**.

2. ```{image} ../images/quickstart-save/quickstart-save-copy-dialog.png
    :align: right
    ```

    Nella finestra di dialogo **Seleziona un progetto...**, inserisci un nome per il nuovo progetto, in questo esempio
    `Giro del lago`. Scegli **Database** e fai clic su **OK**.

3. ```{image} ../images/quickstart-save/quickstart-save-copy-folder.png
    :align: right
    ```

    Nella finestra di dialogo **Seleziona cartella superiore...**, fai clic sulla freccia a sinistra del database per
    espanderlo. Seleziona la cartella per il progetto, in questo esempio il gruppo **Projects**, e fai clic su **OK**.

4. Nella finestra di dialogo **Cartella database...**, lascia selezionato **Progetto** e fai clic su **OK**.

5. ```{image} ../images/quickstart-save/quickstart-save-copied.png
    :align: right
    ```

    La copia compare nel riquadro **Spazio di lavoro** come **Giro del lago @ Projects**, con un pulsante di
    salvataggio.

6. ```{image} ../images/quickstart-save/quickstart-save-copied-db.png
    :align: right
    ```

    Nel riquadro **Database** compare anche la cartella **Giro del lago**. È ancora vuota.

7. Fai clic sul pulsante di salvataggio di **Giro del lago @ Projects** nel riquadro **Spazio di lavoro**. QMapShack
    scrive gli elementi nella cartella.

La finestra di dialogo **Seleziona un progetto...** del passo 2 si apre anche da
**Spazio di lavoro → Aggiungi progetto vuoto** e quando aggiungi un nuovo elemento a un nuovo progetto. Scegli lì
**Database** per creare quel progetto in un database.
