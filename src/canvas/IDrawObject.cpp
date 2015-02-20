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

#include "canvas/IDrawObject.h"
#include "units/IUnit.h"

#include <QtWidgets>

IDrawObject::IDrawObject(QObject *parent)
    : QObject(parent)
    , opacity(100)
    , minScale(NOFLOAT)
    , maxScale(NOFLOAT)
    , showPolygons(true)
    , showPolylines(true)
    , showPOIs(true)
    , cacheSizeMB(100)
    , cacheExpiration(8)
{
}

IDrawObject::~IDrawObject()
{
}

void IDrawObject::saveConfig(QSettings& cfg)
{
    cfg.setValue("opacity", getOpacity());
    cfg.setValue("minScale", getMinScale());
    cfg.setValue("maxScale", getMaxScale());
    cfg.setValue("showPolygons", getShowPolygons());
    cfg.setValue("showPolylines", getShowPolylines());
    cfg.setValue("showPOIs", getShowPOIs());    
    cfg.setValue("cacheSizeMB", cacheSizeMB);
    cfg.setValue("cacheExpiration", cacheExpiration);
}

void IDrawObject::loadConfig(QSettings& cfg)
{
    slotSetOpacity(cfg.value("opacity", getOpacity()).toDouble());
    setMinScale(cfg.value("minScale", getMinScale()).toDouble());
    setMaxScale(cfg.value("maxScale", getMaxScale()).toDouble());
    slotSetShowPolygons(cfg.value("showPolygons", getShowPolygons()).toBool());
    slotSetShowPolylines(cfg.value("showPolylines", getShowPolylines()).toBool());
    slotSetShowPOIs(cfg.value("showPOIs", getShowPOIs()).toBool());
    slotSetCacheSize(cfg.value("cacheSizeMB", getCacheSize()).toInt());
    slotSetCacheExpiration(cfg.value("cacheExpiration", getCacheExpiration()).toInt());

    emit sigPropertiesChanged();
}


bool IDrawObject::isOutOfScale(const QPointF& scale)
{
    if((getMinScale() != NOFLOAT) && (scale.x() < getMinScale()))
    {
        return true;
    }
    if((getMaxScale() != NOFLOAT) && (scale.x() > getMaxScale()))
    {
        return true;
    }

    return false;
}

void IDrawObject::getLayers(QListWidget& list)
{
    list.clear();
}

void IDrawObject::setMinScale(qreal s)
{
    if((s != NOFLOAT) && (maxScale != NOFLOAT))
    {
        if(s > maxScale)
        {
            return;
        }
    }
    minScale = s;
}

void IDrawObject::setMaxScale(qreal s)
{
    if((s != NOFLOAT) && (minScale != NOFLOAT))
    {
        if(s < minScale)
        {
            return;
        }
    }
    maxScale = s;
}
