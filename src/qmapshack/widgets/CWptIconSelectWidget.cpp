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

#include "CWptIconSelectWidget.h"

#include <helpers/CWptIconManager.h>

#include <QBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMouseEvent>
#include <QPainter>
#include <QResizeEvent>
#include <QScrollArea>
#include <QScrollBar>

#include "widgets/CIconGrid.h"

CWptIconSelectWidget::CWptIconSelectWidget(QWidget* parent) : QWidget(parent) {
  scrollArea = new QScrollArea(this);
  scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
  scrollArea->verticalScrollBar()->setSingleStep(CIconGrid::kTileSize);
  scrollArea->setWidgetResizable(true);
  iconGrid = new CIconGrid(scrollArea);
  scrollArea->setWidget(iconGrid);

  iconName = new QLabel(this);
  QFont f = iconName->font();
  f.setBold(true);
  iconName->setFont(f);
  iconName->setAlignment(Qt::AlignCenter);

  actionClearFilter = new QAction(this);
  actionClearFilter->setObjectName("actionClearFilter");
  QIcon icon4;
  icon4.addFile(QString::fromUtf8(":/icons/32x32/Filter.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
  actionClearFilter->setIcon(icon4);
  actionClearFilter->setText(tr("Clear Filter"));

  iconFilter = new QLineEdit(this);
  iconFilter->setPlaceholderText(tr("start typing..."));
  iconFilter->setToolTip(tr("Filter: Start to type and the list will be reduced to matching items."));
  iconFilter->addAction(actionClearFilter, QLineEdit::TrailingPosition);

  layout = new QBoxLayout(QBoxLayout::TopToBottom, this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->addWidget(iconFilter);
  layout->addWidget(iconName);
  layout->addWidget(scrollArea);

  connect(iconGrid, &CIconGrid::sigIconName, iconName, &QLabel::setText);
  connect(iconGrid, &CIconGrid::sigSelectedIcon, this, &CWptIconSelectWidget::sigSelectedIcon);
  connect(iconFilter, &QLineEdit::textChanged, this, &CWptIconSelectWidget::slotFilter);
  connect(actionClearFilter, &QAction::triggered, iconFilter, &QLineEdit::clear);
  connect(&CWptIconManager::self(), &CWptIconManager::sigChanged, this, &CWptIconSelectWidget::slotWptListChanged);

  iconGrid->updateIconList(iconFilter->text());
}

void CWptIconSelectWidget::slotFilter(const QString& str) {
  actionClearFilter->setIcon(str.isEmpty() ? QIcon("://icons/32x32/Filter.png") : QIcon("://icons/32x32/Cancel.png"));
  iconGrid->updateIconList(iconFilter->text());
}

void CWptIconSelectWidget::slotWptListChanged() { iconGrid->updateIconList(iconFilter->text()); }
