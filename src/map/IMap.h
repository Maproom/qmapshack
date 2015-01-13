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

#ifndef IMAP_H
#define IMAP_H

#include "canvas/IDrawContext.h"
#include "canvas/IDrawObject.h"
#include <QImage>
#include <QMutex>
#include <QPointer>
#include <proj_api.h>

class CMapDraw;
class IMapProp;

class IMap : public IDrawObject
{
public:
    IMap(quint32 features, CMapDraw * parent);
    virtual ~IMap();

    enum features_e
    {
        eFeatVisibility  = 0x00000001
        ,eFeatVectorItems = 0x00000002
        ,eFeatTileCache   = 0x00000004
        ,eFeatLayers      = 0x00000008
    };

    virtual void draw(IDrawContext::buffer_t& buf) = 0;

    /**
       @brief Test if map has been loaded successfully
       @return Return false if map is not loaded
     */
    bool activated()
    {
        return( isActivated);
    }

    /**
       @brief Get the map's setup widget.

       As default an instance of CMapPropSetup is used. For other setups you have
       to override this method.

       @return A pointer to the widget. Use a smart pointer to store as the widget can be destroyed at any time
     */
    virtual IMapProp *getSetup();

    /**
       @brief getInfo
       @param px
       @param str
     */
    virtual void getInfo(const QPoint& px, QString& str)
    {
        Q_UNUSED(px); Q_UNUSED(str);
    }
    /**
       @brief getToolTip
       @param px
       @param str
     */
    virtual void getToolTip(const QPoint& px, QString& str)
    {
        Q_UNUSED(px); Q_UNUSED(str);
    }

    /**
       @brief Return copyright notice if any
       @return If no copyright notice has been decoded the string will be empty
     */
    const QString& getCopyright()
    {
        return( copyright);
    }

    bool hasFeatureVisibility()
    {
        return( flagsFeature & eFeatVisibility);
    }
    bool hasFeatureVectorItems()
    {
        return( flagsFeature & eFeatVectorItems);
    }
    bool hasFeatureTileCache()
    {
        return( flagsFeature & eFeatTileCache);
    }
    bool hasFeatureLayers()
    {
        return( flagsFeature & eFeatLayers);
    }

    /**
       @brief Find a matching street polyline

       The polyline must be close enough in terms of pixel to point 1 and 2. "Close enough" is defined by
       the threshold. The returned poylline uses lon/lat as coordinates.

       @param pt1           first point in [rad]
       @param pt2           second point in [rad]
       @param threshold     the "close enough" threshold in [pixel]
       @param polyline      the resulting polyline, if any, in [rad]
       @return              Return true if a line has been found.
     */
    virtual bool findPolylineCloseBy(QPointF& pt1, QPointF& pt2, qint32 threshold, QPolygonF& polyline);

protected:
    void convertRad2M(QPointF &p);
    void convertM2Rad(QPointF &p);


    /**
       @brief Reproject (translate, rotate, scale) tile befor drwaing it.
       @param img   the tile as QImage
       @param l     a 4 point polygon to fit the tile in
       @param p     the QPainter used to paint the tile
     */
    void drawTile(QImage& img, QPolygonF& l, QPainter& p);

    /// the drawcontext this map belongs to
    CMapDraw * map;

    /// source projection of the current map file
    /**
        Has to be set by subclass. Destruction has to be
        handeled by subclass.
     */
    projPJ pjsrc;
    /// target projection
    /**
        Is set by IMap() to WGS84. Will be freed by ~IMap()
     */
    projPJ pjtar;

    /**
       @brief True if map was loaded successfully
     */
    bool isActivated;

    /// the setup dialog. Use getSetup() for access
    QPointer<IMapProp> setup;

    /// flag field for features defined in features_e
    quint32 flagsFeature;


    QString copyright;
};


#endif //IMAP_H

