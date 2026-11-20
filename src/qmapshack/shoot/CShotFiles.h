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
#include <QPoint>
#include <QSet>
#include <QSize>
#include <QString>
#include <QStringList>
#include <optional>

/**
   @brief One page's files in the checkout: the shot file, its scenarios' configurations, the images.

   Every rule about what an operation may change and what it takes with it lives here, so the panel only asks and
   calls. An operation returns an empty string on success, else what went wrong; a failed operation changes nothing.
   No rendering, no widgets.
 */
class CShotFiles {
 public:
  /** What the panel calls the base; nothing stores it. */
  static inline const QString kBaseLabel = QStringLiteral("(base)");
  /** @return @p scenario, or kBaseLabel for the empty one */
  static QString label(const QString& scenario) { return scenario.isEmpty() ? kBaseLabel : scenario; }

  enum state_e {
    eTaken,        ///< referenced by the page, has an image
    eNoImage,      ///< referenced by the page, has a shot, no image
    eNotUsed,      ///< has a shot, no page references it
    eMissing,      ///< referenced by the page, neither shot nor image
    eUnregistered  ///< referenced by the page, has an image, no shot
  };

  /** One image of the page as the panel lists it. */
  struct row_t {
    QString id;
    QString scenario;  ///< empty is `(base)`
    QString note;
    state_e state = eTaken;
    bool changed = false;       ///< a work image waits to be published
    bool takeable = false;      ///< the shot names what to photograph
    bool revertable = false;    ///< a work image or the entry from before the take waits
    bool marksChanged = false;  ///< its marks changed since its image was taken
    QString imagePath;          ///< the work image, else the published one; empty when there is none
    QString publishedPath;      ///< empty when there is none
  };

  CShotFiles(const QDir& repo, const QString& page);

  /** @return why @p name cannot be a scenario, empty when it can */
  static QString nameProblem(const QString& name);
  /** @return the scenario whose name differs from @p name only in case, empty when there is none */
  QString caseTwinOf(const QString& name) const;
  /** @return the entry of @p names that differs from @p name only in case, empty when there is none */
  static QString caseTwinIn(const QStringList& names, const QString& name);
  /** @return true when @p name, a shot id or scenario name joined below a directory, stays below it */
  static bool staysInside(const QString& name);

  /**
     @brief Replace @p path by @p bytes through QSaveFile, so a failed write leaves the old file whole.

     @return empty, or why it failed in the writer's words
   */
  static QString writeWhole(const QString& path, const QByteArray& bytes);
  /** @return the bytes of @p path, empty when it cannot be read */
  static QByteArray readWhole(const QString& path);
  /** @return the documentation's language: the run's `--locale` up to `_`, `en` without one */
  static QString language();
  /** @return the folder of language()'s pages, shot files and images; fixtures and tile caches are shared */
  static QString languageDir(const QDir& repo);
  /** @return where a session's images wait to be published */
  static QString workDir(const QDir& repo);
  /** @return the checkout holding @p shotFile, a page's shot file in languageDir()'s `shots/` */
  static QDir repoOfShotFile(const QString& shotFile);

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
  /** @return the tile cache root of this page: the shared one, or the page's own when its fixture has maps */
  QString tileCache() const;
  /** @return the tile cache root of the default fixture's maps, which every page without maps of its own shares */
  static QString sharedTileCache(const QDir& repo);
  /** @brief Add to fixtureDir() what it lacks: an empty folder per part and a README saying what goes in */
  QString makeFixtureDir() const;
  /** @return the parts the page's fixture has itself; every other one is the default's (shots.py fixture_part()) */
  QStringList ownFixtureParts() const;
  /** @return the parts the page marks with an `EMPTY` file: the page runs without them (shots.py fixture_part()) */
  QStringList emptyFixtureParts() const;
  /** @return the page's fixture folder and every part folder in it */
  QStringList fixtureFolders() const;
  /** @return the INI file the panel's size and where the panel and the application window sit are kept in */
  QString placementFile() const;
  /**
     @return @p at moved so a frame of @p size there lies on the available area of the screen under its centre,
     the top left corner first when it is larger; nothing when no screen is under the centre
   */
  static std::optional<QPoint> onScreen(const QPoint& at, const QSize& size);
  QString pageFile() const;
  /** @return `doc/shots/<page>/<scenario>.ini` */
  QString scenarioConfig(const QString& scenario) const;
  QString publishedImage(const QString& id) const;
  QString workImage(const QString& id) const;
  /** @return where an image just taken waits for the writer's Keep: `doc/shots/_cache/pending/` */
  QString pendingDir() const;
  QString pendingImage(const QString& id) const;
  /** @return where storeShot() keeps @p id's entry from before it was taken again, until revert or publish */
  QString workEntry(const QString& id) const;

  /** @return where a recording waits until it replays: `doc/shots/_cache/<page>-trial.json` */
  QString trialFile() const;
  /** @return the parked recording's settings: `doc/shots/_cache/<page>-trial.ini` */
  QString trialConfig() const;

  /** @return true when `doc/<lang>/images/_work/` holds an image of this page */
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
  /** @return the row of @p id, none when the page has no such image */
  std::optional<row_t> row(const QString& id) const;

  /** @return the ids of this page's shots no page under `doc/pages` references */
  QStringList unusedShots() const;

  /** @return true when changing @p id's scenario throws away anything but `id` and `scenario`, or an image */
  bool rebindLoses(const QString& id) const;

  /** @brief Rename a scenario: its configuration, then every reference in the shot file. */
  QString renameScenario(const QString& from, const QString& to);

  /** @brief Delete a scenario; its shots are reduced to their `id` and lose their images and its configuration. */
  QString deleteScenario(const QString& name);

  /** @brief Take own @p id in @p scenario (empty is `(base)`): reduced to `id` and `scenario`, images deleted. */
  QString rebindShot(const QString& id, const QString& scenario);

  /** @brief Remove shots and their images; the scenarios stay. */
  QString removeShots(const QStringList& ids);

  /** @brief Delete @p id's work image and put back its entry from before it was taken again. */
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

  /** @return the marks placed on @p id before it has a shot; kept in marksFile() */
  QJsonArray pendingMarks(const QString& id) const;
  /** @brief Keep @p marks for @p id until its first image takes them; empty drops them. */
  QString setPendingMarks(const QString& id, const QJsonArray& marks) const;
  /** @return the ids whose marks changed since their image was taken */
  QStringList marksChanged() const;
  QString setMarksChanged(const QString& id, bool on) const;
  /** @brief Forget pending marks and "marks changed" of ids that have neither a shot nor a page line. */
  QString dropStaleMarks() const;

 private:
  /** @return a row of @p id holding only where its images are */
  row_t imagesOf(const QString& id) const;
  QString readShotFile(QJsonObject& content) const;
  /** @return the launcher's own record of pending marks and changed ones: `doc/shots/_cache/<page>-marks.json` */
  QString marksFile() const;
  QJsonObject readMarksFile() const;
  QString writeMarksFile(const QJsonObject& content) const;
  QString writeShotFile(const QJsonObject& content) const;
  /** @brief Delete the published and the work image of every id, and the work entry; shots.py unused does too. */
  void removeImages(const QStringList& ids) const;
  QSet<QString> references(const QString& file) const;
  QSet<QString> allReferences() const;

  QDir repo;
  QString page;
};

#endif  // CSHOTFILES_H
