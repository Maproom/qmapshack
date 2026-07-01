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

/// One named preset of the 5 slope-angle thresholds used by IDem::slopeColorByte(); see
/// IDem::slopePresets and IDem::getCurrentSlopeStepTable().
struct SlopePresets {
  const char* name;
  const qreal steps[5];
};

/**
   @brief Abstract base class for a Digital Elevation Model (DEM) drawable.

   IDem renders a raster elevation grid as one or more optional shading layers on top of
   the map: hillshading, slope-angle shading (alpha or graded color), an elevation
   threshold overlay, and a continuous elevation-color gradient with an optional legend.
   It also answers point queries for elevation and slope (e.g. for track elevation
   profiles).

   A concrete subclass owns the actual data source (currently CDemVRT, backed by GDAL,
   and CDemWCS, a thin specialization of it) and is responsible for:
   - loading/opening the source and setting isActivated, proj, xsize_px/ysize_px and
     xscale/yscale once during construction,
   - implementing draw(), which decodes the requested view rectangle, reads the raw
     elevation samples via the subclass's own data access, and feeds them to
     computeShading() below,
   - implementing getElevationAt()/getSlopeAt() for point queries.

   @note computeShading() computes every enabled layer for one chunk in a single pass and
         is const/reentrant by design: CDemVRT::draw() calls it in parallel via a
         QThreadPool over disjoint chunks of the same output buffers, so it (and the
         per-layer kernels it calls) must never mutate IDem state.

   Configuration (which layers are enabled, their parameters, color tables, etc.) is
   persisted via saveConfig()/loadConfig() and changed at runtime through the public
   slots below, typically wired up from CDemPropSetup's UI.
 */
class IDem : public IDrawObject {
  Q_OBJECT
 public:
  IDem(CDemDraw* parent);
  virtual ~IDem();

  /// @brief Persist the layer enable flags, their parameters and color-table selection.
  void saveConfig(QSettings& cfg) override;

  /// @brief Restore what saveConfig() persisted.
  void loadConfig(QSettings& cfg) override;

  /**
     @brief Render the requested view rectangle's enabled shading layers into buf.image.

     Called from the canvas's draw thread. Implementations are expected to read only the
     part of their data source the current view covers and to be a no-op if the object
     is out of its configured min/max scale or no shading layer is enabled.

     @param buf the canvas buffer to draw into; also carries the view's corner
                coordinates and zoom/scale factors
   */
  virtual void draw(IDrawContext::buffer_t& buf) = 0;

  /**
     @brief Look up the elevation at a single point.
     @param pos        position in WGS84 [rad]
     @param checkScale if true, return NOFLOAT when the object is out of its configured
                        min/max draw scale (see IDrawObject::isOutOfScale())
     @return elevation in meters, or NOFLOAT if pos is outside the data source's coverage
   */
  virtual qreal getElevationAt(const QPointF& pos, bool checkScale) = 0;

  /**
     @brief Look up the terrain slope at a single point.
     @param pos        position in WGS84 [rad]
     @param checkScale if true, return NOFLOAT when the object is out of its configured
                        min/max draw scale (see IDrawObject::isOutOfScale())
     @return slope in degrees, or NOFLOAT if pos is outside the data source's coverage
   */
  virtual qreal getSlopeAt(const QPointF& pos, bool checkScale) = 0;

  /// @brief True once the data source has been opened/loaded successfully.
  bool activated() const { return isActivated; }

  /**
     @brief True if this DEM's overview pyramid needs attention (missing or too shallow)
            and that hasn't been suppressed for this file.

     Always false except for CDemVRT, where it drives the proactive warning badge in
     CMapItemDelegate - the same underlying advice used by the reactive advisory dialog
     that fires on a render timeout, just queried ahead of any render.
   */
  virtual bool showsOverviewWarning() const { return false; }

  /**
     @brief Get the dem's setup widget.

     As default an instance of CDemPropSetup is used. For other setups you have
     to override this method.

     @return A pointer to the widget. Use a smart pointer to store as the widget can be destroyed at any time
   */
  virtual IDemProp* getSetup();

  /// @brief True if the hillshading layer is enabled; see slotSetHillshading().
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

  /// @brief True if the alpha-only slope-shading layer is enabled; see slotSetSlopeShading().
  bool doSlopeShading() const { return bSlopeShading; }

  /// @brief Get the slope-shading intensity as the slider value (0..100) that produced it.
  int getFactorSlopeShading() const;

  /// @brief True if the graded slope-color layer is enabled; see slotSetSlopeColor().
  bool doSlopeColor() const { return bSlopeColor; }

  /// @brief True if the elevation-threshold highlight layer is enabled; see slotSetElevationLimit().
  bool doElevationLimit() const { return bElevationLimit; }

  /// @brief The elevation threshold (in the user's configured unit) set by setElevationLimit().
  int getElevationLimit() const { return elevationValue; }

  /// @brief True if the elevation-color gradient layer is enabled; see slotSetElevationShading().
  bool doElevationShading() const { return bElevationShading; }

  /// @brief Lower bound (in the user's configured unit) of the elevationShadeByte() gradient.
  int getElevationShadeLimitLow() const { return elevationShadeLimitLow; }

  /// @brief Upper bound (in the user's configured unit) of the elevationShadeByte() gradient.
  int getElevationShadeLimitHi() const { return elevationShadeLimitHi; }

  /// @brief The color table slopeColorByte() indexes into; see slopetable.
  const QVector<QRgb> getSlopeColorTable() const { return slopetable; }

  /// @brief True if the elevation-shading legend should be drawn; see slotShowElevationShadeScale().
  bool doShowElevationShadeScale() const { return bShowElevationShadeScale; }

  /// Named slope-angle-threshold presets offered alongside the custom table; selected via
  /// setSlopeStepTable() and read back via getCurrentSlopeStepTable().
  static const struct SlopePresets slopePresets[7];
  static const size_t slopePresetCount = sizeof(IDem::slopePresets) / sizeof(IDem::slopePresets[0]);

  /**
     @brief The 5 slope-angle thresholds (in degrees) currently used by slopeColorByte() to
            pick one of its 6 color bands.
     @return slopeCustomStepTable if getSlopeStepTableIndex() == CUSTOM_SLOPE_COLORTABLE,
             otherwise slopePresets[getSlopeStepTableIndex()].steps
   */
  const qreal* getCurrentSlopeStepTable() const;

  /// @brief Index into slopePresets, or CUSTOM_SLOPE_COLORTABLE; see setSlopeStepTable().
  int getSlopeStepTableIndex() const { return gradeSlopeColor; }

  /**
     @brief Select which slope-angle-threshold table slopeColorByte() uses.
     @param idx index into slopePresets, or CUSTOM_SLOPE_COLORTABLE to use the values set
                via setSlopeStepTableCustomValue()
   */
  void setSlopeStepTable(int idx);
  /// @brief Set one of the 5 thresholds of the custom slope-angle table (see CUSTOM_SLOPE_COLORTABLE).
  void setSlopeStepTableCustomValue(int idx, int val);
  /// @brief Set the threshold used by elevationLimitByte(), in the user's configured unit.
  void setElevationLimit(int val);

  /// @brief (Re-)build elevationShadeTable, the color gradient used by elevationShadeByte().
  void initElevationShadeTable();
  /// @brief Set the lower bound of the elevationShadeByte() gradient.
  void setElevationShadeLow(int val);
  /// @brief Set the upper bound of the elevationShadeByte() gradient.
  void setElevationShadeHi(int val);

  /// Size (in samples) of the neighborhood window slopeOfWindowInterp() operates on:
  /// eWinsize3x3 for a window already aligned to the raster grid (used by the per-pixel
  /// shading methods below, with x=y=0), or eWinsize4x4 for a query at an arbitrary
  /// (non-pixel-aligned) point - e.g. CDemVRT::getSlopeAt() - where the extra ring of
  /// neighbors lets the 3x3 window itself be bilinearly interpolated at fractional (x,y)
  /// before the slope is computed from it.
  enum winsize_e { eWinsize3x3 = 9, eWinsize4x4 = 16 };

 public slots:
  /// @brief Enable/disable the hillshading layer; see doHillshading().
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

  /// @brief Enable/disable the alpha-only slope-shading layer; see doSlopeShading().
  void slotSetSlopeShading(bool yes) { bSlopeShading = yes; }

  /// @brief Set the slope-shading intensity from a slider value (0..100); see getFactorSlopeShading().
  void slotSetFactorSlopeShade(int f);

  /// @brief Enable/disable the graded slope-color layer; see doSlopeColor().
  void slotSetSlopeColor(bool yes) { bSlopeColor = yes; }

  /// @brief Enable/disable the elevation-threshold highlight layer; see doElevationLimit().
  void slotSetElevationLimit(bool yes) { bElevationLimit = yes; }

  /// @brief Enable/disable the elevation-color gradient layer; see doElevationShading().
  void slotSetElevationShading(bool yes) { bElevationShading = yes; }

  /// @brief Show/hide the elevation-shading legend drawn by CDemVRT::drawElevationShadeScale().
  void slotShowElevationShadeScale(bool yes);

 protected:
  /// Per-pixel output buffers computeShading() writes one chunk of; a null entry means
  /// that layer is disabled, so computeShading() skips it entirely - including whichever
  /// shared sub-computation (slope, max elevation) only that layer would have needed.
  struct shading_buffers_t {
    QVector<quint8>* hillshade = nullptr;
    QVector<quint8>* slopeShade = nullptr;
    QVector<quint8>* slopeColor = nullptr;
    QVector<quint8>* elevationLimit = nullptr;
    QVector<quint8>* elevationShade = nullptr;
  };

  /**
     @brief Compute every enabled shading layer for one chunk.

     Runs up to 3 independent passes over the chunk - hillshade; slopeShade+slopeColor;
     elevationLimit+elevationShade - each entered only if at least one of its layers is
     active in `buffers`, so "is this layer enabled" never needs testing inside a pixel
     loop. slopeOfWindowInterp() and maxElevationInWindow() are each computed at most once
     per pixel and shared between the two layers in their pass, rather than recomputed once
     per layer.

     @param data    raw elevation samples, kept 1px wider/taller than each output buffer on
                     every side so each pixel's 3x3 neighborhood window can be read even at
                     the edges of the requested area. Indexed with stride+2.
     @param buffers which layers to compute; see shading_buffers_t
     @param x, y    top-left corner of the chunk to process, in the output buffers'/stride
                    coordinates
     @param stride  row stride of each output buffer, in pixels
     @param w, h    size of the chunk to process
   */
  void computeShading(const QVector<float>& data, const shading_buffers_t& buffers, quint32 x, quint32 y,
                      quint32 stride, quint32 w, quint32 h) const;

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

  /// the owning CDemDraw; provides the canvas's projection, redraw signaling and pixel
  /// coordinate conversion that subclasses need while reading their data source
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

  /// grayscale + transparent-at-255 color table for computeShading()'s hillshading output
  /// (Format_Indexed8)
  QVector<QRgb> graytable;

  /// color gradient for elevationShadeByte()'s output (Format_Indexed8); see initElevationShadeTable()
  QVector<QRgb> elevationShadeTable;

  /// color table for slopeColorByte()'s output (Format_Indexed8): index 0 transparent,
  /// 1..5 the colors for each of the 5 slope-grade bands in getCurrentSlopeStepTable()
  QVector<QRgb> slopetable;

  /// color table for elevationLimitByte()'s output (Format_Indexed8): index 0 transparent,
  /// index 1 the highlight color for samples at/above getElevationLimit()
  QVector<QRgb> elevationtable;

  /// non-zero if the data source declares a noData value (GDAL's GetNoDataValue()
  /// success flag; kept as int rather than bool to match that API directly)
  int hasNoData = 0;

  /// the data source's noData value, in its native units; only meaningful if hasNoData is set
  double noData = 0;

 private:
  /// out = alpha (0 = transparent, up to 255) for slope (degrees, or NOFLOAT for noData),
  /// proportional to slope and getFactorSlopeShading(); see computeShading().
  quint8 slopeShadeByte(qreal slope) const;

  /// out = index 0..5 into slopetable for slope (degrees, or NOFLOAT for noData), picking
  /// the color band against slopeStepTable's 5 thresholds; see computeShading().
  quint8 slopeColorByte(qreal slope, const qreal* slopeStepTable) const;

  /// out = index into elevationtable: 1 if elevation (in the user's configured unit, see
  /// maxElevationInWindow()) >= getElevationLimit(), else 0; see computeShading().
  quint8 elevationLimitByte(qreal elevation) const;

  /// out = index 0..255 into elevationShadeTable: elevation (in the user's configured
  /// unit, see maxElevationInWindow()) mapped linearly between limitLow (0) and limitHi
  /// (255); see computeShading().
  quint8 elevationShadeByte(qreal elevation, int limitLow, int limitHi) const;

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
