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

#ifndef CROUTERBROUTERSETUP_H
#define CROUTERBROUTERSETUP_H

#include <QtCore>
#include <QWebPage>
#include <QScriptValue>

class CRouterBRouterSetup : public QObject
{
    Q_OBJECT
public:
    CRouterBRouterSetup(QObject * parent);
    ~CRouterBRouterSetup();

    enum mode_e { ModeLocal, ModeOnline };
    struct tile_s { QPoint tile; QDateTime date; qreal size; };

    void load();
    void save();

    void resetAll();
    void resetOnlineWebUrl();
    void resetOnlineServiceUrl();
    void resetOnlineProfilesUrl();
    void resetLocalProfileDir();
    void resetLocalCustomProfileDir();
    void resetLocalSegmentsDir();
    void resetLocalHost();
    void resetLocalPort();
    void resetLocalNumberThreads();
    void resetLocalMaxRunningTime();
    void resetLocalJavaOpts();
    void resetBinariesUrl();
    void resetSegmentsUrl();

    QStringList getProfiles() const;

    void addProfile(const QString &profile);
    void deleteProfile(const QString &profile);
    void profileUp(const QString &profile);
    void profileDown(const QString &profile);

    void updateLocalProfiles();

    void loadOnlineConfig() const;
    void displayProfileAsync(const QString &profile) const;
    void displayOnlineProfileAsync(const QString &profile) const;

    QString findJava() const;
    bool isLocalBRouterInstalled() const;

    void onInvalidSetup();

signals:
    void sigOnlineConfigChanged() const;
    void sigProfilesChanged() const;
    void sigDisplayOnlineProfileFinished(const QString profile, const QString content) const;
    void sigError(const QString error, const QString details) const;

private slots:
    void slotOnlineRequestFinished(QNetworkReply *reply);

private:
    enum request_e { type_config, type_profile };
    enum profileRequest_e { ProfileInstall, ProfileDisplay };

    QDir getProfileDir(const mode_e mode) const;
    void loadOnlineProfileAsync(const QString &profile, const profileRequest_e mode) const;
    void loadOnlineConfigFinished(QNetworkReply* reply);
    void loadOnlineProfileFinished(QNetworkReply * reply);
    void emitOnlineConfigScriptError(const QScriptValue &error) const;
    void emitNetworkError(QNetworkReply * reply) const;
    mode_e modeFromString(const QString &mode) const;
    QString stringFromMode(const mode_e mode) const;

    QStringList onlineProfiles;
    QStringList localProfiles;

    QNetworkAccessManager * networkAccessManager;

    bool expertMode;
    mode_e installMode;
    QString onlineWebUrl;
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
    QString localNumberThreads;
    QString localMaxRunningTime;
    QString localJavaOpts;
    QString binariesUrl;
    QString segmentsUrl;

    const bool defaultExpertMode = false;
    const mode_e defaultInstallMode = ModeOnline;
    const QString defaultOnlineWebUrl = "http://brouter.de/brouter-web/";
    const QString defaultOnlineServiceUrl = "http://h2096617.stratoserver.net:443";
    const QString defaultOnlineProfilesUrl = "http://brouter.de/brouter/profiles2/";
    const QString defaultLocalDir = ".";
    const QString defaultLocalProfileDir = "profiles2";
    const QString defaultLocalCustomProfileDir = "customprofiles";
    const QString defaultLocalSegmentsDir = "segments4";
    const QString defaultLocalHost = "127.0.0.1";
    const QString defaultLocalPort = "17777";
    const QString defaultLocalNumberThreads = "1";
    const QString defaultLocalMaxRunningTime = "300";
    const QString defaultLocalJavaOpts = "-Xmx128M -Xms128M -Xmn8M";
    const QString defaultBinariesUrl = "http://brouter.de/brouter_bin/";
    const QString defaultSegmentsUrl = "http://brouter.de/brouter/segments4/";

    const QString onlineCacheDir = "BRouter";

    friend class CRouterBRouter;
    friend class CRouterBRouterSetupPage;
    friend class CRouterBRouterSetupWizard;
    friend class CRouterBRouterTilesSelect;
};

#endif
