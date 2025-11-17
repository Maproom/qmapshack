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

#ifndef CMAPITEMWIDGET_H
#define CMAPITEMWIDGET_H

#include <QHBoxLayout>
#include <QLabel>
#include <QToolButton>
#include <QVBoxLayout>
#include <QWidget>

class CMapItemWidget : public QWidget {
  Q_OBJECT
 public:
  enum class eStatus {
    Inactive,
    Active,
    Missing,
    Unused,
  };

  CMapItemWidget();
  ~CMapItemWidget() override;

  void setName(const QString& name) {
    mapName = name;
    labelName->setText(name);
  }
  const QString& getName() const { return mapName; }
  void setStatus(eStatus status);
  eStatus getStatus() const { return status; }

 signals:
  void sigActivate(bool);

 private slots:
  void slotSetChecked(bool yes);

 private:
  eStatus status = eStatus::Unused;
  QString mapName;
  QVBoxLayout* layout1;
  QHBoxLayout* layout2;
  QLabel* labelName;
  QLabel* labelStatus;
  QToolButton* buttonActivate;
};

#endif  // CMAPITEMWIDGET_H
