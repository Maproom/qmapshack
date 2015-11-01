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

#ifndef CCOLORLEGEND_H
#define CCOLORLEGEND_H

#include <QWidget>

class CColorLegend : public QWidget
{
    Q_OBJECT
public:
    CColorLegend(QWidget *parent);
    ~CColorLegend();

    QSize sizeHint() const
    {
        return QSize(20, 256);
    }

    void setMinimum(qreal min);

    void setMaximum(qreal max);

    void setUnit(const QString &unit);

protected:
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);

private:
    void paintLabel(QPainter &p, qreal value);

    const int colorWidth  =  10;
    const int colorHeight = 256;

    QRect colorRect;
    QString unit;
    qreal minimum;
    qreal maximum;
};

#endif // CCOLORLEGEND_H

