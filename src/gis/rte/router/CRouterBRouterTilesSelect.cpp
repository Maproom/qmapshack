/**********************************************************************************************
    Copyright (C) 2014-2015 Oliver Eichler oliver.eichler@gmx.de
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

#include <QtCore>
#include <QWebPage>
#include <QWebFrame>
#include <QWebElement>
#include "CRouterBRouterProgressBar.h"
#include "CRouterBRouterTilesSelect.h"
#include "CRouterBRouterTilesSelectArea.h"
#include "CRouterBRouterTilesSelectLayout.h"
#include "CRouterBRouterSetupException.h"
#include "CMainWindow.h"

CRouterBRouterTilesSelect::CRouterBRouterTilesSelect(QWidget *parent)
    : QWidget(parent)
{
    outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(0,0,0,0);
    widgetSelect = new QWidget(this);
    widgetSelect->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding);
    outerLayout->addWidget(widgetSelect);
    widgetSelect->show();

    QTemporaryFile temp;
    temp.open();
    temp.close();

    QSettings view(temp.fileName(), QSettings::IniFormat);
    view.clear();

    CCanvas * source = CMainWindow::self().getVisibleCanvas();
    if(source)
    {
        source->saveConfig(view);
    }
    //view.setValue("grid/color",QColor(Qt::red).name());
    view.setValue("map/zoomIndex",16);

    CCanvas * canvas = new CCanvas(widgetSelect,"BRouterTileDownload");
    // clone canvas by a temporary configuration file

    canvas->loadConfig(view);

    selectArea = new CRouterBRouterTilesSelectArea(widgetSelect,this,canvas);

    QLayout * selectLayout = new CRouterBRouterTilesSelectLayout(widgetSelect);
    selectLayout->addWidget(canvas);
    selectLayout->addWidget(selectArea);
    canvas->lower();
    canvas->show();
    selectArea->show();

    statusLayout = new QFormLayout();
    outerLayout->addLayout(statusLayout);

    QHBoxLayout * buttonsLayout = new QHBoxLayout();
    outerLayout->addLayout(buttonsLayout);

    pushSelectOutdated = new QPushButton("select outdated", this);
    pushClearSelection = new QPushButton("clear selection", this);
    pushDeleteSelection = new QPushButton("delete selection", this);
    pushDownload = new QPushButton("Download", this);

    buttonsLayout->addWidget(pushSelectOutdated);
    buttonsLayout->addWidget(pushClearSelection);
    buttonsLayout->addWidget(pushDeleteSelection);
    buttonsLayout->addWidget(pushDownload);

    tilesDownloadManager = new QNetworkAccessManager();

    connect(pushClearSelection, &QPushButton::clicked, this, &CRouterBRouterTilesSelect::slotClearSelection);
    connect(pushDeleteSelection, &QPushButton::clicked, this, &CRouterBRouterTilesSelect::slotDeleteSelected);
    connect(pushSelectOutdated, &QPushButton::clicked, this, &CRouterBRouterTilesSelect::slotSelectOutdated);
    connect(pushDownload, &QPushButton::clicked, this, &CRouterBRouterTilesSelect::slotDownload);
    connect(selectArea, &CRouterBRouterTilesSelectArea::tileClicked, this, &CRouterBRouterTilesSelect::slotTileClicked);
    connect(this, &CRouterBRouterTilesSelect::selectedTilesChanged, this, &CRouterBRouterTilesSelect::slotUpdateButtons);
    connect(this, &CRouterBRouterTilesSelect::tilesChanged, selectArea, &CRouterBRouterTilesSelectArea::updateTiles);
    connect(this, &CRouterBRouterTilesSelect::selectedTilesChanged, selectArea, &CRouterBRouterTilesSelectArea::updateTiles);
    connect(&tilesWebPage, &QWebPage::loadFinished, this, &CRouterBRouterTilesSelect::slotLoadOnlineTilesRequestFinished);
    connect(tilesDownloadManager, &QNetworkAccessManager::finished, this, &CRouterBRouterTilesSelect::slotDownloadFinished);
}

CRouterBRouterTilesSelect::~CRouterBRouterTilesSelect()
{
}

void CRouterBRouterTilesSelect::setSetup(CRouterBRouterSetup * setup)
{
    this->setup = setup;
}

void CRouterBRouterTilesSelect::initialize()
{
    initializeTiles();
    slotClearSelection();
}

void CRouterBRouterTilesSelect::slotTileClicked(const QPoint & tile)
{
    const tile_s data = getOnlineTileData(tile);
    if (data.tile != noTile)
    {
        if(selectedTiles.contains(tile))
        {
            deselectTile(tile);
        }
        else
        {
            selectTile(tile);
        }
        emit selectedTilesChanged();
    }
}

void CRouterBRouterTilesSelect::slotSelectOutdated()
{
    bool changed(false);
    for (QPoint tile : outdatedTiles)
    {
        if (!selectedTiles.contains(tile))
        {
            selectTile(tile);
            changed = true;
        }
    }
    if (changed)
    {
        emit selectedTilesChanged();
    }
}

void CRouterBRouterTilesSelect::slotDeleteSelected()
{
    for (QPoint tile : selectedTiles)
    {
        deleteTile(tile);
    }
    slotClearSelection();
}

void CRouterBRouterTilesSelect::slotClearSelection()
{
    bool changed(false);
    for (QPoint tile : selectedTiles)
    {
        deselectTile(tile);
        changed = true;
    }
    if (changed)
    {
        emit selectedTilesChanged();
    }
}

void CRouterBRouterTilesSelect::slotUpdateButtons()
{
    pushClearSelection->setEnabled(!selectedTiles.isEmpty());
    bool enabled = false;
    for (QPoint tile : selectedTiles)
    {
        if (outdatedTiles.contains(tile) or currentTiles.contains(tile))
        {
            enabled = true;
            break;
        }
    }
    pushDeleteSelection->setEnabled(enabled);
    enabled = false;
    for (QPoint tile : outdatedTiles)
    {
        if (!selectedTiles.contains(tile))
        {
            enabled = true;
            break;
        }
    }
    pushSelectOutdated->setEnabled(enabled);
    enabled = false;
    for (QPoint tile : selectedTiles)
    {
        if (!currentTiles.contains(tile))
        {
            enabled = true;
            break;
        }
    }
    pushDownload->setEnabled(enabled);
}

void CRouterBRouterTilesSelect::selectTile(const QPoint tile)
{
    selectedTiles << tile;
    QString fileName = fileNameFromTile(tile);
    if (!tilesDownloadStatus.contains(fileName))
    {
        status_s * status = new status_s;
        status->labelFilename = new QLabel(fileName,this);
        const tile_s remote = getOnlineTileData(tile);
        if (currentTiles.contains(tile))
        {
            const tile_s local = getLocalTileData(tile);
            status->labelStatus = new QLabel(QString("available %1 (%2) / installed %3 (%4)")
                                             .arg(formatSize(remote.size))
                                             .arg(remote.date.date().toString(Qt::DefaultLocaleShortDate))
                                             .arg(formatSize(local.size))
                                             .arg(local.date.date().toString(Qt::DefaultLocaleShortDate))
                                             ,this);
        }
        else
        {
            status->labelStatus = new QLabel(QString("available %1 (%2) / installed -")
                                             .arg(formatSize(remote.size))
                                             .arg(remote.date.date().toString(Qt::DefaultLocaleShortDate))
                                             ,this);
        }
        status->file = nullptr;
        status->progress = nullptr;
        statusLayout->addRow(status->labelFilename,status->labelStatus);
        tilesDownloadStatus.insert(fileName,status);
    }
}

void CRouterBRouterTilesSelect::deselectTile(QPoint tile)
{
    selectedTiles.remove(selectedTiles.indexOf(tile));
    const QString fileName = fileNameFromTile(tile);
    QHash<QString,status_s*>::const_iterator it = tilesDownloadStatus.constFind(fileName);
    if (it != tilesDownloadStatus.constEnd())
    {
        status_s * status = it.value();
        if (status->labelStatus != nullptr)
        {
            statusLayout->removeWidget(status->labelStatus);
            status->labelStatus->hide();
            delete status->labelStatus;
            status->labelStatus = nullptr;
        }
        if (status->file == nullptr and status->progress == nullptr and status->labelFilename != nullptr)
        {
            statusLayout->removeWidget(status->labelFilename);
            status->labelFilename->hide();
            delete status->labelFilename;
            status->labelFilename = nullptr;
            tilesDownloadStatus.remove(fileName);
            delete status;
        }
    }
}

void CRouterBRouterTilesSelect::initializeTiles()
{
    tilesWebPage.mainFrame()->load(QUrl(setup->segmentsUrl));
    readTiles();
}

void CRouterBRouterTilesSelect::slotLoadOnlineTilesRequestFinished()
{
    onlineTiles.clear();
    QVector<QPoint> onlineTilesTmp;
    QWebElement htmlElement = tilesWebPage.mainFrame()->documentElement();
    QWebElementCollection anchorElements = htmlElement.findAll("table tr td a");

    if (anchorElements.count() > 0)
    {
        // 'E10_N20.rd5'
        QRegExp rxTileName("([EW])(\\d{1,3})_([NS])(\\d{1,3})\\.rd5");

        // '16-Feb-2017 20:48  '
        // const QString dateFormat = "d-MMM-yyyy H:mm";
        // QDateFormat conversion depends on user-locale, doesn't work here
        QRegExp rxDate("(\\d{1,2})-(Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec)-(\\d{4}) (\\d{1,2}):(\\d{2})");

        // 8.2M 271K 9.3K
        QRegExp rxSize(" {0,2}(\\d{1,3}|\\d\\.\\d)([KMG])");

        for (QWebElement anchorElement : anchorElements)
        {
            QString tileName = anchorElement.toPlainText();
            //only anchors matching the desired pattern
            if (rxTileName.indexIn(tileName) > -1)
            {
                QWebElement dateElement = anchorElement.parent().nextSibling();
                QWebElement sizeElement = dateElement.nextSibling();

                tile_s newTile;
                newTile.tile = tileFromFileName(tileName);

                QString date = dateElement.toPlainText();
                if (rxDate.indexIn((date)) > -1)
                {
                    int day = rxDate.cap(1).toInt();
                    QString monthStr = rxDate.cap(2);
                    int month = monthStr == "Jan" ? 1 :
                                monthStr == "Feb" ? 2 :
                                monthStr == "Mar" ? 3 :
                                monthStr == "Apr" ? 4 :
                                monthStr == "May" ? 5 :
                                monthStr == "Jun" ? 6 :
                                monthStr == "Jul" ? 7 :
                                monthStr == "Aug" ? 8 :
                                monthStr == "Sep" ? 9 :
                                monthStr == "Oct" ? 10 :
                                monthStr == "Nov" ? 11 :
                                                    12;
                    int year = rxDate.cap(3).toInt();
                    int hour = rxDate.cap(4).toInt();
                    int min  = rxDate.cap(5).toInt();

                    newTile.date = QDateTime(QDate(year,month,day),QTime(hour,min,0));
                }

                QString size = sizeElement.toPlainText();
                if (rxSize.indexIn(size) > -1)
                {
                    newTile.size = rxSize.cap(1).toFloat() * (rxSize.cap(2) == "M" ? 1048576 :
                                                              rxSize.cap(2) == "G" ? 1073741824 :
                                                              rxSize.cap(2) == "K" ? 1024 :
                                                                                     1);
                }
                onlineTiles << newTile;
                onlineTilesTmp << newTile.tile;
            }
        }
    }
    invalidTiles.clear();
    for (int x = -180; x < 180; x += 5)
    {
        for (int y = -85; y < 85; y += 5)
        {
            QPoint tmp(x,y);
            if (!onlineTilesTmp.contains(tmp))
            {
                invalidTiles << tmp;
            }
        }
    }
    readTiles();
}

const QString CRouterBRouterTilesSelect::formatSize(const qint64 size)
{
    if (size >= 2147483648)
    {
        return QString("%1G").arg(size/1073741824);
    }
    else if (size >= 1073741824)
    {
        return QString("%1G").arg(qreal(size/107374182)/10);
    }
    else if (size >= 2097152)
    {
        return QString("%1M").arg(size/1048576);
    }
    else if (size >= 1048576)
    {
        return QString("%1M").arg(qreal(size/104858)/10);
    }
    else if (size >= 2048)
    {
        return QString("%1K").arg(size/1024);
    }
    else if (size >= 1000)
    {
        return QString("%1K").arg(qreal(size/102)/10);
    }
    else
    {
        return QString("%1").arg(size);
    }
}

const QPoint CRouterBRouterTilesSelect::tileFromFileName(const QString fileName)
{
    // 'E10_N20.rd5'
    QRegExp rxTileName("([EW])(\\d{1,3})_([NS])(\\d{1,3})\\.rd5");
    if (rxTileName.indexIn(fileName) > -1)
    {
        return QPoint(rxTileName.cap(2).toInt() * (rxTileName.cap(1) == "E" ? 1 : -1)
                     ,rxTileName.cap(4).toInt() * (rxTileName.cap(3) == "N" ? 1 : -1));
    }
    else
    {
        throw new CRouterBRouterSetupException();
    }
}

const QString CRouterBRouterTilesSelect::fileNameFromTile(const QPoint tile)
{
    return QString("%1%2_%3%4.rd5").arg(tile.x()<0 ? "W" : "E")
                                   .arg(abs(tile.x()))
                                   .arg(tile.y()<0 ? "S" : "N")
                                   .arg(abs(tile.y()));
}

const QString CRouterBRouterTilesSelect::absoluteFileNameFromTile(const QPoint tile)
{
    return segmentsDir().absoluteFilePath(fileNameFromTile(tile));
}

QDir CRouterBRouterTilesSelect::segmentsDir()
{
    return QDir(QDir(setup->localDir).absoluteFilePath(setup->localSegmentsDir));
}

const CRouterBRouterTilesSelect::tile_s CRouterBRouterTilesSelect::getOnlineTileData(const QPoint tile)
{
    for (tile_s tileData : onlineTiles)
    {
        if (tileData.tile == tile)
        {
            return tileData;
        }
    }
    return noTileData;
}

const CRouterBRouterTilesSelect::tile_s CRouterBRouterTilesSelect::getLocalTileData(const QPoint tile)
{
    QFileInfo info(absoluteFileNameFromTile(tile));
    if (info.exists())
    {
        tile_s tileData = { tile, info.created(), info.size() };
        return tileData;
    }
    return noTileData;
}

void CRouterBRouterTilesSelect::slotDownload()
{
    for (const QPoint tile : selectedTiles)
    {
        const QString fileName = fileNameFromTile(tile);
        QHash<QString,status_s*>::const_iterator it = tilesDownloadStatus.constFind(fileName);
        if (it != tilesDownloadStatus.constEnd())
        {
            status_s * status = it.value();
            if (!currentTiles.contains(tile) and status->progress == nullptr)
            {
                const QDir dir = segmentsDir();
                if (!dir.exists())
                {
                    QDir(setup->localDir).mkpath(setup->localSegmentsDir);
                }

                status->file = new QFile(dir.absoluteFilePath(fileName+".tmp"));
                status->file->open(QIODevice::WriteOnly);

                const QString tileUrl = setup->segmentsUrl + fileName;
                const QUrl url(tileUrl);

                QNetworkRequest request;
                request.setUrl(url);

                QNetworkReply* reply = tilesDownloadManager->get(request);
                reply->setProperty("tile", fileName);

                if (status->labelStatus != nullptr)
                {
                    statusLayout->removeWidget(status->labelStatus);
                    status->labelStatus->hide();
                    delete status->labelStatus;
                    status->labelStatus = nullptr;
                }

                int row;
                QFormLayout::ItemRole role;
                statusLayout->getWidgetPosition(dynamic_cast<QWidget*>(status->labelFilename),&row,&role);
                if (row > -1)
                {
                    status->progress = new CRouterBRouterProgressBar(this);
                    statusLayout->setWidget(row,QFormLayout::FieldRole,dynamic_cast<QWidget*>(status->progress));
                    connect(reply, &QNetworkReply::downloadProgress, status->progress, &CRouterBRouterProgressBar::updateProgress);
                }
                tilesDownloadManagerReplies << reply;
                outstandingTiles << tile;

                connect(reply, &QNetworkReply::readyRead, this, &CRouterBRouterTilesSelect::slotDownloadReadReady);
            }
        }
    }
    slotClearSelection();
}

void CRouterBRouterTilesSelect::slotDownloadReadReady()
{
    for (QNetworkReply * reply : tilesDownloadManagerReplies)
    {
        if (reply->bytesAvailable() > 0)
        {
            QString fileName = reply->property("tile").toString();
            QHash<QString,status_s*>::const_iterator it = tilesDownloadStatus.constFind(fileName);
            if (it != tilesDownloadStatus.constEnd())
            {
                status_s * status = it.value();
                if (status->file != nullptr)
                {
                    status->file->write(reply->readAll());
                }
            }
        }
    }
}

void CRouterBRouterTilesSelect::slotDownloadFinished(QNetworkReply* reply)
{
    if (tilesDownloadManagerReplies.contains(reply))
    {
        tilesDownloadManagerReplies.remove(tilesDownloadManagerReplies.indexOf(reply));
    }

    QString fileName = reply->property("tile").toString();
    QPoint tile = tileFromFileName(fileName);

    if (outstandingTiles.contains(tile))
    {
        outstandingTiles.remove(outstandingTiles.indexOf(tile));
    }

    QHash<QString,status_s*>::const_iterator it = tilesDownloadStatus.constFind(fileName);
    if (it != tilesDownloadStatus.constEnd())
    {
        status_s * status = it.value();
        if (status->labelFilename != nullptr)
        {
            statusLayout->removeWidget(status->labelFilename);
            status->labelFilename->hide();
            delete status->labelFilename;
            status->labelFilename = nullptr;
        }
        if (status->progress != nullptr)
        {
            statusLayout->removeWidget(status->progress);
            status->progress->hide();
            delete status->progress;
            status->progress = nullptr;
        }
        if (status->file != nullptr)
        {
            if(reply->error() == QNetworkReply::NoError)
            {
                status->file->write(reply->readAll());
                status->file->close();
                status->file->rename(absoluteFileNameFromTile(tile));
            }
            else
            {
                status->file->close();
                status->file->remove();
                //TODO add errormessage
            }
            delete status->file;
        }
        tilesDownloadStatus.remove(fileName);
        delete status;
        reply->deleteLater();
    }
    readTiles();
}

void CRouterBRouterTilesSelect::deleteTile(const QPoint tile)
{
    QFile segment(absoluteFileNameFromTile(tile));
    segment.remove();
    readTiles();
}

void CRouterBRouterTilesSelect::readTiles()
{
    outdatedTiles.clear();
    currentTiles.clear();

    QDir dir = segmentsDir();
    QStringList segments = dir.entryList();
    QRegExp rxTileName("([EW])(\\d{1,3})_([NS])(\\d{1,3})\\.rd5$");
    for (QString segment : segments)
    {
        if (rxTileName.indexIn(segment) > -1)
        {
            QPoint tile = tileFromFileName(segment);
            if (onlineTiles.isEmpty())
            {
                outdatedTiles << tile;
            }
            else
            {
                for (tile_s online : onlineTiles)
                {
                    if (online.tile == tile)
                    {
                        QFileInfo info = QFileInfo(dir,segment);
                        if (info.created() > online.date)
                        {
                            currentTiles << tile;
                        }
                        else
                        {
                            outdatedTiles << tile;
                        }
                        break;
                    }
                }
            }
        }
    }
    emit tilesChanged();
}
