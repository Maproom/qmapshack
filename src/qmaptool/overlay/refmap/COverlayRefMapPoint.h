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

#ifndef COVERLAYREFMAPPOINT_H
#define COVERLAYREFMAPPOINT_H

#include <QCoreApplication>
#include <QTreeWidgetItem>

class COverlayRefMapPoint : public QTreeWidgetItem
{
    Q_DECLARE_TR_FUNCTIONS(COverlayRefMapPoint)
public:
    COverlayRefMapPoint(qint32 cnt, const QPointF& ptRef, const QPointF& ptPtx, QTreeWidget * parent);
    virtual ~COverlayRefMapPoint() = default;

    enum column_e
    {
        eColumnCnt
        , eColumnXY
        , eColumnLonLat
    };

    void setPtPtx(const QPointF& pt);
    void setPtRef(const QPointF& pt);
    void setIndex(int n);

    const QPointF& getPtPtx() const
    {
        return ptPtx;
    }

    const QPointF& getPtRef() const
    {
        return ptRef;
    }

    bool operator<(const QTreeWidgetItem& p) const override
    {
        const COverlayRefMapPoint& pt = dynamic_cast<const COverlayRefMapPoint&>(p);
        qreal v1 = ptPtx.y() * 1000000000 + ptPtx.x();
        qreal v2 = pt.ptPtx.y() * 1000000000 + pt.ptPtx.x();
        return v1 < v2;
    }

private:
    QPointF ptRef;
    QPointF ptPtx;
};

#endif //COVERLAYREFMAPPOINT_H

