/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler <oliver.eichler@gmx.de>

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

#include "units/IUnit.h"
#include <proj_api.h>
#include <QObject>

class QSettings;
class QListWidget;
class QPainter;
class IDrawContext;

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
    int getOpacity() const
    {
        return opacity;
    }

    /**
       @brief Read the minimum scale factor the object should be displayed
       @return A factor or NOFLOAT if no minimum has been set
     */
    qreal getMinScale() const
    {
        return minScale;
    }
    /**
       @brief Read the maximum scale factor the object should be displayed
       @return A factor or NOFLOAT if no maximum has been set
     */
    qreal getMaxScale() const
    {
        return maxScale;
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


    /**
       @brief Get QListWidgetItems to enable/disable map layers.

       As this property is a bit more complex the idea is to reimplement the method
       if the map type has layers. The implementation probably will clear the list
       and insert a checkable list widget item into the list. Additionally it will
       connect to the QListWidget's signals to be noticed by a change.

       Different to other properties, that will get queried when ever the property
       widget think it needs an update, getLayers() will only be called once upon
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
    bool isOutOfScale(const QPointF& scale) const;

    /**
       @brief Setup a map cache using cachePath, cacheSizeMB and cacheExpiration

       The default implementation does noting. Streaming maps will probably override
       it to reconfigure their cache. The method is called when ever a cache property is
       changed.
     */
    virtual void configureCache()
    {
    }


    // draw tiles with low quality re-projection but fast
    void drawTileLQ(const QImage& img, QPolygonF& l, QPainter& p, IDrawContext& context, projPJ pjsrc, projPJ pjtar);
    // draw tiles with high quality re-projection but slow
    void drawTileHQ(const QImage& img, QPolygonF& l, QPainter& p, IDrawContext& context, projPJ pjsrc, projPJ pjtar);

private:
    /// the opacity level of a map
    qreal opacity = 100;
    /// the minimum scale a map is visible
    qreal minScale = NOFLOAT;
    /// the maximum scale a map is visible
    qreal maxScale = NOFLOAT;
};

#endif //IDRAWOBJECT_H

