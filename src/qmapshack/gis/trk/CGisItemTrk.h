/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler <oliver.eichler@gmx.de>
    Copyright (C) 2019 Henri Hornburg <hrnbg@t-online.de>

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

#ifndef CGISITEMTRK_H
#define CGISITEMTRK_H

#include <interpolation.h>

#include <QDebug>
#include <QPen>
#include <QPointer>
#include <functional>

#include "gis/IGisItem.h"
#include "gis/IGisLine.h"
#include "gis/trk/CActivityTrk.h"
#include "gis/trk/CEnergyCycling.h"
#include "gis/trk/CTrackData.h"
#include "gis/trk/filter/CFilterSpeedCycle.h"
#include "gis/trk/filter/CFilterSpeedHike.h"
#include "helpers/CLimit.h"
#include "helpers/CValue.h"

using std::numeric_limits;

class QDomNode;
class IGisProject;
class INotifyTrk;
class CDetailsTrk;
class CScrOptTrk;
class QSqlDatabase;
class CQlgtTrack;
class IQlgtOverlay;
class CProgressDialog;
class CPropertyTrk;
class CCanvas;

#define ASCENT_THRESHOLD 5
#define MIN_WIDTH_INFO_BOX 300

class CGisItemTrk : public IGisItem, public IGisLine {
  Q_DECLARE_TR_FUNCTIONS(CGisItemTrk)
 public:
  enum focusmode_e { eFocusMouseMove, eFocusMouseClick };

  enum mode_e { eModeNormal, eModeRange };

  enum rangestate_e {
    eRangeStateInvalid,
    eRangeStateIdle,
    eRangeStateClicked1st,
    eRangeStateClicked2nd,
    eRangeStateMove1st,
    eRangeStateMove2nd
  };

  enum visual_e {
    eVisualNone = 0,
    eVisualColorLegend = 0x01,
    eVisualPlot = 0x02,
    eVisualDetails = 0x04,
    eVisualProject = 0x08,
    eVisualColorAct = 0x10,
    eVisualTrkTable = 0x20,
    eVisualTrkInfo = 0x40,
    eVisualRangeTool = 0x80,
    eVisualAll = -1
  };

  /** @brief Used to create a new track from a part of an existing track */
  CGisItemTrk(const QString& name, qint32 idx1, qint32 idx2, const CTrackData& srctrk, IGisProject* project);

  /** @brief Used to create a copy of track with new parent */
  CGisItemTrk(const CGisItemTrk& parentTrk, IGisProject* project, int idx, bool clone);

  /** @brief Used to restore a track from a line of coordinates */
  CGisItemTrk(const SGisLine& l, const QString& name, IGisProject* project, int idx);

  /** @brief Used to create track from GPX file */
  CGisItemTrk(const QDomNode& xml, IGisProject* project);

  /** @brief Used to restore track from history structure */
  CGisItemTrk(const history_t& hist, const QString& dbHash, IGisProject* project);

  /** @brief Used to restore track from database */
  CGisItemTrk(quint64 id, QSqlDatabase& db, IGisProject* project);

  /** @brief Clone QLandkarte GT track */
  CGisItemTrk(const CQlgtTrack& trk1, IGisProject* project = nullptr);

  /** @brief Load track from file (e.g. TwoNav *trk) */
  CGisItemTrk(const QString& filename, IGisProject* project);

  CGisItemTrk(const IQlgtOverlay& ovl, IGisProject* project = nullptr);

  /**
     @brief Creates a new track via provided trkdata
     @param trkdata  The track's new data (will be moved, don't use your "copy" after construction!
     @param project  The project this track belongs to
   */
  CGisItemTrk(CTrackData& trkdata, IGisProject* project);

  virtual ~CGisItemTrk();

  /**
     @brief Overide IGisItem::updateHistory() method

      same as changed();

   */
  void updateHistory(quint32 visuals);

  /**
     @brief Update all registered visuals via the INotifyTrk interface
     @param a bit field of visuals to be updated
     @param who a string for debug purposes
   */
  void updateVisuals(quint32 visuals, const QString& who);

  /**
     @brief Create a cloned copy of this track
     @return The cloned item a pointer
   */
  IGisItem* createClone() override;

  /**
     @brief Save track to GPX tree
     @param gpx   The <gpx> node to append by the track
   */
  void save(QDomNode& gpx, bool strictGpx11) override;

  /**
     @brief Save track to TwoNav track file
     @param dir   the path to store the file
   */
  bool saveTwoNav(const QString& filename);

  /**
     @brief Save track as TCX course (including correlated course points)
     @param coursesNode   The node to append by the course
   */
  void saveTCXcourse(QDomNode& coursesNode);

  /**
     @brief Save track as TCX activity
     @param activitiesNode   The node to append by the activity
   */
  void saveTCXactivity(QDomNode& activitiesNode);

  /**
     @brief Read serialized track from a binary data stream
     @param stream  the data stream to read from
     @return A reference to the stream
   */
  QDataStream& operator<<(QDataStream& stream) override;
  /**
     @brief Serialize track into a binary data stream
     @param stream  the data stream to write to.
     @return A reference to the stream
   */
  QDataStream& operator>>(QDataStream& stream) const override;

  /// get name of track
  const QString& getName() const override { return trk.name.isEmpty() ? noName : trk.name; }

  CEnergyCycling& getEnergyCycling() { return energyCycling; }

  /// returns "true" when trk has no time-related invalid points
  bool isTrkTimeValid() const { return (allValidFlags & CTrackData::trkpt_t::eInvalidTime) == 0; }
  bool isTrkElevationInvalid() const { return allValidFlags & CTrackData::trkpt_t::eInvalidEle; }
  bool isTrkSlopeInvalid() const { return allValidFlags & CTrackData::trkpt_t::eInvalidSlope; }

  QDateTime getTimestamp() const override { return getTimeStart(); }

  /// get the track color as index into the Garmin color table
  int getColorIdx() const { return colorIdx; }

  /// get the track color a Qt color object
  const QColor& getColor() const { return color; }

  /**
     @brief get a summary of the track
     @param showName  if true the track name is shown
     @return
   */
  QString getInfo(quint32 feature) const override;
  /// get a summary of a selected range
  QString getInfoRange() const;
  /// get a summary of a selected range defined by two track points
  QString getInfoRange(const CTrackData::trkpt_t& trkpt1, const CTrackData::trkpt_t& trkpt2) const;
  /// get a summary for a track point
  QString getInfoTrkPt(const CTrackData::trkpt_t& pt) const;
  /// get a progress summary for a selected track point
  QString getInfoProgress(const CTrackData::trkpt_t& pt) const;

  QString getInfoLimits() const;

  quint32 getTotalElapsedSeconds() const { return totalElapsedSeconds; }
  quint32 getTotalElapsedSecondsMoving() const { return totalElapsedSecondsMoving; }

  qreal getTotalAscent() const { return totalAscent; }
  qreal getTotalDescent() const { return totalDescent; }
  qreal getTotalDistance() const { return totalDistance; }

  const QString& getComment() const override { return trk.cmt; }
  const QString& getDescription() const override { return trk.desc; }
  const QList<link_t>& getLinks() const override { return trk.links; }

  qint32 getCntTotalPoints() const { return cntTotalPoints; }

  const QDateTime& getTimeStart() const { return timeStart; }
  const QDateTime& getTimeEnd() const { return timeEnd; }
  qint32 getNumberOfVisiblePoints() const { return cntVisiblePoints; }
  qint32 getNumberOfInvalidPoints() const { return cntInvalidPoints; }
  const CActivityTrk& getActivities() const { return activities; }
  const CPropertyTrk* getPropertyHandler() const { return propHandler; }
  const CTrackData::trkpt_t* getMouseMoveFocusPoint() const { return mouseMoveFocus; }
  quint32 getAllValidFlags() const { return allValidFlags; }

  /// get the track as a simple coordinate polyline
  void getPolylineFromData(QPolygonF& l) const;
  /// get the track as polyline with elevation, pixel and GIS coordinates.
  void getPolylineFromData(SGisLine& l) const override;
  /// get a range of the track as polyline with elevation, pixel and GIS coordinates.
  void getPolylineRangeFromData(SGisLine& l, qint32 rangeStart, qint32 rangeEnd, bool getSubPixel = false) const;
  void getPolylineDegFromData(QPolygonF& l) const override;

  /**
     @brief Get the elevation of a track point
     @param idx   The total index of the point
     @return The elevation or NOINT if the index is invalid, or the track point has no elevation value.
   */
  qint32 getElevation(qint32 idx) const;

  void getMouseRange(int& idx1, int& idx2, bool total) const;
  void getMouseRange(const CTrackData::trkpt_t*& mr1, const CTrackData::trkpt_t*& mr2) const;

  const searchValue_t getValueByKeyword(searchProperty_e keyword) override;

  /**
     @brief Convert the track to a route
   */
  void toRoute();

  /** @defgroup ColorSource Stuff related to coloring tracks using data from different sources

      @{
   */
 public:
  static const struct ColorizeSource unknownColorizeSource;

  /** @brief Set the colorize source to the source specified.

      @param src  The new source to use.
   */
  void setColorizeSource(QString src);

  /** @brief Get the current colorize source.

      @return  The new source to use.
   */
  QString getColorizeSource() const { return colorSourceLimit.getSource(); }

  QStringList getExistingDataSources() const;

  void setColorizeLimitLow(qreal limit);
  qreal getColorizeLimitLow() const { return colorSourceLimit.getMin(); }

  void setColorizeLimitHigh(qreal limit);
  qreal getColorizeLimitHigh() const { return colorSourceLimit.getMax(); }

  QString getColorizeUnit() const;

  qreal getMin(const QString& source) const;
  qreal getMax(const QString& source) const;

 private:
  void drawColorized(QPainter& p) const;
  void drawColorizedByActivity(QPainter& p) const;
  void setPen(QPainter& p, QPen& pen, trkact_t act) const;
  /**@}*/

 public:
  bool isRangeSelected() const;

  void setName(const QString& str);
  void setColor(int idx);
  /// set the width of the inner track line by factor
  bool setMode(mode_e m, const QString& owner);
  const mode_e getMode() const { return mode; }

  // Note: Maybe we have a race condition here.
  // Usually the QPointer to "dlgDetails" should be cleared if the object is destroyed.
  void clearDlgDetails() { dlgDetails.clear(); }

  void setComment(const QString& str) override;
  void setDescription(const QString& str) override;
  void setLinks(const QList<link_t>& links) override;
  void setDataFromPolyline(const SGisLine& l) override;
  quint32 getNumberOfAttachedWpt() { return numberOfAttachedWpt; }

  /**
     @brief Manually set the elevation value of a single track point
     @param idx   the total index of the track point
     @param ele   the new elevation value
   */
  void setElevation(qint32 idx, qint32 ele);

  /**
     @brief display the track screen options

     @param origin    the point on screen to anchor the options
     @param mouse     the mouse object causing the request
     @return          a pointer to the screen option widget
   */
  IScrOpt* getScreenOptions(const QPoint& origin, IMouse* mouse) override;
  /**
     @brief Get a screen pixel of the track close to the given position on the screen
     @param screenPos Screen position as pixel coordinate
     @return The screen coordinates as pixel of a track point close by
   */
  QPointF getPointCloseBy(const QPoint& screenPos) override;
  /**
     @brief isCloseTo
     @param pos Screen position as pixel coordinate
     @return True if point is considered close enough
   */
  bool isCloseTo(const QPointF& pos) override;

  bool isWithin(const QRectF& area, selflags_t flags) override;

  void drawItem(QPainter& p, const QPolygonF& viewport, QList<QRectF>& blockedAreas, CGisDraw* gis) override;
  void drawItem(QPainter& p, const QRectF& viewport, CGisDraw* gis) override;
  void drawLabel(QPainter& p, const QPolygonF&, QList<QRectF>& blockedAreas, const QFontMetricsF& fm,
                 CGisDraw* gis) override;
  void drawHighlight(QPainter& p) override;
  void drawRange(QPainter& p, CGisDraw* gis);

  /**
     @brief Switch user focus on and off.

     If the focus is switched on any other track having the focus will loose it.

     @param yes   set true to gain focus.
   */
  void gainUserFocus(bool yes) override;
  /**
     @brief Make sure the track has lost focus.

     If the track has the focus, keyUserFocus will be reset. In all other cases nothing will be done.

   */
  void looseUserFocus();

  /**
     @brief Make sure a CDetailsTrk widget is registered with the main tab widget
   */
  void edit() override;

  /**
     @brief Cut track at mouseClickFocus
     @return Return true on success.
   */
  bool cut();

  /**
     @brief Add the desc field for the track point mouseClickFocus is pointing to
     @return Return true on success.
   */
  bool addTrkPtDesc();

  /**
     @brief Set the desc field for a track point referenced by it's total index
     @param idxTotal  the index of the track point
     @param desc      the string to set
     @return Return true on success
   */
  bool setTrkPtDesc(int idxTotal, const QString& desc);

  /**
     @brief Delete track point descriptions for one or several track points
     @param idxTotal  a list of total indices
   */
  bool delTrkPtDesc(const QList<int>& idxTotal);

  /**
     @brief Reverse the complete track

     @note All timestamps will be removed
   */
  void reverse();

  /**
     @brief Combine this track with several others.

     @param keysPreSel list of pre-selected track item keys

     Handle the complete process of selecting tracks, choosing the order and
     the final name with dialogs.
   */
  void combine(const QList<key_t>& keys);

  /**
     @brief Set the CTrackData::trkpt_t::eFlagHidden flag

     The flag is set for all track points between mouseClickFocus and mouseMoveFocus,
     regardless of their previous state.

   */
  void hideSelectedPoints();

  /**
     @brief Reset the CTrackData::trkpt_t::eFlagHidden flag

     The flag is reset for all track points between mouseClickFocus and mouseMoveFocus,
     regardless of their previous state.

   */
  void showSelectedPoints();

  /**
     @brief Permanently remove selected points from track
   */
  void deleteSelectedPoints();

  /**
     @brief Set the activity flag for all track points
     @param flag  one of CTrackData::trkpt_t::flag_e::eAct...
   */
  void setActivity(trkact_t act);

  /**
     @brief Sets the activity flag for a selected range of track points

     The range has to be selected already. The activity will be selected by a dialog displayed
     in this method.
   */
  void setActivityRange(trkact_t act);

  /**
     @brief Copy a section into a new track object

     The section is defined by mouseClickFocus and mouseMoveFocus, All points are copied,
     including the hidden (CTrackData::trkpt_t::eFlagHidden) ones.

   */
  void copySelectedPoints() const;

  /**
     @brief Check for user focus

     @return True if the track has user focus
   */
  bool hasUserFocus() const override { return key == keyUserFocus; }

  /**
     @brief Get the key of the current track with user focus

     @return If no track has the focus an empty string is returned
   */
  static const key_t& getKeyUserFocus() { return keyUserFocus; }

  /**
     @brief Each plot widget that operates on the track must register during it's construction

     see registeredPlots for a detailed discussion

     @param plot
   */
  void registerVisual(INotifyTrk* visual);

  /**
     @brief Each plot widget that operates on the track must unregister during it's destruction

     see registeredPlots for a detailed discussion

     @param plot
   */
  void unregisterVisual(INotifyTrk* visual);

  /**
     @brief Use point with the distance from start matching best the given distance.

     @param dist      the distance in [m]
   */
  bool setMouseFocusByDistance(qreal dist, focusmode_e fmode, const QString& owner);

  /**
     @brief Use point with time from start matching best the given time delta

     @param time      a time delta in [s] relative to the start time
   */
  bool setMouseFocusByTime(quint32 time, focusmode_e fmode, const QString& owner);

  /**
     @brief Use the point that is closest to the given point on the screen.

     @param pt        a point on the screen in pixel.
   */
  QPointF setMouseFocusByPoint(const QPoint& pt, focusmode_e fmode, const QString& owner);

  /**
     @brief Use point with given index counter
     @param idx
   */
  bool setMouseFocusByTotalIndex(qint32 idx, focusmode_e fmode, const QString& owner);
  bool setMouseRangeByTotalIndex(qint32 idx1, qint32 idx2, const QString& owner);

  bool incMouseRangeBegin(qint32 i, const QString& owner);
  bool decMouseRangeBegin(qint32 i, const QString& owner);
  bool incMouseRangeEnd(qint32 i, const QString& owner);
  bool decMouseRangeEnd(qint32 i, const QString& owner);

  /**
     @brief Start a new mouse range right before the current selected
     @param owner
     @return
   */
  bool newMouseRangeBegin(const QString& owner);
  /**
     @brief Start a new mouse range right after the current selected
     @param owner
     @return
   */
  bool newMouseRangeEnd(const QString& owner);

  rangestate_e getRangState() const { return mode != eModeRange ? eRangeStateInvalid : rangeState; }

  void resetMouseRange();

  /** @defgroup Filter All filters implemented by CGisItemTrks.

      @note All filter implementations are found in src/gis/trk/filter/filter.cpp

      @{
   */
  /**
     @brief Reduce the amount of visible track points with the help of the Douglas Peuker algorithm

     @param dist the Douglas Peuker distance in meters
   */
  void filterReducePoints(qreal dist);

  /** @brief Remove track points without valid location at the beginning of the track */
  void filterRemoveInvalidPoints();

  /** @param points  size of Median filter */
  void filterSmoothProfile(int points);

  /** @param offset elevation offset in meters */
  void filterOffsetElevation(int offset);

  /** @param date new date for start of track */
  void filterNewDate(const QDateTime& date);

  /** @param delta interval to increase timestamps in seconds a*/
  void filterObscureDate(int delta);

  /** @param speed speed in meter per seconds */
  void filterSpeed(qreal speed);
  void filterSpeed(const CFilterSpeedCycle::cycling_type_t& cyclingType);
  void filterSpeed(const CFilterSpeedHike::hiking_type_t& hikingType);

  void filterGetSlopeLimits(qreal& minSlope, qreal& maxSlope) const;

  void filterTerrainSlope();
  void filterReplaceElevation(CCanvas* canvas);
  void filterInterpolateElevation();
  void filterReset();
  void filterDelete();
  void filterSplitSegment();
  void filterSplitTrack(qint8 nTracks);
  void filterDeleteExtension(const QString& ext);
  void filterSubPt2Pt();
  void filterChangeStartPoint(qint32 idxNewStartPoint, const QString& wptName);
  void filterLoopsCut(qreal dist);
  void filterZeroSpeedDriftCleaner(qreal distance, qreal ratio);
  /** @} */

  /**
     @brief Correlate waypoints with the track points

     If a waypoint correlates with a trackpoint it's key is written to
     CTrackData::trkpt_t::keyWpt.

     @param progress  a progress dialog as this operation can take quite some time
     @param current   the current progress if the operation is done for several tracks
   */
  void findWaypointsCloseBy(CProgressDialog& progress, quint32& current);

  bool findPolylineCloseBy(const QPointF& pt1, const QPointF& pt2, qint32& threshold, QPolygonF& polyline);

 private:
  /// no don't really use it, use CGisItemTrk(quint32 visuals) instead
  void updateHistory() override { updateHistory(eVisualAll); }

  void setSymbol() override;
  /**
     @brief Read track data from section in GPX file
     @param xml   The XML <trk> section
     @param trk   The track structure to fill
   */
  void readTrk(const QDomNode& xml, CTrackData& trk);

  /**
     @brief Restore track from TwoNav *trk file
     @param filename
   */
  bool readTwoNav(const QString& filename);

  /**
     @brief Consolidate points and subpoints
   */
  void consolidatePoints();

  /**
     @brief Derive secondary data from the track data

     This has to be called each time the track data is changed.
   */
  void deriveSecondaryData();

  /**
   * @brief Reset internal data like range selection and details dialog
   */
  void resetInternalData();

  void verifyTrkPt(CTrackData::trkpt_t*& last, CTrackData::trkpt_t& trkpt);

  /** @defgroup ExtremaExtensions Stuff related to calculation of extrema/extensions

      @{
   */
 public:
  struct limits_t {
    void setMin(qreal val, const QPointF& pos) {
      if (min > val) {
        min = val;
        posMin = pos;
      }
    }

    void setMax(qreal val, const QPointF& pos) {
      if (max < val) {
        max = val;
        posMax = pos;
      }
    }

    qreal min = numeric_limits<qreal>::max();
    QPointF posMin = NOPOINTF;

    qreal max = numeric_limits<qreal>::lowest();
    QPointF posMax = NOPOINTF;
  };
  /**@}*/

 private:
  QSet<QString> existingExtensions;
  QHash<QString, limits_t> extrema;
  void updateExtremaAndExtensions();

  enum limit_type_e { eLimitTypeMin, eLimitTypeMax };
  void drawLimitLabels(limit_type_e type, const QString& label, const QPointF& pos, QPainter& p,
                       const QFontMetricsF& fm, QList<QRectF>& blockedAreas);

  /**
     @brief Tell the point of focus to all plots and the detail dialog

     @param pt        A pointer to the point itself
     @param fmode     The reason for the focus
     @param owner     A string to identify owner of the operation
   */
  bool publishMouseFocus(const CTrackData::trkpt_t* pt, focusmode_e fmode, const QString& owner);
  void publishMouseFocusNormalMode(const CTrackData::trkpt_t* pt, focusmode_e fmode);
  void publishMouseFocusRangeMode(const CTrackData::trkpt_t* pt, focusmode_e fmode);

  /**
     @brief Replace all trackpoints by the coordinates stored in the polyline

     The DEM layer will be queried for elevation data. All other data is lost.

     @param l     A polyline with coordinates [rad]
   */
  void readTrackDataFromGisLine(const SGisLine& l);
  /**
     @brief Override IGisItem::changed() method

     As the CDetailsTrk is no modal dialog that blocks the GUI from any other input the track
     can be changed while the widget is visible. Therefore it needs some feedback to update the
     CDetailsTrk widget. Usually this would be a signal. However CGisItemTrk is a QTreeWidgetItem
     and therefor no QObject. Fortunately there the dlgDetails pointer. So CDetailsTrk::setupGui()
     can be called from changed()

     @param what  The reason string
     @param icon  An icon string
   */
  void changed(const QString& what, const QString& icon) override;

  /// setup colorIdx, color, bullet and icon
  void setColor(const QColor& c);
  /// setup track icon by color
  void setIcon(const QString& iconColor);

  bool moveMouseFocus(const CTrackData::trkpt_t*& pt, qint32 idxNext, const QString& owner);
  void setMouseFocusVisuals(const CTrackData::trkpt_t* pt);
  void setMouseRangeFocusVisuals(const CTrackData::trkpt_t* pt1, const CTrackData::trkpt_t* pt2);
  void setMouseClickFocusVisuals(const CTrackData::trkpt_t* pt);

  bool adjustIndicesForRemoveOperations(qint32& idx1, qint32& idx2);

 public:
  /**
     @brief Read only access to the track data.
     @return
   */
  const CTrackData& getTrackData() const { return trk; }

  void updateFromDB(quint64 id, QSqlDatabase& db) override;

 private:
  fGetLimit _getMin = [this](const QString& source) { return getMin(source); };

  fGetLimit _getMax = [this](const QString& source) { return getMax(source); };

  qreal getMinProp(const QString& source) const;
  qreal getMaxProp(const QString& source) const;
  QString getUnitProp(const QString& source) const;

  fGetLimit _getMinProp = [this](const QString& source) { return getMinProp(source); };

  fGetLimit _getMaxProp = [this](const QString& source) { return getMaxProp(source); };

  fGetUnit _getUnitProp = [this](const QString& source) { return getUnitProp(source); };

  fMarkChanged _markChanged = [this]() { updateHistory(eVisualNone); };

 public:
  CLimit limitsGraph1{"TrackDetails/Graph1", _getMin, _getMax, _getMinProp, _getMaxProp, _getUnitProp, _markChanged};
  CLimit limitsGraph2{"TrackDetails/Graph2", _getMin, _getMax, _getMinProp, _getMaxProp, _getUnitProp, _markChanged};
  CLimit limitsGraph3{"TrackDetails/Graph3", _getMin, _getMax, _getMinProp, _getMaxProp, _getUnitProp, _markChanged};
  CLimit limitsGraphRange{"TrackDetails/Graph3", _getMin,      _getMax,     _getMinProp,
                          _getMaxProp,           _getUnitProp, _markChanged};
  CLimit colorSourceLimit{"TrackDetails/Style", _getMin, _getMax, _getMin, _getMax, _getUnitProp, _markChanged};

 private:
  /// this is the GPX structure oriented data of the track
  CTrackData trk;

  /// the key of the track having the user focus.
  static key_t keyUserFocus;
  /// drawing and mouse interaction is dependent on the mode
  mode_e mode = eModeNormal;

  /**
     \defgroup TrackStatistics Some statistical values over the complete track
   */
  /**@{*/
  quint32 allValidFlags = 0;
  qint32 cntInvalidPoints = 0;
  qint32 cntTotalPoints = 0;
  qint32 cntVisiblePoints = 0;
  QDateTime timeStart;
  QDateTime timeEnd;
  qreal totalDistance = 0;
  qreal totalAscent = 0;
  qreal totalDescent = 0;
  qreal totalElapsedSeconds = 0;
  qreal totalElapsedSecondsMoving = 0;
  quint32 numberOfAttachedWpt = 0;
  CEnergyCycling energyCycling{*this};

  void checkForInvalidPoints();
  /**@}*/

  /**
      \defgroup DrawUtilies Objects used to draw the track
   */
  /**@{*/

  unsigned colorIdx = 4;  //< the track line color by index
  QColor color;           //< the track line color

  QPixmap bullet;        //< the trackpoint bullet icon
  QPolygonF lineSimple;  //< the current track line as screen pixel coordinates
  QPolygonF lineFull;    //< visible and invisible points

  qint32 penWidthFg = 1;   //< inner trackline width
  qint32 penWidthBg = 3;   //< outer trackline width
  qint32 penWidthHi = 11;  //< highlighted trackline width

  qint32 widthInfoBox = MIN_WIDTH_INFO_BOX;

  /// the pen with the actual track color
  QPen penForeground{Qt::blue, qreal(penWidthFg), Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin};
  /// background (border) color of all tracks
  QPen penBackground{Qt::white, qreal(penWidthBg), Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin};

  fValueOnChange onChange = [this](const QVariant& val) {
    int w = qRound(1.0 * val.toDouble());

    penWidthFg = w;
    penWidthBg = w + 2;
    penWidthHi = w + 8;

    penForeground.setWidth(penWidthFg);
    penBackground.setWidth(penWidthBg);
  };

 public:
  CValue lineScale{"TrackDetails/lineScale", 1.0, _markChanged, onChange};
  CValue showArrows{"TrackDetails/showArrows", true, _markChanged};

 private:
  /**@}*/

  /**
      A list of INotifyTrk objects that need to get informed on any change in data.

      @note This is necessary because QTreeWidgetItem is not derived from QObject.
            Thus no signals and slots can be handled. Probably this is because the
            signal/slot system would be a huge overhead on treewidgets with a large
            amount of items.

            Anyway we need some kind of signaling between the track object and the
            INotifyTrk objects displaying the data. And we have to keep in mind that
            the track can be delete by the user at any time. That is why no other
            object is allowed to save a pointer to the track. It must store the
            key. But accessing the track via key is expensive.

            That is why we make an exception here. As the track will delete all
            registered INotifyTrk objects upon destruction, it should be ok to store
            the track object in the INotifyTrk object, too. By that INotifyTrk and track can
            easily communicate with each other.

      @note CDetailsTrk is an INotifyTrk, too. But it is a bit special as it has to be destroyed
            right after all other INotifyTrk have been destroyed. That is why it is not part of
            that set.
   */
  QSet<INotifyTrk*> registeredVisuals;

  /**
      \defgroup FocusRange Variables to handle mouse focus and range selection
   */
  /**@{*/
  /// state variable for range selection
  rangestate_e rangeState = eRangeStateIdle;

  /**
      @brief Identify source of current range selection

      Each range selection operation has to provide an owner string.
      If mouseFocusOwner is not empty and different to the passed
      owner string the operation must be rejected.

   */
  QString mouseFocusOwner;

  const CTrackData::trkpt_t* mouseMoveFocus = nullptr;   //< the current track point selected by mouse movement
  const CTrackData::trkpt_t* mouseClickFocus = nullptr;  //< the last track point the user clicked on
  const CTrackData::trkpt_t* mouseRange1 = nullptr;      //< the first point of a range selection
  const CTrackData::trkpt_t* mouseRange2 = nullptr;      //< the second point of a range selection
  /**@}*/

  QPointer<CDetailsTrk> dlgDetails;  //< the track's details dialog if any
  QPointer<CScrOptTrk> scrOpt;       //< the track's screen option if visible

  /// all function concerning track activities have been moved to CActivityTrk
  CActivityTrk activities = {this};

  /// all functions and data concerning graphs
  CPropertyTrk* propHandler = nullptr;

  /**
      \defgroup Data and API related to track interpolation
   */
  /**@{*/
 public:
  enum quality_e { eQualityFine = 8, eQualityMedium = 4, eQualityCoarse = 2 };

  void setupInterpolation(bool on, qint32 q);
  bool isInterpolationEnabled() const { return interp.valid; }

  qreal getElevationInterpolated(qreal d);

 private:
  struct interpolate_t {
    bool valid = false;
    quality_e Q = eQualityCoarse;
    alglib::ae_int_t m = 0;
    alglib::spline1dinterpolant p;
    alglib::spline1dfitreport rep;
  };

  interpolate_t interp;

  using fSearch = std::function<const searchValue_t(CGisItemTrk*)>;
  static QMap<searchProperty_e, fSearch> keywordLambdaMap;
  static QMap<searchProperty_e, fSearch> initKeywordLambdaMap();

  /**@}*/
};

class INotifyTrk {
 public:
  INotifyTrk(CGisItemTrk::visual_e mask) : mask(mask) {}
  virtual ~INotifyTrk() = default;

  virtual void updateData() = 0;
  virtual void setMouseFocus(const CTrackData::trkpt_t* pt) = 0;
  virtual void setMouseRangeFocus(const CTrackData::trkpt_t* pt1, const CTrackData::trkpt_t* pt2) = 0;
  virtual void setMouseClickFocus(const CTrackData::trkpt_t* pt) = 0;

  const CGisItemTrk::visual_e mask;
};

using fTrkPtGetVal = std::function<qreal(const CTrackData::trkpt_t&)>;

#endif  // CGISITEMTRK_H
