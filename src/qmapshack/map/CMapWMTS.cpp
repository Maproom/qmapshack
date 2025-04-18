/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler <oliver.eichler@gmx.de>

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

#include "map/CMapWMTS.h"

#include <ogr_spatialref.h>

#include <QtNetwork>
#include <QtWidgets>
#include <QtXml>

#include "CMainWindow.h"
#include "helpers/CDraw.h"
#include "map/CMapDraw.h"
#include "map/cache/CDiskCache.h"
#include "units/IUnit.h"

CMapWMTS::CMapWMTS(const QString& filename, CMapDraw* parent) : IMapOnline(parent) {
  qDebug() << "------------------------------";
  qDebug() << "WMTS: try to open" << filename;

  QFile file(filename);
  if (!file.open(QIODevice::ReadOnly)) {
    QMessageBox::critical(CMainWindow::getBestWidgetForParent(), tr("Error..."), tr("Failed to open %1").arg(filename),
                          QMessageBox::Abort, QMessageBox::Abort);
    return;
  }

  QDomDocument dom;
  const QDomDocument::ParseResult& result = dom.setContent(&file);
  if (!result) {
    file.close();
    QMessageBox::critical(CMainWindow::getBestWidgetForParent(), tr("Error..."),
                          tr("Failed to read: %1\nline %2, column %3:\n %4")
                              .arg(filename)
                              .arg(result.errorLine)
                              .arg(result.errorColumn)
                              .arg(result.errorMessage),
                          QMessageBox::Abort, QMessageBox::Abort);
    return;
  }
  file.close();

  // start to decode XML
  // validate content as WMTS capability sheet
  const QDomElement& xmlCapabilities = dom.documentElement();
  if (xmlCapabilities.tagName() != "Capabilities") {
    QMessageBox::critical(CMainWindow::getBestWidgetForParent(), tr("Error..."),
                          tr("Failed to read: %1\nUnknown structure.").arg(filename), QMessageBox::Abort,
                          QMessageBox::Abort);
    return;
  }
  const QDomNode& xmlServiceIdentification = xmlCapabilities.namedItem("ServiceIdentification");
  QString ServiceType = xmlServiceIdentification.firstChildElement("ServiceType").text();
  QString ServiceTypeVersion = xmlServiceIdentification.firstChildElement("ServiceTypeVersion").text();

  if (!ServiceType.contains("WMTS", Qt::CaseInsensitive) || ServiceTypeVersion != "1.0.0") {
    QMessageBox::critical(
        CMainWindow::getBestWidgetForParent(), tr("Error..."),
        tr("Unexpected service. '* WMTS 1.0.0' is expected. '%1 %2' is read.").arg(ServiceType, ServiceTypeVersion),
        QMessageBox::Abort, QMessageBox::Abort);
    return;
  }

  // read setup of all layers
  const QDomNode& xmlContents = xmlCapabilities.namedItem("Contents");
  const QDomNodeList& xmlLayers = xmlContents.toElement().elementsByTagName("Layer");
  const int N = xmlLayers.count();
  for (int n = 0; n < N; n++) {
    QString str;
    QStringList values;
    const QDomNode& xmlLayer = xmlLayers.at(n);
    layer_t layer;

    layer.title = xmlLayer.firstChildElement("Title").text();

    // read bounding box
    const QDomNode& xmlBoundingBox = xmlLayer.firstChildElement("WGS84BoundingBox");
    str = xmlBoundingBox.namedItem("LowerCorner").toElement().text();
    values = str.split(" ");

    QPointF bottomLeft;
    if (values.size() == 2) {
      bottomLeft = QPointF(values[0].toDouble(), values[1].toDouble());
    }

    str = xmlBoundingBox.namedItem("UpperCorner").toElement().text();
    values = str.split(" ");
    QPointF topRight;
    if (values.size() == 2) {
      topRight = QPointF(values[0].toDouble(), values[1].toDouble());
    }

    layer.boundingBox.setBottomLeft(bottomLeft);
    layer.boundingBox.setTopRight(topRight);

    const QDomNode& xmlStyle = xmlLayer.firstChildElement("Style");
    layer.styles << xmlStyle.namedItem("Identifier").toElement().text();

    const QDomNode& xmlTileMatrixSetLink = xmlLayer.firstChildElement("TileMatrixSetLink");
    layer.tileMatrixSet = xmlTileMatrixSetLink.namedItem("TileMatrixSet").toElement().text();

    // read limits if any
    const QDomNode& xmlTileMatrixSetLimits = xmlTileMatrixSetLink.firstChildElement("TileMatrixSetLimits");
    if (xmlTileMatrixSetLimits.isElement()) {
      const QDomNodeList& xmlTileMatrixLimits =
          xmlTileMatrixSetLimits.toElement().elementsByTagName("TileMatrixLimits");
      const int L = xmlTileMatrixLimits.count();
      for (int l = 0; l < L; l++) {
        const QDomNode& xmlTileMatrixLimit = xmlTileMatrixLimits.at(l);
        QString Identifier = xmlTileMatrixLimit.namedItem("TileMatrix").toElement().text();
        layer.limits[Identifier] = limit_t();
        limit_t& limit = layer.limits[Identifier];

        limit.minTileRow = xmlTileMatrixLimit.namedItem("MinTileRow").toElement().text().toInt();
        limit.maxTileRow = xmlTileMatrixLimit.namedItem("MaxTileRow").toElement().text().toInt();
        limit.minTileCol = xmlTileMatrixLimit.namedItem("MinTileCol").toElement().text().toInt();
        limit.maxTileCol = xmlTileMatrixLimit.namedItem("MaxTileCol").toElement().text().toInt();
      }
    }

    // read resource URL of layer and replace placeholders by information that is already available
    const QDomNode& xmlResourceURL = xmlLayer.firstChildElement("ResourceURL");
    const QDomNamedNodeMap& attr = xmlResourceURL.attributes();

    layer.resourceURL = attr.namedItem("template").nodeValue();
    layer.resourceURL = layer.resourceURL.replace("{style}", layer.styles[0], Qt::CaseInsensitive);
    layer.resourceURL = layer.resourceURL.replace("{TileMatrixSet}", layer.tileMatrixSet, Qt::CaseInsensitive);

    // read and replace dimensions in url string by default value
    const QDomNodeList& xmlDimensions = xmlLayer.toElement().elementsByTagName("Dimension");
    const int D = xmlDimensions.count();
    for (int d = 0; d < D; d++) {
      const QDomNode& xmlDimension = xmlDimensions.at(d);

      QString Identifier = xmlDimension.namedItem("Identifier").toElement().text();
      QString Default = xmlDimension.namedItem("Default").toElement().text();

      layer.resourceURL = layer.resourceURL.replace("{" + Identifier + "}", Default, Qt::CaseInsensitive);
    }

    if (!httpsCheck(layer.resourceURL)) {
      return;
    }

    // enable layer by default
    layer.enabled = true;
    layers << layer;
  }

  // Add custom headers
  const QDomElement& xmlRawHeader = xmlContents.firstChildElement("RawHeader");
  const QDomNodeList& xmlValues = xmlRawHeader.elementsByTagName("Value");
  const int H = xmlValues.count();
  for (qint32 n = 0; n < H; ++n) {
    const QDomNode& xmlValue = xmlValues.item(n);
    registerHeaderItem(xmlValue.attributes().namedItem("name").nodeValue(), xmlValue.toElement().text());
  }

  // if there is more than one layer the layer list in the properties widget has to be enabled.
  if (layers.size() > 1) {
    flagsFeature |= eFeatLayers;
  }

  // read setup of all tile matrices
  const QDomNodeList& xmlTileMatrixSets = xmlContents.childNodes();
  const int M = xmlTileMatrixSets.count();
  for (int m = 0; m < M; m++) {
    const QDomNode& xmlTileMatrixSet = xmlTileMatrixSets.at(m);
    if (xmlTileMatrixSet.nodeName() != "TileMatrixSet") {
      continue;
    }

    QString Identifier = xmlTileMatrixSet.namedItem("Identifier").toElement().text();
    tilesets[Identifier] = tileset_t();
    tileset_t& tileset = tilesets[Identifier];

    // read projection string
    QString str = xmlTileMatrixSet.namedItem("SupportedCRS").toElement().text();

    char* ptr1 = (char*)malloc(str.toLatin1().size() + 1);
    char* ptr2 = nullptr;

    strncpy(ptr1, str.toLatin1().data(), str.toLatin1().size() + 1);
    OGRSpatialReference oSRS;

    if (str.startsWith("EPSG")) {
      QStringList tokens = str.split(":");
      oSRS.importFromEPSG(tokens.last().toInt());
    } else {
      oSRS.importFromURN(ptr1);
    }
    oSRS.exportToWkt(&ptr2);

    qDebug() << ptr1 << ptr2;

    tileset.proj.init(ptr2, "EPSG:4326");

    free(ptr1);
    free(ptr2);

    if (!tileset.proj.isValid()) {
      QMessageBox::warning(CMainWindow::getBestWidgetForParent(), tr("Error..."),
                           tr("No georeference information found."));
      return;
    }

    // read information about all matrix levels
    const QDomNodeList& xmlTileMatrixN = xmlTileMatrixSet.toElement().elementsByTagName("TileMatrix");
    const int N = xmlTileMatrixN.count();
    for (int n = 0; n < N; n++) {
      QString str;
      QStringList values;
      const QDomNode& xmlTileMatrix = xmlTileMatrixN.at(n);
      QString Identifier = xmlTileMatrix.namedItem("Identifier").toElement().text();
      tileset.tilematrix[Identifier] = tilematrix_t();
      tilematrix_t& matrix = tileset.tilematrix[Identifier];

      str = xmlTileMatrix.namedItem("TopLeftCorner").toElement().text();
      values = str.split(" ");
      matrix.topLeft = QPointF(values[0].toDouble(), values[1].toDouble());
      matrix.scale = xmlTileMatrix.namedItem("ScaleDenominator").toElement().text().toDouble();
      matrix.tileWidth = xmlTileMatrix.namedItem("TileWidth").toElement().text().toInt();
      matrix.tileHeight = xmlTileMatrix.namedItem("TileHeight").toElement().text().toInt();
      matrix.matrixWidth = xmlTileMatrix.namedItem("MatrixWidth").toElement().text().toInt();
      matrix.matrixHeight = xmlTileMatrix.namedItem("MatrixHeight").toElement().text().toInt();
    }
  }
  // ----  done reading XML file

  // create default cache path from filename
  QFileInfo fi(filename);
  slotSetCachePath(QDir(CMapDraw::getCacheRoot()).absoluteFilePath(fi.completeBaseName()));

  name = fi.completeBaseName().replace("_", " ");

  isActivated = true;
}

void CMapWMTS::getLayers(QListWidget& list) {
  if (!mutex.tryLock(100)) {
    return;
  }

  list.clear();
  if (layers.size() > 1) {
    int i = 0;
    for (const layer_t& layer : std::as_const(layers)) {
      QListWidgetItem* item = new QListWidgetItem(layer.title, &list);
      bool enabled = layer.enabled;
      item->setData(Qt::UserRole, i++);
      item->setCheckState(enabled ? Qt::Checked : Qt::Unchecked);
    }

    connect(&list, &QListWidget::itemChanged, this, &CMapWMTS::slotLayersChanged);
  }
  mutex.unlock();
}

void CMapWMTS::saveConfig(QSettings& cfg) /* override */
{
  QMutexLocker lock(&mutex);

  IMapOnline::saveConfig(cfg);
  if (layers.size() < 2) {
    return;
  }

  // save indices of enabled layers
  QStringList enabled;
  for (int i = 0; i < layers.size(); i++) {
    if (layers[i].enabled) {
      enabled << QString::number(i);
    }
  }
  cfg.setValue("enabledLayers", enabled);
}

void CMapWMTS::loadConfig(QSettings& cfg) /* override */
{
  QMutexLocker lock(&mutex);

  IMapOnline::loadConfig(cfg);
  if (layers.size() < 2) {
    return;
  }

  QStringList enabled;
  // set all layers to disabled first
  for (int i = 0; i < layers.size(); i++) {
    layers[i].enabled = false;
    enabled << QString::number(i);
  }

  // enable layers stored in configuration
  enabled = cfg.value("enabledLayers", enabled).toStringList();
  for (const QString& str : std::as_const(enabled)) {
    int idx = str.toInt();
    if (idx < layers.size()) {
      layers[idx].enabled = true;
    }
  }
}

void CMapWMTS::slotLayersChanged(QListWidgetItem* item) {
  QMutexLocker lock(&mutex);

  bool isChecked = (item->checkState() == Qt::Checked);
  int idx = item->data(Qt::UserRole).toInt();
  if (idx < 0) {
    QListWidget* list = item->listWidget();
    list->blockSignals(true);

    for (int i = 0; i < layers.size(); i++) {
      list->item(i + 1)->setCheckState(isChecked ? Qt::Checked : Qt::Unchecked);
      layers[i].enabled = isChecked;
    }

    list->blockSignals(false);
  } else {
    layers[idx].enabled = isChecked;
  }

  map->emitSigCanvasUpdate();
}

void CMapWMTS::draw(IDrawContext::buffer_t& buf) /* override */
{
  QMutexLocker lock(&mutex);

  timeLastUpdate.start();
  urlQueue.clear();

  if (map->needsRedraw()) {
    return;
  }

  QPointF bufferScale = buf.scale * buf.zoomFactor;

  if (isOutOfScale(bufferScale)) {
    return;
  }

  // get pixel offset of top left buffer corner
  QPointF pp = buf.ref1;
  map->convertRad2Px(pp);

  // start to draw the map
  QPainter p(&buf.image);
  USE_ANTI_ALIASING(p, true);
  p.setOpacity(getOpacity() / 100.0);
  p.translate(-pp);

  // calculate maximum viewport
  qreal x1 = buf.ref1.x() < buf.ref4.x() ? buf.ref1.x() : buf.ref4.x();
  qreal y1 = buf.ref1.y() > buf.ref2.y() ? buf.ref1.y() : buf.ref2.y();

  qreal x2 = buf.ref2.x() > buf.ref3.x() ? buf.ref2.x() : buf.ref3.x();
  qreal y2 = buf.ref3.y() < buf.ref4.y() ? buf.ref3.y() : buf.ref4.y();

  if (x1 < -180.0 * DEG_TO_RAD) {
    x1 = -180 * DEG_TO_RAD;
  }
  if (x2 > 180.0 * DEG_TO_RAD) {
    x2 = 180 * DEG_TO_RAD;
  }

  QRectF viewport(QPointF(x1, y1) * RAD_TO_DEG, QPointF(x2, y2) * RAD_TO_DEG);

  // draw layers
  for (const layer_t& layer : std::as_const(layers)) {
    if (!layer.boundingBox.intersects(viewport) || !layer.enabled) {
      continue;
    }

    const tileset_t& tileset = tilesets[layer.tileMatrixSet];
    const QMap<QString, limit_t>& limits = layer.limits;

    // convert viewport to layer's coordinate system
    QPointF pt1(x1, y1);
    QPointF pt2(x2, y2);

    tileset.proj.transform(pt1, PJ_INV);
    tileset.proj.transform(pt2, PJ_INV);

    if (tileset.proj.isSrcLatLong()) {
      pt1 *= RAD_TO_DEG;
      pt2 *= RAD_TO_DEG;
    }

    // search matrix ID of tile level with best matching scale
    QString tileMatrixId;
    QPointF s1 = (pt2 - pt1) / QPointF(buf.image.width(), buf.image.height());
    qreal d = NOFLOAT;
    const QStringList& keys = tileset.tilematrix.keys();
    for (const QString& key : keys) {
      const tilematrix_t& tilematrix = tileset.tilematrix[key];
      qreal s2 = tilematrix.scale * 0.28e-3;

      if (qAbs(s2 - s1.x()) < d) {
        tileMatrixId = key;
        d = qAbs(s2 - s1.x());
      }
    }

    // get min/max col/row values for that level
    qint32 minRow, maxRow, minCol, maxCol;
    const tilematrix_t& tilematrix = tileset.tilematrix[tileMatrixId];
    if (!limits.isEmpty()) {
      if (limits.contains(tileMatrixId)) {
        const limit_t& limit = limits[tileMatrixId];
        minCol = limit.minTileCol;
        maxCol = limit.maxTileCol;
        minRow = limit.minTileRow;
        maxRow = limit.maxTileRow;
      } else {
        // layer has limits but not for the selected tileMatrixId -> skip layer
        continue;
      }
    } else {
      minCol = 0;
      maxCol = tilematrix.matrixWidth;
      minRow = 0;
      maxRow = tilematrix.matrixHeight;
    }

    // derive range of col/row to request tiles
    qreal xscale = tilematrix.scale * 0.28e-3;
    qreal yscale = -tilematrix.scale * 0.28e-3;

    qint32 col1 = qFloor((pt1.x() - tilematrix.topLeft.x()) / (xscale * tilematrix.tileWidth));
    qint32 row1 = qFloor((pt1.y() - tilematrix.topLeft.y()) / (yscale * tilematrix.tileHeight));
    qint32 col2 = qFloor((pt2.x() - tilematrix.topLeft.x()) / (xscale * tilematrix.tileWidth));
    qint32 row2 = qFloor((pt2.y() - tilematrix.topLeft.y()) / (yscale * tilematrix.tileHeight));

    if (col1 < minCol) {
      col1 = minCol;
    }
    if (col1 > maxCol) {
      col1 = maxCol;
    }
    if (row1 < minRow) {
      row1 = minRow;
    }
    if (row1 > maxRow) {
      row1 = maxRow;
    }

    if (col2 < minCol) {
      col2 = minCol;
    }
    if (col2 > maxCol) {
      col2 = maxCol;
    }
    if (row2 < minRow) {
      row2 = minRow;
    }
    if (row2 > maxRow) {
      row2 = maxRow;
    }

    // start to request tiles. draw tiles in cache, queue urls of tile yet to be requested
    for (qint32 row = row1; row <= row2; row++) {
      for (qint32 col = col1; col <= col2; col++) {
        QString url = layer.resourceURL;
        url = url.replace("{TileMatrix}", tileMatrixId, Qt::CaseInsensitive);
        url = url.replace("{TileRow}", QString::number(row), Qt::CaseInsensitive);
        url = url.replace("{TileCol}", QString::number(col), Qt::CaseInsensitive);

        if (diskCache->contains(url)) {
          QImage img;
          diskCache->restore(url, img);

          QPolygonF l;

          qreal xx1 = col * (xscale * tilematrix.tileWidth) + tilematrix.topLeft.x();
          qreal yy1 = row * (yscale * tilematrix.tileHeight) + tilematrix.topLeft.y();
          qreal xx2 = (col + 1) * (xscale * tilematrix.tileWidth) + tilematrix.topLeft.x();
          qreal yy2 = (row + 1) * (yscale * tilematrix.tileHeight) + tilematrix.topLeft.y();

          l << QPointF(xx1, yy1) << QPointF(xx2, yy1) << QPointF(xx2, yy2) << QPointF(xx1, yy2);

          tileset.proj.transform(l, PJ_FWD);

          drawTile(img, l, p);
        } else {
          urlQueue << url;
        }
      }
    }

    emit sigQueueChanged();
  }
}
