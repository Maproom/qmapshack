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

#ifndef CWPTICONSELECTWIDGET_H
#define CWPTICONSELECTWIDGET_H

#include <QWidget>

class QScrollArea;
class QBoxLayout;
class CIconGrid;
class QLabel;
class QLineEdit;
class QComboBox;

class CWptIconSelectWidget : public QWidget {
  Q_OBJECT
 public:
  CWptIconSelectWidget(QWidget* parent);
  virtual ~CWptIconSelectWidget() = default;

 signals:
  void sigSelectedIcon(const QString& name);

 private slots:
  void slotFilterChanged(const QString& str);
  void slotCategoryChanged(const QString& str);
  void slotWptListChanged();

 private:
  QScrollArea* scrollArea;
  QBoxLayout* layout1;
  QLabel* iconName;
  QLineEdit* iconFilter;
  QComboBox* categoryFilter;
  QBoxLayout* layout2;
  CIconGrid* iconGrid;
  QAction* actionClearFilter;
};

#endif  // CWPTICONSELECTWIDGET_H
