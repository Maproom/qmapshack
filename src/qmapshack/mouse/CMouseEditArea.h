/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler <oliver.eichler@gmx.de>
    Copyright (C) 2018 Norbert Truchsess <norbert.truchsess@t-online.de>

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

#ifndef CMOUSEEDITAREA_H
#define CMOUSEEDITAREA_H

#include "gis/IGisItem.h"
#include "mouse/line/IMouseEditLine.h"

class CGisItemOvlArea;

class CMouseEditArea : public IMouseEditLine
{
    Q_OBJECT
public:
    CMouseEditArea(const QPointF& point, CGisDraw* gis, CCanvas* canvas, CMouseAdapter* mouse);
    CMouseEditArea(CGisItemOvlArea& area, CGisDraw* gis, CCanvas* canvas, CMouseAdapter* mouse);
    virtual ~CMouseEditArea();

protected slots:
    void slotAbort()      override;
    void slotCopyToNew()  override;
    void slotCopyToOrig() override;

protected:
    void drawLine(const QPolygonF& l, const QColor color, int width, QPainter& p) override;

    IGisLine* getGisLine() const override;
};

#endif //CMOUSEEDITAREA_H

