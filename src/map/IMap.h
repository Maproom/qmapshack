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
struct poi_t;

class IMap : public IDrawObject
{
    Q_OBJECT
public:
    IMap(quint32 features, CMapDraw * parent);
    virtual ~IMap();

    void saveConfig(QSettings& cfg) override;

    void loadConfig(QSettings& cfg) override;

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
    bool activated() const
    {
        return isActivated;
    }

    /**
       @brief Get the map's setup widget.

       As default an instance of CMapPropSetup is used. For other setups you have
       to override this method.

       @return A pointer to the widget. Use a smart pointer to store as the widget can be destroyed at any time
     */
    virtual IMapProp *getSetup();

    virtual void getInfo(const QPoint&, QString&) {}

    virtual void getToolTip(const QPoint&, QString&) const {}

    virtual void findPOICloseBy(const QPoint&, poi_t&) const {}

    /**
       @brief Return copyright notice if any
       @return If no copyright notice has been decoded the string will be empty
     */
    const QString& getCopyright() const
    {
        return copyright;
    }

    bool hasFeatureVisibility() const
    {
        return flagsFeature & eFeatVisibility;
    }
    bool hasFeatureVectorItems() const
    {
        return flagsFeature & eFeatVectorItems;
    }
    bool hasFeatureTileCache() const
    {
        return flagsFeature & eFeatTileCache;
    }
    bool hasFeatureLayers() const
    {
        return flagsFeature & eFeatLayers;
    }

    bool getShowPolygons() const
    {
        return showPolygons;
    }

    bool getShowPolylines() const
    {
        return showPolylines;
    }

    bool getShowPOIs() const
    {
        return showPOIs;
    }

    const QString& getCachePath() const
    {
        return cachePath;
    }

    qint32 getCacheSize() const
    {
        return cacheSizeMB;
    }

    qint32 getCacheExpiration() const
    {
        return cacheExpiration;
    }

    qint32 getAdjustDetailLevel() const
    {
        return adjustDetailLevel;
    }

    /**
       @brief Find a matching street polyline

       The polyline must be close enough in terms of pixel to point 1 and 2. "Close enough" is defined by
       the threshold. The returned polyline uses lon/lat as coordinates.

       @param pt1           first point in [rad]
       @param pt2           second point in [rad]
       @param threshold     the "close enough" threshold in [pixel]
       @param polyline      the resulting polyline, if any, in [rad]
       @return              Return true if a line has been found.
     */
    virtual bool findPolylineCloseBy(const QPointF& pt1, const QPointF& pt2, qint32 threshold, QPolygonF& polyline);

public slots:
    void slotSetShowPolygons(bool yes)
    {
        showPolygons = yes;
    }
    void slotSetShowPolylines(bool yes)
    {
        showPolylines = yes;
    }
    void slotSetShowPOIs(bool yes)
    {
        showPOIs = yes;
    }

    void slotSetCachePath(const QString& path)
    {
        cachePath = path;
        configureCache();
    }
    void slotSetCacheSize(qint32 size)
    {
        cacheSizeMB = size;
        configureCache();
    }
    void slotSetCacheExpiration(qint32 days)
    {
        cacheExpiration = days;
        configureCache();
    }

    void slotSetAdjustDetailLevel(qint32 level)
    {
        adjustDetailLevel = level;
    }

protected:
    void convertRad2M(QPointF &p) const;
    void convertM2Rad(QPointF &p) const;

    /**
       @brief Detect what reprojection is needed and select the correct handler

       This has to be called prior to the loop that calls drawTile();

     */
    void detectTileDrawMode(const IDrawContext::buffer_t &buf);

    /**
       @brief Reproject (translate, rotate, scale) tile before drawing it.
       @param img   the tile as QImage
       @param l     a 4 point polygon to fit the tile in
       @param p     the QPainter used to paint the tile
     */
    void drawTile(const QImage& img, QPolygonF& l, QPainter& p)
    {
        (this->*fDrawTile)(img, l, p);
    }

private:
    void (IMap::*fDrawTile)(const QImage& img, QPolygonF& l, QPainter& p) = &IMap::drawTileLQ;
    // draw tiles with low quality re-projection but fast
    void drawTileLQ(const QImage& img, QPolygonF& l, QPainter& p);
    // draw tiles with hi quality re-projection but slow
    void drawTileHQ(const QImage& img, QPolygonF& l, QPainter& p);

protected:
    /// the drawcontext this map belongs to
    CMapDraw * map;

    /**
        Source projection of the current map file
        Has to be set by subclass. Destruction has to be
        handled by subclass.
     */
    projPJ pjsrc = nullptr;

    /**
        target projection
        Is set by IMap() to WGS84. Will be freed by ~IMap()
     */
    projPJ pjtar = nullptr;

    /**
       @brief True if map was loaded successfully
     */
    bool isActivated = false;

    /// the setup dialog. Use getSetup() for access
    QPointer<IMapProp> setup;

    /// flag field for features defined in features_e
    quint32 flagsFeature;

    bool showPolygons  = true; //< vector maps only: hide/show polygons
    bool showPolylines = true; //< vector maps only: hide/show polylines
    bool showPOIs      = true; //< vector maps only: hide/show point of interest
    qint32 adjustDetailLevel = 0; //< vector maps only: alter threshold to show deatils.

    QString cachePath;            //< streaming map only: path to cached tiles
    qint32 cacheSizeMB     = 100; //< streaming map only: maximum size of all tiles in cache [MByte]
    qint32 cacheExpiration =   8; //< streaming map only: maximum age of tiles in cache [days]

    QString copyright; //< a copyright string to be displayed as tool tip
};


#endif //IMAP_H

