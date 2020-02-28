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

#include "canvas/CCanvas.h"
#include "CMainWindow.h"
#include "gis/CGisWorkspace.h"
#include "gis/rte/CGisItemRte.h"
#include "gis/rte/CScrOptRte.h"
#include "helpers/CDraw.h"
#include "mouse/CScrOptSemaphoreLocker.h"
#include "mouse/IMouse.h"

CScrOptRte::CScrOptRte(CGisItemRte *rte, const QPoint& point, IMouse *parent)
    : IScrOpt(parent)
    , key(rte->getKey())
{
    setupUi(this);
    setOrigin(point);
    label->setFont(CMainWindow::self().getMapFont());
    label->setText(rte->getInfo(IGisItem::eFeatureShowName|IGisItem::eFeatureShowLinks));
    adjustSize();

    toolInstruction->setEnabled(rte->isCalculated());
    toolInstruction->setChecked(rte->hasUserFocus());

    bool isOnDevice = rte->isOnDevice();
    toolEdit->setDisabled(isOnDevice);

    anchor = rte->getPointCloseBy(point);
    if((anchor - point).manhattanLength() > 50)
    {
        anchor = point;
    }
    move(anchor.toPoint() + QPoint(-width()/2, SCR_OPT_OFFSET));
    toolNogo->setChecked(rte->isNogo());
    show();

    connect(toolEditDetails, &QToolButton::clicked, this, &CScrOptRte::slotEditDetails);
    connect(toolTags,        &QToolButton::clicked, this, &CScrOptRte::slotTags);
    connect(toolDelete,      &QToolButton::clicked, this, &CScrOptRte::slotDelete);
    connect(toolCopy,        &QToolButton::clicked, this, &CScrOptRte::slotCopy);
    connect(toolCalc,        &QToolButton::clicked, this, &CScrOptRte::slotCalc);
    connect(toolReset,       &QToolButton::clicked, this, &CScrOptRte::slotReset);
    connect(toolEdit,        &QToolButton::clicked, this, &CScrOptRte::slotEdit);
    connect(toolReverse,     &QToolButton::clicked, this, &CScrOptRte::slotReverse);
    connect(toolInstruction, &QToolButton::toggled, this, &CScrOptRte::slotInstruction);
    connect(toolToTrack,     &QToolButton::clicked, this, &CScrOptRte::slotToTrack);
    connect(toolNogo,        &QToolButton::clicked, this, &CScrOptRte::slotNogo);

    connect(label, &QLabel::linkActivated, this, &CScrOptRte::slotLinkActivated);
}

CScrOptRte::~CScrOptRte()
{
}

void CScrOptRte::slotEditDetails()
{
    CScrOptSemaphoreLocker lock(*this);
    CGisWorkspace::self().editItemByKey(key);
    close();
}

void CScrOptRte::slotDelete()
{
    CScrOptSemaphoreLocker lock(*this);
    CGisWorkspace::self().delItemByKey(key);
    close();
}

void CScrOptRte::slotCopy()
{
    CScrOptSemaphoreLocker lock(*this);
    CGisWorkspace::self().copyItemByKey(key);
    close();
}

void CScrOptRte::slotCalc()
{
    CScrOptSemaphoreLocker lock(*this);
    CGisWorkspace::self().calcRteByKey(key);
    close();
}

void CScrOptRte::slotReset()
{
    CScrOptSemaphoreLocker lock(*this);
    CGisWorkspace::self().resetRteByKey(key);
    close();
}

void CScrOptRte::slotEdit()
{
    CScrOptSemaphoreLocker lock(*this);
    CGisWorkspace::self().editRteByKey(key);
    close();
}

void CScrOptRte::slotReverse()
{
    CScrOptSemaphoreLocker lock(*this);
    CGisWorkspace::self().reverseRteByKey(key);
    close();
}

void CScrOptRte::slotInstruction(bool on)
{
    CScrOptSemaphoreLocker lock(*this);
    CGisWorkspace::self().focusRteByKey(on, key);
    close();
}

void CScrOptRte::slotToTrack()
{
    CScrOptSemaphoreLocker lock(*this);
    CGisWorkspace::self().convertRouteToTrack(key);
    close();
}

void CScrOptRte::slotNogo()
{
    CScrOptSemaphoreLocker lock(*this);
    CGisWorkspace::self().toggleNogoItem(key);
    close();
}

void CScrOptRte::slotTags()
{
    CScrOptSemaphoreLocker lock(*this);
    CGisWorkspace::self().tagItemsByKey({key});
    close();
}

void CScrOptRte::draw(QPainter& p)
{
    IGisItem *item = CGisWorkspace::self().getItemByKey(key);
    if(nullptr == item)
    {
        close();
        return;
    }
    item->drawHighlight(p);

    CDraw::bubble(p, geometry(), anchor.toPoint(), backgroundColor);
}

