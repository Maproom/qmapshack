/**********************************************************************************************
    Copyright (C) 2014-2015 Oliver Eichler <oliver.eichler@gmx.de>
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

#include "gis/rte/router/brouter/CRouterBRouterTilesSelect.h"

#include <QNetworkReply>
#include <QNetworkRequest>
#include <QWebEnginePage>

#include "canvas/CCanvas.h"
#include "gis/rte/router/brouter/CRouterBRouterSetup.h"
#include "gis/rte/router/brouter/CRouterBRouterTilesSelectArea.h"
#include "gis/rte/router/brouter/CRouterBRouterTilesSelectLayout.h"
#include "gis/rte/router/brouter/CRouterBRouterTilesStatus.h"

const QPoint CRouterBRouterTilesSelect::noTile = QPoint(INT_MIN, INT_MIN);
const int CRouterBRouterTilesSelect::minTileLat = -180;
const int CRouterBRouterTilesSelect::maxTileLat = 180;
const int CRouterBRouterTilesSelect::minTileLon = -85;
const int CRouterBRouterTilesSelect::maxTileLon = 85;
const int CRouterBRouterTilesSelect::tileSize = 5;
// pattern for filenames of tiles: 'E10_N20.rd5'
const QString CRouterBRouterTilesSelect::patternTileName = QString("([EW])(\\d{1,3})_([NS])(\\d{1,3})\\.rd5$");
const QRegularExpression CRouterBRouterTilesSelect::regExpTileName =
    QRegularExpression(CRouterBRouterTilesSelect::patternTileName);
// pattern for tiles date parsing: '16-Feb-2017 20:48  '
const QString CRouterBRouterTilesSelect::patternDate =
    "(\\d{1,2}-(Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec)-\\d{4} \\d{1,2}:\\d{2})";
const QRegularExpression CRouterBRouterTilesSelect::regExpDate =
    QRegularExpression(CRouterBRouterTilesSelect::patternDate);
const QString CRouterBRouterTilesSelect::formatDate = "dd-MMM-yyyy HH:mm";
const QLocale CRouterBRouterTilesSelect::localeDate = QLocale(QLocale::English, QLocale::UnitedStates);
// pattern for tiles size parsing: 8.2M 271K 9.3K
const QString CRouterBRouterTilesSelect::patternSize = " {0,2}(\\d{1,3}|\\d\\.\\d)([KMG])";
const QRegularExpression CRouterBRouterTilesSelect::regExpSize =
    QRegularExpression(CRouterBRouterTilesSelect::patternSize);

CRouterBRouterTilesSelect::CRouterBRouterTilesSelect(QWidget* parent) : QWidget(parent) {
  for (int x = minTileLat; x < maxTileLat; x += tileSize) {
    for (int y = minTileLon; y < maxTileLon; y += tileSize) {
      CRouterBRouterTilesStatus* status = new CRouterBRouterTilesStatus(this);
      tilesDownloadStatus.insert(fileNameFromTile(QPoint(x, y)), status);
    }
  }

  outerLayout = new QVBoxLayout(this);
  setLayout(outerLayout);
  outerLayout->setContentsMargins(0, 0, 0, 0);
  widgetSelect = new QWidget(this);
  widgetSelect->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
  outerLayout->addWidget(widgetSelect);
  widgetSelect->show();

  QTemporaryFile temp;
  temp.open();
  temp.close();

  QSettings view(temp.fileName(), QSettings::IniFormat);
  view.clear();
  view.setValue("map/zoomIndex", 16);
  view.setValue("scales", 1);
  view.setValue("proj", "+proj=merc");
  view.setValue("grid/proj", "+proj=longlat +datum=WGS84 +no_defs");

  CCanvas* canvas = new CCanvas(widgetSelect, "BRouterTileDownload");
  // clone canvas by a temporary configuration file
  canvas->loadConfig(view);
  canvas->setMap("://map/World.gemf");

  selectArea = new CRouterBRouterTilesSelectArea(widgetSelect, canvas);

  QLayout* selectLayout = new CRouterBRouterTilesSelectLayout(widgetSelect);
  widgetSelect->setLayout(selectLayout);
  selectLayout->addWidget(canvas);
  selectLayout->addWidget(selectArea);
  canvas->lower();
  canvas->show();
  selectArea->show();

  QHBoxLayout* statusLayout = new QHBoxLayout();
  outerLayout->addLayout(statusLayout);

  statusLabel = new QLabel(this);
  statusProgress = new QProgressBar(this);

  statusLayout->addWidget(statusLabel);
  statusLayout->addWidget(statusProgress);

  statusLabel->setText(tr("available routing-data is being determined."));
  statusProgress->setVisible(false);

  errorLabel = new QLabel(this);
  outerLayout->addWidget(errorLabel);
  errorLabel->setVisible(false);

  QHBoxLayout* buttonsLayout = new QHBoxLayout();
  outerLayout->addLayout(buttonsLayout);

  pushSelectOutdated = new QPushButton(tr("Select outdated"), this);
  pushClearSelection = new QPushButton(tr("Clear Selection"), this);
  pushDeleteSelection = new QPushButton(tr("Delete selection"), this);
  pushDownload = new QPushButton(tr("Download"), this);

  buttonsLayout->addWidget(pushSelectOutdated);
  buttonsLayout->addWidget(pushClearSelection);
  buttonsLayout->addWidget(pushDeleteSelection);
  buttonsLayout->addWidget(pushDownload);

  tilesDownloadManager = new QNetworkAccessManager(this);

  tilesWebPage = new QWebEnginePage(this);

  connect(pushClearSelection, &QPushButton::clicked, this, &CRouterBRouterTilesSelect::slotClearSelection);
  connect(pushDeleteSelection, &QPushButton::clicked, this, &CRouterBRouterTilesSelect::slotDeleteSelected);
  connect(pushSelectOutdated, &QPushButton::clicked, this, &CRouterBRouterTilesSelect::slotSelectOutdated);
  connect(pushDownload, &QPushButton::clicked, this, &CRouterBRouterTilesSelect::slotDownload);
  connect(selectArea, &CRouterBRouterTilesSelectArea::sigTileClicked, this,
          &CRouterBRouterTilesSelect::slotTileClicked);
  connect(selectArea, &CRouterBRouterTilesSelectArea::sigTileToolTipChanged, this,
          &CRouterBRouterTilesSelect::slotTileToolTipChanged);
  connect(tilesWebPage, &QWebEnginePage::loadFinished, this,
          &CRouterBRouterTilesSelect::slotLoadOnlineTilesRequestFinished);
  connect(tilesDownloadManager, &QNetworkAccessManager::finished, this,
          &CRouterBRouterTilesSelect::slotDownloadFinished);
}

CRouterBRouterTilesSelect::~CRouterBRouterTilesSelect() {}

void CRouterBRouterTilesSelect::setSetup(CRouterBRouterSetup* setup) { this->setup = setup; }

void CRouterBRouterTilesSelect::slotTileClicked(const QPoint& tile) {
  CRouterBRouterTilesStatus* status = getTileStatus(tile);
  if (status->isLocal || status->isRemote) {
    status->isSelected = !status->isSelected;
    update();
  }
}

void CRouterBRouterTilesSelect::slotSelectOutdated() {
  bool changed(false);

  for (QHash<QString, CRouterBRouterTilesStatus*>::const_iterator it = tilesDownloadStatus.constBegin();
       it != tilesDownloadStatus.constEnd(); ++it) {
    CRouterBRouterTilesStatus* status = it.value();
    if (!status->isSelected && status->isOutdated) {
      changed = true;
      status->isSelected = true;
    }
  }
  if (changed) {
    update();
  }
}

void CRouterBRouterTilesSelect::slotDeleteSelected() {
  bool changed(false);

  for (QHash<QString, CRouterBRouterTilesStatus*>::const_iterator it = tilesDownloadStatus.constBegin();
       it != tilesDownloadStatus.constEnd(); ++it) {
    CRouterBRouterTilesStatus* status = it.value();
    if (status->isSelected) {
      status->isSelected = false;
      changed = true;
      if (status->isLocal) {
        QFile segment(segmentsDir().absoluteFilePath(it.key()));
        if (!segment.remove()) {
          error(tr("Error removing %1: %2").arg(segment.fileName(), segment.errorString()));
          break;
        }
        status->isLocal = false;
      }
    }
  }
  if (changed) {
    update();
  }
}

void CRouterBRouterTilesSelect::slotClearSelection() {
  bool changed(false);
  for (QHash<QString, CRouterBRouterTilesStatus*>::const_iterator it = tilesDownloadStatus.constBegin();
       it != tilesDownloadStatus.constEnd(); ++it) {
    CRouterBRouterTilesStatus* status = it.value();
    if (status->isSelected) {
      changed = true;
      status->isSelected = false;
    }
  }
  if (changed) {
    update();
  }
}

void CRouterBRouterTilesSelect::updateButtons() const {
  bool hasSelected(false);
  bool hasLocalSelected(false);
  bool hasUnselectedOutdated(false);
  bool hasSelectedForDownload(false);

  for (QHash<QString, CRouterBRouterTilesStatus*>::const_iterator it = tilesDownloadStatus.constBegin();
       it != tilesDownloadStatus.constEnd(); ++it) {
    CRouterBRouterTilesStatus* status = it.value();
    if (status->isSelected) {
      hasSelected = true;

      if (status->isLocal) {
        hasLocalSelected = true;
      }
      if (status->isOutdated || !status->isLocal) {
        hasSelectedForDownload = true;
      }
    } else if (status->isOutdated) {
      hasUnselectedOutdated = true;
    }
  }

  pushClearSelection->setEnabled(hasSelected);
  pushDeleteSelection->setEnabled(hasLocalSelected);
  pushSelectOutdated->setEnabled(hasUnselectedOutdated);
  pushDownload->setEnabled(hasSelectedForDownload);
}

void CRouterBRouterTilesSelect::initialize()

{
  try {
    for (QHash<QString, CRouterBRouterTilesStatus*>::const_iterator it = tilesDownloadStatus.constBegin();
         it != tilesDownloadStatus.constEnd(); ++it) {
      CRouterBRouterTilesStatus* status = it.value();
      status->isLocal = false;
      status->isRemote = false;
      status->isOutdated = false;
    }
    const QDir& dir = segmentsDir();
    if (!dir.exists()) {
      if (!QDir(setup->localDir).mkpath(setup->localSegmentsDir)) {
        throw tr("Error creating segments directory %1").arg(dir.path());
      }
    }

    const QStringList& segments = dir.entryList();
    for (const QString& segment : segments) {
      const QRegularExpressionMatch& match = regExpTileName.match(segment);
      if (match.hasMatch()) {
        const QPoint& tile = tileFromFileName(segment);
        if (tile != noTile) {
          CRouterBRouterTilesStatus* status = getTileStatus(tile);

          const QFileInfo& info = QFileInfo(dir, segment);
          status->localDate = info.birthTime();
          status->localSize = info.size();
          status->isLocal = true;
        }
      }
    }
    clearError();
    initialized = true;
    tilesWebPage->load(QUrl(setup->getSegmentsUrl()));
  } catch (const QString& msg) {
    error(msg);
  }
  update();
  emit sigCompleteChanged();
}

void CRouterBRouterTilesSelect::slotLoadOnlineTilesRequestFinished(bool ok) {
  if (!ok) {
    segmentsError(tr("Network Error"));
    update();
    return;
  }
  tilesWebPage->runJavaScript(
      QString("var tiles = [];"
              "var xpathResult = "
              "document.evaluate('.//a',document.body,null,XPathResult.UNORDERED_NODE_ITERATOR_TYPE,null);"
              "var reTileName = /")
          .append(patternTileName)
          .append("/;")
          .append("var reDateSize = / +")
          .append(patternDate)
          .append(" +(\\d+|")
          .append(patternSize)
          .append(")\\n$/;")
          .append("var anchor = xpathResult.iterateNext();"
                  "while(anchor) {"
                  "  if(reTileName.test(anchor.innerHTML)) {"
                  "    var tile = {};"
                  "    tile.name = anchor.innerHTML;"
                  "    var datesize = anchor.nextSibling.textContent.match(reDateSize);"
                  "    tile.date = datesize[1];"
                  "    tile.size = datesize[3];"
                  "    tiles.push(tile);"
                  "  }"
                  "  anchor = xpathResult.iterateNext();"
                  "}"
                  "tiles;"),
      [this](const QVariant& v) { afterSlotLoadOnlineTilesRequestFinishedRunJavascript(v); });
}

void CRouterBRouterTilesSelect::afterSlotLoadOnlineTilesRequestFinishedRunJavascript(const QVariant& v) {
  const QList<QVariant>& results = v.toList();
  if (results.count() == 0) {
    segmentsError(tr("invalid result, no files found"));
    update();
    return;
  }

  for (const QVariant& result : results) {
    const QMap<QString, QVariant>& tileMap = result.toMap();

    const QString& tileName = tileMap.value("name").toString();
    // only anchors matching the desired pattern
    const QRegularExpressionMatch& match = regExpTileName.match(tileName);
    if (match.hasMatch()) {
      const QPoint& tile = tileFromFileName(tileName);

      if (tile != noTile) {
        CRouterBRouterTilesStatus* status = getTileStatus(tile);
        if (status != nullptr) {
          status->isRemote = true;

          const QString& date = tileMap.value("date").toString();

          const QRegularExpressionMatch& matchDate = regExpDate.match(date);
          if (!matchDate.hasMatch()) {
            segmentsError(tr("cannot parse: %1 is not a date").arg(date));
            update();
            return;
          }

          status->remoteDate = localeDate.toDateTime(date, formatDate);

          const QString& size = tileMap.value("size").toString();
          const QRegularExpressionMatch& matchSize = regExpSize.match(size);
          if (!matchSize.hasMatch()) {
            bool ok = false;
            status->remoteSize = size.toLongLong(&ok, 10);
            if (!ok) {
              segmentsError(tr("cannot parse: %1 is not a valid size").arg(size));
              update();
              return;
            }
          } else {
            status->remoteSize = matchSize.captured(1).toFloat() * (matchSize.captured(2) == "M"   ? 1048576
                                                                    : matchSize.captured(2) == "G" ? 1073741824
                                                                    : matchSize.captured(2) == "K" ? 1024
                                                                                                   : 1);
          }

          if (status->isLocal && status->remoteDate > status->localDate) {
            status->isOutdated = true;
          }
        }
      }
    }
  }
  clearError();
  update();
}

QString CRouterBRouterTilesSelect::formatSize(const qint64 size) {
  if (size >= 2147483648) {
    return QString("%1G").arg(size / 1073741824);
  } else if (size >= 1073741824) {
    return QString("%1G").arg(qreal(size / 107374182) / 10);
  } else if (size >= 2097152) {
    return QString("%1M").arg(size / 1048576);
  } else if (size >= 1048576) {
    return QString("%1M").arg(qreal(size / 104858) / 10);
  } else if (size >= 2048) {
    return QString("%1K").arg(size / 1024);
  } else if (size >= 1000) {
    return QString("%1K").arg(qreal(size / 102) / 10);
  } else {
    return QString("%1").arg(size);
  }
}

QPoint CRouterBRouterTilesSelect::tileFromFileName(const QString& fileName) {
  const QRegularExpressionMatch& match = regExpTileName.match(fileName);
  if (match.hasMatch()) {
    return QPoint(match.captured(2).toInt() * (match.captured(1) == "E" ? 1 : -1),
                  match.captured(4).toInt() * (match.captured(3) == "N" ? 1 : -1));
  } else {
    return noTile;
  }
}

QString CRouterBRouterTilesSelect::fileNameFromTile(const QPoint tile) {
  return QString("%1%2_%3%4.rd5")
      .arg(tile.x() < 0 ? "W" : "E")
      .arg(abs(tile.x()))
      .arg(tile.y() < 0 ? "S" : "N")
      .arg(abs(tile.y()));
}

QDir CRouterBRouterTilesSelect::segmentsDir() const {
  return QDir(QDir(setup->localDir).absoluteFilePath(setup->localSegmentsDir));
}

void CRouterBRouterTilesSelect::slotDownload() {
  bool changed(false);

  for (QHash<QString, CRouterBRouterTilesStatus*>::const_iterator it = tilesDownloadStatus.constBegin();
       it != tilesDownloadStatus.constEnd(); ++it) {
    CRouterBRouterTilesStatus* status = it.value();
    if (status->isSelected && (status->isOutdated || !status->isLocal) && status->file == nullptr) {
      const QString& fileName = it.key();

      const QDir& dir = segmentsDir();
      if (!dir.exists()) {
        error(tr("segments directory does not exist: ").arg(dir.path()));
        break;
      }

      status->isSelected = false;
      changed = true;

      status->file = new QFile(dir.absoluteFilePath(fileName + ".tmp"));
      if (!status->file->open(QIODevice::WriteOnly)) {
        const QString tmpName = status->file->fileName();
        const QString tmpError = status->file->errorString();
        delete status->file;
        status->file = nullptr;
        error(tr("error creating file %1: %2").arg(tmpName, tmpError));
        break;
      }

      QUrl url(setup->getSegmentsUrl());
      QString path = url.path();
      if (!path.endsWith('/')) {
        path.append("/");
      }
      path.append(fileName);
      url.setPath(path);

      QNetworkReply* reply = tilesDownloadManager->get(QNetworkRequest(url));
      reply->setProperty("tile", fileName);

      tilesDownloadManagerReplies << reply;

      connect(reply, &QNetworkReply::downloadProgress, status, &CRouterBRouterTilesStatus::slotUpdateProgress);
      connect(reply, &QNetworkReply::readyRead, this, &CRouterBRouterTilesSelect::slotDownloadReadReady);
    }
  }

  if (changed) {
    update();
  }
}

void CRouterBRouterTilesSelect::slotDownloadReadReady() {
  for (QNetworkReply* reply : std::as_const(tilesDownloadManagerReplies)) {
    if (reply->bytesAvailable() > 0) {
      try {
        const QString& fileName = reply->property("tile").toString();
        QHash<QString, CRouterBRouterTilesStatus*>::const_iterator it = tilesDownloadStatus.constFind(fileName);
        if (it == tilesDownloadStatus.constEnd()) {
          throw tr("no valid request for filename %1").arg(fileName);
        }
        CRouterBRouterTilesStatus* status = it.value();
        if (status->file == nullptr) {
          throw tr("no open file assigned to request for %1").arg(fileName);
        }
        if (status->file->write(reply->readAll()) < 0) {
          QString tmpName = status->file->fileName();
          QString msg = status->file->errorString();
          status->file->close();
          status->file->remove();
          delete status->file;
          status->file = nullptr;
          throw tr("error writing to file %1: %2").arg(tmpName, msg);
        }
      } catch (const QString& msg) {
        reply->abort();
        error(msg);
      }
    }
  }
  updateStatus();
}

void CRouterBRouterTilesSelect::slotDownloadFinished(QNetworkReply* reply) {
  if (tilesDownloadManagerReplies.contains(reply)) {
    tilesDownloadManagerReplies.remove(tilesDownloadManagerReplies.indexOf(reply));
  }
  reply->deleteLater();

  const QString& fileName = reply->property("tile").toString();
  QHash<QString, CRouterBRouterTilesStatus*>::const_iterator it = tilesDownloadStatus.constFind(fileName);
  if (it == tilesDownloadStatus.constEnd()) {
    error(tr("no valid request for filename %1").arg(fileName));
  } else {
    CRouterBRouterTilesStatus* status = it.value();
    status->isLocal = false;
    if (status->file == nullptr) {
      error(tr("no open file assigned to request for %1").arg(fileName));
    } else {
      if (reply->error() != QNetworkReply::NoError) {
        error(fileName + ": " + reply->errorString());
      } else if (status->file->write(reply->readAll()) < 0) {
        error(tr("error writing to file %1: %2").arg(status->file->fileName(), status->file->errorString()));
        status->file->close();
        status->file->remove();
      } else {
        const QString& finalName = segmentsDir().absoluteFilePath(it.key());
        QFile oldFile(finalName);
        if (oldFile.exists()) {
          oldFile.remove();
        }
        if (status->file->rename(finalName)) {
          status->isLocal = true;
          status->isOutdated = false;
          status->file->close();
          QFileInfo info(*status->file);
          status->localDate = info.birthTime();
          status->localSize = info.size();
          clearError();
        } else {
          error(tr("error renaming file %1 to %2: %3")
                    .arg(status->file->fileName(), finalName, status->file->errorString()));
          status->file->close();
          status->file->remove();
        }
      }
      delete status->file;
      status->file = nullptr;
    }
  }
  update();
}

void CRouterBRouterTilesSelect::cancelDownload() const {
  for (QNetworkReply* reply : tilesDownloadManagerReplies) {
    reply->abort();
  }
}

void CRouterBRouterTilesSelect::update() {
  updateButtons();
  updateStatus();
  updateTiles();
}

void CRouterBRouterTilesSelect::updateStatus() {
  int numCurrent(0);
  int sizeCurrent(0);
  int numOutdated(0);
  int sizeOutdated(0);
  int numOutstanding(0);
  int sizeDownloadMax(0);
  int sizeOutstanding(0);
  int sizeDownloaded(0);

  bool downloading(false);

  for (QHash<QString, CRouterBRouterTilesStatus*>::const_iterator it = tilesDownloadStatus.constBegin();
       it != tilesDownloadStatus.constEnd(); ++it) {
    const CRouterBRouterTilesStatus* status = it.value();

    if (status->file != nullptr && status->progressMax > 0) {
      numOutstanding++;
      sizeDownloadMax += status->progressMax;
      sizeOutstanding += status->progressMax - status->progressVal;
      sizeDownloaded += status->progressVal;
      downloading = true;
    } else if (status->isSelected && status->isRemote && (!status->isLocal || status->isOutdated)) {
      numOutstanding++;
      sizeOutstanding += status->remoteSize;
    }
    if (status->isOutdated) {
      numOutdated++;
      sizeOutdated += status->localSize;
    } else if (status->isLocal) {
      numCurrent++;
      sizeCurrent += status->localSize;
    }
  }

  statusLabel->setText(QString(tr("up-to-date: %1 (%2), outdated: %3 (%4), to be downloaded: %5 (%6)"))
                           .arg(numCurrent)
                           .arg(formatSize(sizeCurrent))
                           .arg(numOutdated)
                           .arg(formatSize(sizeOutdated))
                           .arg(numOutstanding)
                           .arg(formatSize(sizeOutstanding)));
  statusProgress->setVisible(downloading);
  statusProgress->setRange(0, sizeDownloadMax);
  statusProgress->setValue(sizeDownloaded);

  if (this->downloading != downloading || (numOutstanding > 0) != downloadSelected) {
    this->downloading = downloading;
    downloadSelected = numOutstanding > 0;
    emit sigCompleteChanged();
  }
}

void CRouterBRouterTilesSelect::error(const QString& error) const {
  errorLabel->setText(error);
  errorLabel->setVisible(true);
}

void CRouterBRouterTilesSelect::segmentsError(const QString& msg) const {
  error(tr("Error retrieving available routing data from %1: %2").arg(setup->getSegmentsUrl(), msg));
}

void CRouterBRouterTilesSelect::clearError() const { errorLabel->setVisible(false); }

CRouterBRouterTilesStatus* CRouterBRouterTilesSelect::getTileStatus(QPoint tile) const {
  QHash<QString, CRouterBRouterTilesStatus*>::const_iterator it = tilesDownloadStatus.constFind(fileNameFromTile(tile));
  if (it != tilesDownloadStatus.constEnd()) {
    return it.value();
  }
  return nullptr;
}

void CRouterBRouterTilesSelect::updateTiles() const {
  QVector<QPoint> gridTiles;
  QVector<QPoint> invalidTiles;
  QVector<QPoint> outdatedTiles;
  QVector<QPoint> currentTiles;
  QVector<QPoint> outstandingTiles;
  QVector<QPoint> selectedTiles;

  for (QHash<QString, CRouterBRouterTilesStatus*>::const_iterator it = tilesDownloadStatus.constBegin();
       it != tilesDownloadStatus.constEnd(); ++it) {
    const QPoint& tile = tileFromFileName(it.key());
    Q_ASSERT(tile != noTile);
    const CRouterBRouterTilesStatus* status = it.value();
    if (status->file != nullptr) {
      outstandingTiles << tile;
    } else if (status->isSelected) {
      selectedTiles << tile;
    } else if (status->isLocal) {
      if (status->isOutdated) {
        outdatedTiles << tile;
      } else {
        currentTiles << tile;
      }
    } else if (status->isRemote) {
      gridTiles << tile;
    } else {
      invalidTiles << tile;
    }
  }

  selectArea->setGridTiles(gridTiles);
  selectArea->setInvalidTiles(invalidTiles);
  selectArea->setOutdatedTiles(outdatedTiles);
  selectArea->setCurrentTiles(currentTiles);
  selectArea->setOutstandingTiles(outstandingTiles);
  selectArea->setSelectedTiles(selectedTiles);

  selectArea->update();
}

void CRouterBRouterTilesSelect::slotTileToolTipChanged(const QPoint& tile) const {
  const QString& format = QLocale().dateTimeFormat(QLocale::ShortFormat);
  const CRouterBRouterTilesStatus* status = getTileStatus(tile);

  if (status->file != nullptr) {
    selectArea->setTileToolTip(QString(tr("being downloaded: %1 of %2"))
                                   .arg(formatSize(status->progressVal), formatSize(status->progressMax)));
  } else if (status->isOutdated) {
    selectArea->setTileToolTip(QString(tr("local data outdated (%1, %2 - remote %3, %4)"))
                                   .arg(formatSize(status->localSize), status->localDate.toString(format),
                                        formatSize(status->remoteSize), status->remoteDate.toString(format)));
  } else if (status->isLocal) {
    selectArea->setTileToolTip(QString(tr("local data up to date (%1, %2)"))
                                   .arg(formatSize(status->localSize), status->localDate.toString(format)));
  } else if (status->isRemote) {
    selectArea->setTileToolTip(QString(tr("no local data, online available: %1 (%2)"))
                                   .arg(formatSize(status->remoteSize), status->remoteDate.toString(format)));
  } else {
    selectArea->setTileToolTip(tr("no routing-data available"));
  }
}
