/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler <oliver.eichler@gmx.de>
    Copyright (C) 2017 Norbert Truchsess <norbert.truchsess@t-online.de>

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

#include "mouse/CMouseNormal.h"

#include <QtWidgets>

#include "canvas/CCanvas.h"
#include "gis/CGisDraw.h"
#include "gis/CGisWorkspace.h"
#include "gis/prj/IGisProject.h"
#include "gis/rte/CGisItemRte.h"
#include "gis/search/CGeoSearchWeb.h"
#include "gis/trk/CGisItemTrk.h"
#include "mouse/CMouseAdapter.h"
#include "mouse/CScrOptUnclutter.h"
#include "poi/IPoiFile.h"
#include "realtime/CRtWorkspace.h"
#include "widgets/CFadingIcon.h"

CMouseNormal::CMouseNormal(CGisDraw* gis, CCanvas* canvas, CMouseAdapter* mouse) : IMouse(gis, canvas, mouse) {
  cursor = QCursor(QPixmap(":/cursors/cursorMoveMap.png"), 0, 0);
  screenUnclutter = new CScrOptUnclutter(this);
}

CMouseNormal::~CMouseNormal() {}

void CMouseNormal::stopTracking() const {
  const IGisItem::key_t& key = CGisItemTrk::getKeyUserFocus();
  if (!key.item.isEmpty()) {
    CGisItemTrk* trk = dynamic_cast<CGisItemTrk*>(CGisWorkspace::self().getItemByKey(key));
    if (trk != nullptr) {
      trk->setMouseFocusByPoint(NOPOINT, CGisItemTrk::eFocusMouseMove, "CMouseNormal");
    }
  }
}

void CMouseNormal::unfocus() { resetState(); }

void CMouseNormal::rightButtonDown(const QPoint& point) {
  // right button cancels unclutter and item-options
  if (screenUnclutter->size() > 0 || !screenItemOption.isNull()) {
    resetState();
  }
  showContextMenu(point);
}

void CMouseNormal::longPress(const QPoint& point) {
  // longpress does not simulate right button, it just opens menu if
  // neiterh unclutter nor options are active
  if (screenUnclutter->size() == 0 && screenItemOption.isNull()) {
    showContextMenu(point);
  }
}

void CMouseNormal::mouseMoved(const QPoint& point) {
  screenUnclutter->mouseMove(point);
  if (!screenItemOption.isNull()) {
    screenItemOption->mouseMove(point);
  }

  switch (stateItemSel) {
    case eStateIdle:
      CGisWorkspace::self().mouseMove(point);
      CRtWorkspace::self().mouseMove(point);

    // break; skip break intentionally
    case eStateHooverSingle:
    case eStateHooverMultiple: {
      const IGisItem::key_t& keyTrk = CGisItemTrk::getKeyUserFocus();
      if (!keyTrk.item.isEmpty()) {
        CGisItemTrk* trk = dynamic_cast<CGisItemTrk*>(CGisWorkspace::self().getItemByKey(keyTrk));
        if (trk != nullptr) {
          trk->setMouseFocusByPoint(point, CGisItemTrk::eFocusMouseMove, "CMouseNormal");
        }
      }

      const IGisItem::key_t& keyRte = CGisItemRte::getKeyUserFocus();
      if (!keyRte.item.isEmpty()) {
        CGisItemRte* rte = dynamic_cast<CGisItemRte*>(CGisWorkspace::self().getItemByKey(keyRte));
        if (rte != nullptr) {
          rte->setMouseFocusByPoint(point, CGisItemRte::eFocusMouseMove, "CMouseNormal");
        }
      }
      break;
    }

    default:;
  }

  posPoiHighlight.clear();
  curPois.clear();
  canvas->findPoiCloseBy(point, curPois, posPoiHighlight);

  canvas->displayInfo(point);
  canvas->update();
}

void CMouseNormal::mouseDragged(const QPoint& start, const QPoint& last, const QPoint& end) {
  // start to block map moving when a previous click
  // has triggered a selection of any kind
  if (stateItemSel < eStateNoMapMovePossible) {
    IMouse::mouseDragged(start, last, end);
  } else {
    mouseMoved(end);
  }
}

void CMouseNormal::leftClicked(const QPoint& point) {
  switch (stateItemSel) {
    case eStateIdle: {
      CGisWorkspace::self().slotWksItemSelectionReset();
      break;
    }

    case eStateHooverSingle: {
      stateItemSel = eStateIdle;

      IGisItem* item = CGisWorkspace::self().getItemByKey(screenUnclutter->getItemKey());
      if (nullptr != item) {
        scrollToItem(item);
        if (setScreenOption(point, item)) {
          stateItemSel = eStateShowItemOptions;
        }
        stopTracking();
      }
      break;
    }

    case eStateHooverMultiple: {
      screenUnclutter->setOrigin(point);
      stateItemSel = eStateUnclutterMultiple;
      stopTracking();
      break;
    }

    case eStateUnclutterMultiple: {
      const CScrOptUnclutter::item_t* scrOpt = screenUnclutter->selectItem(point);
      if (scrOpt != nullptr) {
        IGisItem* item = CGisWorkspace::self().getItemByKey(scrOpt->key);
        screenUnclutter->clear();  // CAUTION!! this will delete the object scrOpt is pointing to.
        scrOpt = nullptr;
        if (item) {
          scrollToItem(item);
          if (setScreenOption(screenUnclutter->getOrigin(), item)) {
            stateItemSel = eStateShowItemOptions;
            break;
          }
        }
      }
      resetState();
      CGisWorkspace::self().slotWksItemSelectionReset();
      break;
    }

    case eStateShowItemOptions: {
      resetState();
      CGisWorkspace::self().slotWksItemSelectionReset();
      break;
    }
  }

  canvas->update();
}

void CMouseNormal::doubleClicked(const QPoint& point) {
  if (stateItemSel == eStateIdle) {
    const IGisItem::key_t& keyTrk = CGisItemTrk::getKeyUserFocus();
    CGisWorkspace::self().focusTrkByKey(false, keyTrk);

    const IGisItem::key_t& keyRte = CGisItemRte::getKeyUserFocus();
    CGisWorkspace::self().focusRteByKey(false, keyRte);
  }
}

void CMouseNormal::scaleChanged() { resetState(); }

void CMouseNormal::resetState() {
  screenUnclutter->clear();
  if (!screenItemOption.isNull() && screenItemOption->isNotLocked()) {
    screenItemOption->close();
  }
  stateItemSel = eStateIdle;
  canvas->update();
}

void CMouseNormal::scrollToItem(IGisItem* item) {
  QTreeWidget* treeWidget = item->treeWidget();
  // block signals as this is an internal
  // change and no user interaction with
  // the tree widget
  treeWidget->blockSignals(true);
  treeWidget->collapseAll();
  treeWidget->setCurrentItem(item);
  treeWidget->scrollToItem(item, QAbstractItemView::PositionAtCenter);
  treeWidget->blockSignals(false);
}

bool CMouseNormal::setScreenOption(const QPoint& pt, IGisItem* item) {
  CGisItemTrk* trk = dynamic_cast<CGisItemTrk*>(item);
  if (trk && trk->setMouseFocusByPoint(pt, CGisItemTrk::eFocusMouseClick, "CMouseNormal") == NOPOINTF) {
    new CFadingIcon(pt, "://icons/48x48/NotPossible.png", canvas);
    return false;
  }

  delete screenItemOption;
  screenItemOption = item->getScreenOptions(pt, this);

  return !screenItemOption.isNull();
}

void CMouseNormal::draw(QPainter& p, CCanvas::redraw_e needsRedraw, const QRect& rect) {
  // no mouse interaction while gis thread is running
  if (gis->isRunning()) {
    return;
  }

  switch (stateItemSel) {
    case eStateIdle:
    case eStateHooverSingle:
    case eStateHooverMultiple: {
      for (QPointF pos : posPoiHighlight) {
        gis->convertRad2Px(pos);
        QRectF r = IPoiFile::iconHighlight().rect();
        r.moveCenter(pos);
        p.drawImage(r, IPoiFile::iconHighlight());
      }

      /*
          Collect and draw items close to the last mouse position in the draw method.

          This might be a bit odd but there are two reasons:

          1) Multiple update events are combined by the event loop. Thus multiple mouse move
             events are reduced to a single paint event. As getItemsByPos() is quite cycle
             intense this seems like a good idea.

          2) The list of items passed back by getItemsByPos() must not be stored. That is why
             the list has to be generated within the draw handler to access the item's drawHighlight()
             method.

       */
      screenUnclutter->clear();

      QList<IGisItem*> items;
      CGisWorkspace::self().getItemsByPos(mouse->getPoint(), items);

      if (items.empty() || items.size() > 8) {
        stateItemSel = eStateIdle;
        break;
      }

      for (IGisItem* item : std::as_const(items)) {
        item->drawHighlight(p);
        screenUnclutter->addItem(item);
      }

      stateItemSel = (screenUnclutter->size() == 1) ? eStateHooverSingle : eStateHooverMultiple;
      break;
    }

    case eStateUnclutterMultiple: {
      screenUnclutter->draw(p);
      break;
    }

    case eStateShowItemOptions: {
      if (screenItemOption.isNull()) {
        stateItemSel = eStateIdle;
        break;
      }

      // the screen option might not be destroyed yet, but already hidden
      if (screenItemOption->isVisible()) {
        screenItemOption->draw(p);
      }
      break;
    }

    default:;
  }
}

void CMouseNormal::slotAddPoi(const IPoiItem& poi) const {
  CGisWorkspace::self().addPoiAsWpt(poi);
  canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawGis);
}

void CMouseNormal::slotAddWpt() const {
  QPointF pt = mouse->getPoint();
  gis->convertPx2Rad(pt);
  pt *= RAD_TO_DEG;

  CGisWorkspace::self().addWptByPos(pt);
  canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawGis);
}

void CMouseNormal::slotAddTrk() const {
  QPointF pt = mouse->getPoint();
  gis->convertPx2Rad(pt);

  canvas->setMouseEditTrk(pt);
  canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawGis);
}

void CMouseNormal::slotAddRte() const {
  QPointF pt = mouse->getPoint();
  gis->convertPx2Rad(pt);

  canvas->setMouseEditRte(pt);
  canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawGis);
}

void CMouseNormal::slotAddArea() const {
  QPointF pt = mouse->getPoint();
  gis->convertPx2Rad(pt);

  canvas->setMouseEditArea(pt);
  canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawGis);
}

void CMouseNormal::slotRuler() const {
  canvas->setMouseRuler();
  canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawGis);
}

void CMouseNormal::slotCopyPosition() const {
  QPointF pt = mouse->getPoint();
  gis->convertPx2Rad(pt);

  QString position;
  IUnit::degToStr(pt.x() * RAD_TO_DEG, pt.y() * RAD_TO_DEG, position);

  QClipboard* clipboard = QApplication::clipboard();
  clipboard->setText(position);
}

void CMouseNormal::slotCopyPositionGrid() const {
  QString position;
  QPointF pt = mouse->getPoint();
  gis->convertPx2Rad(pt);

  canvas->convertGridPos2Str(pt * RAD_TO_DEG, position, true);

  QClipboard* clipboard = QApplication::clipboard();
  clipboard->setText(position);
}

void CMouseNormal::slotSelectArea() const { canvas->setMouseSelect(); }

void CMouseNormal::showContextMenu(const QPoint& point) {
  QMenu menu(canvas);
  if (curPois.count() > 0 && curPois.count() <= 5) {
    for (IPoiItem poi : curPois) {
      menu.addAction(QIcon("://icons/32x32/AddWpt.png"), tr("Add POI %1 as Waypoint").arg(poi.name), this,
                     [this, poi] { slotAddPoi(poi); });
    }
  } else if (curPois.count() > 5) {
    menu.addAction(QIcon("://icons/32x32/AddWpt.png"), tr("Zoom in to add POIs as Waypoints"));
  }
  QPointF pt = mouse->getPoint();
  gis->convertPx2Rad(pt);

  menu.addAction(QIcon("://icons/32x32/AddWpt.png"), tr("Add Waypoint"), this, &CMouseNormal::slotAddWpt);
  menu.addAction(QIcon("://icons/32x32/AddTrk.png"), tr("Add Track"), this, &CMouseNormal::slotAddTrk);
  menu.addAction(QIcon("://icons/32x32/AddRte.png"), tr("Add Route"), this, &CMouseNormal::slotAddRte);
  menu.addAction(QIcon("://icons/32x32/AddArea.png"), tr("Add Area"), this, &CMouseNormal::slotAddArea);
  menu.addSeparator();
  menu.addAction(QIcon("://icons/32x32/CSrcDistance.png"), tr("Ruler"), this, &CMouseNormal::slotRuler);
  menu.addSeparator();
  menu.addAction(QIcon("://icons/32x32/SelectArea.png"), tr("Select Items On Map"), this,
                 &CMouseNormal::slotSelectArea);
  menu.addSeparator();
  menu.addMenu(CGeoSearchWeb::self().getMenu(pt * RAD_TO_DEG, &menu));
  menu.addAction(QIcon("://icons/32x32/CopyPosition.png"), tr("Copy position"), this, &CMouseNormal::slotCopyPosition);
  menu.addAction(QIcon("://icons/32x32/CopyGrid.png"), tr("Copy position (Grid)"), this,
                 &CMouseNormal::slotCopyPositionGrid);

  QPoint p = canvas->mapToGlobal(point);
  menu.exec(p);
}
