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

class CRouterBRouterSetup
{
public:
    CRouterBRouterSetup();
    ~CRouterBRouterSetup();
    enum Mode { Mode_Local, Mode_Online, Mode_None };
    bool expertMode;
    Mode installMode;
    QString onlineWebUrl;
    QString onlineServiceUrl;
    QString onlineProfilesUrl;
    QStringList onlineProfiles;
    QString localDir;
    QString localProfileDir;
    QString localSegmentsDir;
    QStringList localProfiles;
    QString localHost;
    QString localPort;
    QString binariesUrl;

    const bool defaultExpertMode = false;
    const Mode defaultInstallMode = Mode_Online;
    const QString defaultOnlineWebUrl = "http://brouter.de/brouter-web/";
    const QString defaultOnlineServiceUrl = "http://h2096617.stratoserver.net:443";
    const QString defaultOnlineProfilesUrl = "http://brouter.de/brouter/profiles2/";
    const QString defaultLocalDir = ".";
    const QString defaultLocalProfileDir = "profiles2";
    const QString defaultLocalSegmentsDir = "segments4";
    const QString defaultLocalHost = "127.0.0.1";
    const QString defaultLocalPort = "17777";
    const QString defaultBinariesUrl = "http://brouter.de/brouter_bin/";

    void load();
    void save();

    Mode modeFromString(QString mode);
    QString stringFromMode(Mode mode);

    QString readLocalProfile(QString profile);
    QString readOnlineProfile(QString profile);
    void readLocalProfiles();
    void readOnlineProfiles();
    void loadOnlineConfig();
    void loadOnlineProfiles();
    QString readProfile(Mode mode, QString profile);
    QDir getLocalProfileDir();
private:
    QDir getProfileDir(Mode mode);
    void readProfiles(Mode mode);
};

#endif
