# Datenbanken

Diese Seite zeigt, wie Sie Projekte in einer QMapShack-Datenbank speichern. Die Bilder nutzen die
Beispieldatenbank `Example.db`.

```{index} single: Datenbank; hinzufügen
```

## Eine Datenbank anlegen

Eine Datenbank ist eine Datei, in der Ihre Projekte in Ordnern liegen. Der Bereich **Datenbank** zeigt Ihre
Datenbanken.

1. ```{image} ../images/quickstart-database/quickstart-database-add-menu.png
    :align: right
    ```

    Klicken Sie mit der rechten Maustaste auf eine freie Stelle im Bereich **Datenbank** und dann auf
    **Datenbank hinzufügen**.

2. ```{image} ../images/quickstart-database/quickstart-database-add-dialog.png
    :align: right
    ```

    Geben Sie bei **Name** `Example` ein und lassen Sie **SQLite** ausgewählt.

3. Klicken Sie auf den Plus-Knopf, der im Bild von Schritt 2 markiert ist. Wählen Sie ein Verzeichnis aus, geben Sie
    einen Dateinamen ein und klicken Sie auf **Speichern**.

    Der Dialog öffnet sich in dem Verzeichnis, in dem Sie zuletzt eine Datenbank gewählt haben. Wenn Sie das
    [Hauptverzeichnis](quickstart-main-window.md#das-hauptverzeichnis-einrichten) eingerichtet haben, stellt
    QMapShack ihn auf dessen Verzeichnis `Databases`.

    Haben Sie schon eine Datenbankdatei, klicken Sie stattdessen auf den Ordner-Knopf rechts neben dem Plus-Knopf.
    Wählen Sie die Datei aus und klicken Sie auf **Öffnen**.

4. ```{image} ../images/quickstart-database/quickstart-database-panel.png
    :align: right
    ```

    Klicken Sie auf **OK**. Die Datenbank erscheint im Bereich **Datenbank**.

```{index} single: Datenbank; Ordner
```

## Ordner

In einer Datenbank liegen die Daten in Ordnern. Die Farbe eines Ordners zeigt seine Art:

- Ein Ordner vom Typ **Gruppe** (blau) enthält andere Ordner und hilft, die Daten zu strukturieren. Sie können ihn nicht
    in den Arbeitsplatz laden. Legen Sie Ordner vom Typ **Projekt** in einen Ordner vom Typ **Gruppe**, nicht direkt in
    die Datenbank.
- Ein Ordner vom Typ **Projekt** (grün) enthält wie eine Projektdatei Tracks, Wegpunkte, Routen und Gebiete. Sie können
    ihn in den Arbeitsplatz laden.
- Ein Ordner vom Typ **Sonstige** (orange) funktioniert wie ein Ordner vom Typ **Projekt**. Wofür Sie ihn nutzen,
    entscheiden Sie selbst. So können Sie etwa die Aufzeichnung einer Tour in einen Ordner vom Typ **Projekt** legen und
    die Planung in einen Ordner vom Typ **Sonstige** darunter.

1. ```{image} ../images/quickstart-database/quickstart-database-folder-menu.png
    :align: right
    ```

    Klicken Sie mit der rechten Maustaste auf die Datenbank **Example** und dann auf **Ordner hinzufügen**.

2. ```{image} ../images/quickstart-database/quickstart-database-folder-dialog.png
    :align: right
    ```

    Geben Sie bei **Ordnername** `Touren` ein und wählen Sie **Gruppe** aus.

3. ```{image} ../images/quickstart-database/quickstart-database-folder-group.png
    :align: right
    ```

    Klicken Sie auf **OK**. Die Gruppe **Touren** erscheint unter **Example**.

4. ```{image} ../images/quickstart-database/quickstart-database-folder-project.png
    :align: right
    ```

    Klicken Sie mit der rechten Maustaste auf **Touren** und dann auf **Ordner hinzufügen**. Geben Sie
    `Bergtour` ein, wählen Sie **Projekt** aus und klicken Sie auf **OK**.

5. ```{image} ../images/quickstart-database/quickstart-database-folders.png
    :align: right
    ```

    Klicken Sie mit der rechten Maustaste auf **Bergtour** und dann auf **Ordner hinzufügen**. Geben Sie
    `Planung` ein, wählen Sie **Sonstige** aus und klicken Sie auf **OK**.

```{index} single: Datenbank; importieren
```

## Dateien importieren

Sie können Projektdateien in einer Datenbank ablegen, ohne sie in den Arbeitsplatz zu laden.

1. ```{image} ../images/quickstart-database/quickstart-database-import-menu.png
    :align: right
    ```

    Klicken Sie mit der rechten Maustaste auf den Ordner, unter dem die Projekte liegen sollen, hier die
    Gruppe **Touren**, und dann auf **Aus Dateien importieren...**.

2. Wählen Sie eine oder mehrere Dateien aus, zum Beispiel `Vilsalpsee.gpx`, und klicken Sie auf **Öffnen**.

3. ```{image} ../images/quickstart-database/quickstart-database-imported.png
    :align: right
    ```

    QMapShack legt für jede Datei einen Ordner vom Typ **Projekt** in dem Ordner an, auf den Sie geklickt haben. Hier
    erscheint **Vilsalpsee** unter **Touren**. Der Ordner trägt den Namen des Projekts aus der Datei. Ein Projekt,
    das schon in der Datenbank liegt, überspringt QMapShack.

```{index} single: Datenbank; exportieren
```

## Als GPX exportieren

Sie können eine Datenbank oder einen ihrer Ordner als GPX-Dateien exportieren, zum Beispiel um die Daten
weiterzugeben.

1. ```{image} ../images/quickstart-database/quickstart-database-export-menu.png
    :align: right
    ```

    Klicken Sie mit der rechten Maustaste auf die Datenbank **Example** und dann auf
    **Als GPX exportieren...**.

2. ```{image} ../images/quickstart-database/quickstart-database-export-dialog.png
    :align: right
    ```

    Klicken Sie auf den im Bild markierten Ordner-Knopf und wählen Sie das Zielverzeichnis aus. Aktivieren Sie
    **GPX 1.1 ohne Erweiterungen**, um die eigenen Daten von QMapShack wegzulassen.

3. Klicken Sie auf **Starten**. QMapShack legt für die Datenbank und jede Gruppe ein Verzeichnis an und schreibt jeden
    Ordner vom Typ **Projekt** und **Sonstige** in eine eigene GPX-Datei. Der Dateiname eines Ordners vom Typ
    **Sonstige** beginnt mit dem Namen des Ordners darüber.

4. Wenn **Fertig!** erscheint, klicken Sie auf **Schließen**.

    ```{image} ../images/quickstart-database/quickstart-database-exported.png
    :class: below-right
    ```

```{index} single: Datenbank; Arbeitsplatz
```

## Mit dem Arbeitsplatz arbeiten

Die Datenbank speichert Ihre Daten. Ändern können Sie sie im Arbeitsplatz. Laden Sie ein Projekt, ändern Sie es,
speichern Sie es in die Datenbank zurück und entladen Sie es wieder.

1. ```{image} ../images/quickstart-database/quickstart-database-loaded-db.png
    :align: right
    ```

    Klicken Sie in der Zeile von **Vilsalpsee** ganz rechts auf den Lade-Knopf, der im Bild markiert ist.

2. ```{image} ../images/quickstart-database/quickstart-database-loaded-wks.png
    :align: right
    ```

    Das Projekt erscheint im Bereich **Arbeitsplatz** als **Vilsalpsee @ Touren**: sein Name und der Ordner, in
    dem es liegt.

3. Bearbeiten Sie das Projekt wie eine Projektdatei, zum Beispiel indem Sie
    [einen Wegpunkt hinzufügen](quickstart-waypoints.md). Klicken Sie dann im Bereich **Arbeitsplatz** auf den
    Speichern-Knopf in der Zeile des Projekts. QMapShack schreibt die Änderungen in die Datenbank.

4. Klicken Sie im Bereich **Datenbank** noch einmal auf den Lade-Knopf, um das Projekt zu entladen. Hat das Projekt
    ungespeicherte Änderungen, fragt QMapShack zuerst: **Speichern** speichert sie, **Nein** verwirft sie,
    **Abbrechen** lässt das Projekt geladen.

Wenn Sie im Arbeitsplatz ein Element aus einem Projekt entfernen, bleibt es in der Datenbank, auch nach dem
Speichern. Um es aus einem Ordner zu löschen, klicken Sie im Bereich **Datenbank** mit der rechten Maustaste darauf
und dann auf **Element löschen**. Ein Element, das in keinem Ordner mehr liegt, landet in **Verloren & Gefunden**.
Löschen Sie es dort, ist es endgültig entfernt.
