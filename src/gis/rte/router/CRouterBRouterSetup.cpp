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
