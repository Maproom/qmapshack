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
#include "gis/trk/CGisItemTrk.h"
#include "helpers/INotifiable.h"

class CColorLegend : public QWidget, public INotifiable
{
    Q_OBJECT
public:
    CColorLegend(QWidget *parent);
    CColorLegend(QWidget *parent, CGisItemTrk *trk);
    ~CColorLegend();

    void setMinimum(qreal min);
    void setMaximum(qreal max);

    void setUnit(const QString &unit);

    void notify();

protected:
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);

private:
    int paintLabel(QPainter &p, qreal value);

    const int colorWidth  =  18;
    const int colorHeight = 256;

    QRect colorRect;
    QString unit;
    qreal minimum;
    qreal maximum;
    bool  background = false;
    int   xOffset    = 0;
    CGisItemTrk *trk = nullptr;
};

#endif // CCOLORLEGEND_H

