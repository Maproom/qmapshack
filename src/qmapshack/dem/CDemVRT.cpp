/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler <oliver.eichler@gmx.de>
                  2019 Johannes Zellner johannes@zellner.org

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

#include "dem/CDemVRT.h"

#include <gdal.h>
#include <gdal_priv.h>
#include <gdalwarper.h>

#include <QtWidgets>

#include "CMainWindow.h"
#include "dem/CDemDraw.h"
#include "helpers/CDraw.h"
#include "units/IUnit.h"

CDemVRT::CDemVRT(const QString& filename, CDemDraw* parent) : IDem(parent), filename(filename) {
  qDebug() << "------------------------------";
  qDebug() << "VRT: try to open" << filename;

  dataset = (GDALDataset*)GDALOpen(filename.toUtf8(), GA_ReadOnly);
  if (nullptr == dataset) {
    QMessageBox::warning(CMainWindow::getBestWidgetForParent(), tr("Error..."),
                         tr("Failed to load file:") % '\n' % filename);
    return;
  }

  QString fileItem;
  char** fileList = dataset->GetFileList();
  int n = 0;
  while (fileList[n] != nullptr) {
#if defined(Q_OS_WIN32)
    fileItem = QString::fromLocal8Bit(fileList[n]);
    if (QFileInfo(fileItem).exists()) {
      n++;
      continue;
    }
#endif  // defined(Q_OS_WIN32)
    fileItem = QString::fromUtf8(fileList[n]);
    if (QFileInfo(fileItem).exists()) {
      n++;
      continue;
    }
    n = -1;
    break;
  }
  CSLDestroy(fileList);
  if (n < 0) {
    GDALClose(dataset);
    dataset = nullptr;
    QMessageBox::warning(
        CMainWindow::getBestWidgetForParent(), tr("Error..."),
        tr("File does not exist:") % '\n' % fileItem % '\n' % tr("referenced by file:") % '\n' % filename);
    return;
  }

  if (dataset->GetRasterCount() != 1) {
    GDALClose(dataset);
    dataset = nullptr;
    QMessageBox::warning(CMainWindow::getBestWidgetForParent(), tr("Error..."),
                         tr("DEM must have one band with 16bit or 32bit data:") % '\n' % filename);
    return;
  }

  GDALRasterBand* pBand = dataset->GetRasterBand(1);
  if (nullptr == pBand) {
    GDALClose(dataset);
    dataset = nullptr;
    QMessageBox::warning(CMainWindow::getBestWidgetForParent(), tr("Error..."),
                         tr("DEM must have one band with 16bit or 32bit data:") % '\n' % filename);
    return;
  }

  hasOverviews = pBand->GetOverviewCount() != 0;
  qDebug() << "has overviews" << hasOverviews;

  noData = pBand->GetNoDataValue(&hasNoData);
  qDebug() << "no data:" << hasNoData << noData;

  // ------- setup warped VRT ---------------
  // if the projection of the dataset is different then that of the drawing context we wrap the dataset in a virtual
  // warped dataset to transparently resample it into the drawing contexts projection.
  OGRSpatialReference targetSRS;
  OGRErr rv = targetSRS.SetFromUserInput(dem->getProjection().toUtf8());
  const OGRSpatialReference* sourceSRS = dataset->GetSpatialRef();
  if (rv == OGRERR_NONE && sourceSRS != nullptr && !sourceSRS->IsSame(&targetSRS)) {
    srcDataset = dataset;

    GDALWarpOptions* psOptions = GDALCreateWarpOptions();
    psOptions->pProgressArg = dem;
    psOptions->pfnProgress = [](double dfc, const char* msg, void* arg) -> int {
      auto dem = reinterpret_cast<CDemDraw*>(arg);
      return !dem->needsRedraw();
    };

    dataset = GDALDataset::FromHandle(GDALAutoCreateWarpedVRT(
        GDALDataset::ToHandle(srcDataset), nullptr, targetSRS.exportToWkt().c_str(), GRA_Bilinear, 0.1, psOptions));

    GDALDestroyWarpOptions(psOptions);

    if (dataset == nullptr) {
      GDALClose(srcDataset);
      srcDataset = nullptr;
      QMessageBox::warning(CMainWindow::getBestWidgetForParent(), tr("Error..."),
                           tr("Failed to create Warp for:") % '\n' % filename);
      return;
    }

    if (hasOverviews) {
      // to make GDAL take advantage of overviews in the original dataset we need to add "virtual overviews" with the
      // same decimation factors to the WarpedVRT so first build a list of the overviews the original dataset
      // contains...
      QVector<qint32> overviews(pBand->GetOverviewCount());
      for (int i = 0; i < overviews.size(); ++i) {
        GDALRasterBand* overview = pBand->GetOverview(i);
        qreal decimationFactor = (qreal)pBand->GetXSize() / overview->GetXSize();
        overviews[i] = qRound(decimationFactor);
      }
      // ...and attach them as virtual overview
      CPLSetConfigOption("VRT_VIRTUAL_OVERVIEWS", "YES");
      dataset->BuildOverviews("NONE", overviews.size(), overviews.data(), 0, nullptr, nullptr, nullptr);
      CPLSetConfigOption("VRT_VIRTUAL_OVERVIEWS", "NO");
    }
  }

  // ------- setup projection ---------------
  proj.init(dataset->GetProjectionRef(), "EPSG:4326");

  if (!proj.isValid()) {
    GDALClose(dataset);
    dataset = nullptr;
    GDALClose(srcDataset);
    srcDataset = nullptr;
    QMessageBox::warning(CMainWindow::getBestWidgetForParent(), tr("Error..."),
                         tr("No georeference information found:") % '\n' % filename);
    return;
  }

  xsize_px = dataset->GetRasterXSize();
  ysize_px = dataset->GetRasterYSize();

  qreal adfGeoTransform[6];
  dataset->GetGeoTransform(adfGeoTransform);

  xscale = adfGeoTransform[1];
  yscale = adfGeoTransform[5];

  trFwd.translate(adfGeoTransform[0], adfGeoTransform[3]);
  trFwd.scale(adfGeoTransform[1], adfGeoTransform[5]);

  if (adfGeoTransform[4] != 0.0) {
    trFwd.rotate(qAtan(adfGeoTransform[2] / adfGeoTransform[4]));
  }

  if (proj.isSrcLatLong()) {
    xscale *= 111120;
    yscale *= 111120;
    // convert to RAD to match internal notations
    trFwd = trFwd * DEG_TO_RAD;
  }

  trInv = trFwd.inverted();

  ref1 = trFwd.map(QPointF(0, 0));
  ref2 = trFwd.map(QPointF(xsize_px, 0));
  ref3 = trFwd.map(QPointF(xsize_px, ysize_px));
  ref4 = trFwd.map(QPointF(0, ysize_px));

  qDebug() << ref1 << ref2 << ref3 << ref4;
  boundingBox = QRectF(ref1, ref3);

  qDebug() << "FF" << trFwd;
  qDebug() << "RR" << trInv;

  connect(dem, &CDemDraw::sigNeedsRedraw, this, &CDemVRT::slotNeedsRedraw);

  isActivated = true;
}

CDemVRT::~CDemVRT() {
  threadPool.waitForDone();
  GDALClose(dataset);
  GDALClose(srcDataset);
}

void CDemVRT::slotNeedsRedraw() { threadPool.clear(); }

qreal CDemVRT::getElevationAt(const QPointF& pos, bool checkScale) {
  if (!proj.isValid() || (checkScale && outOfScale)) {
    return NOFLOAT;
  }

  float e[4];
  QPointF pt = pos;

  proj.transform(pt, PJ_INV);

  if (!boundingBox.contains(pt)) {
    return NOFLOAT;
  }

  pt = trInv.map(pt);

  qreal x = pt.x() - qFloor(pt.x());
  qreal y = pt.y() - qFloor(pt.y());

  mutex.lock();
  CPLErr err = dataset->RasterIO(GF_Read, qFloor(pt.x()), qFloor(pt.y()), 2, 2, &e, 2, 2, GDT_Float32, 1, 0, 0, 0, 0);
  mutex.unlock();
  if (err == CE_Failure) {
    return NOFLOAT;
  }

  if (hasNoData && ((e[0] == noData) || (e[1] == noData) || (e[2] == noData) || (e[3] == noData))) {
    return NOFLOAT;
  }

  qreal b1 = e[0];
  qreal b2 = e[1] - e[0];
  qreal b3 = e[2] - e[0];
  qreal b4 = e[0] - e[1] - e[2] + e[3];

  qreal ele = b1 + b2 * x + b3 * y + b4 * x * y;

  return ele;
}

qreal CDemVRT::getSlopeAt(const QPointF& pos, bool checkScale) {
  if (!proj.isValid() || (checkScale && outOfScale)) {
    return NOFLOAT;
  }

  QPointF pt = pos;

  proj.transform(pt, PJ_INV);

  if (!boundingBox.contains(pt)) {
    return NOFLOAT;
  }

  pt = trInv.map(pt);

  qreal x = pt.x() - qFloor(pt.x());
  qreal y = pt.y() - qFloor(pt.y());

  float win[eWinsize4x4];
  {
    QMutexLocker lock(&mutex);

    CPLErr err = dataset->RasterIO(GF_Read, qFloor(pt.x()) - 1, qFloor(pt.y()) - 1, 4, 4, &win, 4, 4, GDT_Float32, 1, 0,
                                   0, 0, 0);
    if (err != CE_None) {
      return NOFLOAT;
    }
  }

  for (int i = 0; i < eWinsize4x4; i++) {
    if (hasNoData && win[i] == noData) {
      return NOFLOAT;
    }
  }

  qreal slope = slopeOfWindowInterp(win, eWinsize4x4, x, y);
  return slope;
}

void CDemVRT::draw(IDrawContext::buffer_t& buf) {
  if (dem->needsRedraw()) {
    return;
  }

  QPointF bufferScale = buf.scale * buf.zoomFactor;
  outOfScale = isOutOfScale(bufferScale);

  if (outOfScale ||
      (!doHillshading() && !doSlopeShading() && !doSlopeColor() && !doElevationLimit() && !doElevationShading())) {
    QThread::msleep(100);
    return;
  }

  // use bufferScale (and therefore the zoom level) and the pixel scale of the DEM to caluclate a downsampling factor
  qreal buf_scale_x = qAbs(bufferScale.x() / xscale);
  qreal buf_scale_y = qAbs(bufferScale.y() / yscale);
  // <1 would mean GDAL does upscaling which is pointless
  if (buf_scale_x < 1.0) {
    buf_scale_x = 1.0;
  }
  if (buf_scale_y < 1.0) {
    buf_scale_y = 1.0;
  }

  // corners of the area we shall draw
  QPointF pt1 = buf.ref1;
  QPointF pt2 = buf.ref2;
  QPointF pt3 = buf.ref3;
  QPointF pt4 = buf.ref4;

  proj.transform(pt1, PJ_INV);
  proj.transform(pt2, PJ_INV);
  proj.transform(pt3, PJ_INV);
  proj.transform(pt4, PJ_INV);

  pt1 = trInv.map(pt1);
  pt2 = trInv.map(pt2);
  pt3 = trInv.map(pt3);
  pt4 = trInv.map(pt4);

  // bounds of the area to draw in the coordinate space of the DEM
  qreal left = pt1.x() < pt4.x() ? pt1.x() : pt4.x();
  qreal right = pt2.x() > pt3.x() ? pt2.x() : pt3.x();
  qreal top = pt1.y() < pt2.y() ? pt1.y() : pt2.y();
  qreal bottom = pt4.y() > pt3.y() ? pt4.y() : pt3.y();

  if ((top > ysize_px) || (left > xsize_px) || (bottom < 0) || (right < 0)) {
    // current view is entirely outside the bounds of the DEM so there is nothing to draw
    return;
  }

  // clip bounds
  if (left < buf_scale_x) {
    left = buf_scale_x;
  }
  if (top < buf_scale_y) {
    top = buf_scale_y;
  }
  if (right + buf_scale_x > xsize_px) {
    right = xsize_px - buf_scale_x;
  }
  if (bottom + buf_scale_y > ysize_px) {
    bottom = ysize_px - buf_scale_y;
  }

  // guard against degenerate/inverted bounds (e.g. extreme geotransforms or
  // projections where the corner pairing above doesn't yield left<right/top<bottom)
  if (right <= left || bottom <= top) {
    return;
  }

  // the shading algorithms need a extra 1px border
  // that border needs to be scaled as well
  qreal x = left - buf_scale_x;
  qreal y = top - buf_scale_y;
  qreal w = right - left;
  qreal h = bottom - top;

  // dimensions we request from the DEM
  qreal w_dem = w + 2.0 * buf_scale_x;
  qreal h_dem = h + 2.0 * buf_scale_y;

  // dimensions of the buffer GDAL will read into
  quint32 w_buf = qRound(w_dem / buf_scale_x);
  quint32 h_buf = qRound(h_dem / buf_scale_y);
  if (w_buf < 3 || h_buf < 3) {
    return;
  }

  QVector<float> data(static_cast<qsizetype>(w_buf) * h_buf);
  {
    QMutexLocker lock(&mutex);

    // add a temporary GDAL error handler to filter out errors that are caused by intentionally aborted reads
    // automatically removed at end of scope
    CPLErrorHandlerPusher oCurrentHandler(
        [](CPLErr eErrClass, CPLErrorNum errNo, const char* msg) {
          if (errNo == CPLE_UserInterrupt || errNo == CPLE_AppDefined) {
            return;
          }
          CPLDefaultErrorHandler(eErrClass, errNo, msg);
        },
        nullptr);

    // by requesting a different size than the size of the buffer GDAL will automatically do scaling for us and use
    // overviews
    CPLErr err = dataset->GetRasterBand(1)->ReadRaster(
        data.data(), static_cast<size_t>(w_buf) * h_buf, x, y, w_dem, h_dem, w_buf, h_buf, GRIORA_Bilinear,
        [](double dfc, const char* msg, void* arg) -> int {
          auto dem = reinterpret_cast<CDemDraw*>(arg);
          return !dem->needsRedraw();
        },
        dem);

    if (err != CE_None) {
      return;
    }
  }

  quint32 w_used = w_buf - 2;
  quint32 h_used = h_buf - 2;

  QVector<uchar> outbuf(w_used * h_used);

  using shadeFnPtr =
      void (CDemVRT::*)(QVector<float>&, QVector<uchar>&, quint32, quint32, quint32, quint32, quint32) const;
  auto computeShading = [=, this, &data, &outbuf](shadeFnPtr shadeFn) {
    // run the shadings in paralell on equal sized chunks
    quint32 n_x = 4;
    quint32 n_y = 4;
    for (quint32 i = 0; i < n_y; ++i) {
      for (quint32 j = 0; j < n_x; ++j) {
        if (dem->needsRedraw()) {
          threadPool.waitForDone();
          return false;
        }

        quint32 step_w_buf = w_used / n_x;
        quint32 step_h_buf = h_used / n_y;
        quint32 x_chunk = step_w_buf * j;
        quint32 y_chunk = step_h_buf * i;
        quint32 w_chunk = (j == n_x - 1) ? (w_used - x_chunk) : step_w_buf;
        quint32 h_chunk = (i == n_y - 1) ? (h_used - y_chunk) : step_h_buf;

        threadPool.start([=, this, &data, &outbuf]() {
          std::invoke(shadeFn, this, data, outbuf, x_chunk, y_chunk, w_used, w_chunk, h_chunk);
        });
      }
    }
    threadPool.waitForDone();
    return true;
  };

  // get pixel offset of top left buffer corner
  QPointF pp = buf.ref1;
  dem->convertRad2Px(pp);

  // start to draw the map
  QPainter p(&buf.image);
  USE_ANTI_ALIASING(p, true);
  p.translate(-pp);

  qreal o1 = getOpacity() / 100.0;
  qreal o2 = qMin(o1 + 0.4, 1.0);
  p.setOpacity(o1);

  // compute the destination rect we will draw the shadings into
  QPointF top_left = trFwd.map(QPointF(left, top));
  QPointF bottom_right = trFwd.map(QPointF(right, bottom));
  proj.transform(top_left, PJ_FWD);
  proj.transform(bottom_right, PJ_FWD);
  dem->convertRad2Px(top_left);
  dem->convertRad2Px(bottom_right);
  QRectF dest(top_left, bottom_right);

  if (doHillshading()) {
    if (!computeShading(&CDemVRT::hillshading)) {
      return;
    }
    QImage img(outbuf.constData(), w_used, h_used, w_used, QImage::Format_Indexed8);
    img.setColorTable(graytable);
    p.drawImage(dest, img);
  }
  if (doSlopeShading()) {
    if (!computeShading(&CDemVRT::slopeShading)) {
      return;
    }
    QImage img(outbuf.constData(), w_used, h_used, w_used, QImage::Format_Alpha8);
    p.drawImage(dest, img);
  }
  if (doSlopeColor()) {
    if (!computeShading(&CDemVRT::slopecolor)) {
      return;
    }
    QImage img(outbuf.constData(), w_used, h_used, w_used, QImage::Format_Indexed8);
    img.setColorTable(slopetable);
    p.setOpacity(o2);
    p.drawImage(dest, img);
    p.setOpacity(o1);
  }
  if (doElevationLimit()) {
    if (!computeShading(&CDemVRT::elevationLimit)) {
      return;
    }
    QImage img(outbuf.constData(), w_used, h_used, w_used, QImage::Format_Indexed8);
    img.setColorTable(elevationtable);
    p.setOpacity(o2);
    p.drawImage(dest, img);
    p.setOpacity(o1);
  }
  if (doElevationShading()) {
    if (!computeShading(&CDemVRT::elevationShading)) {
      return;
    }
    QImage img(outbuf.constData(), w_used, h_used, w_used, QImage::Format_Indexed8);
    img.setColorTable(elevationShadeTable);
    p.drawImage(dest, img);
  }

  drawElevationShadeScale(p);
}

void CDemVRT::drawElevationShadeScale(QPainter& p) const {
  if (doElevationShading() && doShowElevationShadeScale()) {
    p.save();

    // heading and limits
    p.setOpacity(1.0);
    QRect visibleCanvasArea = CMainWindow::self().getVisibleCanvas()->rect();
    qreal limitLow = std::min(getElevationShadeLimitLow(), getElevationShadeLimitHi());
    qreal limitHi = std::max(getElevationShadeLimitLow(), getElevationShadeLimitHi());
    CDraw::text(tr("Ele."), p, QPointF(visibleCanvasArea.width() - 70, 30), Qt::black);

    // labels
    int nmbOfLabels = 7;
    int yOffset = 30;
    for (int i = 0; i < nmbOfLabels; i++) {
      qreal meter = i / (double)(nmbOfLabels - 1) * (limitHi - limitLow) + limitLow;
      QString val, unit;
      IUnit::self().meter2elevation(meter, val, unit);
      CDraw::text(QString("%1 %2").arg(val, unit), p,
                  QPointF(visibleCanvasArea.width() - 70, 50 + (nmbOfLabels - 1 - i) * yOffset), Qt::black);
    }

    // color bar
    for (int i = yOffset + 10; i <= nmbOfLabels * yOffset; i++) {
      qreal hue = 240 * (1 - (double)(i - yOffset - 10.) / (nmbOfLabels * yOffset - yOffset - 10));
      const QColor& color = QColor::fromHsv(hue, 255, 255);
      p.setPen(color);
      p.drawLine(QPointF(visibleCanvasArea.width() - 30, yOffset + 10 + (nmbOfLabels * yOffset) - i),
                 QPointF(visibleCanvasArea.width() - 15, yOffset + 10 + (nmbOfLabels * yOffset) - i));
    }

    p.restore();
  }
}
