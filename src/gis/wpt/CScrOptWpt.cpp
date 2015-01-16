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
#include "gis/wpt/CGisItemWpt.h"
#include "gis/wpt/CProjWpt.h"
#include "gis/wpt/CScrOptWpt.h"
#include "mouse/IMouse.h"

#include <QtWidgets>

CScrOptWpt::CScrOptWpt(CGisItemWpt *wpt, const QPoint& point, IMouse *parent)
    : IScrOpt(parent->getCanvas())
{
    key         = wpt->getKey();

    setupUi(this);
    setOrigin(point);
    label->setFont(CMainWindow::self().getMapFont());
    label->setText(wpt->getInfo());
    adjustSize();

    toolMove->setEnabled(!wpt->isReadOnly());
    toolProj->setEnabled(!wpt->isGeocache());
    photoAlbum->reload(wpt->getImages());

    anchor = wpt->getPointCloseBy(point);
    move(anchor.toPoint() + QPoint(-width()/2,SCR_OPT_OFFSET));
    show();

    connect(toolDelete, SIGNAL(clicked()), this, SLOT(slotDelete()));
    connect(toolEdit, SIGNAL(clicked()), this, SLOT(slotEdit()));
    connect(toolCopy, SIGNAL(clicked()), this, SLOT(slotCopy()));
    connect(toolMove, SIGNAL(clicked()), this, SLOT(slotMove()));
    connect(toolProj, SIGNAL(clicked()), this, SLOT(slotProj()));

    adjustSize();
}

CScrOptWpt::~CScrOptWpt()
{
}

void CScrOptWpt::slotDelete()
{
    CGisWidget::self().delItemByKey(key);
    deleteLater();
}

void CScrOptWpt::slotEdit()
{
    CGisWidget::self().editItemByKey(key);
    deleteLater();
}

void CScrOptWpt::slotCopy()
{
    CGisWidget::self().copyItemByKey(key);
    deleteLater();
}

void CScrOptWpt::slotMove()
{
    CGisWidget::self().moveWptByKey(key);
    deleteLater();
}

void CScrOptWpt::slotProj()
{
    CGisWidget::self().projWptByKey(key);
    deleteLater();
}


void CScrOptWpt::draw(QPainter& p)
{
    IGisItem * item = CGisWidget::self().getItemByKey(key);
    if(item == 0)
    {
        deleteLater();
        return;
    }
    item->drawHighlight(p);

    drawBubble2(anchor, p);
}
