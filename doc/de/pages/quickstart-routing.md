# Routing einrichten

Diese Seite beschreibt, wie Sie einen Router einrichten, damit QMapShack einen Track für Sie entlang von Wegen und Straßen
zeichnen kann.

```{index} single: Routing
```

```{index} single: Router
```

## Was ein Router macht

Ein *Router* berechnet die Strecke zwischen zwei Punkten entlang vorhandener Wege und Straßen. Wenn Sie einen Track zeichnen, müssen Sie lediglich die Punkte anklicken, durch die er führen soll. Der Router ermittelt automatisch den Weg dazwischen.

Damit der Router den Weg bereits beim Klicken ergänzen kann, muss er *sofort* auf Routingdaten zugreifen können, die auf Ihrem Computer gespeichert sind. 

Ein Router, der die Daten von einem Internetserver abruft, kann das nicht leisten. Er müsste bei jedem Klick eine neue Anfrage senden. Um Missbrauch zu vermeiden, begrenzen die Server jedoch die Anzahl der Anfragen innerhalb eines bestimmten Zeitraums. Oder sie sperren Programme, die zu viele Anfragen in kurzer Zeit senden.

```{index} single: Routing; Router auswählen
```

## Einen Router auswählen

QMapShack unterstützt zwei verschiedene Router: Routino und BRouter. Wählen Sie den gewünschten Router aus der Liste oben im Bereich **Streckenführung** aus.

- **Routino (offline)** berechnet Routen anhand von Daten, die auf Ihrem Computer gespeichert sind. Diese Daten erstellen Sie aus OpenStreetMap-Daten über **Werkzeug → Routino-Datenbank erstellen**. Die Anleitung auf dieser Seite beschreibt die Verwendung von Routino.
- **BRouter** ist einfacher einzurichten. Als **BRouter (online)** sendet er Anfragen an den Server von BRouter-Web und kann daher nicht sofort antworten. Wenn Sie BRouter auf Ihrem Computer installieren und lokal ausführen, steht er als **BRouter (offline)** zur Verfügung und kann sofort antworten.

```{index} single: BRouter; installieren
```

## BRouter installieren

```{image} ../images/quickstart-routing/quickstart-routing-brouter-setup.png
:align: right
```


BRouter benötigt Java. Installieren Sie daher zuerst Java. Ohne Java kann der Einrichtungsassistent nicht fortgesetzt werden.

Ein Assistent hilft Ihnen BRouter zu installieren. Wählen Sie in der Liste **BRouter (online)** aus und klicken Sie auf **Einrichten**.

Wählen Sie im Assistenten **BRouter Einrichtung** die Option **Lokale Installation** aus und folgen Sie den Anweisungen auf den einzelnen Seiten.

Der Assistent lädt BRouter sowie die Routingdaten für die Gebiete, die Sie auf der Karte auswählen, herunter und speichert sie im Verzeichnis `BRouter` Ihres [Hauptverzeichnisses](quickstart-main-window.md#das-hauptverzeichnis-einrichten).

Nach Abschluss der Einrichtung zeigt die Liste der Router den Eintrag **BRouter (offline)** an.

```{index} single: Routino; einrichten
```

## Routino einrichten

Routino berechnet Routen mithilfe einer Datenbank, die QMapShack aus OpenStreetMap-Daten erstellt. Dazu benötigen Sie einen Kartenausschnitt Ihrer Region im .pbf Format, beispielsweise von [Geofabrik][GEOFABRIK].

Da Routino keine Routen zwischen verschiedenen Datenbanken berechnen kann, muss der Kartenausschnitt alle Gebiete abdecken, durch die eine Route führen soll.

Sie können auch mehrere .pbf Dateien zu einer gemeinsamen Datenbank zusammenfassen.

Unter Linux installieren Sie zunächst das Routino-Paket Ihrer Distribution. Dieses enthält das benötigte Werkzeug `planetsplitter`. Unter Windows wird `planetsplitter` bei der Installation von Routino automatisch mitinstalliert.

1. Wählen Sie oben im Bereich **Streckenführung** den Eintrag **Routino (offline)** aus.
2. Klicken Sie auf **Werkzeug → Routino Datenbank erstellen**.
3. Klicken Sie auf die Schaltfläche mit dem Ordnersymbol neben **Quelldateien auswählen** und wählen Sie die gewünschten .pbf Dateien aus.
4. Wählen Sie in der Liste **Zielpfad** das Verzeichnis aus, in dem die Datenbank gespeichert werden soll. Die Liste enthält alle für Routino registrierten Verzeichnisse, darunter auch das Verzeichnis `Routino` Ihres [Hauptverzeichnisses](quickstart-main-window.md#das-hauptverzeichnis-einrichten).

    Über die Schaltfläche mit dem Ordnersymbol links neben **Zielpfad** können Sie weitere Verzeichnisse hinzufügen.
5. Geben Sie im Feld **Dateipräfix** einen Namen für die Datenbank ein.
6. Klicken Sie auf **Starten**, um die Datenbank zu erstellen. Dieser Vorgang kann einige Zeit in Anspruch nehmen.

    Sobald das Werkzeug **!!! erledigt !!!** meldet, wird die erstellte Datenbank im Bereich **Streckenführung** unter **Datenbank** angezeigt.
7. ```{image} ../images/quickstart-routing/quickstart-routing-routino-select.png
    :align: right
    ```

    Wählen Sie unter **Datenbank** die gewünschte Datenbank aus. Legen Sie anschließend unter **Profil** fest, wie Sie unterwegs sind, beispielsweise als **Fußgänger** oder mit dem **Fahrrad**.

Eine extern erstellte Datenbank besteht aus vier Dateien: `<name>-nodes.mem`, `<name>-segments.mem`, `<name>-ways.mem` und `<name>-relations.mem`.

Kopieren Sie diese Dateien direkt in das Verzeichnis `Routino`, nicht in ein Unterverzeichnis. Nach einigen Sekunden erscheint die Datenbank unter **Datenbank**.

Als Name wird der gemeinsame Dateiname verwendet. Dabei werden Unterstriche durch Leerzeichen ersetzt. So erscheint beispielsweise die Datenbank aus den Dateien `tannheimer_tal-*.mem` in der Liste unter dem Namen **tannheimer tal**.


[GEOFABRIK]: https://download.geofabrik.de "Geofabrik downloads"
