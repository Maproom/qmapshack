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
#include "gis/CGisWidget.h"
#include "gis/prj/IGisProject.h"
#include "gis/trk/CGisItemTrk.h"
#include "gis/trk/CScrOptTrk.h"
#include "helpers/CDraw.h"
#include "mouse/IMouse.h"

CScrOptTrk::CScrOptTrk(CGisItemTrk * trk, const QPoint& point, IMouse *parent)
    : IScrOpt(parent)
    , key(trk->getKey())
{
    setupUi(this);
    setOrigin(point);
    label->setFont(CMainWindow::self().getMapFont());
    label->setText(trk->getInfo(true, false));
    adjustSize();

    toolProfile->setChecked(trk->hasUserFocus());

    bool isOnDevice = trk->isOnDevice();
    toolCut->setDisabled(isOnDevice);
    toolEdit->setDisabled(isOnDevice);
    toolReverse->setDisabled(isOnDevice);
    toolRange->setDisabled(isOnDevice);

    IGisProject * project = trk->getParentProject();
    if(project != nullptr)
    {
        toolCombine->setEnabled(project->getItemCountByType(IGisItem::eTypeTrk) > 1);
    }

    anchor = trk->getPointCloseBy(point);
    if((anchor - point).manhattanLength() > 50)
    {
        anchor = point;
        toolCut->setEnabled(false);
    }

    move(anchor.toPoint() + QPoint(-width()/2,SCR_OPT_OFFSET));
    show();

    connect(toolEditDetails, &QToolButton::clicked, this, &CScrOptTrk::hide);
    connect(toolDelete,      &QToolButton::clicked, this, &CScrOptTrk::hide);
    connect(toolCopy,        &QToolButton::clicked, this, &CScrOptTrk::hide);
    connect(toolProfile,     &QToolButton::toggled, this, &CScrOptTrk::hide);
    connect(toolCut,         &QToolButton::clicked, this, &CScrOptTrk::hide);
    connect(toolEdit,        &QToolButton::clicked, this, &CScrOptTrk::hide);
    connect(toolReverse,     &QToolButton::clicked, this, &CScrOptTrk::hide);
    connect(toolCombine,     &QToolButton::clicked, this, &CScrOptTrk::hide);
    connect(toolRange,       &QToolButton::clicked, this, &CScrOptTrk::hide);
    connect(toolEditDetails, &QToolButton::clicked, this, &CScrOptTrk::slotEditDetails);
    connect(toolDelete,      &QToolButton::clicked, this, &CScrOptTrk::slotDelete);
    connect(toolCopy,        &QToolButton::clicked, this, &CScrOptTrk::slotCopy);
    connect(toolProfile,     &QToolButton::toggled, this, &CScrOptTrk::slotProfile);
    connect(toolCut,         &QToolButton::clicked, this, &CScrOptTrk::slotCut);
    connect(toolEdit,        &QToolButton::clicked, this, &CScrOptTrk::slotEdit);
    connect(toolReverse,     &QToolButton::clicked, this, &CScrOptTrk::slotReverse);
    connect(toolCombine,     &QToolButton::clicked, this, &CScrOptTrk::slotCombine);
    connect(toolRange,       &QToolButton::clicked, this, &CScrOptTrk::slotRange);
    connect(toolCopyWithWpt, &QToolButton::clicked, this, &CScrOptTrk::slotCopyWithWpt);


    // reset user focus if the track has it
    trk->setMouseFocusByPoint(NOPOINT, CGisItemTrk::eFocusMouseMove, "CScrOptTrk");
    trk->setMouseFocusByPoint(point, CGisItemTrk::eFocusMouseClick, "CScrOptTrk");
}

CScrOptTrk::~CScrOptTrk()
{
}

void CScrOptTrk::slotDelete()
{
    CGisWidget::self().delItemByKey(key);
    deleteLater();
}

void CScrOptTrk::slotCopy()
{
    CGisWidget::self().copyItemByKey(key);
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

void CScrOptTrk::slotReverse()
{
    CGisWidget::self().reverseTrkByKey(key);
    deleteLater();
}

void CScrOptTrk::slotCombine()
{
    CGisWidget::self().combineTrkByKey(key);
    deleteLater();
}

void CScrOptTrk::slotRange()
{
    CGisWidget::self().rangeTrkByKey(key);
    deleteLater();
}

void CScrOptTrk::slotCopyWithWpt()
{
    CGisWidget::self().copyTrkWithWptByKey(key);
    deleteLater();
}

void CScrOptTrk::draw(QPainter& p)
{
    IGisItem * item = CGisWidget::self().getItemByKey(key);
    if(nullptr == item)
    {
        QWidget::deleteLater();
        return;
    }
    item->drawHighlight(p);

    CDraw::bubble(p, geometry(), anchor.toPoint());
}
