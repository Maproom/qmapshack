# Aggiungere waypoint

Questa pagina mostra come aggiungere un waypoint alla mappa. Le immagini usano la mappa di
[Attivare le mappe](quickstart-maps.md), il DEM di [Dati altimetrici e ombreggiatura](quickstart-dem.md) e
`Example.gpx` di [Caricare un file GPX](quickstart-load-file.md).

Immagini: *© OpenStreetMap contributors*; *Datenquelle: Land Tirol - data.tirol.gv.at, CC BY 4.0*;
*`Example.gpx`: fuzzybear, CC BY-SA 4.0*.

```{index} single: Waypoint; aggiungere
```

## Aggiungere un waypoint

1. ```{image} ../images/quickstart-waypoints/quickstart-waypoints-add-waypoint.png
    :align: right
    ```

    Fai clic con il tasto destro sul punto della mappa in cui deve andare il waypoint e fai clic su
    **Aggiungi waypoint**. Se tieni premuto per un momento il tasto sinistro del mouse sulla mappa, si apre lo stesso
    menu.

2. ```{image} ../images/quickstart-waypoints/quickstart-waypoints-project.png
    :align: right
    ```

    Seleziona il progetto in cui va il waypoint, oppure inserisci un nome per un nuovo progetto in
    **Nome nuovo progetto**.

    Per un nuovo progetto, dopo aver inserito il nome, scegli sotto **Il nuovo progetto sarà creato come** il formato in
    cui QMapShack lo salva. **\*.qms** è il formato di file proprio di QMapShack, **\*.gpx** un file GPX che leggono
    anche altri programmi e dispositivi. QMapShack ne preseleziona uno, quindi controlla che sia quello che vuoi. La
    terza opzione, **Database**, è spiegata in [Database](quickstart-database.md).

    Fai clic su **OK**.

3. Si apre la finestra dei dettagli del waypoint. Mostra nome, simbolo, posizione, quota e ora del waypoint.

    ```{image} ../images/quickstart-waypoints/quickstart-waypoints-details.png
    :class: below-right
    ```

    La **Quota** viene dal DEM; vedi [Dati altimetrici e ombreggiatura](quickstart-dem.md).

4. Fai clic sul nome, inseriscine uno nuovo e premi **Invio**.

    ```{image} ../images/quickstart-waypoints/quickstart-waypoints-named.png
    :class: below-right
    ```

5. ```{image} ../images/quickstart-waypoints/quickstart-waypoints-symbol.png
    :align: right
    ```

    Fai clic sul simbolo a sinistra del nome per sceglierne un altro.

6. ```{image} ../images/quickstart-waypoints/quickstart-waypoints-closed.png
    :align: right
    ```

    Premi **Esc** per chiudere la finestra. Ora il waypoint compare sulla mappa con il suo nome.

7. ```{image} ../images/quickstart-waypoints/quickstart-waypoints-saved.png
    :align: right
    ```

    Il riquadro **Spazio di lavoro** ora elenca il waypoint sotto il suo progetto.

Il pulsante di salvataggio nella riga del progetto, evidenziato nell'immagine, indica che il progetto ha modifiche non
salvate in un file. Fai clic su di esso per salvare il progetto.

La prima volta che salvi il progetto, QMapShack chiede dove salvarlo. La finestra di dialogo propone il nome del
progetto come nome del file. Si apre nella cartella da cui hai caricato o in cui hai salvato l'ultima volta. Se hai
impostato la [cartella home](quickstart-main-window.md#impostare-la-cartella-home), QMapShack apre la finestra di
dialogo nella sottocartella `GPX` della cartella home.

Nella riga del waypoint, lo stesso simbolo non è un pulsante ma un indicatore di modifica. Mostra che il waypoint è
cambiato dall'ultimo salvataggio del progetto.

Il primo waypoint si chiama `wpt`. Un nuovo waypoint riceve l'ultimo simbolo che hai scelto e l'ultimo nome che hai
dato. Se quel nome finisce con un numero, QMapShack lo incrementa: dopo `Rifugio 3` viene `Rifugio 4`.
