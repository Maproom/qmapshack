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

#include "gis/rte/CScrOptRte.h"
#include "gis/rte/CGisItemRte.h"
#include "gis/CGisWidget.h"
#include "mouse/IMouse.h"
#include "canvas/CCanvas.h"
#include "CMainWindow.h"

CScrOptRte::CScrOptRte(CGisItemRte *rte, const QPoint& point, IMouse *parent)
    : IScrOpt(parent->getCanvas())
    , key(rte->getKey())
{
    setupUi(this);
    setOrigin(point);
    label->setFont(CMainWindow::self().getMapFont());
    label->setText(rte->getInfo());
    adjustSize();

    anchor = rte->getPointCloseBy(point);
    move(anchor.toPoint() + QPoint(-width()/2,SCR_OPT_OFFSET));
    show();

    connect(toolDelete, SIGNAL(clicked()), this, SLOT(slotDelete()));
}

CScrOptRte::~CScrOptRte()
{

}

void CScrOptRte::slotDelete()
{
    CGisWidget::self().delItemByKey(key);
}


void CScrOptRte::draw(QPainter& p)
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
