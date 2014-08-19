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

#include "plot/CPlot.h"
#include "canvas/CCanvas.h"

#include <QtWidgets>

CPlot::CPlot(CPlotData::axistype_e type, mode_e mode, QWidget *parent)
    : QWidget(parent)
    , mode(mode)
    , needsRedraw(true)
    , showScale(true)
    , thinLine(false)
    , posMouse(-1, -1)
{
    setMouseTracking(true);

    data = new CPlotData(type, this);

    if(mode == eModeIcon)
    {
        showScale = false;
        thinLine = true;
    }
}

CPlot::~CPlot()
{

}

void CPlot::paintEvent(QPaintEvent * e)
{
    QPainter p(this);
    draw(p);
}

void CPlot::resizeEvent(QResizeEvent * e)
{
//    setSizes();

//    initialYMin = m_pData->y().min();
//    initialYMax = m_pData->y().max();

    buffer = QImage(e->size(), QImage::Format_ARGB32);

    needsRedraw = true;
    update();
}

void CPlot::leaveEvent(QEvent * e)
{
    needsRedraw = true;
    posMouse    = QPoint(-1, -1);

//    CTrackDB::self().setPointOfFocusByIdx(-1);
    QApplication::restoreOverrideCursor();

    update();
}


void CPlot::enterEvent(QEvent * e)
{
    needsRedraw = true;
    QApplication::setOverrideCursor(Qt::PointingHandCursor);
    update();
}

void CPlot::draw(QPainter& p)
{
    if(needsRedraw)
    {
        draw();
        needsRedraw = false;
    }

    p.drawImage(0,0,buffer);
}

void CPlot::mouseMoveEvent(QMouseEvent * e)
{
    posMouse = QPoint(-1,-1);

    posMouse = e->pos();
}

void CPlot::draw()
{
    buffer.fill(Qt::transparent);
    QPainter p(&buffer);
    USE_ANTI_ALIASING(p, true);

    if(mode == eModeNormal)
    {
        p.fillRect(rect(),Qt::white);
    }
    else if(mode == eModeIcon)
    {
        QRect r = rect();
        r.adjust(2,2,-2,-2);
        if(underMouse() || posMouse.x() != -1)
        {
            p.setPen(CCanvas::penBorderBlue);
            p.setBrush(QColor(255,255,255,255));
        }
        else
        {
            p.setPen(CCanvas::penBorderBlack);
            p.setBrush(QColor(255,255,255,150));
        }
        PAINT_ROUNDED_RECT(p,r);
    }

}
