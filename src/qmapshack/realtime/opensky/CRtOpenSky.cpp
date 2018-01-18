/**********************************************************************************************
    Copyright (C) 2018 Oliver Eichler oliver.eichler@gmx.de

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

#include "realtime/opensky/CRtOpenSky.h"

#include <QtGui>

CRtOpenSky::CRtOpenSky(QTreeWidget *parent)
    : IRtItem(parent)
{
    setIcon(eColumnIcon, QIcon("://icons/32x32/OpenSky.png"));
    setText(eColumnName, "OpenSky");    
    setCheckState(eColumnCheckBox, Qt::Checked);
}


void CRtOpenSky::drawItem(QPainter& p, const QPolygonF& viewport, QList<QRectF>& blockedAreas, CRtDraw * rt)
{
    if(checkState(eColumnCheckBox) != Qt::Checked)
    {
        return;
    }

    p.setPen(Qt::black);
    p.drawRect(QRect(200,200,100,100));
}
