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

#ifndef CITEMLISTWIDGET_H
#define CITEMLISTWIDGET_H

#include "ui_IItemListWidget.h"

class QSettings;
class IItem;

class CItemListWidget : public QWidget, private Ui::IItemListWidget {
  Q_OBJECT
 public:
  CItemListWidget(QWidget* parent);
  virtual ~CItemListWidget() = default;

  void saveSettings(QSettings& cfg);
  void loadSettings(QSettings& cfg);

  IItem* currentItem();
  IItem* item(int n);

  qint32 count() const { return listFiles->count(); }

  template <typename LessThan>
  void sort(LessThan lessThan) {
    listFiles->blockSignals(true);

    QList<QListWidgetItem*> items;
    while (listFiles->count() != 0) {
      items << listFiles->takeItem(0);
    }

    std::sort(items.begin(), items.end(), lessThan);

    for (QListWidgetItem* item : std::as_const(items)) {
      listFiles->addItem(item);
    }

    listFiles->blockSignals(false);
  }

 signals:
  void sigAddItem(const QString& filename, QListWidget* list);
  void sigSelectionChanged();
  void sigChanged();

 protected slots:
  void slotFiles();
  void slotLoadCurrentMap();
  void slotDeleteFiles();
  void slotDeleteFile();
  void slotSelectionChanged();

 protected:
  void addFiles(const QStringList& files);
};

#endif  // CITEMLISTWIDGET_H
