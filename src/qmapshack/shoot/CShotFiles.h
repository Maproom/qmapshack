/**********************************************************************************************
   Copyright (C) 2026 Oliver Eichler <oliver.eichler@gmx.de>

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

#ifndef CSHOTFILES_H
#define CSHOTFILES_H

#include <QDir>
#include <QJsonArray>
#include <QJsonObject>
#include <QList>
#include <QPair>
#include <QSet>
#include <QString>
#include <QStringList>

/**
   @brief One page's files in the checkout: the shot file, its scenarios' configurations, the pictures.

   Every rule about what an operation may change and what it takes with it lives here, so the panel only asks and
   calls. An operation returns an empty string on success, else what went wrong; a failed operation changes nothing.
   No rendering, no widgets.
 */
class CShotFiles {
 public:
  /** What the panel calls the base; nothing stores it. */
  static inline const QString kBaseLabel = QStringLiteral("(base)");

  enum state_e {
    eTaken,        ///< referenced by the page, has a picture
    eNoImage,      ///< referenced by the page, has a shot, no picture
    eNotUsed,      ///< has a shot, no page references it
    eMissing,      ///< referenced by the page, neither shot nor picture
    eUnregistered  ///< referenced by the page, has a picture, no shot
  };

  /** One picture of the page as the panel lists it. */
  struct row_t {
    QString id;
    QString scenario;  ///< empty is `(base)`
    QString note;
    state_e state = eTaken;
    bool changed = false;     ///< a work picture waits to be published
    bool takeable = false;    ///< the shot names what to photograph
    bool revertable = false;  ///< a work picture or the entry from before the take waits
    QString imagePath;        ///< the work picture, else the published one; empty when there is none
    QString publishedPath;    ///< empty when there is none
  };

  CShotFiles(const QDir& repo, const QString& page);

  /** @return why @p name cannot be a scenario, empty when it can */
  static QString nameProblem(const QString& name);
  /** @return the scenario whose name differs from @p name only in case, empty when there is none */
  QString caseTwinOf(const QString& name) const;
  /** @return true when @p name, a shot id or scenario name joined below a directory, stays below it */
  static bool staysInside(const QString& name);

  QString shotFile() const;
  /** @return why the shot file cannot be read, empty when it can or does not exist */
  QString shotFileProblem() const;
  /** @return the page's own base, `doc/shots/<page>.ini`, a whole configuration */
  QString baseFile() const;
  /** @return the bases a page's base can be copied from: the default fixture's, then every other page's, by label */
  QList<QPair<QString, QString>> bases() const;
  /** @brief Replace the page's base with a copy of @p source; a failure leaves the old one as it was */
  QString copyBase(const QString& source) const;
  /** @return `doc/shots/fixtures/<page>`, what the page's fixture differs in from the default */
  QString fixtureDir() const;
  /** @brief Add to fixtureDir() what it lacks: an empty folder per part and a README saying what goes in */
  QString makeFixtureDir() const;
  /** @return the parts the page's fixture has itself; every other one is the default's (shots.py fixture_part()) */
  QStringList ownFixtureParts() const;
  /** @return the page's fixture folder and every part folder in it */
  QStringList fixtureFolders() const;
  /** @return the INI file the panel's size and where the panel and the application window sit are kept in */
  QString placementFile() const;
  QString pageFile() const;
  /** @return `doc/shots/<page>/<scenario>.ini` */
  QString scenarioConfig(const QString& scenario) const;
  QString publishedImage(const QString& id) const;
  QString workImage(const QString& id) const;
  /** @return where storeShot() keeps @p id's entry from before it was taken again, until revert or publish */
  QString workEntry(const QString& id) const;

  /** @return where a recording waits until it replays: `doc/shots/_cache/<page>-trial.json` */
  QString trialFile() const;
  /** @return the parked recording's settings: `doc/shots/_cache/<page>-trial.ini` */
  QString trialConfig() const;

  /** @return true when `doc/images/_work/` holds a picture of any page */
  bool hasUnpublishedImages() const;

  QStringList scenarioNames() const;
  QJsonObject scenarios() const;
  /** @return the entry for @p id, empty when there is none */
  QJsonObject shot(const QString& id) const;
  /** @return the ids of the shots taken in @p scenario */
  QStringList shotsIn(const QString& scenario) const;

  /** @return true for an id of this page, `<page>/<name>` */
  bool isOwn(const QString& id) const;

  /** @return the shot file's shots in order, then the page's own references without a shot, sorted */
  QList<row_t> rows() const;

  /** @return the ids of this page's shots no page under `doc/pages` references */
  QStringList unusedShots() const;

  /** @return true when changing @p id's scenario throws away anything but `id` and `scenario`, or a picture */
  bool rebindLoses(const QString& id) const;

  /** @brief Rename a scenario: its configuration, then every reference in the shot file. */
  QString renameScenario(const QString& from, const QString& to);

  /** @brief Delete a scenario; its shots are reduced to their `id` and lose their pictures and its configuration. */
  QString deleteScenario(const QString& name);

  /** @brief Take own @p id in @p scenario (empty is `(base)`): reduced to `id` and `scenario`, pictures deleted. */
  QString rebindShot(const QString& id, const QString& scenario);

  /** @brief Remove shots and their pictures; the scenarios stay. */
  QString removeShots(const QStringList& ids);

  /** @brief Delete @p id's work picture and put back its entry from before it was taken again. */
  QString revertShot(const QString& id);

  /** @brief Write @p steps to trialFile(); trialConfig() is the caller's to write. */
  QString parkRecording(const QJsonArray& steps) const;

  /** @brief Read the steps parkRecording() wrote. */
  QString parkedRecording(QJsonArray& steps) const;

  /** @brief Delete trialFile() and trialConfig(). */
  void dropParked() const;

  /**
     @brief Store @p steps as the scenario @p name, replacing one of that name, and @p config as its configuration.

     @param config  an INI file copied to scenarioConfig(), empty to leave the configuration alone
   */
  QString storeScenario(const QString& name, const QJsonArray& steps, const QString& config);

  /** @brief Store own @p shot, replacing the one with its id; the first replaced since publish is kept for revert. */
  QString storeShot(const QJsonObject& shot);

 private:
  QString readShotFile(QJsonObject& content) const;
  QString writeShotFile(const QJsonObject& content) const;
  /** @brief Delete the published and the work picture of every id. */
  void removePictures(const QStringList& ids) const;
  QSet<QString> references(const QString& file) const;
  QSet<QString> allReferences() const;

  QDir repo;
  QString page;
};

#endif  // CSHOTFILES_H
