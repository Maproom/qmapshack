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

#include "map/CMapTMS.h"

#include <QtNetwork>
#include <QtQml>
#include <QtWidgets>
#include <QtXml>

#include "CMainWindow.h"
#include "gis/proj_x.h"
#include "helpers/CDraw.h"
#include "map/CMapDraw.h"
#include "map/cache/CDiskCache.h"

namespace {
// extend of web mercator per direction in (projected) meters
// meaning the web mercator projection goes from -C to +C in both axis
constexpr qreal C = 20037508.34;

// convert from web mercator meters to tile indices
QPoint toTile(const QPointF& p, int z) {
  int n = (1 << z);  // n tiles across the map
  // normalize into [0,1]
  qreal x = (1 + p.x() / C) / 2;
  qreal y = (1 - p.y() / C) / 2;

  // tile that contains p
  int x_tile = x * n;
  int y_tile = y * n;
  // clamp to valid range
  x_tile = std::clamp(x_tile, 0, n - 1);
  y_tile = std::clamp(y_tile, 0, n - 1);

  return QPoint(x_tile, y_tile);
};

// get web mercator meters for the NW corner of a tile
QPointF fromTile(const QPoint& p, int z) {
  int n = (1 << z);
  // get normalized again
  qreal x = static_cast<qreal>(p.x()) / n;
  qreal y = static_cast<qreal>(p.y()) / n;

  return QPointF((2 * x - 1) * C, (2 * y - 1) * -C);
};

// euclidian modulo
int eucmod(int a, int b) {
  int r = a % b;
  return r >= 0 ? r : r + std::abs(b);
};
}  // namespace

CMapTMS::CMapTMS(const QString& filename, CMapDraw* parent) : IMapOnline(parent) {
  qDebug() << "------------------------------";
  qDebug() << "TMS: try to open" << filename;

  proj.init("EPSG:3857", "EPSG:4326");

  qDebug() << "tms:" << proj.getProjSrc();

  QFile file(filename);
  if (!file.open(QIODevice::ReadOnly)) {
    QMessageBox::critical(CMainWindow::getBestWidgetForParent(), tr("Error..."), tr("Failed to open %1").arg(filename),
                          QMessageBox::Abort, QMessageBox::Abort);
    return;
  }

  QDomDocument dom;
  const QDomDocument::ParseResult& result = dom.setContent(&file, QDomDocument::ParseOption::UseNamespaceProcessing);
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

  const QDomElement& xmlTms = dom.firstChildElement("TMS");
  name = xmlTms.firstChildElement("Title").text();
  copyright = xmlTms.firstChildElement("Copyright").text();

  if (xmlTms.firstChildElement("MaxZoomLevel").isElement()) {
    maxZoomLevel = xmlTms.firstChildElement("MaxZoomLevel").text().toInt();
  }

  if (xmlTms.firstChildElement("MinZoomLevel").isElement()) {
    minZoomLevel = xmlTms.firstChildElement("MinZoomLevel").text().toInt();
  }

  const QDomNodeList& xmlLayers = xmlTms.elementsByTagName("Layer");
  qint32 N = xmlLayers.count();
  layers.resize(N);

  for (qint32 n = 0; n < N; ++n) {
    const QDomNode& xmlLayer = xmlLayers.item(n);
    int idx = xmlLayer.attributes().namedItem("idx").nodeValue().toInt();

    if (idx >= layers.count()) {
      QMessageBox::critical(CMainWindow::getBestWidgetForParent(), tr("Error..."),
                            tr("Malformed TMS file. The layer numbers do not index "
                               "the available layers. Layer numbers start from index 0."),
                            QMessageBox::Abort);
      return;
    }

    layers[idx].strUrl = xmlLayer.namedItem("ServerUrl").toElement().text();
    layers[idx].script = xmlLayer.namedItem("Script").toElement().text();
    layers[idx].minZoomLevel = minZoomLevel;
    layers[idx].maxZoomLevel = maxZoomLevel;
    layers[idx].tileSizePx = 256;

    layers[idx].strUrl.replace("{z}", "%1", Qt::CaseInsensitive);
    layers[idx].strUrl.replace("{x}", "%2", Qt::CaseInsensitive);
    layers[idx].strUrl.replace("{y}", "%3", Qt::CaseInsensitive);

    if (xmlLayer.namedItem("Title").isElement()) {
      layers[idx].title = xmlLayer.namedItem("Title").toElement().text();
    } else {
      layers[idx].title = tr("Layer %1").arg(idx + 1);
    }

    if (xmlLayer.firstChildElement("MinZoomLevel").isElement()) {
      layers[idx].minZoomLevel = xmlLayer.firstChildElement("MinZoomLevel").text().toInt();
    }

    if (xmlLayer.firstChildElement("MaxZoomLevel").isElement()) {
      layers[idx].maxZoomLevel = xmlLayer.firstChildElement("MaxZoomLevel").text().toInt();
    }

    if (!httpsCheck(layers[idx].strUrl)) {
      return;
    }
  }

  const QDomElement& xmlRawHeader = xmlTms.firstChildElement("RawHeader");
  const QDomNodeList& xmlValues = xmlRawHeader.elementsByTagName("Value");
  N = xmlValues.count();
  for (qint32 n = 0; n < N; ++n) {
    const QDomNode& xmlValue = xmlValues.item(n);
    registerHeaderItem(xmlValue.attributes().namedItem("name").nodeValue(), xmlValue.toElement().text());
  }

  // if there is more than one layer the layer list in the properties widget has to be enabled.
  if (layers.size() > 1) {
    flagsFeature |= eFeatLayers;
  }

  // create default cache path from filename
  QFileInfo fi(filename);
  slotSetCachePath(QDir(CMapDraw::getCacheRoot()).absoluteFilePath(fi.completeBaseName()));

  name = fi.completeBaseName().replace("_", " ");

  isActivated = true;
}

void CMapTMS::getLayers(QListWidget& list) /* override */
{
  if (!mutex.tryLock(100)) {
    return;
  }

  list.clear();
  if (layers.size() > 1) {
    int i = 0;
    for (const layer_t& layer : std::as_const(layers)) {
      QListWidgetItem* item = new QListWidgetItem(layer.title, &list);
      int enabled = layer.enabled;
      item->setData(Qt::UserRole, i++);
      item->setCheckState(enabled ? Qt::Checked : Qt::Unchecked);
    }

    connect(&list, &QListWidget::itemChanged, this, &CMapTMS::slotLayersChanged);
  }
  mutex.unlock();
}

void CMapTMS::saveConfig(QSettings& cfg) /* override */
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

void CMapTMS::loadConfig(QSettings& cfg) {
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

void CMapTMS::slotLayersChanged(QListWidgetItem* item) {
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

QString CMapTMS::createUrl(const layer_t& layer, int x, int y, int z) {
  QMutexLocker lock(&mutex);

  if (layer.strUrl.startsWith("script")) {
    QString filename = layer.strUrl.mid(9);
    QFile scriptFile(filename);
    if (!scriptFile.open(QIODevice::ReadOnly)) {
      return "";
    }
    QTextStream stream(&scriptFile);
    QString contents = stream.readAll();
    scriptFile.close();

    QJSEngine engine;
    QJSValue fun = engine.evaluate(contents, filename);

    if (fun.isError()) {
      qDebug() << "Uncaught exception at line" << fun.property("lineNumber").toInt() << ":" << fun.toString();
    }

    QJSValueList args;
    args << z << x << y;
    QJSValue res = fun.call(args);
    return res.toString();
  } else if (!layer.script.isEmpty()) {
    QJSEngine engine;
    QJSValue fun = engine.evaluate(layer.script);

    QJSValueList args;
    args << z << x << y;
    QJSValue res = fun.call(args);
    return res.toString();
  }

  return layer.strUrl.arg(z).arg(x).arg(y);
}

void CMapTMS::draw(IDrawContext::buffer_t& buf) /* override */
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

  // get the top-left and bottom-right corners into web mercator
  auto r1 = buf.ref1;
  auto r3 = buf.ref3;
  proj.transform(r1, PJ_INV);
  proj.transform(r3, PJ_INV);
  QPointF pt1(r1.x(), r1.y());
  QPointF pt2(r3.x(), r3.y());

  // start to draw the map
  QPainter p(&buf.image);
  USE_ANTI_ALIASING(p, true);
  p.setOpacity(getOpacity() / 100.0);
  QPointF pp = buf.ref1;
  map->convertRad2Px(pp);
  p.translate(-pp);

  for (auto& layer : layers) {
    if (!layer.enabled) {
      continue;
    }

    // calculate zoom level
    qreal diff = pt2.x() - pt1.x();
    qreal width = diff >= 0 ? diff : diff + 2 * C;
    qint32 z = std::round(std::log2(2 * C / (width / buf.image.width() * layer.tileSizePx)));
    z = qMax(z, layer.minZoomLevel);
    if (z > layer.maxZoomLevel) {
      continue;
    }
    int n = (1 << z);

    // tile indices of the two corners and number of tiles between them
    auto t1 = toTile(pt1, z);
    auto t2 = toTile(pt2, z);
    auto dim = t2 - t1;
    dim = {eucmod(dim.x(), n), dim.y()};

    for (int i = 0; i <= dim.y(); ++i) {
      for (int j = 0; j <= dim.x(); ++j) {
        int x = eucmod(t1.x() + j, n);
        int y = t1.y() + i;
        QString url = createUrl(layer, x, y, z);

        if (diskCache->contains(url)) {
          QImage img;
          diskCache->restore(url, img);
          img.setDevicePixelRatio(buf.image.devicePixelRatio());
          if (img.width() != layer.tileSizePx) {
            // we got a tile with a different size then expected
            // (which is normal for the first tile we get from e.g. a HiDPI source)
            // remember it's size and request a redraw
            layer.tileSizePx = img.width();
            map->emitSigCanvasUpdate();
          }

          // TODO throw away drawTile and handle drawing in a sane way
          // then we'll also get wraparound on the antimeridian
          // (this function is ready but it's impossible with drawTile)
          QPolygonF l;
          l << fromTile({x, y}, z) << fromTile({x + 1, y}, z) << fromTile({x + 1, y + 1}, z) << fromTile({x, y + 1}, z);
          proj.transform(l, PJ_FWD);
          drawTile(img, l, p);
        } else {
          urlQueue << url;
        }
      }
    }
    emit sigQueueChanged();
  }
}
