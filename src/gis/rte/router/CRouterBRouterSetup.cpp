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

#include "CRouterBRouterSetup.h"
#include "CRouterBRouterSetupException.h"

#include "helpers/CSettings.h"
#include <QtNetwork>
#include <QtScript>
#include <QtWebKit>
#include <QWebPage>
#include <QWebFrame>

CRouterBRouterSetup::CRouterBRouterSetup()
{
    tilesDownloadManager = new QNetworkAccessManager(this);
    tileDownloadRunning = false;

    connect(tilesDownloadManager, &QNetworkAccessManager::finished, this, &CRouterBRouterSetup::slotLoadOnlineTileDownloadFinished);
    connect(&tilesWebPage, &QWebPage::loadFinished, this, &CRouterBRouterSetup::slotLoadOnlineTilesRequestFinished);
}

CRouterBRouterSetup::~CRouterBRouterSetup()
{
}

void CRouterBRouterSetup::load()
{
    SETTINGS;
    cfg.beginGroup("Route/brouter");
    try
    {
        installMode = modeFromString(cfg.value("installMode",stringFromMode(defaultInstallMode)).toString());
    }
    catch(CRouterBRouterSetupException& e)
    {
        installMode = defaultInstallMode;
    }
    expertMode = cfg.value("expertMode",defaultExpertMode).toBool();
    onlineWebUrl = cfg.value("onlineWebUrl", defaultOnlineWebUrl).toString();
    onlineServiceUrl = cfg.value("onlineServiceUrl", defaultOnlineServiceUrl).toString();
    onlineProfilesUrl = cfg.value("onlineProfilesUrl", defaultOnlineProfilesUrl).toString();
    localDir = cfg.value("localDir", defaultLocalDir).toString();
    localProfileDir = cfg.value("localProfileDir", defaultLocalProfileDir).toString();
    localSegmentsDir = cfg.value("localSegmentsDir", defaultLocalSegmentsDir).toString();
    localHost = cfg.value("localHost", defaultLocalHost).toString();
    localPort = cfg.value("localPort", defaultLocalPort).toString();
    binariesUrl = cfg.value("binariesUrl",defaultBinariesUrl).toString();
    segmentsUrl = cfg.value("segmentsUrl",defaultSegmentsUrl).toString();
    cfg.endGroup();

    readProfiles(ModeLocal);
    readProfiles(ModeOnline);
}

void CRouterBRouterSetup::save()
{
    SETTINGS;
    cfg.beginGroup("Route/brouter");
    cfg.setValue("expertMode",expertMode);
    cfg.setValue("installMode", stringFromMode(installMode));
    cfg.setValue("onlineWebUrl", onlineWebUrl);
    cfg.setValue("onlineServiceUrl", onlineServiceUrl);
    cfg.setValue("onlineProfilesUrl", onlineProfilesUrl);
    cfg.setValue("localDir", localDir);
    cfg.setValue("localProfileDir", localProfileDir);
    cfg.setValue("localSegmentsDir", localSegmentsDir);
    cfg.setValue("localHost", localHost);
    cfg.setValue("localPort", localPort);
    cfg.setValue("binariesUrl", binariesUrl);
    cfg.setValue("segmentsUrl", segmentsUrl);
    cfg.endGroup();
}

const CRouterBRouterSetup::mode_e CRouterBRouterSetup::modeFromString(const QString mode)
{
    if (mode == "online")
    {
        return ModeOnline;
    }
    else if (mode == "local")
    {
        return ModeLocal;
    }
    throw CRouterBRouterSetupException();
}

const QString CRouterBRouterSetup::stringFromMode(const mode_e mode)
{
    switch(mode)
    {
    case ModeLocal:
    {
        return "local";
    }
    case ModeOnline:
    {
        return "online";
    }
    default:
    {
        throw CRouterBRouterSetupException();
    }
    }
}

void CRouterBRouterSetup::readProfiles()
{
    readProfiles(installMode);
}

void CRouterBRouterSetup::readProfiles(mode_e mode)
{
    const QDir dir = getProfileDir(mode);
    (mode == ModeLocal ? localProfiles : onlineProfiles).clear();
    if (dir.exists())
    {
        foreach(const QString profile,dir.entryList())
        {
            if(profile.endsWith(".brf"))
            {
                (mode == ModeLocal ? localProfiles : onlineProfiles) << profile.left(profile.length()-4);
            }
        }
    }
}

const QDir CRouterBRouterSetup::getProfileDir()
{
    return getProfileDir(installMode);
}

const QDir CRouterBRouterSetup::getProfileDir(mode_e mode)
{
    switch(mode)
    {
    case ModeLocal:
    {
        return QDir(QDir(localDir).absoluteFilePath(localProfileDir));
    }
    case ModeOnline:
    {
        QDir brouterDir(QDir(IAppSetup::getPlatformInstance()->defaultCachePath()).absoluteFilePath(onlineCacheDir));
        if (!brouterDir.exists())
        {
            brouterDir.mkpath(brouterDir.absolutePath());
        }
        return brouterDir;
    }
    default:
    {
        throw CRouterBRouterSetupException();
    }
    }
}

const QStringList CRouterBRouterSetup::getProfiles()
{
    return installMode == ModeLocal ? localProfiles : onlineProfiles;
}

const QString CRouterBRouterSetup::getProfileContent(const int index)
{
    return getProfileContent(installMode, getProfiles().at(index));
}

const QString CRouterBRouterSetup::getProfileContent(const QString profile)
{
    return getProfileContent(installMode, profile);
}

const QString CRouterBRouterSetup::getProfileContent(const mode_e mode, const QString profile)
{
    const QDir dir = getProfileDir(mode);
    const QString filename = dir.absoluteFilePath(profile + ".brf");
    QFile file(filename);
    if (file.exists())
    {
        file.open(QIODevice::ReadOnly);
        return QString(file.readAll());
    }
    return QString();
}

void CRouterBRouterSetup::loadOnlineConfig()
{
    QNetworkRequest request;
    const QUrl configUrl = QUrl(onlineWebUrl + "config.js");
    const QString configHost = configUrl.host();
    request.setUrl(configUrl);

    QNetworkAccessManager networkAccessManager;
    QNetworkReply * reply = networkAccessManager.get(request);
    reply->deleteLater();

    QEventLoop eventLoop;
    QObject::connect(reply, SIGNAL(finished()), &eventLoop, SLOT(quit()));
    eventLoop.exec();

    const QString jsConfig(reply->readAll());

    QScriptEngine engine;

    const QString jsSetup = QString(\
                "(function(){\
                   window = {};\
                   window.location = {};\
                   window.location.hostname = '%1';\
                   window.location.search = {};\
                   window.location.search.slice = function() {};\
                   URLSearchParams = function() {};\
                   BR = {};\
                  })();").arg(configHost);

//TODO: error handling
    const QString run1 = engine.evaluate(jsSetup).toString();
    const QString run2 = engine.evaluate(jsConfig).toString();

    const QScriptValue br = engine.globalObject().property("BR");
    const QScriptValue conf = br.property("conf");
    onlineServiceUrl = conf.property("host").toString();
    onlineProfilesUrl = conf.property("profilesUrl").toString();
    const QScriptValue profiles = conf.property("profiles");
    const qint32 len = profiles.property("length").toInt32();
    onlineProfilesAvailable.clear();
    for(qint32 i=0;i<len;i++)
    {
        onlineProfilesAvailable << profiles.property(i).toString();
    }
}

const QString CRouterBRouterSetup::getOnlineProfileContent(const QString profile)
{
    return QString(loadOnlineProfile(profile));
}

void CRouterBRouterSetup::installOnlineProfile(const QString profile)
{
    const QDir dir = getProfileDir();
    const QString filename = dir.absoluteFilePath(profile + ".brf");
    QFile file(filename);
    file.open(QIODevice::WriteOnly);
    file.write(loadOnlineProfile(profile));
    file.close();
}

const QByteArray CRouterBRouterSetup::loadOnlineProfile(const QString profile)
{
    QNetworkAccessManager networkAccessManager;

    QNetworkRequest request;
    const QUrl profileUrl = QUrl(onlineProfilesUrl + profile +".brf");
    request.setUrl(profileUrl);

    QNetworkReply * reply = networkAccessManager.get(request);
    reply->deleteLater();

    QEventLoop eventLoop;
    QObject::connect(reply, SIGNAL(finished()), &eventLoop, SLOT(quit()));
    eventLoop.exec();

    return reply->readAll();
}

void CRouterBRouterSetup::initializeTiles()
{
    tilesWebPage.mainFrame()->load(QUrl(segmentsUrl));
    readTiles();
}

void CRouterBRouterSetup::slotLoadOnlineTilesRequestFinished()
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
                    newTile.size = rxSize.cap(1).toFloat() * (rxSize.cap(2) == "M" ? 1000 :
                                                              rxSize.cap(2) == "G" ? 1000000 :
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

const QPoint CRouterBRouterSetup::tileFromFileName(QString fileName)
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

const QString CRouterBRouterSetup::fileNameFromTile(QPoint tile)
{
    return QString("%1%2_%3%4.rd5").arg(tile.x()<0 ? "W" : "E")
                                   .arg(abs(tile.x()))
                                   .arg(tile.y()<0 ? "S" : "N")
                                   .arg(abs(tile.y()));
}

void CRouterBRouterSetup::installOnlineTile(const QPoint tile)
{
    if (!outstandingTiles.contains(tile))
    {
        outstandingTiles << tile;
        emit tilesLocalChanged();
    }
    downloadOutstandingTiles();
}

void CRouterBRouterSetup::downloadOutstandingTiles()
{
    if (!tileDownloadRunning and outstandingTiles.size() > 0)
    {
        QString fileName = fileNameFromTile(outstandingTiles.first());
        QString tileUrl = segmentsUrl + fileName;
        QUrl url(tileUrl);

        QNetworkRequest request;
        request.setUrl(url);

        QNetworkReply* reply = tilesDownloadManager->get(request);

        reply->setProperty("tile", fileName);
        tileDownloadRunning = true;
    }
}

void CRouterBRouterSetup::slotLoadOnlineTileDownloadFinished(QNetworkReply* reply)
{
    tileDownloadRunning = false;

    if(reply->error() != QNetworkReply::NoError)
    {
        //TODO add statusmessage
        reply->deleteLater();
        return;
    }

    QString fileName = reply->property("tile").toString();
    QPoint tile = tileFromFileName(fileName);
    if (outstandingTiles.contains(tile))
    {
        outstandingTiles.remove(outstandingTiles.indexOf(tile));
    }
    QDir segmentsDir = QDir(QDir(localDir).absoluteFilePath(localSegmentsDir));
    if (!segmentsDir.exists())
    {
        QDir(localDir).mkpath(localSegmentsDir);
    }

    QFile out(segmentsDir.absoluteFilePath(fileName));

    out.open(QIODevice::WriteOnly);
    out.write(reply->readAll());
    out.close();

    reply->deleteLater();

    readTiles();
    downloadOutstandingTiles();
}

void CRouterBRouterSetup::deleteTile(const QPoint tile)
{
    QDir segmentsDir = QDir(QDir(localDir).absoluteFilePath(localSegmentsDir));
    QFile segment(segmentsDir.absoluteFilePath(fileNameFromTile(tile)));
    segment.remove();
    readTiles();
}

void CRouterBRouterSetup::readTiles()
{
    oldTiles.clear();
    currentTiles.clear();

    QDir segmentsDir = QDir(QDir(localDir).absoluteFilePath(localSegmentsDir));
    QStringList segments = segmentsDir.entryList();
    QRegExp rxTileName("([EW])(\\d{1,3})_([NS])(\\d{1,3})\\.rd5");
    for (QString segment : segments)
    {
        if (rxTileName.indexIn(segment) > -1)
        {
            QPoint tile = tileFromFileName(segment);
            for (tile_s online : onlineTiles)
            {
                if (online.tile == tile)
                {
                    QFileInfo info = QFileInfo(segmentsDir,segment);
                    if (info.created() > online.date)
                    {
                        currentTiles << tile;
                    }
                    else
                    {
                        oldTiles << tile;
                    }
                    break;
                }
            }
        }
    }
    emit tilesLocalChanged();
}

QVector<QPoint> CRouterBRouterSetup::getInvalidTiles()
{
    return invalidTiles;
}

QVector<QPoint> CRouterBRouterSetup::getCurrentTiles()
{
    return currentTiles;
}

QVector<QPoint> CRouterBRouterSetup::getOutdatedTiles()
{
    return oldTiles;
}

QVector<QPoint> CRouterBRouterSetup::getOnlineTilesAvailable()
{
    QVector<QPoint> tiles;
    for (tile_s online : onlineTiles)
    {
        tiles << online.tile;
    }
    return tiles;
}

QVector<QPoint> CRouterBRouterSetup::getOutstandingTiles()
{
    return outstandingTiles;
}
