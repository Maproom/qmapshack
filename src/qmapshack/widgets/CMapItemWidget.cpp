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

CMapItemWidget::CMapItemWidget() {
  labelName = new QLabel(this);
  labelName->setAttribute(Qt::WA_TransparentForMouseEvents, true);
  labelStatus = new QLabel(tr("unknown"), this);
  QFont f = labelName->font();
  f.setPointSize(f.pointSize() - 2);
  labelStatus->setFont(f);
  labelStatus->setAttribute(Qt::WA_TransparentForMouseEvents, true);

  buttonActivate = new QToolButton(this);
  buttonActivate->setCheckable(true);
  buttonActivate->setIcon(QIcon("://icons/32x32/ShowNone.png"));

  layout2 = new QHBoxLayout(this);

  layout1 = new QVBoxLayout();
  layout1->setContentsMargins(0, 0, 0, 0);
  layout1->setSpacing(0);
  layout1->addWidget(labelName);
  layout1->addWidget(labelStatus);

  layout2->addLayout(layout1);
  layout2->setContentsMargins(2, 2, 2, 2);
  layout2->setSpacing(0);
  layout2->addWidget(buttonActivate);

  connect(buttonActivate, &QToolButton::clicked, this, &CMapItemWidget::sigActivate);
  connect(buttonActivate, &QToolButton::toggled, this, &CMapItemWidget::slotSetChecked);
}

CMapItemWidget::~CMapItemWidget() { /*qDebug() << "~CMapItemWidget()" << labelName->text();*/ }

void CMapItemWidget::setStatus(eStatus status) {
  this->status = status;
  switch (status) {
    case eStatus::Inactive:
      labelStatus->setText(tr("inactive"));
      labelName->setText(mapName);
      labelName->setEnabled(false);
      buttonActivate->setChecked(false);
      break;

    case eStatus::Active:
      labelStatus->setText(tr("active"));
      labelName->setText("<b>" + mapName + "</b>");
      labelName->setEnabled(true);
      buttonActivate->setChecked(true);
      break;

    case eStatus::Missing:
      labelStatus->setText("<font color='red'>!!!" + tr("missing") + "!!!</font>");
      labelName->setText(mapName);
      labelName->setEnabled(false);
      buttonActivate->setChecked(false);
      break;

    case eStatus::Unused:
      labelName->setEnabled(false);
      labelName->setText(mapName);
      labelStatus->setText("-");
      buttonActivate->setChecked(false);
      break;
  }

  buttonActivate->setEnabled(status != eStatus::Missing);
}

void CMapItemWidget::slotSetChecked(bool yes) {
  if (yes) {
    buttonActivate->setIcon(QIcon("://icons/32x32/ShowAll.png"));
  } else {
    buttonActivate->setIcon(QIcon("://icons/32x32/ShowNone.png"));
  }
}
