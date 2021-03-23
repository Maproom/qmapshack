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

#ifndef CROUTERBROUTERSETUP_H
#define CROUTERBROUTERSETUP_H

#include "units/IUnit.h"
#include <QtCore>

class QJSValue;
class QNetworkAccessManager;
class QNetworkReply;
class QWebEnginePage;

class CRouterBRouterSetup : public QObject
{
    Q_OBJECT
public:
    CRouterBRouterSetup(QObject * parent);
    ~CRouterBRouterSetup();

    enum mode_e { eModeLocal, eModeOnline, eModeIllegal };
    struct tile_t { QPoint tile; QDateTime date; qreal size; };

    void load();
    void save();

    void resetAll();
    void resetInstallMode() { installMode = defaultInstallMode; }
    void resetOnlineConfigUrl() { expertConfigUrl = defaultConfigUrl; }
    void resetOnlineServiceUrl() { onlineServiceUrl = defaultOnlineServiceUrl; }
    void resetOnlineProfilesUrl() { onlineProfilesUrl = defaultOnlineProfilesUrl; }
    void resetLocalProfileDir() { localProfileDir = defaultLocalProfileDir; }
    void resetLocalCustomProfileDir() { localCustomProfileDir = defaultLocalCustomProfileDir; }
    void resetLocalSegmentsDir() { localSegmentsDir = defaultLocalSegmentsDir; }
    void resetLocalHost() { localHost = defaultLocalHost; }
    void resetLocalPort() { localPort = defaultLocalPort; }
    void resetLocalBindLocalonly() { localBindLocalonly = defaultLocalBindLocalonly; }
    void resetLocalNumberThreads() { localNumberThreads = defaultLocalNumberThreads; }
    void resetLocalMaxRunningTime() { localMaxRunningTime = defaultLocalMaxRunningTime; }
    void resetLocalJavaOpts() { localJavaOpts = defaultLocalJavaOpts; }
    void resetBinariesUrl() { expertBinariesUrl = defaultBinariesUrl; }
    void resetSegmentsUrl() { expertSegmentsUrl = defaultSegmentsUrl; }

    QStringList getProfiles() const;

    void addProfile(const QString &profile);
    void deleteProfile(const QString &profile);
    void profileUp(const QString &profile);
    void profileDown(const QString &profile);

    void readLocalProfiles();

    void loadOnlineConfig(bool update) const;
    void loadOnlineVersion() const;
    void loadExpertBinariesPage() const;
    void loadExpertSegmentsPage() const;
    void loadLocalOnlineProfiles(bool update) const;
    void displayProfileAsync(const QString &profile);
    void displayOnlineProfileAsync(const QString &profile) const;

    QString findJava() const;
    bool isLocalBRouterInstalled() const;
    bool isLocalBRouterDefaultDir() const;

    QUrl getServiceUrl() const;
    QString getSegmentsUrl() const;
    QString getBinariesUrl() const;
    QString getConfigUrl() const;

    void parseBRouterVersion(const QString &text);

    void onInvalidSetup();

signals:
    void sigOnlineConfigLoaded();
    void sigVersionChanged();
    void sigBinariesPageLoaded();
    void sigSegmentsPageLoaded();
    void sigProfilesChanged();
    void sigDisplayOnlineProfileFinished(const QString profile, const QString content);
    void sigError(const QString error, const QString details);

private slots:
    void slotOnlineRequestFinished(QNetworkReply *reply);
    void slotLoadOnlineProfilesRequestFinished(bool ok);

private:
    enum request_e { eTypeConfig, eTypeProfile, eTypeBinariesPage, eTypeSegmentsPage, eTypeVersion };
    enum profileRequest_e { eProfileInstall, eProfileDisplay };

    QDir getProfileDir(const mode_e mode) const;
    void loadOnlineProfileAsync(const QString &profile, const profileRequest_e mode) const;
    void loadOnlineConfigFinished(QNetworkReply* reply);
    void loadOnlineVersionFinished(QNetworkReply* reply);
    void loadOnlineProfileFinished(QNetworkReply * reply);
    void afterSlotLoadOnlineProfilesRequestFinishedRunJavascript(const QVariant &v);
    void mergeOnlineProfiles(const QStringList &profilesLoaded);
    void emitOnlineConfigScriptError(const QJSValue &error);
    void emitNetworkError(QString error);
    mode_e modeFromString(const QString &mode) const;
    QString stringFromMode(const mode_e mode) const;

    QStringList onlineProfiles;
    QStringList localProfiles;

    QNetworkAccessManager * networkAccessManager;
    QWebEnginePage * profilesWebPage;

    bool expertMode;
    mode_e installMode;
    QString expertConfigUrl;
    QString onlineServiceUrl;
    QString onlineProfilesUrl;
    QStringList onlineProfilesAvailable;
    QString localDir;
    QString localJavaExecutable;
    QString localProfileDir;
    QString localCustomProfileDir;
    QString localSegmentsDir;
    QString localHost;
    QString localPort;
    bool localBindLocalonly;
    QString localNumberThreads;
    QString localMaxRunningTime;
    QString localJavaOpts;
    QString expertBinariesUrl;
    QString expertSegmentsUrl;

    int versionMajor { NOINT };
    int versionMinor { NOINT };
    int versionPatch { NOINT };

    const bool defaultExpertMode = false;
    const mode_e defaultInstallMode = eModeOnline;
    const QString defaultConfigUrl = "https://brouter.de/brouter-web/config.js";
    const QString defaultOnlineServiceUrl = "https://brouter.de";
    const QString defaultOnlineProfilesUrl = "https://brouter.de/brouter/profiles2/";
    const QString defaultLocalDir = ".";
    const QString defaultLocalProfileDir = "profiles2";
    const QString defaultLocalCustomProfileDir = "customprofiles";
    const QString defaultLocalSegmentsDir = "segments4";
    const QString defaultLocalHost = "127.0.0.1";
    const QString defaultLocalPort = "17777";
    const bool defaultLocalBindLocalonly = true;
    const QString defaultLocalNumberThreads = "1";
    const QString defaultLocalMaxRunningTime = "300";
    const QString defaultLocalJavaOpts = "-Xmx128M -Xms128M -Xmn8M";
    const QString defaultBinariesUrl = "https://brouter.de/brouter_bin/";
    const QString defaultSegmentsUrl = "https://brouter.de/brouter/segments4/";

    const QString onlineCacheDir = "BRouter";

    friend class CRouterBRouter;
    friend class CRouterBRouterLocal;
    friend class CRouterBRouterSetupPage;
    friend class CRouterBRouterSetupWizard;
    friend class CRouterBRouterTilesSelect;
};

#endif
