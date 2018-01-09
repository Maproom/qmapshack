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
#include "gis/CGisWorkspace.h"
#include "gis/rte/CGisItemRte.h"
#include "gis/rte/CScrOptRte.h"
#include "helpers/CDraw.h"
#include "mouse/IMouse.h"

CScrOptRte::CScrOptRte(CGisItemRte *rte, const QPoint& point, IMouse *parent)
    : IScrOpt(parent)
    , key(rte->getKey())
{
    setupUi(this);
    setOrigin(point);
    label->setFont(CMainWindow::self().getMapFont());
    label->setText(rte->getInfo(IGisItem::eFeatureShowName));
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
    move(anchor.toPoint() + QPoint(-width()/2,SCR_OPT_OFFSET));
    show();

    connect(toolEditDetails, &QToolButton::clicked, this, &CScrOptRte::hide);
    connect(toolDelete,      &QToolButton::clicked, this, &CScrOptRte::hide);
    connect(toolCopy,        &QToolButton::clicked, this, &CScrOptRte::hide);
    connect(toolCalc,        &QToolButton::clicked, this, &CScrOptRte::hide);
    connect(toolReset,       &QToolButton::clicked, this, &CScrOptRte::hide);
    connect(toolEdit,        &QToolButton::clicked, this, &CScrOptRte::hide);
    connect(toolInstruction, &QToolButton::toggled, this, &CScrOptRte::hide);
    connect(toolToTrack,     &QToolButton::clicked, this, &CScrOptRte::hide);

    connect(toolEditDetails, &QToolButton::clicked, this, &CScrOptRte::slotEditDetails);
    connect(toolDelete,      &QToolButton::clicked, this, &CScrOptRte::slotDelete);
    connect(toolCopy,        &QToolButton::clicked, this, &CScrOptRte::slotCopy);
    connect(toolCalc,        &QToolButton::clicked, this, &CScrOptRte::slotCalc);
    connect(toolReset,       &QToolButton::clicked, this, &CScrOptRte::slotReset);
    connect(toolEdit,        &QToolButton::clicked, this, &CScrOptRte::slotEdit);
    connect(toolReverse,     &QToolButton::clicked, this, &CScrOptRte::slotReverse);
    connect(toolInstruction, &QToolButton::toggled, this, &CScrOptRte::slotInstruction);
    connect(toolToTrack,     &QToolButton::clicked, this, &CScrOptRte::slotToTrack);
}

CScrOptRte::~CScrOptRte()
{
}

void CScrOptRte::slotEditDetails()
{
    CGisWorkspace::self().editItemByKey(key);
    deleteLater();
}

void CScrOptRte::slotDelete()
{
    CGisWorkspace::self().delItemByKey(key);
    deleteLater();
}

void CScrOptRte::slotCopy()
{
    CGisWorkspace::self().copyItemByKey(key);
    deleteLater();
}

void CScrOptRte::slotCalc()
{
    CGisWorkspace::self().calcRteByKey(key);
    deleteLater();
}

void CScrOptRte::slotReset()
{
    CGisWorkspace::self().resetRteByKey(key);
    deleteLater();
}

void CScrOptRte::slotEdit()
{
    CGisWorkspace::self().editRteByKey(key);
    deleteLater();
}

void CScrOptRte::slotReverse()
{
    CGisWorkspace::self().reverseRteByKey(key);
    deleteLater();
}

void CScrOptRte::slotInstruction(bool on)
{
    CGisWorkspace::self().focusRteByKey(on, key);
    deleteLater();
}

void CScrOptRte::slotToTrack()
{
    CGisWorkspace::self().convertRouteToTrack(key);
    deleteLater();
}

void CScrOptRte::draw(QPainter& p)
{
    IGisItem *item = CGisWorkspace::self().getItemByKey(key);
    if(nullptr == item)
    {
        QWidget::deleteLater();
        return;
    }
    item->drawHighlight(p);

    CDraw::bubble(p, geometry(), anchor.toPoint());
}

