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

#include "CRouterBRouterTilesSelect.h"
#include "CRouterBRouterTilesSelectArea.h"
#include "CRouterBRouterTilesSelectLayout.h"
#include "CRouterBRouterTilesStatus.h"
#include "canvas/CCanvas.h"
#include <QToolTip>

CRouterBRouterTilesSelectArea::CRouterBRouterTilesSelectArea(QWidget * parent, CCanvas * canvas)
    : QWidget(parent)
{
    outdatedTilesPen.setColor(Qt::gray);
    outdatedTilesPen.setWidth(1);
    outdatedTilesBrush.setColor(Qt::gray);
    outdatedTilesBrush.setStyle(Qt::Dense5Pattern);

    currentTilesPen.setColor(Qt::darkGreen);
    currentTilesPen.setWidth(1);
    currentTilesBrush.setColor(Qt::darkGreen);
    currentTilesBrush.setStyle(Qt::Dense3Pattern);

    selectedTilesPen.setColor(Qt::blue);
    selectedTilesPen.setWidth(1);
    selectedTilesBrush.setColor(Qt::blue);
    selectedTilesBrush.setStyle(Qt::Dense3Pattern);

    outstandingTilesPen.setColor(Qt::yellow);
    outstandingTilesPen.setWidth(1);
    outstandingTilesBrush.setColor(Qt::yellow);
    outstandingTilesBrush.setStyle(Qt::Dense3Pattern);

    invalidTilesPen.setColor(Qt::gray);
    invalidTilesPen.setWidth(1);
    invalidTilesBrush.setColor(Qt::gray);
    invalidTilesBrush.setStyle(Qt::DiagCrossPattern);

    this->canvas = canvas;

    setMouseTracking(true);
}

CRouterBRouterTilesSelectArea::~CRouterBRouterTilesSelectArea()
{
}

bool CRouterBRouterTilesSelectArea::event(QEvent * event)
{
    if (event->type() == QEvent::ToolTip)
    {
        QHelpEvent *helpEvent = static_cast<QHelpEvent *>(event);
        QPoint tile = tileUnderMouse(helpEvent->pos());
        if (currentTile != tile)
        {
            emit sigTileToolTipChanged(tile);
            currentTile = tile;
        }

        QToolTip::showText(helpEvent->globalPos(), tileToolTip);
        return true;
    }
    return QWidget::event(event);
}

void CRouterBRouterTilesSelectArea::paintEvent(QPaintEvent *event)
{
    drawInvalidTiles();
    drawOutdatedTiles();
    drawCurrentTiles();
    drawSelectedTiles();
    drawOutstandingTiles();
}

void CRouterBRouterTilesSelectArea::mouseDoubleClickEvent(QMouseEvent * event)
{
}

void CRouterBRouterTilesSelectArea::mouseMoveEvent(QMouseEvent * event)
{
    if (event->buttons() == Qt::LeftButton)
    {
        canvas->moveMap(QPointF(event->pos()-mousePos));
        mousePos = event->pos();
    }
}

void CRouterBRouterTilesSelectArea::mousePressEvent(QMouseEvent * event)
{
    if (event->buttons() == Qt::LeftButton)
    {
        startPos = mousePos = event->pos();
    }
    button = event->buttons();
}

void CRouterBRouterTilesSelectArea::mouseReleaseEvent(QMouseEvent * event)
{
    if (button == Qt::LeftButton)
    {
        QPoint pos = event->pos();
        canvas->moveMap(QPointF(pos-mousePos));
        if (pos == startPos)
        {
            emit sigTileClicked(tileUnderMouse(pos));
        }
    }
}

void CRouterBRouterTilesSelectArea::drawOutdatedTiles()
{
    QPainter painter(this);
    painter.setPen(outdatedTilesPen);
    painter.setBrush(outdatedTilesBrush);

    for(const QPoint & tile : outdatedTiles)
    {
        painter.drawPolygon(tilePolygon(tile));
    }
}

void CRouterBRouterTilesSelectArea::drawOutstandingTiles()
{
    QPainter painter(this);
    painter.setPen(outstandingTilesPen);
    painter.setBrush(outstandingTilesBrush);

    for(const QPoint & tile : outstandingTiles)
    {
        painter.drawPolygon(tilePolygon(tile));
    }
}

void CRouterBRouterTilesSelectArea::drawInvalidTiles()
{
    QPainter painter(this);
    painter.setPen(invalidTilesPen);
    painter.setBrush(invalidTilesBrush);

    for(const QPoint & tile : invalidTiles)
    {
        painter.drawPolygon(tilePolygon(tile));
    }
}

void CRouterBRouterTilesSelectArea::drawCurrentTiles()
{
    QPainter painter(this);
    painter.setPen(currentTilesPen);
    painter.setBrush(currentTilesBrush);

    for(const QPoint & tile : currentTiles)
    {
        painter.drawPolygon(tilePolygon(tile));
    }
}

void CRouterBRouterTilesSelectArea::drawSelectedTiles()
{
    QPainter painter(this);
    painter.setPen(selectedTilesPen);
    painter.setBrush(selectedTilesBrush);

    for(const QPoint & tile : selectedTiles)
    {
        painter.drawPolygon(tilePolygon(tile));
    }
}

QPoint CRouterBRouterTilesSelectArea::tileUnderMouse(const QPointF & mousePos) const
{
    QPointF pos(mousePos);
    canvas->convertPx2Rad(pos);
    QPointF posDegF = pos * RAD_TO_DEG;
    QPoint tile(posDegF.x() > 0 ? posDegF.x()/5 : posDegF.x()/5 - 1
               ,posDegF.y() > 0 ? posDegF.y()/5 : posDegF.y()/5 - 1);
    return tile * 5;
}

QPolygonF CRouterBRouterTilesSelectArea::tilePolygon(const QPoint & tile) const
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

void CRouterBRouterTilesSelectArea::setInvalidTiles(const QVector<QPoint> &tiles)
{
    invalidTiles = tiles;
}

void CRouterBRouterTilesSelectArea::setOutdatedTiles(const QVector<QPoint> &tiles)
{
    outdatedTiles = tiles;
}

void CRouterBRouterTilesSelectArea::setCurrentTiles(const QVector<QPoint> &tiles)
{
    currentTiles = tiles;
}

void CRouterBRouterTilesSelectArea::setOutstandingTiles(const QVector<QPoint> &tiles)
{
    outstandingTiles = tiles;
}

void CRouterBRouterTilesSelectArea::setSelectedTiles(const QVector<QPoint> &tiles)
{
    selectedTiles = tiles;
}

void CRouterBRouterTilesSelectArea::setTileToolTip(QString toolTip)
{
    tileToolTip = toolTip;
}
