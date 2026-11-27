# Ihre Arbeit speichern

Diese Seite erklärt, wie Sie Ihre Projekte als Datei oder in einer Datenbank speichern.

```{index} single: Projekt; speichern
```

## Ungespeicherte Änderungen erkennen

Beim erneuten Start stellt QMapShack alle Projekte im Bereich **Arbeitsplatz** wieder her. Dazu speichert QMapShack eine eigene Kopie jedes Projekts.

Die Originaldatei, aus der Sie ein Projekt geladen haben, bleibt unverändert, bis Sie das Projekt speichern. Ein neu angelegtes Projekt hat zunächst keine zugehörige Datei. Diese wird erst beim Speichern erstellt.

1. ```{image} ../images/quickstart-save/quickstart-save-changed.png
    :align: right
    ```

    Suchen Sie im Bereich **Arbeitsplatz** das Projekt mit ungespeicherten Änderungen. In seiner Zeile erscheint eine Speichern-Schaltfläche, die im Bild hervorgehoben ist.

    Im Beispiel enthält **Meine Wegpunkte** einen neu hinzugefügten Wegpunkt, während **Example** unverändert ist.
2. Klicken Sie auf die Speichern-Schaltfläche. Wurde das Projekt noch nie gespeichert, fragt QMapShack zunächst nach dem Speicherort. Weitere Informationen finden Sie unter [In eine Datei speichern](#in-eine-datei-speichern). Nach dem Speichern verschwindet die Schaltfläche.

In der Zeile eines Elements dient dasselbe Symbol nicht als Schaltfläche, sondern als Änderungsanzeige. Es signalisiert, dass das Element seit dem letzten Speichern des Projekts geändert wurde.

## In eine Datei speichern

1. Klicken Sie im Bereich **Arbeitsplatz** mit der rechten Maustaste auf das Projekt und wählen Sie **Speichern unter...**.
2. Wählen Sie im Dialog den gewünschten Dateityp aus, geben Sie einen Dateinamen ein und klicken Sie auf **Speichern**.

Ist dem Projekt bereits eine Datei zugeordnet, wählen Sie im selben Menü den Eintrag **Speichern**. QMapShack überschreibt die vorhandene Datei ohne Rückfrage.

Folgende Dateitypen werden unterstützt:

- **QMapShack Binary .qms** ist das native Dateiformat von QMapShack. Es speichert alle Projektdaten, einschließlich der Änderungshistorie. Dadurch können Sie Änderungen auch später noch rückgängig machen. Verwenden Sie dieses Format für Ihre eigenen Projekte.
- **GPS Exchange Format .gpx** wird auch von vielen anderen Programmen und GPS-Geräten unterstützt. Die Änderungshistorie bleibt beim Speichern erhalten. Nach dem erneuten Laden der Datei können Sie die Änderungen jedoch nicht mehr rückgängig machen.

**Als GPX o. Erw. speichern...** im Projektmenü speichert eine standardkonforme GPX-1.1-Datei ohne QMapShack-spezifische Daten. Verwenden Sie diese Option für Programme, die die Erweiterungen von QMapShack nicht unterstützen.

Wenn Sie ein Projekt in einem anderen Dateiformat speichern, erstellt QMapShack eine Kopie. Die ursprüngliche Datei bleibt dem Projekt zugeordnet, und die Speichern-Schaltfläche bleibt erhalten.

**Datei → Alle GIS-Daten speichern** (**Strg+S**) speichert alle Projekte im Bereich **Arbeitsplatz**. Wurde ein Projekt noch nie gespeichert, fragt QMapShack nach dem gewünschten Speicherort.

Wenn Sie ein Projekt mit ungespeicherten Änderungen schließen, fragt QMapShack, ob Sie die Änderungen vorher speichern möchten.

```{index} single: Projekt; in Datenbank speichern
```

## In eine Datenbank speichern

Ein Projekt, das Sie aus einer Datenbank geladen haben, speichern Sie mit seiner Speichern-Schaltfläche wieder in den zugehörigen Datenbankordner. Weitere Informationen finden Sie unter [Mit dem Arbeitsplatz arbeiten](quickstart-database.md#mit-dem-arbeitsplatz-arbeiten).

Um eine Datei als Projekt in einer Datenbank abzulegen, haben Sie zwei Möglichkeiten: 

- Kopieren Sie das Projekt im Bereich **Arbeitsplatz**, wie unten beschrieben, oder 
- importieren Sie die Datei. Siehe [Dateien importieren](quickstart-database.md#dateien-importieren).

1. ```{image} ../images/quickstart-save/quickstart-save-copy-menu.png
    :align: right
    ```

    Klicken Sie im Bereich **Arbeitsplatz** mit der rechten Maustaste auf das Projekt und wählen Sie **Kopieren nach...**.
2. ```{image} ../images/quickstart-save/quickstart-save-copy-dialog.png
    :align: right
    ```

    Geben Sie im Dialog **Ein Projekt auswählen...** einen Namen für das neue Projekt ein, hier beispielsweise `Seetour`. Wählen Sie als Ziel **Datenbank** aus und klicken Sie auf **OK**.
3. ```{image} ../images/quickstart-save/quickstart-save-copy-folder.png
    :align: right
    ```

    Klicken Sie im Dialog **Übergeordneten Ordner auswählen...** auf den Pfeil links neben der Datenbank, um deren Inhalt aufzuklappen. Wählen Sie den Ordner aus, in dem das Projekt abgelegt werden soll (hier die Gruppe **Projects**) und klicken Sie auf **OK**.
4. Lassen Sie im Dialog **Datenbankordner...** die Option **Projekt** ausgewählt und klicken Sie auf **OK**.
5. ```{image} ../images/quickstart-save/quickstart-save-copied.png
    :align: right
    ```

    Die Kopie erscheint im Bereich **Arbeitsplatz** unter dem Namen **Seetour @ Projects**. Daneben wird eine Speichern-Schaltfläche angezeigt.
6. ```{image} ../images/quickstart-save/quickstart-save-copied-db.png
    :align: right
    ```

    Auch im Bereich **Datenbank** erscheint der Ordner **Seetour**. Er ist zunächst noch leer.
7. Klicken Sie im Bereich **Arbeitsplatz** auf die Speichern-Schaltfläche neben **Seetour @ Projects**. QMapShack speichert daraufhin die Elemente des Projekts im entsprechenden Datenbankordner.

Der Dialog **Ein Projekt auswählen...** aus Schritt 2 erscheint auch, wenn Sie **Arbeitsplatz → Leeres Projekt hinzufügen** wählen oder ein neues Element in einem neuen Projekt anlegen.

Wählen Sie in diesem Dialog **Datenbank** aus, um das neue Projekt direkt in einer Datenbank anzulegen.