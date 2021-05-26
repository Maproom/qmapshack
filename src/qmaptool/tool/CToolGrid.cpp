/**********************************************************************************************
    Copyright (C) 2017 Oliver Eichler <oliver.eichler@gmx.de>

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
#include "items/CItemRefMap.h"
#include "overlay/COverlayRefMap.h"
#include "tool/CToolGrid.h"

#include <QtWidgets>

#define TESTITEM(cmd) \
    if(nullptr == item) \
    { \
        return cmd; \
    } \


CToolGrid::CToolGrid(QWidget* parent)
    : QWidget(parent)
{
    setupUi(this);
    labelHelp->setText(tr("By placing 4 reference points at the corners of a grid "
                          "square and referencing them by their top left corner, "
                          "the width and height, all the other grid points can be "
                          "estimated."));

    labelHelp->setVisible(CMainWindow::self().showToolHelp()->isChecked());
    connect(CMainWindow::self().showToolHelp(), &QAction::toggled, labelHelp, &QLabel::setVisible);

    connect(pushOk, &QPushButton::clicked, this, &CToolGrid::slotOk);
    connect(pushCancel, &QPushButton::clicked, this, &CToolGrid::slotCancel);
    connect(overlay, &COverlayGridTool::sigChanged, pushOk, &QPushButton::setEnabled);
    connect(pushReset, &QPushButton::clicked, overlay, &COverlayGridTool::slotReset);
}

void CToolGrid::registerItem(CItemRefMap* item)
{
    this->item = item;
    overlay->registerItem(item);
}

bool CToolGrid::drawFx(QPainter& p, CCanvas::redraw_e needsRedraw)
{
    TESTITEM(false)
    item->drawFx(p, needsRedraw);
    overlay->drawFx(p, needsRedraw);

    return true;
}

void CToolGrid::mousePressEventFx(QMouseEvent* e)
{
    TESTITEM()
    item->CItemFile::mousePressEventFx(e);
}

void CToolGrid::mouseMoveEventFx(QMouseEvent* e)
{
    TESTITEM()
    item->CItemFile::mouseMoveEventFx(e);
    if(!item->getMapIsMoving())
    {
        overlay->mouseMoveEventFx(e);
    }
}

void CToolGrid::mouseReleaseEventFx(QMouseEvent* e)
{
    TESTITEM()
    if(!item->getMapDidMove())
    {
        overlay->mouseReleaseEventFx(e);
    }
    item->CItemFile::mouseReleaseEventFx(e);
}

void CToolGrid::wheelEventFx(QWheelEvent* e)
{
    TESTITEM()
    item->CItemFile::wheelEventFx(e);
}

void CToolGrid::leaveEventFx(QEvent* e)
{
    TESTITEM()
    item->CItemFile::leaveEventFx(e);
    overlay->leaveEventFx(e);
}

QCursor CToolGrid::getCursorFx()
{
    TESTITEM(Qt::ArrowCursor)
    return overlay->getCursorFx();
}


void CToolGrid::slotOk()
{
    TESTITEM()
    item->addRefPoints(overlay->getRefPoints());
    slotCancel();
}


void CToolGrid::slotCancel()
{
    CMainWindow::self().showToolBox();
    registerItem(nullptr);
}
