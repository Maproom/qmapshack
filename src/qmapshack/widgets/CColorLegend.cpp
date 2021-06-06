/**********************************************************************************************
    Copyright (C) 2015 Christian Eichler <code@christian-eichler.de>

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
#include "gis/trk/CGisItemTrk.h"
#include "gis/trk/CKnownExtension.h"
#include "helpers/CDraw.h"
#include "widgets/CColorLegend.h"

#include <QtWidgets>

CColorLegend::CColorLegend(QWidget* parent, CGisItemTrk* trk)
    : QWidget(parent)
    , INotifyTrk(CGisItemTrk::eVisualColorLegend)
    , trk(trk)
{
    colorRect = QRect(0, 0, colorWidth, colorHeight);
    colorRect.moveCenter(QPoint(xOffset + colorWidth / 2, height() / 2));

    if(nullptr != trk)
    {
        background = true;
        xOffset = 5;

        trk->registerVisual(this);

        // read data from trk
        CColorLegend::updateData();
    }
}

CColorLegend::~CColorLegend()
{
    if(trk)
    {
        trk->unregisterVisual(this);
    }
}

void CColorLegend::setMouseFocus(const CTrackData::trkpt_t* pt)
{
    if(nullptr == pt)
    {
        val = NOFLOAT;
        return;
    }

    QString colorSource = trk->getColorizeSource();
    auto valueFunc = CKnownExtension::get(colorSource).valueFunc;
    const qreal factor = CKnownExtension::get(colorSource).factor;

    val = factor * valueFunc(*pt);
    val = qMin(val, maximum);
    val = qMax(val, minimum);
}

void CColorLegend::updateData()
{
    if(!trk->getColorizeSource().isEmpty() && (trk->getColorizeSource() != "activity"))
    {
        unit = trk->getColorizeUnit();
        minimum = trk->getColorizeLimitLow();
        maximum = trk->getColorizeLimitHigh();

        update();
        show();
    }
    else
    {
        hide();
    }
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

void CColorLegend::setUnit(const QString& unit)
{
    this->unit = unit;
    update();
}

int CColorLegend::paintLabel(QPainter& p, qreal value)
{
    const int fontHeight = QFontMetrics(p.font()).ascent() + 1;
    const qreal relativePos = (value - minimum) / (maximum - minimum);
    const int posY = colorRect.bottom() + fontHeight / 2 - (2 + colorRect.height()) * relativePos + 1;

    int posX = xOffset + colorWidth + 3;

    p.setPen( QPen(QBrush(palette().color(QPalette::Foreground)), 2.) );
    p.drawLine(posX, posY - fontHeight / 2 + 1, posX + 2, posY - fontHeight / 2 + 1);

    if(value == minimum || value == maximum
       || (posY > colorRect.top() + 3 * fontHeight / 2 && posY < colorRect.bottom() - fontHeight / 2))
    {
        posX += 5;
        int precision = !((int)value == value);     // returns 0 or 1
        const QString& labelText = QString("%1%2").arg(value, 0, 'f', precision).arg(unit);

        p.drawText(posX, posY, labelText);
        posX += QFontMetrics(p.font()).width(labelText);
    }

    return posX;
}

void CColorLegend::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);

    colorRect.setHeight(height() - 20);
    colorRect.moveCenter(QPoint(xOffset + colorWidth / 2, height() / 2));
    updateGeometry();
}

static qreal legendRound(qreal value, int powOffset)
{
    if(value == 0)
    {
        return 0;
    }

    int l10 = (int) (value > 0) ? log10(value) : log10(-value);

    qreal div = pow(10, l10 + powOffset);
    return ceil(value / div) * div;
}

void CColorLegend::paintEvent(QPaintEvent*/*event*/)
{
    const QFont& font = CMainWindow::self().getMapFont();
    if(isEnabled())
    {
        QPainter p(this);
        p.setFont(font);

        if(background)
        {
            p.setRenderHint(QPainter::Antialiasing);
            p.setOpacity(0.6);

            p.setPen( QPen(QBrush(Qt::darkGray), 2.) );
            p.setBrush(palette().color(backgroundRole()));
            p.drawRoundedRect(1, 1, width() - 2, height() - 2, RECT_RADIUS, RECT_RADIUS);

            p.setOpacity(1.f);
            p.setRenderHint(QPainter::Antialiasing, false);
        }

        // draw the black frame
        QRect borderRect(colorRect);
        borderRect += QMargins(1, 1, 1, 1);
        p.setPen( QPen(
                      QBrush(palette().color(QPalette::Foreground)),
                      2.,
                      Qt::SolidLine,
                      Qt::SquareCap,
                      Qt::MiterJoin
                      )
                  );
        p.drawRect(borderRect);

        // draw the gradient
        QLinearGradient grad(colorRect.topLeft(), colorRect.bottomLeft());
        grad.setColorAt(1.00, QColor(  0, 0, 255));   // blue
        grad.setColorAt(0.60, QColor(  0, 255, 0));   // green
        grad.setColorAt(0.40, QColor(255, 255, 0));   // yellow
        grad.setColorAt(0.00, QColor(255, 0, 0));     // red
        p.fillRect(colorRect, grad);

        int reqWidth = paintLabel(p, minimum);
        reqWidth = qMax(paintLabel(p, maximum), reqWidth);

        // draw values inbetween min/max
        const qreal delta = maximum - minimum;
        qint32 step = legendRound(delta / 8, 0);
        qint32 roundedMinimum = legendRound(minimum, delta > 60 ? -1 : 0);

        for(qint32 v = roundedMinimum; v < maximum; v += step)
        {
            reqWidth = qMax(paintLabel(p, v), reqWidth);
        }

        if(reqWidth + 5 != width())
        {
            setMinimumWidth(reqWidth + 5);
            resize(reqWidth + 5, height());
        }

        if(val != NOFLOAT)
        {
            qreal y = qFloor(colorRect.bottom() - (val - minimum) * (colorRect.height() - 1) / (maximum - minimum));
            p.setPen(QPen(Qt::darkGray, 2));
            p.drawLine(colorRect.left() + 2, y, colorRect.right() - 2, y);
        }

        p.end();
    }
}
