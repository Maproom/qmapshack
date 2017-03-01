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
#include "setup/IAppSetup.h"
#include <QWebPage>

class CRouterBRouterSetup : public QObject
{
    Q_OBJECT
public:
    CRouterBRouterSetup();
    ~CRouterBRouterSetup();
    enum mode_e { ModeLocal, ModeOnline };
    struct tile_s { QPoint tile; QDateTime date; qreal size; };
    bool expertMode;
    mode_e installMode;
    QString onlineWebUrl;
    QString onlineServiceUrl;
    QString onlineProfilesUrl;
    QStringList onlineProfilesAvailable;
    QString localDir;
    QString localProfileDir;
    QString localSegmentsDir;
    QString localHost;
    QString localPort;
    QString binariesUrl;
    QString segmentsUrl;

    void load();
    void save();

    void resetOnlineWebUrl();
    void resetOnlineServiceUrl();
    void resetOnlineProfilesUrl();
    void resetLocalProfileDir();
    void resetLocalSegmentsDir();
    void resetLocalHost();
    void resetLocalPort();
    void resetBinariesUrl();
    void resetSegmentsUrl();

    const QStringList getProfiles();

    void addProfile(const QString profile);
    void deleteProfile(const QString profile);
    void profileUp(const QString profile);
    void profileDown(const QString profile);

    void updateLocalProfiles();

    void loadOnlineConfig();
    void displayProfileAsync(const QString profile);
    void displayOnlineProfileAsync(const QString profile);

    bool isLocalBRouterInstalled();

signals:
    void onlineConfigChanged();
    void profilesChanged();
    void displayOnlineProfileFinished(QString profile, QString content);

private slots:
    void slotOnlineRequestFinished(QNetworkReply *reply);

private:
    const bool defaultExpertMode = false;
    const mode_e defaultInstallMode = ModeOnline;
    const QString defaultOnlineWebUrl = "http://brouter.de/brouter-web/";
    const QString defaultOnlineServiceUrl = "http://h2096617.stratoserver.net:443";
    const QString defaultOnlineProfilesUrl = "http://brouter.de/brouter/profiles2/";
    const QString defaultLocalDir = ".";
    const QString defaultLocalProfileDir = "profiles2";
    const QString defaultLocalSegmentsDir = "segments4";
    const QString defaultLocalHost = "127.0.0.1";
    const QString defaultLocalPort = "17777";
    const QString defaultBinariesUrl = "http://brouter.de/brouter_bin/";
    const QString defaultSegmentsUrl = "http://brouter.de/brouter/segments4/";

    const QString onlineCacheDir = "BRouter";

    QStringList onlineProfiles;
    QStringList localProfiles;

    const QDir getProfileDir(mode_e mode);

    enum request_e { type_config, type_profile };
    enum profileRequest_e { ProfileInstall, ProfileDisplay };

    void loadOnlineProfileAsync(const QString profile, profileRequest_e mode);
    void loadOnlineConfigFinished(QNetworkReply* reply);
    void loadOnlineProfileFinished(QNetworkReply * reply);

    const mode_e modeFromString(QString mode);
    const QString stringFromMode(mode_e mode);

    QNetworkAccessManager * networkAccessManager;
};

#endif
