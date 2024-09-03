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

#include "overlay/gridtool/CGridPlacer.h"

#include <QtWidgets>
#include <functional>

#include "canvas/IDrawContext.h"
#include "items/CItemRefMap.h"
#include "overlay/gridtool/CGridPoint.h"
using std::bind;

CGridPlacer::CGridPlacer(QWidget* parent) : QWidget(parent) {
  setupUi(this);

  labelHelp->setText(
      tr("Select one of the corners and place the marker at "
         "the corresponding grid crossing on the map. All "
         "4 corners have to be placed."));

  QButtonGroup* group = new QButtonGroup(this);
  group->addButton(radioPoint1);
  group->addButton(radioPoint2);
  group->addButton(radioPoint3);
  group->addButton(radioPoint4);

  connect(radioPoint1, &QToolButton::toggled, this, bind(&CGridPlacer::slotSetPoint, this, 0, std::placeholders::_1));
  connect(radioPoint2, &QToolButton::toggled, this, bind(&CGridPlacer::slotSetPoint, this, 1, std::placeholders::_1));
  connect(radioPoint3, &QToolButton::toggled, this, bind(&CGridPlacer::slotSetPoint, this, 2, std::placeholders::_1));
  connect(radioPoint4, &QToolButton::toggled, this, bind(&CGridPlacer::slotSetPoint, this, 3, std::placeholders::_1));
  connect(pushReset, &QPushButton::clicked, this, &CGridPlacer::slotReset);
  connect(pushSetArea, &QPushButton::clicked, this, &CGridPlacer::slotSetArea);
}

void CGridPlacer::registerItem(CItemRefMap* item) {
  this->item = item;

  if (item != nullptr) {
    points = QVector<CGridPoint>(4);

    for (CGridPoint& point : points) {
      point.registerItem(item);
    }

    radioPoint1->setChecked(true);
  }
}

void CGridPlacer::saveSettings(QSettings& cfg) {
  cfg.beginGroup("Points");
  for (int i = 0; i < points.size(); i++) {
    const QPointF& pt = points[i].getPoint();
    if (pt != NOPOINTF) {
      cfg.setValue(QString::number(i), pt);
    }
  }
  cfg.endGroup();
}

void CGridPlacer::loadSettings(QSettings& cfg) {
  cfg.beginGroup("Points");
  for (int i = 0; i < points.size(); i++) {
    QPointF pt = cfg.value(QString::number(i), NOPOINTF).toPointF();
    points[i].setPoint(pt);
  }
  cfg.endGroup();
  updateStatus();
}

bool CGridPlacer::drawFx(QPainter& p, CCanvas::redraw_e needsRedraw) {
  for (CGridPoint& point : points) {
    point.drawFx(p, needsRedraw);
  }

  return true;
}

void CGridPlacer::mouseMoveEventFx(QMouseEvent* e) { points[idx].mouseMoveEventFx(e); }

void CGridPlacer::mouseReleaseEventFx(QMouseEvent* e) {
  points[idx].mouseReleaseEventFx(e);
  updateStatus();
}

void CGridPlacer::leaveEventFx(QEvent* e) { points[idx].leaveEventFx(e); }

QCursor CGridPlacer::getCursorFx() { return points[idx].getCursorFx(); }

void CGridPlacer::slotSetPoint(qint32 i, bool on) {
  if (on) {
    idx = i;
  }
}

void CGridPlacer::slotReset() {
  for (CGridPoint& point : points) {
    point.setPoint(NOPOINTF);
  }

  updateStatus();

  item->getDrawContext()->triggerCompleteUpdate(CCanvas::eRedrawOverlay);
}

void CGridPlacer::updateStatus() {
  /// @todo optimize this

  const QPointF& pt1 = points[0].getPoint();
  const QPointF& pt2 = points[1].getPoint();
  const QPointF& pt3 = points[2].getPoint();
  const QPointF& pt4 = points[3].getPoint();

  statusIsOk = true;

  if (pt1 == NOPOINTF) {
    statusIsOk = false;
    labelStatusPoint1->setText(tr("Point 1 - not set"));
  } else {
    labelStatusPoint1->setText("<b style='color: green'>" + tr("Point 1 - ok") + "</b>");

    if ((pt2 != NOPOINTF) && (pt2.x() < pt1.x())) {
      statusIsOk = false;
      labelStatusPoint1->setText("<b style='color: red'>" + tr("Point 1 - bad") + "</b>");
    }
    if ((pt4 != NOPOINTF) && (pt4.y() < pt1.y())) {
      statusIsOk = false;
      labelStatusPoint1->setText("<b style='color: red'>" + tr("Point 1 - bad") + "</b>");
    }
  }

  if (pt2 == NOPOINTF) {
    statusIsOk = false;
    labelStatusPoint2->setText(tr("Point 2 - not set"));
  } else {
    labelStatusPoint2->setText("<b style='color: green'>" + tr("Point 2 - ok") + "</b>");

    if ((pt1 != NOPOINTF) && (pt1.x() > pt2.x())) {
      statusIsOk = false;
      labelStatusPoint2->setText("<b style='color: red'>" + tr("Point 2 - bad") + "</b>");
    }
    if ((pt3 != NOPOINTF) && (pt3.y() < pt2.y())) {
      statusIsOk = false;
      labelStatusPoint2->setText("<b style='color: red'>" + tr("Point 2 - bad") + "</b>");
    }
  }

  if (pt3 == NOPOINTF) {
    statusIsOk = false;
    labelStatusPoint3->setText(tr("Point 3 - not set"));
  } else {
    labelStatusPoint3->setText("<b style='color: green'>" + tr("Point 3 - ok") + "</b>");

    if ((pt4 != NOPOINTF) && (pt4.x() > pt3.x())) {
      statusIsOk = false;
      labelStatusPoint3->setText("<b style='color: red'>" + tr("Point 3 - bad") + "</b>");
    }
    if ((pt2 != NOPOINTF) && (pt2.y() > pt3.y())) {
      statusIsOk = false;
      labelStatusPoint3->setText("<b style='color: red'>" + tr("Point 3 - bad") + "</b>");
    }
  }

  if (pt4 == NOPOINTF) {
    statusIsOk = false;
    labelStatusPoint4->setText(tr("Point 4 - not set"));
  } else {
    labelStatusPoint4->setText("<b style='color: green'>" + tr("Point 4 - ok") + "</b>");

    if ((pt3 != NOPOINTF) && (pt3.x() < pt4.x())) {
      statusIsOk = false;
      labelStatusPoint4->setText("<b style='color: red'>" + tr("Point 4 - bad") + "</b>");
    }
    if ((pt1 != NOPOINTF) && (pt1.y() > pt4.y())) {
      statusIsOk = false;
      labelStatusPoint4->setText("<b style='color: red'>" + tr("Point 4 - bad") + "</b>");
    }
  }

  pushSetArea->setEnabled(statusIsOk);

  emit sigChanged();
}

void CGridPlacer::slotSetArea() {
  qreal bottom = -NOFLOAT;
  qreal top = NOFLOAT;
  qreal left = NOFLOAT;
  qreal right = -NOFLOAT;

  for (const CGridPoint& point : std::as_const(points)) {
    const QPointF& pt = point.getPoint();

    top = qMin(pt.y(), top);
    bottom = qMax(pt.y(), bottom);
    left = qMin(pt.x(), left);
    right = qMax(pt.x(), right);
  }

  QRectF r(0, 0, 1, 1);
  r.setLeft(left);
  r.setRight(right);
  r.setTop(top);
  r.setBottom(bottom);

  emit sigSetArea(r);
}
