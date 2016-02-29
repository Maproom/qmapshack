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

#include "gis/CGisDraw.h"
#include "gis/CGisWidget.h"
#include "mouse/CMouseSelect.h"
#include "mouse/CScrOptSelect.h"

#include <QtWidgets>

CMouseSelect::CMouseSelect(CGisDraw *gis, CCanvas *parent)
    : IMouseSelect(gis, parent)
{
    cursor = QCursor(QPixmap("://cursors/cursorSelectArea.png"),0,0);

    canvas->reportStatus("IMouseSelect", tr("<b>Select Items On Map</b><br/>Select a rectangular area on the map. Use the left mouse button and move the mouse. Abort with a right click. Adjust the selection by point-click-move on the corners."));

    CScrOptSelect * scrOptSelect;
    scrOpt = scrOptSelect = new CScrOptSelect(this);

    connect(scrOptSelect->toolCopy,   &QToolButton::clicked, this, &CMouseSelect::slotCopy);
    connect(scrOptSelect->toolDelete, &QToolButton::clicked, this, &CMouseSelect::slotDelete);
}

CMouseSelect::~CMouseSelect()
{
    canvas->reportStatus("CMouseSelect::Stat", "");
}



void CMouseSelect::findItems(QList<IGisItem*>& items)
{
    CScrOptSelect * scrOptSelect = dynamic_cast<CScrOptSelect*>((IScrOpt*)scrOpt);
    IGisItem::selflags_t modeSelection = scrOptSelect->getModeSelection();

    if((rectSelection == rectLastSel) && (modeSelection == modeLastSel))
    {
        if(itemKeys.isEmpty())
        {
            scrOptSelect->frameFunction->setDisabled(true);
        }
        else
        {
            scrOptSelect->frameFunction->setEnabled(true);
            CGisWidget::self().getItemsByKeys(itemKeys, items);
        }
    }
    else
    {
        itemKeys.clear();

        QRectF area;
        rectRad2Px(rectSelection, area);
        CGisWidget::self().getItemsByArea(area, modeSelection, items);

        quint32 cntWpt = 0;
        quint32 cntTrk = 0;
        quint32 cntRte = 0;
        quint32 cntOvl = 0;
        foreach(IGisItem * item, items)
        {
            itemKeys << item->getKey();
            switch(item->type())
            {
            case IGisItem::eTypeWpt:
                cntWpt++;
                break;

            case IGisItem::eTypeTrk:
                cntTrk++;
                break;

            case IGisItem::eTypeRte:
                cntRte++;
                break;

            case IGisItem::eTypeOvl:
                cntOvl++;
                break;
            }
        }

        QString msg = tr("<b>Selected:</b><br/>");
        msg += tr("%1 tracks<br/>").arg(cntTrk);
        msg += tr("%1 waypoints<br/>").arg(cntWpt);
        msg += tr("%1 routes<br/>").arg(cntRte);
        msg += tr("%1 areas<br/>").arg(cntOvl);

        canvas->reportStatus("CMouseSelect::Stat",msg);

        scrOptSelect->frameFunction->setDisabled(itemKeys.isEmpty());

        rectLastSel = rectSelection;
        modeLastSel = modeSelection;
    }
}

void CMouseSelect::draw(QPainter& p, CCanvas::redraw_e needsRedraw, const QRect &rect)
{
    if(rectSelection.isNull())
    {
        return;
    }

    QList<IGisItem*> items;
    findItems(items);

    foreach(IGisItem * item, items)
    {
        item->drawHighlight(p);
    }

    IMouseSelect::draw(p, needsRedraw, rect);
}



void CMouseSelect::slotCopy() const
{
    CGisWidget::self().copyItemsByKey(itemKeys);
    canvas->resetMouse();
}

void CMouseSelect::slotDelete() const
{
    CGisWidget::self().delItemsByKey(itemKeys);
    canvas->resetMouse();
}
