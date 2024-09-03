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

#include "map/CMapVRT.h"

#include <gdal_priv.h>

#include <QtWidgets>

#include "CMainWindow.h"
#include "helpers/CDraw.h"
#include "map/CMapDraw.h"
#include "units/IUnit.h"

#define TILELIMIT 2500
#define TILESIZEX 64
#define TILESIZEY 64

CMapVRT::CMapVRT(const QString& filename, CMapDraw* parent) : IMap(eFeatVisibility, parent), filename(filename) {
  qDebug() << "------------------------------";
  qDebug() << "VRT: try to open" << filename;

  dataset = (GDALDataset*)GDALOpen(filename.toUtf8(), GA_ReadOnly);

  if (nullptr == dataset) {
    QMessageBox::warning(CMainWindow::getBestWidgetForParent(), tr("Error..."),
                         tr("Failed to load file: %1").arg(filename));
    return;
  }

  // ------- setup color table ---------
  rasterBandCount = dataset->GetRasterCount();
  if (rasterBandCount == 1) {
    GDALRasterBand* pBand = dataset->GetRasterBand(1);

    if (nullptr == pBand) {
      GDALClose(dataset);
      dataset = nullptr;
      QMessageBox::warning(CMainWindow::getBestWidgetForParent(), tr("Error..."),
                           tr("Failed to load file: %1").arg(filename));
      return;
    }

    if (pBand->GetColorInterpretation() == GCI_PaletteIndex) {
      GDALColorTable* pct = pBand->GetColorTable();
      for (int i = 0; i < pct->GetColorEntryCount(); ++i) {
        const GDALColorEntry& e = *pct->GetColorEntry(i);
        colortable << qRgba(e.c1, e.c2, e.c3, e.c4);
      }
    } else if (pBand->GetColorInterpretation() == GCI_GrayIndex) {
      for (int i = 0; i < 256; ++i) {
        colortable << qRgba(i, i, i, 255);
      }
    } else {
      GDALClose(dataset);
      dataset = nullptr;
      QMessageBox::warning(CMainWindow::getBestWidgetForParent(), tr("Error..."),
                           tr("File must be 8 bit palette or gray indexed."));
      return;
    }

    int success = 0;
    qreal idx = pBand->GetNoDataValue(&success);
    if (success) {
      if ((idx >= 0) && (idx < colortable.size())) {
        QColor tmp(colortable[idx]);
        tmp.setAlpha(0);
        colortable[idx] = tmp.rgba();
      } else {
        qDebug() << "Index for no data value is out of bound";
        return;
      }
    }
  }

  if (dataset->GetRasterCount() > 0) {
    hasOverviews = dataset->GetRasterBand(1)->GetOverviewCount() != 0;
  }

  // if the master VRT does not return a positive overview feedback
  // test all files combined by the VRT to have overviews.
  if (!hasOverviews) {
    qDebug() << "extended test for overviews";
    hasOverviews = testForOverviews(filename);
  }
  qDebug() << "has overviews" << hasOverviews;

  // ------- setup projection ---------------
  proj.init(dataset->GetProjectionRef(), "EPSG:4326");

  if (!proj.isValid()) {
    delete dataset;
    dataset = nullptr;
    QMessageBox::warning(CMainWindow::getBestWidgetForParent(), tr("Error..."),
                         tr("No georeference information found."));
    return;
  }

  xsize_px = dataset->GetRasterXSize();
  ysize_px = dataset->GetRasterYSize();

  qreal adfGeoTransform[6];
  dataset->GetGeoTransform(adfGeoTransform);

  xscale = adfGeoTransform[1];
  yscale = adfGeoTransform[5];
  xrot = adfGeoTransform[4];
  yrot = adfGeoTransform[2];

  trFwd.translate(adfGeoTransform[0], adfGeoTransform[3]);
  trFwd.scale(adfGeoTransform[1], adfGeoTransform[5]);

  if (adfGeoTransform[4] != 0.0) {
    trFwd.rotate(qAtan(adfGeoTransform[2] / adfGeoTransform[4]));
  }

  if (proj.isSrcLatLong()) {
    // convert to RAD to match internal notations
    trFwd = trFwd * DEG_TO_RAD;
  }

  trInv = trFwd.inverted();

  ref1 = trFwd.map(QPointF(0, 0));
  ref2 = trFwd.map(QPointF(xsize_px, 0));
  ref3 = trFwd.map(QPointF(xsize_px, ysize_px));
  ref4 = trFwd.map(QPointF(0, ysize_px));

  qDebug() << "FF" << trFwd;
  qDebug() << "RR" << trInv;

  isActivated = true;
}

CMapVRT::~CMapVRT() { GDALClose(dataset); }

bool CMapVRT::testForOverviews(const QString& filename) {
  QFile file(filename);
  if (!file.open(QIODevice::ReadOnly)) {
    qDebug() << "Failed to open" << filename;
    return false;
  }

  QDomDocument xml;
  QString msg;
  int line;
  int column;
  if (!xml.setContent(&file, false, &msg, &line, &column)) {
    file.close();
    throw tr("Failed to read: %1\nline %2, column %3:\n %4").arg(filename).arg(line).arg(column).arg(msg);
    qDebug() << "Failed to read:" << filename << Qt::endl << "line" << line << ", column" << column << Qt::endl << msg;
    return false;
  }
  file.close();

  QSet<QString> files;
  QDir basePath(QFileInfo(filename).absoluteDir());
  const QDomElement& xmlVrt = xml.documentElement();

  {
    const QDomNodeList& xmlComplexSources = xmlVrt.elementsByTagName("ComplexSource");
    const int N = xmlComplexSources.count();
    for (int n = 0; n < N; ++n) {
      const QDomNode& xmlComplexSource = xmlComplexSources.item(n);
      const QDomNode& xmlSourceFilename = xmlComplexSource.namedItem("SourceFilename");
      const QDomNamedNodeMap& attr = xmlSourceFilename.attributes();
      QString subFilename = xmlSourceFilename.toElement().text();

      if (attr.contains("relativeToVRT") && (attr.namedItem("relativeToVRT").nodeValue() == "1")) {
        subFilename = basePath.absoluteFilePath(subFilename);
      }

      files << subFilename;
    }
  }

  {
    const QDomNodeList& xmlSimpleSources = xmlVrt.elementsByTagName("SimpleSource");
    const int N = xmlSimpleSources.count();
    for (int n = 0; n < N; ++n) {
      const QDomNode& xmlSimpleSource = xmlSimpleSources.item(n);
      const QDomNode& xmlSourceFilename = xmlSimpleSource.namedItem("SourceFilename");
      const QDomNamedNodeMap& attr = xmlSourceFilename.attributes();
      QString subFilename = xmlSourceFilename.toElement().text();

      if (attr.contains("relativeToVRT") && (attr.namedItem("relativeToVRT").nodeValue() == "1")) {
        subFilename = basePath.absoluteFilePath(subFilename);
      }

      files << subFilename;
    }
  }

  if (files.isEmpty()) {
    return false;
  }

  for (const QString& file : std::as_const(files)) {
    using pGDALDataset = QSharedPointer<GDALDataset>;
    pGDALDataset _dataset = pGDALDataset((GDALDataset*)GDALOpen(file.toUtf8(), GA_ReadOnly), GDALClose);
    // _dataset will be destroyed automatically by shared pointer.

    if (_dataset == nullptr) {
      return false;
    }

    if (_dataset->GetRasterCount() > 0) {
      if (_dataset->GetRasterBand(1)->GetOverviewCount() == 0) {
        return false;
      }
    } else {
      return false;
    }
  }

  return true;
}

void CMapVRT::draw(IDrawContext::buffer_t& buf) /* override */
{
  if (map->needsRedraw()) {
    return;
  }

  QPointF bufferScale = buf.scale * buf.zoomFactor;

  // calculate bounding box;
  QPointF pt1 = ref1;
  QPointF pt2 = ref2;
  QPointF pt3 = ref3;
  QPointF pt4 = ref4;

  proj.transform(pt1, PJ_FWD);
  proj.transform(pt2, PJ_FWD);
  proj.transform(pt3, PJ_FWD);
  proj.transform(pt4, PJ_FWD);

  QPolygonF boundingBox;
  boundingBox << pt1 << pt2 << pt3 << pt4;
  map->convertRad2Px(boundingBox);

  // get pixel offset of top left buffer corner
  QPointF pp = buf.ref1;
  map->convertRad2Px(pp);

  // calculate area to read from file
  pt1 = buf.ref1;
  pt2 = buf.ref2;
  pt3 = buf.ref3;
  pt4 = buf.ref4;

  proj.transform(pt1, PJ_INV);
  proj.transform(pt2, PJ_INV);
  proj.transform(pt3, PJ_INV);
  proj.transform(pt4, PJ_INV);

  pt1 = trInv.map(pt1);
  pt2 = trInv.map(pt2);
  pt3 = trInv.map(pt3);
  pt4 = trInv.map(pt4);

  qreal left, right, top, bottom;
  left = pt1.x() < pt4.x() ? pt1.x() : pt4.x();
  right = pt2.x() > pt3.x() ? pt2.x() : pt3.x();
  top = pt1.y() < pt2.y() ? pt1.y() : pt2.y();
  bottom = pt4.y() > pt3.y() ? pt4.y() : pt3.y();

  if (left < 0) {
    left = 0;
  }
  if (left > xsize_px) {
    left = xsize_px;
  }

  if (top < 0) {
    top = 0;
  }
  if (top > ysize_px) {
    top = ysize_px;
  }

  if (right > xsize_px) {
    right = xsize_px;
  }
  if (right < 0) {
    right = 0;
  }

  if (bottom > ysize_px) {
    bottom = ysize_px;
  }
  if (bottom < 0) {
    bottom = 0;
  }

  qint32 imgw = TILESIZEX;
  qint32 imgh = TILESIZEY;
  qint32 dx = imgw;
  qint32 dy = imgh;

  // estimate number of tiles and use it as a limit if no
  // user defined limit is given
  qreal nTiles = ((right - left) * (bottom - top) / (dx * dy));
  if (hasOverviews) {
    // if there are overviews tiles can be reduced by reading
    // with a scale factor from file. Increase amount of pixel
    // read until tile limit is met.
    while (nTiles > TILELIMIT) {
      dx *= 2;
      dy *= 2;
      nTiles /= 4;
    }
  } else {
    nTiles = getMaxScale() == NOFLOAT ? nTiles : 0;
  }

  // start to draw the map
  QPainter p(&buf.image);
  USE_ANTI_ALIASING(p, true);
  p.setOpacity(getOpacity() / 100.0);
  p.translate(-pp);

  //    qDebug() << imgw << dx << nTiles;
  // limit number of tiles to keep performance
  if (!isOutOfScale(bufferScale) && (nTiles < TILELIMIT)) {
    for (qint32 y = top; y < bottom; y += dy) {
      if (map->needsRedraw()) {
        break;
      }

      for (qint32 x = left; x < right; x += dx) {
        if (map->needsRedraw()) {
          break;
        }

        // read tile from file
        CPLErr err = CE_Failure;

        // reduce tile size at the border of the file
        qreal dx_used = dx;
        qreal dy_used = dy;
        qreal imgw_used = imgw;
        qreal imgh_used = imgh;

        if ((x + dx) > xsize_px) {
          dx_used = xsize_px - x;
          imgw_used = qRound(imgw * dx_used / dx) & 0xFFFFFFFC;
        }
        if ((y + dy) > ysize_px) {
          dy_used = ysize_px - y;
          imgh_used = imgh * dy_used / dy;
        }

        dx_used = qFloor(dx_used);
        dy_used = qFloor(dy_used);
        imgw_used = qRound(imgw_used);
        imgh_used = qRound(imgh_used);

        if (imgw_used < 1 || imgh_used < 1) {
          continue;
        }

        QImage img;
        if (rasterBandCount == 1) {
          GDALRasterBand* pBand;
          pBand = dataset->GetRasterBand(1);

          img = QImage(QSize(imgw_used, imgh_used), QImage::Format_Indexed8);
          img.setColorTable(colortable);

          err = pBand->RasterIO(GF_Read, x, y, dx_used, dy_used, img.bits(), imgw_used, imgh_used, GDT_Byte, 0, 0);
        } else {
          img = QImage(imgw_used, imgh_used, QImage::Format_ARGB32);
          img.fill(qRgba(255, 255, 255, 255));

          QVector<quint8> buffer(imgw_used * imgh_used);

          QRgb testPix = qRgba(GCI_RedBand, GCI_GreenBand, GCI_BlueBand, GCI_AlphaBand);

          for (int b = 1; b <= rasterBandCount; ++b) {
            GDALRasterBand* pBand;
            pBand = dataset->GetRasterBand(b);

            err = pBand->RasterIO(GF_Read, x, y, dx_used, dy_used, buffer.data(), imgw_used, imgh_used, GDT_Byte, 0, 0);

            if (!err) {
              int pbandColour = pBand->GetColorInterpretation();
              unsigned int offset;

              for (offset = 0; offset < sizeof(testPix) && *(((quint8*)&testPix) + offset) != pbandColour; offset++) {
              }
              if (offset < sizeof(testPix)) {
                quint8* pTar = img.bits() + offset;
                quint8* pSrc = buffer.data();
                const int size = buffer.size();

                for (int i = 0; i < size; ++i) {
                  *pTar = *pSrc;
                  pTar += sizeof(testPix);
                  pSrc += 1;
                }
              }
            }
          }
        }

        if (err) {
          continue;
        }

        QPolygonF l;
        l << QPointF(x, y) << QPointF(x + dx_used, y) << QPointF(x + dx_used, y + dy_used) << QPointF(x, y + dy_used);
        l = trFwd.map(l);

        proj.transform(l, PJ_FWD);

        drawTile(img, l, p);
      }
    }
  }

  p.setPen(Qt::black);
  p.setBrush(Qt::NoBrush);
  p.drawPolygon(boundingBox);
}
