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

#include <QLabel>
#include <QPointer>
#include <QWidget>

class QToolButton;
class QHBoxLayout;
class QVBoxLayout;
class IDrawObject;
class CIndicator;
class QPropertyAnimation;
class QGraphicsOpacityEffect;

class CMapItemWidget : public QWidget {
  Q_OBJECT
 public:
  enum class eStatus {
    Inactive,
    Active,
    Missing,
    Unused,
  };

  CMapItemWidget(const QString& type);
  ~CMapItemWidget() override;

  void setName(const QString& name) {
    mapName = name;
    labelName->setText(name);
  }
  const QString& getName() const { return mapName; }
  void setStatus(eStatus status);
  eStatus getStatus() const { return status; }

  void setDrawObject(IDrawObject* object, const QPointF& scale);

  void setAccess(const QString& ele);

 signals:
  void sigActivate(bool);

 public slots:
  void slotScaleChanged(const QPointF& scale);

 private slots:
  void slotSetChecked(bool yes);

 private:
  const QString typeIMap;
  eStatus status = eStatus::Unused;
  QString mapName;
  QVBoxLayout* layout1;
  QHBoxLayout* layout2;
  QLabel* labelName;
  QLabel* labelStatus;
  QLabel* labelAccess;
  CIndicator* indicatorVisibility;
  QToolButton* buttonActivate;
  QTimer* timerAccess;
  QPropertyAnimation* animationLabelAccess;
  QGraphicsOpacityEffect* effectLabelAccess;

  QPointer<IDrawObject> map;
};

#endif  // CMAPITEMWIDGET_H
