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

#include "gis/trk/CScrOptTrk.h"
#include "gis/trk/CGisItemTrk.h"
#include "gis/CGisWidget.h"
#include "mouse/IMouse.h"
#include "canvas/CCanvas.h"
#include "CMainWindow.h"

CScrOptTrk::CScrOptTrk(CGisItemTrk * trk, const QPoint& origin, IMouse *parent)
    : IScrOpt(parent->getCanvas())
    , key(trk->getKey())
{
    setupUi(this);
    setOrigin(origin);
    label->setFont(CMainWindow::self().getMapFont());
    label->setText(trk->getInfo());

    toolProfile->setChecked(trk->hasUserFocus());
    toolEdit->setEnabled(!trk->isReadOnly());

    anchor = trk->getPointCloseBy(origin);
    move(anchor.toPoint() + QPoint(SCR_OPT_OFFSET,SCR_OPT_OFFSET));
    adjustSize();
    show();        

    connect(toolDelete, SIGNAL(clicked()), this, SLOT(slotDelete()));
    connect(toolEditDetails, SIGNAL(clicked()), this, SLOT(slotEditDetails()));
    connect(toolProfile, SIGNAL(toggled(bool)), this, SLOT(slotProfile(bool)));
    connect(toolCut, SIGNAL(clicked()), this, SLOT(slotCut()));
    connect(toolEdit, SIGNAL(clicked()), this, SLOT(slotEdit()));
}

CScrOptTrk::~CScrOptTrk()
{

}

void CScrOptTrk::slotDelete()
{
    CGisWidget::self().delItemByKey(key);
    deleteLater();
}

void CScrOptTrk::slotEditDetails()
{
    CGisWidget::self().editItemByKey(key);
    deleteLater();
}

void CScrOptTrk::slotProfile(bool on)
{
    CGisWidget::self().focusTrkByKey(on, key);
    deleteLater();
}

void CScrOptTrk::slotCut()
{
    CGisWidget::self().cutTrkByKey(key);
    deleteLater();
}

void CScrOptTrk::slotEdit()
{
    CGisWidget::self().editTrkByKey(key);
    deleteLater();
}

void CScrOptTrk::draw(QPainter& p)
{
    IGisItem * item = CGisWidget::self().getItemByKey(key);
    if(item == 0)
    {
        QWidget::deleteLater();
        return;
    }
    item->drawHighlight(p);

    drawBubble(anchor, p);
}
