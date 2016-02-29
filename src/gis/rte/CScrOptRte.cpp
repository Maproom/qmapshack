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
    label->setText(rte->getInfo());
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

    connect(toolEditDetails, &QToolButton::clicked, this, &CScrOptRte::slotEditDetails);
    connect(toolDelete,      &QToolButton::clicked, this, &CScrOptRte::slotDelete);
    connect(toolCopy,        &QToolButton::clicked, this, &CScrOptRte::slotCopy);
    connect(toolCalc,        &QToolButton::clicked, this, &CScrOptRte::slotCalc);
    connect(toolReset,       &QToolButton::clicked, this, &CScrOptRte::slotReset);
    connect(toolEdit,        &QToolButton::clicked, this, &CScrOptRte::slotEdit);
    connect(toolInstruction, &QToolButton::toggled, this, &CScrOptRte::slotInstruction);
}

CScrOptRte::~CScrOptRte()
{
}

void CScrOptRte::slotEditDetails()
{
    CGisWidget::self().editItemByKey(key);
    deleteLater();
}

void CScrOptRte::slotDelete()
{
    CGisWidget::self().delItemByKey(key);
    deleteLater();
}

void CScrOptRte::slotCopy()
{
    CGisWidget::self().copyItemByKey(key);
    deleteLater();
}

void CScrOptRte::slotCalc()
{
    CGisWidget::self().calcRteByKey(key);
    deleteLater();
}

void CScrOptRte::slotReset()
{
    CGisWidget::self().resetRteByKey(key);
    deleteLater();
}

void CScrOptRte::slotEdit()
{
    CGisWidget::self().editRteByKey(key);
    deleteLater();
}

void CScrOptRte::slotInstruction(bool on)
{
    CGisWidget::self().focusRteByKey(on, key);
    deleteLater();
}

void CScrOptRte::draw(QPainter& p)
{
    IGisItem *item = CGisWidget::self().getItemByKey(key);
    if(nullptr == item)
    {
        QWidget::deleteLater();
        return;
    }
    item->drawHighlight(p);

    CDraw::bubble(p, geometry(), anchor.toPoint());
}

