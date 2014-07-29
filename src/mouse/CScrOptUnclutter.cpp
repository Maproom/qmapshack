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

#include "mouse/CScrOptUnclutter.h"
#include "gis/IGisItem.h"
#include "CMainWindow.h"
#include "canvas/CCanvas.h"

#include <QtWidgets>

const QPoint CScrOptUnclutter::positions[] =
{
      QPoint(-50,-23)
    , QPoint( 50,-23)
    , QPoint(-50, 23)
    , QPoint( 50, 23)
    , QPoint(-22,-55)
    , QPoint( 22,-55)
    , QPoint(-22, 55)
    , QPoint( 22, 55)

};


CScrOptUnclutter::CScrOptUnclutter(QWidget *parent)
    : IScrOpt(parent)
    , doSpecialCursor(false)
{

}

CScrOptUnclutter::~CScrOptUnclutter()
{

}

void CScrOptUnclutter::clear()
{
    if(doSpecialCursor)
    {
        QApplication::restoreOverrideCursor();
        doSpecialCursor = false;
    }
    items.clear();
}

void CScrOptUnclutter::mouseMoveEvent(QMouseEvent * e)
{
    IScrOpt::mouseMoveEvent(e);

    foreach(const item_t& item, items)
    {
        if(item.active.contains(mousePos))
        {
            if(!doSpecialCursor)
            {
                QApplication::setOverrideCursor(Qt::PointingHandCursor);
                doSpecialCursor = true;
            }
            return;
        }
    }

    if(doSpecialCursor)
    {
        QApplication::restoreOverrideCursor();
        doSpecialCursor = false;
        return;
    }
}

void CScrOptUnclutter::addItem(IGisItem * gisItem)
{
    items << item_t();
    item_t& item    = items.last();
    item.name       = gisItem->getName();
    item.key        = gisItem->getKey();
    item.icon       = gisItem->getIcon();
    item.area       = item.icon.rect();
    item.active     = item.area.adjusted(-10,-10,10,10);
}

QString CScrOptUnclutter::getItemKey(int index)
{
    if(index < items.size())
    {
        return items[index].key;
    }
    return QString::Null();
}

const CScrOptUnclutter::item_t * CScrOptUnclutter::selectItem(const QPoint& point)
{
    foreach(const item_t& item, items)
    {
        if(item.active.contains(point))
        {
            return &item;
        }
    }
    return 0;
}

void CScrOptUnclutter::draw(QPainter& p)
{
    QFontMetrics fm(CMainWindow::self().getMapFont());
    for(int cnt = 0; cnt < items.size(); cnt++)
    {
        item_t& item = items[cnt];

        if(item.text.isNull())
        {

            item.area.moveCenter(origin + positions[cnt]);
            item.active.moveCenter(item.area.center());
            item.text = fm.boundingRect(item.name);
            if(cnt & 0x01)
            {
                item.text.moveTopLeft(item.area.topRight() + QPoint( 17, fm.height()/2));
            }
            else
            {
                item.text.moveTopRight(item.area.topLeft() + QPoint(-17, fm.height()/2));
            }
            item.text.adjust(-4, -3, 4, 3);
        }
    }

    foreach(const item_t& item, items)
    {


        p.setPen(Qt::NoPen);
        p.setBrush(QColor(255,255,255,255));
        p.drawEllipse(item.area.center(), 20,20);
        p.setPen(QPen(Qt::lightGray,2));
        p.drawRoundedRect(item.text, 3, 3);

//        p.setPen(QPen(Qt::lightGray,2));
        p.setBrush(Qt::NoBrush);
        p.drawEllipse(item.area.center(), 18,18);

        p.drawPixmap(item.area, item.icon);
        CCanvas::drawText(item.name, p, item.text, Qt::darkBlue);
    }
}
