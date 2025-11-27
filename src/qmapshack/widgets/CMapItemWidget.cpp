/**********************************************************************************************
    Copyright (C) 2025 Oliver Eichler <oliver.eichler@gmx.de>

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

#include "widgets/CMapItemWidget.h"

#include <QGraphicsOpacityEffect>
#include <QPainter>
#include <QToolButton>

#include "canvas/IDrawObject.h"
#include "helpers/CDraw.h"

constexpr Qt::GlobalColor kColorOut = Qt::lightGray;
constexpr Qt::GlobalColor kColorIn = Qt::darkGreen;

class CIndicator final : public QWidget {
 public:
  CIndicator(QWidget* parent) : QWidget(parent) {}

  void setColor(const QColor& c) {
    color = c;
    update();
  }

 protected:
  void paintEvent(QPaintEvent* e) override {
    QPainter p(this);
    USE_ANTI_ALIASING(p, true);

    p.setBrush(color);
    p.setPen(color);
    p.drawRoundedRect(rect(), 4, 4);
  }

 private:
  QColor color = Qt::lightGray;
};

CMapItemWidget::CMapItemWidget(const QString& typeIMap) : typeIMap(typeIMap) {
  labelName = new QLabel(this);
  labelName->setAttribute(Qt::WA_TransparentForMouseEvents, true);
  labelStatus = new QLabel(tr("unknown"), this);
  QFont f = labelName->font();
  f.setPointSize(f.pointSize() - 2);
  labelStatus->setFont(f);
  labelStatus->setAttribute(Qt::WA_TransparentForMouseEvents, true);

  labelAccess = new QLabel(this);
  labelAccess->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
  labelAccess->setFont(f);

  effectLabelAccess = new QGraphicsOpacityEffect(labelAccess);
  labelAccess->setGraphicsEffect(effectLabelAccess);

  animationLabelAccess = new QPropertyAnimation(effectLabelAccess, "opacity");
  animationLabelAccess->setDuration(1000);
  animationLabelAccess->setStartValue(1.0);
  animationLabelAccess->setEndValue(0.0);
  animationLabelAccess->setEasingCurve(QEasingCurve::OutQuad);

  indicatorVisibility = new CIndicator(this);
  indicatorVisibility->setFixedSize(5, 20);

  buttonActivate = new QToolButton(this);
  buttonActivate->setCheckable(true);
  buttonActivate->setIcon(QIcon(":/icons/32x32/ShowNone.png"));

  layout2 = new QHBoxLayout(this);
  layout2->setContentsMargins(2, 2, 2, 2);
  layout2->setSpacing(3);

  layout1 = new QVBoxLayout();
  layout1->setContentsMargins(0, 0, 0, 0);
  layout1->setSpacing(0);

  layout1->addWidget(labelName);
  layout1->addWidget(labelStatus);

  layout2->addLayout(layout1);
  layout2->addWidget(labelAccess);
  layout2->addWidget(indicatorVisibility);
  layout2->addWidget(buttonActivate);

  timerAccess = new QTimer(this);
  timerAccess->setSingleShot(true);
  timerAccess->setInterval(1000);

  connect(buttonActivate, &QToolButton::clicked, this, &CMapItemWidget::sigActivate);
  connect(buttonActivate, &QToolButton::toggled, this, &CMapItemWidget::slotSetChecked);
  connect(timerAccess, &QTimer::timeout, this, [this]() {
    animationLabelAccess->stop();
    animationLabelAccess->start(QAbstractAnimation::KeepWhenStopped);
  });
}

CMapItemWidget::~CMapItemWidget() { /*qDebug() << "~CMapItemWidget()" << labelName->text();*/ }

void CMapItemWidget::setDrawObject(IDrawObject* object, const QPointF& scale) {
  map = object;
  indicatorVisibility->setHidden(map.isNull());
  slotScaleChanged(scale);
}

void CMapItemWidget::setAccess(const QString& ele) {
  labelAccess->setText(ele);
  effectLabelAccess->setOpacity(1.0);
  timerAccess->start();
}

void CMapItemWidget::setStatus(eStatus status) {
  this->status = status;
  switch (status) {
    case eStatus::Inactive:
      labelStatus->setText(tr("inactive"));
      labelName->setText(mapName);
      labelName->setEnabled(false);
      buttonActivate->setChecked(false);
      buttonActivate->setToolTip(tr("Activate %1").arg(typeIMap));
      indicatorVisibility->hide();
      break;

    case eStatus::Active:
      labelStatus->setText(tr("active"));
      labelName->setText("<b>" + mapName + "</b>");
      labelName->setEnabled(true);
      buttonActivate->setChecked(true);
      buttonActivate->setToolTip(tr("Deactivate %1").arg(typeIMap));
      indicatorVisibility->show();
      break;

    case eStatus::Missing:
      labelStatus->setText("<span style='color:red'>!!!" + tr("missing") + "!!!</span>");
      labelName->setText(mapName);
      labelName->setEnabled(false);
      buttonActivate->setChecked(false);
      buttonActivate->setToolTip(tr("Activate %1").arg(typeIMap));
      indicatorVisibility->hide();
      break;

    case eStatus::Unused:
      labelName->setEnabled(false);
      labelName->setText(mapName);
      labelStatus->setText("-");
      buttonActivate->setChecked(false);
      buttonActivate->setToolTip(tr("Activate %1").arg(typeIMap));
      indicatorVisibility->hide();
      break;
  }

  buttonActivate->setEnabled(status != eStatus::Missing);
}

void CMapItemWidget::slotSetChecked(bool yes) {
  if (yes) {
    buttonActivate->setIcon(QIcon(":/icons/32x32/ShowAll.png"));
  } else {
    buttonActivate->setIcon(QIcon(":/icons/32x32/ShowNone.png"));
  }
}

void CMapItemWidget::slotScaleChanged(const QPointF& scale) {
  if (map.isNull()) {
    return;
  }
  if (map->isOutOfScale(scale)) {
    indicatorVisibility->setColor(kColorOut);
    indicatorVisibility->setToolTip(tr("%1 is not visible at current scale").arg(typeIMap));
  } else {
    indicatorVisibility->setColor(kColorIn);
    indicatorVisibility->setToolTip(tr("%1 is visible at current scale").arg(typeIMap));
  }
}
