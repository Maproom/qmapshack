/**********************************************************************************************
    Copyright (C) 2018 Oliver Eichler <oliver.eichler@gmx.de>

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
#include "gis/CGisWorkspace.h"
#include "helpers/CDraw.h"
#include "helpers/CSettings.h"
#include "mouse/CMouseRangeTrk.h"
#include "mouse/range/CRangeToolSetup.h"
#include "mouse/range/CScrOptRangeTool.h"
#include "plot/CPlot.h"

#include <QtWidgets>

CScrOptRangeTool::CScrOptRangeTool(CGisItemTrk& trk, CMouseRangeTrk *mouse, CCanvas *canvas)
    : IScrOpt(mouse)
    , INotifyTrk(CGisItemTrk::eVisualRangeTool)
    , owner(mouse->objectName())
    , trk(trk)
    , canvas(canvas)
    , myMouse(mouse)
{
    setupUi(this);

    SETTINGS;
    cfg.beginGroup("TrackRange");
    actionHidePoints = eAction(cfg.value("actionHidePoints", int(actionHidePoints)).toInt());
    actionShowPoints = eAction(cfg.value("actionShowPoints", int(actionShowPoints)).toInt());
    actionActivity = eAction(cfg.value("actionActivity", int(actionActivity)).toInt());
    actionCopy = eAction(cfg.value("actionCopy", int(actionCopy)).toInt());
    actionDelete = eAction(cfg.value("actionDelete", int(actionDelete)).toInt());
    enableUpdateCavas = cfg.value("enableUpdateCavas", enableUpdateCavas).toBool();
    cfg.endGroup();

    trk.registerVisual(this);

    widgetTable->setTrack(&trk);

    plot = new CPlot(&trk, trk.limitsGraphRange, IPlot::eModeSimple, this);
    plot->setObjectName(owner);
    plot->setMinimumHeight(50);
    plot->setup(trk.getPropertyHandler()->propBySource("ql:ele"));
    plot->updateData();

    QBoxLayout * l = dynamic_cast<QBoxLayout*>(layout());
    l->insertWidget(0, plot);

    canvas->allowShowTrackOverlays(false);
    canvas->slotUpdateTrackInfo(false);
    connect(canvas, &CCanvas::sigResize, this, &CScrOptRangeTool::slotCanvasResize);

    adjustSize();
    slotCanvasResize(canvas->size());
    show();

    connect(toolBegDn, &QToolButton::pressed, this, [this](){updateCanvas = true; this->trk.incMouseRangeBegin(-1, owner);});
    connect(toolBegUp, &QToolButton::pressed, this, [this](){updateCanvas = true; this->trk.incMouseRangeBegin(+1, owner);});
    connect(toolEndDn, &QToolButton::pressed, this, [this](){updateCanvas = true; this->trk.incMouseRangeEnd(-1, owner);});
    connect(toolEndUp, &QToolButton::pressed, this, [this](){updateCanvas = true; this->trk.incMouseRangeEnd(+1, owner);});

    connect(toolBegDn, &QToolButton::released, this, &CScrOptRangeTool::slotResetUpdateCanvas);
    connect(toolBegUp, &QToolButton::released, this, &CScrOptRangeTool::slotResetUpdateCanvas);
    connect(toolEndDn, &QToolButton::released, this, &CScrOptRangeTool::slotResetUpdateCanvas);
    connect(toolEndUp, &QToolButton::released, this, &CScrOptRangeTool::slotResetUpdateCanvas);

    connect(toolBegToEnd, &QToolButton::pressed, this, [this](){updateCanvas = true; this->trk.newMouseRangeEnd(owner);});
    connect(toolEndToBeg, &QToolButton::pressed, this, [this](){updateCanvas = true; this->trk.newMouseRangeBegin(owner);});

    connect(toolBegToEnd, &QToolButton::released, this, &CScrOptRangeTool::slotResetUpdateCanvas);
    connect(toolEndToBeg, &QToolButton::released, this, &CScrOptRangeTool::slotResetUpdateCanvas);

    connect(toolReset, &QToolButton::clicked, this, &CScrOptRangeTool::slotResetRange);
    connect(toolHidePoints, &QToolButton::clicked, this, &CScrOptRangeTool::slotHidePoints);
    connect(toolShowPoints, &QToolButton::clicked, this, &CScrOptRangeTool::slotShowPoints);
    connect(toolSetActivity, &QToolButton::clicked, this, &CScrOptRangeTool::slotActivity);
    connect(toolDelete, &QToolButton::clicked, this, &CScrOptRangeTool::slotDelete);
    connect(toolCopy, &QToolButton::clicked, this, &CScrOptRangeTool::slotCopy);

    connect(toolSetup, &QToolButton::clicked, this, &CScrOptRangeTool::slotSetup);
}

CScrOptRangeTool::~CScrOptRangeTool()
{
    trk.unregisterVisual(this);
    canvas->allowShowTrackOverlays(true);
    canvas->slotUpdateTrackInfo(false);
    canvas->resetMouse();
}

void CScrOptRangeTool::slotCanvasResize(const QSize& sizeCanvas)
{
    qint32 cw = canvas->width();
    qint32 ch = canvas->height();

    QFontMetrics fm(CMainWindow::self().getMapFont());

    qint32 w = qMin(fm.width("X") * 125, sizeCanvas.width() - 60);
    qint32 h = height();

    setMinimumWidth(w);
    origin = {(cw - w) >> 1, ch - h - 10};
    move(origin);
    resize(w, height());
}

void CScrOptRangeTool::draw(QPainter& p)
{
    p.setPen(CDraw::penBorderGray);
    p.setBrush(CDraw::brushBackWhite);
    p.drawRoundedRect(geometry(), RECT_RADIUS, RECT_RADIUS);
}

void CScrOptRangeTool::updateData()
{
}

void CScrOptRangeTool::setMouseFocus(const CTrackData::trkpt_t * pt)
{
    if(pt == nullptr)
    {
        return;
    }

    if(updateCanvas && enableUpdateCavas)
    {
        canvas->followPosition({pt->lon, pt->lat});
    }
}

void CScrOptRangeTool::setMouseRangeFocus(const CTrackData::trkpt_t * pt1, const CTrackData::trkpt_t * pt2)
{
    canvas->triggerCompleteUpdate(CCanvas::eRedrawMouse);

    qint32 cnt = -1;
    if(pt1 != nullptr && pt2 != nullptr)
    {
        cnt = qAbs(pt1->idxTotal - pt2->idxTotal);
    }

    eSelState state = cnt == -1 ? eSelState::None : cnt == 0 ? eSelState::One : eSelState::Multiple;

    if(state == selState)
    {
        return;
    }

    frame->setEnabled(state != eSelState::None);
    toolBegDn->setEnabled(state != eSelState::None);
    toolBegUp->setEnabled(state == eSelState::Multiple);
    toolEndDn->setEnabled(state == eSelState::Multiple);
    toolEndUp->setEnabled(state != eSelState::None);
    toolBegToEnd->setEnabled(state == eSelState::Multiple);
    toolEndToBeg->setEnabled(state == eSelState::Multiple);
    toolReset->setEnabled(state != eSelState::None);
    selState = state;
}

void CScrOptRangeTool::setMouseClickFocus(const CTrackData::trkpt_t * pt)
{
}

void CScrOptRangeTool::slotSetup()
{
    CRangeToolSetup dlg(*this);
    dlg.exec();

    SETTINGS;
    cfg.beginGroup("TrackRange");
    cfg.setValue("actionHidePoints", int(actionHidePoints));
    cfg.setValue("actionShowPoints", int(actionShowPoints));
    cfg.setValue("actionActivity", int(actionActivity));
    cfg.setValue("actionCopy", int(actionCopy));
    cfg.setValue("actionDelete", int(actionDelete));
    cfg.setValue("enableUpdateCavas", enableUpdateCavas);
    cfg.endGroup();
}

void CScrOptRangeTool::slotResetRange()
{
    QMutexLocker lock(&IGisItem::mutexItems);
    trk.resetMouseRange();
    canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawGis);
}

void CScrOptRangeTool::slotHidePoints()
{
    QMutexLocker lock(&IGisItem::mutexItems);

    trk.hideSelectedPoints();
    actions[int(actionHidePoints)]();
    canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawGis);
}

void CScrOptRangeTool::slotShowPoints()
{
    QMutexLocker lock(&IGisItem::mutexItems);

    trk.showSelectedPoints();
    actions[int(actionShowPoints)]();
    canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawGis);
}

void CScrOptRangeTool::slotActivity()
{
    // mutex will be locked triggering a menu item
    CActivityTrk::getMenu(trk.getKey(), &CGisWorkspace::self(), true);
    actions[int(actionActivity)]();
    canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawGis);
}

void CScrOptRangeTool::slotCopy()
{
    QMutexLocker lock(&IGisItem::mutexItems);

    trk.copySelectedPoints();
    actions[int(actionCopy)]();
    canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawGis);
}

void CScrOptRangeTool::slotDelete()
{
    QMutexLocker lock(&IGisItem::mutexItems);

    trk.deleteSelectedPoints();
    actions[int(actionDelete)]();
    canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawGis);
}
