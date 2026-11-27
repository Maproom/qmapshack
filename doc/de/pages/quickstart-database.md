# Datenbanken

Diese Seite zeigt, wie Sie Projekte in einer QMapShack-Datenbank speichern. Die Bilder nutzen die
Beispieldatenbank `Example.db`.

```{index} single: Datenbank; hinzufügen
```

## Eine Datenbank anlegen

Eine Datenbank ist eine einzelne Datei, in der Ihre Projekte in Ordnern organisiert sind. Der Bereich **Datenbank** listet Ihre Datenbanken auf.

1. ```{image} ../images/quickstart-database/quickstart-database-add-menu.png
    :align: right
    ```

    Klicken Sie mit der rechten Maustaste auf eine freie Stelle im Bereich **Datenbank** und dann auf
    **Datenbank hinzufügen**.

2. ```{image} ../images/quickstart-database/quickstart-database-add-dialog.png
    :align: right
    ```

    Geben Sie bei **Name** `Example` ein und lassen Sie **SQLite** ausgewählt.

3. Klicken Sie auf die im Bild zu Schritt 2 markierte Plus-Schaltfläche. Wählen Sie ein Verzeichnis aus, geben Sie
    einen Dateinamen ein und klicken Sie auf **Speichern**.

    Der Dialog öffnet sich in dem Verzeichnis, in dem Sie zuletzt eine Datenbank ausgewählt haben. Wenn Sie das [Hauptverzeichnis][HAUPTVERZ] eingerichtet haben, öffnet QMapShack den Dialog standardmäßig im Unterverzeichnis `Databases` dieses Hauptverzeichnisses.

    Wenn Sie bereits eine Datenbankdatei haben, klicken Sie stattdessen auf die Ordner-Schaltfläche rechts neben der Plus-Schaltfläche. Wählen Sie die Datei aus und klicken Sie auf **Öffnen**.

4. ```{image} ../images/quickstart-database/quickstart-database-panel.png
    :align: right
    ```

    Klicken Sie auf **OK**. Anschließend erscheint die Datenbank im Bereich **Datenbank**.

```{index} single: Datenbank; Ordner
```

## Ordner

Eine Datenbank organisiert ihre Daten in Ordnern. Die Farbe eines Ordners zeigt seine Art:

- Ein Ordner vom Typ **Gruppe** (blau) enthält weitere Ordner und dient dazu, die Daten zu strukturieren. Sie können ihn nicht in den Arbeitsplatz laden. Legen Sie Ordner vom Typ **Projekt** in einem Ordner vom Typ **Gruppe** ab, nicht direkt in die Datenbank.
- Ein Ordner vom Typ **Projekt** (grün) enthält wie eine Projektdatei Tracks, Wegpunkte, Routen und Gebiete. Sie können
    ihn in den Arbeitsplatz laden.
- Ein Ordner vom Typ **Sonstige** (orange) funktioniert wie ein Ordner vom Typ **Projekt**. Sie können ihn jedoch frei verwenden. Beispielsweise können Sie die Aufzeichnung einer Tour in einem Ordner vom Typ **Projekt** und die dazugehörige Planung in einem darunterliegenden Ordner vom Typ **Sonstige** ablegen.

1. ```{image} ../images/quickstart-database/quickstart-database-folder-menu.png
    :align: right
    ```

    Klicken Sie mit der rechten Maustaste auf die Datenbank **Example** und dann auf **Ordner hinzufügen**.
2. ```{image} ../images/quickstart-database/quickstart-database-folder-dialog.png
    :align: right
    ```

    Geben Sie bei **Ordnername** `Touren` ein und wählen Sie als Typ **Gruppe** aus.
3. ```{image} ../images/quickstart-database/quickstart-database-folder-group.png
    :align: right
    ```
    Klicken Sie auf **OK**. Die Gruppe **Touren** erscheint unterhalb von **Example**.
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

Sie können Projektdateien in einer Datenbank speichern, ohne sie in den Arbeitsplatz zu laden.

1. ```{image} ../images/quickstart-database/quickstart-database-import-menu.png
    :align: right
    ```

    Klicken Sie mit der rechten Maustaste auf den Ordner, in dem die Projekte abgelegt werden sollen, hier die Gruppe **Touren**, und wählen Sie **Aus Dateien importieren...**.

2. Wählen Sie eine oder mehrere Dateien aus, zum Beispiel `Vilsalpsee.gpx`, und klicken Sie auf **Öffnen**.
3. ```{image} ../images/quickstart-database/quickstart-database-imported.png
    :align: right
    ```

    QMapShack legt für jede Datei einen Ordner vom Typ **Projekt** in dem zuvor ausgewählten Ordner an. In diesem Beispiel erscheint **Vilsalpsee** unter **Touren**. Der neue Ordner erhält den Namen des Projekts aus der Datei. Projekte, die bereits in der Datenbank vorhanden sind, überspringt QMapShack.

```{index} single: Datenbank; exportieren
```

## Als GPX exportieren

Sie können eine Datenbank oder einen ihrer Ordner als .gpx Dateien exportieren, zum Beispiel um die Daten
weiterzugeben.

1. ```{image} ../images/quickstart-database/quickstart-database-export-menu.png
    :align: right
    ```

    Klicken Sie mit der rechten Maustaste auf die Datenbank **Example** und dann auf
    **Als GPX exportieren...**.
2. ```{image} ../images/quickstart-database/quickstart-database-export-dialog.png
    :align: right
    ```

    Klicken Sie auf die im Bild markierte Ordner-Schaltfläche und wählen Sie das Zielverzeichnis aus. Aktivieren Sie **GPX 1.1 ohne Erweiterungen**, um QMapShack-spezifische Daten auszuschließen.
3. Klicken Sie auf **Starten**. QMapShack legt ein Verzeichnis für die Datenbank und jeweils ein eigenes Verzeichnis für jede Gruppe an. Für jeden Ordner vom Typ **Projekt** und **Sonstige** erstellt es eine eigene .gpx Datei. Der Dateiname eines Ordners vom Typ **Sonstige** beginnt mit dem Namen seines übergeordneten Ordners.
4. Wenn **Fertig!** erscheint, klicken Sie auf **Schließen**.

    ```{image} ../images/quickstart-database/quickstart-database-exported.png
    :class: below-right
    ```

```{index} single: Datenbank; Arbeitsplatz
```

## Mit dem Arbeitsplatz arbeiten

Die Datenbank speichert Ihre Daten. Sie können diese im Arbeitsplatz bearbeiten. Laden Sie ein Projekt, ändern Sie es,
speichern Sie es in die Datenbank zurück und entladen Sie es wieder.

1. ```{image} ../images/quickstart-database/quickstart-database-loaded-db.png
    :align: right
    ```

    Klicken Sie in der Zeile von **Vilsalpsee** ganz rechts auf die im Bild markierte Laden-Schaltfläche.
2. ```{image} ../images/quickstart-database/quickstart-database-loaded-wks.png
    :align: right
    ```

    Das Projekt erscheint im Bereich **Arbeitsplatz** unter dem Namen **Vilsalpsee @ Touren**. Dieser setzt sich aus dem Projektnamen und dem Ordner zusammen, in dem es liegt.
3. Bearbeiten Sie das Projekt wie eine normale Projektdatei, zum Beispiel indem Sie [einen Wegpunkt hinzufügen][WEGPKT]. Klicken Sie anschließend im Bereich **Arbeitsplatz** in der Projektzeile auf die Speichern-Schaltfläche. QMapShack übernimmt die Änderungen in die Datenbank.
4. Klicken Sie im Bereich **Datenbank** erneut auf die Laden-Schaltfläche, um das Projekt zu entladen. Hat das Projekt ungespeicherte Änderungen, fragt QMapShack zunächst, wie es weitergehen soll:

    - **Speichern** speichert die Änderungen vor dem Entladen.
    - **Nein** verwirft die Änderungen und entlädt das Projekt.
    - **Abbrechen** bricht den Vorgang ab und lässt das Projekt geladen.

Wenn Sie ein Element im Arbeitsplatz aus einem Projekt entfernen, bleibt es in der Datenbank erhalten, auch wenn Sie das Projekt speichern. Um ein Element aus einem Ordner zu löschen, klicken Sie im Bereich **Datenbank** mit der rechten Maustaste darauf und wählen **Element löschen**. Ein Element, das keinem Ordner mehr zugeordnet ist, wird nach **Verloren & Gefunden** verschoben. Wenn Sie es dort löschen, wird es endgültig entfernt.


[HAUPTVERZ]: https://quickstart-main-window.md#das-hauptverzeichnis-einrichten
[WEGPKT]:    https://quickstart-waypoints.md