/**********************************************************************************************
    Copyright (C) 2017 Norbert Truchsess norbert.truchsess@t-online.de

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

**********************************************************************************************/

#ifndef CROUTERBROUTERTILESSELECT_H
#define CROUTERBROUTERTILESSELECT_H

#include <QDateTime>
#include <QDir>
#include <QLabel>
#include <QNetworkAccessManager>
#include <QProgressBar>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWebPage>

class CRouterBRouterTilesSelectArea;
class CRouterBRouterTilesStatus;
class CRouterBRouterSetup;

class CRouterBRouterTilesSelect : public QWidget
{
    Q_OBJECT
public:
    CRouterBRouterTilesSelect(QWidget * parent);
    virtual ~CRouterBRouterTilesSelect();

    void setSetup(CRouterBRouterSetup * setup);
    void initialize();

signals:
    void tilesChanged();
    void selectedTilesChanged();

public slots:
    void slotLoadOnlineTilesRequestFinished();
    void slotDownloadFinished(QNetworkReply* reply);
    void slotDownloadReadReady();

private slots:
    void slotTileClicked(const QPoint & tile);
    void slotUpdateButtons();
    void slotClearSelection();
    void slotDeleteSelected();
    void slotSelectOutdated();
    void slotDownload();
    void slotUpdateStatus();

private:
    struct tile_s { QPoint tile; QDateTime date; qint64 size; };

    void initializeTiles();
    void selectTile(const QPoint tile);
    void deselectTile(const QPoint tile);
    void deleteTile(const QPoint tile);

    QPoint tileFromFileName(const QString fileName) const;
    QString fileNameFromTile(const QPoint tile) const;
    QString absoluteFileNameFromTile(QPoint tile) const;

    tile_s getOnlineTileData(const QPoint tile) const;
    tile_s getLocalTileData(const QPoint tile) const;

    QDir segmentsDir() const;
    void readTiles();
    const QString formatSize(const qint64 size);

    const QPoint noTile = QPoint(INT_MIN,INT_MIN);
    const tile_s noTileData = { noTile, QDateTime::fromMSecsSinceEpoch(0), INT_MIN };

    CRouterBRouterSetup * setup;

    QVBoxLayout * outerLayout;
    QWidget * widgetSelect;
    QLabel * statusLabel;
    QProgressBar * statusProgress;

    QPushButton * pushSelectOutdated;
    QPushButton * pushClearSelection;
    QPushButton * pushDeleteSelection;
    QPushButton * pushDownload;

    CRouterBRouterTilesSelectArea * selectArea;

    QVector<tile_s> onlineTiles;

    QVector<QPoint> invalidTiles;
    QVector<QPoint> outdatedTiles;
    QVector<QPoint> currentTiles;
    QVector<QPoint> outstandingTiles;
    QVector<QPoint> selectedTiles;

    QWebPage * tilesWebPage;

    QNetworkAccessManager * tilesDownloadManager;
    QVector<QNetworkReply*> tilesDownloadManagerReplies;

    QHash<QString,CRouterBRouterTilesStatus*> tilesDownloadStatus;

    friend class CRouterBRouterTilesSelectArea;
};

#endif //CROUTERBROUTERTILESSELECT_H

