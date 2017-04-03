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

#include "canvas/IDrawContext.h"
#include "canvas/IDrawObject.h"
#include "units/IUnit.h"

#include <QtWidgets>

IDrawObject::IDrawObject(QObject *parent)
    : QObject(parent)
{
}

IDrawObject::~IDrawObject()
{
}

void IDrawObject::saveConfig(QSettings& cfg)
{
    cfg.setValue("opacity",  getOpacity());
    cfg.setValue("minScale", getMinScale());
    cfg.setValue("maxScale", getMaxScale());
}

void IDrawObject::loadConfig(QSettings& cfg)
{
    slotSetOpacity(cfg.value("opacity",  getOpacity() ).toDouble());
    setMinScale(   cfg.value("minScale", getMinScale()).toDouble());
    setMaxScale(   cfg.value("maxScale", getMaxScale()).toDouble());

    emit sigPropertiesChanged();
}


bool IDrawObject::isOutOfScale(const QPointF& scale) const
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

void IDrawObject::drawTileLQ(const QImage& img, QPolygonF& l, QPainter& p, IDrawContext& context, projPJ pjsrc, projPJ pjtar)
{
    QPolygonF tmp = l;
    context.convertRad2Px(l);

    // adjust the tiles width and height to fit the buffer's scale
    qreal dx1 = l[0].x() - l[1].x();
    qreal dy1 = l[0].y() - l[1].y();
    qreal dx2 = l[0].x() - l[3].x();
    qreal dy2 = l[0].y() - l[3].y();
    qreal w   = qCeil( qSqrt(dx1*dx1 + dy1*dy1));
    qreal h   = qCeil( qSqrt(dx2*dx2 + dy2*dy2));

    // switch to HQ if the gaps get visible
    if((qAbs(dy1) > 2) || (qAbs(dx2) > 2))
    {
        drawTileHQ(img, tmp, p, context, pjsrc, pjtar);
        return;
    }

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


void IDrawObject::drawTileHQ(const QImage& img, QPolygonF& l, QPainter& p, IDrawContext& context, projPJ pjsrc, projPJ pjtar)
{
    // the sub-tiles need a sensible size
    // if they get too small there will be too much
    // rounding effects.
    qint32 nStepsX = 8;
    qint32 nStepsY = 8;
    if(img.width()/nStepsX < 32)
    {
        nStepsX = 4;
    }
    if(img.height()/nStepsY < 32)
    {
        nStepsY = 4;
    }

    // transform the rad coordinates from l into the coord. system
    // of the map
    pj_transform(pjtar, pjsrc, 4, 2, &l[0].rx(), &l[0].ry(), 0);


    // calculate nStepsX*nStepsY squares evenly distributed over the tile
    // in map coords
    qreal subStepX = (l[1].x() - l[0].x()) / nStepsX;
    qreal subStepY = (l[3].y() - l[0].y()) / nStepsY;
    qreal offsetX  = l[0].x();
    qreal offsetY  = l[0].y();

    QPolygonF quads(nStepsX * nStepsY * 4);
    QPointF* pPt = quads.data();

    for(int y = 0; y < nStepsY; ++y)
    {
        for(int x = 0; x < nStepsX; ++x)
        {
            pPt->rx() = offsetX;
            pPt->ry() = offsetY;
            ++pPt;
            pPt->rx() = offsetX + subStepX;
            pPt->ry() = offsetY;
            ++pPt;
            pPt->rx() = offsetX + subStepX;
            pPt->ry() = offsetY + subStepY;
            ++pPt;
            pPt->rx() = offsetX;
            pPt->ry() = offsetY + subStepY;
            ++pPt;

            offsetX += subStepX;
        }

        offsetX  = l[0].x();
        offsetY += subStepY;
    }

    // transform the squares back to lon/lat coords in rad
    pj_transform(pjsrc, pjtar, nStepsX * nStepsY * 4, 2, &quads[0].rx(), &quads[0].ry(), 0);
    // convert the lon/lat coords of the squares into pixel coords of the
    // canvas using the view's projection
    context.convertRad2Px(quads);

    QRectF rect(0,0, img.width()/nStepsX, img.height() / nStepsY);
    const qreal rw = rect.width();
    const qreal rh = rect.height();

    // iterate over all squares, calculate the translation and rotation and draw the part of the
    // tile matching the square.
    pPt = quads.data();
    for(int y = 0; y < nStepsY; ++y)
    {
        rect.moveTop(y * rh);
        for(int x = 0; x < nStepsX; ++x, pPt += 4)
        {
            // adjust the tiles width and height to fit the buffer's scale
            qreal dx1 = pPt[0].x() - pPt[1].x();
            qreal dy1 = pPt[0].y() - pPt[1].y();
            qreal dx2 = pPt[0].x() - pPt[3].x();
            qreal dy2 = pPt[0].y() - pPt[3].y();
            qreal w   = qRound( qSqrt(dx1*dx1 + dy1*dy1));
            qreal h   = qRound( qSqrt(dx2*dx2 + dy2*dy2));

            // calculate rotation. This is not really a reprojection but might be good enough for close zoom levels
            qreal a = qAtan(dy1/dx1) * RAD_TO_DEG;

            // move rect to select the part of the tile to draw.
            rect.moveLeft(x * rw);

            // finally translate, scale, rotate and draw tile
            p.save();
            p.translate(pPt[0]);
            p.scale(w/rw, h/rh);
            p.rotate(a);
            p.drawImage(QPoint(0,0),img, rect);
            p.restore();
        }
    }
}
