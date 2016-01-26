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

class CColorLegend : public QWidget, public INotifyTrk
{
    Q_OBJECT
public:
    CColorLegend(QWidget *parent, CGisItemTrk *trk = nullptr);
    ~CColorLegend();

    void setMinimum(qreal min);
    void setMaximum(qreal max);

    void setUnit(const QString &unit);

    void updateData() override;
    void setMouseFocus(const CGisItemTrk::trkpt_t *pt) override;
    void setMouseRangeFocus(const CGisItemTrk::trkpt_t *pt1, const CGisItemTrk::trkpt_t *pt2) override {}
    void setMouseClickFocus(const CGisItemTrk::trkpt_t *pt) override {}


protected:
    void paintEvent(QPaintEvent  *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    int paintLabel(QPainter &p, qreal value);

    const int colorWidth  =  18;
    const int colorHeight = 256;

    QRect colorRect;
    QString unit;
    qreal minimum       = 0;
    qreal maximum       = 0;
    bool background     = false;
    int xOffset         = 1;
    CGisItemTrk *trk    = nullptr;
    qreal val           = NOFLOAT;
};

#endif // CCOLORLEGEND_H

