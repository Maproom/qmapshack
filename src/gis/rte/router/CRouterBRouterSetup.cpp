/**********************************************************************************************
    Copyright (C) 2017 Norbert Truchsessr norbert.truchsess@t-online.de

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

#include "gis/rte/router/CRouterBRouterSetup.h"
#include "helpers/CSettings.h"
#include <QtNetwork>
#include <QtScript>

CRouterBRouterSetup::CRouterBRouterSetup()
{
}

CRouterBRouterSetup::~CRouterBRouterSetup()
{
}

void CRouterBRouterSetup::load()
{
    SETTINGS;
    cfg.beginGroup("Route/brouter");

    installMode = modeFromString(cfg.value("installMode",stringFromMode(Mode_Online)).toString());
    expertMode = cfg.value("expertMode",false).toBool();
    onlineWebUrl = cfg.value("onlineWebUrl", defaultOnlineWebUrl).toString();
    onlineServiceUrl = cfg.value("onlineServiceUrl", defaultOnlineServiceUrl).toString();
    onlineProfilesUrl = cfg.value("onlineProfilesUrl", defaultOnlineProfilesUrl).toString();
    localDir = cfg.value("localDir", defaultLocalDir).toString();
    localProfileDir = cfg.value("localProfileDir", defaultLocalProfileDir).toString();
    localSegmentsDir = cfg.value("localSegmentsDir", defaultLocalSegmentsDir).toString();
    localHost = cfg.value("localHost", defaultLocalHost).toString();
    localPort = cfg.value("localPort", defaultLocalPort).toString();
    binariesUrl = cfg.value("binariesUrl",defaultBinariesUrl).toString();
    cfg.endGroup();

    readLocalProfiles();
    readOnlineProfiles();

    if (onlineProfiles.isEmpty())
    {
        onlineProfiles << "trekking";
        onlineProfiles << "fastbike";
        onlineProfiles << "car-test";
        onlineProfiles << "safety";
        onlineProfiles << "shortest";
        onlineProfiles << "trekking-ignore-cr";
        onlineProfiles << "trekking-steep";
        onlineProfiles << "trekking-noferries";
        onlineProfiles << "trekking-nosteps";
        onlineProfiles << "moped";
        onlineProfiles << "rail";
        onlineProfiles << "river";
        onlineProfiles << "vm-forum-liegerad-schnell";
        onlineProfiles << "vm-forum-velomobil-schnell";
        onlineProfiles << "fastbike-lowtraffic";
        onlineProfiles << "fastbike-asia-pacific";
        onlineProfiles << "hiking-beta";
    }
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
    cfg.endGroup();
}

CRouterBRouterSetup::Mode CRouterBRouterSetup::modeFromString(QString mode)
{
    if (mode == "online")
    {
        return Mode_Online;
    }
    else if (mode == "local")
    {
        return Mode_Local;
    }
    else
    {
        return Mode_None;
    }
}

QString CRouterBRouterSetup::stringFromMode(Mode mode)
{
    switch(mode)
    {
    case Mode_Local:
    {
        return "local";
    }
    case Mode_Online:
    {
        return "online";
    }
    default:
        return "";
    }
}

void CRouterBRouterSetup::readLocalProfiles()
{
    readProfiles(Mode_Local);
}

void CRouterBRouterSetup::readOnlineProfiles()
{
    readProfiles(Mode_Online);
}

void CRouterBRouterSetup::readProfiles(Mode mode)
{
    QDir dir = getProfileDir(mode);
    (mode == Mode_Local ? localProfiles : onlineProfiles).clear();
    if (dir.exists())
    {
        QString profile;
        QStringList entries = dir.entryList();

        foreach(profile,entries)
        {
            if(profile.endsWith(".brf"))
            {
                (mode == Mode_Local ? localProfiles : onlineProfiles) << profile.left(profile.length()-4);
            }
        }
    }
}

QDir CRouterBRouterSetup::getLocalProfileDir()
{
    return getProfileDir(Mode_Local);
}

QDir CRouterBRouterSetup::getProfileDir(Mode mode)
{
    switch(mode)
    {
    case Mode_Local:
    {
        return QDir(QDir(localDir).absoluteFilePath(localProfileDir));
    }
    case Mode_Online:
    {
        return QDir(IAppSetup::getPlatformInstance()->defaultCachePath());
    }
    default:
    {
        return QDir();
    }
    }
}

QString CRouterBRouterSetup::readLocalProfile(QString profile)
{
    return readProfile(Mode_Local, profile);
}

QString CRouterBRouterSetup::readOnlineProfile(QString profile)
{
    return readProfile(Mode_Online, profile);
}

QString CRouterBRouterSetup::readProfile(Mode mode, QString profile)
{
    QDir dir = getProfileDir(mode);
    QString filename = dir.absoluteFilePath(profile + ".brf");
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
    QUrl configUrl = QUrl(onlineWebUrl + "config.js");
    QString configHost = configUrl.host();
    request.setUrl(configUrl);

    QNetworkAccessManager networkAccessManager;
    QNetworkReply * reply = networkAccessManager.get(request);
    reply->deleteLater();

    QEventLoop eventLoop;
    QObject::connect(reply, SIGNAL(finished()), &eventLoop, SLOT(quit()));
    eventLoop.exec();

    QString jsConfig(reply->readAll());

    QScriptEngine engine;

    QString jsSetup = QString(\
                "(function(){\
                   window = {};\
                   window.location = {};\
                   window.location.hostname = '%1';\
                   window.location.search = {};\
                   window.location.search.slice = function() {};\
                   URLSearchParams = function() {};\
                   BR = {};\
                  })();").arg(configHost);

    QString run1 = engine.evaluate(jsSetup).toString();
    QString run2 = engine.evaluate(jsConfig).toString();

    QScriptValue br = engine.globalObject().property("BR");
    QScriptValue conf = br.property("conf");
    onlineServiceUrl = conf.property("host").toString();
    onlineProfilesUrl = conf.property("profilesUrl").toString();
    QScriptValue profiles = conf.property("profiles");
    qint32 len = profiles.property("length").toInt32();
    onlineProfiles.clear();
    for(qint32 i=0;i<len;i++)
    {
        onlineProfiles << profiles.property(i).toString();
    }
}

void CRouterBRouterSetup::loadOnlineProfiles()
{
    QNetworkAccessManager networkAccessManager;

    QDir dir(IAppSetup::getPlatformInstance()->defaultCachePath());

    for(QString profile:onlineProfiles)
    {
        QNetworkRequest request;
        QUrl profileUrl = QUrl(onlineProfilesUrl + profile +".brf");
        request.setUrl(profileUrl);

        QNetworkReply * reply = networkAccessManager.get(request);
        reply->deleteLater();

        QEventLoop eventLoop;
        QObject::connect(reply, SIGNAL(finished()), &eventLoop, SLOT(quit()));
        eventLoop.exec();

        QString filename = dir.absoluteFilePath(profile + ".brf");
        QFile file(filename);
        file.open(QIODevice::WriteOnly);
        file.write(reply->readAll());
        file.close();
    }
}
