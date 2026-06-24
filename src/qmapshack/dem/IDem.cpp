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

#include "dem/IDem.h"

#include <QtWidgets>

#include "dem/CDemDraw.h"
#include "dem/CDemPropSetup.h"

// Read one sample from a row-major buffer of row width dx.
template <typename T>
inline T getValue(const QVector<T>& data, int x, int y, int dx) {
  return data[x + y * dx];
}

// Fill w[0..8] with the row-major 3x3 neighborhood of data centered on (x, y):
//   w[0] w[1] w[2]      (x-1,y-1) (x,y-1) (x+1,y-1)
//   w[3] w[4] w[5]  i.e. (x-1,y)  (x,y)   (x+1,y)
//   w[6] w[7] w[8]      (x-1,y+1) (x,y+1) (x+1,y+1)
// Callers pass an (x, y) that is already offset into data's 1px border (see the @param
// data docs on IDem::computeShading()), so the -1/+1 neighbors are always in bounds.
template <typename T>
inline void fillWindow(const QVector<T>& data, int x, int y, int stride, T* w) {
  w[0] = getValue(data, x - 1, y - 1, stride);
  w[1] = getValue(data, x, y - 1, stride);
  w[2] = getValue(data, x + 1, y - 1, stride);
  w[3] = getValue(data, x - 1, y, stride);
  w[4] = getValue(data, x, y, stride);
  w[5] = getValue(data, x + 1, y, stride);
  w[6] = getValue(data, x - 1, y + 1, stride);
  w[7] = getValue(data, x, y + 1, stride);
  w[8] = getValue(data, x + 1, y + 1, stride);
}

const struct SlopePresets IDem::slopePresets[7]{
    /* http://www.alpenverein.de/bergsport/sicherheit/skitouren-schneeschuh-sicher-im-schnee/dav-snowcard_aid_10619.html
     */
    {"Grade 1 (DAV Snowcard)", {27.0, 31.0, 34.0, 39.0, 50.0}},
    {"Grade 2 (DAV Snowcard)", {27.0, 30.0, 32.0, 35.0, 39.0}},
    {"Grade 3 (DAV Snowcard)", {27.0, 29.0, 30.0, 31.0, 34.0}},
    {"Grade 4 (DAV Snowcard)", {23.0, 25.0, 27.0, 28.0, 30.0}},

    {"level country", {3.0, 6.0, 8.0, 12.0, 15.0}},
    {"secondary mountain", {4.0, 7.0, 10.0, 15.0, 20.0}},
    {"lofty mountain", {10.0, 15.0, 20.0, 30.0, 50.0}}};

IDem::IDem(CDemDraw* parent) : IDrawObject(parent), dem(parent) {
  slotSetOpacity(17);

  graytable.resize(256);
  for (int i = 0; i < 255; i++) {
    graytable[i] = qRgba(i, i, i, 255);
  }
  graytable[255] = qRgba(0, 0, 0, 0);

  initElevationShadeTable();

  slopetable << qRgba(0, 0, 0, 0);
  slopetable << qRgba(0, 128, 0, 100);
  slopetable << qRgba(0, 255, 0, 100);
  slopetable << qRgba(255, 255, 0, 100);
  slopetable << qRgba(255, 128, 0, 100);
  slopetable << qRgba(255, 0, 0, 100);

  elevationtable << qRgba(0, 0, 0, 0);        // transparent
  elevationtable << qRgba(255, 0, 255, 128);  // magenta
}

IDem::~IDem() {}

void IDem::saveConfig(QSettings& cfg) {
  IDrawObject::saveConfig(cfg);

  cfg.setValue("doHillshading", bHillshading);
  cfg.setValue("factorHillshading", factorHillshading);
  cfg.setValue("doSlopeShading", bSlopeShading);
  cfg.setValue("factorSlopeShading", factorSlopeShading);
  cfg.setValue("doSlopeColor", bSlopeColor);

  cfg.setValue("gradeSlopeColor", gradeSlopeColor);
  cfg.setValue("slopeCustomValue0", slopeCustomStepTable[0]);
  cfg.setValue("slopeCustomValue1", slopeCustomStepTable[1]);
  cfg.setValue("slopeCustomValue2", slopeCustomStepTable[2]);
  cfg.setValue("slopeCustomValue3", slopeCustomStepTable[3]);
  cfg.setValue("slopeCustomValue4", slopeCustomStepTable[4]);

  cfg.setValue("doElevationLimit", bElevationLimit);
  cfg.setValue("elevationValue", elevationValue);

  cfg.setValue("doElevationShading", bElevationShading);
  cfg.setValue("elevationShadeLimitLow", elevationShadeLimitLow);
  cfg.setValue("elevationShadeLimitHi", elevationShadeLimitHi);
  cfg.setValue("showElevationShadeScale", bShowElevationShadeScale);
}

void IDem::loadConfig(QSettings& cfg) {
  IDrawObject::loadConfig(cfg);

  bHillshading = cfg.value("doHillshading", bHillshading).toBool();
  factorHillshading = cfg.value("factorHillshading", factorHillshading).toFloat();
  bSlopeShading = cfg.value("doSlopeShading", bSlopeShading).toBool();
  factorSlopeShading = cfg.value("factorSlopeShading", factorSlopeShading).toFloat();
  bSlopeColor = cfg.value("doSlopeColor", bSlopeColor).toBool();
  gradeSlopeColor = cfg.value("gradeSlopeColor", gradeSlopeColor).toInt();

  slopeCustomStepTable[0] = cfg.value("slopeCustomValue0", 5.).toFloat();
  slopeCustomStepTable[1] = cfg.value("slopeCustomValue1", 10.).toFloat();
  slopeCustomStepTable[2] = cfg.value("slopeCustomValue2", 15.).toFloat();
  slopeCustomStepTable[3] = cfg.value("slopeCustomValue3", 20.).toFloat();
  slopeCustomStepTable[4] = cfg.value("slopeCustomValue4", 25.).toFloat();

  bElevationLimit = cfg.value("doElevationLimit", bElevationLimit).toBool();
  elevationValue = cfg.value("elevationValue", 0).toInt();

  bElevationShading = cfg.value("doElevationShading", bElevationShading).toBool();
  elevationShadeLimitLow = cfg.value("elevationShadeLimitLow", 0).toInt();
  elevationShadeLimitHi = cfg.value("elevationShadeLimitHi", 0).toInt();
  bShowElevationShadeScale = cfg.value("showElevationShadeScale", bShowElevationShadeScale).toBool();
}

IDemProp* IDem::getSetup() {
  if (setup.isNull()) {
    setup = new CDemPropSetup(this, dem);
  }

  return setup;
}

void IDem::slotSetFactorHillshade(int f) {
  if (f == 0) {
    factorHillshading = 1.0;
  } else if (f < 0) {
    factorHillshading = -1.0 / f;
  } else {
    factorHillshading = f;
  }
}

void IDem::slotSetFactorSlopeShade(int f) { factorSlopeShading = f / 100.; }

void IDem::setSlopeStepTableCustomValue(int idx, int val) { slopeCustomStepTable[idx] = (qreal)val; }

void IDem::setElevationLimit(int val) { elevationValue = val; }

void IDem::setElevationShadeLow(int val) { elevationShadeLimitLow = val; }

void IDem::setElevationShadeHi(int val) { elevationShadeLimitHi = val; }

void IDem::setSlopeStepTable(int idx) {
  gradeSlopeColor = idx;
  dem->emitSigCanvasUpdate();
}

void IDem::initElevationShadeTable() {
  elevationShadeTable.resize(256);

  elevationShadeTable[0] = qRgba(75, 75, 75, 255);
  for (int i = 0; i < 254; i++) {
    const QColor color = QColor::fromHsv(240. * (253 - i) / 253, 255, 255);
    elevationShadeTable[i + 1] = color.rgb();
  }
  elevationShadeTable[255] = qRgba(180, 180, 180, 255);
}

const qreal* IDem::getCurrentSlopeStepTable() const {
  if (CUSTOM_SLOPE_COLORTABLE == gradeSlopeColor) {
    return slopeCustomStepTable;
  } else {
    return slopePresets[gradeSlopeColor].steps;
  }
}

int IDem::getFactorHillshading() const {
  if (factorHillshading == 1.0) {
    return 0;
  } else if (factorHillshading < 1) {
    return qRound(-1.0 / factorHillshading);
  } else {
    return qRound(factorHillshading);
  }
}

void IDem::computeShading(const QVector<float>& data, const ShadingBuffers& buffers, quint32 x, quint32 y,
                          quint32 stride, quint32 w, quint32 h) const {
  // 3 independent passes - hillshade, slopeShade+slopeColor, elevationLimit+elevationShade -
  // each entered only if at least one of its layers is active, so "is this layer enabled"
  // is decided once per call rather than tested inside any pixel loop. slopeOfWindowInterp()
  // and maxElevationInWindow() are each computed at most once per pixel and shared between
  // the two layers in their pass, rather than recomputed once per layer.

  if (buffers.hillshade != nullptr) {
    // Horn's method (the same algorithm GDAL's own "gdaldem hillshade" uses): dx/dy are a
    // Sobel-style gradient (hence the doubled middle terms), combined with a fixed light
    // source (azimuth 315° = NW, altitude 45° - only the z-factor/vertical exaggeration is
    // user-configurable, via factorHillshading) into cang, the cosine of the light's
    // incidence angle. cang is then remapped from its natural -1..1 range to the 1..254
    // output range, leaving 0 unused and 255 reserved to mark noData (transparent in
    // graytable).
    constexpr qreal zFactor = 0.125;
    constexpr qreal zFactorSquared = zFactor * zFactor;
    constexpr qreal azimuth = 315 * DEG_TO_RAD;
    const qreal sinAltitude = qSin(45 * DEG_TO_RAD);
    const qreal zFactorCosAltitude = zFactor * qCos(45 * DEG_TO_RAD);
    const qreal cosAzimuth = qCos(azimuth);
    const qreal sinAzimuth = qSin(azimuth);
    // xscale/yscale/factorHillshading are loop-invariant; precompute the reciprocals once so
    // the per-pixel gradient is a multiply instead of a divide.
    const qreal invXScale = 1.0 / (xscale * factorHillshading);
    const qreal invYScale = 1.0 / (yscale * factorHillshading);

    for (unsigned int m = 0; m < h; m++) {
      quint8* scan = buffers.hillshade->data() + (m + y) * stride + x;
      for (unsigned int n = 0; n < w; n++) {
        float win[eWinsize3x3];
        fillWindow(data, n + x + 1, m + y + 1, stride + 2, win);

        if (hasNoData && win[4] == noData) {
          scan[n] = 255;
          continue;
        }

        qreal dx = ((win[0] + win[3] + win[3] + win[6]) - (win[2] + win[5] + win[5] + win[8])) * invXScale;
        qreal dy = ((win[6] + win[7] + win[7] + win[8]) - (win[0] + win[1] + win[1] + win[2])) * invYScale;
        qreal xx_plus_yy = dx * dx + dy * dy;
        // r*sin(aspect-azimuth), with aspect=atan2(dy,dx) and r=sqrt(xx_plus_yy): expand via
        // the angle-subtraction identity and substitute sin(aspect)=dy/r, cos(aspect)=dx/r -
        // the r cancels algebraically, so this also covers dx=dy=0 without a singularity,
        // while skipping atan2()/sin() entirely.
        qreal rSinAspectMinusAzimuth = dy * cosAzimuth - dx * sinAzimuth;
        qreal cang =
            (sinAltitude - zFactorCosAltitude * rSinAspectMinusAzimuth) / qSqrt(1 + zFactorSquared * xx_plus_yy);
        scan[n] = (cang <= 0.0) ? 1 : static_cast<quint8>(1.0 + 254.0 * cang);
      }
    }
  }

  if (buffers.slopeShade != nullptr || buffers.slopeColor != nullptr) {
    const bool wantSlopeShade = buffers.slopeShade != nullptr;
    const bool wantSlopeColor = buffers.slopeColor != nullptr;
    const qreal* currentSlopeStepTable = wantSlopeColor ? getCurrentSlopeStepTable() : nullptr;

    for (unsigned int m = 0; m < h; m++) {
      quint8* shadeScan = wantSlopeShade ? buffers.slopeShade->data() + (m + y) * stride + x : nullptr;
      quint8* colorScan = wantSlopeColor ? buffers.slopeColor->data() + (m + y) * stride + x : nullptr;
      for (unsigned int n = 0; n < w; n++) {
        float win[eWinsize3x3];
        fillWindow(data, n + x + 1, m + y + 1, stride + 2, win);
        const qreal slope = slopeOfWindowInterp(win, eWinsize3x3, 0, 0);

        if (wantSlopeShade) {
          shadeScan[n] = slopeShadeByte(slope);
        }
        if (wantSlopeColor) {
          colorScan[n] = slopeColorByte(slope, currentSlopeStepTable);
        }
      }
    }
  }

  if (buffers.elevationLimit != nullptr || buffers.elevationShade != nullptr) {
    const bool wantElevationLimit = buffers.elevationLimit != nullptr;
    const bool wantElevationShade = buffers.elevationShade != nullptr;
    const int elevationLimitLow = std::min(getElevationShadeLimitLow(), getElevationShadeLimitHi());
    const int elevationLimitHi = std::max(getElevationShadeLimitLow(), getElevationShadeLimitHi());

    for (unsigned int m = 0; m < h; m++) {
      quint8* limitScan = wantElevationLimit ? buffers.elevationLimit->data() + (m + y) * stride + x : nullptr;
      quint8* shadeScan = wantElevationShade ? buffers.elevationShade->data() + (m + y) * stride + x : nullptr;
      for (unsigned int n = 0; n < w; n++) {
        float win[eWinsize3x3];
        fillWindow(data, n + x + 1, m + y + 1, stride + 2, win);
        const qreal elevation = maxElevationInWindow(win);

        if (wantElevationLimit) {
          limitScan[n] = elevationLimitByte(elevation);
        }
        if (wantElevationShade) {
          shadeScan[n] = elevationShadeByte(elevation, elevationLimitLow, elevationLimitHi);
        }
      }
    }
  }
}

int IDem::getFactorSlopeShading() const { return qRound(factorSlopeShading * 100.); }

quint8 IDem::slopeShadeByte(qreal slope) const {
  if (slope == NOFLOAT) {
    return 0;
  }
  int alphaValue = slope * 255. / 90.     // map slope angle to alpha [0 .. 255]
                   * factorSlopeShading;  // apply slider value [0.25 .. 3.0]
  if (alphaValue > 255) {
    alphaValue = 255;
  }
  return alphaValue;
}

qreal IDem::bilinear(qreal a, qreal b, qreal c, qreal d, qreal x, qreal y) {
  return a + x * (b - a) + y * (c - a) + x * y * (a - b - c + d);
}

qreal IDem::slopeOfWindowInterp(float* win2, winsize_e size, qreal x, qreal y) const {
  for (int i = 0; i < size; i++) {
    if (hasNoData && win2[i] == noData) {
      return NOFLOAT;
    }
  }

  qreal win[eWinsize3x3];
  switch (size) {
    case eWinsize3x3:
      for (int i = 0; i < 9; i++) {
        win[i] = win2[i];
      }
      break;

    case eWinsize4x4:
      // win2 is a 4x4 grid (row-major, indices 0..15); slide a bilinearly-interpolated
      // 2x2 sample over it at every one of the resulting 3x3 window's positions, so win
      // ends up as the 3x3 neighborhood of the fractional point (x, y) would have, had
      // the raster actually been sampled there instead of at the nearest pixel.
      win[0] = bilinear(win2[0], win2[1], win2[4], win2[5], x, y);
      win[1] = bilinear(win2[1], win2[2], win2[5], win2[6], x, y);
      win[2] = bilinear(win2[2], win2[3], win2[6], win2[7], x, y);

      win[3] = bilinear(win2[4], win2[5], win2[8], win2[9], x, y);
      win[4] = bilinear(win2[5], win2[6], win2[9], win2[10], x, y);
      win[5] = bilinear(win2[6], win2[7], win2[10], win2[11], x, y);

      win[6] = bilinear(win2[8], win2[9], win2[12], win2[13], x, y);
      win[7] = bilinear(win2[9], win2[10], win2[13], win2[14], x, y);
      win[8] = bilinear(win2[10], win2[11], win2[14], win2[15], x, y);
      break;

    default:
      return NOFLOAT;
  }

  // same Sobel-style gradient as computeShading()'s hillshading branch; 8 is the kernel's total weight (1+2+1 on
  // each side), normalizing dx/dy back to an average per-unit-distance slope
  qreal dx = ((win[0] + win[3] + win[3] + win[6]) - (win[2] + win[5] + win[5] + win[8])) / (xscale);
  qreal dy = ((win[6] + win[7] + win[7] + win[8]) - (win[0] + win[1] + win[1] + win[2])) / (yscale);
  qreal k = dx * dx + dy * dy;
  qreal slope = qAtan(qSqrt(k) / (8 * 1.0)) * 180.0 / M_PI;

  return slope;
}

quint8 IDem::slopeColorByte(qreal slope, const qreal* slopeStepTable) const {
  if (slope == NOFLOAT) {
    return 0;
  }
  if (slope > slopeStepTable[4]) {
    return 5;
  } else if (slope > slopeStepTable[3]) {
    return 4;
  } else if (slope > slopeStepTable[2]) {
    return 3;
  } else if (slope > slopeStepTable[1]) {
    return 2;
  } else if (slope > slopeStepTable[0]) {
    return 1;
  } else {
    return 0;
  }
}

qreal IDem::maxElevationInWindow(const float* win) const {
  qreal meters = -2.0;
  for (unsigned int i = 0; i < eWinsize3x3; i++) {
    if ((!hasNoData || win[i] != noData) && win[i] > meters) {
      meters = win[i];
    }
  }

  // Skip IUnit::meter2elevation()'s virtual call and its QString unit assignment (which
  // would be discarded here anyway): this runs once per output pixel across multiple
  // threadPool threads, and unit's shared QString refcount becomes a cross-core
  // contention point otherwise. elevationFactor is the exact scale meter2elevation()
  // applies internally.
  return (meters == NOFLOAT) ? NOFLOAT : meters * IUnit::self().elevationFactor;
}

quint8 IDem::elevationLimitByte(qreal elevation) const { return (elevation >= getElevationLimit()) ? 1 : 0; }

quint8 IDem::elevationShadeByte(qreal elevation, int limitLow, int limitHi) const {
  if (elevation < limitLow) {
    return 0;
  } else if (elevation < limitHi) {
    const qreal relLimit = (elevation - limitLow) / (limitHi - limitLow);
    return 1 + relLimit * 253;
  } else {
    return 255;
  }
}

void IDem::slotShowElevationShadeScale(bool yes) { bShowElevationShadeScale = yes; }
