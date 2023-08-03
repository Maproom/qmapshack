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

#include <QtCore>

#include "units/IUnit.h"

class CRouterBRouterSetupWizard;
class QJSValue;
class QNetworkAccessManager;
class QNetworkReply;
class QWebEnginePage;

struct CRouterBRouterLocalSetupStatus {
 public:
  CRouterBRouterLocalSetupStatus(const bool& isJavaExisting, const bool& isJavaValid, const bool& isJavaOutdated,
                                 const bool& isLocalBRouterJar, const bool& isLocalBRouterCandidate,
                                 const bool& isValidBRouterVersion)
      : isJavaExisting(isJavaExisting),
        isJavaValid(isJavaValid),
        isJavaOutdated(isJavaOutdated),
        isLocalBRouterJar(isLocalBRouterJar),
        isLocalBRouterCandidate(isLocalBRouterCandidate),
        isValidBRouterVersion(isValidBRouterVersion) {}

 private:
  const bool isJavaExisting;
  const bool isJavaValid;
  const bool isJavaOutdated;
  const bool isLocalBRouterJar;
  const bool isLocalBRouterCandidate;
  const bool isValidBRouterVersion;

  friend class CRouterBRouterSetup;
  friend class CRouterBRouterSetupWizard;
};

class CRouterBRouterSetup : public QObject {
  Q_OBJECT
 public:
  CRouterBRouterSetup(QObject* parent);
  ~CRouterBRouterSetup();

  enum mode_e { eModeLocal, eModeOnline, eModeIllegal };
  struct tile_t {
    QPoint tile;
    QDateTime date;
    qreal size;
  };

  void load();
  void save();

  void resetAll();
  void resetInstallMode() { installMode = defaultInstallMode; }
  void resetOnlineConfigUrl() { expertConfigUrl = defaultConfigUrl; }
  void resetOnlineServiceUrl() { onlineServiceUrl = defaultOnlineServiceUrl; }
  void resetOnlineProfilesUrl() { onlineProfilesUrl = defaultOnlineProfilesUrl; }
  void resetLocalBRouterJar() { setLocalBRouterJar(defaultLocalBRouterJar); }
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

  void addProfile(const QString& profile);
  void deleteProfile(const QString& profile);
  void profileUp(const QString& profile);
  void profileDown(const QString& profile);

  void readLocalProfiles();

  void loadOnlineConfig(bool update) const;
  void loadOnlineVersion() const;
  void loadExpertBinariesPage() const;
  void loadExpertSegmentsPage() const;
  void loadLocalOnlineProfiles(bool update) const;
  void displayProfileAsync(const QString& profile);
  void displayOnlineProfileAsync(const QString& profile) const;

  void setJava(const QString& path);
  QString findJava() const;
  void setLocalBRouterJar(const QString& path);
  bool isLocalBRouterDefaultDir() const;

  QUrl getServiceUrl() const;
  QString getSegmentsUrl() const;
  QString getBinariesUrl() const;
  QString getConfigUrl() const;

  void parseBRouterVersion(const QString& text);
  CRouterBRouterLocalSetupStatus checkLocalBRouterInstallation();

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
  void slotOnlineRequestFinished(QNetworkReply* reply);
  void slotLoadOnlineProfilesRequestFinished(bool ok);

 private:
  enum request_e { eTypeConfig, eTypeProfile, eTypeBinariesPage, eTypeSegmentsPage, eTypeVersion };
  enum profileRequest_e { eProfileInstall, eProfileDisplay };

  QDir getProfileDir(const mode_e mode) const;
  QDir getDownloadDir() const;
  void installLocalBRouter(QStringList& messageList);
  void installLocalBRouterFile(const QFileInfo& srcFileInfo, const QDir& targetDir, QStringList& messageList) const;
  bool tryJavaVersion(const QStringList& arguments, const QString& pattern);
  bool parseJavaVersion(const QString& javaOutput, QRegularExpression& re);
  void loadOnlineProfileAsync(const QString& profile, const profileRequest_e mode) const;
  void loadOnlineConfigFinished(QNetworkReply* reply);
  void loadOnlineVersionFinished(QNetworkReply* reply);
  void loadOnlineProfileFinished(QNetworkReply* reply);
  void afterSlotLoadOnlineProfilesRequestFinishedRunJavascript(const QVariant& v);
  void mergeOnlineProfiles(const QStringList& profilesLoaded);
  void emitOnlineConfigScriptError(const QJSValue& error);
  void emitNetworkError(QString error);
  mode_e modeFromString(const QString& mode) const;
  QString stringFromMode(const mode_e mode) const;

  QStringList onlineProfiles;
  QStringList localProfiles;

  QNetworkAccessManager* networkAccessManager;
  QWebEnginePage* profilesWebPage;

  bool expertMode;
  mode_e installMode;
  QString expertConfigUrl;
  QString onlineServiceUrl;
  QString onlineProfilesUrl;
  QStringList onlineProfilesAvailable;
  QString localDir;
  QString localJavaExecutable;
  QString localBRouterJar;
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

  int versionMajor{NOINT};
  int versionMinor{NOINT};
  int versionPatch{NOINT};

  int javaMajorVersion{NOINT};
  int classMajorVersion{NOINT};

  bool isLocalBRouterValid;

  const bool defaultExpertMode = false;
  const mode_e defaultInstallMode = eModeOnline;
  static constexpr const char* defaultConfigUrl = "https://brouter.de/brouter-web/config.js";
  static constexpr const char* defaultOnlineServiceUrl = "https://brouter.de";
  static constexpr const char* defaultOnlineProfilesUrl = "https://brouter.de/brouter/profiles2/";
  static constexpr const char* defaultLocalDir = ".";
  static constexpr const char* defaultLocalBRouterJar = "brouter.jar";
  static constexpr const char* defaultLocalProfileDir = "profiles2";
  static constexpr const char* defaultLocalCustomProfileDir = "customprofiles";
  static constexpr const char* defaultLocalSegmentsDir = "segments4";
  static constexpr const char* defaultLocalHost = "127.0.0.1";
  static constexpr const char* defaultLocalPort = "17777";
  const bool defaultLocalBindLocalonly = true;
  static constexpr const char* defaultLocalNumberThreads = "1";
  static constexpr const char* defaultLocalMaxRunningTime = "300";
  static constexpr const char* defaultLocalJavaOpts = "-Xmx128M -Xms128M -Xmn8M";
  static constexpr const char* defaultBinariesUrl = "https://api.github.com/repos/abrensch/brouter/releases";
  static constexpr const char* defaultSegmentsUrl = "https://brouter.de/brouter/segments4/";

  static constexpr const char* onlineProfileCacheDir = "BRouterProfiles";
  static constexpr const char* downloadCacheDir = "BRouterDownload";

  friend class CRouterBRouter;
  friend class CRouterBRouterLocal;
  friend class CRouterBRouterDownloadPage;
  friend class CRouterBRouterSetupPage;
  friend class CRouterBRouterSetupWizard;
  friend class CRouterBRouterTilesSelect;
};

#endif
