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
#include "CMainWindow.h"

CRouterBRouterTilesSelect::CRouterBRouterTilesSelect(QWidget *parent)
    : QWidget(parent)
{
    QTemporaryFile temp;
    temp.open();
    temp.close();

    QSettings view(temp.fileName(), QSettings::IniFormat);
    view.clear();

    CCanvas * source = CMainWindow::self().getVisibleCanvas();
    if(source)
    {
        source->saveConfig(view);
    }
    //view.setValue("grid/color",QColor(Qt::red).name());
    view.setValue("map/zoomIndex",16);

    CCanvas * canvas = new CCanvas(this,"BRouterTileDownload");
    // clone canvas by a temporary configuration file

    canvas->loadConfig(view);
    canvas->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    CRouterBRouterTilesSelectArea * selectArea = new CRouterBRouterTilesSelectArea(this,canvas);
    selectArea->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

//    QLayout * layout = new QVBoxLayout(this);
    QLayout * layout = new CRouterBRouterTilesSelectLayout(this);
    layout->addWidget(canvas);
    layout->addWidget(selectArea);
    canvas->lower();
    canvas->show();
    selectArea->show();
    //area->setAttribute(Qt::WA_TransparentForMouseEvents);
}

CRouterBRouterTilesSelect::~CRouterBRouterTilesSelect()
{
}


CRouterBRouterTilesSelectArea::CRouterBRouterTilesSelectArea(QWidget * parent, CCanvas * canvas)
    : QWidget(parent)
{
    this->canvas = canvas;
}

CRouterBRouterTilesSelectArea::~CRouterBRouterTilesSelectArea()
{
}

void CRouterBRouterTilesSelectArea::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QPen pen;
    pen.setColor(Qt::red);
    pen.setWidth(2);
    painter.setPen(pen);
    QRect rect = this->rect();
    painter.drawRect(rect);
    drawNewTiles();
}


void CRouterBRouterTilesSelectArea::mouseDoubleClickEvent(QMouseEvent * event)
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

void CRouterBRouterTilesSelectArea::mouseMoveEvent(QMouseEvent * event)
{
    canvas->moveMap(QPointF(event->pos()-pPressed));
    pPressed = event->pos();
}

void CRouterBRouterTilesSelectArea::mousePressEvent(QMouseEvent * event)
{
    pPressed = event->pos();
}

void CRouterBRouterTilesSelectArea::mouseReleaseEvent(QMouseEvent * event)
{
    canvas->moveMap(QPointF(event->pos()-pPressed));
}

void CRouterBRouterTilesSelectArea::drawExistingTiles()
{
    for(QPoint tile: existingTiles)
    {
        drawTileMark(tile,Qt::blue);
    }
}

void CRouterBRouterTilesSelectArea::drawNewTiles()
{
    for(QPoint tile:newTiles)
    {
        drawTileMark(tile,Qt::red);
    }
}

void CRouterBRouterTilesSelectArea::drawTileMark(QPoint tile, Qt::GlobalColor color)
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

CRouterBRouterTilesSelectLayout::CRouterBRouterTilesSelectLayout(QWidget * parent) : QLayout(parent)
{

}

CRouterBRouterTilesSelectLayout::~CRouterBRouterTilesSelectLayout()
{

}

void CRouterBRouterTilesSelectLayout::addItem(QLayoutItem * item)
{
    items.append(item);
}

QSize CRouterBRouterTilesSelectLayout::sizeHint() const
{
    return QSize(200,200);
}

void CRouterBRouterTilesSelectLayout::setGeometry(const QRect & r)
{
    for (QLayoutItem * item : items)
    {
        item->setGeometry(r);
    }
}

QLayoutItem * CRouterBRouterTilesSelectLayout::itemAt(int index) const
{
    return items.at(index);
}

QLayoutItem * CRouterBRouterTilesSelectLayout::takeAt(int index)
{
    QLayoutItem * item = items.at(index);
    items.removeAt(index);
    return item;
}

int CRouterBRouterTilesSelectLayout::count() const
{
    return items.size();
}
