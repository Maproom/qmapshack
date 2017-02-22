/**********************************************************************************************
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
#include "CRouterBRouterTilesSelectArea.h"
#include "CRouterBRouterTilesSelectLayout.h"

#include "CMainWindow.h"

CRouterBRouterTilesSelectArea::CRouterBRouterTilesSelectArea(CRouterBRouterTilesSelect * parent, CCanvas * canvas)
    : QWidget(dynamic_cast<QWidget *>(parent))
{
    outdatedTilesPen.setColor(Qt::gray);
    outdatedTilesPen.setWidth(1);
    outdatedTilesBrush.setColor(Qt::gray);
    outdatedTilesBrush.setStyle(Qt::Dense5Pattern);

    existingTilesPen.setColor(Qt::darkGreen);
    existingTilesPen.setWidth(1);
    existingTilesBrush.setColor(Qt::darkGreen);
    existingTilesBrush.setStyle(Qt::Dense3Pattern);

    selectedTilesPen.setColor(Qt::blue);
    selectedTilesPen.setWidth(1);
    selectedTilesBrush.setColor(Qt::blue);
    selectedTilesBrush.setStyle(Qt::Dense3Pattern);

    this->canvas = canvas;
    this->select = parent;
}

CRouterBRouterTilesSelectArea::~CRouterBRouterTilesSelectArea()
{
}

void CRouterBRouterTilesSelectArea::paintEvent(QPaintEvent *event)
{
    drawOutdatedTiles();
    drawExistingTiles();
    drawSelectedTiles();
}


void CRouterBRouterTilesSelectArea::mouseDoubleClickEvent(QMouseEvent * event)
{
}

void CRouterBRouterTilesSelectArea::mouseMoveEvent(QMouseEvent * event)
{
    canvas->moveMap(QPointF(event->pos()-mousePos));
    mousePos = event->pos();
}

void CRouterBRouterTilesSelectArea::mousePressEvent(QMouseEvent * event)
{
    startPos = mousePos = event->pos();
}

void CRouterBRouterTilesSelectArea::mouseReleaseEvent(QMouseEvent * event)
{
    QPoint pos = event->pos();
    canvas->moveMap(QPointF(pos-mousePos));
    if (pos == startPos)
    {
        QPoint tile = tileUnderMouse(event->pos());
        if (selectedTiles.contains(tile))
        {
            selectedTiles.remove(selectedTiles.indexOf(tile));
        }
        else
        {
            selectedTiles.append(tile);
        }
        select->selectedTilesChangedEvent();
    }
}

void CRouterBRouterTilesSelectArea::drawOutdatedTiles()
{
    QPainter painter(this);
    painter.setPen(outdatedTilesPen);
    painter.setBrush(outdatedTilesBrush);
    for(QPoint tile : outdatedTiles)
    {
        painter.drawPolygon(tilePolygon(tile));
    }
}

void CRouterBRouterTilesSelectArea::drawExistingTiles()
{
    QPainter painter(this);
    painter.setPen(existingTilesPen);
    painter.setBrush(existingTilesBrush);

    for(QPoint tile : existingTiles)
    {
        painter.drawPolygon(tilePolygon(tile));
    }
}

void CRouterBRouterTilesSelectArea::drawSelectedTiles()
{
    QPainter painter(this);
    painter.setPen(selectedTilesPen);
    painter.setBrush(selectedTilesBrush);

    for(QPoint tile : selectedTiles)
    {
        painter.drawPolygon(tilePolygon(tile));
    }
}

QPoint CRouterBRouterTilesSelectArea::tileUnderMouse(const QPointF & mousePos)
{
    QPointF pos(mousePos);
    canvas->convertPx2Rad(pos);
    QPointF posDegF = pos * RAD_TO_DEG;
    QPoint tile(posDegF.x() > 0 ? posDegF.x()/5 : posDegF.x()/5 - 1
               ,posDegF.y() > 0 ? posDegF.y()/5 : posDegF.y()/5 - 1);
    return tile * 5;
}

QPolygonF CRouterBRouterTilesSelectArea::tilePolygon(const QPoint & tile)
{
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

    QPolygonF polygon;
    polygon << p0;
    polygon << p1;
    polygon << p2;
    polygon << p3;
    polygon << p0;

    return polygon;
}
