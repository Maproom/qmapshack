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


void IMap::drawTile(const QImage& img, QPolygonF& l, QPainter& p)
{
    QVector<QPointF> tmp = l;

    QTime t;
    t.start();

    map->convertRad2Px(l);

    // adjust the tiles width and height to fit the buffer's scale
    qreal dx1 = l[0].x() - l[1].x();
    qreal dy1 = l[0].y() - l[1].y();
    qreal dx2 = l[0].x() - l[3].x();
    qreal dy2 = l[0].y() - l[3].y();
    qreal w   = qCeil( qSqrt(dx1*dx1 + dy1*dy1));
    qreal h   = qCeil( qSqrt(dx2*dx2 + dy2*dy2));

    // calculate rotation. This is not really a reprojection but might be good enough for close zoom levels
    qreal a = qAtan(dy1/dx1) * RAD_TO_DEG;

    // finally translate, scale, rotate and draw tile
    p.save();
    p.translate(l[0]);
    p.scale(w/img.width(), h/img.height());
    p.rotate(a);
    p.drawImage(0,0,img);
    p.restore();

    qint32 t1 = t.elapsed();

    // test code
    p.setPen(Qt::black);

    pj_transform(pjtar, pjsrc, 4, 2, &tmp[0].rx(), &tmp[0].ry(), 0);

    qreal dxSub = (tmp[1].x() - tmp[0].x()) / 10.0;
    qreal dySub = (tmp[3].y() - tmp[0].y()) / 10.0;

    QRectF rect(0,0, img.width()/10.0, img.height() / 10.0);
    qreal ox = tmp[0].x();
    qreal oy = tmp[0].y();
    for(int x = 0; x < 10; ++x)
    {
        rect.moveTop(0);
        rect.moveLeft(x * rect.width());

        for(int y = 0; y < 10; ++y)
        {
            QVector<QPointF> pl = {
            QPointF(ox, oy),
            QPointF(ox + dxSub, oy),
            QPointF(ox + dxSub, oy + dySub),
            QPointF(ox, oy + dySub)
            };

            pj_transform(pjsrc, pjtar, 4, 2, &pl[0].rx(), &pl[0].ry(), 0);
            map->convertRad2Px(pl);

            // adjust the tiles width and height to fit the buffer's scale
            qreal dx1 = pl[0].x() - pl[1].x();
            qreal dy1 = pl[0].y() - pl[1].y();
            qreal dx2 = pl[0].x() - pl[3].x();
            qreal dy2 = pl[0].y() - pl[3].y();
            qreal w   = qCeil( qSqrt(dx1*dx1 + dy1*dy1));
            qreal h   = qCeil( qSqrt(dx2*dx2 + dy2*dy2));

            // calculate rotation. This is not really a reprojection but might be good enough for close zoom levels
            qreal a = qAtan(dy1/dx1) * RAD_TO_DEG;

            rect.moveTop(rect.height() * y);
            const QImage& i = img.copy(rect.toRect());

            // finally translate, scale, rotate and draw tile
            p.save();
            p.translate(pl[0]);
            p.scale(w/i.width(), h/i.height());
            p.rotate(a);
            p.drawImage(0,0,i);
            p.restore();

            oy += dySub;
        }

        ox += dxSub;
        oy  = tmp[0].y();
    }

    qint32 t2 = t.elapsed() - t1;
    qDebug() << t1 << t2;

}

bool IMap::findPolylineCloseBy(const QPointF&, const QPointF&, qint32, QPolygonF&)
{
    return false;
}
