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

#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QGuiApplication>
#include <QJsonDocument>
#include <QRegularExpression>
#include <QSaveFile>
#include <QScreen>
#include <algorithm>

#include "shoot/CShotPage.h"

namespace {
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
  return QString();
}

QString CShotFiles::shotFile() const { return repo.absoluteFilePath("doc/shots/" + page + ".json"); }

QString CShotFiles::pageFile() const { return repo.absoluteFilePath("doc/pages/" + page + ".md"); }

QString CShotFiles::scenarioConfig(const QString& scenario) const {
  return repo.absoluteFilePath("doc/shots/" + page + "/" + scenario + ".ini");
}

QString CShotFiles::publishedImage(const QString& id) const {
  return repo.absoluteFilePath("doc/images/" + id + ".png");
}

QString CShotFiles::workImage(const QString& id) const {
  return repo.absoluteFilePath("doc/images/_work/" + id + ".png");
}

QString CShotFiles::placementFile() const { return repo.absoluteFilePath("doc/shots/_cache/doc-panel.ini"); }

std::optional<QPoint> CShotFiles::onScreen(const QPoint& at, const QSize& size) {
  const QScreen* screen = QGuiApplication::screenAt(QRect(at, size).center());
  if (nullptr == screen) {
    return std::nullopt;
  }
  const QRect& room = screen->availableGeometry();
  return QPoint(std::max(room.left(), std::min(at.x(), room.right() + 1 - size.width())),
                std::max(room.top(), std::min(at.y(), room.bottom() + 1 - size.height())));
}

bool CShotFiles::hasUnpublishedImages() const {
  QDirIterator walk(repo.absoluteFilePath("doc/images/_work/" + page), {"*.png"}, QDir::Files,
                    QDirIterator::Subdirectories);
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
  content = document.object();
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
    row_t row = picturesOf(shot["id"].toString());
    row.scenario = shot["scenario"].toString();
    row.note = shot["note"].toString();
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
  if (!QFileInfo::exists(workImage(id))) {
    return QString("%1 was not taken again.").arg(id);
  }
  if (!QFile::remove(workImage(id))) {
    return QString("%1 cannot be deleted.").arg(workImage(id));
  }
  return QString();
}
