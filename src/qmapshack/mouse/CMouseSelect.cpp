/**********************************************************************************************
    Copyright (C) 2016 Oliver Eichler <oliver.eichler@gmx.de>

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

#include "mouse/CMouseSelect.h"

#include <QtWidgets>

#include "CMainWindow.h"
#include "gis/CGisDraw.h"
#include "gis/CGisWorkspace.h"
#include "gis/trk/CActivityTrk.h"
#include "helpers/CTryMutexLocker.h"
#include "helpers/CWptIconManager.h"
#include "mouse/CScrOptSelect.h"
#include "poi/IPoiItem.h"

QMutex CMouseSelect::mutex;

CMouseSelect::CMouseSelect(CGisDraw* gis, CCanvas* canvas, CMouseAdapter* mouse) : IMouseSelect(gis, canvas, mouse) {
  cursor = QCursor(QPixmap("://cursors/cursorSelectArea.png"), 0, 0);

  canvas->reportStatus("IMouseSelect", tr("<b>Select Items On Map</b><br/>Select a rectangular area on the map. "
                                          "Use the left mouse button and move the mouse. Abort with a right "
                                          "click. Adjust the selection by point-click-move on the corners."));

  CScrOptSelect* scrOptSelect;
  scrOpt = scrOptSelect = new CScrOptSelect(this);

  connect(&CGisWorkspace::self(), &CGisWorkspace::sigChanged, this, &CMouseSelect::slotUpdate);
  connect(scrOptSelect->toolCopy, &QToolButton::clicked, this, &CMouseSelect::slotCopy);
  connect(scrOptSelect->toolRoute, &QToolButton::clicked, this, &CMouseSelect::slotRoute);
  connect(scrOptSelect->toolEditPrxWpt, &QToolButton::clicked, this, &CMouseSelect::slotEditPrxWpt);
  connect(scrOptSelect->toolSymWpt, &QToolButton::clicked, this, &CMouseSelect::slotSymWpt);
  connect(scrOptSelect->toolEleWptTrk, &QToolButton::clicked, this, &CMouseSelect::slotEleWptTrk);
  connect(scrOptSelect->toolCombineTrk, &QToolButton::clicked, this, &CMouseSelect::slotCombineTrk);
  connect(scrOptSelect->toolActivityTrk, &QToolButton::clicked, this, &CMouseSelect::slotActivityTrk);
  connect(scrOptSelect->toolColorTrk, &QToolButton::clicked, this, &CMouseSelect::slotColorTrk);
  connect(scrOptSelect->toolDelete, &QToolButton::clicked, this, &CMouseSelect::slotDelete);
}

CMouseSelect::~CMouseSelect() { canvas->reportStatus("CMouseSelect::Stat", ""); }

void CMouseSelect::findItems(QList<IGisItem*>& items) {
  CTryMutexLocker<QMutex> lock(mutex);
  if (!lock.try_lock()) {
    return;
  }

  CScrOptSelect* scrOptSelect = dynamic_cast<CScrOptSelect*>((IScrOpt*)scrOpt);
  IGisItem::selflags_t modeSelection = scrOptSelect->getModeSelection();

  if ((rectSelection == rectLastSel) && (modeSelection == modeLastSel)) {
    if (!itemKeys.isEmpty()) {
      CGisWorkspace::self().getItemsByKeys(itemKeys, items);
    }
  } else {
    itemKeys.clear();

    QRectF area;
    rectRad2Deg(rectSelection, area);
    CGisWorkspace::self().getItemsByArea(area, modeSelection, items);

    cntWpt = 0;
    cntTrk = 0;
    cntRte = 0;
    cntOvl = 0;
    for (IGisItem* item : std::as_const(items)) {
      itemKeys << item->getKey();
      switch (item->type()) {
        case IGisItem::eTypeWpt:
          cntWpt++;
          break;

        case IGisItem::eTypeTrk:
          cntTrk++;
          break;

        case IGisItem::eTypeRte:
          cntRte++;
          break;

        case IGisItem::eTypeOvl:
          cntOvl++;
          break;
      }
    }

    posPoiHighlight.clear();
    poisFound.clear();
    if (modeSelection & IGisItem::eSelectionPoi) {
      canvas->findPoisIn(area, poisFound, posPoiHighlight);
    } else {
      poisFound = {};
    }
    cntPoi = poisFound.count();

    QString msg = tr("<b>Selected:</b><br/>");
    if (scrOptSelect->toolItemTrk->isChecked()) {
      msg += tr("%1 tracks<br/>").arg(cntTrk);
    }

    if (scrOptSelect->toolItemWpt->isChecked()) {
      msg += tr("%1 waypoints<br/>").arg(cntWpt);
    }

    if (scrOptSelect->toolItemRte->isChecked()) {
      msg += tr("%1 routes<br/>").arg(cntRte);
    }

    if (scrOptSelect->toolItemOvl->isChecked()) {
      msg += tr("%1 areas<br/>").arg(cntOvl);
    }

    if (scrOptSelect->toolItemPoi->isChecked()) {
      msg += tr("%1 POIs<br/>").arg(cntPoi);
    }
    canvas->reportStatus("CMouseSelect::Stat", msg);

    rectLastSel = rectSelection;
    modeLastSel = modeSelection;
  }

  scrOptSelect->frameFunction->setDisabled(items.isEmpty() && poisFound.isEmpty());
  scrOptSelect->toolDelete->setDisabled(items.isEmpty());
  scrOptSelect->toolSymWpt->setEnabled(cntWpt);
  scrOptSelect->toolRoute->setEnabled(cntWpt > 1);
  scrOptSelect->toolEditPrxWpt->setEnabled(cntWpt);
  scrOptSelect->toolCombineTrk->setEnabled(cntTrk > 1);
  scrOptSelect->toolActivityTrk->setEnabled(cntTrk > 0);
  scrOptSelect->toolColorTrk->setEnabled(cntTrk > 0);
  scrOptSelect->toolEleWptTrk->setEnabled((cntWpt > 0) | (cntTrk > 0));
}

void CMouseSelect::draw(QPainter& p, CCanvas::redraw_e needsRedraw, const QRect& rect) {
  if (rectSelection.isNull()) {
    return;
  }

  QList<IGisItem*> items;
  findItems(items);

  for (IGisItem* item : std::as_const(items)) {
    item->drawHighlight(p);
  }

  for (QPointF pos : std::as_const(posPoiHighlight)) {
    if (pos != NOPOINTF) {
      gis->convertRad2Px(pos);
      QRectF r(0, 0, 42, 42);
      r.moveCenter(pos);
      p.drawImage(r, QImage("://cursors/poiHighlightRed.png"));
    }
  }

  IMouseSelect::draw(p, needsRedraw, rect);
}

void CMouseSelect::slotUpdate() {
  rectLastSel = {};
  canvas->update();
}

void CMouseSelect::slotCopy() const {
  QMutexLocker lock(&mutex);

  // Project is a nullptr if the user cancels the Dialog
  IGisProject* project = CGisWorkspace::self().copyItemsByKey(itemKeys);
  if (project != nullptr) {
    CGisWorkspace::self().addPoisAsWpt(poisFound, project);
  }
  canvas->resetMouse();
}

void CMouseSelect::slotDelete() const {
  QMutexLocker lock(&mutex);
  CGisWorkspace::self().delItemsByKey(itemKeys);
  canvas->resetMouse();
}

void CMouseSelect::slotRoute() const {
  QMutexLocker lock(&mutex);
  CGisWorkspace::self().makeRteFromWpt(itemKeys);
  canvas->resetMouse();
}

void CMouseSelect::slotEditPrxWpt() const {
  QMutexLocker lock(&mutex);
  CGisWorkspace::self().editPrxWpt(itemKeys);
  canvas->resetMouse();
}

void CMouseSelect::slotCombineTrk() const {
  QMutexLocker lock(&mutex);
  CGisWorkspace::self().combineTrkByKey(itemKeys, itemKeys);
  canvas->resetMouse();
}

void CMouseSelect::slotActivityTrk() const {
  QMutexLocker lock(&mutex);
  CActivityTrk::getMenu(itemKeys, CMainWindow::self().getBestWidgetForParent(), true);
  canvas->resetMouse();
}

void CMouseSelect::slotColorTrk() const {
  QMutexLocker lock(&mutex);
  CGisWorkspace::self().colorTrkByKey(itemKeys);
  canvas->resetMouse();
}

void CMouseSelect::slotSymWpt() const {
  QMutexLocker lock(&mutex);
  QString iconName = CWptIconManager::self().selectWptIcon(CMainWindow::self().getBestWidgetForParent());
  if (iconName.isEmpty()) {
    return;
  }

  CGisWorkspace::self().changeWptSymByKey(itemKeys, iconName);
  canvas->resetMouse();
}

void CMouseSelect::slotEleWptTrk() const {
  QMutexLocker lock(&mutex);
  CGisWorkspace::self().addEleToWptTrkByKey(itemKeys);
  canvas->resetMouse();
}
