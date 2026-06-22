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

#ifndef IDEM_H
#define IDEM_H

#include <QObject>
#include <QPointer>

#include "canvas/IDrawContext.h"
#include "canvas/IDrawObject.h"
#include "gis/proj_x.h"

#define CUSTOM_SLOPE_COLORTABLE (-1)

class CDemDraw;
class IDemProp;
class QSettings;

struct SlopePresets {
  const char* name;
  const qreal steps[5];
};

class IDem : public IDrawObject {
  Q_OBJECT
 public:
  IDem(CDemDraw* parent);
  virtual ~IDem();

  void saveConfig(QSettings& cfg) override;

  void loadConfig(QSettings& cfg) override;

  virtual void draw(IDrawContext::buffer_t& buf) = 0;

  virtual qreal getElevationAt(const QPointF& pos, bool checkScale) = 0;
  virtual qreal getSlopeAt(const QPointF& pos, bool checkScale) = 0;

  bool activated() const { return isActivated; }

  /**
     @brief Get the dem's setup widget.

     As default an instance of CDemPropSetup is used. For other setups you have
     to override this method.

     @return A pointer to the widget. Use a smart pointer to store as the widget can be destroyed at any time
   */
  virtual IDemProp* getSetup();

  bool doHillshading() const { return bHillshading; }

  /**
     @brief Get the hillshading z-factor as the slider value that produced it.

     Reverses the encoding done by slotSetFactorHillshade(): a factor of 1.0 maps back
     to 0, factors > 1 map back to themselves, and factors < 1 map back to -1/factor.

     @note slider values -1, 0 and +1 all encode the same neutral factor 1.0, so this
           always reports 0 for any of them. The mapping is not perfectly invertible
           at that point; it is treated as a harmless dead zone rather than fixed.

     @return the slider value
   */
  int getFactorHillshading() const;

  bool doSlopeShading() const { return bSlopeShading; }

  int getFactorSlopeShading() const;

  bool doSlopeColor() const { return bSlopeColor; }

  bool doElevationLimit() const { return bElevationLimit; }

  int getElevationLimit() const { return elevationValue; }

  bool doElevationShading() const { return bElevationShading; }

  int getElevationShadeLimitLow() const { return elevationShadeLimitLow; }

  int getElevationShadeLimitHi() const { return elevationShadeLimitHi; }

  const QVector<QRgb> getSlopeColorTable() const { return slopetable; }

  bool doShowElevationShadeScale() const { return bShowElevationShadeScale; }

  static const struct SlopePresets slopePresets[7];
  static const size_t slopePresetCount = sizeof(IDem::slopePresets) / sizeof(IDem::slopePresets[0]);

  const qreal* getCurrentSlopeStepTable() const;

  int getSlopeStepTableIndex() const { return gradeSlopeColor; }

  void setSlopeStepTable(int idx);
  void setSlopeStepTableCustomValue(int idx, int val);
  void setElevationLimit(int val);

  void initElevationShadeTable();
  void setElevationShadeLow(int val);
  void setElevationShadeHi(int val);

  enum winsize_e { eWinsize3x3 = 9, eWinsize4x4 = 16 };

 public slots:
  void slotSetHillshading(bool yes) { bHillshading = yes; }

  /**
     @brief Set the hillshading z-factor (vertical exaggeration) from a slider value.

     One integer slider covers both >1 and <1 factors symmetrically:
     - f == 0: neutral factor 1.0
     - f > 0:  direct multiplier f
     - f < 0:  fractional reduction 1/|f|

     getFactorHillshading() reverses this encoding.

     @param f the slider value
   */
  void slotSetFactorHillshade(int f);

  void slotSetSlopeShading(bool yes) { bSlopeShading = yes; }

  void slotSetFactorSlopeShade(int f);

  void slotSetSlopeColor(bool yes) { bSlopeColor = yes; }

  void slotSetElevationLimit(bool yes) { bElevationLimit = yes; }

  void slotSetElevationShading(bool yes) { bElevationShading = yes; }

  void slotShowElevationShadeScale(bool yes);

 protected:
  /**
     @brief Per-pixel shading algorithms shared by all DEM backends.

     @param data   raw elevation samples, kept 1px wider/taller than `out` on every
                    side so each pixel's 3x3 (or 4x4) neighborhood window can be read
                    even at the edges of the requested area. Indexed with stride+2.
     @param out    output buffer, `w` x `h` pixels at row stride `stride`
     @param x, y   top-left corner of the chunk to process, in `out`/`stride` coordinates
     @param stride row stride of `out`, in pixels
     @param w, h   size of the chunk to process
   */
  void hillshading(const QVector<float>& data, QVector<uchar>& out, quint32 x, quint32 y, quint32 stride, quint32 w,
                   quint32 h) const;

  void slopeShading(const QVector<float>& data, QVector<uchar>& out, quint32 x, quint32 y, quint32 stride, quint32 w,
                    quint32 h) const;

  void slopecolor(const QVector<float>& data, QVector<uchar>& out, quint32 x, quint32 y, quint32 stride, quint32 w,
                  quint32 h) const;

  void elevationLimit(const QVector<float>& data, QVector<uchar>& out, quint32 x, quint32 y, quint32 stride, quint32 w,
                      quint32 h) const;

  void elevationShading(const QVector<float>& data, QVector<uchar>& out, quint32 x, quint32 y, quint32 stride,
                        quint32 w, quint32 h) const;

  /**
     @brief Elevation, in the user's configured unit, of the highest valid sample in a
            3x3 window.

     Uses the maximum rather than the mean so a single high pixel in the neighborhood is
     enough to flag/color the whole window - keeps thin ridgelines visible at the
     elevation-limit/elevation-shading resolution. noData samples are ignored, unless
     the DEM has no noData value at all (hasNoData == false), in which case every sample
     is treated as valid.

     @param win 3x3 window of raw elevation samples in meters (see fillWindow())
     @return elevation in the user's configured unit
   */
  qreal maxElevationInWindow(const float* win) const;

  /**
     @brief Bilinear interpolation between the 4 corners of a unit square.
     @param a top-left value (x=0, y=0)
     @param b top-right value (x=1, y=0)
     @param c bottom-left value (x=0, y=1)
     @param d bottom-right value (x=1, y=1)
     @param x Fractional value (0..1) for interpolation in x
     @param y Fractional value (0..1) for interpolation in y
     @return  Interpolated value
   */
  static qreal bilinear(qreal a, qreal b, qreal c, qreal d, qreal x, qreal y);

  /**
     @brief Slope in degrees based on a window. Origin is at point (1,1), counting from zero.
     @param win2  window data
     @param size  size of window (eWinsize3x3 or eWinsize4x4)
     @param x     Fractional value (0..1) for interpolation in x (4x4 window only)
     @param y     Fractional value (0..1) for interpolation in y (4x4 window only)
     @return      Slope in degrees
   */
  qreal slopeOfWindowInterp(float* win2, winsize_e size, qreal x, qreal y) const;

  CDemDraw* dem;

  /**
      target should always be "EPSG:4326"
      source will be the map's projection
   */
  CProj proj;

  /// width in number of px
  qint32 xsize_px = 0;
  /// height in number of px
  qint32 ysize_px = 0;

  /// scale [px/m]
  qreal xscale = 1.0;
  /// scale [px/m]
  qreal yscale = 1.0;

  /**
     @brief True if map was loaded successfully
   */
  bool isActivated = false;

  /// the setup dialog. Use getSetup() for access
  QPointer<IDemProp> setup;

  QVector<QRgb> graytable;

  QVector<QRgb> elevationShadeTable;

  QVector<QRgb> slopetable;

  QVector<QRgb> elevationtable;

  int hasNoData = 0;

  double noData = 0;

 private:
  bool bHillshading = false;
  qreal factorHillshading = 1.0 / 6.0;
  bool bSlopeShading = false;
  qreal factorSlopeShading = 1.0;
  bool bSlopeColor = false;
  bool bElevationLimit = false;
  int gradeSlopeColor = 0;
  qreal slopeCustomStepTable[5] = {5.0, 10.0, 15.0, 20.0, 25.0};
  int elevationValue = 0;
  bool bElevationShading = false;
  int elevationShadeLimitLow = 0;
  int elevationShadeLimitHi = 0;
  bool bShowElevationShadeScale = false;
};

#endif  // IDEM_H
