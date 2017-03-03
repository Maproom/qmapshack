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

CRouterBRouterTilesSelectArea::CRouterBRouterTilesSelectArea(QWidget * parent, CRouterBRouterTilesSelect * select, CCanvas * canvas)
    : QWidget(dynamic_cast<QWidget *>(parent))
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
    this->select = select;

    setMouseTracking(true);
}

CRouterBRouterTilesSelectArea::~CRouterBRouterTilesSelectArea()
{
}

void CRouterBRouterTilesSelectArea::updateTiles()
{
    update();
}

bool CRouterBRouterTilesSelectArea::event(QEvent * event)
{
    if (event->type() == QEvent::ToolTip)
    {
        QHelpEvent *helpEvent = static_cast<QHelpEvent *>(event);
        QPoint tile = tileUnderMouse(helpEvent->pos());
        if (select->invalidTiles.contains(tile))
        {
            QToolTip::showText(helpEvent->globalPos(),tr("no routing-data available"));
        }
        else
        {
            QString fileName = select->fileNameFromTile(tile);
            QHash<QString,CRouterBRouterTilesStatus*>::const_iterator it = select->tilesDownloadStatus.constFind(fileName);
            if (it != select->tilesDownloadStatus.constEnd())
            {
                CRouterBRouterTilesStatus * status = it.value();

                if (select->outstandingTiles.contains(tile))
                {
                    QToolTip::showText(helpEvent->globalPos(),
                                       QString(tr("being downloaded (%1 of %2)"))
                                       .arg(select->formatSize(status->val))
                                       .arg(select->formatSize(status->max)));
                }
                else if (select->selectedTiles.contains(tile))
                {
                    CRouterBRouterTilesSelect::tile_s data = select->getOnlineTileData(tile);
                    QToolTip::showText(helpEvent->globalPos(),
                                       QString(tr("selected (%1, %2"))
                                       .arg(select->formatSize(status->size))
                                       .arg(data.date.toString(Qt::DefaultLocaleShortDate)));
                }
            }
            else if (select->outdatedTiles.contains(tile))
            {
                CRouterBRouterTilesSelect::tile_s local = select->getLocalTileData(tile);
                CRouterBRouterTilesSelect::tile_s remote = select->getOnlineTileData(tile);
                QToolTip::showText(helpEvent->globalPos(),
                                   QString(tr("local data outdated (%1, %2 - remote %3, %4)"))
                                   .arg(select->formatSize(local.size))
                                   .arg(local.date.toString(Qt::DefaultLocaleShortDate))
                                   .arg(select->formatSize(remote.size))
                                   .arg(remote.date.toString(Qt::DefaultLocaleShortDate)));
            }
            else if (select->currentTiles.contains(tile))
            {
                CRouterBRouterTilesSelect::tile_s local = select->getLocalTileData(tile);
                QToolTip::showText(helpEvent->globalPos(),
                                   QString(tr("local data up to date (%1, %2)"))
                                   .arg(select->formatSize(local.size))
                                   .arg(local.date.toString(Qt::DefaultLocaleShortDate)));
            }
            else
            {
                CRouterBRouterTilesSelect::tile_s remote = select->getOnlineTileData(tile);
                QToolTip::showText(helpEvent->globalPos(),
                                   QString(tr("no local data, online available (%1, %2)"))
                                   .arg(select->formatSize(remote.size))
                                   .arg(remote.date.toString(Qt::DefaultLocaleShortDate)));
            }
        }
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
    else
    {
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
    for(QPoint tile : select->outdatedTiles)
    {
        painter.drawPolygon(tilePolygon(tile));
    }
}

void CRouterBRouterTilesSelectArea::drawOutstandingTiles()
{
    QPainter painter(this);
    painter.setPen(outstandingTilesPen);
    painter.setBrush(outstandingTilesBrush);
    for(QPoint tile : select->outstandingTiles)
    {
        painter.drawPolygon(tilePolygon(tile));
    }
}

void CRouterBRouterTilesSelectArea::drawInvalidTiles()
{
    QPainter painter(this);
    painter.setPen(invalidTilesPen);
    painter.setBrush(invalidTilesBrush);
    for(QPoint tile : select->invalidTiles)
    {
        painter.drawPolygon(tilePolygon(tile));
    }
}

void CRouterBRouterTilesSelectArea::drawCurrentTiles()
{
    QPainter painter(this);
    painter.setPen(currentTilesPen);
    painter.setBrush(currentTilesBrush);

    for(QPoint tile : select->currentTiles)
    {
        painter.drawPolygon(tilePolygon(tile));
    }
}

void CRouterBRouterTilesSelectArea::drawSelectedTiles()
{
    QPainter painter(this);
    painter.setPen(selectedTilesPen);
    painter.setBrush(selectedTilesBrush);

    for(QPoint tile : select->selectedTiles)
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
