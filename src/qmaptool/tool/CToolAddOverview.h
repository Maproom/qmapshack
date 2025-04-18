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

#ifndef CTOOLADDOVERVIEW_H
#define CTOOLADDOVERVIEW_H

#include "items/IItem.h"
#include "tool/ITool.h"
#include "tool/IToolGui.h"
#include "ui_IToolAddOverview.h"

class CToolAddOverview : public IToolGui, public ITool, private Ui::IToolAddOverview {
  Q_OBJECT
 public:
  CToolAddOverview(QWidget* parent);
  virtual ~CToolAddOverview();

  void setupChanged() override;

  FORWARD_LIST_ALL(itemList)

 private slots:
  void slotAddItem(const QString& filename, QListWidget* list);
  void slotMapSelectionChanged();
  void slotSelectionChanged(Qt::CheckState state);
  void slotStart();
  void slotFinished(qint32 id);

 protected:
  void buildCmd(QList<CShellCmd>& cmds, const IItem* iitem) override;
};

#endif  // CTOOLADDOVERVIEW_H
