/**********************************************************************************************
    Copyright (C) 2017 Norbert Truchsess <norbert.truchsess@t-online.de>

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

#include "gis/rte/router/brouter/CRouterBRouterTilesSelectArea.h"

#include <QToolTip>

#include "canvas/CCanvas.h"
#include "gis/proj_x.h"
#include "gis/rte/router/brouter/CRouterBRouterTilesSelect.h"

const QPen CRouterBRouterTilesSelectArea::gridPen = QPen(Qt::magenta);
const QPen CRouterBRouterTilesSelectArea::outdatedTilesPen = QPen(Qt::gray);
const QPen CRouterBRouterTilesSelectArea::currentTilesPen = QPen(Qt::darkGreen);
const QPen CRouterBRouterTilesSelectArea::selectedTilesPen = QPen(Qt::blue);
const QPen CRouterBRouterTilesSelectArea::outstandingTilesPen = QPen(Qt::yellow);
const QPen CRouterBRouterTilesSelectArea::invalidTilesPen = QPen(Qt::gray);
const QBrush CRouterBRouterTilesSelectArea::outdatedTilesBrush = QBrush(Qt::gray, Qt::Dense5Pattern);
const QBrush CRouterBRouterTilesSelectArea::currentTilesBrush = QBrush(Qt::darkGreen, Qt::Dense3Pattern);
const QBrush CRouterBRouterTilesSelectArea::selectedTilesBrush = QBrush(Qt::blue, Qt::Dense3Pattern);
const QBrush CRouterBRouterTilesSelectArea::outstandingTilesBrush = QBrush(Qt::yellow, Qt::Dense3Pattern);
const QBrush CRouterBRouterTilesSelectArea::invalidTilesBrush = QBrush(Qt::gray, Qt::DiagCrossPattern);

CRouterBRouterTilesSelectArea::CRouterBRouterTilesSelectArea(QWidget* parent, CCanvas* canvas) : QWidget(parent) {
  this->canvas = canvas;

  setMouseTracking(true);
}

CRouterBRouterTilesSelectArea::~CRouterBRouterTilesSelectArea() {}

bool CRouterBRouterTilesSelectArea::event(QEvent* event) {
  if (event->type() == QEvent::ToolTip) {
    QHelpEvent* helpEvent = static_cast<QHelpEvent*>(event);
    const QPoint& tile = tileUnderMouse(helpEvent->pos());
    if (currentTile != tile) {
      emit sigTileToolTipChanged(tile);
      currentTile = tile;
    }

    QToolTip::showText(helpEvent->globalPos(), tileToolTip);
    return true;
  }
  return QWidget::event(event);
}

void CRouterBRouterTilesSelectArea::paintEvent(QPaintEvent* event) {
  drawGrid();
  drawTiles(invalidTilesPen, invalidTilesBrush, invalidTiles);
  drawTiles(outdatedTilesPen, outdatedTilesBrush, outdatedTiles);
  drawTiles(currentTilesPen, currentTilesBrush, currentTiles);
  drawTiles(selectedTilesPen, selectedTilesBrush, selectedTiles);
  drawTiles(outstandingTilesPen, outstandingTilesBrush, outstandingTiles);
}

void CRouterBRouterTilesSelectArea::mouseMoveEvent(QMouseEvent* event) {
  if (event->buttons() == Qt::LeftButton) {
    canvas->moveMap(QPointF(event->pos() - mousePos));
    mousePos = event->pos();
  }
}

void CRouterBRouterTilesSelectArea::mousePressEvent(QMouseEvent* event) {
  if (event->buttons() == Qt::LeftButton) {
    startPos = mousePos = event->pos();
  }
  button = event->buttons();
}

void CRouterBRouterTilesSelectArea::mouseReleaseEvent(QMouseEvent* event) {
  if (button == Qt::LeftButton) {
    const QPoint& pos = event->pos();
    canvas->moveMap(QPointF(pos - mousePos));
    if (pos == startPos) {
      emit sigTileClicked(tileUnderMouse(pos));
    }
  }
}

void CRouterBRouterTilesSelectArea::drawGrid() {
  QPainter painter(this);
  painter.setPen(gridPen);

  for (const QPoint& tile : std::as_const(gridTiles)) {
    painter.drawPolyline(gridPolygon(tile));
  }
}

void CRouterBRouterTilesSelectArea::drawTiles(const QPen& pen, const QBrush& brush, const QVector<QPoint>& tiles) {
  QPainter painter(this);
  painter.setPen(pen);
  painter.setBrush(brush);

  for (const QPoint& tile : tiles) {
    painter.drawPolygon(tilePolygon(tile));
  }
}

QPoint CRouterBRouterTilesSelectArea::tileUnderMouse(const QPointF& mousePos) const {
  QPointF pos(mousePos);
  canvas->convertPx2Rad(pos);
  QPointF posDegF = pos * RAD_TO_DEG;
  QPoint tile(posDegF.x() > 0 ? posDegF.x() / CRouterBRouterTilesSelect::tileSize
                              : posDegF.x() / CRouterBRouterTilesSelect::tileSize - 1,
              posDegF.y() > 0 ? posDegF.y() / CRouterBRouterTilesSelect::tileSize
                              : posDegF.y() / CRouterBRouterTilesSelect::tileSize - 1);
  return tile * CRouterBRouterTilesSelect::tileSize;
}

QPolygonF CRouterBRouterTilesSelectArea::tilePolygon(const QPoint& tile) const {
  QPointF p0(tile.x(), tile.y());
  QPointF p1(tile.x() + CRouterBRouterTilesSelect::tileSize, tile.y());
  QPointF p2(tile.x() + CRouterBRouterTilesSelect::tileSize, tile.y() + CRouterBRouterTilesSelect::tileSize);
  QPointF p3(tile.x(), tile.y() + CRouterBRouterTilesSelect::tileSize);

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

  return polygon;
}

QPolygonF CRouterBRouterTilesSelectArea::gridPolygon(const QPoint& tile) const {
  QPointF p0(tile.x(), tile.y());
  QPointF p1(tile.x() + CRouterBRouterTilesSelect::tileSize, tile.y());
  QPointF p2(tile.x() + CRouterBRouterTilesSelect::tileSize, tile.y() + CRouterBRouterTilesSelect::tileSize);

  p0 *= DEG_TO_RAD;
  p1 *= DEG_TO_RAD;
  p2 *= DEG_TO_RAD;

  canvas->convertRad2Px(p0);
  canvas->convertRad2Px(p1);
  canvas->convertRad2Px(p2);

  QPolygonF polygon;
  polygon << p0;
  polygon << p1;
  polygon << p2;

  return polygon;
}
