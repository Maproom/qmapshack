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

#include "map/CMapDraw.h"
#include "map/CMapPropSetup.h"
#include "map/IMap.h"
#include "units/IUnit.h"

#include <QtWidgets>

IMap::IMap(quint32 features, CMapDraw *parent)
    : IDrawObject(parent)
    , map(parent)
    , flagsFeature(features)
{
    pjtar = pj_init_plus("+proj=longlat +ellps=WGS84 +datum=WGS84 +no_defs");
}

IMap::~IMap()
{
    pj_free(pjtar);
    pj_free(pjsrc);
    delete setup;
}

/* virtual */ void IMap::saveConfig(QSettings& cfg) /* override */
{
    IDrawObject::saveConfig(cfg);

    if(hasFeatureVectorItems())
    {
        cfg.setValue("showPolygons", getShowPolygons());
        cfg.setValue("showPolylines", getShowPolylines());
        cfg.setValue("showPOIs", getShowPOIs());
    }

    if(hasFeatureTileCache())
    {
        cfg.setValue("cacheSizeMB", cacheSizeMB);
        cfg.setValue("cacheExpiration", cacheExpiration);
    }
}

/* virtual */ void IMap::loadConfig(QSettings& cfg) /* override */
{
    IDrawObject::loadConfig(cfg);

    slotSetShowPolygons   (cfg.value("showPolygons",    getShowPolygons()   ).toBool());
    slotSetShowPolylines  (cfg.value("showPolylines",   getShowPolylines()  ).toBool());
    slotSetShowPOIs       (cfg.value("showPOIs",        getShowPOIs()       ).toBool());
    slotSetCacheSize      (cfg.value("cacheSizeMB",     getCacheSize()      ).toInt());
    slotSetCacheExpiration(cfg.value("cacheExpiration", getCacheExpiration()).toInt());
}

IMapProp *IMap::getSetup()
{
    if(setup.isNull())
    {
        setup = new CMapPropSetup(this, map);
    }

    return setup;
}


void IMap::convertRad2M(QPointF &p) const
{
    if(pjsrc == 0)
    {
        return;
    }
    pj_transform(pjtar,pjsrc,1,0,&p.rx(),&p.ry(),0);
}

void IMap::convertM2Rad(QPointF &p) const
{
    if(pjsrc == 0)
    {
        return;
    }
    pj_transform(pjsrc,pjtar,1,0,&p.rx(),&p.ry(),0);
}


void IMap::drawTile(QImage& img, QPolygonF& l, QPainter& p)
{
    map->convertRad2Px(l);

    // adjust the tiles width and height to fit the buffer's scale
    qreal dx1   = l[0].x() - l[1].x();
    qreal dy1   = l[0].y() - l[1].y();
    qreal dx2   = l[0].x() - l[3].x();
    qreal dy2   = l[0].y() - l[3].y();
    qreal w     = qCeil( qSqrt(dx1*dx1 + dy1*dy1));
    qreal h     = qCeil( qSqrt(dx2*dx2 + dy2*dy2));

    // calculate rotation. This is not really a reprojection but might be good enough for close zoom levels
    qreal a = qAtan(dy1/dx1) * RAD_TO_DEG;

    // finally translate, scale, rotate and draw tile
    p.save();
    p.translate(l[0]);
    p.scale(w/img.width(), h/img.height());
    p.rotate(a);
    p.drawImage(0,0,img);
    p.restore();
}

bool IMap::findPolylineCloseBy(const QPointF& pt1, const QPointF& pt2, qint32 threshold, QPolygonF& polyline)
{
    Q_UNUSED(pt1);
    Q_UNUSED(pt2);
    Q_UNUSED(threshold);
    Q_UNUSED(polyline);
    return false;
}
