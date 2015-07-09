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

#include "CMainWindow.h"
#include "canvas/CCanvas.h"
#include "gis/CGisWidget.h"
#include "gis/ovl/CGisItemOvlArea.h"
#include "gis/ovl/CScrOptOvlArea.h"
#include "mouse/IMouse.h"

CScrOptOvlArea::CScrOptOvlArea(CGisItemOvlArea *area, const QPoint &point, IMouse *parent)
    : IScrOpt(parent)
{
    key = area->getKey();

    setupUi(this);
    setOrigin(point);
    label->setFont(CMainWindow::self().getMapFont());
    label->setText(area->getInfo());
    adjustSize();

    anchor = area->getPointCloseBy(point);
    if((anchor - point).manhattanLength() > 50)
    {
        anchor = point;
    }
    move(anchor.toPoint() + QPoint(-width()/2,SCR_OPT_OFFSET));
    show();

    connect(toolEditDetails, SIGNAL(clicked()), this, SLOT(slotEditDetails()));
    connect(toolDelete, SIGNAL(clicked()), this, SLOT(slotDelete()));
    connect(toolCopy, SIGNAL(clicked()), this, SLOT(slotCopy()));
    connect(toolEdit, SIGNAL(clicked()), this, SLOT(slotEdit()));
}

CScrOptOvlArea::~CScrOptOvlArea()
{
}

void CScrOptOvlArea::slotEditDetails()
{
    hide();
    CGisWidget::self().editItemByKey(key);
    deleteLater();
}

void CScrOptOvlArea::slotCopy()
{
    CGisWidget::self().copyItemByKey(key);
    deleteLater();
}


void CScrOptOvlArea::slotDelete()
{
    CGisWidget::self().delItemByKey(key);
    deleteLater();
}

void CScrOptOvlArea::slotEdit()
{
    CGisWidget::self().editAreaByKey(key);
    deleteLater();
}

void CScrOptOvlArea::draw(QPainter& p)
{
    IGisItem * item = CGisWidget::self().getItemByKey(key);
    if(item == 0)
    {
        QWidget::deleteLater();
        return;
    }
    item->drawHighlight(p);

    drawBubble2(anchor, p);
}
