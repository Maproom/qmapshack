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

#include "CMainWindow.h"
#include "canvas/CCanvas.h"
#include "gis/rte/router/brouter/CRouterBRouterSetup.h"
#include "gis/rte/router/brouter/CRouterBRouterTilesSelect.h"
#include "gis/rte/router/brouter/CRouterBRouterTilesSelectArea.h"
#include "gis/rte/router/brouter/CRouterBRouterTilesSelectLayout.h"
#include "gis/rte/router/brouter/CRouterBRouterTilesStatus.h"
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QWebElement>
#include <QWebFrame>

const QPoint CRouterBRouterTilesSelect::noTile = QPoint(INT_MIN, INT_MIN);
const int CRouterBRouterTilesSelect::minTileLat = -180;
const int CRouterBRouterTilesSelect::maxTileLat =  180;
const int CRouterBRouterTilesSelect::minTileLon =  -85;
const int CRouterBRouterTilesSelect::maxTileLon =   85;
const int CRouterBRouterTilesSelect::tileSize   =    5;

CRouterBRouterTilesSelect::CRouterBRouterTilesSelect(QWidget *parent)
    : QWidget(parent)
{
    for (int x = minTileLat; x < maxTileLat; x += tileSize)
    {
        for (int y = minTileLon; y < maxTileLon; y += tileSize)
        {
            CRouterBRouterTilesStatus * status = new CRouterBRouterTilesStatus(this);
            tilesDownloadStatus.insert(fileNameFromTile(QPoint(x,y)),status);
        }
    }

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
    view.setValue("map/zoomIndex",16);
    view.setValue("scales",1);
    view.setValue("proj","+proj=merc");
    view.setValue("grid/proj","+proj=longlat +datum=WGS84 +no_defs");

    CCanvas * canvas = new CCanvas(widgetSelect,"BRouterTileDownload");
    // clone canvas by a temporary configuration file

    canvas->loadConfig(view);

    selectArea = new CRouterBRouterTilesSelectArea(widgetSelect,canvas);

    QLayout * selectLayout = new CRouterBRouterTilesSelectLayout(widgetSelect);
    selectLayout->addWidget(canvas);
    selectLayout->addWidget(selectArea);
    canvas->lower();
    canvas->show();
    selectArea->show();

    QHBoxLayout * statusLayout = new QHBoxLayout();
    outerLayout->addLayout(statusLayout);

    statusLabel = new QLabel(this);
    statusProgress = new QProgressBar(this);

    statusLayout->addWidget(statusLabel);
    statusLayout->addWidget(statusProgress);

    statusLabel->setText(tr("available routing-data is being determined."));
    statusProgress->setVisible(false);

    errorLabel = new QLabel(this);
    outerLayout->addWidget(errorLabel);
    errorLabel->setVisible(false);

    QHBoxLayout * buttonsLayout = new QHBoxLayout();
    outerLayout->addLayout(buttonsLayout);

    pushSelectOutdated = new QPushButton(tr("Select outdated"), this);
    pushClearSelection = new QPushButton(tr("Clear Selection"), this);
    pushDeleteSelection = new QPushButton(tr("Delete selection"), this);
    pushDownload = new QPushButton(tr("Download"), this);

    buttonsLayout->addWidget(pushSelectOutdated);
    buttonsLayout->addWidget(pushClearSelection);
    buttonsLayout->addWidget(pushDeleteSelection);
    buttonsLayout->addWidget(pushDownload);

    tilesDownloadManager = new QNetworkAccessManager(this);

    tilesWebPage = new QWebPage(this);

    connect(pushClearSelection, &QPushButton::clicked, this, &CRouterBRouterTilesSelect::slotClearSelection);
    connect(pushDeleteSelection, &QPushButton::clicked, this, &CRouterBRouterTilesSelect::slotDeleteSelected);
    connect(pushSelectOutdated, &QPushButton::clicked, this, &CRouterBRouterTilesSelect::slotSelectOutdated);
    connect(pushDownload, &QPushButton::clicked, this, &CRouterBRouterTilesSelect::slotDownload);
    connect(selectArea, &CRouterBRouterTilesSelectArea::sigTileClicked, this, &CRouterBRouterTilesSelect::slotTileClicked);
    connect(selectArea, &CRouterBRouterTilesSelectArea::sigTileToolTipChanged, this, &CRouterBRouterTilesSelect::slotTileToolTipChanged);
    connect(tilesWebPage, &QWebPage::loadFinished, this, &CRouterBRouterTilesSelect::slotLoadOnlineTilesRequestFinished);
    connect(tilesDownloadManager, &QNetworkAccessManager::finished, this, &CRouterBRouterTilesSelect::slotDownloadFinished);
}

CRouterBRouterTilesSelect::~CRouterBRouterTilesSelect()
{
}

void CRouterBRouterTilesSelect::setSetup(CRouterBRouterSetup * setup)
{
    this->setup = setup;
}

void CRouterBRouterTilesSelect::slotTileClicked(const QPoint &tile)
{
    CRouterBRouterTilesStatus * status = getTileStatus(tile);
    if (status->isLocal || status->isRemote)
    {
        status->isSelected = !status->isSelected;
        update();
    }
}

void CRouterBRouterTilesSelect::slotSelectOutdated()
{
    bool changed(false);

    for (QHash<QString,CRouterBRouterTilesStatus * >::const_iterator it = tilesDownloadStatus.constBegin();
         it != tilesDownloadStatus.constEnd();
         ++it)
    {
        CRouterBRouterTilesStatus * status = it.value();
        if (!status->isSelected && status->isOutdated)
        {
            changed = true;
            status->isSelected = true;
        }
    }
    if (changed)
    {
        update();
    }
}

void CRouterBRouterTilesSelect::slotDeleteSelected()
{
    bool changed(false);

    for (QHash<QString,CRouterBRouterTilesStatus * >::const_iterator it = tilesDownloadStatus.constBegin();
         it != tilesDownloadStatus.constEnd();
         ++it)
    {
        CRouterBRouterTilesStatus * status = it.value();
        if (status->isSelected)
        {
            status->isSelected = false;
            changed = true;
            if (status->isLocal)
            {
                QFile segment(segmentsDir().absoluteFilePath(it.key()));
                if (!segment.remove())
                {
                    error(tr("Error removing %1: %2")
                          .arg(segment.fileName())
                          .arg(segment.errorString()));
                    break;
                }
                status->isLocal = false;
            }
        }
    }
    if (changed)
    {
        update();
    }
}

void CRouterBRouterTilesSelect::slotClearSelection()
{
    bool changed(false);
    for (QHash<QString,CRouterBRouterTilesStatus * >::const_iterator it = tilesDownloadStatus.constBegin();
         it != tilesDownloadStatus.constEnd();
         ++it)
    {
        CRouterBRouterTilesStatus * status = it.value();
        if (status->isSelected)
        {
            changed = true;
            status->isSelected = false;
        }
    }
    if (changed)
    {
        update();
    }
}

void CRouterBRouterTilesSelect::updateButtons() const
{
    bool hasSelected(false);
    bool hasLocalSelected(false);
    bool hasUnselectedOutdated(false);
    bool hasSelectedForDownload(false);

    for (QHash<QString,CRouterBRouterTilesStatus * >::const_iterator it = tilesDownloadStatus.constBegin();
         it != tilesDownloadStatus.constEnd();
         ++it)
    {
        CRouterBRouterTilesStatus * status = it.value();
        if (status->isSelected)
        {
            hasSelected = true;

            if (status->isLocal)
            {
                hasLocalSelected = true;
            }
            if (status->isOutdated || !status->isLocal)
            {
                hasSelectedForDownload = true;
            }
        }
        else if (status->isOutdated)
        {
            hasUnselectedOutdated = true;
        }
    }

    pushClearSelection->setEnabled(hasSelected);
    pushDeleteSelection->setEnabled(hasLocalSelected);
    pushSelectOutdated->setEnabled(hasUnselectedOutdated);
    pushDownload->setEnabled(hasSelectedForDownload);
}

void CRouterBRouterTilesSelect::initialize()

{
    try
    {
        for (QHash<QString,CRouterBRouterTilesStatus * >::const_iterator it = tilesDownloadStatus.constBegin();
             it != tilesDownloadStatus.constEnd();
             ++it)
        {
            CRouterBRouterTilesStatus * status = it.value();
            status->isLocal = false;
            status->isRemote = false;
            status->isOutdated = false;
        }
        const QDir &dir = segmentsDir();
        if (!dir.exists())
        {
            if (!QDir(setup->localDir).mkpath(setup->localSegmentsDir))
            {
                throw tr("Error creating segments directory %1").arg(dir.path());
            }
        }

        const QStringList &segments = dir.entryList();
        const QRegExp rxTileName("([EW])(\\d{1,3})_([NS])(\\d{1,3})\\.rd5$");
        for (const QString &segment : segments)
        {
            if (rxTileName.indexIn(segment) > -1)
            {
                const QPoint& tile = tileFromFileName(segment);
                if (tile != noTile)
                {
                    CRouterBRouterTilesStatus * status = getTileStatus(tile);

                    const QFileInfo &info = QFileInfo(dir,segment);
                    status->localDate = info.created();
                    status->localSize = info.size();
                    status->isLocal = true;
                }
            }
        }
        clearError();
        initialized = true;
        tilesWebPage->mainFrame()->load(QUrl(setup->segmentsUrl));
    }
    catch (const QString &msg)
    {
        error(msg);
    }
    update();
    emit sigCompleteChanged();
}

void CRouterBRouterTilesSelect::slotLoadOnlineTilesRequestFinished(bool ok)
{
    try
    {
        if (!ok)
        {
            throw tr("Network Error");
        }
        else
        {
            const QWebElement &htmlElement = tilesWebPage->mainFrame()->documentElement();
            const QWebElementCollection &anchorElements = htmlElement.findAll("table tr td a");

            if (anchorElements.count() == 0)
            {
                throw tr("invalid result, no files found");
            }

            // 'E10_N20.rd5'
            const QRegExp rxTileName("([EW])(\\d{1,3})_([NS])(\\d{1,3})\\.rd5");

            // '16-Feb-2017 20:48  '
            // const QString dateFormat = "d-MMM-yyyy H:mm";
            // QDateFormat conversion depends on user-locale, doesn't work here
            const QRegExp rxDate("(\\d{1,2})-(Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec)-(\\d{4}) (\\d{1,2}):(\\d{2})");

            // 8.2M 271K 9.3K
            const QRegExp rxSize(" {0,2}(\\d{1,3}|\\d\\.\\d)([KMG])");

            for (const QWebElement &anchorElement : anchorElements)
            {
                const QString &tileName = anchorElement.toPlainText();
                //only anchors matching the desired pattern
                if (rxTileName.indexIn(tileName) > -1)
                {
                    const QWebElement &dateElement = anchorElement.parent().nextSibling();
                    const QWebElement &sizeElement = dateElement.nextSibling();

                    const QPoint &tile = tileFromFileName(tileName);

                    if (tile != noTile)
                    {
                        CRouterBRouterTilesStatus * status = getTileStatus(tile);
                        if (status != nullptr)
                        {
                            status->isRemote = true;

                            const QString &date = dateElement.toPlainText();
                            if (rxDate.indexIn(date) < 0)
                            {
                                throw tr("cannot parse: %1 is not a date").arg(date);
                            }
                            int day = rxDate.cap(1).toInt();
                            const QString &monthStr = rxDate.cap(2);
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

                            status->remoteDate = QDateTime(QDate(year,month,day),QTime(hour,min,0));

                            const QString &size = sizeElement.toPlainText();
                            if (rxSize.indexIn(size) < 0)
                            {
                                throw tr("cannot parse: %1 is not a valid size").arg(size);
                            }
                            status->remoteSize = rxSize.cap(1).toFloat() * (rxSize.cap(2) == "M" ? 1048576 :
                                                                            rxSize.cap(2) == "G" ? 1073741824 :
                                                                            rxSize.cap(2) == "K" ? 1024 :
                                                                            1);
                            if (status->isLocal && status->remoteDate > status->localDate)
                            {
                                status->isOutdated = true;
                            }
                        }
                    }
                }
            }
        }
        clearError();
    }
    catch (const QString &msg)
    {
        error(tr("Error retrieving available routing data from %1: %2").arg(setup->segmentsUrl).arg(msg));
    }
    update();
}

QString CRouterBRouterTilesSelect::formatSize(const qint64 size)
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

QPoint CRouterBRouterTilesSelect::tileFromFileName(const QString &fileName)
{
    // 'E10_N20.rd5'
    const QRegExp rxTileName("([EW])(\\d{1,3})_([NS])(\\d{1,3})\\.rd5");
    if (rxTileName.indexIn(fileName) > -1)
    {
        return QPoint(rxTileName.cap(2).toInt() * (rxTileName.cap(1) == "E" ? 1 : -1)
                      ,rxTileName.cap(4).toInt() * (rxTileName.cap(3) == "N" ? 1 : -1));
    }
    else
    {
        return noTile;
    }
}

QString CRouterBRouterTilesSelect::fileNameFromTile(const QPoint tile)
{
    return QString("%1%2_%3%4.rd5").arg(tile.x()<0 ? "W" : "E")
           .arg(abs(tile.x()))
           .arg(tile.y()<0 ? "S" : "N")
           .arg(abs(tile.y()));
}

QDir CRouterBRouterTilesSelect::segmentsDir() const
{
    return QDir(QDir(setup->localDir).absoluteFilePath(setup->localSegmentsDir));
}

void CRouterBRouterTilesSelect::slotDownload()
{
    bool changed(false);

    for (QHash<QString,CRouterBRouterTilesStatus*>::const_iterator it = tilesDownloadStatus.constBegin();
         it != tilesDownloadStatus.constEnd();
         ++it)
    {
        CRouterBRouterTilesStatus * status = it.value();
        if (status->isSelected && (status->isOutdated || !status->isLocal) && status->file == nullptr)
        {
            const QString &fileName = it.key();

            const QDir &dir = segmentsDir();
            if (!dir.exists())
            {
                error(tr("segments directory does not exist: ").arg(dir.path()));
                break;
            }

            status->isSelected = false;
            changed = true;

            status->file = new QFile(dir.absoluteFilePath(fileName+".tmp"));
            if (!status->file->open(QIODevice::WriteOnly))
            {
                const QString tmpName = status->file->fileName();
                const QString tmpError = status->file->errorString();
                delete status->file;
                status->file = nullptr;
                error(tr("error creating file %1: %2").arg(tmpName).arg(tmpError));
                break;
            }

            QNetworkReply* reply = tilesDownloadManager->get(QNetworkRequest(QUrl(setup->segmentsUrl + fileName)));
            reply->setProperty("tile", fileName);

            tilesDownloadManagerReplies << reply;

            connect(reply, &QNetworkReply::downloadProgress, status, &CRouterBRouterTilesStatus::slotUpdateProgress);
            connect(reply, &QNetworkReply::readyRead, this, &CRouterBRouterTilesSelect::slotDownloadReadReady);
        }
    }

    if (changed)
    {
        update();
    }
}

void CRouterBRouterTilesSelect::slotDownloadReadReady()
{
    for (QNetworkReply * reply : tilesDownloadManagerReplies)
    {
        if (reply->bytesAvailable() > 0)
        {
            try
            {
                const QString &fileName = reply->property("tile").toString();
                QHash<QString,CRouterBRouterTilesStatus*>::const_iterator it = tilesDownloadStatus.constFind(fileName);
                if (it == tilesDownloadStatus.constEnd())
                {
                    throw tr("no valid request for filename %1").arg(fileName);
                }
                CRouterBRouterTilesStatus * status = it.value();
                if (status->file == nullptr)
                {
                    throw tr("no open file assigned to request for %1").arg(fileName);
                }
                if (status->file->write(reply->readAll()) < 0)
                {
                    QString tmpName = status->file->fileName();
                    QString msg = status->file->errorString();
                    status->file->close();
                    status->file->remove();
                    delete status->file;
                    status->file = nullptr;
                    throw tr("error writing to file %1: %2").arg(tmpName).arg(msg);
                }
            }
            catch (const QString &msg)
            {
                reply->abort();
                error(msg);
            }
        }
    }
    updateStatus();
}

void CRouterBRouterTilesSelect::slotDownloadFinished(QNetworkReply* reply)
{
    if (tilesDownloadManagerReplies.contains(reply))
    {
        tilesDownloadManagerReplies.remove(tilesDownloadManagerReplies.indexOf(reply));
    }
    reply->deleteLater();

    const QString &fileName = reply->property("tile").toString();
    QHash<QString,CRouterBRouterTilesStatus*>::const_iterator it = tilesDownloadStatus.constFind(fileName);
    if (it == tilesDownloadStatus.constEnd())
    {
        error(tr("no valid request for filename %1").arg(fileName));
    }
    else
    {
        CRouterBRouterTilesStatus * status = it.value();
        status->isLocal = false;
        if (status->file == nullptr)
        {
            error(tr("no open file assigned to request for %1").arg(fileName));
        }
        else
        {
            if(reply->error() != QNetworkReply::NoError)
            {
                error(fileName + ": "+reply->errorString());
            }
            else if (status->file->write(reply->readAll()) < 0)
            {
                error(tr("error writing to file %1: %2")
                      .arg(status->file->fileName())
                      .arg(status->file->errorString()));
                status->file->close();
                status->file->remove();
            }
            else if (!status->file->rename(segmentsDir().absoluteFilePath(it.key())))
            {
                error(tr("error renaming file %1 to %2: %3")
                      .arg(status->file->fileName())
                      .arg(segmentsDir().absoluteFilePath(it.key()))
                      .arg(status->file->errorString()));
                status->file->close();
                status->file->remove();
            }
            else
            {
                status->isLocal = true;
                status->isOutdated = false;
                status->file->close();
                QFileInfo info(*status->file);
                status->localDate = info.created();
                status->localSize = info.size();
                clearError();
            }
            delete status->file;
            status->file = nullptr;
        }
    }
    update();
}

void CRouterBRouterTilesSelect::cancelDownload() const
{
    for (QNetworkReply * reply : tilesDownloadManagerReplies)
    {
        reply->abort();
    }
}

void CRouterBRouterTilesSelect::update()
{
    updateButtons();
    updateStatus();
    updateTiles();
}

void CRouterBRouterTilesSelect::updateStatus()
{
    int numCurrent(0);
    int sizeCurrent(0);
    int numOutdated(0);
    int sizeOutdated(0);
    int numOutstanding(0);
    int sizeDownloadMax(0);
    int sizeOutstanding(0);
    int sizeDownloaded(0);

    bool downloading(false);

    for (QHash<QString,CRouterBRouterTilesStatus*>::const_iterator it = tilesDownloadStatus.constBegin();
         it != tilesDownloadStatus.constEnd();
         ++it)
    {
        const CRouterBRouterTilesStatus * status = it.value();

        if (status->file != nullptr && status->progressMax > 0)
        {
            numOutstanding++;
            sizeDownloadMax += status->progressMax;
            sizeOutstanding += status->progressMax - status->progressVal;
            sizeDownloaded += status->progressVal;
            downloading = true;
        }
        else if (status->isSelected && status->isRemote && (!status->isLocal || status->isOutdated))
        {
            numOutstanding++;
            sizeOutstanding += status->remoteSize;
        }
        if (status->isOutdated)
        {
            numOutdated++;
            sizeOutdated += status->localSize;
        }
        else if (status->isLocal)
        {
            numCurrent++;
            sizeCurrent += status->localSize;
        }
    }

    statusLabel->setText(QString(tr("up-to-date: %1 (%2), outdated: %3 (%4), to be downloaded: %5 (%6)"))
                         .arg(numCurrent).arg(formatSize(sizeCurrent))
                         .arg(numOutdated).arg(formatSize(sizeOutdated))
                         .arg(numOutstanding).arg(formatSize(sizeOutstanding)));
    statusProgress->setVisible(downloading);
    statusProgress->setRange(0,sizeDownloadMax);
    statusProgress->setValue(sizeDownloaded);

    if (this->downloading != downloading || (numOutstanding > 0) != downloadSelected)
    {
        this->downloading = downloading;
        downloadSelected = numOutstanding > 0;
        emit sigCompleteChanged();
    }
}

void CRouterBRouterTilesSelect::error(const QString &error) const
{
    errorLabel->setText(error);
    errorLabel->setVisible(true);
}

void CRouterBRouterTilesSelect::clearError() const
{
    errorLabel->setVisible(false);
}

CRouterBRouterTilesStatus * CRouterBRouterTilesSelect::getTileStatus(QPoint tile) const
{
    QHash<QString,CRouterBRouterTilesStatus*>::const_iterator it = tilesDownloadStatus.constFind(fileNameFromTile(tile));
    if (it != tilesDownloadStatus.constEnd())
    {
        return it.value();
    }
    return nullptr;
}

void CRouterBRouterTilesSelect::updateTiles() const
{
    QVector<QPoint> gridTiles;
    QVector<QPoint> invalidTiles;
    QVector<QPoint> outdatedTiles;
    QVector<QPoint> currentTiles;
    QVector<QPoint> outstandingTiles;
    QVector<QPoint> selectedTiles;

    for (QHash<QString,CRouterBRouterTilesStatus * >::const_iterator it = tilesDownloadStatus.constBegin();
         it != tilesDownloadStatus.constEnd();
         ++it)
    {
        const QPoint& tile = tileFromFileName(it.key());
        Q_ASSERT(tile != noTile);
        const CRouterBRouterTilesStatus * status = it.value();
        if (status->file != nullptr)
        {
            outstandingTiles << tile;
        }
        else if (status->isSelected)
        {
            selectedTiles << tile;
        }
        else if (status->isLocal)
        {
            if (status->isOutdated)
            {
                outdatedTiles << tile;
            }
            else
            {
                currentTiles << tile;
            }
        }
        else if (status->isRemote)
        {
            gridTiles << tile;
        }
        else
        {
            invalidTiles << tile;
        }
    }

    selectArea->setGridTiles(gridTiles);
    selectArea->setInvalidTiles(invalidTiles);
    selectArea->setOutdatedTiles(outdatedTiles);
    selectArea->setCurrentTiles(currentTiles);
    selectArea->setOutstandingTiles(outstandingTiles);
    selectArea->setSelectedTiles(selectedTiles);

    selectArea->update();
}

void CRouterBRouterTilesSelect::slotTileToolTipChanged(const QPoint &tile) const
{
    const CRouterBRouterTilesStatus * status = getTileStatus(tile);

    if (status->file != nullptr)
    {
        selectArea->setTileToolTip(QString(tr("being downloaded: %1 of %2"))
                                   .arg(formatSize(status->progressVal))
                                   .arg(formatSize(status->progressMax)));
    }
    else if (status->isOutdated)
    {
        selectArea->setTileToolTip(QString(tr("local data outdated (%1, %2 - remote %3, %4)"))
                                   .arg(formatSize(status->localSize))
                                   .arg(status->localDate.toString(Qt::DefaultLocaleShortDate))
                                   .arg(formatSize(status->remoteSize))
                                   .arg(status->remoteDate.toString(Qt::DefaultLocaleShortDate)));
    }
    else if (status->isLocal)
    {
        selectArea->setTileToolTip(QString(tr("local data up to date (%1, %2)"))
                                   .arg(formatSize(status->localSize))
                                   .arg(status->localDate.toString(Qt::DefaultLocaleShortDate)));
    }
    else if (status->isRemote)
    {
        selectArea->setTileToolTip(QString(tr("no local data, online available: %1 (%2)"))
                                   .arg(formatSize(status->remoteSize))
                                   .arg(status->remoteDate.toString(Qt::DefaultLocaleShortDate)));
    }
    else
    {
        selectArea->setTileToolTip(tr("no routing-data available"));
    }
}
