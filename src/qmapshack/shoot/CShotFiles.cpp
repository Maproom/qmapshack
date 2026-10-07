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

#include "shoot/CShotFiles.h"

#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QRegularExpression>
#include <QSaveFile>

#include "shoot/CShotPage.h"

namespace {
/** A fixture's parts, one folder each; shots.py FIXTURE_PARTS. */
const QStringList kFixtureParts = {"projects", "maps", "dem", "poi", "routino", "database"};
/** Describe a part and are no part of it; shots.py FIXTURE_NOTES. */
const QStringList kFixtureNotes = {"SOURCE.md", "README.md"};
/** @return the index of the shot with @p id, -1 when there is none */
qsizetype indexOf(const QJsonArray& shots, const QString& id) {
  for (qsizetype i = 0; i < shots.size(); i++) {
    if (shots.at(i).toObject()["id"].toString() == id) {
      return i;
    }
  }
  return -1;
}
}  // namespace

CShotFiles::CShotFiles(const QDir& repo, const QString& page) : repo(repo), page(page) {}

QString CShotFiles::nameProblem(const QString& name) {
  if (name.isEmpty()) {
    return "A scenario needs a name.";
  }
  if (CShotPage::kBaseScenario == name || kBaseLabel == name) {
    return QString("%1 is reserved.").arg(name);
  }
  // Passed as an argument of its own to shots.py and to the state process.
  if (name.startsWith('-')) {
    return QString("%1 starts with -, which reads as a command line option.").arg(name);
  }
  // It names `<name>.ini` on every platform.
  static const QRegularExpression forbidden(R"([/\\:*?"<>|\x00-\x1f])");
  if (name.contains(forbidden) || name != name.trimmed() || name.endsWith('.')) {
    return QString("%1 cannot be a file name. Leave out / \\ : * ? \" < > |, and spaces or dots at the ends.")
        .arg(name);
  }
  // Windows opens a device for these, whatever the extension.
  static const QRegularExpression device(R"(^(con|prn|aux|nul|com[1-9]|lpt[1-9])(\..*)?$)",
                                         QRegularExpression::CaseInsensitiveOption);
  if (device.match(name).hasMatch()) {
    return QString("%1 is a device name on Windows and cannot be a file name.").arg(name);
  }
  return QString();
}

bool CShotFiles::staysInside(const QString& name) {
  // Ids and scenario names come from files a writer may edit by hand. shots.py's stays_inside() is the same rule.
  if (name.isEmpty() || name.startsWith('/') || name.contains('\\') || name.contains(':')) {
    return false;
  }
  const QStringList& parts = name.split('/');
  for (const QString& part : parts) {
    if (part.isEmpty() || "." == part || ".." == part) {
      return false;
    }
  }
  return true;
}

QString CShotFiles::shotFile() const { return repo.absoluteFilePath("doc/shots/" + page + ".json"); }

QString CShotFiles::pageFile() const { return repo.absoluteFilePath("doc/pages/" + page + ".md"); }

QString CShotFiles::scenarioConfig(const QString& scenario) const {
  return staysInside(scenario) ? repo.absoluteFilePath("doc/shots/" + page + "/" + scenario + ".ini") : QString();
}

QString CShotFiles::publishedImage(const QString& id) const {
  return staysInside(id) ? repo.absoluteFilePath("doc/images/" + id + ".png") : QString();
}

QString CShotFiles::workImage(const QString& id) const {
  return staysInside(id) ? repo.absoluteFilePath("doc/images/_work/" + id + ".png") : QString();
}

QString CShotFiles::workEntry(const QString& id) const {
  return staysInside(id) ? repo.absoluteFilePath("doc/images/_work/" + id + ".shot.json") : QString();
}

QString CShotFiles::baseFile() const { return repo.absoluteFilePath("doc/shots/" + page + ".ini"); }

QList<QPair<QString, QString>> CShotFiles::bases() const {
  QList<QPair<QString, QString>> found{
      {"the default fixture's", repo.absoluteFilePath("doc/shots/fixtures/default/shots.ini")}};
  const QFileInfoList& inis =
      QDir(repo.absoluteFilePath("doc/shots")).entryInfoList({"*.ini"}, QDir::Files, QDir::Name);
  for (const QFileInfo& ini : inis) {
    if (ini.completeBaseName() != page) {
      found << qMakePair(QString("page %1").arg(ini.completeBaseName()), ini.absoluteFilePath());
    }
  }
  return found;
}

QString CShotFiles::copyBase(const QString& source) const {
  QFile in(source);
  if (!in.open(QIODevice::ReadOnly)) {
    return QString("%1 cannot be read.").arg(source);
  }
  // Swapped in only by commit(), so a failure leaves the old base as it was.
  QSaveFile out(baseFile());
  if (!out.open(QIODevice::WriteOnly) || out.write(in.readAll()) < 0 || !out.commit()) {
    return QString("%1 cannot be written: %2").arg(baseFile(), out.errorString());
  }
  return QString();
}

QString CShotFiles::fixtureDir() const { return repo.absoluteFilePath("doc/shots/fixtures/" + page); }

QStringList CShotFiles::ownFixtureParts() const {
  QStringList own;
  for (const QString& part : kFixtureParts) {
    // Dotfiles are hidden by the filter, as shots.py skips them.
    const QStringList& entries =
        QDir(QDir(fixtureDir()).absoluteFilePath(part)).entryList(QDir::AllEntries | QDir::NoDotAndDotDot);
    for (const QString& entry : entries) {
      if (!kFixtureNotes.contains(entry)) {
        own << part;
        break;
      }
    }
  }
  return own;
}

QStringList CShotFiles::fixtureFolders() const {
  QStringList folders{fixtureDir()};
  for (const QString& part : kFixtureParts) {
    folders << QDir(fixtureDir()).absoluteFilePath(part);
  }
  return folders;
}

QString CShotFiles::makeFixtureDir() const {
  // Also for a folder a writer or an older build made: whatever is missing is added, nothing replaced.
  for (const QString& part : kFixtureParts) {
    if (!QDir().mkpath(QDir(fixtureDir()).absoluteFilePath(part))) {
      return QString("%1 cannot be created.").arg(QDir(fixtureDir()).absoluteFilePath(part));
    }
  }
  const QString& readme = QDir(fixtureDir()).absoluteFilePath("README.md");
  if (QFileInfo::exists(readme)) {
    return QString();
  }
  QSaveFile out(readme);
  if (!out.open(QIODevice::WriteOnly)) {
    return QString("%1 cannot be created.").arg(out.fileName());
  }
  out.write(QString("# Fixture of the page %1\n\n"
                    "Put into a part's folder only what this page needs different from `../default/`: "
                    "`projects/`, `maps/`, `dem/`, `poi/`, `routino/`, `database/`. A part holding something "
                    "replaces the default's part whole; an empty or missing one is the default's.\n")
                .arg(page)
                .toUtf8());
  return out.commit() ? QString() : QString("%1 cannot be written.").arg(out.fileName());
}

QString CShotFiles::placementFile() const { return repo.absoluteFilePath("doc/shots/_cache/doc-panel.ini"); }

QString CShotFiles::trialFile() const { return repo.absoluteFilePath("doc/shots/_cache/" + page + "-trial.json"); }

QString CShotFiles::trialConfig() const { return repo.absoluteFilePath("doc/shots/_cache/" + page + "-trial.ini"); }

bool CShotFiles::hasUnpublishedImages() const {
  QDirIterator walk(repo.absoluteFilePath("doc/images/_work"), {"*.png"}, QDir::Files, QDirIterator::Subdirectories);
  return walk.hasNext();
}

QString CShotFiles::readShotFile(QJsonObject& content) const {
  QFile in(shotFile());
  if (!in.exists()) {
    content = QJsonObject();
    return QString();
  }
  if (!in.open(QIODevice::ReadOnly)) {
    return QString("%1 cannot be read: %2").arg(shotFile(), in.errorString());
  }
  QJsonParseError error;
  const QJsonDocument& document = QJsonDocument::fromJson(in.readAll(), &error);
  if (QJsonParseError::NoError != error.error || !document.isObject()) {
    return QString("%1 is not a shot file: %2").arg(shotFile(), error.errorString());
  }
  // Read as empty, a member of the wrong type would be dropped by the next write.
  const QJsonObject& object = document.object();
  if ((object.contains("shots") && !object["shots"].isArray()) ||
      (object.contains("scenarios") && !object["scenarios"].isObject())) {
    return QString("%1 is not a shot file: shots must be a list and scenarios an object.").arg(shotFile());
  }
  content = object;
  return QString();
}

QString CShotFiles::writeShotFile(const QJsonObject& content) const {
  // QSaveFile: a failed write leaves the old file whole. QJsonDocument::Indented is what shots.py writes too.
  QSaveFile out(shotFile());
  if (!out.open(QIODevice::WriteOnly) || out.write(QJsonDocument(content).toJson(QJsonDocument::Indented)) < 0 ||
      !out.commit()) {
    return QString("%1 cannot be written: %2").arg(shotFile(), out.errorString());
  }
  return QString();
}

void CShotFiles::removePictures(const QStringList& ids) const {
  for (const QString& id : ids) {
    QFile::remove(publishedImage(id));
    QFile::remove(workImage(id));
    QFile::remove(workEntry(id));
  }
}

QSet<QString> CShotFiles::references(const QString& file) const {
  QSet<QString> referenced;
  QFile in(file);
  if (!in.open(QIODevice::ReadOnly | QIODevice::Text)) {
    return referenced;
  }
  // shots.py's PICTURE_REFERENCE; without the option Qt's \w is ASCII and misses `größe.png`, Python's is not.
  static const QRegularExpression pattern("images/([\\w./-]+)\\.png", QRegularExpression::UseUnicodePropertiesOption);
  QRegularExpressionMatchIterator matches = pattern.globalMatch(QString::fromUtf8(in.readAll()));
  while (matches.hasNext()) {
    referenced << matches.next().captured(1);
  }
  return referenced;
}

QSet<QString> CShotFiles::allReferences() const {
  // shots.py's page_references(): every page, because a page may use another page's picture.
  QSet<QString> referenced;
  QDirIterator walk(repo.absoluteFilePath("doc/pages"), {"*.md"}, QDir::Files, QDirIterator::Subdirectories);
  while (walk.hasNext()) {
    referenced.unite(references(walk.next()));
  }
  return referenced;
}

QString CShotFiles::shotFileProblem() const {
  QJsonObject content;
  return readShotFile(content);
}

QJsonObject CShotFiles::scenarios() const {
  QJsonObject content;
  readShotFile(content);
  return content["scenarios"].toObject();
}

QStringList CShotFiles::scenarioNames() const { return scenarios().keys(); }

QJsonObject CShotFiles::shot(const QString& id) const {
  QJsonObject content;
  readShotFile(content);
  const QJsonArray& shots = content["shots"].toArray();
  const qsizetype index = indexOf(shots, id);
  return (index < 0) ? QJsonObject() : shots.at(index).toObject();
}

QStringList CShotFiles::shotsIn(const QString& scenario) const {
  QJsonObject content;
  readShotFile(content);
  QStringList ids;
  const QJsonArray& shots = content["shots"].toArray();
  for (const QJsonValue& value : shots) {
    if (value.toObject()["scenario"].toString() == scenario) {
      ids << value.toObject()["id"].toString();
    }
  }
  return ids;
}

QList<CShotFiles::row_t> CShotFiles::rows() const {
  QJsonObject content;
  readShotFile(content);
  const QSet<QString>& own = references(pageFile());
  const QSet<QString>& anywhere = allReferences();

  QList<row_t> rows;
  QSet<QString> known;
  const QJsonArray& shots = content["shots"].toArray();
  for (const QJsonValue& value : shots) {
    const QJsonObject& shot = value.toObject();
    row_t row;
    row.id = shot["id"].toString();
    row.scenario = shot["scenario"].toString();
    row.note = shot["note"].toString();
    row.changed = QFileInfo::exists(workImage(row.id));
    // A rebound shot is reduced to its id and scenario and photographs nothing.
    // CShotPage::run() and shots.py replay skip the others.
    row.takeable = shot.contains("widget") || shot.contains("exposure");
    row.revertable = row.changed || QFileInfo::exists(workEntry(row.id));
    row.publishedPath = QFileInfo::exists(publishedImage(row.id)) ? publishedImage(row.id) : QString();
    row.imagePath = row.changed ? workImage(row.id) : row.publishedPath;
    row.state = !anywhere.contains(row.id) ? eNotUsed : (row.imagePath.isEmpty() ? eNoImage : eTaken);
    known << row.id;
    rows << row;
  }

  QStringList missing(own.begin(), own.end());
  missing.sort();
  for (const QString& id : std::as_const(missing)) {
    // Another page's picture belongs to that page's shot file.
    if (known.contains(id) || !isOwn(id)) {
      continue;
    }
    row_t row;
    row.id = id;
    row.changed = QFileInfo::exists(workImage(id));
    row.revertable = row.changed || QFileInfo::exists(workEntry(id));
    row.publishedPath = QFileInfo::exists(publishedImage(id)) ? publishedImage(id) : QString();
    row.imagePath = row.changed ? workImage(id) : row.publishedPath;
    // A picture without a shot was drawn by hand or taken before the shot file.
    row.state = row.imagePath.isEmpty() ? eMissing : eUnregistered;
    row.note = row.imagePath.isEmpty() ? "The page uses it; there is neither a shot nor a picture."
                                       : "The page uses it and the picture exists, but no shot takes it.";
    rows << row;
  }
  return rows;
}

QStringList CShotFiles::unusedShots() const {
  QJsonObject content;
  readShotFile(content);
  const QSet<QString>& anywhere = allReferences();
  QStringList ids;
  const QJsonArray& shots = content["shots"].toArray();
  for (const QJsonValue& value : shots) {
    const QString& id = value.toObject()["id"].toString();
    if (!anywhere.contains(id)) {
      ids << id;
    }
  }
  ids.sort();
  return ids;
}

bool CShotFiles::isOwn(const QString& id) const { return id.startsWith(page + "/"); }

bool CShotFiles::rebindLoses(const QString& id) const {
  // Everything but these two goes, a note as much as a widget address.
  QJsonObject entry = shot(id);
  entry.remove("id");
  entry.remove("scenario");
  return !entry.isEmpty() || QFileInfo::exists(publishedImage(id)) || QFileInfo::exists(workImage(id));
}

QString CShotFiles::renameScenario(const QString& from, const QString& to) {
  if (const QString& problem = nameProblem(to); !problem.isEmpty()) {
    return problem;
  }
  if (const QString& twin = caseTwinOf(to); !twin.isEmpty() && twin != from) {
    return QString("%1 differs from the scenario %2 only in case.").arg(to, twin);
  }
  QJsonObject content;
  if (const QString& error = readShotFile(content); !error.isEmpty()) {
    return error;
  }
  QJsonObject scenarios = content["scenarios"].toObject();
  if (!scenarios.contains(from)) {
    return QString("There is no scenario %1.").arg(from);
  }
  // A rename that changes only the case finds its own configuration on a case-insensitive file system.
  const bool caseOnly = 0 == from.compare(to, Qt::CaseInsensitive);
  if (scenarios.contains(to) || (!caseOnly && QFileInfo::exists(scenarioConfig(to)))) {
    return QString("There is a scenario called %1 already.").arg(to);
  }

  scenarios[to] = scenarios.value(from);
  scenarios.remove(from);
  content["scenarios"] = scenarios;
  QJsonArray shots = content["shots"].toArray();
  for (qsizetype i = 0; i < shots.size(); i++) {
    QJsonObject entry = shots.at(i).toObject();
    if (entry["scenario"].toString() == from) {
      entry["scenario"] = to;
      shots.replace(i, entry);
    }
  }
  content["shots"] = shots;

  // The configuration first: left behind, the scenario would open on the base.
  const bool hasConfig = QFileInfo::exists(scenarioConfig(from));
  if (hasConfig && !QFile::rename(scenarioConfig(from), scenarioConfig(to))) {
    return QString("%1 cannot be renamed to %2.").arg(scenarioConfig(from), scenarioConfig(to));
  }
  if (const QString& error = writeShotFile(content); !error.isEmpty()) {
    if (hasConfig) {
      QFile::rename(scenarioConfig(to), scenarioConfig(from));
    }
    return error;
  }
  return QString();
}

QString CShotFiles::deleteScenario(const QString& name) {
  QJsonObject content;
  if (const QString& error = readShotFile(content); !error.isEmpty()) {
    return error;
  }
  QJsonObject scenarios = content["scenarios"].toObject();
  if (!scenarios.contains(name)) {
    return QString("There is no scenario %1.").arg(name);
  }
  scenarios.remove(name);
  content["scenarios"] = scenarios;

  // A widget address and a rectangle frame something else in another state.
  QStringList affected;
  QJsonArray shots = content["shots"].toArray();
  for (qsizetype i = 0; i < shots.size(); i++) {
    const QJsonObject& entry = shots.at(i).toObject();
    if (entry["scenario"].toString() == name) {
      affected << entry["id"].toString();
      shots.replace(i, QJsonObject{{"id", entry["id"]}});
    }
  }
  content["shots"] = shots;

  if (const QString& error = writeShotFile(content); !error.isEmpty()) {
    return error;
  }
  removePictures(affected);
  QFile::remove(scenarioConfig(name));
  return QString();
}

QString CShotFiles::rebindShot(const QString& id, const QString& scenario) {
  if (!isOwn(id)) {
    return QString("%1 belongs to another page.").arg(id);
  }
  if (!scenario.isEmpty() && !scenarioNames().contains(scenario)) {
    return QString("There is no scenario %1.").arg(scenario);
  }
  QJsonObject content;
  if (const QString& error = readShotFile(content); !error.isEmpty()) {
    return error;
  }
  QJsonObject bare{{"id", id}};
  if (!scenario.isEmpty()) {
    bare["scenario"] = scenario;
  }
  QJsonArray shots = content["shots"].toArray();
  const qsizetype index = indexOf(shots, id);
  if (index < 0) {
    shots.append(bare);
  } else {
    shots.replace(index, bare);
  }
  content["shots"] = shots;

  if (const QString& error = writeShotFile(content); !error.isEmpty()) {
    return error;
  }
  removePictures({id});
  return QString();
}

QString CShotFiles::removeShots(const QStringList& ids) {
  QJsonObject content;
  if (const QString& error = readShotFile(content); !error.isEmpty()) {
    return error;
  }
  const QSet<QString> gone(ids.begin(), ids.end());
  QJsonArray keep;
  const QJsonArray& shots = content["shots"].toArray();
  for (const QJsonValue& value : shots) {
    if (!gone.contains(value.toObject()["id"].toString())) {
      keep.append(value);
    }
  }
  content["shots"] = keep;

  if (const QString& error = writeShotFile(content); !error.isEmpty()) {
    return error;
  }
  removePictures(ids);
  return QString();
}

QString CShotFiles::revertShot(const QString& id) {
  const bool hasImage = QFileInfo::exists(workImage(id));
  const bool hasEntry = QFileInfo::exists(workEntry(id));
  if (!hasImage && !hasEntry) {
    return QString("%1 was not taken again.").arg(id);
  }
  if (hasEntry) {
    QFile in(workEntry(id));
    if (!in.open(QIODevice::ReadOnly)) {
      return QString("%1 cannot be read.").arg(workEntry(id));
    }
    QJsonParseError parsed;
    const QJsonDocument& entry = QJsonDocument::fromJson(in.readAll(), &parsed);
    in.close();
    // An unreadable entry would read as {}, which removes the shot.
    if (QJsonParseError::NoError != parsed.error || !entry.isObject()) {
      return QString("%1 is damaged; nothing was reverted.").arg(workEntry(id));
    }
    const QJsonObject& before = entry.object();

    QJsonObject content;
    if (const QString& error = readShotFile(content); !error.isEmpty()) {
      return error;
    }
    QJsonArray shots = content["shots"].toArray();
    const qsizetype index = indexOf(shots, id);
    // An empty entry: the shot did not exist before.
    if (before.isEmpty()) {
      if (index >= 0) {
        shots.removeAt(index);
      }
    } else if (index < 0) {
      shots.append(before);
    } else {
      shots.replace(index, before);
    }
    content["shots"] = shots;
    if (const QString& error = writeShotFile(content); !error.isEmpty()) {
      return error;
    }
    QFile::remove(workEntry(id));
  }
  if (hasImage && !QFile::remove(workImage(id))) {
    return QString("%1 cannot be deleted.").arg(workImage(id));
  }
  return QString();
}

QString CShotFiles::parkRecording(const QJsonArray& steps) const {
  if (!QDir().mkpath(QFileInfo(trialFile()).absolutePath())) {
    return QString("%1 cannot be created.").arg(QFileInfo(trialFile()).absolutePath());
  }
  QSaveFile out(trialFile());
  if (!out.open(QIODevice::WriteOnly) || out.write(QJsonDocument(steps).toJson(QJsonDocument::Indented)) < 0 ||
      !out.commit()) {
    return QString("%1 cannot be written: %2").arg(trialFile(), out.errorString());
  }
  return QString();
}

QString CShotFiles::parkedRecording(QJsonArray& steps) const {
  QFile in(trialFile());
  if (!in.open(QIODevice::ReadOnly)) {
    return QString("There is no parked recording %1.").arg(trialFile());
  }
  QJsonParseError error;
  const QJsonDocument& document = QJsonDocument::fromJson(in.readAll(), &error);
  if (QJsonParseError::NoError != error.error || !document.isArray()) {
    return QString("%1 is no recording: %2").arg(trialFile(), error.errorString());
  }
  steps = document.array();
  return QString();
}

void CShotFiles::dropParked() const {
  QFile::remove(trialFile());
  QFile::remove(trialConfig());
}

QString CShotFiles::caseTwinOf(const QString& name) const {
  // On a case-insensitive file system both would be one configuration file.
  const QStringList& names = scenarioNames();
  for (const QString& other : names) {
    if (other != name && 0 == other.compare(name, Qt::CaseInsensitive)) {
      return other;
    }
  }
  return QString();
}

QString CShotFiles::storeScenario(const QString& name, const QJsonArray& steps, const QString& config) {
  if (const QString& problem = nameProblem(name); !problem.isEmpty()) {
    return problem;
  }
  if (const QString& twin = caseTwinOf(name); !twin.isEmpty()) {
    return QString("%1 differs from the scenario %2 only in case.").arg(name, twin);
  }
  QJsonObject content;
  if (const QString& error = readShotFile(content); !error.isEmpty()) {
    return error;
  }
  QJsonObject scenarios = content["scenarios"].toObject();
  scenarios[name] = steps;
  content["scenarios"] = scenarios;

  // Copied beside the target first, so a failed copy or shot file write leaves the old configuration.
  const QString& target = scenarioConfig(name);
  const QString& part = target + ".part";
  if (!config.isEmpty()) {
    QFile::remove(part);
    if (!QDir().mkpath(QFileInfo(target).absolutePath()) || !QFile::copy(config, part)) {
      QFile::remove(part);
      return QString("%1 cannot be copied to %2.").arg(config, part);
    }
  }
  if (const QString& error = writeShotFile(content); !error.isEmpty()) {
    QFile::remove(part);
    return error;
  }
  if (!config.isEmpty()) {
    QFile::remove(target);
    if (!QFile::rename(part, target)) {
      return QString("The scenario %1 is stored, its configuration is left in %2.").arg(name, part);
    }
  }
  return QString();
}

QString CShotFiles::storeShot(const QJsonObject& shot) {
  const QString& id = shot["id"].toString();
  if (!isOwn(id)) {
    return QString("%1 belongs to another page.").arg(id);
  }
  QJsonObject content;
  if (const QString& error = readShotFile(content); !error.isEmpty()) {
    return error;
  }
  QJsonArray shots = content["shots"].toArray();
  const qsizetype index = indexOf(shots, id);

  // Only the first since publish or revert: that is the entry the published picture belongs to.
  if (!QFileInfo::exists(workEntry(id))) {
    if (!QDir().mkpath(QFileInfo(workEntry(id)).absolutePath())) {
      return QString("%1 cannot be created.").arg(QFileInfo(workEntry(id)).absolutePath());
    }
    const QJsonObject& before = (index < 0) ? QJsonObject() : shots.at(index).toObject();
    QSaveFile out(workEntry(id));
    if (!out.open(QIODevice::WriteOnly) || out.write(QJsonDocument(before).toJson(QJsonDocument::Indented)) < 0 ||
        !out.commit()) {
      return QString("%1 cannot be written: %2").arg(workEntry(id), out.errorString());
    }
  }

  if (index < 0) {
    shots.append(shot);
  } else {
    shots.replace(index, shot);
  }
  content["shots"] = shots;
  return writeShotFile(content);
}
