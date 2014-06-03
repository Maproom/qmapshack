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
#include "GeoMath.h"
#include "map/CMap.h"
#include "map/CGrid.h"
#include "units/IUnit.h"

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

    setMouseTracking(true);

    map     = new CMap(this);
    grid    = new CGrid(map);
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

    if(!isVisible())
    {
        return;
    }

    const QRectF& r = e->rect();
    QPointF posFocus(12.00, 49.00);

    QPainter p;
    p.begin(this);
    USE_ANTI_ALIASING(p,true);

    // fill the backbround with default pattern
    p.fillRect(rect(), QBrush(Qt::darkGreen, Qt::CrossPattern));

    // ----- start to draw geo-referenced content -----
    // move coordinate system to center of the screen
    p.translate(width() >> 1, height() >> 1);

    map->draw(p, needsRedraw, posFocus, r);

    // restore coordinate system to default
    p.resetTransform();
    // ----- start to draw static content -----

    grid->draw(p, rect());

    drawScale(p);

    p.end();
    needsRedraw = false;

}

void CCanvas::mousePressEvent(QMouseEvent * e)
{

    QWidget::mousePressEvent(e);
}

void CCanvas::mouseMoveEvent(QMouseEvent * e)
{
    QString str;
    QPointF pos = e->pos();
    map->convertPx2Rad(pos);
    emit sigMousePosition(pos * RAD_TO_DEG);

    QWidget::mouseMoveEvent(e);
}

void CCanvas::mouseReleaseEvent(QMouseEvent *e)
{

    QWidget::mouseReleaseEvent(e);
}

void CCanvas::wheelEvent(QWheelEvent * e)
{
    map->zoom(/*CResources::self().flipMouseWheel()*/ 0 ? (e->delta() > 0) : (e->delta() < 0), needsRedraw);
    update();
}


void CCanvas::drawText(const QString& str, QPainter& p, const QPoint& center, const QColor& color)
{
    CCanvas::drawText(str,p,center, color, CMainWindow::self().getMapFont());
}


void CCanvas::drawText(const QString& str, QPainter& p, const QPoint& center, const QColor& color, const QFont& font)
{

    QFontMetrics    fm(font);
    QRect           r = fm.boundingRect(str);

    r.moveCenter(center);

    p.setPen(Qt::white);
    p.setFont(font);

    p.drawText(r.topLeft() - QPoint(-1,-1), str);
    p.drawText(r.topLeft() - QPoint( 0,-1), str);
    p.drawText(r.topLeft() - QPoint(+1,-1), str);

    p.drawText(r.topLeft() - QPoint(-1, 0), str);
    p.drawText(r.topLeft() - QPoint(+1, 0), str);

    p.drawText(r.topLeft() - QPoint(-1,+1), str);
    p.drawText(r.topLeft() - QPoint( 0,+1), str);
    p.drawText(r.topLeft() - QPoint(+1,+1), str);

    p.setPen(color);
    p.drawText(r.topLeft(),str);

}


void CCanvas::drawText(const QString& str, QPainter& p, const QRect& r, const QColor& color)
{

    p.setPen(Qt::white);
    p.setFont(CMainWindow::self().getMapFont());

    p.drawText(r.adjusted(-1,-1,-1,-1),Qt::AlignCenter,str);
    p.drawText(r.adjusted( 0,-1, 0,-1),Qt::AlignCenter,str);
    p.drawText(r.adjusted(+1,-1,+1,-1),Qt::AlignCenter,str);

    p.drawText(r.adjusted(-1, 0,-1, 0),Qt::AlignCenter,str);
    p.drawText(r.adjusted(+1, 0,+1, 0),Qt::AlignCenter,str);

    p.drawText(r.adjusted(-1,+1,-1,+1),Qt::AlignCenter,str);
    p.drawText(r.adjusted( 0,+1, 0,+1),Qt::AlignCenter,str);
    p.drawText(r.adjusted(+1,+1,+1,+1),Qt::AlignCenter,str);

    p.setPen(color);
    p.drawText(r,Qt::AlignCenter,str);

}

void CCanvas::drawScale(QPainter& p)
{

    if(!CMainWindow::self().isScaleVisible())
    {
        return;
    }


    // step I: get the approximate distance for 200px in the bottom right corner
    QPointF brc(rect().bottomRight() - QPoint(50,30));
    QPointF pt1 = brc;
    QPointF pt2 = brc - QPoint(-200,0);

    map->convertPx2Rad(pt1);
    map->convertPx2Rad(pt2);

    qreal d = GPS_Math_Distance(pt1.x(), pt1.y(), pt2.x(), pt2.y());

    // step II: derive the actual scale length in [m]
    qreal a = (int)log10(d);
    qreal b = log10(d) - a;

    if(0 <= b && b < log10(3.0f))
    {
        d = 1 * pow(10,a);
    }
    else if(log10(3.0f) < b && b < log10(5.0f))
    {
        d = 3 * pow(10,a);
    }
    else
    {
        d = 5 * pow(10,a);
    }

    // step III: convert the scale length from [m] into [px]
    pt1 = brc;
    map->convertPx2Rad(pt1);
    pt2 = GPS_Math_Wpt_Projection(pt1, d, -90 * DEG_TO_RAD);

    map->convertRad2Px(pt1);
    map->convertRad2Px(pt2);

    p.setPen(QPen(Qt::white, 9));
    p.drawLine(pt1, pt2 + QPoint(9,0));
    p.setPen(QPen(Qt::black, 7));
    p.drawLine(pt1, pt2 + QPoint(9,0));
    p.setPen(QPen(Qt::white, 5));
    p.drawLine(pt1, pt2 + QPoint(9,0));

    QVector<qreal> pattern;
    pattern << 2 << 4;
    QPen pen(Qt::black, 5, Qt::CustomDashLine);
    pen.setDashPattern(pattern);
    p.setPen(pen);
    p.drawLine(pt1, pt2 + QPoint(9,0));


    QPoint pt3(pt2.x() + (pt1.x() - pt2.x())/2, pt2.y());

    QString val, unit;
    IUnit::self().meter2distance(d,val,unit);
    drawText(QString("%1 %2").arg(val).arg(unit), p, pt3, Qt::black);
}

