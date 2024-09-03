/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler <oliver.eichler@gmx.de>

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

#include "mouse/CScrOptUnclutter.h"

#include <QtWidgets>

#include "CMainWindow.h"
#include "canvas/CCanvas.h"
#include "gis/IGisItem.h"
#include "helpers/CDraw.h"

const QPoint CScrOptUnclutter::positions[9][8] = {
    {},

    {QPoint(-50, -23)},

    {QPoint(-30, 0), QPoint(30, 0)},

    {QPoint(0, -30), QPoint(30, 30), QPoint(-30, 30)},

    {QPoint(-30, -30), QPoint(30, -30), QPoint(-30, 30), QPoint(30, 30)},

    {QPoint(-25, 40), QPoint(25, 40), QPoint(-40, -5), QPoint(40, -5), QPoint(0, -40)},

    {QPoint(-40, -22), QPoint(40, -22), QPoint(-40, 22), QPoint(40, 22), QPoint(0, -55), QPoint(0, 55)},

    {QPoint(-50, -23), QPoint(50, -23), QPoint(-45, 21), QPoint(45, 21), QPoint(-22, -55), QPoint(22, -55),
     QPoint(0, 50)},

    {QPoint(-50, -23), QPoint(50, -23), QPoint(-50, 23), QPoint(50, 23), QPoint(-22, -55), QPoint(22, -55),
     QPoint(-22, 55), QPoint(22, 55)}};

CScrOptUnclutter::CScrOptUnclutter(IMouse* mouse) : IScrOpt(mouse) { hide(); }

CScrOptUnclutter::~CScrOptUnclutter() {}

void CScrOptUnclutter::clear() {
  if (doSpecialCursor) {
    CCanvas::restoreOverrideCursor("CScrOptUnclutter::clear()");
    doSpecialCursor = false;
  }
  items.clear();
}

void CScrOptUnclutter::mouseMove(const QPoint& pos) {
  IScrOpt::mouseMove(pos);

  for (const item_t& item : std::as_const(items)) {
    if (item.active.contains(mousePos) || item.text.contains(mousePos)) {
      if (!doSpecialCursor) {
        CCanvas::setOverrideCursor(Qt::PointingHandCursor, "CScrOptUnclutter::mouseMoveEvent");
        doSpecialCursor = true;
      }
      return;
    }
  }

  if (doSpecialCursor) {
    CCanvas::restoreOverrideCursor("CScrOptUnclutter::mouseMoveEvent");
    doSpecialCursor = false;
    return;
  }
}

void CScrOptUnclutter::addItem(IGisItem* gisItem) {
  items << item_t();
  item_t& item = items.last();
  item.hasUserFocus = gisItem->hasUserFocus();
  item.name = gisItem->getNameEx();
  item.key = gisItem->getKey();
  item.icon = gisItem->getDisplayIcon();
  item.area = item.icon.rect();
  item.active = item.area.adjusted(-10, -10, 10, 10);
}

IGisItem::key_t CScrOptUnclutter::getItemKey(int index) {
  if (index < items.size()) {
    return items[index].key;
  }
  return IGisItem::key_t();
}

const CScrOptUnclutter::item_t* CScrOptUnclutter::selectItem(const QPoint& point) {
  for (const item_t& item : std::as_const(items)) {
    if (item.active.contains(point) || item.text.contains(point)) {
      return &item;
    }
  }
  return nullptr;
}

void CScrOptUnclutter::draw(QPainter& p) {
  const int N = items.size();
  QFontMetrics fm(CMainWindow::self().getMapFont());
  for (int cnt = 0; cnt < N; cnt++) {
    item_t& item = items[cnt];

    if (item.text.isNull()) {
      item.area.moveCenter(origin + positions[N][cnt]);
      item.active.moveCenter(item.area.center());
      item.text = fm.boundingRect(item.name);
      if (cnt & 0x01) {
        item.text.moveTopLeft(item.area.topRight() + QPoint(17, fm.height() / 2));
      } else {
        item.text.moveTopRight(item.area.topLeft() + QPoint(-17, fm.height() / 2));
      }
      item.text.adjust(-4, -3, 4, 3);
    }
  }

  for (const item_t& item : std::as_const(items)) {
    p.setPen(Qt::NoPen);
    p.setBrush(QColor(255, 255, 255, 255));
    p.drawEllipse(item.area.center(), 20, 20);
    p.drawRoundedRect(item.text, RECT_RADIUS, RECT_RADIUS);

    p.setPen(QPen(item.hasUserFocus ? Qt::red : Qt::lightGray, 2));
    p.setBrush(Qt::NoBrush);
    p.drawRoundedRect(item.text, RECT_RADIUS, RECT_RADIUS);
    p.drawEllipse(item.area.center(), 18, 18);

    p.drawPixmap(item.area, item.icon);
    CDraw::text(item.name, p, item.text, Qt::darkBlue);
  }
}
