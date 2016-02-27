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

#include <functional>
using std::bind;

CMouseSelect::CMouseSelect(CGisDraw *gis, CCanvas *parent)
    : IMouse(gis, parent)
{
    cursor = QCursor(QPixmap("://cursors/cursorSelectArea.png"),0,0);

    canvas->reportStatus("CMouseSelect", tr("<b>Select Items On Map</b><br/>Select a rectangular area on the map. Use the left mouse button and move the mouse. Abort with a right click. Adjust the selection by point-click-move on the corners."));

    scrOptSelect  = new CScrOptSelect(this);
    modeSelection = scrOptSelect->toolModeExact->isChecked() ? IGisItem::eSelectionExact : IGisItem::eSelectionIntersect;

    auto slotModeExact      = bind(&CMouseSelect::slotModeSwitch, this, IGisItem::eSelectionExact, std::placeholders::_1);
    auto slotModeIntersect  = bind(&CMouseSelect::slotModeSwitch, this, IGisItem::eSelectionIntersect, std::placeholders::_1);

    connect(scrOptSelect->toolModeExact,     &QToolButton::toggled, this, slotModeExact);
    connect(scrOptSelect->toolModeIntersect, &QToolButton::toggled, this, slotModeIntersect);
    connect(scrOptSelect->toolCopy,          &QToolButton::clicked, this, &CMouseSelect::slotCopy);
    connect(scrOptSelect->toolDelete,        &QToolButton::clicked, this, &CMouseSelect::slotDelete);
}

CMouseSelect::~CMouseSelect()
{
    canvas->reportStatus("CMouseSelect", "");
    canvas->reportStatus("CMouseSelect::Stat", "");
    delete scrOptSelect;
}

void CMouseSelect::rectRad2Px(const QRectF& rectSrc, QRectF& rectTar) const
{
    QPointF pt1 = rectSrc.topLeft();
    QPointF pt2 = rectSrc.bottomRight();

    gis->convertRad2Px(pt1);
    gis->convertRad2Px(pt2);

    rectTar = QRectF(pt1,pt2);
}

void CMouseSelect::placeScrOpt()
{
    QRectF rectSel;
    rectRad2Px(rectSelection, rectSel);

    if(((rectTopLeft.width() + rectTopRight.width() + scrOptSelect->width()) < rectSel.width()) && (scrOptSelect->height() < rectSel.height()))
    {
        scrOptSelect->move(rectSel.x() + (rectSel.width()  - scrOptSelect->width())/2, rectSel.y());
        scrOptSelect->show();
    }
    else
    {
        scrOptSelect->hide();
    }
}

void CMouseSelect::findItems()
{
    if((rectSelection == rectLastSel) && (modeSelection == modeLastSel))
    {
        return;
    }

    itemKeys.clear();
    QList<IGisItem*> items;
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
    msg += tr("%1 waypoints<br/>").arg(cntWpt);
    msg += tr("%1 tracks<br/>").arg(cntTrk);
    msg += tr("%1 routes<br/>").arg(cntRte);
    msg += tr("%1 areas<br/>").arg(cntOvl);

    canvas->reportStatus("CMouseSelect::Stat",msg);

    rectLastSel = rectSelection;
}

void CMouseSelect::draw(QPainter& p, CCanvas::redraw_e needsRedraw, const QRect &rect)
{
    if(rectSelection.isNull())
    {
        return;
    }

    QList<IGisItem*> items;
    CGisWidget::self().getItemsByKeys(itemKeys, items);
    foreach(IGisItem * item, items)
    {
        item->drawHighlight(p);
    }

    QRectF rectSel;
    rectRad2Px(rectSelection, rectSel);

    QRectF rectScr = canvas->rect();

    rectTopLeft.moveTopLeft(rectSel.topLeft());
    rectTopRight.moveTopRight(rectSel.topRight());
    rectBottomLeft.moveBottomLeft(rectSel.bottomLeft());
    rectBottomRight.moveBottomRight(rectSel.bottomRight());

    QPainterPath path;
    path.addRect(rectScr);
    path.addRect(rectSel);

    p.setPen(Qt::black);
    p.setBrush(QColor(0,0,0,128));
    p.drawPath(path);

    p.setBrush(Qt::lightGray);
    p.drawRect(rectTopLeft);
    p.drawRect(rectTopRight);
    p.drawRect(rectBottomLeft);
    p.drawRect(rectBottomRight);

    p.setBrush(Qt::red);
    switch(corner)
    {
    case eCornerTopLeft:
        p.drawRect(rectTopLeft);
        break;

    case eCornerTopRight:
        p.drawRect(rectTopRight);
        break;

    case eCornerBottomLeft:
        p.drawRect(rectBottomLeft);
        break;

    case eCornerBottomRight:
        p.drawRect(rectBottomRight);
        break;
    }

    placeScrOpt();
    findItems();
}

void CMouseSelect::mousePressEvent(QMouseEvent * e)
{
    e->accept();

    canvas->reportStatus("CMouseSelect", "");

    if(e->button() == Qt::RightButton)
    {
        canvas->resetMouse();
        canvas->update();
    }
    else if(e->button() == Qt::LeftButton)
    {
        switch(state)
        {
        case eStateIdle:
        {
            QPointF pos = e->pos();
            gis->convertPx2Rad(pos);
            rectSelection.setTopLeft(pos);
            rectSelection.setBottomRight(pos);
            posInitial = pos;
            state = eStateInitial;
            break;
        }

        case eStateMap:
        {
            if(corner != eCornerNone)
            {
                state = eStateResize;
            }
            else
            {
                lastPos = e->pos();
                state = eStateMapMoving;
            }
            break;
        }
        }
    }
}

void CMouseSelect::mouseMoveEvent(QMouseEvent * e)
{
    e->accept();

    switch(state)
    {
    case eStateInitial:
    {
        QPointF pos = e->pos();
        gis->convertPx2Rad(pos);

        if(pos.x() < posInitial.x())
        {
            rectSelection.setLeft(pos.x());
        }
        else
        {
            rectSelection.setRight(pos.x());
        }

        if(pos.y() < posInitial.y())
        {
            rectSelection.setBottom(pos.y());
        }
        else
        {
            rectSelection.setTop(pos.y());
        }

        placeScrOpt();
        canvas->update();
        break;
    }

    case eStateMap:
    {
        corner_e _corner = corner;
        QPoint pos = e->pos();
        if(rectTopLeft.contains(pos))
        {
            offset = pos - rectTopLeft.topLeft();
            corner = eCornerTopLeft;
        }
        else if(rectTopRight.contains(pos))
        {
            offset = pos - rectTopRight.topRight();
            corner = eCornerTopRight;
        }
        else if(rectBottomLeft.contains(pos))
        {
            offset = pos - rectBottomLeft.bottomLeft();
            corner = eCornerBottomLeft;
        }
        else if(rectBottomRight.contains(pos))
        {
            offset = pos - rectBottomRight.bottomRight();
            corner = eCornerBottomRight;
        }
        else
        {
            corner = eCornerNone;
        }

        if(corner != _corner)
        {
            canvas->update();
        }
        break;
    }

    case eStateMapMoving:
    {
        QPoint pos = e->pos();

        if(pos != lastPos)
        {
            QPoint delta = pos - lastPos;
            canvas->moveMap(delta);
            lastPos     = pos;
        }
        break;
    }

    case eStateResize:
    {
        QPointF pos = e->pos() - offset;
        gis->convertPx2Rad(pos);
        switch(corner)
        {
        case eCornerTopLeft:
            rectSelection.setTopLeft(pos);
            break;

        case eCornerTopRight:
            rectSelection.setTopRight(pos);
            break;

        case eCornerBottomLeft:
            rectSelection.setBottomLeft(pos);
            break;

        case eCornerBottomRight:
            rectSelection.setBottomRight(pos);
            break;
        }

        canvas->update();
        break;
    }
    }
}

void CMouseSelect::mouseReleaseEvent(QMouseEvent *e)
{
    e->accept();

    if(!rectSelection.isNull())
    {
        QPointF pt1 = rectSelection.topLeft();
        QPointF pt2 = rectSelection.bottomRight();

        gis->convertRad2Px(pt1);
        gis->convertRad2Px(pt2);

        QRectF rectSel(pt1,pt2);

        if(rectSel.width() < 40 || rectSel.height() < 40)
        {
            rectSelection = QRectF();
        }
    }

    state = rectSelection.isNull() ? eStateIdle : eStateMap;

    canvas->update();
}

void CMouseSelect::wheelEvent(QWheelEvent * e)
{
}

void CMouseSelect::slotModeSwitch(IGisItem::selection_e mode, bool checked)
{
    if(!checked)
    {
        return;
    }
    switch(mode)
    {
    case IGisItem::eSelectionExact:
        canvas->reportStatus("CMouseSelect", tr("<b>Exact Mode</b><br/>All selected items have to be completely inside the selected area.<br/>"));
        modeSelection = mode;
        break;

    case IGisItem::eSelectionIntersect:
        canvas->reportStatus("CMouseSelect", tr("<b>Intersecting Mode</b><br/>All selected items have to be inside or at least intersect the selected area.<br/>"));
        modeSelection = mode;
        break;
    }

    findItems();
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
