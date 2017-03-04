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

#include <QWebElement>
#include <QWebFrame>
#include <QNetworkRequest>
#include <QNetworkReply>
#include "CRouterBRouterSetup.h"
#include "CRouterBRouterTilesStatus.h"
#include "CRouterBRouterTilesSelect.h"
#include "CRouterBRouterTilesSelectArea.h"
#include "CRouterBRouterTilesSelectLayout.h"
#include "CRouterBRouterSetupException.h"
#include "CMainWindow.h"
#include "canvas/CCanvas.h"

CRouterBRouterTilesSelect::CRouterBRouterTilesSelect(QWidget *parent)
    : QWidget(parent)
{
    for (int x = -180; x < 180; x += 5)
    {
        for (int y = -85; y < 85; y += 5)
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
    view.setValue("grid/color",QColor(Qt::magenta).name());
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

    statusLabel->setText("test");

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

void CRouterBRouterTilesSelect::slotTileClicked(const QPoint & tile)
{
    CRouterBRouterTilesStatus * status = getTileStatus(tile);
    status->isSelected = !status->isSelected;
    updateButtons();
    updateStatus();
    updateTiles();
}

void CRouterBRouterTilesSelect::slotSelectOutdated()
{
    bool changed(false);

    for (QHash<QString,CRouterBRouterTilesStatus * >::const_iterator it = tilesDownloadStatus.constBegin();
         it != tilesDownloadStatus.constEnd();
         it++)
    {
        CRouterBRouterTilesStatus * status = it.value();
        if (!status->isSelected and status->isLocal and status->isRemote and status->remoteDate > status->localDate)
        {
            changed = true;
            status->isSelected = true;
        }
    }
    if (changed)
    {
        updateButtons();
        updateStatus();
        updateTiles();
    }
}

void CRouterBRouterTilesSelect::slotDeleteSelected()
{
    bool changed(false);
    for (QHash<QString,CRouterBRouterTilesStatus * >::const_iterator it = tilesDownloadStatus.constBegin();
         it != tilesDownloadStatus.constEnd();
         it++)
    {
        CRouterBRouterTilesStatus * status = it.value();
        if (status->isSelected)
        {
            if (status->isLocal)
            {
                QFile segment(segmentsDir().absoluteFilePath(it.key()));
                segment.remove();
                status->isLocal = false;
            }
            status->isSelected = false;
            changed = true;
        }
    }
    if (changed)
    {
        updateButtons();
        updateStatus();
        updateTiles();
    }
}

void CRouterBRouterTilesSelect::slotClearSelection()
{
    bool changed(false);
    for (QHash<QString,CRouterBRouterTilesStatus * >::const_iterator it = tilesDownloadStatus.constBegin();
         it != tilesDownloadStatus.constEnd();
         it++)
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
        updateButtons();
        updateStatus();
        updateTiles();
    }
}

void CRouterBRouterTilesSelect::updateButtons()
{
    bool hasSelected(false);
    bool hasLocalSelected(false);
    bool hasUnselectedOutdated(false);
    bool hasSelectedOutdated(false);

    for (QHash<QString,CRouterBRouterTilesStatus * >::const_iterator it = tilesDownloadStatus.constBegin();
         it != tilesDownloadStatus.constEnd();
         it++)
    {
        CRouterBRouterTilesStatus * status = it.value();
        if (status->isSelected)
        {
            hasSelected = true;

            if (status->isLocal)
            {
                hasLocalSelected = true;
            }
            if (status->isOutdated)
            {
                hasSelectedOutdated = true;
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
    pushDownload->setEnabled(hasSelectedOutdated);
}

void CRouterBRouterTilesSelect::initialize()
{
    for (QHash<QString,CRouterBRouterTilesStatus * >::const_iterator it = tilesDownloadStatus.constBegin();
         it != tilesDownloadStatus.constEnd();
         it++)
    {
        CRouterBRouterTilesStatus * status = it.value();
        status->isLocal = false;
        status->isRemote = false;
        status->isSelected = false;
        status->isOutdated = false;
    }
    QDir dir = segmentsDir();
    QStringList segments = dir.entryList();
    QRegExp rxTileName("([EW])(\\d{1,3})_([NS])(\\d{1,3})\\.rd5$");
    for (QString segment : segments)
    {
        if (rxTileName.indexIn(segment) > -1)
        {
            QPoint tile = tileFromFileName(segment);
            CRouterBRouterTilesStatus * status = getTileStatus(tile);

            QFileInfo info = QFileInfo(dir,segment);
            status->localDate = info.created();
            status->localSize = info.size();
            status->isLocal = true;
        }
    }
    tilesWebPage->mainFrame()->load(QUrl(setup->segmentsUrl));
}

void CRouterBRouterTilesSelect::slotLoadOnlineTilesRequestFinished()
{
    QWebElement htmlElement = tilesWebPage->mainFrame()->documentElement();
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

                QPoint tile = tileFromFileName(tileName);

                CRouterBRouterTilesStatus * status = getTileStatus(tile);
                if (status != nullptr)
                {
                    status->isRemote = true;

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

                        status->remoteDate = QDateTime(QDate(year,month,day),QTime(hour,min,0));
                    }

                    QString size = sizeElement.toPlainText();
                    if (rxSize.indexIn(size) > -1)
                    {
                        status->remoteSize = rxSize.cap(1).toFloat() * (rxSize.cap(2) == "M" ? 1048576 :
                                                                  rxSize.cap(2) == "G" ? 1073741824 :
                                                                  rxSize.cap(2) == "K" ? 1024 :
                                                                                         1);
                    }
                    if (status->isLocal and status->remoteDate > status->localDate)
                    {
                        status->isOutdated = true;
                    }
                }
            }
        }
    }
    updateStatus();
    updateButtons();
    updateTiles();
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

QPoint CRouterBRouterTilesSelect::tileFromFileName(const QString fileName)
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

    QHash<QString,CRouterBRouterTilesStatus*>::const_iterator it = tilesDownloadStatus.constBegin();
    if (it != tilesDownloadStatus.constEnd())
    {
        CRouterBRouterTilesStatus * status = it.value();
        if (status->isSelected and status->isRemote and status->file == nullptr)
        {
            QString fileName = it.key();

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

            connect(reply, &QNetworkReply::downloadProgress, status, &CRouterBRouterTilesStatus::updateProgress);

            tilesDownloadManagerReplies << reply;

            connect(reply, &QNetworkReply::readyRead, this, &CRouterBRouterTilesSelect::slotDownloadReadReady);

            status->isSelected = false;

            changed = true;
        }
    }
    if (changed)
    {
        updateButtons();
        updateStatus();
        updateTiles();
    }
}

void CRouterBRouterTilesSelect::slotDownloadReadReady()
{
    for (QNetworkReply * reply : tilesDownloadManagerReplies)
    {
        if (reply->bytesAvailable() > 0)
        {
            QString fileName = reply->property("tile").toString();
            QHash<QString,CRouterBRouterTilesStatus*>::const_iterator it = tilesDownloadStatus.constFind(fileName);
            if (it != tilesDownloadStatus.constEnd())
            {
                CRouterBRouterTilesStatus * status = it.value();
                if (status->file != nullptr)
                {
                    status->file->write(reply->readAll());
                }
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

    QString fileName = reply->property("tile").toString();
    QHash<QString,CRouterBRouterTilesStatus*>::const_iterator it = tilesDownloadStatus.constFind(fileName);
    if (it != tilesDownloadStatus.constEnd())
    {
        CRouterBRouterTilesStatus * status = it.value();
        if (status->file != nullptr)
        {
            if(reply->error() == QNetworkReply::NoError)
            {
                status->file->write(reply->readAll());
                status->file->close();
                status->file->rename(segmentsDir().absoluteFilePath(it.key()));
                status->isLocal = true;
                status->localDate = QFileInfo(*status->file).created();
            }
            else
            {
                status->file->close();
                status->file->remove();
                status->isLocal = false;
                //TODO add errormessage
            }
            delete status->file;
        }
        reply->deleteLater();
    }
    updateButtons();
    updateStatus();
    updateTiles();
}

void CRouterBRouterTilesSelect::updateStatus()
{
    int total(0);
    int value(0);
    int num(0);
    bool downloading(false);

    for (QHash<QString,CRouterBRouterTilesStatus*>::const_iterator it = tilesDownloadStatus.constBegin();
         it != tilesDownloadStatus.constEnd();
         it++)
    {
        const CRouterBRouterTilesStatus * status = it.value();

        if (status->file != nullptr and status->progressMax > 0)
        {
            total += status->progressMax;
            value += status->progressVal;
            downloading = true;
        }
        else
        {
            total += status->remoteSize;
        }
        num++;
    }

    statusLabel->setText(QString(tr("%1 files, %2 total")).arg(num).arg(formatSize(total)));
    statusProgress->setVisible(downloading);
    statusProgress->setRange(0,total);
    statusProgress->setValue(value);
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

void CRouterBRouterTilesSelect::updateTiles()
{
    QVector<QPoint> * invalidTiles = new QVector<QPoint>;
    QVector<QPoint> * outdatedTiles = new QVector<QPoint>;
    QVector<QPoint> * currentTiles = new QVector<QPoint>;
    QVector<QPoint> * outstandingTiles = new QVector<QPoint>;
    QVector<QPoint> * selectedTiles = new QVector<QPoint>;

    for (QHash<QString,CRouterBRouterTilesStatus * >::const_iterator it = tilesDownloadStatus.constBegin();
         it != tilesDownloadStatus.constEnd();
         it++)
    {
        QPoint tile = tileFromFileName(it.key());
        CRouterBRouterTilesStatus * status = it.value();
        if (status->isSelected)
        {
            *selectedTiles << tile;
        }
        else if (status->isLocal)
        {
            if (!status->isRemote or status->remoteDate > status->localDate)
            {
                *outdatedTiles << tile;
            }
            else
            {
                *currentTiles << tile;
            }
        }
        else if (!status->isRemote)
        {
            *invalidTiles << tile;
        }
        if (status->file != nullptr)
        {
            *outstandingTiles << tile;
        }
    }

    selectArea->setInvalidTiles(invalidTiles);
    selectArea->setOutdatedTiles(outdatedTiles);
    selectArea->setCurrentTiles(currentTiles);
    selectArea->setOutstandingTiles(outstandingTiles);
    selectArea->setSelectedTiles(selectedTiles);

    selectArea->update();
}

void CRouterBRouterTilesSelect::slotTileToolTipChanged(const QPoint & tile)
{
    CRouterBRouterTilesStatus * status = getTileStatus(tile);

    if (status->file != nullptr)
    {
        selectArea->setTileToolTip(QString(tr("being downloaded (%1 of %2)"))
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
        selectArea->setTileToolTip(QString(tr("no local data, online available (%1, %2)"))
                                   .arg(formatSize(status->remoteSize))
                                   .arg(status->remoteDate.toString(Qt::DefaultLocaleShortDate)));
    }
    else
    {
        selectArea->setTileToolTip(tr("no routing-data available"));
    }
}
