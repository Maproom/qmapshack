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
#include <algorithm>

#include "CMainWindow.h"
#include "dem/CDemDraw.h"
#include "helpers/CDraw.h"
#include "helpers/CGdalVrtUtil.h"
#include "units/IUnit.h"

CDemVRT::CDemVRT(const QString& filename, CDemDraw* parent, bool supportsOverviewAdvisory)
    : IDem(parent), filename(filename), supportsOverviewAdvisory(supportsOverviewAdvisory) {
  qDebug() << "------------------------------";
  qDebug() << "VRT: try to open" << filename;

  dataset = GDALDataset::FromHandle(GDALOpen(filename.toUtf8(), GA_ReadOnly));
  if (nullptr == dataset) {
    QMessageBox::warning(CMainWindow::getBestWidgetForParent(), tr("Error..."),
                         tr("Failed to load file:") % '\n' % filename);
    return;
  }

  QString missingFile;
  if (!CGdalVrtUtil::allReferencedFilesExist(dataset, missingFile)) {
    CGdalVrtUtil::closeDataset(dataset);
    QMessageBox::warning(
        CMainWindow::getBestWidgetForParent(), tr("Error..."),
        tr("File does not exist:") % '\n' % missingFile % '\n' % tr("referenced by file:") % '\n' % filename);
    return;
  }

  if (dataset->GetRasterCount() != 1) {
    CGdalVrtUtil::closeDataset(dataset);
    QMessageBox::warning(CMainWindow::getBestWidgetForParent(), tr("Error..."),
                         tr("DEM must have exactly one raster band:") % '\n' % filename);
    return;
  }

  GDALRasterBand* pBand = dataset->GetRasterBand(1);
  if (nullptr == pBand) {
    CGdalVrtUtil::closeDataset(dataset);
    QMessageBox::warning(CMainWindow::getBestWidgetForParent(), tr("Error..."),
                         tr("DEM must have exactly one raster band:") % '\n' % filename);
    return;
  }

  // Float64 included despite being implausible for real-world elevation storage: some WCS
  // servers' DescribeCoverage responses don't pin down a concrete bit width, and GDAL's WCS
  // driver then defaults to double. Harmless to allow - every read below requests GDT_Float32
  // from GDAL regardless of source type, so this check is purely a plausibility gate, not a
  // requirement of the I/O path.
  const GDALDataType bandType = pBand->GetRasterDataType();
  if (bandType != GDT_Int16 && bandType != GDT_UInt16 && bandType != GDT_Int32 && bandType != GDT_UInt32 &&
      bandType != GDT_Float32 && bandType != GDT_Float64) {
    CGdalVrtUtil::closeDataset(dataset);
    QMessageBox::warning(CMainWindow::getBestWidgetForParent(), tr("Error..."),
                         tr("DEM must have one band with 16bit, 32bit or 64bit numeric data:") % '\n' % filename);
    return;
  }

  // dataset's own size before any reprojection below replaces it with a warped VRT;
  // used to rescale weakestMaxFactor into the final dataset's pixel grid further down
  const qint32 preWarpXSize = pBand->GetXSize();
  const qint32 preWarpYSize = pBand->GetYSize();

  // skipped entirely for remote sources (CDemWCS, supportsOverviewAdvisory == false): the
  // per-file fallback inside collectOverviewFactors() calls GetFileList() then GDALOpen()
  // on every referenced "file" - meaningless, and an extra request against the same
  // remote endpoint, for a source with no local files to inspect. Safe to skip:
  // overviewAdvice/overviewFactors are only ever consulted below/in draw() when
  // supportsOverviewAdvisory is also true.
  CGdalVrtUtil::overview_factors_t overviewFactors;
  if (supportsOverviewAdvisory) {
    qreal masterGeoTransform[6];
    const qreal masterPixelSizeX =
        (dataset->GetGeoTransform(masterGeoTransform) == CE_None) ? qAbs(masterGeoTransform[1]) : 0.0;

    qDebug() << "OVR: branch:" << (pBand->GetOverviewCount() > 0 ? "band-level" : "per-file probe")
             << "GetOverviewCount =" << pBand->GetOverviewCount();

    overviewFactors = CGdalVrtUtil::collectOverviewFactors(dataset, pBand, masterPixelSizeX);

    for (const CGdalVrtUtil::file_overview_info_t& info : overviewFactors.perFileInfo) {
      qDebug() << "OVR:  " << QFileInfo(info.path).fileName() << "factors:" << info.factors;
    }
    qDebug() << "OVR: factors =" << overviewFactors.factors << "weakestMaxFactor =" << overviewFactors.weakestMaxFactor;

    // DEM data is always single-band continuous elevation, never categorical/palette
    overviewAdvice =
        CGdalVrtUtil::buildOverviewAdvice(dataset, pBand, filename, /*isPaletteIndexed=*/false, overviewFactors);
  }

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
    psOptions->pfnProgress = &CGdalVrtUtil::progressCallback;

    dataset = GDALDataset::FromHandle(GDALAutoCreateWarpedVRT(
        GDALDataset::ToHandle(srcDataset), nullptr, targetSRS.exportToWkt().c_str(), GRA_Bilinear, 0.1, psOptions));

    GDALDestroyWarpOptions(psOptions);

    if (dataset == nullptr) {
      CGdalVrtUtil::closeDataset(srcDataset);
      QMessageBox::warning(CMainWindow::getBestWidgetForParent(), tr("Error..."),
                           tr("Failed to create Warp for:") % '\n' % filename);
      return;
    }
  }

  // ------- setup projection ---------------
  proj.init(dataset->GetProjectionRef(), "EPSG:4326");

  if (!proj.isValid()) {
    CGdalVrtUtil::closeDataset(dataset);
    CGdalVrtUtil::closeDataset(srcDataset);
    QMessageBox::warning(CMainWindow::getBestWidgetForParent(), tr("Error..."),
                         tr("No georeference information found:") % '\n' % filename);
    return;
  }

  xsize_px = dataset->GetRasterXSize();
  ysize_px = dataset->GetRasterYSize();

  if (supportsOverviewAdvisory) {
    // a reprojection (the warp block above) can change the dataset's own pixel density -
    // e.g. between a projected CRS in meters and a geographic CRS in degrees, or simply a
    // different output resolution GDAL chose - so rescale weakestMaxFactor (collected
    // pre-warp, in the original dataset's own pixel grid) into the current dataset's
    // pixel grid, matching what draw()'s neededFactor (derived from the current
    // xscale/yscale) is compared against. A no-op (ratio 1.0) whenever no warp happened,
    // since the raster size is then unchanged.
    const qreal warpScale =
        qMax(static_cast<qreal>(xsize_px) / preWarpXSize, static_cast<qreal>(ysize_px) / preWarpYSize);
    overviewAdvice.weakestMaxFactor = qMax(1, qRound(overviewAdvice.weakestMaxFactor * warpScale));

    const qint32 finalOvrCount = dataset->GetRasterBand(1)->GetOverviewCount();
    qDebug() << "OVR: post-warp GetOverviewCount =" << finalOvrCount << "warpScale =" << warpScale
             << "weakestMaxFactor =" << overviewAdvice.weakestMaxFactor;
    if (overviewAdvice.overviewsMissing) {
      qDebug() << "OVR: assessment: no overviews - advisory will fire on slow render";
    } else if (finalOvrCount == 0) {
      qDebug() << "OVR: assessment: source overviews exist but VRT lacks <OverviewList> - add it";
    } else {
      qDebug() << "OVR: assessment: OK, overviews up to factor" << overviewAdvice.weakestMaxFactor;
    }
  }

  qreal adfGeoTransform[6];
  if (dataset->GetGeoTransform(adfGeoTransform) != CE_None) {
    CGdalVrtUtil::closeDataset(dataset);
    CGdalVrtUtil::closeDataset(srcDataset);
    QMessageBox::warning(CMainWindow::getBestWidgetForParent(), tr("Error..."),
                         tr("No pixel-to-map transform found:") % '\n' % filename);
    return;
  }

  xscale = adfGeoTransform[1];
  yscale = adfGeoTransform[5];

  // Build trFwd directly from GDAL's affine matrix instead of decomposing it into
  // translate+scale+rotate: adfGeoTransform[2]/[4] is a general shear term, not
  // necessarily a pure rotation, so there is no single angle that reproduces it via
  // QTransform::rotate() (which, in addition, takes degrees - qAtan() returns radians).
  trFwd = QTransform(adfGeoTransform[1], adfGeoTransform[4], adfGeoTransform[2], adfGeoTransform[5], adfGeoTransform[0],
                     adfGeoTransform[3]);

  if (proj.isSrcLatLong()) {
    xscale *= 111120;
    yscale *= 111120;
    // Scale every element of the homogeneous matrix by DEG_TO_RAD to convert trFwd's
    // mapped output from degrees to radians. This works because QTransform::map() never
    // reads m13/m23/m33 as long as the transform stays non-projective (true here, since
    // trFwd is built only from translate/scale/shear) - so scaling the whole matrix
    // scales the mapped point without having to touch dx/dy and the linear part separately.
    trFwd = trFwd * DEG_TO_RAD;
  }

  trInv = trFwd.inverted();

  // use all four corners (not just the nominally adjacent pair) since a rotated or
  // skewed geotransform can move any corner to the extreme
  const QPointF c1 = trFwd.map(QPointF(0, 0));
  const QPointF c2 = trFwd.map(QPointF(xsize_px, 0));
  const QPointF c3 = trFwd.map(QPointF(xsize_px, ysize_px));
  const QPointF c4 = trFwd.map(QPointF(0, ysize_px));
  boundingBox = QRectF(QPointF(std::min({c1.x(), c2.x(), c3.x(), c4.x()}), std::min({c1.y(), c2.y(), c3.y(), c4.y()})),
                       QPointF(std::max({c1.x(), c2.x(), c3.x(), c4.x()}), std::max({c1.y(), c2.y(), c3.y(), c4.y()})));

  qDebug() << "bounding box" << boundingBox;
  qDebug() << "FF" << trFwd;
  qDebug() << "RR" << trInv;

  connect(dem, &CDemDraw::sigNeedsRedraw, this, &CDemVRT::slotNeedsRedraw);

  isActivated = true;
}

CDemVRT::~CDemVRT() {
  threadPool.waitForDone();
  QMutexLocker lock(&mutex);
  CGdalVrtUtil::closeDataset(dataset);
  CGdalVrtUtil::closeDataset(srcDataset);
}

void CDemVRT::saveConfig(QSettings& cfg) {
  IDem::saveConfig(cfg);
  cfg.setValue("suppressOverviewAdvisory", suppressOverviewAdvisory.load());
}

void CDemVRT::loadConfig(QSettings& cfg) {
  IDem::loadConfig(cfg);
  suppressOverviewAdvisory = cfg.value("suppressOverviewAdvisory", suppressOverviewAdvisory.load()).toBool();
}

void CDemVRT::slotNeedsRedraw() { threadPool.clear(); }

bool CDemVRT::toRasterPixel(const QPointF& pos, QPointF& pixel) const {
  QPointF pt = pos;
  proj.transform(pt, PJ_INV);

  if (!boundingBox.contains(pt)) {
    return false;
  }

  pixel = trInv.map(pt);
  return true;
}

qreal CDemVRT::getElevationAt(const QPointF& pos, bool checkScale) {
  if (!proj.isValid() || (checkScale && outOfScale)) {
    return NOFLOAT;
  }

  QPointF pt;
  if (!toRasterPixel(pos, pt)) {
    return NOFLOAT;
  }

  qreal x = pt.x() - qFloor(pt.x());
  qreal y = pt.y() - qFloor(pt.y());

  float e[4];
  CPLErr err;
  {
    QMutexLocker lock(&mutex);
    err = dataset->RasterIO(GF_Read, qFloor(pt.x()), qFloor(pt.y()), 2, 2, e, 2, 2, GDT_Float32, 1, 0, 0, 0, 0);
  }
  if (err != CE_None) {
    return NOFLOAT;
  }

  if (hasNoData && ((e[0] == noData) || (e[1] == noData) || (e[2] == noData) || (e[3] == noData))) {
    return NOFLOAT;
  }

  return bilinear(e[0], e[1], e[2], e[3], x, y);
}

qreal CDemVRT::getSlopeAt(const QPointF& pos, bool checkScale) {
  if (!proj.isValid() || (checkScale && outOfScale)) {
    return NOFLOAT;
  }

  QPointF pt;
  if (!toRasterPixel(pos, pt)) {
    return NOFLOAT;
  }

  qreal x = pt.x() - qFloor(pt.x());
  qreal y = pt.y() - qFloor(pt.y());

  float win[eWinsize4x4];
  {
    QMutexLocker lock(&mutex);

    CPLErr err =
        dataset->RasterIO(GF_Read, qFloor(pt.x()) - 1, qFloor(pt.y()) - 1, 4, 4, win, 4, 4, GDT_Float32, 1, 0, 0, 0, 0);
    if (err != CE_None) {
      return NOFLOAT;
    }
  }

  // slopeOfWindowInterp() already returns NOFLOAT if any sample in win is noData
  return slopeOfWindowInterp(win, eWinsize4x4, x, y);
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
  qreal buf_scale_x = qAbs(bufferScale.x() / xscale / buf.image.devicePixelRatio());
  qreal buf_scale_y = qAbs(bufferScale.y() / yscale / buf.image.devicePixelRatio());
  // <1 would mean GDAL does upscaling which is pointless
  if (buf_scale_x < 1.0) {
    buf_scale_x = 1.0;
  }
  if (buf_scale_y < 1.0) {
    buf_scale_y = 1.0;
  }

  // corners of the area we shall draw, converted from the canvas projection into the
  // DEM's own pixel coordinate space
  auto toDemPixel = [this](QPointF pt) {
    proj.transform(pt, PJ_INV);
    return trInv.map(pt);
  };
  const QPointF pt1 = toDemPixel(buf.ref1);
  const QPointF pt2 = toDemPixel(buf.ref2);
  const QPointF pt3 = toDemPixel(buf.ref3);
  const QPointF pt4 = toDemPixel(buf.ref4);

  // bounds of the area to draw in the coordinate space of the DEM
  // use all four corners (not just the nominally adjacent pair) since a rotated
  // geotransform or a skewing reprojection can move any corner to the extreme
  qreal left = std::min({pt1.x(), pt2.x(), pt3.x(), pt4.x()});
  qreal right = std::max({pt1.x(), pt2.x(), pt3.x(), pt4.x()});
  qreal top = std::min({pt1.y(), pt2.y(), pt3.y(), pt4.y()});
  qreal bottom = std::max({pt1.y(), pt2.y(), pt3.y(), pt4.y()});

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

  CGdalVrtUtil::read_deadline_t deadline{dem};
  deadline.timer.start();

  data.resize(static_cast<qsizetype>(w_buf) * h_buf);
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
    CPLErr err = dataset->GetRasterBand(1)->ReadRaster(data.data(), static_cast<size_t>(w_buf) * h_buf, x, y, w_dem,
                                                       h_dem, w_buf, h_buf, GRIORA_Bilinear,
                                                       &CGdalVrtUtil::progressCallbackWithDeadline, &deadline);

    if (err != CE_None) {
      if (deadline.timedOut) {
        if (supportsOverviewAdvisory && !suppressOverviewAdvisory && !advisoryShownThisSession) {
          advisoryShownThisSession = true;
          dem->emitOverviewAdvisory(this);
        } else if (!advisoryOpen) {
          dem->emitSigCanvasUpdate();
        }
      }
      return;
    }
  }

  quint32 w_used = w_buf - 2;
  quint32 h_used = h_buf - 2;

  // resize and wire up only the buffers for layers that are actually enabled;
  // computeShading() skips any layer whose shading_buffers_t entry is left null
  shading_buffers_t buffers;
  if (doHillshading()) {
    hillshadeBuf.resize(w_used * h_used);
    buffers.hillshade = &hillshadeBuf;
  }
  if (doSlopeShading()) {
    slopeShadeBuf.resize(w_used * h_used);
    buffers.slopeShade = &slopeShadeBuf;
  }
  if (doSlopeColor()) {
    slopeColorBuf.resize(w_used * h_used);
    buffers.slopeColor = &slopeColorBuf;
  }
  if (doElevationLimit()) {
    elevationLimitBuf.resize(w_used * h_used);
    buffers.elevationLimit = &elevationLimitBuf;
  }
  if (doElevationShading()) {
    elevationShadeBuf.resize(w_used * h_used);
    buffers.elevationShade = &elevationShadeBuf;
  }

  // compute every enabled layer for the whole image in parallel on a 4x4 grid of chunks,
  // blocking until either all chunks are done (true) or a fresher redraw makes the result
  // moot (false, with whatever work was already queued left to finish in the background)
  auto computeAllShading = [=, this]() {
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

        threadPool.start([=, this]() { computeShading(data, buffers, x_chunk, y_chunk, w_used, w_chunk, h_chunk); });
      }
    }
    threadPool.waitForDone();
    return true;
  };

  if (!computeAllShading()) {
    return;
  }

  // get pixel offset of top left buffer corner
  QPointF pp = buf.ref1;
  dem->convertRad2Px(pp);

  // start to draw the map
  QPainter p(&buf.image);
  USE_ANTI_ALIASING(p, true);
  p.translate(-pp);

  qreal o1 = getOpacity() / 100.0;
  qreal o2 = qMin(o1 + 0.4, 1.0);

  // compute the destination rect we will draw the shadings into
  QPointF top_left = trFwd.map(QPointF(left, top));
  QPointF bottom_right = trFwd.map(QPointF(right, bottom));
  proj.transform(top_left, PJ_FWD);
  proj.transform(bottom_right, PJ_FWD);
  dem->convertRad2Px(top_left);
  dem->convertRad2Px(bottom_right);
  QRectF dest(top_left, bottom_right);

  // paint one already-computed layer into dest at the given opacity; colorTable may be
  // null (e.g. for the alpha-only slope shading layer)
  auto paintLayer = [&](const QVector<quint8>& layerBuf, QImage::Format format, const QVector<QRgb>* colorTable,
                        qreal opacity) {
    QImage img(layerBuf.constData(), w_used, h_used, w_used, format);
    if (colorTable != nullptr) {
      img.setColorTable(*colorTable);
    }
    p.setOpacity(opacity);
    p.drawImage(dest, img);
  };

  if (doHillshading()) {
    paintLayer(hillshadeBuf, QImage::Format_Indexed8, &graytable, o1);
  }
  if (doSlopeShading()) {
    paintLayer(slopeShadeBuf, QImage::Format_Alpha8, nullptr, o1);
  }
  if (doSlopeColor()) {
    paintLayer(slopeColorBuf, QImage::Format_Indexed8, &slopetable, o2);
  }
  if (doElevationLimit()) {
    paintLayer(elevationLimitBuf, QImage::Format_Indexed8, &elevationtable, o2);
  }
  if (doElevationShading()) {
    paintLayer(elevationShadeBuf, QImage::Format_Indexed8, &elevationShadeTable, o1);
  }

  drawElevationShadeScale(p);
}

void CDemVRT::drawElevationShadeScale(QPainter& p) const {
  if (!doElevationShading() || !doShowElevationShadeScale()) {
    return;
  }

  // legend layout, anchored to the top-right corner of the visible canvas
  constexpr int kLabelCount = 7;        // number of elevation labels shown
  constexpr int kRowHeight = 30;        // vertical spacing between label rows
  constexpr int kTextRightMargin = 70;  // x offset of the heading/labels from the right edge
  constexpr int kHeadingY = 30;         // y position of the "Ele." heading
  constexpr int kFirstLabelY = 50;      // y position of the lowest-elevation label
  constexpr int kBarTopGap = 10;        // vertical gap between the heading row and the color bar
  constexpr int kBarLeftX = 30;         // x offset of the color bar's left edge from the right edge
  constexpr int kBarRightX = 15;        // x offset of the color bar's right edge from the right edge
  constexpr int kBarTop = kRowHeight + kBarTopGap;
  constexpr int kBarBottom = kLabelCount * kRowHeight;

  p.save();

  // heading and limits
  p.setOpacity(1.0);
  QRect visibleCanvasArea = CMainWindow::self().getVisibleCanvas()->rect();
  qreal limitLow = std::min(getElevationShadeLimitLow(), getElevationShadeLimitHi());
  qreal limitHi = std::max(getElevationShadeLimitLow(), getElevationShadeLimitHi());
  CDraw::text(tr("Ele."), p, QPointF(visibleCanvasArea.width() - kTextRightMargin, kHeadingY), Qt::black);

  // labels, evenly spaced from limitLow (bottom) to limitHi (top)
  for (int i = 0; i < kLabelCount; i++) {
    qreal meter = i / (double)(kLabelCount - 1) * (limitHi - limitLow) + limitLow;
    QString val, unit;
    IUnit::self().meter2elevation(meter, val, unit);
    CDraw::text(
        QString("%1 %2").arg(val, unit), p,
        QPointF(visibleCanvasArea.width() - kTextRightMargin, kFirstLabelY + (kLabelCount - 1 - i) * kRowHeight),
        Qt::black);
  }

  // color bar, drawn one pixel row at a time with the hue interpolated from blue (low) to red (high)
  for (int i = kBarTop; i <= kBarBottom; i++) {
    qreal hue = 240 * (1 - (double)(i - kBarTop) / (kBarBottom - kBarTop));
    const QColor color = QColor::fromHsv(hue, 255, 255);
    p.setPen(color);
    p.drawLine(QPointF(visibleCanvasArea.width() - kBarLeftX, kBarTop + kBarBottom - i),
               QPointF(visibleCanvasArea.width() - kBarRightX, kBarTop + kBarBottom - i));
  }

  p.restore();
}
