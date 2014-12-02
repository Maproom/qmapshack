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

#include <QPen>
#include <QPointer>

class QDomNode;
class IGisProject;
class IPlot;
class CDetailsTrk;
class CScrOptTrk;
class QSqlDatabase;

#define TRK_N_COLORS 17

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

        enum drawmode_e
        {
              eDrawNormal
            , eDrawRange
        };

        CGisItemTrk(const QString& name, qint32 idx1, qint32 idx2, const trk_t &srctrk, IGisProject *project);
        CGisItemTrk(const CGisItemTrk& parentTrk, IGisProject * project, int idx);
        CGisItemTrk(const QPolygonF& l, const QString &name, IGisProject *project, int idx);
        CGisItemTrk(const QDomNode &xml, IGisProject *project);
        CGisItemTrk(const history_t& hist, IGisProject * project);
        CGisItemTrk(quint64 id, QSqlDatabase& db, IGisProject * project);
        virtual ~CGisItemTrk();

        QDataStream& operator<<(QDataStream& stream);
        QDataStream& operator>>(QDataStream& stream);

        const QString& getName(){return trk.name;}
        int getColorIdx(){return colorIdx;}
        QString getInfo();
        QString getInfoRange();
        QString getInfoTrkPt(const trkpt_t& pt);
        QString getInfoProgress(const trkpt_t& pt);
        quint32 getTotalElapsedSeconds(){return totalElapsedSeconds;}
        const QString& getComment(){return trk.cmt;}
        const QString& getDescription(){return trk.desc;}
        const QList<link_t>& getLinks(){return trk.links;}
        void getPolylineFromData(QPolygonF& l);

        void setName(const QString& str);
        void setColor(int idx);
        void setDrawMode(drawmode_e mode){drawMode = mode;}
        void setComment(const QString& str);
        void setDescription(const QString& str);
        void setLinks(const QList<link_t>& links);
        void setDataFromPolyline(const QPolygonF& l);

        IScrOpt * getScreenOptions(const QPoint &origin, IMouse * mouse);
        QPointF getPointCloseBy(const QPoint& screenPos);
        bool isCloseTo(const QPointF& pos);

        void drawItem(QPainter& p, const QRectF& viewport, QList<QRectF>& blockedAreas, CGisDraw * gis);
        void drawItem(QPainter& p, const QRectF& viewport, CGisDraw * gis);
        void drawLabel(QPainter& p, const QRectF& viewport, QList<QRectF>& blockedAreas, const QFontMetricsF& fm, CGisDraw * gis);
        void drawHighlight(QPainter& p);
        void drawRange(QPainter& p);
        void save(QDomNode& gpx);


        /**
           @brief Switch user focus on and off.

           If the focus is switched on any other track having the focus will loose it.

           @param yes   set true to gain focus.
        */
        void gainUserFocus(bool yes);

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

           Handle the complete process of selecting tracks, choosing the order and
           the final name with dialogs.
         */
        void combine();

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
           @brief Copy a section into a new track object

           The section is defined by mouseClickFocus and mouseMoveFocus, All points are copied,
           including the hidden (trkpt_t::eHidden) ones.

        */
        void copySelectedPoints();

        /**
           @brief Check for user focus

           @return True if the track has user focus
         */
        bool hasUserFocus(){return key == keyUserFocus;}

        /**
           @brief Get the key of the current track with user focus

           @return If no track has the focus an empty string is returned
         */
        static const QString& getKeyUserFocus(){return keyUserFocus;}

        /**
           @brief Each plot widget that operates on the track must register during it's contruction

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
        void setMouseFocusByDistance(qreal dist, focusmode_e mode, IPlot * initiator);

        /**
           @brief Use point with time from start matching best the given time delta

           @param time      a time delta in [s] relative to the start time
           @param initiator a pointer to an initiating IPlot object, or 0
        */
        void setMouseFocusByTime(quint32 time, focusmode_e mode, IPlot * initiator);

        /**
           @brief Use the point that is closest to the given point on the screen.

           @param pt        a point on the screen in pixel.
         */
        QPointF setMouseFocusByPoint(const QPoint& pt, focusmode_e mode);

        /**
           @brief Use point with given index counter
           @param idx
        */
        void setMouseFocusByIndex(qint32 idx, focusmode_e mode);

        static const QColor  lineColors[TRK_N_COLORS];
        static const QString bulletColors[TRK_N_COLORS];

    private:        
        void setSymbol();
        /**
           @brief Read track data from section in GPX file
           @param xml   The XML <trk> section
           @param trk   The track structure to fill
        */
        void readTrk(const QDomNode& xml, trk_t& trk);
        /**
           @brief Derive secondary data from the track data

           This has to be called each time the track data is changed.
        */
        void deriveSecondaryData();
        /**
           @brief Try to get access Nth visible point matching the idx

           This will iterate over all segments and count the visible points. If the
           count matches idx a pointer to the track point is passed

           @param idx The index into all visible points
           @return A null pointer of no point is found.
        */
        const trkpt_t *getVisibleTrkPtByIndex(quint32 idx);

        /**
           @brief Tell the point of focus to all plots and the detail dialog

           @param pt        A pointer to the point itself
           @param mode      The reason for the focus
           @param initiator A pointer to an IPlot object that has set the point of focus. Can be 0.
        */
        void publishMouseFocus(const trkpt_t * pt, focusmode_e mode, IPlot *initiator);
        /**
           @brief Replace all trackpoints by the coordinates stored in the polyline

           The DEM layer will be queried for elevation data. All other data is lost.

           @param l     A polyline with coordinates [rad]
         */
        void readTrackDataFromPolyLine(const QPolygonF &l);
        /**
           @brief Overide IGisItem::changed() method

           As the CDetailsTrk is no modal dialog that blocks the GUI from any other input the track
           can be changed while the widget is visible. Therfore it needs some feedback to update the
           CDetailsTrk widget. Usualy this would be a signal. However CGisItemTrk is a QTreeWidgetItem
           and therefor no QObject. Fortunately there the dlgDetails pointer. So CDetailsTrk::setupGui()
           can be called from changed()

           @param what  The reason string
           @param icon  An icon string
        */
        void changed(const QString& what, const QString& icon);

        void setColor(const QColor& c);
        void setIcon(const QString& c);

    public:
        struct trkpt_t : public wpt_t
        {
            trkpt_t()
                : flags(0)
            {reset();}

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
            }

            enum flag_e
            {
                eHidden   = 0x00000004  ///< mark point as deleted
            };

            quint32 flags;
            /// index within the complete track
            qint32  idxTotal;
            /// offset into lineSimple
            qint32  idxVisible;

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
        };

        struct trkseg_t
        {
            QVector<trkpt_t> pts;
            QMap<QString, QVariant> extensions;
        };

        struct trk_t
        {
            trk_t() : number(0) {}
            // -- all gpx tags - start
            QString name;
            QString cmt;
            QString desc;
            QString src;
            QList<link_t> links;
            quint64 number;
            QString type;
            QVector<trkseg_t> segs;
            // -- all gpx tags - stop
            QMap<QString, QVariant> extensions;

            QString color;
        };

        /**
           @brief Read only access to the track data.
           @return
        */
        const trk_t& getTrackData() const{return trk;}

    private:
        trk_t trk;

        static QString keyUserFocus;
        static const QPen penBackground;

        QPen penForeground;

        drawmode_e drawMode;

        qint32      cntTotalPoints;
        qint32      cntVisiblePoints;
        QDateTime   timeStart;
        QDateTime   timeEnd;
        qreal       totalDistance;
        qreal       totalAscend;
        qreal       totalDescend;
        qreal       totalElapsedSeconds;
        qreal       totalElapsedSecondsMoving;

        /// the track line color
        QColor  color;
        /// the trakpoint bullet icon
        QPixmap bullet;
        /// the track line color by index
        unsigned colorIdx;
        /// the current track line as screen pixel coordinates
        QPolygonF lineSimple;
        /// visible and invisible points
        QPolygonF lineFull;

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

        const trkpt_t * mouseMoveFocus;
        const trkpt_t * mouseClickFocus;

        QPointer<CDetailsTrk> dlgDetails;
        QPointer<CScrOptTrk>  scrOpt;
};

#endif //CGISITEMTRK_H

