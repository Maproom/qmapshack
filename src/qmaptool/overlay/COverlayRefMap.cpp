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

#include "overlay/COverlayRefMap.h"

#include <QtWidgets>
#include <functional>

#include "canvas/IDrawContext.h"
#include "helpers/CDraw.h"
#include "helpers/CSettings.h"
#include "items/CItemRefMap.h"
#include "overlay/refmap/CDialogRefPoint.h"
#include "overlay/refmap/COverlayRefMapPoint.h"
#include "overlay/refmap/CProjWizard.h"
using std::bind;

COverlayRefMap::COverlayRefMap(CItemRefMap* item, QStackedWidget* stackedWidget)
    : IOverlay(stackedWidget), context(item->getDrawContext()), item(item) {
  setupUi(this);

  QFileInfo fi(item->getFilename());
  gcpFilename = fi.completeBaseName() + ".gcp";

  labelHelp->setText(
      tr("If you used the Grid Tool you have to fine tune the reference points by placing them "
         "as much as possible on the grid crossing. Be aware that if you over scale you get "
         "jumping points by rounding effects. Be precise but do not make religion out "
         "of the task.\nIf your mouse focus is on the map you can use the N and B keys to "
         "jump forward an backward in the reference point list. "
         "\nThe is also the option to fine tune the reference points in auto-mode. In this "
         "mode the next reference point is selected automatically right after you placed the "
         "current one. This is very convenient for a large number of reference points."));

  labelHelp->setVisible(CMainWindow::self().showToolHelp()->isChecked());
  connect(CMainWindow::self().showToolHelp(), &QAction::toggled, labelHelp, &QLabel::setVisible);

  treeWidget->addAction(actionDelRefPoint);

  connect(toolNone, &QToolButton::clicked, this,
          bind(&COverlayRefMap::slotSetMode, this, eModeNone, std::placeholders::_1));
  connect(toolRefMove, &QToolButton::clicked, this,
          bind(&COverlayRefMap::slotSetMode, this, eModePointMove, std::placeholders::_1));
  connect(toolRefAdd, &QToolButton::clicked, this,
          bind(&COverlayRefMap::slotSetMode, this, eModePointAdd, std::placeholders::_1));
  connect(toolRefDel, &QToolButton::clicked, this,
          bind(&COverlayRefMap::slotSetMode, this, eModePointDel, std::placeholders::_1));
  connect(toolRefMoveAuto, &QToolButton::clicked, this,
          bind(&COverlayRefMap::slotSetMode, this, eModePointMoveAuto, std::placeholders::_1));
  connect(toolRefDelAll, &QToolButton::clicked, this, &COverlayRefMap::slotResetRef);
  connect(toolLoadGcp, &QToolButton::clicked, this, &COverlayRefMap::slotLoadGcp);
  connect(toolSaveGcp, &QToolButton::clicked, this, &COverlayRefMap::slotSaveGcp);
  connect(toolProjection, &QToolButton::clicked, this, &COverlayRefMap::slotProjWizard);
  connect(toolGridTool, &QToolButton::clicked, this, &COverlayRefMap::slotGridTool);
  connect(toolSort, &QToolButton::clicked, this, &COverlayRefMap::slotSortRefPoints);
  connect(treeWidget, &QTreeWidget::itemSelectionChanged, this, &COverlayRefMap::slotSelectionChanged);
  connect(actionDelRefPoint, &QAction::triggered, this, &COverlayRefMap::slotDelRefPoints);
}

void COverlayRefMap::saveSettings(QSettings& cfg) {
  QByteArray buffer;
  QDataStream stream(&buffer, QIODevice::WriteOnly);
  stream.setByteOrder(QDataStream::LittleEndian);
  stream.setVersion(QDataStream::Qt_5_4);

  const int N = treeWidget->topLevelItemCount();
  stream << N;
  for (int n = 0; n < N; n++) {
    COverlayRefMapPoint* item = dynamic_cast<COverlayRefMapPoint*>(treeWidget->topLevelItem(n));
    if (item != nullptr) {
      stream << item->getPtPtx() << item->getPtRef();
    }
  }

  cfg.beginGroup("grid");
  cfg.setValue("points", buffer);
  cfg.setValue("projection", lineProjection->text());
  cfg.endGroup();
}

void COverlayRefMap::loadSettings(QSettings& cfg) {
  QByteArray buffer;
  cfg.beginGroup("grid");
  buffer = cfg.value("points", buffer).toByteArray();
  lineProjection->setText(cfg.value("projection", "").toString());
  lineProjection->setCursorPosition(0);
  cfg.endGroup();

  QDataStream stream(&buffer, QIODevice::ReadOnly);
  stream.setByteOrder(QDataStream::LittleEndian);
  stream.setVersion(QDataStream::Qt_5_4);

  int N;
  stream >> N;
  for (int n = 0; n < N; n++) {
    QPointF ptPtx;
    QPointF ptRef;
    stream >> ptPtx >> ptRef;
    new COverlayRefMapPoint(n + 1, ptRef, ptPtx, treeWidget);
  }

  updateGui();
  slotSortRefPoints();
  emit sigChanged();
}

void COverlayRefMap::slotSortRefPoints() {
  treeWidget->sortItems(0, Qt::AscendingOrder);
  const int N = treeWidget->topLevelItemCount();
  for (int n = 0; n < N; n++) {
    COverlayRefMapPoint* point = dynamic_cast<COverlayRefMapPoint*>(treeWidget->topLevelItem(n));
    if (point == nullptr) {
      continue;
    }

    point->setIndex(n);
  }

  treeWidget->header()->resizeSections(QHeaderView::ResizeToContents);
}

void COverlayRefMap::addRefPoints(QList<COverlayRefMapPoint*>& points) {
  // it's faster to use a bulk add
  QList<QTreeWidgetItem*> items;
  for (QTreeWidgetItem* item : points) {
    items << item;
  }
  treeWidget->addTopLevelItems(items);

  // as the tree widget owns the items now, the list has to be cleared
  points.clear();
  // reflect changes on the GUI
  updateGui();

  emit sigChanged();
}

QString COverlayRefMap::getMapProjection() const { return lineProjection->text(); }

const QList<COverlayRefMapPoint*> COverlayRefMap::getRefPoints() const {
  QList<COverlayRefMapPoint*> points;

  const int N = treeWidget->topLevelItemCount();
  for (int n = 0; n < N; n++) {
    COverlayRefMapPoint* item = dynamic_cast<COverlayRefMapPoint*>(treeWidget->topLevelItem(n));
    if (item != nullptr) {
      points << item;
    }
  }

  return points;
}

bool COverlayRefMap::drawFx(QPainter& p, CCanvas::redraw_e needsRedraw) {
  QRectF dot1(0, 0, 5, 5);
  const int N = treeWidget->topLevelItemCount();
  for (int n = 0; n < N; n++) {
    COverlayRefMapPoint* item = dynamic_cast<COverlayRefMapPoint*>(treeWidget->topLevelItem(n));
    if (item != nullptr) {
      if (item->isSelected()) {
        p.setPen(QPen(Qt::red, 1));
        p.setBrush(Qt::red);
      } else {
        p.setPen(QPen(Qt::black, 1));
        p.setBrush(Qt::black);
      }

      QPointF pt = item->getPtPtx();
      if ((pt - ptFocus1).manhattanLength() > 30) {
        context->convertMap2Screen(pt);
        dot1.moveCenter(pt);
        p.drawRect(dot1);
      }
    }
  }

  QPointF pt = NOPOINTF;
  if (ptFocus2 != NOPOINTF) {
    pt = ptFocus2;
  } else if (ptFocus1 != NOPOINTF) {
    pt = ptFocus1;
  }

  if (pt != NOPOINTF) {
    context->convertMap2Screen(pt);

    if (movePoint) {
      QPointF pt1 = ptFocus1;
      context->convertMap2Screen(pt1);

      p.setPen(QPen(Qt::red, 1));
      p.setBrush(Qt::red);
      p.drawLine(pt1, pt);
      dot1.moveCenter(pt1);
      p.drawRect(dot1);
    }

    CDraw::drawCrossHairDot(p, pt);
  }

  return true;
}

QPointF COverlayRefMap::isCloseTo(QPointF pt) {
  qint32 min = 30;
  COverlayRefMapPoint* selItem = nullptr;

  context->convertMap2Screen(pt);

  const int N = treeWidget->topLevelItemCount();
  for (int n = 0; n < N; n++) {
    COverlayRefMapPoint* item = dynamic_cast<COverlayRefMapPoint*>(treeWidget->topLevelItem(n));
    if (item == nullptr) {
      continue;
    }

    QPointF ptx = item->getPtPtx();
    context->convertMap2Screen(ptx);

    qint32 d = (pt - ptx).manhattanLength();
    if (d < min) {
      min = d;
      selItem = item;
    }
  }

  if (selItem != nullptr) {
    treeWidget->setCurrentItem(selItem);
    return selItem->getPtPtx();
  } else {
    return NOPOINTF;
  }
}

void COverlayRefMap::mouseMoveEventFx(QMouseEvent* e) {
  QPointF pt = e->pos();
  context->convertScreen2Map(pt);

  switch (mode) {
    case eModePointMove:
    case eModePointMoveAuto:
      mouseMovePointMove(pt);
      break;

    case eModePointAdd:
      mouseMovePointAdd(pt);
      break;

    case eModePointDel:
      mouseMovePointDel(pt);
      break;
  }

  updateGui();
  context->triggerCompleteUpdate(CCanvas::eRedrawOverlay);
}

void COverlayRefMap::mouseReleaseEventFx(QMouseEvent* e) {
  QPointF pt = e->pos();
  context->convertScreen2Map(pt);

  Qt::MouseButton button = e->button();

  switch (mode) {
    case eModePointMove:
    case eModePointMoveAuto:
      mouseReleasePointMove(pt, button);
      break;

    case eModePointAdd:
      mouseReleasePointAdd(pt, button);
      break;

    case eModePointDel:
      mouseReleasePointDel(pt, button);
      break;
  }

  updateGui();
  context->triggerCompleteUpdate(CCanvas::eRedrawOverlay);
}

void COverlayRefMap::mouseMovePointAdd(const QPointF& pt) { ptFocus1 = pt; }

void COverlayRefMap::mouseMovePointDel(const QPointF& pt) { ptFocus1 = isCloseTo(pt); }

void COverlayRefMap::mouseMovePointMove(const QPointF& pt) {
  if (movePoint) {
    ptFocus2 = pt;
  } else {
    ptFocus1 = isCloseTo(pt);
  }
}

void COverlayRefMap::mouseReleasePointAdd(const QPointF& pt, Qt::MouseButton button) {
  if (button == Qt::LeftButton) {
    QPointF ptPtx = pt;
    QPointF ptRef = NOPOINTF;
    // ask for coordinate
    CDialogRefPoint dlg(ptPtx, ptRef, this);
    int res = dlg.exec();
    if (res == QDialog::Accepted) {
      new COverlayRefMapPoint(treeWidget->topLevelItemCount() + 1, ptRef, ptPtx, treeWidget);
      emit sigChanged();
    }
  } else if (button == Qt::RightButton) {
    abortStep();
    toolNone->click();
    CCanvas::restoreOverrideCursor("CRefMapGrid::mouseReleasePointAdd");
  }
}

void COverlayRefMap::mouseReleasePointDel(const QPointF& pt, Qt::MouseButton button) {
  if (button == Qt::LeftButton) {
    if (ptFocus1 == NOPOINTF) {
      return;
    }

    const int N = treeWidget->topLevelItemCount();
    for (int n = 0; n < N; n++) {
      COverlayRefMapPoint* item = dynamic_cast<COverlayRefMapPoint*>(treeWidget->topLevelItem(n));
      if (item == nullptr) {
        continue;
      }

      if (item->getPtPtx() == ptFocus1) {
        delete item;
        abortStep();
        emit sigChanged();
        return;
      }
    }
  } else if (button == Qt::RightButton) {
    CCanvas::restoreOverrideCursor("CRefMapGrid::mouseReleasePointDel");
    toolNone->click();
    abortStep();
  }
}

void COverlayRefMap::mouseReleasePointMove(const QPointF& pt, Qt::MouseButton button) {
  if (button == Qt::LeftButton) {
    if (movePoint) {
      const int N = treeWidget->topLevelItemCount();
      for (int n = 0; n < N; n++) {
        COverlayRefMapPoint* item = dynamic_cast<COverlayRefMapPoint*>(treeWidget->topLevelItem(n));
        if (item == nullptr) {
          continue;
        }

        if (item->getPtPtx() == ptFocus1) {
          item->setPtPtx(pt);
          break;
        }
      }

      switch (mode) {
        case eModePointMove: {
          abortStep();
          CCanvas::restoreOverrideCursor("CRefMapGrid::mouseReleasePointMove");
          break;
        }

        case eModePointMoveAuto: {
          int idx = treeWidget->indexOfTopLevelItem(treeWidget->currentItem()) + 1;
          if (idx >= treeWidget->topLevelItemCount()) {
            abortStep();
            CCanvas::restoreOverrideCursor("CRefMapGrid::mouseReleasePointMove");
          } else {
            COverlayRefMapPoint* point = dynamic_cast<COverlayRefMapPoint*>(treeWidget->topLevelItem(idx));
            if (point == nullptr) {
              return;
            }
            treeWidget->setCurrentItem(point);
            treeWidget->scrollToItem(point);

            QPointF pt1 = point->getPtPtx();
            ptFocus1 = pt1;
            ptFocus2 = pt;
            context->convertMap2Screen(pt1);

            QPointF pt2 = context->getCanvas()->rect().center();
            context->move(pt2 - pt1);
            context->triggerCompleteUpdate(CCanvas::eRedrawAll);
          }
          break;
        }
      }
    } else {
      if (ptFocus1 != NOPOINTF) {
        ptFocus2 = pt;
        movePoint = true;
        CCanvas::setOverrideCursor(Qt::BlankCursor, "CRefMapGrid::mouseReleasePointMove");
      }
    }
  } else if (button == Qt::RightButton) {
    if (!movePoint) {
      toolNone->click();
    }
    abortStep();
    CCanvas::restoreOverrideCursor("CRefMapGrid::mouseReleasePointMove");
  }
}

bool COverlayRefMap::keyPressEventFx(QKeyEvent* e) {
  QTreeWidgetItem* item = treeWidget->currentItem();
  if (item == nullptr) {
    return false;
  }

  int idx = treeWidget->indexOfTopLevelItem(item);

  switch (e->key()) {
    case Qt::Key_N: {
      ++idx;
      idx = qMin(idx, treeWidget->topLevelItemCount() - 1);
      break;
    }

    case Qt::Key_B: {
      --idx;
      idx = qMax(idx, 0);
      break;
    }

    default:
      return false;
  }

  if (movePoint) {
    switch (mode) {
      case eModePointMove:
        return false;

      case eModePointMoveAuto:
        abortStep();
        CCanvas::restoreOverrideCursor("CRefMapGrid::mouseReleasePointMove");
        break;
    }
  }

  COverlayRefMapPoint* point = dynamic_cast<COverlayRefMapPoint*>(treeWidget->topLevelItem(idx));
  if (point == nullptr) {
    return false;
  }
  treeWidget->setCurrentItem(point);
  treeWidget->scrollToItem(point);

  QPointF pt1 = point->getPtPtx();
  QPointF pt2 = context->getCanvas()->rect().center();
  context->convertMap2Screen(pt1);
  context->move(pt2 - pt1);

  context->triggerCompleteUpdate(CCanvas::eRedrawAll);

  return true;
}

QCursor COverlayRefMap::getCursorFx() {
  switch (mode) {
    case COverlayRefMap::eModePointAdd:
      return Qt::BlankCursor;

    case COverlayRefMap::eModePointDel:
      return QCursor(QPixmap(":/cursors/cursorPointDel.png"), 0, 0);

    case COverlayRefMap::eModePointMoveAuto:
    case COverlayRefMap::eModePointMove:
      return QCursor(QPixmap(":/cursors/cursorPointMove.png"), 0, 0);
  }

  return Qt::ArrowCursor;
}

void COverlayRefMap::updateGui() {
  bool isEmpty = treeWidget->topLevelItemCount() == 0;
  toolRefDel->setDisabled(isEmpty);
  toolRefMove->setDisabled(isEmpty);
  toolRefDelAll->setDisabled(isEmpty);
  toolRefMoveAuto->setDisabled(isEmpty);
  toolSaveGcp->setDisabled(isEmpty);
}

void COverlayRefMap::abortStep() {
  movePoint = false;
  ptFocus1 = NOPOINTF;
  ptFocus2 = NOPOINTF;

  context->triggerCompleteUpdate(CCanvas::eRedrawOverlay);
}

void COverlayRefMap::slotSetMode(mode_e m, bool on) {
  if (on) {
    mode = m;
  }
}

void COverlayRefMap::slotSaveGcp() {
  SETTINGS;
  QString path = cfg.value("Path/gcpInput", QDir::homePath()).toString();

  QString filename =
      QFileDialog::getSaveFileName(0, tr("Save reference points..."), path + "/" + gcpFilename, "Ref. points (*.gcp)");
  if (filename.isEmpty()) {
    return;
  }
  cfg.setValue("Path/gcpInput", QFileInfo(filename).absolutePath());

  gcpFilename = filename;

  QFile file(filename);
  file.open(QIODevice::WriteOnly);
  QTextStream out(&file);
  out.setRealNumberPrecision(10);

  out << "#V1.0" << Qt::endl;
  out << "#gcpproj: +proj=longlat +ellps=WGS84 +datum=WGS84 +no_defs" << Qt::endl;

  const int N = treeWidget->topLevelItemCount();
  for (int n = 0; n < N; n++) {
    COverlayRefMapPoint* pt = dynamic_cast<COverlayRefMapPoint*>(treeWidget->topLevelItem(n));
    if (pt == nullptr) {
      continue;
    }
    const QPointF& ptx = pt->getPtPtx();
    const QPointF& ref = pt->getPtRef();

    out << "-gcp " << ptx.x() << " " << ptx.y() << " " << ref.y() << " " << ref.x() << Qt::endl;
  }

  file.close();
}

void COverlayRefMap::slotLoadGcp() {
  SETTINGS;
  QString path = cfg.value("Path/gcpInput", QDir::homePath()).toString();

  QString filename = QFileDialog::getOpenFileName(0, tr("Load reference points..."), path, "Ref. points (*.gcp)");
  if (filename.isEmpty()) {
    return;
  }
  cfg.setValue("Path/gcpInput", QFileInfo(filename).absolutePath());

  QFile file(filename);
  file.open(QIODevice::ReadOnly);
  QString line = file.readLine();
  if (line.trimmed() == "#V1.0") {
    static const QRegularExpression re1(
        "^-gcp\\s(-{0,1}[0-9]+)\\s(-{0,1}[0-9]+)\\s(-{0,1}[0-9\\.]+)\\s(-{0,1}[0-9\\.]+).*$");

    qint32 cnt = 1;
    while (1) {
      const QRegularExpressionMatch& match = re1.match(line);
      if (match.hasMatch()) {
        QPointF ptPtx(match.captured(1).toDouble(), match.captured(2).toDouble());
        QPointF ptRef(match.captured(4).toDouble(), match.captured(3).toDouble());
        new COverlayRefMapPoint(cnt++, ptRef, ptPtx, treeWidget);
      }

      if (file.atEnd()) {
        break;
      }
      line = file.readLine();
    }
  }

  updateGui();

  emit sigChanged();
}

void COverlayRefMap::slotResetRef() {
  int res = QMessageBox::question(this, tr("Delete all reference points..."),
                                  tr("Are you sure to delete all reference points in the list?"),
                                  QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
  if (res != QMessageBox::Yes) {
    return;
  }
  treeWidget->clear();
  updateGui();

  emit sigChanged();
}

void COverlayRefMap::slotProjWizard() {
  CProjWizard dlg(*lineProjection, this);
  dlg.exec();
  lineProjection->setCursorPosition(0);

  emit sigChanged();
}

void COverlayRefMap::slotGridTool() { CMainWindow::self().startGridTool(item); }

void COverlayRefMap::slotSelectionChanged() { context->triggerCompleteUpdate(CCanvas::eRedrawOverlay); }

void COverlayRefMap::slotDelRefPoints() {
  const QList<QTreeWidgetItem*> items = treeWidget->selectedItems();
  if (items.count() > 1) {
    int res = QMessageBox::question(this, tr("Delete..."), tr("Delete all selected reference points?"),
                                    QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    if (res != QMessageBox::Yes) {
      return;
    }
  }

  qDeleteAll(items);

  emit sigChanged();
}

bool COverlayRefMap::isOk() const {
  bool ok = true;
  ok &= (treeWidget->topLevelItemCount() > 2);
  ok &= CProjWizard::validProjStr(lineProjection->text());
  return ok;
}
