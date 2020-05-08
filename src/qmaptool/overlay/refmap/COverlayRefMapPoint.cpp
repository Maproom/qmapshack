/**********************************************************************************************
    Copyright (C) 2017 Oliver Eichler <oliver.eichler@gmx.de>

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

#include "overlay/refmap/COverlayRefMapPoint.h"
#include "units/IUnit.h"

COverlayRefMapPoint::COverlayRefMapPoint(qint32 cnt, const QPointF &ptRef, const QPointF &ptPtx, QTreeWidget *parent)
    : QTreeWidgetItem(parent)
{
    setText(eColumnCnt, QString::number(cnt));
    setPtPtx(ptPtx);
    setPtRef(ptRef);
}

void COverlayRefMapPoint::setPtPtx(const QPointF& pt)
{
    ptPtx.rx() = qRound(pt.x());
    ptPtx.ry() = qRound(pt.y());
    setText(eColumnXY, QString("%1, %2").arg(ptPtx.x()).arg(ptPtx.y()));
}

void COverlayRefMapPoint::setPtRef(const QPointF& pt)
{
    ptRef = pt;
    QString str;
    if(!IUnit::self().degToStr(ptRef.x(), ptRef.y(), str))
    {
        str = tr("bad coordinate");
    }
    setText(eColumnLonLat, str);
}

void COverlayRefMapPoint::setIndex(int n)
{
    setText(eColumnCnt, QString::number(n));
}

