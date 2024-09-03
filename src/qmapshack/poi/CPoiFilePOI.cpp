/**********************************************************************************************
    Copyright (C) 2020 Oliver Eichler <oliver.eichler@gmx.de>
    Copyright (C) 2021 Henri Hornburg <pingurus@t-online.de>

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

#include "poi/CPoiFilePOI.h"

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QtWidgets>

#include "helpers/CDraw.h"
#include "helpers/CTryMutexLocker.h"
#include "poi/CPoiCategory.h"
#include "poi/CPoiDraw.h"
#include "poi/CPoiIconCategory.h"
#include "poi/IPoiFile.h"
#include "poi/IPoiItem.h"

CPoiFilePOI::CPoiFilePOI(const QString& filename, CPoiDraw* parent)
    : IPoiFile(parent), filename(filename), loadTimer(new QTimer(this)) {
  // Set true if the file could be open and loaded successfully
  // If not set true the system will take care to destroy this object
  isActivated = true;

  // Set up a timer to only start reloading and redrawing the map when the user has made his selections
  loadTimer->setSingleShot(true);
  loadTimer->setInterval(500);
  connect(loadTimer, &QTimer::timeout, poi, &CPoiDraw::emitSigCanvasUpdate);

  // Open database here so it belongs to the right thread
  if (!QSqlDatabase::contains(filename + "_bbox")) {
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", filename + "_bbox");
    db.setDatabaseName(filename);
    if (!db.open()) {
      qDebug() << "failed to open database" << db.lastError();
      isActivated = false;
      return;
    }
  }

  QSqlQuery query("SELECT value FROM main.metadata WHERE name='bounds'", QSqlDatabase::database(filename + "_bbox"));

  if (query.next()) {
    const QStringList& corners = query.value(0).toString().split(",");
    qreal top = corners[2].toDouble();
    qreal left = corners[1].toDouble();
    qreal width = corners[3].toDouble() - left;
    qreal height = corners[0].toDouble() - top;
    bbox = QRectF(left, top, width, height);
  } else {
    qDebug() << "failed to retrieve bounding box of " << filename;
    isActivated = false;
  }
  // Database is no longer needed
  QSqlDatabase::removeDatabase(filename + "_bbox");
}

void CPoiFilePOI::draw(IDrawContext::buffer_t& buf) {
  // !!!! NOTE !!!!
  // This is running in it's own thread, not the main thread.
  // Use CPoiFilePOI::mutex whenever you access data shared between
  // the main thread and this thread. Do not block it more than
  // neccessary.

  // do that on a regular base while you draw the buffer
  // to abort drawing ass soon as possible if there is
  // another draw request pending
  if (poi->needsRedraw()) {
    return;
  }

  QPainter p(&buf.image);
  USE_ANTI_ALIASING(p, true);
  p.setOpacity(getOpacity() / 100.0);

  QPointF pp = buf.ref1;
  poi->convertRad2Px(pp);
  p.translate(-pp);
  p.setBrush(Qt::white);
  p.setPen(Qt::NoPen);

  // calculate maximum viewport
  qreal xMin = qMin(buf.ref1.x(), buf.ref4.x());
  qreal xMax = qMax(buf.ref2.x(), buf.ref3.x());
  qreal yMin = qMin(buf.ref3.y(), buf.ref4.y());
  qreal yMax = qMax(buf.ref1.y(), buf.ref2.y());

  if (xMin < -180.0 * DEG_TO_RAD) {
    xMin = -180 * DEG_TO_RAD;
  }
  if (xMax > 180.0 * DEG_TO_RAD) {
    xMax = 180 * DEG_TO_RAD;
  }

  // draw POI
  QMutexLocker lock(&mutex);
  displayedPois.clear();
  QRectF freeSpaceRect(QPointF(), IPoiFile::iconSize() * 2);
  // Find POIs in view
  const QList<quint64>& keys = categoryActivated.keys();
  for (quint64 categoryID : keys) {
    if (categoryActivated[categoryID] != Qt::Checked) {
      continue;
    }
    for (int minLonM10 = qFloor(xMin * RAD_TO_DEG * 10); minLonM10 < qCeil(xMax * RAD_TO_DEG * 10); minLonM10++) {
      for (int minLatM10 = qFloor(yMin * RAD_TO_DEG * 10); minLatM10 < qCeil(yMax * RAD_TO_DEG * 10); minLatM10++) {
        if (!loadedPoisByArea.contains(categoryID) || !loadedPoisByArea[categoryID].contains(minLonM10) ||
            !loadedPoisByArea[categoryID][minLonM10].contains(minLatM10)) {
          loadPOIsFromFile(categoryID, minLonM10, minLatM10);
        }
        if (poi->needsRedraw()) {
          return;
        }
        for (quint64 poiToDrawID : std::as_const(loadedPoisByArea)[categoryID][minLonM10][minLatM10]) {
          const CPoiItemPOI& poiToDraw = loadedPois[poiToDrawID];
          QPointF pt = poiToDraw.getCoordinates();
          poi->convertRad2Px(pt);

          freeSpaceRect.moveCenter(pt);

          bool foundIntersection = false;
          for (poiGroup_t& poiGroup : displayedPois) {
            if (poiGroup.iconLocation.intersects(freeSpaceRect)) {
              foundIntersection = true;
              poiGroup.pois.insert(poiToDrawID);
              break;
            }
          }

          if (!foundIntersection) {
            poiGroup_t poiGroup;
            QRectF iconRect(QPointF(), IPoiFile::iconSize());
            iconRect.moveCenter(pt);
            poiGroup.iconLocation = iconRect;
            poiGroup.iconCenter = poiToDraw.getCoordinates();
            poiGroup.pois.insert(poiToDrawID);
            displayedPois.append(poiGroup);
          }
        }
      }
    }
  }

  // Draw Icons
  for (const poiGroup_t& poiGroup : std::as_const(displayedPois)) {
    QFontMetricsF fm(CMainWindow::self().getMapFont());

    QPixmap icon;
    getPoiIcon(icon, poiGroup);
    icon = icon.scaled(IPoiFile::iconSize(), Qt::KeepAspectRatio, Qt::SmoothTransformation);

    const QRectF& iconLocation = poiGroup.iconLocation;

    p.drawRect(iconLocation.adjusted(-1, -1, 1, 1));
    p.drawPixmap(iconLocation.topLeft(), icon);
    p.save();
    if (poiGroup.pois.count() > 1) {
      const QString& text = QString::number(poiGroup.pois.count());

      QRectF labelRect = fm.boundingRect(text);
      labelRect.moveCenter(iconLocation.bottomRight());
      p.drawRect(labelRect.adjusted(-1, -1, 1, 1));
      CDraw::text(text, p, labelRect.toRect(), Qt::darkBlue);
    } else if (CMainWindow::self().isPoiText()) {
      // Draw Name
      const QString& name = loadedPois[*poiGroup.pois.begin()].getName();
      QRectF rect = fm.boundingRect(name);
      rect.adjust(-2, -2, 2, 2);

      bool noLocationFound = false;
      // place label on bottom
      rect.moveCenter(iconLocation.center());
      rect.moveTop(iconLocation.bottom());
      if (overlapsWithIcon(rect)) {
        // place label on top
        rect.moveBottom(iconLocation.top());
        if (overlapsWithIcon(rect)) {
          // place label on right
          rect.moveCenter(iconLocation.center());
          rect.moveLeft(iconLocation.right());
          if (overlapsWithIcon(rect)) {
            // place label on left
            rect.moveRight(iconLocation.left());
            if (overlapsWithIcon(rect)) {
              // failed to place label anywhere
              noLocationFound = true;
            }
          }
        }
      }
      if (!noLocationFound) {
        CDraw::text(name, p, rect.toRect(), Qt::darkBlue);
      }
    }
    p.restore();
  }
}

bool CPoiFilePOI::findPoiCloseBy(const QPoint& px, QSet<IPoiItem>& poiItems, QList<QPointF>& posPoiHighlight) const {
  CTryMutexLocker<QRecursiveMutex> lock(mutex);
  if (!lock.try_lock()) {
    return false;
  }

  poiGroup_t poiGroup;
  if (getPoiGroupCloseBy(px, poiGroup)) {
    for (quint64 key : std::as_const(poiGroup.pois)) {
      poiItems.insert(loadedPois[key].toPoi());
    }
    posPoiHighlight.append(poiGroup.iconCenter);
    return true;
  }
  return false;
}

void CPoiFilePOI::findPoisIn(const QRectF& degRect, QSet<IPoiItem>& pois, QList<QPointF>& posPoiHighlight) {
  CTryMutexLocker<QRecursiveMutex> lock(mutex);
  if (!lock.try_lock()) {
    return;
  }

  // Treat highlighting and POIs seperately, as highlighting only applies to items in the current view

  // Find POIs
  QSet<quint64> copiedItems;  // Some Items may appear in multiple categories. We only want to copy those once.
  const QList<quint64>& keys = categoryActivated.keys();
  for (quint64 categoryID : keys) {
    if (categoryActivated[categoryID] != Qt::Checked) {
      continue;
    }
    for (int minLonM10 = qFloor(degRect.left() * 10); minLonM10 <= qFloor(degRect.right() * 10); minLonM10++) {
      for (int minLatM10 = qFloor(degRect.bottom() * 10); minLatM10 <= qFloor(degRect.top() * 10); minLatM10++) {
        // Imagine the user moves the screen in an l-shape while updating the selection rectangle. It is possible that
        // some tiles are not laded then
        if (!loadedPoisByArea.contains(categoryID) || !loadedPoisByArea[categoryID].contains(minLonM10) ||
            !loadedPoisByArea[categoryID][minLonM10].contains(minLatM10)) {
          loadPOIsFromFile(categoryID, minLonM10, minLatM10);
        }
        for (quint64 poiFoundID : std::as_const(loadedPoisByArea)[categoryID][minLonM10][minLatM10]) {
          const CPoiItemPOI& poiItemFound = loadedPois[poiFoundID];
          if (!copiedItems.contains(poiItemFound.getKey())) {
            // Maybe look through the whole code of selecting items from a map to avoid this conversion
            if (degRect.contains(poiItemFound.getCoordinates() * RAD_TO_DEG)) {
              pois.insert(poiItemFound.toPoi());
              copiedItems.insert(poiItemFound.getKey());
            }
          }
        }
      }
    }
  }

  // Find Highlights
  for (const poiGroup_t& poiGroup : std::as_const(displayedPois)) {
    if (degRect.contains(poiGroup.iconCenter * RAD_TO_DEG)) {
      posPoiHighlight.append(poiGroup.iconCenter);
    }
  }
}

bool CPoiFilePOI::getToolTip(const QPoint& px, QString& str) const {
  CTryMutexLocker<QRecursiveMutex> lock(mutex);
  if (!lock.try_lock()) {
    return false;
  }

  poiGroup_t poiGroup;
  bool success = getPoiGroupCloseBy(px, poiGroup);
  if (success) {
    if (poiGroup.pois.count() == 1) {
      const CPoiItemPOI& poiFound = loadedPois[*poiGroup.pois.begin()];
      const QString& name = poiFound.getName(false);
      if (!name.isEmpty()) {
        str += "<b>" + name + "</b><br>\n";
      }
      str += tr("Category: ") + "<b>" + poiFound.getCategory() + "</b><br>\n";
      str += poiFound.getDesc();
      str += "<br>\n";
      const QList<IGisItem::link_t>& links = poiFound.getLinks();
      if (!links.isEmpty()) {
        str += tr("Links: ");
        bool isFirstLink = true;
        for (const IGisItem::link_t& link : links) {
          if (isFirstLink) {
            isFirstLink = false;
          } else {
            str += ", ";
          }
          str += link.text;
        }
      }
    } else {
      str += "<i>" + tr("Zoom in to see more details.") + "</i>";
      if (poiGroup.pois.count() <= 10) {
        str += "<br>\n" + tr("POIs at this point:");
        for (quint64 poiID : std::as_const(poiGroup.pois)) {
          str += "<br>\n<b>" + loadedPois[poiID].getName() + "</b>";
        }
      }
    }
  }
  return success;
}

void CPoiFilePOI::addTreeWidgetItems(QTreeWidget* widget) {
  QMutexLocker lock(&mutex);

  // Open database here so it belongs to the right thread
  if (!QSqlDatabase::contains(filename + "_widget")) {
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", filename + "_widget");
    db.setDatabaseName(filename);
    if (!db.open()) {
      qDebug() << "failed to open database" << db.lastError();
      isActivated = false;
      return;
    }
  }

  QSqlQuery query("SELECT id, name, parent FROM main.poi_categories ORDER BY id DESC",
                  QSqlDatabase::database(filename + "_widget"));

  QMap<uint, CPoiCategory*> categoryMap;

  while (query.next()) {
    bool hasParent = false;
    quint64 parentID = query.value(eSqlColumnCategoryParent).toUInt(&hasParent);
    if (!hasParent) {
      continue;  // This is the root item
    }
    const QString& categoryName = query.value(eSqlColumnCategoryName).toString();
    quint64 categoryID = query.value(eSqlColumnCategoryId).toUInt();
    categoryNames[categoryID] = categoryName;
    // This assumes, that the parent categories always have a higher ID than their children. This seems to be the case
    // for all Mapsforge poi files
    if (categoryMap.contains(parentID)) {
      categoryMap[categoryID] =
          new CPoiCategory(categoryName, categoryID, categoryActivated[categoryID], categoryMap[parentID]);
    } else {
      categoryMap[categoryID] = new CPoiCategory(categoryName, categoryID, widget);
    }
  }
}

void CPoiFilePOI::slotCheckedStateChanged(QTreeWidgetItem* item) {
  QMutexLocker lock(&mutex);

  CPoiCategory* categoryItem = static_cast<CPoiCategory*>(item);
  if (categoryItem == nullptr) {
    return;
  }

  categoryActivated[categoryItem->getId()] = categoryItem->checkState();

  loadTimer->start();
}

void CPoiFilePOI::getPoiIcon(QPixmap& icon, const CPoiFilePOI::poiGroup_t& poiGroup) {
  if (poiGroup.pois.count() > 1) {
    icon = QPixmap("://icons/poi/SJJB/png/poi_point_of_interest.n.32.png");
  } else {
    getPoiIcon(icon, loadedPois[*poiGroup.pois.begin()]);
  }
}

void CPoiFilePOI::getPoiIcon(QPixmap& icon, const CPoiItemPOI& poi, const QString& definingTag) {
  if (!definingTag.isEmpty() && tagMap.contains(definingTag)) {
    icon = tagMap[definingTag].getIcon(poi.getRawData());
    return;
  }
  for (const QString& tag : poi.getRawData()) {
    if (tagMap.contains(tag)) {
      icon = tagMap[tag].getIcon(poi.getRawData());
      return;
    }
  }
  icon = QPixmap("://icons/poi/SJJB/png/poi_point_of_interest.n.32.png");
}

bool CPoiFilePOI::overlapsWithIcon(const QRectF& rect) const {
  for (const poiGroup_t& poiGroup : displayedPois) {
    if (poiGroup.iconLocation.intersects(rect)) {
      return true;
    }
  }
  return false;
}

bool CPoiFilePOI::getPoiGroupCloseBy(const QPoint& px, CPoiFilePOI::poiGroup_t& poiItem) const {
  for (const poiGroup_t& poiGroup : displayedPois) {
    if (poiGroup.iconLocation.contains(px)) {
      poiItem = poiGroup;
      return true;
    }
  }
  return false;
}

void CPoiFilePOI::loadPOIsFromFile(quint64 categoryID, int minLonM10, int minLatM10) {
  QMutexLocker lock(&mutex);

  // check if query is within bounds
  if (!bbox.intersects({minLonM10 / 10.0, minLatM10 / 10.0, 0.1, 0.1})) {
    return;
  }

  // Open Database here so it is owned by the right thread
  if (!QSqlDatabase::contains(filename)) {
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", filename);
    db.setDatabaseName(filename);
    if (!db.open()) {
      qDebug() << "failed to open database" << db.lastError();
      isActivated = false;
      return;
    }
  }

  QSqlQuery query(QSqlDatabase::database(filename));
  query.prepare(
      "SELECT main.poi_index.maxLat, main.poi_index.maxLon, main.poi_index.minLat, main.poi_index.minLon, "
      "main.poi_data.data, main.poi_data.id "
      "FROM main.poi_data, main.poi_index "
      "WHERE main.poi_data.id IN "
      "("
      "    SELECT main.poi_category_map.id "
      "    FROM main.poi_category_map "
      "    WHERE main.poi_category_map.id IN "
      "    ( "
      "        SELECT main.poi_index.id "
      "        FROM main.poi_index "
      "        WHERE main.poi_index.maxLat<:maxLat "
      "        AND main.poi_index.minLat>=:minLat "
      "        AND main.poi_index.maxLon<:maxLon "
      "        AND main.poi_index.minLon>=:minLon "
      "    ) "
      "    AND main.poi_category_map.category=:categoryID "
      ") "
      "AND main.poi_data.id = main.poi_index.id");
  query.bindValue(":maxLat", QString::number((minLatM10 + 1) / 10., 'f'));
  query.bindValue(":minLat", QString::number(minLatM10 / 10., 'f'));
  query.bindValue(":maxLon", QString::number((minLonM10 + 1) / 10., 'f'));
  query.bindValue(":minLon", QString::number(minLonM10 / 10., 'f'));
  query.bindValue(":categoryID", categoryID);
  query.exec();
  while (query.next()) {
    quint64 key = query.value(eSqlColumnPoiId).toUInt();
    const QStringList& data = query.value(eSqlColumnPoiData).toString().split("\r");
    QString garminIcon;
    for (const QString& tag : data) {
      if (tagMap.contains(tag)) {
        garminIcon = tagMap[tag].getGarminSym();
        break;
      }
    }
    loadedPoisByArea[categoryID][minLonM10][minLatM10].append(key);
    // TODO: this overwrites a POI if it already was loaded. The difference between those will be the category. Some
    // better handling should be done
    loadedPois[key] = CPoiItemPOI(
        data,
        QPointF((query.value(eSqlColumnPoiMaxLon).toDouble() + query.value(eSqlColumnPoiMinLon).toDouble()) / 2 *
                    DEG_TO_RAD,
                (query.value(eSqlColumnPoiMaxLat).toDouble() + query.value(eSqlColumnPoiMinLat).toDouble()) / 2 *
                    DEG_TO_RAD),
        key, categoryNames[categoryID], garminIcon);
  }
  // Close database, as this method is called from mutiple threads.
  QSqlDatabase::removeDatabase(filename);
}
