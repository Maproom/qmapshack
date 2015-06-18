/**********************************************************************************************
    Copyright (C) 2014-2015 Oliver Eichler oliver.eichler@gmx.de

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

#ifndef CMOUSEEDITRTE_H
#define CMOUSEEDITRTE_H

#include "gis/IGisItem.h"
#include "mouse/line/IMouseEditLine.h"

class CGisItemRte;

class CMouseEditRte : public IMouseEditLine
{
public:
    CMouseEditRte(const QPointF& point, CGisDraw * gis, CCanvas * parent);
    CMouseEditRte(CGisItemRte &rte, CGisDraw * gis, CCanvas * parent);
    virtual ~CMouseEditRte();

    void mousePressEvent(QMouseEvent * e);

protected slots:
    void slotAbort();
    void slotCopyToNew();
    void slotCopyToOrig();


protected:
    IGisLine * getGisLine();

};

#endif //CMOUSEEDITRTE_H

