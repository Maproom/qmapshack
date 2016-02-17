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

#ifndef CPLOTTRACK_H
#define CPLOTTRACK_H

#include "plot/ITrack.h"
#include <QWidget>

class CGisItemTrk;

class CPlotTrack : public QWidget, public ITrack
{
public:
    CPlotTrack(QWidget * parent);
    CPlotTrack(CGisItemTrk * trk, QWidget * parent);
    virtual ~CPlotTrack();

    void setMouseFocus(qreal lon, qreal lat);

protected:
    void resizeEvent(QResizeEvent *e) override;
    void paintEvent(QPaintEvent  *e) override;

private:

    QPointF pos = NOPOINTF;
};

#endif //CPLOTTRACK_H

