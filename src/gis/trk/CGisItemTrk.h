/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler oliver.eichler@gmx.de

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

#include "gis/IGisItem.h"
#include "gis/IGisLine.h"
#include "gis/trk/CActivityTrk.h"

#include <QPen>
#include <QPointer>
#include <functional>

class QDomNode;
class IGisProject;
class IPlot;
class CDetailsTrk;
class CScrOptTrk;
class QSqlDatabase;
class CQlgtTrack;
class IQlgtOverlay;
class QDir;
class CProgressDialog;
class CPropertyTrk;
class CColorLegend;

#define TRK_N_COLORS          17
#define ASCEND_THRESHOLD       5

#include <QDebug>


class CGisItemTrk : public IGisItem, public IGisLine
{
public:
    struct trk_t;
    struct trkpt_t;

    enum focusmode_e
    {
        eFocusMouseMove
        ,eFocusMouseClick
    };

    enum mode_e
    {
        eModeNormal
        , eModeRange
    };

    /**
       @brief Used to create a new track from a part of an existing track
       @param name
       @param idx1
       @param idx2
       @param srctrk
       @param project
     */
    CGisItemTrk(const QString& name, qint32 idx1, qint32 idx2, const trk_t &srctrk, IGisProject *project);
    /**
       @brief Used to create a copy of track with new parent
       @param parentTrk
       @param project
       @param idx
       @param clone
     */
    CGisItemTrk(const CGisItemTrk& parentTrk, IGisProject * project, int idx, bool clone);
    /**
       @brief Used to restore a track from a line of coordinates
       @param l
       @param name
       @param project
       @param idx
     */
    CGisItemTrk(const SGisLine &l, const QString &name, IGisProject *project, int idx);
    /**
       @brief Used to create track from GPX file
       @param xml
       @param project
     */
    CGisItemTrk(const QDomNode &xml, IGisProject *project);
    /**
       @brief Used to restore track from history structure
       @param hist
       @param project
     */
    CGisItemTrk(const history_t& hist, IGisProject * project);
    /**
       @brief Used to restore track from database
       @param id
       @param db
       @param project
     */
    CGisItemTrk(quint64 id, QSqlDatabase& db, IGisProject * project);
    /**
       @brief Clone QLandkarte GT track
       @param trk1
     */
    CGisItemTrk(const CQlgtTrack& trk1);
    /**
       @brief Load track from file (e.g. TwoNav *trk)
       @param filename
       @param project
     */
    CGisItemTrk(const QString& filename, IGisProject * project);

    CGisItemTrk(const IQlgtOverlay& ovl);

    virtual ~CGisItemTrk();

    /**
       @brief Save track to GPX tree
       @param gpx   The <gpx> node to append by the track
     */
    void save(QDomNode& gpx);
    /**
       @brief Save track to TwoNav track file
       @param dir   the path to store the file
     */
    bool saveTwoNav(const QString& filename);
    /**
       @brief Read serialized track from a binary data stream
       @param stream  the data stream to read from
       @return A reference to the stream
     */
    QDataStream& operator<<(QDataStream& stream);
    /**
       @brief Serialize track into a binary data stream
       @param stream  the data stream to write to.
       @return A reference to the stream
     */
    QDataStream& operator>>(QDataStream& stream) const;

    /// get name of track
    const QString& getName() const
    {
        return trk.name.isEmpty() ? noName : trk.name;
    }

    /// get the track color as index into the Garmin color table
    int getColorIdx() const
    {
        return colorIdx;
    }

    /// get the track color a Qt color object
    const QColor& getColor() const
    {
        return color;
    }


    /**
       @brief get a summary of the track
       @param allowEdit if true the track name is a link to allow interactions like edit
       @return
     */
    QString getInfo(bool allowEdit = false) const;
    /// get a summary of a selected range
    QString getInfoRange();
    /// get a summary of a selected range defined by two track points
    QString getInfoRange(const trkpt_t& pt1, const trkpt_t& pt2);
    /// get a summary for a track point
    QString getInfoTrkPt(const trkpt_t& pt);
    /// get a progress summary for a selected track point
    QString getInfoProgress(const trkpt_t& pt);

    quint32 getTotalElapsedSeconds() const
    {
        return totalElapsedSeconds;
    }

    quint32 getTotalElapsedSecondsMoving()
    {
        return totalElapsedSecondsMoving;
    }

    qreal getTotalAscend() const
    {
        return totalAscend;
    }

    qreal getTotalDescend() const
    {
        return totalDescend;
    }

    qreal getTotalDistance() const
    {
        return totalDistance;
    }

    const QString& getComment() const
    {
        return trk.cmt;
    }
    const QString& getDescription() const
    {
        return trk.desc;
    }
    const QList<link_t>& getLinks() const
    {
        return trk.links;
    }
    /// get the track as a simple coordinate polyline
    void getPolylineFromData(QPolygonF &l);
    /// get the track as polyline with elevation, pixel and GIS coordinates.
    void getPolylineFromData(SGisLine& l);

    const QDateTime& getTimeStart() const
    {
        return timeStart;
    }

    qint32 getNumberOfVisiblePoints() const
    {
        return cntVisiblePoints;
    }

    const CActivityTrk& getActivities() const
    {
        return activities;
    }

    const CPropertyTrk * getPropertyHandler() const
    {
        return propHandler;
    }
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
    QString getColorizeSource()
    {
        return colorSource;
    }

    QStringList getExistingDataSources() const;

    void setColorizeLimitLow(qreal limit);
    qreal getColorizeLimitLow() const
    {
        return limitLow;
    }

    void setColorizeLimitHigh(qreal limit);
    qreal getColorizeLimitHigh() const
    {
        return limitHigh;
    }

    const QString getColorizeUnit() const;

    void getExtrema(qreal &min, qreal &max, const QString &source) const;

private:
    QString colorSource  = "";

    // the low and high limit for (slope-)colored drawing of tracks
    qreal limitLow  = -10;
    qreal limitHigh =  10;

    void drawColorized(QPainter &p);
    /**@}*/


    /**
       @brief Get the indices of visible points for a selected range

       If no range is selected both indices will be NOIDX.

       @param idx1 a reference to receive the first index
       @param idx2 a reference to receive the second index
     */
public:
    void getSelectedVisiblePoints(qint32& idx1, qint32& idx2);

    void setName(const QString& str);
    void setColor(int idx);
    bool setMode(mode_e m, const QString &owner);
    void setComment(const QString& str);
    void setDescription(const QString& str);
    void setLinks(const QList<link_t>& links);
    void setDataFromPolyline(const SGisLine &l);

    /**
       @brief display the track screen options

       @param origin    the point on screen to anchor the options
       @param mouse     the mouse object causing the request
       @return          a pointer to the screen option widget
     */
    IScrOpt * getScreenOptions(const QPoint &origin, IMouse * mouse);
    /**
       @brief Get a screen pixel of the track close to the given position on the screen
       @param screenPos Screen position as pixel coordinate
       @return The screen coordinates as pixel of a track point close by
     */
    QPointF getPointCloseBy(const QPoint& screenPos);
    /**
       @brief isCloseTo
       @param pos Screen position as pixel coordinate
       @return True if point is considered close enough
     */
    bool isCloseTo(const QPointF& pos);

    void drawItem(QPainter& p, const QPolygonF& viewport, QList<QRectF>& blockedAreas, CGisDraw * gis);
    void drawItem(QPainter& p, const QRectF& viewport, CGisDraw * gis);
    void drawLabel(QPainter& p, const QPolygonF& viewport, QList<QRectF>& blockedAreas, const QFontMetricsF& fm, CGisDraw * gis);
    void drawHighlight(QPainter& p);
    void drawRange(QPainter& p);

    /**
       @brief Switch user focus on and off.

       If the focus is switched on any other track having the focus will loose it.

       @param yes   set true to gain focus.
     */
    void gainUserFocus(bool yes);
    /**
       @brief Make sure the track has lost focus.

       If the track has the focus, keyUserFocus will be reset. In all other cases nothing will be done.

     */
    void looseUserFocus();

    /**
       @brief Make sure a CDetailsTrk widget is registered with the main tab widget
     */
    void edit();

    /**
       @brief Cut track at mouseClickFocus
       @return Return true on success.
     */
    bool cut();

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
    void combine(const QList<key_t> &keysPreSel);

    /**
       @brief Set the trkpt_t::eHidden flag

       The flag is set for all track points between mouseClickFocus and mouseMoveFocus,
       regardless of their previous state.

     */
    void hideSelectedPoints();

    /**
       @brief Reset the trkpt_t::eHidden flag

       The flag is reset for all track points between mouseClickFocus and mouseMoveFocus,
       regardless of their previous state.

     */
    void showSelectedPoints();

    /**
       @brief Set the activity flag for all track points
       @param flag  one of trkpt_t::flag_e::eAct...
       @param name  the name of the activity
       @param icon  a resource icon string to display with the activity
     */
    void setActivity(quint32 flag, const QString &name, const QString &icon);

    /**
       @brief Sets the activity flag for a selected range of track points

       The range has to be selected already. The activity will be selected by a dialog displayed
       in this method.

     */
    void setActivity();

    /**
       @brief Copy a section into a new track object

       The section is defined by mouseClickFocus and mouseMoveFocus, All points are copied,
       including the hidden (trkpt_t::eHidden) ones.

     */
    void copySelectedPoints();

    /**
       @brief Check for user focus

       @return True if the track has user focus
     */
    bool hasUserFocus() const
    {
        return key == keyUserFocus;
    }

    /**
       @brief Get the key of the current track with user focus

       @return If no track has the focus an empty string is returned
     */
    static const key_t& getKeyUserFocus()
    {
        return keyUserFocus;
    }

    /**
       @brief Each plot widget that operates on the track must register during it's construction

       see registeredPlots for a detailed discussion

       @param plot
     */
    void registerPlot(IPlot * plot);

    /**
       @brief Each plot widget that operates on the track must unregister during it's destruction

       see registeredPlots for a detailed discussion

       @param plot
     */
    void unregisterPlot(IPlot * plot);

    /**
       @brief Use point with the distance from start matching best the given distance.

       @param dist      the distance in [m]
       @param initiator a pointer to an initiating IPlot object, or 0
     */
    bool setMouseFocusByDistance(qreal dist, focusmode_e fmode, const QString& owner);

    /**
       @brief Use point with time from start matching best the given time delta

       @param time      a time delta in [s] relative to the start time
       @param initiator a pointer to an initiating IPlot object, or 0
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

    /**
       @brief Reduce the amount of visible track points with the help of the Douglas Peuker algorithm

       @note All filter implementations are found in src/gis/trk/filter/filter.cpp

       @param dist the Douglas Peuker distance in meters
     */
    void filterReducePoints(qreal dist);

    /**
       @brief Remove track points without valid location at the beginning of the track

       @note All filter implementations are found in src/gis/trk/filter/filter.cpp
     */
    void filterRemoveNullPoints();
    /**
       @brief filterReset

       @note All filter implementations are found in src/gis/trk/filter/filter.cpp
     */
    void filterReset();
    /**
       @brief filterDelete

       @note All filter implementations are found in src/gis/trk/filter/filter.cpp
     */
    void filterDelete();
    /**
       @brief filterSmoothProfile

       @note All filter implementations are found in src/gis/trk/filter/filter.cpp

       @param points  size of Median filter
     */
    void filterSmoothProfile(int points);
    /**
       @brief filterReplaceElevation
     */
    void filterReplaceElevation();
    /**
       @brief filterOffsetElevation

       @note All filter implementations are found in src/gis/trk/filter/filter.cpp

       @param offset elevation offset in meters
     */
    void filterOffsetElevation(int offset);
    /**
       @brief filterNewDate

       @note All filter implementations are found in src/gis/trk/filter/filter.cpp

       @param date new date for start of track
     */
    void filterNewDate(const QDateTime& date);
    /**
       @brief filterObscureDate

       @note All filter implementations are found in src/gis/trk/filter/filter.cpp

       @param delta intervall to increase timestamps in seconds
     */
    void filterObscureDate(int delta);
    /**
       @brief filterSpeed

       @note All filter implementations are found in src/gis/trk/filter/filter.cpp

       @param speed speed in meter per seconds
     */
    void filterSpeed(qreal speed);

    /**
       @brief Correlate waypoints with the track points

       If a waypoint correlates with a trackpoint it's key is written to
       trkpt_t::keyWpt.

       @param progress  a progress dialog as this operation can take quite some time
       @param current   the current progress if the operation is done for several tracks
     */
    void findWaypointsCloseBy(CProgressDialog &progress, quint32 &current);

private:
    void setSymbol();
    /**
       @brief Read track data from section in GPX file
       @param xml   The XML <trk> section
       @param trk   The track structure to fill
     */
    void readTrk(const QDomNode& xml, trk_t& trk);

    /**
       @brief Restore track from TwoNav *trk file
       @param filename
     */
    bool readTwoNav(const QString& filename);
    /**
       @brief Derive secondary data from the track data

       This has to be called each time the track data is changed.
     */
    void deriveSecondaryData();


    /** @defgroup ExtremaExtensions Stuff related to calculation of extremas/extensions

        @{
     */
public:
    struct limits_t
    {
        qreal min;
        qreal max;
    };
    /**@}*/

private:
    QSet<QString> existingExtensions;
    QHash<QString, limits_t> extrema;
    void updateExtremaAndExtensions();

    /**
       @brief Try to get access Nth visible point matching the idx

       This will iterate over all segments and count the visible points. If the
       count matches idx a pointer to the track point is returned.

       @param idx The index into all visible points
       @return A null pointer of no point is found.
     */
    const trkpt_t *getTrkPtByVisibleIndex(qint32 idx);
    /**
       @brief Try to get access Nth point

       This will iterate over all segments. If the index matches
       a pointer to the track point is returned.

       @param idx The index into all points
       @return A null pointer of no point is found.
     */
    const trkpt_t *getTrkPtByTotalIndex(qint32 idx);

    /**
       @brief Check if the track point at index it the last one visible
       @param idxTotal  The point's index
       @return True if it is the last one visible
     */
    bool isTrkPtLastVisible(qint32 idxTotal);

    /**
       @brief Check if the track point at index it the first one visible
       @param idxTotal  The point's index
       @return True if it is the first one visible
     */
    bool isTrkPtFirstVisible(qint32 idxTotal);
    /**
       @brief Tell the point of focus to all plots and the detail dialog

       @param pt        A pointer to the point itself
       @param fmode     The reason for the focus
       @param owner     A string to identify owner of the operation
     */
    bool publishMouseFocus(const trkpt_t * pt, focusmode_e fmode, const QString &owner);
    void publishMouseFocusNormalMode(const trkpt_t * pt, focusmode_e fmode);
    void publishMouseFocusRangeMode(const trkpt_t * pt, focusmode_e fmode);
    /**
       @brief Replace all trackpoints by the coordinates stored in the polyline

       The DEM layer will be queried for elevation data. All other data is lost.

       @param l     A polyline with coordinates [rad]
     */
    void readTrackDataFromGisLine(const SGisLine &l);
    /**
       @brief Overide IGisItem::changed() method

       As the CDetailsTrk is no modal dialog that blocks the GUI from any other input the track
       can be changed while the widget is visible. Therefore it needs some feedback to update the
       CDetailsTrk widget. Usually this would be a signal. However CGisItemTrk is a QTreeWidgetItem
       and therefor no QObject. Fortunately there the dlgDetails pointer. So CDetailsTrk::setupGui()
       can be called from changed()

       @param what  The reason string
       @param icon  An icon string
     */
    void changed(const QString& what, const QString& icon);
    /**
       @brief Overide IGisItem::updateHistory() method

        same as changed();

     */
    void updateHistory();

    /// setup colorIdx, color, bullet and icon
    void setColor(const QColor& c);
    /// setup track icon by color
    void setIcon(const QString& iconColor);

    enum visual_e
    {
        eVisualNone = 0
        , eVisualColorLegend = 0x1
        , eVisualPlots = 0x2
        , eVisualDetails = 0x4
        , eVisualAll = -1
    };

    void updateVisuals(quint32 visuals, const QString &who);

public:
    struct trkpt_t : public wpt_t
    {
        trkpt_t()
        {
            reset();
        }

        void reset()
        {
            deltaDistance   = NOFLOAT;
            distance        = NOFLOAT;
            ascend          = NOFLOAT;
            descend         = NOFLOAT;
            elapsedSeconds  = NOFLOAT;
            elapsedSecondsMoving = NOFLOAT;
            slope1          = NOFLOAT;
            slope2          = NOFLOAT;
            speed           = NOFLOAT;
            idxVisible      = NOIDX;
        }

        enum flag_e
        {
            eHidden     = 0x00000004      ///< mark point as deleted

                          // activity flags
            ,eActNone   = 0x00000000
            ,eActFoot   = 0x80000000
            ,eActCycle  = 0x40000000
            ,eActBike   = 0x20000000
            ,eActCar    = 0x10000000
            ,eActCable  = 0x08000000
            ,eActSwim   = 0x04000000
            ,eActShip   = 0x02000000
            ,eActAero   = 0x01000000
            ,eActMask   = 0xFF000000    ///< mask for activity flags
            ,eActMaxNum = 8             ///< maximum number of activity flags. this is defined by the mask
        };

        quint32 flags = 0;
        /// index within the complete track
        qint32 idxTotal;
        /// offset into lineSimple
        qint32 idxVisible;

        /// the distance to the last point
        qreal deltaDistance;
        /// the distance from the start of the track
        qreal distance;
        /// the ascend from the start of the track
        qreal ascend;
        /// the descend from the start of the track
        qreal descend;
        /// the slope [°] over several points close by
        qreal slope1;
        /// the slope [%] over several points close by
        qreal slope2;
        /// the speed over several points close by
        qreal speed;
        /// the seconds since the start of the track
        qreal elapsedSeconds;
        /// the seconds since the start of the track with moving speed
        qreal elapsedSecondsMoving;
        /// the key of an attached waypoint
        key_t keyWpt;
        /// track point extensions
        QHash<QString,QVariant> extensions;
    };

    struct trkseg_t
    {
        QVector<trkpt_t> pts;
    };

    struct trk_t
    {
        trk_t()
        {
        }
        // -- all gpx tags - start
        QString name;
        QString cmt;
        QString desc;
        QString src;
        QList<link_t> links;
        quint64 number = 0;
        QString type;
        QVector<trkseg_t> segs;
        // -- all gpx tags - stop

        QString color;
    };

    /**
       @brief Read only access to the track data.
       @return
     */
    const trk_t& getTrackData() const
    {
        return trk;
    }

    void registerColorLegend(CColorLegend *obj);
    void unregisterColorLegend(CColorLegend *obj);

private:
    /// this is the GPX structure oriented data of the track
    trk_t trk;

    /// the key of the track having the user focus.
    static key_t keyUserFocus;
    /// background (border) color of all tracks
    static const QPen penBackground;
    /// drawing and mouse interaction is dependent on the mode
    mode_e mode = eModeNormal;

    /**
       \defgroup TrackStatistics Some statistical values over the complete track
     */
    /**@{*/
    qint32 cntTotalPoints   = 0;
    qint32 cntVisiblePoints = 0;
    QDateTime timeStart;
    QDateTime timeEnd;
    qreal totalDistance = 0;
    qreal totalAscend   = 0;
    qreal totalDescend  = 0;
    qreal totalElapsedSeconds = 0;
    qreal totalElapsedSecondsMoving = 0;
    /**@}*/

    /**
        \defgroup DrawUtilies Objects used to draw the track
     */
    /**@{*/
    /// the track line color by index
    unsigned colorIdx = 4;
    /// the track line color
    QColor color;
    /// the pen with the actual track color
    QPen penForeground {Qt::blue, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin};
    /// the trackpoint bullet icon
    QPixmap bullet;
    /// the current track line as screen pixel coordinates
    QPolygonF lineSimple;
    /// visible and invisible points
    QPolygonF lineFull;
    /**@}*/


    /**
        A list of plot objects that need to get informed on any change in data.

        @note This is necessary because QTreeWidgetItem is not derived from QObject.
              Thus no signals and slots can be handled. Probably this is because the
              signal/slot system would be a huge overhead on treewidgets with a large
              amount of items.

              Anyway we need some kind of signaling between the track object and the
              plot objects displaying the data. And we have to keep in mind that
              the track can be delete by the user at any time. That is why no other
              object is allowed to save a pointer to the track. It must store the
              key. But accessing the track via key is expensive.

              That is why we make an exception here. As the track will delete all
              registered plot objects upon destruction, it should be ok to store
              the track object in the plot object, too. By that plot and track can
              easily communicate with each other.
     */
    QSet<IPlot*> registeredPlots;

    QSet<CColorLegend*> registeredColorLegends;


    /**
        \defgroup FocusRange Variables to handle mouse focus and range selection
     */
    /**@{*/
    enum rangestate_e
    {
        eRangeStateIdle
        , eRangeState1st
        , eRangeState2nd
    };

    /// state variable for range selection
    rangestate_e rangeState = eRangeStateIdle;

    /**
        @brief Identify source of current range selection

        Each range selection operation has to provide an owner string.
        If mouseFocusOwner is not empty and different to the passed
        owner string the operation must be rejected.

     */
    QString mouseFocusOwner;

    /// the current track point selected by mouse movement
    const trkpt_t * mouseMoveFocus = 0;
    /// the last track point the user clicked on
    const trkpt_t * mouseClickFocus = 0;
    /// the first point of a range selection
    const trkpt_t * mouseRange1 = 0;
    /// the second point of a range selection
    const trkpt_t * mouseRange2 = 0;
    /**@}*/

    /// the track's details dialog if any
    QPointer<CDetailsTrk> dlgDetails;

    /// the track's screen option if visible
    QPointer<CScrOptTrk>  scrOpt;

    /// all function concerning track activities have been moved to CActivityTrk
    CActivityTrk activities = {this};

    /// all functions and data concerning graphs
    CPropertyTrk * propHandler = nullptr;
};

using fTrkPtGetVal = std::function<qreal(const CGisItemTrk::trkpt_t&)>;

#endif //CGISITEMTRK_H
