/**********************************************************************************************
    Copyright (C) 2017 Norbert Truchsess <norbert.truchsess@t-online.de>

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

#include "gis/rte/router/brouter/CRouterBRouterSetup.h"

#include "helpers/CSettings.h"
#include "setup/IAppSetup.h"
#include <QJSEngine>
#include <QMessageBox>
#include <QNetworkReply>
#include <QWebEnginePage>

CRouterBRouterSetup::CRouterBRouterSetup(QObject *parent)
    : QObject(parent)
{
    networkAccessManager = new QNetworkAccessManager(this);
    profilesWebPage = new QWebEnginePage(this);
    connect(networkAccessManager, &QNetworkAccessManager::finished, this, &CRouterBRouterSetup::slotOnlineRequestFinished);
    connect(profilesWebPage, &QWebEnginePage::loadFinished, this, &CRouterBRouterSetup::slotLoadOnlineProfilesRequestFinished);
}

CRouterBRouterSetup::~CRouterBRouterSetup()
{
}

void CRouterBRouterSetup::load()
{
    SETTINGS;
    cfg.beginGroup("Route/brouter");
    installMode = modeFromString(cfg.value("installMode", stringFromMode(defaultInstallMode)).toString());
    expertMode = cfg.value("expertMode", defaultExpertMode).toBool();
    expertConfigUrl = cfg.value("expertConfigUrl", defaultConfigUrl).toString();
    onlineServiceUrl = cfg.value("onlineServiceUrl", defaultOnlineServiceUrl).toString();
    onlineProfilesUrl = cfg.value("onlineProfilesUrl", defaultOnlineProfilesUrl).toString();
    localDir = cfg.value("localDir", defaultLocalDir).toString();
    localJavaExecutable = cfg.value("localJava", findJava()).toString();
    localProfileDir = cfg.value("localProfileDir", defaultLocalProfileDir).toString();
    localCustomProfileDir = cfg.value("localCustomProfileDir", defaultLocalCustomProfileDir).toString();
    localSegmentsDir = cfg.value("localSegmentsDir", defaultLocalSegmentsDir).toString();
    localHost = cfg.value("localHost", defaultLocalHost).toString();
    localPort = cfg.value("localPort", defaultLocalPort).toString();
    localBindLocalonly = cfg.value("localBindLocalonly", defaultLocalBindLocalonly).toBool();
    localNumberThreads = cfg.value("localNumberThreads", defaultLocalNumberThreads).toString();
    localMaxRunningTime = cfg.value("localMaxRunningTime", defaultLocalMaxRunningTime).toString();
    localJavaOpts = cfg.value("localJavaOpts", defaultLocalJavaOpts).toString();
    expertBinariesUrl = cfg.value("expertBinariesUrl", defaultBinariesUrl).toString();
    expertSegmentsUrl = cfg.value("expertSegmentsUrl", defaultSegmentsUrl).toString();
    onlineProfiles.clear();
    int size = cfg.beginReadArray("online");
    for (int i = 0; i < size; i++)
    {
        cfg.setArrayIndex(i);
        onlineProfiles << cfg.value("profile").toString();
    }
    cfg.endArray();
    localProfiles.clear();
    size = cfg.beginReadArray("local");
    for (int i = 0; i < size; i++)
    {
        cfg.setArrayIndex(i);
        localProfiles << cfg.value("profile").toString();
    }
    cfg.endArray();
    cfg.endGroup();

    if (installMode == eModeLocal)
    {
        readLocalProfiles();
    }
    else if (installMode == eModeOnline)
    {
        if (!expertMode)
        {
            loadOnlineConfig(true);
        }
    }
    else
    {
        onInvalidSetup();
    }
}

void CRouterBRouterSetup::save()
{
    SETTINGS;
    cfg.beginGroup("Route/brouter");
    cfg.setValue("expertMode", expertMode);
    cfg.setValue("installMode", stringFromMode(installMode));
    cfg.setValue("expertConfigUrl", expertConfigUrl);
    cfg.setValue("onlineServiceUrl", onlineServiceUrl);
    cfg.setValue("onlineProfilesUrl", onlineProfilesUrl);
    cfg.setValue("localDir", localDir);
    cfg.setValue("localJava", localJavaExecutable);
    cfg.setValue("localProfileDir", localProfileDir);
    cfg.setValue("localCustomProfileDir", localCustomProfileDir);
    cfg.setValue("localSegmentsDir", localSegmentsDir);
    cfg.setValue("localHost", localHost);
    cfg.setValue("localPort", localPort);
    cfg.setValue("localBindLocalonly", localBindLocalonly);
    cfg.setValue("localNumberThreads", localNumberThreads);
    cfg.setValue("localMaxRunningTime", localMaxRunningTime);
    cfg.setValue("localJavaOpts", localJavaOpts);
    cfg.setValue("expertBinariesUrl", expertBinariesUrl);
    cfg.setValue("expertSegmentsUrl", expertSegmentsUrl);
    cfg.beginWriteArray("online");
    for (int i = 0; i < onlineProfiles.size(); i++)
    {
        cfg.setArrayIndex(i);
        cfg.setValue("profile", onlineProfiles.at(i));
    }
    cfg.endArray();
    cfg.beginWriteArray("local");
    for (int i = 0; i < localProfiles.size(); i++)
    {
        cfg.setArrayIndex(i);
        cfg.setValue("profile", localProfiles.at(i));
    }
    cfg.endArray();
    cfg.endGroup();
}

void CRouterBRouterSetup::resetAll()
{
    resetInstallMode();
    resetOnlineConfigUrl();
    resetOnlineServiceUrl();
    resetOnlineProfilesUrl();
    resetLocalProfileDir();
    resetLocalCustomProfileDir();
    resetLocalSegmentsDir();
    resetLocalHost();
    resetLocalPort();
    resetLocalBindLocalonly();
    resetLocalNumberThreads();
    resetLocalMaxRunningTime();
    resetLocalJavaOpts();
    resetBinariesUrl();
    resetSegmentsUrl();
}

CRouterBRouterSetup::mode_e CRouterBRouterSetup::modeFromString(const QString& mode) const
{
    if (mode == "online")
    {
        return eModeOnline;
    }
    else if (mode == "local")
    {
        return eModeLocal;
    }
    else
    {
        return eModeIllegal;
    }
}

QString CRouterBRouterSetup::stringFromMode(const mode_e mode) const
{
    if (mode == eModeLocal)
    {
        return "local";
    }
    else
    {
        Q_ASSERT(mode == eModeOnline);
        return "online";
    }
}

void CRouterBRouterSetup::addProfile(const QString &profile)
{
    if (installMode == eModeLocal)
    {
        loadOnlineProfileAsync(profile, eProfileInstall);
    }
    else
    {
        Q_ASSERT(installMode == eModeOnline);
        if (!onlineProfiles.contains(profile))
        {
            onlineProfiles << profile;
            emit sigProfilesChanged();
        }
    }
}

void CRouterBRouterSetup::deleteProfile(const QString &profile)
{
    if (installMode == eModeLocal)
    {
        const QString &filename = getProfileDir(eModeLocal).absoluteFilePath(profile + ".brf");
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
    else
    {
        Q_ASSERT(installMode == eModeOnline);
        onlineProfiles.removeAt(onlineProfiles.indexOf(profile));
        emit sigProfilesChanged();
    }
}

void CRouterBRouterSetup::profileUp(const QString &profile)
{
    if (installMode == eModeLocal)
    {
        int index = localProfiles.indexOf(profile);
        if (index > 0)
        {
            localProfiles.removeAt(index);
            localProfiles.insert(index - 1, profile);
            emit sigProfilesChanged();
        }
    }
    else
    {
        Q_ASSERT(installMode == eModeOnline);
        int index = onlineProfiles.indexOf(profile);
        if (index > 0)
        {
            onlineProfiles.removeAt(index);
            onlineProfiles.insert(index - 1, profile);
            emit sigProfilesChanged();
        }
    }
}

void CRouterBRouterSetup::profileDown(const QString &profile)
{
    if (installMode == eModeLocal)
    {
        int index = localProfiles.indexOf(profile);
        if (index > -1 && index < localProfiles.size() - 1)
        {
            localProfiles.removeAt(index);
            localProfiles.insert(index + 1, profile);
            emit sigProfilesChanged();
        }
    }
    else
    {
        Q_ASSERT(installMode == eModeOnline);
        int index = onlineProfiles.indexOf(profile);
        if (index > -1 && index < onlineProfiles.size() - 1)
        {
            onlineProfiles.removeAt(index);
            onlineProfiles.insert(index + 1, profile);
            emit sigProfilesChanged();
        }
    }
}

void CRouterBRouterSetup::readLocalProfiles()
{
    bool changed(false);
    const QDir& dir = getProfileDir(eModeLocal);
    QStringList installedProfiles;
    if (dir.exists())
    {
        const QStringList& profiles = dir.entryList();
        for(const QString &profile : profiles)
        {
            if (profile.endsWith(".brf"))
            {
                installedProfiles << profile.left(profile.length() - 4);
            }
        }
    }
    const QStringList localProfilesTmp(localProfiles);
    for (const QString &profile : localProfilesTmp)
    {
        if (!installedProfiles.contains(profile))
        {
            localProfiles.removeAt(localProfiles.indexOf(profile));
            changed = true;
        }
    }
    for (const QString &profile : qAsConst(installedProfiles))
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
    if (mode == eModeLocal)
    {
        return QDir(QDir(localDir).absoluteFilePath(localProfileDir));
    }
    else
    {
        Q_ASSERT(mode == eModeOnline);
        QDir brouterDir(QDir(IAppSetup::getPlatformInstance()->defaultCachePath()).absoluteFilePath(onlineCacheDir));
        if (!brouterDir.exists())
        {
            brouterDir.mkpath(brouterDir.absolutePath());
        }
        return brouterDir;
    }
}

QStringList CRouterBRouterSetup::getProfiles() const
{
    if (installMode == eModeLocal)
    {
        return localProfiles;
    }
    else
    {
        Q_ASSERT(installMode == eModeOnline);
        return onlineProfiles;
    }
}

void CRouterBRouterSetup::loadLocalOnlineProfiles(bool update) const
{
    profilesWebPage->setProperty("update", update);
    profilesWebPage->load(QUrl(onlineProfilesUrl));
}

void CRouterBRouterSetup::loadOnlineConfig(bool update) const
{
    const QUrl configUrl(getConfigUrl());
    const QString &configHost = configUrl.host();
    const QString &configScheme = configUrl.scheme();

    QNetworkReply * reply = networkAccessManager->get(QNetworkRequest(configUrl));
    reply->setProperty("configHost", configHost);
    reply->setProperty("configScheme", configScheme);
    reply->setProperty("type", eTypeConfig);
    reply->setProperty("update", update);
}

void CRouterBRouterSetup::loadOnlineVersion() const
{
    QUrl serviceUrl = getServiceUrl();
    serviceUrl.setQuery(QUrlQuery("lonlats=11.138570,48.676961|11.138570,48.676961&profile=shortest&alternativeidx=0&format=gpx"));
    QNetworkReply * reply = networkAccessManager->get(QNetworkRequest(serviceUrl));
    reply->setProperty("type", eTypeVersion);
}

void CRouterBRouterSetup::loadExpertBinariesPage() const
{
    QNetworkReply * reply = networkAccessManager->get(QNetworkRequest(expertBinariesUrl));
    reply->setProperty("type", eTypeBinariesPage);
}

void CRouterBRouterSetup::loadExpertSegmentsPage() const
{
    QNetworkReply * reply = networkAccessManager->get(QNetworkRequest(expertSegmentsUrl));
    reply->setProperty("type", eTypeSegmentsPage);
}

void CRouterBRouterSetup::slotOnlineRequestFinished(QNetworkReply *reply)
{
    reply->deleteLater();
    const request_e type = request_e(reply->property("type").toInt());

    if (reply->error() != QNetworkReply::NoError && type != eTypeVersion)
    {
        emitNetworkError(reply->errorString());
        return;
    }

    switch (type)
    {
    case eTypeConfig:
    {
        loadOnlineConfigFinished(reply);
        break;
    }

    case eTypeProfile:
    {
        loadOnlineProfileFinished(reply);
        break;
    }

    case eTypeVersion:
    {
        loadOnlineVersionFinished(reply);
        break;
    }

    case eTypeBinariesPage:
    {
        emit sigBinariesPageLoaded();
        break;
    }

    case eTypeSegmentsPage:
    {
        emit sigSegmentsPageLoaded();
        break;
    }
    }
}

void CRouterBRouterSetup::loadOnlineConfigFinished(QNetworkReply *reply)
{
    const QString &configHost = reply->property("configHost").toString();
    const QString &configScheme = reply->property("configScheme").toString();
    const bool update = reply->property("update").toBool();

    const QString jsConfig(reply->readAll());

    QJSEngine engine;

    const QString &jsSetup = QString( \
        "(function(){\
                   window = {};\
                   window.location = {};\
                   window.location.hostname = '%1';\
                   window.location.protocol = '%2:';\
                   window.location.search = {};\
                   window.location.search.slice = function() {};\
                   URLSearchParams = function() {};\
                   BR = {};\
                  })();").arg(configHost, configScheme);

    engine.evaluate(jsSetup);
    const QJSValue &val = engine.evaluate(jsConfig);
    if (val.isError())
    {
        emitOnlineConfigScriptError(val);
        return;
    }

    const QJSValue &br = engine.globalObject().property("BR");
    if (!br.isObject() || br.isError())
    {
        emitOnlineConfigScriptError(br);
        return;
    }
    const QJSValue &conf = br.property("conf");
    if (!conf.isObject() || conf.isError())
    {
        emitOnlineConfigScriptError(conf);
        return;
    }

    const QJSValue &host = conf.property("host");
    if (!host.isString() || host.isError())
    {
        emitOnlineConfigScriptError(host);
        return;
    }
    if (update && onlineServiceUrl != host.toString())
    {
        onlineServiceUrl = host.toString();
    }
    const QJSValue &url = conf.property("profilesUrl");
    if (!url.isString() || url.isError())
    {
        emitOnlineConfigScriptError(url);
        return;
    }
    if (update && onlineProfilesUrl != url.toString())
    {
        onlineProfilesUrl = url.toString();
    }

    if (update && !expertMode)
    {
        if  (installMode == eModeOnline)
        {
            const QJSValue &profiles = conf.property("profiles");
            if (!profiles.isArray() || profiles.isError())
            {
                emitOnlineConfigScriptError(profiles);
                return;
            }
            const qint32 len = profiles.property("length").toInt();

            QStringList onlineProfilesLoaded;
            for(qint32 i = 0; i < len; i++)
            {
                const QJSValue &profile = profiles.property(i);
                if (!profile.isString() || profile.isError())
                {
                    emitOnlineConfigScriptError(profile);
                    return;
                }
                onlineProfilesLoaded << profile.toString();
            }

            mergeOnlineProfiles(onlineProfilesLoaded);

            const QStringList onlineProfilesTmp(onlineProfiles);
            for (const QString &profile : onlineProfilesTmp)
            {
                if (!onlineProfilesAvailable.contains(profile))
                {
                    onlineProfiles.removeAt(onlineProfiles.indexOf(profile));
                }
            }

            emit sigProfilesChanged();
        }
        else
        {
            loadLocalOnlineProfiles(true);
        }
    }
    emit sigOnlineConfigLoaded();
}

void CRouterBRouterSetup::loadOnlineVersionFinished(QNetworkReply *reply)
{
    versionMajor = NOINT;
    versionMinor = NOINT;
    versionPatch = NOINT;

    if (reply->error() != QNetworkReply::NoError)
    {
        emitNetworkError(reply->errorString());
        return;
    }
    const QString gpx(reply->readAll());
    const QRegExp reVersion = QRegExp("^<\\?xml.+<gpx.+creator=\"(.*)\"");

    if (reVersion.indexIn(gpx) > -1)
    {
        parseBRouterVersion(reVersion.cap(1));
        return;
    }
    emit sigError("invalid reply", "response is not brouter-gpx");
}

void CRouterBRouterSetup::parseBRouterVersion(const QString &text)
{
    // version string is either like "BRouter 1.4.9 / 24092017"
    // or (without the date) like "BRouter-1.4.9"
    QRegExp reVersion("\\bBRouter[- ](\\d+)\\.(\\d+)\\.(\\d+)\\b");
    if (reVersion.indexIn(text) > -1)
    {
        bool ok;
        versionMajor = reVersion.cap(1).toInt(&ok);
        if (ok)
        {
            versionMinor = reVersion.cap(2).toInt(&ok);
        }
        if (ok)
        {
            versionPatch = reVersion.cap(3).toInt(&ok);
        }
        if (ok)
        {
            emit sigVersionChanged();
            return;
        }
    }
    versionMajor = NOINT;
    versionMinor = NOINT;
    versionPatch = NOINT;

    emit sigVersionChanged();
}


void CRouterBRouterSetup::slotLoadOnlineProfilesRequestFinished(bool ok)
{
    if (!ok)
    {
        emitNetworkError(tr("%1 not accessible").arg(onlineProfilesUrl));
        return;
    }
    else
    {
        profilesWebPage->runJavaScript(
            "var profiles = [];"
            "var xpathResult = document.evaluate('.//@href',document.body,null,XPathResult.UNORDERED_NODE_ITERATOR_TYPE,null);"
            "var href = xpathResult.iterateNext();"
            "while(href) {"
            "  var pmatch = href.value.match(/(\\S+)\\.brf/);"
            "  if (pmatch != null) {"
            "    profiles.push(pmatch[1]);"
            "  }"
            "  href = xpathResult.iterateNext();"
            "}"
            "profiles;",
            [this](const QVariant& v) { afterSlotLoadOnlineProfilesRequestFinishedRunJavascript(v); }
            );
    }
}

void CRouterBRouterSetup::afterSlotLoadOnlineProfilesRequestFinishedRunJavascript(const QVariant &v)
{
    QStringList onlineProfilesLoaded = v.toStringList();

    if (onlineProfilesLoaded.size() == 0)
    {
        emitNetworkError(tr("%1 invalid result").arg(onlineProfilesUrl));
    }
    else
    {
        if (profilesWebPage->property("update").toBool())
        {
            mergeOnlineProfiles(onlineProfilesLoaded);
        }
        emit sigProfilesChanged();
    }
}

void CRouterBRouterSetup::mergeOnlineProfiles(const QStringList &onlineProfilesLoaded)
{
    const QStringList onlineProfilesAvailableTmp(onlineProfilesAvailable);
    for (const QString &profile : onlineProfilesAvailableTmp)
    {
        if (!onlineProfilesLoaded.contains(profile))
        {
            onlineProfilesAvailable.removeAt(onlineProfilesAvailable.indexOf(profile));
        }
    }
    for (const QString &profile : onlineProfilesLoaded)
    {
        if (!onlineProfilesAvailable.contains(profile))
        {
            onlineProfilesAvailable << profile;
        }
    }
}

void CRouterBRouterSetup::emitOnlineConfigScriptError(const QJSValue &error)
{
    emit sigError(tr("Error parsing online-config:"), error.toString());
}

void CRouterBRouterSetup::emitNetworkError(QString error)
{
    emit sigError(tr("Network error:"), error);
}

void CRouterBRouterSetup::displayProfileAsync(const QString &profile)
{
    if (installMode == eModeLocal)
    {
        QFile file(getProfileDir(eModeLocal).absoluteFilePath(profile + ".brf"));
        if (file.exists())
        {
            file.open(QIODevice::ReadOnly);
            const QByteArray &content = file.readAll();
            file.close();
            emit sigDisplayOnlineProfileFinished(profile, QString(content));
        }
    }
    else
    {
        Q_ASSERT(installMode == eModeOnline);
        loadOnlineProfileAsync(profile, eProfileDisplay);
    }
}

void CRouterBRouterSetup::displayOnlineProfileAsync(const QString &profile) const
{
    loadOnlineProfileAsync(profile, eProfileDisplay);
}

void CRouterBRouterSetup::loadOnlineProfileAsync(const QString &profile, const profileRequest_e mode) const
{
    QUrl url(onlineProfilesUrl);
    QString path = url.path();
    if (!path.endsWith('/'))
    {
        path.append("/");
    }
    path.append(profile).append(".brf");
    url.setPath(path);
    QNetworkReply * reply = networkAccessManager->get(QNetworkRequest(url));
    reply->setProperty("type", eTypeProfile);
    reply->setProperty("profile", profile);
    reply->setProperty("request", mode);
}

void CRouterBRouterSetup::loadOnlineProfileFinished(QNetworkReply * reply)
{
    const QString &profile = reply->property("profile").toString();
    const profileRequest_e mode = profileRequest_e(reply->property("request").toInt());

    const QByteArray &content = reply->readAll();
    if (mode == eProfileInstall)
    {
        const QDir dir = getProfileDir(eModeLocal);
        const QString filename = dir.absoluteFilePath(profile + ".brf");
        QFile file(filename);
        file.open(QIODevice::WriteOnly);
        file.write(content);
        file.close();
        readLocalProfiles();
    }
    else
    {
        Q_ASSERT(mode == eProfileDisplay);
        emit sigDisplayOnlineProfileFinished(profile, QString(content));
    }
}

bool CRouterBRouterSetup::isLocalBRouterInstalled() const
{
    const QDir dir(localDir);
    return QFile(dir.absoluteFilePath("brouter.jar")).exists() && QDir(dir.absoluteFilePath(localProfileDir)).exists();
}

bool CRouterBRouterSetup::isLocalBRouterDefaultDir() const
{
    return localDir == defaultLocalDir;
}


QString CRouterBRouterSetup::findJava() const
{
    return IAppSetup::getPlatformInstance()->findExecutable("java");
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

QUrl CRouterBRouterSetup::getServiceUrl() const
{
    if (installMode == CRouterBRouterSetup::eModeLocal)
    {
        QUrl url(QString("http://"));
        url.setHost(localHost);
        url.setPort(localPort.toInt());
        url.setPath("/brouter");
        return url;
    }
    else
    {
        Q_ASSERT(installMode == CRouterBRouterSetup::eModeOnline);
        QUrl url(onlineServiceUrl);
        if (url.path() == "")
        {
            url.setPath("/brouter");
        }
        return url;
    }
}

QString CRouterBRouterSetup::getSegmentsUrl() const
{
    return expertMode ? expertSegmentsUrl : defaultSegmentsUrl;
}

QString CRouterBRouterSetup::getBinariesUrl() const
{
    return expertMode ? expertBinariesUrl : defaultBinariesUrl;
}

QString CRouterBRouterSetup::getConfigUrl() const
{
    return expertMode ? expertConfigUrl : defaultConfigUrl;
}
