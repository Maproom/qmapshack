/**********************************************************************************************
    Copyright (C) 2018 Oliver Eichler <oliver.eichler@gmx.de>

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

#ifndef CITEMTREEWIDGET_H
#define CITEMTREEWIDGET_H

#include "ui_IItemTreeWidget.h"

#include "canvas/CCanvas.h"

class QSettings;
class ITool;
class CItemMap;

class CItemTreeWidget : public QWidget, private Ui::IItemTreeWidget {
  Q_OBJECT
public:
  enum column_e { eColumnName };

  CItemTreeWidget(QWidget *parent);
  virtual ~CItemTreeWidget() = default;

  void saveSettings(QSettings &cfg);
  void loadSettings(QSettings &cfg);

  ITool *currentItem();

  template <typename LessThan> void sort(LessThan lessThan) {
    treeFiles->blockSignals(true);

    QList<QTreeWidgetItem *> items;
    while (treeFiles->topLevelItemCount() != 0) {
      items << treeFiles->takeTopLevelItem(0);
    }

    std::sort(items.begin(), items.end(), lessThan);

    for (QTreeWidgetItem *item : std::as_const(items)) {
      treeFiles->addTopLevelItem(item);
    }

    treeFiles->blockSignals(false);
  }

  qint32 topLevelItemCount() const { return treeFiles->topLevelItemCount(); }

  QTreeWidgetItem *topLevelItem(int n) { return treeFiles->topLevelItem(n); }

  bool drawFx(QPainter &p, CCanvas::redraw_e needsRedraw);

signals:
  void sigSelectionChanged();
  void sigChanged();

protected slots:
  void slotFiles();
  void slotDeleteFiles();
  void slotDeleteFile();
  void slotSelectionChanged();

protected:
  void addFiles(const QStringList &files);
  CItemMap *findMapByHash(const QString &hash);
};

#endif // CITEMTREEWIDGET_H
