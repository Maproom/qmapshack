/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler oliver.eichler@gmx.de
    Copyright (C) 2017 Norbert Truchsess norbert.truchsess@t-online.de

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

#include <QtCore>
#include "CRouterBRouterTilesSelect.h"

CRouterBRouterTilesSelect::CRouterBRouterTilesSelect(QWidget *parent, CCanvas * map)
    : QWidget(parent)
{
    canvas = map;
}

CRouterBRouterTilesSelect::~CRouterBRouterTilesSelect()
{
}

void CRouterBRouterTilesSelect::paintEvent(QPaintEvent *event)
{
}


void CRouterBRouterTilesSelect::mouseDoubleClickEvent(QMouseEvent * event)
{
    QPointF pos = QPointF(event->pos());
    canvas->convertPx2Rad(pos);
    QPointF posDegF = pos * RAD_TO_DEG;
    QPoint tile(posDegF.x()/5,posDegF.y()/5);
    tile *=5;
    if (newTiles.contains(tile))
    {
        newTiles.remove(newTiles.indexOf(tile));
    }
    else
    {
        newTiles.append(tile);
    }
}

void CRouterBRouterTilesSelect::mouseMoveEvent(QMouseEvent * event)
{
    canvas->moveMap(QPointF(event->pos()-pPressed));
    pPressed = event->pos();
}

void CRouterBRouterTilesSelect::mousePressEvent(QMouseEvent * event)
{
    pPressed = event->pos();
}

void CRouterBRouterTilesSelect::mouseReleaseEvent(QMouseEvent * event)
{
    canvas->moveMap(QPointF(event->pos()-pPressed));
}

void CRouterBRouterTilesSelect::drawExistingTiles()
{
    for(QPoint tile: existingTiles)
    {
        drawTileMark(tile,Qt::blue);
    }
}

void CRouterBRouterTilesSelect::drawNewTiles()
{
    for(QPoint tile:newTiles)
    {
        drawTileMark(tile,Qt::red);
    }
}

void CRouterBRouterTilesSelect::drawTileMark(QPoint tile, Qt::GlobalColor color)
{
    QPainter painter(this);
    QPen pen;
    pen.setColor(color);
    pen.setWidth(2);
    painter.setPen(pen);

    QPointF p0(tile.x(),tile.y());
    QPointF p1(tile.x()+5,tile.y());
    QPointF p2(tile.x()+5,tile.y()+5);
    QPointF p3(tile.x(),tile.y()+5);

    p0 *= DEG_TO_RAD;
    p1 *= DEG_TO_RAD;
    p2 *= DEG_TO_RAD;
    p3 *= DEG_TO_RAD;

    canvas->convertRad2Px(p0);
    canvas->convertRad2Px(p1);
    canvas->convertRad2Px(p2);
    canvas->convertRad2Px(p3);

    painter.drawLine(p0,p1);
    painter.drawLine(p1,p2);
    painter.drawLine(p2,p3);
    painter.drawLine(p3,p0);
    painter.drawLine(p0,p2);
    painter.drawLine(p1,p3);
}
