# Usare un dispositivo GPS

Questa pagina mostra come trasferire le tue tracce e i tuoi waypoint su un dispositivo GPS Garmin e come recuperare le
tracce che ha registrato.

```{index} single: Dispositivo; collegare
```

## Collegare il dispositivo

Questa pagina usa un dispositivo Garmin collegato come memoria di massa USB, che il tuo computer mostra come
un'unità. QMapShack gestisce allo stesso modo i dispositivi che si collegano tramite MTP (Media Transfer Protocol) e
quelli di altre marche. Ciò che serve al tuo computer per un dispositivo è descritto nella pagina del wiki
[GPS Devices][WIKI_DEVICES] (in inglese).

1. Collega il dispositivo al tuo computer con il suo cavo USB.
2. ```{image} ../images/quickstart-device/quickstart-device-connected.png
    :align: right
    ```

    Il dispositivo compare nel riquadro **Spazio di lavoro** con il nome del suo modello. Sotto di esso, QMapShack
    elenca ogni file di tracce e waypoint del dispositivo come un progetto.

Se il dispositivo non compare, fai clic su **Spazio di lavoro → Modifica spazio di lavoro** e verifica che
**Abilita supporto dispositivo** sia selezionato nella scheda **Generale**. È selezionato per impostazione predefinita.

```{index} single: Dispositivo; inviare progetto
```

## Inviare un progetto al dispositivo

1. ```{image} ../images/quickstart-device/quickstart-device-send-menu.png
    :align: right
    ```

    Fai clic con il tasto destro sul progetto nel riquadro **Spazio di lavoro** e fai clic su
    **Invia al dispositivo**. Puoi fare clic su questa voce solo mentre è collegato un dispositivo.

2. ```{image} ../images/quickstart-device/quickstart-device-sent.png
    :align: right
    ```

    Il progetto compare sotto il dispositivo. QMapShack lo salva sul dispositivo come file GPX con il nome del
    progetto, in questo esempio `Example.gpx`. Nel nome del file, QMapShack omette tutti i caratteri tranne lettere,
    cifre e `_`.

Se è collegato più di un dispositivo, QMapShack chiede a quali inviare il progetto. Se invii di nuovo il progetto, la
sua copia sul dispositivo viene sostituita.

```{index} single: Dispositivo; recuperare registrazione
```

## Recuperare una registrazione dal dispositivo

1. ```{image} ../images/quickstart-device/quickstart-device-recording.png
    :align: right
    ```

    Fai clic sulla freccia a sinistra del progetto con la tua registrazione, in questo esempio
    **Track 2025-06-14 101530**, per vederne la traccia.

2. ```{image} ../images/quickstart-device/quickstart-device-copy-menu.png
    :align: right
    ```

    Non puoi modificare gli elementi sul dispositivo, quindi copia la traccia nello spazio di lavoro. Fai clic con il
    tasto destro sulla traccia e fai clic su **Copia in...**.

3. ```{image} ../images/quickstart-device/quickstart-device-copy-dialog.png
    :align: right
    ```

    Nella finestra di dialogo **Seleziona un progetto...**, seleziona un progetto dello spazio di lavoro o inserisci
    un nome per uno nuovo, in questo esempio `Le mie registrazioni`. Fai clic su **OK**.

4. ```{image} ../images/quickstart-device/quickstart-device-copied.png
    :align: right
    ```

    La traccia ora è nel progetto dello spazio di lavoro, dove puoi modificarla. Salva il progetto per conservarla;
    vedi [Salvare il lavoro](quickstart-save.md).

[WIKI_DEVICES]: https://github.com/Maproom/qmapshack/wiki/DocGisDevices
