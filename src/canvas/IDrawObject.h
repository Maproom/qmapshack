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

#ifndef IDRAWOBJECT_H
#define IDRAWOBJECT_H

#include <QObject>

class QSettings;
class QListWidget;

class IDrawObject : public QObject
{
    Q_OBJECT
public:
    IDrawObject(QObject * parent);
    virtual ~IDrawObject();

    virtual void saveConfig(QSettings& cfg);

    virtual void loadConfig(QSettings& cfg);

    /**
       @brief Read opacity value
       @return Return the opacity in a range of 0..100(full opacity)
     */
    int getOpacity()
    {
        return( opacity);
    }

    /**
       @brief Read the minimum scale factor the object should be displayed
       @return A factor or NOFLOAT if no minimum has been set
     */
    qreal getMinScale()
    {
        return( minScale);
    }
    /**
       @brief Read the maximum scale factor the object should be displayed
       @return A factor or NOFLOAT if no maximum has been set
     */
    qreal getMaxScale()
    {
        return( maxScale);
    }
    /**
       @brief Write the minimum scale the object should be displayed at.
       @param s A factor or NOFLOAT if no minimum should be set
     */
    void setMinScale(qreal s);
    /**
       @brief Write the maximum scale the object should be displayed at.
       @param s A factor or NOFLOAT if no maximum should be set
     */
    void setMaxScale(qreal s);

    bool getShowPolygons()
    {
        return( showPolygons);
    }

    bool getShowPolylines()
    {
        return( showPolylines);
    }

    bool getShowPOIs()
    {
        return( showPOIs);
    }

    const QString& getCachePath()
    {
        return( cachePath);
    }

    qint32 getCacheSize()
    {
        return( cacheSizeMB);
    }

    qint32 getCacheExpiration()
    {
        return( cacheExpiration);
    }

    /**
       @brief Get QListWidgetItems to enable/disable map layers.

       As this property is a bit more complex the idea is to reimplement the method
       if the map type has layers. The implementation probably will clear the list
       and insert a checkable list widget item into the list. Additionally it will
       connect to the QListWidget's signals to be noticed by a change.

       Different to other properties, that will get queried when ever the property
       widget think it needs an update, getLayers() will only be called once uppon
       property widget creation.

       The default implementation will simply clear the list.

       @param list
     */
    virtual void getLayers(QListWidget& list);

public slots:
    /**
       @brief Write opacity value
       @param value must be in the range of 0..100(full opacity)
     */
    void slotSetOpacity(int value)
    {
        opacity = value;
    }

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
        cachePath = path; configureCache();
    }
    void slotSetCacheSize(qint32 size)
    {
        cacheSizeMB = size; configureCache();
    }
    void slotSetCacheExpiration(qint32 days)
    {
        cacheExpiration = days; configureCache();
    }

signals:
    /**
       @brief Emitted every time a property of the object is changed
     */
    void sigPropertiesChanged();

protected:
    /**
       @brief Test if the given scale is out of the min/max scale
       @param scale A scale factor for x and y axis
       @return True if x scale is out of the min/max range
     */
    bool isOutOfScale(const QPointF& scale);

    /**
       @brief Setup a map cache using cachePath, cacheSizeMB and cacheExpiration

       The default implementation does noting. Streaming maps will probably override
       it to reconfigure their cache. The method is called when ever a cache property is
       changed.
     */
    virtual void configureCache()
    {
    }

private:
    /// the opacity level of a map
    qreal opacity;
    /// the minimum scale a map is visible
    qreal minScale;
    /// the maximum scale a map is visible
    qreal maxScale;
    /// vector maps only: hide/show polygons
    bool showPolygons;
    /// vector maps only: hide/show polylines
    bool showPolylines;
    /// vector maps only: hide/show point of interest
    bool showPOIs;

    /// streaming map only: path to cached tiles
    QString cachePath;
    /// streaming map only: maximum size of all tiles in cache [MByte]
    qint32 cacheSizeMB;
    /// streaming map only: maximum age of tiles in cache [days]
    qint32 cacheExpiration;
};

#endif //IDRAWOBJECT_H

