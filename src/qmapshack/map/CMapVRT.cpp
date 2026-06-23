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

#include <gdal.h>
#include <gdal_priv.h>
#include <gdalwarper.h>

#include <QtWidgets>
#include <algorithm>

#include "CMainWindow.h"
#include "helpers/CDraw.h"
#include "map/CMapDraw.h"

CMapVRT::CMapVRT(const QString& filename, CMapDraw* parent) : IMap(eFeatVisibility, parent), filename(filename) {
  qDebug() << "------------------------------";
  qDebug() << "VRT: try to open" << filename;

  dataset = GDALDataset::FromHandle(GDALOpen(filename.toUtf8(), GA_ReadOnly));
  if (nullptr == dataset) {
    fail(tr("Failed to load file:") % '\n' % filename);
    return;
  }

  QString missingFile;
  if (!allReferencedFilesExist(dataset, missingFile)) {
    fail(tr("File does not exist:") % '\n' % missingFile % '\n' % tr("referenced by file:") % '\n' % filename);
    return;
  }

  // ------- setup color table ---------
  rasterBandCount = dataset->GetRasterCount();
  GDALRasterBand* pBand = dataset->GetRasterBand(1);
  if (rasterBandCount == 1) {
    if (nullptr == pBand) {
      fail(tr("Failed to load file:") % '\n' % filename);
      return;
    }

    if (pBand->GetColorInterpretation() == GCI_PaletteIndex) {
      GDALColorTable* pct = pBand->GetColorTable();
      for (qint32 i = 0; i < pct->GetColorEntryCount(); ++i) {
        const GDALColorEntry& e = *pct->GetColorEntry(i);
        colortable << qRgba(e.c1, e.c2, e.c3, e.c4);
      }
    } else if (pBand->GetColorInterpretation() == GCI_GrayIndex) {
      for (qint32 i = 0; i < 256; ++i) {
        colortable << qRgba(i, i, i, 255);
      }
    } else {
      fail(tr("File must be 8 bit palette or gray indexed:") % '\n' % filename);
      return;
    }

    int success = 0;
    qreal idx = pBand->GetNoDataValue(&success);
    if (success && (idx >= 0) && (idx < colortable.size())) {
      QColor tmp(colortable[idx]);
      tmp.setAlpha(0);
      colortable[idx] = tmp.rgba();
    }
  }

  qreal masterGeoTransform[6];
  const qreal masterPixelSizeX =
      (pBand != nullptr && dataset->GetGeoTransform(masterGeoTransform) == CE_None) ? qAbs(masterGeoTransform[1]) : 0.0;
  const QVector<qint32> overviewFactors =
      (pBand != nullptr) ? collectOverviewFactors(dataset, pBand, masterPixelSizeX) : QVector<qint32>();
  qDebug() << "overview factors" << overviewFactors;

  // single band palette/gray data is categorical: nearest neighbour avoids blending index
  // values into meaningless colors when resampling. Multi-band true color benefits from
  // a smoother bilinear resampling.
  const GDALResampleAlg resampleAlg = (rasterBandCount == 1) ? GRA_NearestNeighbour : GRA_Bilinear;

  // true color sources that don't already carry their own alpha band have no per-pixel way to
  // mark "outside the source footprint" - ask the warp to synthesize a coverage-tracking
  // destination alpha band so those areas come back transparent instead of opaque garbage.
  // Single band palette/gray data is left alone: it already gets transparency for nodata-marked
  // pixels via the colortable alpha tweak above, and Format_Indexed8 has no room for a separate
  // alpha channel anyway.
  bool hasAlphaBand = false;
  for (qint32 b = 1; b <= rasterBandCount; ++b) {
    const GDALColorInterp bandColour = dataset->GetRasterBand(b)->GetColorInterpretation();
    if (bandColour == GCI_AlphaBand) {
      hasAlphaBand = true;
    } else if (rasterBandCount > 1 && bandColour != GCI_RedBand && bandColour != GCI_GreenBand &&
               bandColour != GCI_BlueBand && bandColour != GCI_GrayIndex) {
      // draw()'s ARGB32 compositing only understands Red/Green/Blue/Alpha/Gray; anything else
      // (e.g. GCI_Undefined, or one of the YCbCr/HSL-style tags) is silently not drawn there
      qWarning() << "CMapVRT:" << filename << "band" << b << "has unsupported color interpretation"
                 << GDALGetColorInterpretationName(bandColour) << "- it will not be drawn";
    }
  }
  const bool addDstAlphaBand = (rasterBandCount > 1) && !hasAlphaBand;

  // ------- setup warped VRT ---------------
  // if the projection of the dataset is different then that of the drawing context we wrap the dataset in a virtual
  // warped dataset to transparently resample it into the drawing contexts projection.
  OGRSpatialReference targetSRS;
  OGRErr rv = targetSRS.SetFromUserInput(map->getProjection().toUtf8());
  const OGRSpatialReference* sourceSRS = dataset->GetSpatialRef();
  if (rv == OGRERR_NONE && sourceSRS != nullptr && !sourceSRS->IsSame(&targetSRS)) {
    srcDataset = dataset;

    GDALWarpOptions* psOptions = GDALCreateWarpOptions();
    psOptions->pProgressArg = map;
    psOptions->pfnProgress = &CMapVRT::progressCallback;

    if (addDstAlphaBand) {
      GDALWarpInitDefaultBandMapping(psOptions, rasterBandCount);
      psOptions->nDstAlphaBand = psOptions->nBandCount + 1;
    }

    dataset = GDALDataset::FromHandle(GDALAutoCreateWarpedVRT(
        GDALDataset::ToHandle(srcDataset), nullptr, targetSRS.exportToWkt().c_str(), resampleAlg, 0.1, psOptions));

    GDALDestroyWarpOptions(psOptions);

    if (dataset == nullptr) {
      fail(tr("Failed to create Warp for:") % '\n' % filename);
      return;
    }

    // pick up the synthetic alpha band (if any) so draw() reads/composites it like any other band
    rasterBandCount = dataset->GetRasterCount();

    if (!overviewFactors.isEmpty()) {
      // attach them as virtual overviews so GDAL can serve a decimated read straight from
      // whichever source file(s) actually have a matching level, instead of always warping
      // at full resolution and only downsampling the output
      CPLSetConfigOption("VRT_VIRTUAL_OVERVIEWS", "YES");
      dataset->BuildOverviews("NONE", overviewFactors.size(), overviewFactors.data(), 0, nullptr, nullptr, nullptr);
      CPLSetConfigOption("VRT_VIRTUAL_OVERVIEWS", "NO");
    }
  }

  // ------- setup projection ---------------
  proj.init(dataset->GetProjectionRef(), "EPSG:4326");

  if (!proj.isValid()) {
    fail(tr("No georeference information found:") % '\n' % filename);
    return;
  }

  xsize_px = dataset->GetRasterXSize();
  ysize_px = dataset->GetRasterYSize();

  if (xsize_px <= 0 || ysize_px <= 0) {
    fail(tr("Raster has an invalid (zero) size:") % '\n' % filename);
    return;
  }

  qreal adfGeoTransform[6];
  if (dataset->GetGeoTransform(adfGeoTransform) != CE_None) {
    fail(tr("No pixel-to-map transform found:") % '\n' % filename);
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
    // Scale every element of the homogeneous matrix by DEG_TO_RAD to convert trFwd's
    // mapped output from degrees to radians. This works because QTransform::map() never
    // reads m13/m23/m33 as long as the transform stays non-projective (true here, since
    // trFwd is built only from translate/scale/shear) - so scaling the whole matrix
    // scales the mapped point without having to touch dx/dy and the linear part separately.
    trFwd = trFwd * DEG_TO_RAD;
  }

  trInv = trFwd.inverted();

  // ------- setup outline ---------------
  // ref1..ref4 outline the *original* (pre-warp) raster's true footprint, not the
  // (possibly enlarged, axis-aligned-in-target-SRS) warped VRT's pixel grid - so
  // drawOutline() still shows the real, potentially rotated shape of the source data
  // instead of the padded bounding box GDALAutoCreateWarpedVRT introduces.
  GDALDataset* origDataset = (srcDataset != nullptr) ? srcDataset : dataset;

  CProj projOrig;
  projOrig.init(origDataset->GetProjectionRef(), "EPSG:4326");

  qreal origGeoTransform[6];
  origDataset->GetGeoTransform(origGeoTransform);

  QTransform origTrFwd(origGeoTransform[1], origGeoTransform[4], origGeoTransform[2], origGeoTransform[5],
                       origGeoTransform[0], origGeoTransform[3]);
  if (projOrig.isSrcLatLong()) {
    origTrFwd = origTrFwd * DEG_TO_RAD;
  }

  QPolygonF outline;
  outline << origTrFwd.map(QPointF(0, 0)) << origTrFwd.map(QPointF(origDataset->GetRasterXSize(), 0))
          << origTrFwd.map(QPointF(origDataset->GetRasterXSize(), origDataset->GetRasterYSize()))
          << origTrFwd.map(QPointF(0, origDataset->GetRasterYSize()));
  projOrig.transform(outline, PJ_FWD);

  ref1 = outline[0];
  ref2 = outline[1];
  ref3 = outline[2];
  ref4 = outline[3];

  qDebug() << "FF" << trFwd;
  qDebug() << "RR" << trInv;

  isActivated = true;
}

CMapVRT::~CMapVRT() {
  closeDataset(dataset);
  closeDataset(srcDataset);
}

int CMapVRT::progressCallback(double /*dfComplete*/, const char* /*message*/, void* pProgressArg) {
  auto* drawCtx = reinterpret_cast<CMapDraw*>(pProgressArg);
  return !drawCtx->needsRedraw();
}

bool CMapVRT::allReferencedFilesExist(GDALDataset* dataset, QString& missingFile) {
  char** fileList = dataset->GetFileList();
  bool allExist = true;
  for (qint32 n = 0; fileList != nullptr && fileList[n] != nullptr; ++n) {
#if defined(Q_OS_WIN32)
    missingFile = QString::fromLocal8Bit(fileList[n]);
    if (QFileInfo::exists(missingFile)) {
      continue;
    }
#endif  // defined(Q_OS_WIN32)
    missingFile = QString::fromUtf8(fileList[n]);
    if (QFileInfo::exists(missingFile)) {
      continue;
    }
    allExist = false;
    break;
  }
  CSLDestroy(fileList);
  return allExist;
}

QVector<qint32> CMapVRT::collectOverviewFactors(GDALDataset* dataset, GDALRasterBand* pBand, qreal pixelSizeX) {
  QSet<qint32> factors;

  if (pBand->GetOverviewCount() != 0) {
    for (qint32 i = 0; i < pBand->GetOverviewCount(); ++i) {
      factors << qRound((qreal)pBand->GetXSize() / pBand->GetOverview(i)->GetXSize());
    }
  } else if (pixelSizeX > 0) {
    char** fileList = dataset->GetFileList();
    for (qint32 n = 0; fileList != nullptr && fileList[n] != nullptr; ++n) {
      GDALDatasetUniquePtr subDataset(GDALDataset::FromHandle(GDALOpen(fileList[n], GA_ReadOnly)));
      GDALRasterBand* subBand = subDataset ? subDataset->GetRasterBand(1) : nullptr;
      qreal subGeoTransform[6];
      if (subBand == nullptr || subBand->GetOverviewCount() == 0 ||
          subDataset->GetGeoTransform(subGeoTransform) != CE_None) {
        continue;
      }

      const qreal subPixelSizeX = qAbs(subGeoTransform[1]);
      for (qint32 i = 0; i < subBand->GetOverviewCount(); ++i) {
        const qreal overviewPixelSizeX = subPixelSizeX * subBand->GetXSize() / subBand->GetOverview(i)->GetXSize();
        factors << qRound(overviewPixelSizeX / pixelSizeX);
      }
    }
    CSLDestroy(fileList);
  }

  QVector<qint32> result(factors.begin(), factors.end());
  std::sort(result.begin(), result.end());
  return result;
}

void CMapVRT::closeDataset(GDALDataset*& dataset) {
  if (dataset != nullptr) {
    GDALClose(dataset);
    dataset = nullptr;
  }
}

void CMapVRT::fail(const QString& msg) {
  closeDataset(dataset);
  closeDataset(srcDataset);
  QMessageBox::warning(CMainWindow::getBestWidgetForParent(), tr("Error..."), msg);
}

bool CMapVRT::computeSourceWindow(const IDrawContext::buffer_t& buf, const QPointF& bufferScale,
                                  sourceWindow_t& window) const {
  // use bufferScale (and therefore the zoom level) and the pixel scale of the map to calculate a downsampling
  // factor; <1 would mean GDAL does upscaling which is pointless
  const qreal bufScaleX = qMax(1.0, qAbs(bufferScale.x() / xscale));
  const qreal bufScaleY = qMax(1.0, qAbs(bufferScale.y() / yscale));

  // corners of the area we shall draw, converted from the canvas projection into the
  // map's own pixel coordinate space
  auto toMapPixel = [this](QPointF pt) {
    proj.transform(pt, PJ_INV);
    return trInv.map(pt);
  };
  const QPointF pt1 = toMapPixel(buf.ref1);
  const QPointF pt2 = toMapPixel(buf.ref2);
  const QPointF pt3 = toMapPixel(buf.ref3);
  const QPointF pt4 = toMapPixel(buf.ref4);

  // bounds of the area to draw in the coordinate space of the map
  qreal left = std::min({pt1.x(), pt2.x(), pt3.x(), pt4.x()});
  qreal right = std::max({pt1.x(), pt2.x(), pt3.x(), pt4.x()});
  qreal top = std::min({pt1.y(), pt2.y(), pt3.y(), pt4.y()});
  qreal bottom = std::max({pt1.y(), pt2.y(), pt3.y(), pt4.y()});

  const bool intersectsMap = (top <= ysize_px) && (left <= xsize_px) && (bottom >= 0) && (right >= 0);
  if (!intersectsMap) {
    return false;
  }

  // current view intersects the bounds of the map, so clip to them
  left = std::clamp(left, 0.0, (qreal)xsize_px);
  top = std::clamp(top, 0.0, (qreal)ysize_px);
  right = std::clamp(right, 0.0, (qreal)xsize_px);
  bottom = std::clamp(bottom, 0.0, (qreal)ysize_px);

  if ((right <= left) || (bottom <= top)) {
    return false;
  }

  window.left = left;
  window.top = top;
  window.right = right;
  window.bottom = bottom;
  // dimensions of the buffer GDAL will read into; requesting a different size than the size of the source
  // window lets GDAL do the scaling for us and use overviews
  window.bufWidth = qMax(1, qRound((right - left) / bufScaleX));
  window.bufHeight = qMax(1, qRound((bottom - top) / bufScaleY));
  return true;
}

QImage CMapVRT::readSourceImage(const sourceWindow_t& window) {
  const qreal w_map = window.right - window.left;
  const qreal h_map = window.bottom - window.top;
  const qint32 w_buf = window.bufWidth;
  const qint32 h_buf = window.bufHeight;

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

  QImage img;
  CPLErr err = CE_Failure;
  if (rasterBandCount == 1) {
    // backs img's pixel buffer: QImage's own row alignment may pad bytesPerLine() beyond
    // w_buf, so we read into a tightly packed member buffer instead (resized, not
    // reallocated, across draw() calls) and hand it to QImage with an explicit
    // bytesPerLine; it outlives img below since indexData is a member, not a local
    indexData.resize(static_cast<qsizetype>(w_buf) * h_buf);

    err = dataset->GetRasterBand(1)->ReadRaster(indexData.data(), static_cast<size_t>(indexData.size()), window.left,
                                                window.top, w_map, h_map, w_buf, h_buf, GRIORA_NearestNeighbour,
                                                &CMapVRT::progressCallback, map);
    if (err == CE_None) {
      img = QImage(indexData.constData(), w_buf, h_buf, w_buf, QImage::Format_Indexed8);
      img.setColorTable(colortable);
    }
  } else {
    img = QImage(w_buf, h_buf, QImage::Format_ARGB32);
    img.fill(qRgba(255, 255, 255, 255));

    bandBuf.resize(static_cast<qsizetype>(w_buf) * h_buf);
    const QRgb testPix = qRgba(GCI_RedBand, GCI_GreenBand, GCI_BlueBand, GCI_AlphaBand);

    // byte offset of channel within one ARGB32 pixel - exploits testPix's in-memory
    // layout, where each component ends up at the offset matching its own enum value
    // since GCI_RedBand/GreenBand/BlueBand/AlphaBand happen to be 3/4/5/6
    auto offsetOf = [&testPix](GDALColorInterp channel) {
      quint32 offset = 0;
      while (offset < sizeof(testPix) && *(((const quint8*)&testPix) + offset) != channel) {
        ++offset;
      }
      return offset;
    };
    auto copyBandInto = [&](quint32 offset) {
      quint8* pTar = img.bits() + offset;
      const quint8* pSrc = bandBuf.constData();
      for (qsizetype i = 0; i < bandBuf.size(); ++i) {
        *pTar = *pSrc;
        pTar += sizeof(testPix);
        pSrc += 1;
      }
    };
    const quint32 offsetR = offsetOf(GCI_RedBand);
    const quint32 offsetG = offsetOf(GCI_GreenBand);
    const quint32 offsetB = offsetOf(GCI_BlueBand);

    for (qint32 b = 1; b <= rasterBandCount; ++b) {
      GDALRasterBand* pBand = dataset->GetRasterBand(b);

      err = pBand->ReadRaster(bandBuf.data(), static_cast<size_t>(bandBuf.size()), window.left, window.top, w_map,
                              h_map, w_buf, h_buf, GRIORA_Bilinear, &CMapVRT::progressCallback, map);
      if (err != CE_None) {
        break;
      }

      const GDALColorInterp pbandColour = pBand->GetColorInterpretation();
      if (pbandColour == GCI_GrayIndex) {
        // a lone gray channel among other bands (e.g. gray+alpha) - duplicate it into
        // R, G and B rather than dropping it
        copyBandInto(offsetR);
        copyBandInto(offsetG);
        copyBandInto(offsetB);
      } else {
        const quint32 offset = offsetOf(pbandColour);
        if (offset < sizeof(testPix)) {
          copyBandInto(offset);
        }
      }
    }
  }

  return (err == CE_None) ? img : QImage();
}

void CMapVRT::drawSourceImage(QPainter& p, const sourceWindow_t& window, const QImage& img) const {
  // Map img onto screen with the exact affine transform instead of an axis-aligned
  // QRectF. GDAL's warp (in the ctor) already resolved any cross-projection curvature
  // into the dataset's own pixel grid, and proj.transform(PJ_FWD) here is the exact
  // inverse of what map->convertRad2Px() does internally to get back to this view's
  // CRS, so the two cancel to identity. What's left is trFwd: a fixed matrix that can
  // carry shear/rotation from a non-north-up source geotransform, composed with
  // convertRad2Px()'s affine pan/zoom - both affine, so 3 corners fully determine the
  // map; no curve-fitting/subdivision (as used for genuinely curved projections
  // elsewhere) is needed.
  auto toScreenPx = [this](QPointF pt) {
    pt = trFwd.map(pt);
    proj.transform(pt, PJ_FWD);
    map->convertRad2Px(pt);
    return pt;
  };
  const QPointF s0 = toScreenPx(QPointF(window.left, window.top));
  const QPointF s1 = toScreenPx(QPointF(window.right, window.top));
  const QPointF s3 = toScreenPx(QPointF(window.left, window.bottom));

  const QTransform imgToScreen((s1.x() - s0.x()) / window.bufWidth, (s1.y() - s0.y()) / window.bufWidth,
                               (s3.x() - s0.x()) / window.bufHeight, (s3.y() - s0.y()) / window.bufHeight, s0.x(),
                               s0.y());

  p.save();
  p.setTransform(imgToScreen, true);
  p.drawImage(QPointF(0, 0), img);
  p.restore();
}

void CMapVRT::drawOutline(QPainter& p) const {
  QPolygonF boundingBox;
  boundingBox << ref1 << ref2 << ref3 << ref4;
  map->convertRad2Px(boundingBox);

  p.setPen(Qt::black);
  p.setBrush(Qt::NoBrush);
  p.drawPolygon(boundingBox);
}

void CMapVRT::draw(IDrawContext::buffer_t& buf) /* override */
{
  if (map->needsRedraw()) {
    return;
  }

  const QPointF bufferScale = buf.scale * buf.zoomFactor;

  // get pixel offset of top left buffer corner
  QPointF pp = buf.ref1;
  map->convertRad2Px(pp);

  // start to draw the map
  QPainter p(&buf.image);
  USE_ANTI_ALIASING(p, true);
  p.setOpacity(getOpacity() / 100.0);
  p.translate(-pp);

  sourceWindow_t window;
  if (!isOutOfScale(bufferScale) && computeSourceWindow(buf, bufferScale, window)) {
    const QImage img = readSourceImage(window);
    if (!img.isNull()) {
      drawSourceImage(p, window, img);
    }
  }

  drawOutline(p);
}
