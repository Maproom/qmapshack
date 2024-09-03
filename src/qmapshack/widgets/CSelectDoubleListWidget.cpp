/**********************************************************************************************
    Copyright (C) 2017 Norbert Truchsess <norbert.truchsess@t-online.de>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

**********************************************************************************************/

#include "CSelectDoubleListWidget.h"

#include <QListWidgetItem>

CSelectDoubleListWidget::CSelectDoubleListWidget(QWidget* parent, IItemFilter* filter)
    : QWidget(parent), filter(filter) {
  setupUi(this);

  connect(listSelected, &QListView::clicked, this, &CSelectDoubleListWidget::slotSelectedClicked);
  connect(listAvailable, &QListView::clicked, this, &CSelectDoubleListWidget::slotAvailableClicked);
  connect(toolAdd, &QToolButton::clicked, this, &CSelectDoubleListWidget::slotAdd);
  connect(toolRemove, &QToolButton::clicked, this, &CSelectDoubleListWidget::slotRemove);
  connect(toolUp, &QToolButton::clicked, this, &CSelectDoubleListWidget::slotUp);
  connect(toolDown, &QToolButton::clicked, this, &CSelectDoubleListWidget::slotDown);
}

CSelectDoubleListWidget::~CSelectDoubleListWidget() {}

void CSelectDoubleListWidget::sortAvailable() { listAvailable->sortItems(); }

void CSelectDoubleListWidget::setAvailable(const QList<QListWidgetItem*>& available) {
  this->available.clear();
  this->available << available;
  listAvailable->clear();
  for (QListWidgetItem* const& item : available) {
    int index = listSelected->row(item);
    if (index < 0) {
      listAvailable->addItem(item);
    } else if (filter != nullptr && !filter->shouldBeMoved(item)) {
      listSelected->takeItem(index);
      listSelected->insertItem(index, item->clone());
      listAvailable->addItem(item);
    }
  }
  updateButtons();
}

void CSelectDoubleListWidget::setSelected(const QList<QListWidgetItem*>& selected) const {
  listSelected->clear();
  for (QListWidgetItem* const& item : selected) {
    int index = listAvailable->row(item);
    if (index < 0) {
      listSelected->addItem(item);
    } else {
      if (filter == nullptr || filter->shouldBeMoved(item)) {
        listAvailable->takeItem(index);
        listSelected->addItem(item);
      } else {
        listSelected->addItem(item->clone());
      }
    }
  }
  updateButtons();
}

void CSelectDoubleListWidget::setFilter(IItemFilter* const& filter) { this->filter = filter; }

void CSelectDoubleListWidget::setLabelAvailable(const QString& label) const { labelAvailable->setText(label); }

void CSelectDoubleListWidget::setLabelSelected(const QString& label) const { labelSelected->setText(label); }

const QList<QListWidgetItem*> CSelectDoubleListWidget::selected() const {
  QList<QListWidgetItem*> selected;
  for (int i = 0; i < listSelected->count(); i++) {
    selected << listSelected->item(i);
  }
  return selected;
}

void CSelectDoubleListWidget::clear() {
  this->available.clear();
  listAvailable->clear();
  listSelected->clear();
}

void CSelectDoubleListWidget::slotSelectedClicked(const QModelIndex& index) const {
  listAvailable->clearSelection();
  updateButtons();
}

void CSelectDoubleListWidget::slotAvailableClicked(const QModelIndex& index) const {
  listSelected->clearSelection();
  updateButtons();
}

void CSelectDoubleListWidget::slotAdd() const {
  const QList<QListWidgetItem*>& items = listAvailable->selectedItems();
  for (QListWidgetItem* const& item : items) {
    if (filter == nullptr || filter->shouldBeMoved(item)) {
      listAvailable->takeItem(listAvailable->row(item));
      listSelected->addItem(item);
    } else {
      listSelected->addItem(item->clone());
    }
  }
  updateButtons();
}

void CSelectDoubleListWidget::slotRemove() const {
  const QList<QListWidgetItem*>& items = listSelected->selectedItems();
  for (QListWidgetItem* const& item : items) {
    if (filter == nullptr || filter->shouldBeMoved(item)) {
      int index = -1;
      for (int i = available.indexOf(item) - 1; i >= 0; i--) {
        index = listAvailable->row(available.at(i));
        if (index >= 0) {
          break;
        }
      }
      index++;
      listSelected->takeItem(listSelected->row(item));
      listAvailable->insertItem(index, item);
    } else {
      delete listSelected->takeItem(listSelected->row(item));
    }
  }
  updateButtons();
}

void CSelectDoubleListWidget::slotUp() const {
  QList<int> indices;
  const QModelIndexList& indexes = listSelected->selectionModel()->selectedIndexes();
  for (const QModelIndex& modelIndex : indexes) {
    indices << modelIndex.row();
  }
  std::sort(indices.begin(), indices.end());

  int i = 0;
  for (int index : std::as_const(indices)) {
    if (index > i) {
      listSelected->insertItem(index - 1, listSelected->takeItem(index));
      listSelected->setCurrentRow(index - 1, QItemSelectionModel::Select);
    }
    i++;
  }
  updateButtons();
}

void CSelectDoubleListWidget::slotDown() const {
  QList<int> indices;
  const QModelIndexList& indexes = listSelected->selectionModel()->selectedIndexes();
  for (const QModelIndex& modelIndex : indexes) {
    indices << modelIndex.row();
  }
  std::sort(indices.begin(), indices.end(), [](int a, int b) { return a > b; });

  int i = listSelected->count() - 1;
  for (int index : std::as_const(indices)) {
    if (index < i) {
      listSelected->insertItem(index + 1, listSelected->takeItem(index));
      listSelected->setCurrentRow(index + 1, QItemSelectionModel::Select);
    }
    i--;
  }
  updateButtons();
}

void CSelectDoubleListWidget::updateButtons() const {
  toolAdd->setEnabled(listAvailable->selectionModel()->hasSelection());

  const QItemSelectionModel* const& selectedSelectionModel = listSelected->selectionModel();
  if (selectedSelectionModel->hasSelection()) {
    toolRemove->setEnabled(true);

    int minSelected = listSelected->count();
    int maxSelected = -1;
    int minUnselected = minSelected;
    int maxUnselected = -1;

    for (int i = 0; i < listSelected->count(); i++) {
      if (selectedSelectionModel->isRowSelected(i, QModelIndex())) {
        if (i < minSelected) {
          minSelected = i;
        }
        maxSelected = i;
      } else {
        if (i < minUnselected) {
          minUnselected = i;
        }
        maxUnselected = i;
      }
    }

    toolUp->setEnabled(minUnselected < maxSelected);
    toolDown->setEnabled(maxUnselected > minSelected);
  } else {
    toolRemove->setEnabled(false);
    toolUp->setEnabled(false);
    toolDown->setEnabled(false);
  }
}
