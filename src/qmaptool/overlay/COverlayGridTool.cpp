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

#include "overlay/COverlayGridTool.h"

#include <QtWidgets>

#include "CMainWindow.h"
#include "canvas/IDrawContext.h"
#include "gis/proj_x.h"
#include "helpers/CSettings.h"
#include "items/CItemRefMap.h"
#include "overlay/refmap/COverlayRefMapPoint.h"
#include "overlay/refmap/CProjWizard.h"

COverlayGridTool::COverlayGridTool(QWidget* parent) : QWidget(parent) {
  setupUi(this);
  labelFinal->setText(
      tr("Before you proceed with 'ok':\n"
         "Please cross check all data once again. A bad reference coordinate will ruin "
         "all the work. Also cross check if the selected area contains as many reference "
         "points as possible at the border. You can easily delete points outside the map "
         "in the Reference Tool. But it's much more effort to set additional points in "
         "case you miss some.  When you are done press 'ok' to transfer the derived "
         "reference points to the Reference Tool.\n\n"
         "The next step will be to use the Reference Tool to adjust the position of all "
         "reference points to the real grid position on the map."));

  connect(CMainWindow::self().showToolHelp(), &QAction::toggled, labelFinal, &QLabel::setVisible);

  QButtonGroup* group = new QButtonGroup(this);
  group->addButton(radioSetRef);
  group->addButton(radioGridPlacer);
  group->addButton(radioSelectArea);

  connect(radioSetRef, &QRadioButton::toggled, widgetSetRef, &QFrame::setEnabled);
  connect(radioGridPlacer, &QRadioButton::toggled, widgetGridPlacer, &CGridPlacer::setEnabled);
  connect(radioSelectArea, &QRadioButton::toggled, widgetSelectArea, &QLabel::setEnabled);

  connect(widgetSetRef, &CGridSetRef::sigChanged, this, &COverlayGridTool::slotCalculate);
  connect(widgetSetRef, &CGridSetRef::sigChanged, this, &COverlayGridTool::slotCheckInput);

  connect(widgetGridPlacer, &CGridPlacer::sigChanged, this, &COverlayGridTool::slotCheckInput);
  connect(widgetGridPlacer, &CGridPlacer::sigSetArea, this, &COverlayGridTool::slotSetArea);
  connect(widgetGridPlacer, &CGridPlacer::sigChanged, this, &COverlayGridTool::slotCalculate);

  connect(widgetSelectArea, &CGridSelArea::sigChanged, this, &COverlayGridTool::slotCalculate);
  connect(widgetSelectArea, &CGridSelArea::sigChanged, this, &COverlayGridTool::slotCheckInput);
}

void COverlayGridTool::slotReset() {
  widgetSetRef->slotReset();
  widgetGridPlacer->slotReset();
  widgetSelectArea->slotReset();

  radioSetRef->setChecked(true);

  SETTINGS;
  cfg.beginGroup("GridTool");
  cfg.remove("");
  cfg.endGroup();
  cfg.sync();
}

void COverlayGridTool::registerItem(CItemRefMap* item) {
  this->item = item;
  if (item != nullptr) {
    context = item->getDrawContext();
    if (context == nullptr) {
      this->item = nullptr;
    }
  } else {
    context = nullptr;
  }

  widgetGridPlacer->registerItem(this->item);
  widgetSelectArea->registerItem(this->item);

  radioSetRef->setChecked(true);

  SETTINGS;
  cfg.beginGroup("GridTool");
  if (this->item != nullptr) {
    widgetSetRef->loadSettings(cfg);
    widgetGridPlacer->loadSettings(cfg);
    widgetSelectArea->loadSettings(cfg);
  } else {
    cfg.remove("");

    widgetSetRef->saveSettings(cfg);
    widgetGridPlacer->saveSettings(cfg);
    widgetSelectArea->saveSettings(cfg);
  }
  cfg.endGroup();

  slotCheckInput();
}

void COverlayGridTool::slotCheckInput() {
  bool group1Ok = widgetSetRef->isOk();
  radioGridPlacer->setEnabled(group1Ok);

  bool group2Ok = widgetGridPlacer->isOk();
  radioSelectArea->setEnabled(group2Ok && group1Ok);

  const QPointF& p1 = widgetGridPlacer->getPoint(0);
  const QPointF& p2 = widgetGridPlacer->getPoint(1);
  const QPointF& p3 = widgetGridPlacer->getPoint(2);
  const QPointF& p4 = widgetGridPlacer->getPoint(3);
  const QRectF& area = widgetSelectArea->getArea();

  bool group3Ok = true;
  group3Ok &= !area.isEmpty();
  group3Ok &= area.isValid();
  group3Ok &= !area.isNull();
  group3Ok &= area.contains(p1);
  group3Ok &= area.contains(p2);
  group3Ok &= area.contains(p3);
  group3Ok &= area.contains(p4);

  labelFinal->setEnabled(group1Ok && group2Ok && group3Ok);

  emit sigChanged(group1Ok && group2Ok && group3Ok);
}

bool COverlayGridTool::drawFx(QPainter& p, CCanvas::redraw_e needsRedraw) {
  if (radioSelectArea->isEnabled()) {
    widgetSelectArea->drawFx(p, needsRedraw);

    QRectF dot1(0, 0, 7, 7);
    p.setPen(QPen(Qt::white, 1));
    p.setBrush(Qt::darkGreen);

    for (COverlayRefMapPoint* point : std::as_const(refPoints)) {
      QPointF pt = point->getPtPtx();
      context->convertMap2Screen(pt);
      dot1.moveCenter(pt);
      p.drawRect(dot1);
    }
  }

  widgetGridPlacer->drawFx(p, needsRedraw);
  return false;
}

void COverlayGridTool::mouseMoveEventFx(QMouseEvent* e) {
  if (radioGridPlacer->isChecked()) {
    widgetGridPlacer->mouseMoveEventFx(e);
  } else if (radioSelectArea->isChecked()) {
    widgetSelectArea->mouseMoveEventFx(e);
  }
}

void COverlayGridTool::mouseReleaseEventFx(QMouseEvent* e) {
  if (radioGridPlacer->isChecked()) {
    widgetGridPlacer->mouseReleaseEventFx(e);
  } else if (radioSelectArea->isChecked()) {
    widgetSelectArea->mouseReleaseEventFx(e);
  }
}

void COverlayGridTool::leaveEventFx(QEvent* e) {
  if (radioGridPlacer->isChecked()) {
    widgetGridPlacer->leaveEventFx(e);
  } else if (radioSelectArea->isChecked()) {
    widgetSelectArea->leaveEventFx(e);
  }
}

QCursor COverlayGridTool::getCursorFx() {
  if (radioGridPlacer->isChecked()) {
    return widgetGridPlacer->getCursorFx();
  } else if (radioSelectArea->isChecked()) {
    return widgetSelectArea->getCursorFx();
  } else {
    return Qt::ArrowCursor;
  }
}

void COverlayGridTool::slotSetArea(const QRectF& rect) {
  qreal hspace = rect.width() / 2;
  qreal vspace = rect.height() / 2;
  QRectF area = rect;

  area.setTopLeft(rect.topLeft() - QPointF(hspace, vspace));
  area.setBottomRight(rect.bottomRight() + QPointF(hspace, vspace));

  widgetSelectArea->slotSetArea(area);
  radioSelectArea->setChecked(true);
}

void COverlayGridTool::slotCalculate() {
  if (!radioSelectArea->isEnabled()) {
    return;
  }

  qDeleteAll(refPoints);
  refPoints.clear();

  CProj proj;
  proj.init(widgetSetRef->getProjection().toLatin1(), "EPSG:4326");
  if (!proj.isValid()) {
    return;
  }

  const QRectF& area = widgetSelectArea->getArea();

  const QPointF& ptTopLeft = widgetGridPlacer->getPoint(0);
  const QPointF& ptTopRight = widgetGridPlacer->getPoint(1);
  const QPointF& ptBottomRight = widgetGridPlacer->getPoint(2);
  const QPointF& ptBottomLeft = widgetGridPlacer->getPoint(3);

  qreal dx11 = ptTopRight.x() - ptTopLeft.x();
  qreal dy11 = ptTopRight.y() - ptTopLeft.y();
  qreal dx12 = ptBottomRight.x() - ptBottomLeft.x();
  qreal dy12 = ptBottomRight.y() - ptBottomLeft.y();
  qreal dx1 = (dx11 + dx12) / 2;
  qreal dy1 = (dy11 + dy12) / 2;

  qreal alpha = qAtan(dy1 / dx1);
  qreal distx = qSqrt(dx1 * dx1 + dy1 * dy1);

  qreal dx21 = ptBottomLeft.x() - ptTopLeft.x();
  qreal dy21 = ptBottomLeft.y() - ptTopLeft.y();
  qreal dx22 = ptBottomRight.x() - ptTopRight.x();
  qreal dy22 = ptBottomRight.y() - ptTopRight.y();
  qreal dx2 = (dx21 + dx22) / 2;
  qreal dy2 = (dy21 + dy22) / 2;

  qreal disty = qSqrt(dx2 * dx2 + dy2 * dy2);

  QMatrix translationMatrix(1, 0, 0, 1, ptTopLeft.x(), ptTopLeft.y());
  QMatrix rotationMatrix(qCos(alpha), qSin(alpha), -qSin(alpha), qCos(alpha), 0, 0);
  QMatrix scalingMatrix(distx, 0, 0, disty, 0, 0);

  // forward matrix index -> map pixel coord
  QMatrix mxFwd = scalingMatrix * rotationMatrix * translationMatrix;
  // backward matrix map pixel coord -> index
  QMatrix mxBwd = mxFwd.inverted();

  QPointF tl = mxBwd.map(area.topLeft());
  QPointF br = mxBwd.map(area.bottomRight());

  int xMin = qCeil(tl.x()) - 1;
  int yMin = qCeil(tl.y()) - 1;

  int xMax = qCeil(br.x()) + 1;
  int yMax = qCeil(br.y()) + 1;

  qreal lonRef = widgetSetRef->getEasting();
  qreal latRef = widgetSetRef->getNorthing();
  qreal dLon = widgetSetRef->getHorizSpacing();
  qreal dLat = widgetSetRef->getVertSpacing();

  bool isLonLat = proj.isSrcLatLong();

  for (int y = yMin; y < yMax; y++) {
    for (int x = xMin; x < xMax; x++) {
      QPointF ptPtx = mxFwd.map(QPointF(x, y));
      if (area.contains(ptPtx)) {
        ptPtx.rx() = qRound(ptPtx.x());
        ptPtx.ry() = qRound(ptPtx.y());

        qreal lat = latRef - y * dLat;
        qreal lon = lonRef + x * dLon;

        if (isLonLat) {
          lon *= DEG_TO_RAD;
          lat *= DEG_TO_RAD;
        }

        proj.transform(lon, lat, PJ_FWD);
        lon *= RAD_TO_DEG;
        lat *= RAD_TO_DEG;

        refPoints << new COverlayRefMapPoint(0, QPointF(lon, lat), ptPtx, nullptr);
      }
    }
  }
}
