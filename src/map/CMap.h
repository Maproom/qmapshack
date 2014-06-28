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

#ifndef CMAP_H
#define CMAP_H

#include <QThread>
#include <QStringList>
#include "IMap.h"

class QPainter;
class CCanvas;
class CMapList;
class QSettings;
class CMapItem;

class CMap : public QThread
{
    Q_OBJECT
    public:
        CMap(CCanvas * parent);
        virtual ~CMap();

        void saveConfig(QSettings& cfg);
        void loadConfig(QSettings& cfg);
        void loadConfigForMapItem(CMapItem * item);

        /**
           @brief Zoom in and out of the map by the scale factors defined in CMapDB::scales.
           @param in            set true to zoom in, and false to zoom out
           @param needsRedraw   if the zoom action makes a redraw nesseccary needsRedraw is set true
         */
        void zoom(bool in, bool& needsRedraw);

        /**
            @brief Draw the active map buffer to the pointer
            @param p            the painte used to draw the map
            @param needsRedraw  set true to trigger a redraw in the background thread
            @param f            the point of focus in [°] that is drawn in the middle of the viewport.
         */

        void draw(QPainter& p, bool needsRedraw, const QPointF &f, const QRectF &r);

        /**
           @brief resize the internal buffer
           @param size  the new size of the canvas
         */
        void resize(const QSize& size);

        /**
           @brief Convert a geo coordinate of format lon/lat WGS84 into the currently used coordinate/projection/datum system.
           @note  The unit is dependent on the currently used projection and must not nesseccarily be meter
           @param p             the point to convert
         */
        void convertRad2M(QPointF &p);
        /**
           @brief Convert a geo coordinate of the currently used projection/datum to lon/lat WGS84
           @note  The unit is dependent on the currently used projection and must not nesseccarily be meter
           @param p             the point to convert
         */
        void convertM2Rad(QPointF &p);
        /**
           @brief Convert a pixel coordinate from the viewport to a geo coordinate in [rad]
           @param p             the point to convert
         */
        void convertPx2Rad(QPointF& p);
        /**
           @brief Convert a geo coordinate in [rad] to a pixel coordinate of the viewport
           @param p             the point to convert
         */
        void convertRad2Px(QPointF& p);
        void convertRad2Px(QPolygonF& poly);

        /**
           @brief Check if the internal needs redraw flag is set
           @return intNeedsRedraw is returned
         */
        bool needsRedraw();

        /**
           @brief Get the projection string of this map object
           @return A proj4 string.
         */
        QString getProjection();

        void getInfo(const QPoint& px, QString& str);
        void getToolTip(const QPoint& px, QString& str);

        static void setupMapPath();
        static void saveMapPath(QSettings &cfg);
        static void loadMapPath(QSettings &cfg);
        static const QStringList& getSupportedFormats(){return supportedFormats;}

    signals:
        void sigCanvasUpdate();
        void sigStartThread();
        void sigStopThread();
        void sigScaleChanged(const QPointF& scale);

    public slots:
        void emitSigCanvasUpdate();

    protected:
        void run();


    private:
        static const qreal scales[];

        void zoom(int idx);

        void buildMapList();

        /**
           @brief Save list of active maps to configuration file
         */
        void saveActiveMapsList(QStringList &keys, QSettings &cfg);
        void saveActiveMapsList(QStringList &keys);
        /**
           @brief Restore list of active maps from configuration file
         */
        void restoreActiveMapsList(QStringList &keys, QSettings &cfg);
        void restoreActiveMapsList(QStringList &keys);


        /// the mutex to serialize access
        QMutex mutex;

        /// internal needs redraw flag
        bool intNeedsRedraw;

        /// the canvas this map object is attached to
        CCanvas * canvas;
        /// map canvas twin buffer
        IMap::buffer_t buffer[2];
        /// the main threads currently used map canvas buffer
        bool bufIndex;
        /// buffer width [px]
        int  bufWidth;
        /// buffer height [px]
        int  bufHeight;
        /// the viewports width [px]
        int  viewWidth;
        /// the viewports height [px]
        int  viewHeight;
        /// the center of the viewport
        QPointF center;

        /// source projection should be the same for all maps
        projPJ  pjsrc;
        /// target projection is always WGS84
        projPJ  pjtar;

        /// the basic scale of the map canvas
        QPointF scale;
        /// index into scales table
        int     zoomIndex;
        /// the actual used scaleFactor
        QPointF zoomFactor;

        /// the next point of focus that will be displayed right in the middle of the viewport
        QPointF focus;
        /// top left corner of next buffer
        QPointF ref1;
        /// top right corner of next buffer
        QPointF ref2;
        /// bottom right corner of next buffer
        QPointF ref3;
        /// bottom left corner of next buffer
        QPointF ref4;

        /// the treewidget holding all active and inactive map items
        CMapList * mapList;

        /// the group label used in QSettings
        QString cfgGroup;

        /// the list of paths to serach maps
        static QStringList mapPaths;

        /// all existing CMap instances
        static QList<CMap*> maps;

        /// a list of supported map formats
        static QStringList supportedFormats;

};

#endif //CMAP_H

