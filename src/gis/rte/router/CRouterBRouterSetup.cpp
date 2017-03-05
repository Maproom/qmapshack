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
#include "setup/IAppSetup.h"
#include <QtScript>
#include <QtWebKit>
#include <QWebPage>
#include <QWebFrame>
#include <QMessageBox>

CRouterBRouterSetup::CRouterBRouterSetup(QObject *parent)
    :QObject(parent)
{
    networkAccessManager = new QNetworkAccessManager(this);
    connect(networkAccessManager, &QNetworkAccessManager::finished, this, &CRouterBRouterSetup::slotOnlineRequestFinished);
}

CRouterBRouterSetup::~CRouterBRouterSetup()
{
}

void CRouterBRouterSetup::load()
{
    SETTINGS;
    cfg.beginGroup("Route/brouter");
    installMode = modeFromString(cfg.value("installMode",stringFromMode(defaultInstallMode)).toString());
    expertMode = cfg.value("expertMode",defaultExpertMode).toBool();
    onlineWebUrl = cfg.value("onlineWebUrl", defaultOnlineWebUrl).toString();
    onlineServiceUrl = cfg.value("onlineServiceUrl", defaultOnlineServiceUrl).toString();
    onlineProfilesUrl = cfg.value("onlineProfilesUrl", defaultOnlineProfilesUrl).toString();
    localDir = cfg.value("localDir", defaultLocalDir).toString();
    localProfileDir = cfg.value("localProfileDir", defaultLocalProfileDir).toString();
    localCustomProfileDir = cfg.value("localCustomProfileDir", defaultLocalCustomProfileDir).toString();
    localSegmentsDir = cfg.value("localSegmentsDir", defaultLocalSegmentsDir).toString();
    localHost = cfg.value("localHost", defaultLocalHost).toString();
    localPort = cfg.value("localPort", defaultLocalPort).toString();
    localNumberThreads = cfg.value("localNumberThreads", defaultLocalNumberThreads).toString();
    localMaxRunningTime = cfg.value("localMaxRunningTime", defaultLocalMaxRunningTime).toString();
    localJavaOpts = cfg.value("localJavaOpts", defaultLocalJavaOpts).toString();
    binariesUrl = cfg.value("binariesUrl",defaultBinariesUrl).toString();
    segmentsUrl = cfg.value("segmentsUrl",defaultSegmentsUrl).toString();
    onlineProfiles.clear();
    int size = cfg.beginReadArray("online");
    for (int i=0; i < size; i++)
    {
        cfg.setArrayIndex(i);
        onlineProfiles << cfg.value("profile").toString();
    }
    cfg.endArray();
    localProfiles.clear();
    size = cfg.beginReadArray("local");
    for (int i=0; i < size; i++)
    {
        cfg.setArrayIndex(i);
        localProfiles << cfg.value("profile").toString();
    }
    cfg.endArray();
    cfg.endGroup();

    if (installMode == ModeLocal)
    {
        updateLocalProfiles();
    }
    else if (installMode == ModeOnline)
    {
        loadOnlineConfig();
    }
    else
    {
        throw CRouterBRouterSetupException();
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
    cfg.setValue("localCustomProfileDir", localCustomProfileDir);
    cfg.setValue("localSegmentsDir", localSegmentsDir);
    cfg.setValue("localHost", localHost);
    cfg.setValue("localPort", localPort);
    cfg.setValue("localNumberThreads", localNumberThreads);
    cfg.setValue("localMaxRunningTime", localMaxRunningTime);
    cfg.setValue("localJavaOpts", localJavaOpts);
    cfg.setValue("binariesUrl", binariesUrl);
    cfg.setValue("segmentsUrl", segmentsUrl);
    cfg.beginWriteArray("online");
    for (int i=0; i < onlineProfiles.size(); i++)
    {
        cfg.setArrayIndex(i);
        cfg.setValue("profile", onlineProfiles.at(i));
    }
    cfg.endArray();
    cfg.beginWriteArray("local");
    for (int i=0; i < localProfiles.size(); i++)
    {
        cfg.setArrayIndex(i);
        cfg.setValue("profile",localProfiles.at(i));
    }
    cfg.endArray();
    cfg.endGroup();
}

void CRouterBRouterSetup::resetAll()
{
    resetOnlineWebUrl();
    resetOnlineServiceUrl();
    resetOnlineProfilesUrl();
    resetLocalProfileDir();
    resetLocalCustomProfileDir();
    resetLocalSegmentsDir();
    resetLocalHost();
    resetLocalPort();
    resetLocalNumberThreads();
    resetLocalMaxRunningTime();
    resetLocalJavaOpts();
    resetBinariesUrl();
    resetSegmentsUrl();
}

void CRouterBRouterSetup::resetOnlineWebUrl()
{
    onlineWebUrl = defaultOnlineWebUrl;
}

void CRouterBRouterSetup::resetOnlineServiceUrl()
{
    onlineServiceUrl = defaultOnlineServiceUrl;
}

void CRouterBRouterSetup::resetOnlineProfilesUrl()
{
    onlineProfilesUrl = defaultOnlineProfilesUrl;
}

void CRouterBRouterSetup::resetLocalProfileDir()
{
    localProfileDir = defaultLocalProfileDir;
}

void CRouterBRouterSetup::resetLocalCustomProfileDir()
{
    localCustomProfileDir = defaultLocalCustomProfileDir;
}

void CRouterBRouterSetup::resetLocalSegmentsDir()
{
    localSegmentsDir = defaultLocalSegmentsDir;
}

void CRouterBRouterSetup::resetLocalHost()
{
    localHost = defaultLocalHost;
}

void CRouterBRouterSetup::resetLocalPort()
{
    localPort = defaultLocalPort;
}

void CRouterBRouterSetup::resetLocalNumberThreads()
{
    localNumberThreads = defaultLocalNumberThreads;
}

void CRouterBRouterSetup::resetLocalMaxRunningTime()
{
    localMaxRunningTime = defaultLocalMaxRunningTime;
}

void CRouterBRouterSetup::resetLocalJavaOpts()
{
    localJavaOpts = defaultLocalJavaOpts;
}

void CRouterBRouterSetup::resetBinariesUrl()
{
    binariesUrl = defaultBinariesUrl;
}

void CRouterBRouterSetup::resetSegmentsUrl()
{
    segmentsUrl = defaultSegmentsUrl;
}

CRouterBRouterSetup::mode_e CRouterBRouterSetup::modeFromString(const QString mode) const
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

QString CRouterBRouterSetup::stringFromMode(const mode_e mode) const
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

void CRouterBRouterSetup::addProfile(const QString profile)
{
    if (installMode == ModeLocal)
    {
        loadOnlineProfileAsync(profile, ProfileInstall);
    }
    else if (installMode == ModeOnline)
    {
        if (!onlineProfiles.contains(profile))
        {
            onlineProfiles << profile;
            emit sigProfilesChanged();
        }
    }
    else
    {
        throw CRouterBRouterSetupException();
    }
}

void CRouterBRouterSetup::deleteProfile(const QString profile)
{
    if (installMode == ModeLocal)
    {
        const QString filename = getProfileDir(ModeLocal).absoluteFilePath(profile + ".brf");
        QFile file(filename);
        if (file.exists())
        {
            file.remove();
        }
        if (localProfiles.contains(profile))
        {
            localProfiles.removeAt(localProfiles.indexOf(profile));
            emit sigProfilesChanged();
        }
    }
    else if (installMode == ModeOnline)
    {
        onlineProfiles.removeAt(onlineProfiles.indexOf(profile));
        emit sigProfilesChanged();
    }
    else
    {
        throw CRouterBRouterSetupException();
    }
}

void CRouterBRouterSetup::profileUp(const QString profile)
{
    if (installMode == ModeLocal)
    {
        int index = localProfiles.indexOf(profile);
        if (index > 0)
        {
            localProfiles.removeAt(index);
            localProfiles.insert(index-1,profile);
            emit sigProfilesChanged();
        }
    }
    else if (installMode == ModeOnline)
    {
        int index = onlineProfiles.indexOf(profile);
        if (index > 0)
        {
            onlineProfiles.removeAt(index);
            onlineProfiles.insert(index-1,profile);
            emit sigProfilesChanged();
        }
    }
    else
    {
        throw CRouterBRouterSetupException();
    }
}

void CRouterBRouterSetup::profileDown(const QString profile)
{
    if (installMode == ModeLocal)
    {
        int index = localProfiles.indexOf(profile);
        if (index > -1 and index < localProfiles.size()-1)
        {
            localProfiles.removeAt(index);
            localProfiles.insert(index+1,profile);
            emit sigProfilesChanged();
        }
    }
    else if (installMode == ModeOnline)
    {
        int index = onlineProfiles.indexOf(profile);
        if (index > -1 and index < onlineProfiles.size()-1)
        {
            onlineProfiles.removeAt(index);
            onlineProfiles.insert(index+1,profile);
            emit sigProfilesChanged();
        }
    }
    else
    {
        throw CRouterBRouterSetupException();
    }
}

void CRouterBRouterSetup::updateLocalProfiles()
{
    bool changed(false);
    const QDir dir = getProfileDir(ModeLocal);
    QStringList installedProfiles;
    if (dir.exists())
    {
        for(const QString profile : dir.entryList())
        {
            if (profile.endsWith(".brf"))
            {
                installedProfiles << profile.left(profile.length()-4);
            }
        }
    }
    for (const QString profile : localProfiles)
    {
        if (!installedProfiles.contains(profile))
        {
            localProfiles.removeAt(localProfiles.indexOf(profile));
            changed = true;
        }
    }
    for (const QString profile : installedProfiles)
    {
        if (!localProfiles.contains(profile))
        {
            localProfiles << profile;
            changed = true;
        }
    }
    if (changed)
    {
        emit sigProfilesChanged();
    }
}

QDir CRouterBRouterSetup::getProfileDir(const mode_e mode) const
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

QStringList CRouterBRouterSetup::getProfiles() const
{
    if (installMode == ModeLocal)
    {
        return localProfiles;
    }
    else if (installMode == ModeOnline)
    {
        return onlineProfiles;
    }
    else
    {
        throw CRouterBRouterSetupException();
    }
}

void CRouterBRouterSetup::loadOnlineConfig()
{
    const QUrl configUrl = QUrl(onlineWebUrl + "config.js");
    const QString configHost = configUrl.host();

    QNetworkReply * reply = networkAccessManager->get(QNetworkRequest(configUrl));
    reply->setProperty("configHost",configHost);
    reply->setProperty("type",type_config);
}

void CRouterBRouterSetup::slotOnlineRequestFinished(QNetworkReply *reply)
{
    request_e type = request_e(reply->property("type").toInt());
    switch(type)
    {
    case type_config:
    {
        loadOnlineConfigFinished(reply);
        break;
    }
    case type_profile:
    {
        loadOnlineProfileFinished(reply);
        break;
    }
    default:
    {
        reply->deleteLater();
    }
    }
}

void CRouterBRouterSetup::loadOnlineConfigFinished(QNetworkReply *reply)
{
    reply->deleteLater();
    QString configHost = reply->property("configHost").toString();

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

    bool changed(false);
    const QString host = conf.property("host").toString();
    if (onlineServiceUrl != host)
    {
        onlineServiceUrl = host;
        changed = true;
    }
    const QString url = conf.property("profilesUrl").toString();
    if (onlineProfilesUrl != url)
    {
        onlineProfilesUrl = url;
        changed = true;
    }
    if (changed)
    {
        emit sigOnlineConfigChanged();
    }

    changed = false;

    const QScriptValue profiles = conf.property("profiles");
    const qint32 len = profiles.property("length").toInt32();

    QStringList onlineProfilesLoaded;
    for(qint32 i=0;i<len;i++)
    {
        onlineProfilesLoaded << profiles.property(i).toString();
    }
    for (QString profile : onlineProfilesAvailable)
    {
        if (!onlineProfilesLoaded.contains(profile))
        {
            onlineProfilesAvailable.removeAt(onlineProfilesAvailable.indexOf(profile));
            changed = true;
        }
    }
    for (QString profile : onlineProfilesLoaded)
    {
        if (!onlineProfilesAvailable.contains(profile))
        {
            onlineProfilesAvailable << profile;
            changed = true;
        }
    }

    for (const QString profile : onlineProfiles)
    {
        if (!onlineProfilesAvailable.contains(profile))
        {
            onlineProfiles.removeAt(onlineProfiles.indexOf(profile));
            changed = true;
        }
    }

    if (changed)
    {
        emit sigProfilesChanged();
    }
}

void CRouterBRouterSetup::displayProfileAsync(const QString profile)
{
    if (installMode == ModeLocal)
    {
        QFile file(getProfileDir(ModeLocal).absoluteFilePath(profile + ".brf"));
        if (file.exists())
        {
            file.open(QIODevice::ReadOnly);
            const QByteArray content = file.readAll();
            file.close();
            emit sigDisplayOnlineProfileFinished(profile, QString(content));
        }
    }
    else if (installMode == ModeOnline)
    {
        loadOnlineProfileAsync(profile, ProfileDisplay);
    }
    else
    {
        throw CRouterBRouterSetupException();
    }
}

void CRouterBRouterSetup::displayOnlineProfileAsync(const QString profile) const
{
    loadOnlineProfileAsync(profile, ProfileDisplay);
}

void CRouterBRouterSetup::loadOnlineProfileAsync(const QString profile, const profileRequest_e mode) const
{
    QNetworkReply * reply = networkAccessManager->get(QNetworkRequest(QUrl(onlineProfilesUrl + profile +".brf")));
    reply->setProperty("type",type_profile);
    reply->setProperty("profile", profile);
    reply->setProperty("request", mode);
}

void CRouterBRouterSetup::loadOnlineProfileFinished(QNetworkReply * reply)
{
    reply->deleteLater();
    const QString profile = reply->property("profile").toString();
    const profileRequest_e mode = profileRequest_e(reply->property("request").toInt());

    const QByteArray content = reply->readAll();
    if (mode == ProfileInstall)
    {
        const QDir dir = getProfileDir(ModeLocal);
        const QString filename = dir.absoluteFilePath(profile + ".brf");
        QFile file(filename);
        file.open(QIODevice::WriteOnly);
        file.write(content);
        file.close();
        updateLocalProfiles();
    }
    else if (mode == ProfileDisplay)
    {
        emit sigDisplayOnlineProfileFinished(profile, QString(content));
    }
}

bool CRouterBRouterSetup::isLocalBRouterInstalled() const
{
    const QDir dir(localDir);
    const QString brouterJarPath = dir.absoluteFilePath("brouter.jar");
    return (QFile(brouterJarPath).exists() and QDir(dir.absoluteFilePath(localProfileDir)).exists());
}

void CRouterBRouterSetup::onInvalidSetup()
{
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setText("BRouter config is inconsistent!");
    msgBox.setInformativeText("Resetting to default values");
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.exec();
    resetAll();
}
