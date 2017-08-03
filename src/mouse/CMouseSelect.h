/**********************************************************************************************
    Copyright (C) 2016 Oliver Eichler oliver.eichler@gmx.de

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

#ifndef CMOUSESELECT_H
#define CMOUSESELECT_H

#include "canvas/CCanvas.h"
#include "gis/IGisItem.h"
#include "mouse/IMouseSelect.h"

class CGisDraw;
class CCanvas;
class CScrOptSelect;

class CMouseSelect : public IMouseSelect
{
    Q_OBJECT
public:
    CMouseSelect(CGisDraw * gis, CCanvas * parent);
    virtual ~CMouseSelect();

    void draw(QPainter& p, CCanvas::redraw_e needsRedraw, const QRect &rect) override;

private slots:
    void slotCopy() const;
    void slotRoute() const;
    void slotSymWpt() const;
    void slotCombine() const;
    void slotDelete() const;

private:
    /**
       @brief Get a temporary list of all items

       @Note: The list of items is only temporary and must not be used outside the calling method.

       @param items a temporary list to collect all item pointers
     */
    void findItems(QList<IGisItem *> &items);


    QList<IGisItem::key_t> itemKeys;
    IGisItem::selflags_t modeLastSel   = IGisItem::eSelectionNone;
    QRectF rectLastSel;

    quint32 cntWpt = 0;
    quint32 cntTrk = 0;
    quint32 cntRte = 0;
    quint32 cntOvl = 0;
};

#endif //CMOUSESELECT_H

