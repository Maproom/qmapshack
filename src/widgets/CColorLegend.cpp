/**********************************************************************************************
    Copyright (C) 2015 Christian Eichler code@christian-eichler.de

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

#include "widgets/CColorLegend.h"

#include <QtWidgets>

CColorLegend::CColorLegend(QWidget *parent)
    : QWidget(parent)
{
    colorRect = QRect(0, 0, colorWidth, colorHeight);
    colorRect.moveCenter(QPoint(colorWidth / 2, height() / 2));
}

CColorLegend::~CColorLegend()
{
}

void CColorLegend::setMinimum(qreal min)
{
    minimum = min;
    update();
}

void CColorLegend::setMaximum(qreal max)
{
    maximum = max;
    update();
}

void CColorLegend::setUnit(const QString &unit)
{
    this->unit = unit;
}

void CColorLegend::paintLabel(QPainter &p, qreal value)
{
    const int fontHeight = QFontMetrics(p.font()).ascent() + 1;
    const int posY = colorRect.bottom() + fontHeight / 2 - colorHeight * (value - minimum) / (maximum - minimum);

    if(value == minimum || value == maximum || (posY > colorRect.top() + 3*fontHeight / 2 && posY < colorRect.bottom() - fontHeight / 2))
    {
        p.drawText(colorWidth + 3, posY, QString("%1%2").arg(value).arg(unit));
    }

    QPen pen(p.pen());
    pen.setWidth(3);
    pen.setColor(Qt::white);
    p.setPen(pen);
    p.drawLine(colorWidth / 2 + 1, posY - fontHeight / 2, colorWidth + 1, posY - fontHeight / 2);

    pen.setColor(Qt::black);
    pen.setWidth(1);
    p.setPen(pen);
    p.drawLine(colorWidth / 2, posY - fontHeight / 2, colorWidth + 1, posY - fontHeight / 2);
}

void CColorLegend::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    colorRect.moveCenter(QPoint(colorWidth / 2, height() / 2));
}

void CColorLegend::paintEvent(QPaintEvent *event)
{
    if(isEnabled())
    {
        QPainter p(this);

        QLinearGradient grad(colorRect.topLeft(), colorRect.bottomLeft());
        grad.setColorAt(1.00, QColor(  0,   0, 255)); // blue
        grad.setColorAt(0.60, QColor(  0, 255,   0)); // green
        grad.setColorAt(0.40, QColor(255, 255,   0)); // yellow
        grad.setColorAt(0.00, QColor(255,   0,   0)); // red
    
        p.fillRect(colorRect, grad);
    
        paintLabel(p, minimum);
        paintLabel(p, maximum);
    
        if(minimum < 0.f && maximum > 0.f)
        {
            paintLabel(p, 0.f);
        }
        p.end();
    }
}
