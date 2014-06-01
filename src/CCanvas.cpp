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

#include "CMainWindow.h"
#include "CCanvas.h"
#include "map/CMap.h"

#include <QtGui>



CCanvas::CCanvas(QWidget *parent)
    : QWidget(parent)
{
    int count = 1;
    while(1)
    {
        QString name = tr("Canvas %1").arg(count);
        if(CMainWindow::self().findChild<CCanvas*>(name) == 0)
        {
            setObjectName(name);
            break;
        }
        count++;
    }

    map = new CMap(this);
}

CCanvas::~CCanvas()
{

}

void CCanvas::saveConfig(QSettings& cfg)
{
    map->saveConfig(cfg);
}

void CCanvas::loadConfig(QSettings& cfg)
{
    map->loadConfig(cfg);
}

void CCanvas::resizeEvent(QResizeEvent * e)
{
    needsRedraw = true;

    QSize s = e->size();
    if(map) map->resize(s);

    QWidget::resizeEvent(e);
}

void CCanvas::paintEvent(QPaintEvent * e)
{
    Q_UNUSED(e);

//    qDebug() << qApp->activeWindow() << qApp->focusWidget();

    if(!isVisible())
    {
        return;
    }

    const QRectF& r = e->rect();
    QPointF posFocus(12.10, 49.01);

    QPainter p;
    p.begin(this);
    //p.setRenderHints(QPainter::TextAntialiasing|QPainter::Antialiasing|QPainter::SmoothPixmapTransform|QPainter::HighQualityAntialiasing, true);

    // fill the backbround with default pattern
    p.fillRect(rect(), QBrush(Qt::darkGreen, Qt::CrossPattern));

    // ----- start to draw geo-referenced content -----
    // move coordinate system to center of the screen
    p.translate(width() >> 1, height() >> 1);

    map->draw(p, needsRedraw, posFocus, r);

    // restore coordinate system to default
    p.resetTransform();
    // ----- start to draw static content -----

    p.end();
    needsRedraw = false;

}

