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

void IMap::saveConfig(QSettings& cfg) /* override */
{
    IDrawObject::saveConfig(cfg);

    if(hasFeatureVectorItems())
    {
        cfg.setValue("showPolygons",  getShowPolygons());
        cfg.setValue("showPolylines", getShowPolylines());
        cfg.setValue("showPOIs",      getShowPOIs());
        cfg.setValue("adjustDetailLevel", getAdjustDetailLevel());
    }

    if(hasFeatureTileCache())
    {
        cfg.setValue("cacheSizeMB",     cacheSizeMB);
        cfg.setValue("cacheExpiration", cacheExpiration);
    }
}

void IMap::loadConfig(QSettings& cfg) /* override */
{
    IDrawObject::loadConfig(cfg);

    slotSetShowPolygons(cfg.value("showPolygons", getShowPolygons()).toBool());
    slotSetShowPolylines(cfg.value("showPolylines", getShowPolylines()).toBool());
    slotSetShowPOIs(cfg.value("showPOIs", getShowPOIs()).toBool());
    slotSetAdjustDetailLevel(cfg.value("adjustDetailLevel", getAdjustDetailLevel()).toInt());
    slotSetCacheSize(cfg.value("cacheSizeMB", getCacheSize()).toInt());
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
    if(pjsrc == nullptr)
    {
        return;
    }
    pj_transform(pjtar,pjsrc,1,0,&p.rx(),&p.ry(),0);
}

void IMap::convertM2Rad(QPointF &p) const
{
    if(pjsrc == nullptr)
    {
        return;
    }
    pj_transform(pjsrc,pjtar,1,0,&p.rx(),&p.ry(),0);
}


bool IMap::findPolylineCloseBy(const QPointF&, const QPointF&, qint32, QPolygonF&)
{
    return false;
}

void IMap::drawTile(const QImage& img, QPolygonF& l, QPainter& p)
{
    drawTileLQ(img, l, p, *map, pjsrc, pjtar);
}

