/**********************************************************************************************
   Copyright (C) 2026 Oliver Eichler <oliver.eichler@gmx.de>

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

#include "shoot/CShotContext.h"

#include <QAbstractItemView>
#include <QDebug>
#include <QImage>
#include <QPainter>
#include <QSlider>
#include <QStyle>
#include <QStyleOptionSlider>
#include <QTreeView>
#include <QWidget>

#include "CMainWindow.h"
#include "gis/CGisListWks.h"
#include "gis/CGisWorkspace.h"
#include "gis/ovl/CGisItemOvlArea.h"
#include "gis/rte/CGisItemRte.h"
#include "gis/trk/CGisItemTrk.h"
#include "gis/wpt/CGisItemWpt.h"
#include "shoot/CShotAddress.h"
#include "shoot/CShotWriter.h"

namespace {
template <typename T>
T* itemOf(const IGisItem::key_t& key) {
  return key.item.isEmpty() ? nullptr : dynamic_cast<T*>(CGisWorkspace::self().getItemByKey(key));
}
}  // namespace

CShotContext::CShotContext(const CShotWriter& writer) : writer(writer) {}

CMainWindow* CShotContext::mainWindow() const { return CMainWindow::isNull() ? nullptr : &CMainWindow::self(); }

CCanvas* CShotContext::canvas() const {
  const CMainWindow* main = mainWindow();
  return (nullptr == main) ? nullptr : main->getVisibleCanvas();
}

const CGisListWks* CShotContext::wksList() const {
  return (nullptr == mainWindow()) ? nullptr : &CGisWorkspace::self().getWksList();
}

IGisProject* CShotContext::project() const {
  if (fixture.project.isEmpty()) {
    return nullptr;
  }
  CGisListWks& wks = const_cast<CGisListWks&>(CGisWorkspace::self().getWksList());
  return wks.getProjectByKey(fixture.project);
}

CGisItemTrk* CShotContext::trk() const { return itemOf<CGisItemTrk>(fixture.trk); }

CGisItemWpt* CShotContext::wpt() const { return itemOf<CGisItemWpt>(fixture.wpt); }

CGisItemRte* CShotContext::rte() const { return itemOf<CGisItemRte>(fixture.rte); }

CGisItemOvlArea* CShotContext::area() const { return itemOf<CGisItemOvlArea>(fixture.area); }

QList<IGisItem::key_t> CShotContext::keys() const {
  QList<IGisItem::key_t> keys;
  for (const IGisItem::key_t& key : {fixture.trk, fixture.wpt, fixture.rte, fixture.area}) {
    if (!key.item.isEmpty()) {
      keys << key;
    }
  }
  return keys;
}

void CShotContext::setFixture(IGisProject* project, CGisItemTrk* trk, CGisItemWpt* wpt, CGisItemRte* rte,
                              CGisItemOvlArea* area) {
  const auto keyOf = [](const IGisItem* item) { return (nullptr == item) ? IGisItem::key_t() : item->getKey(); };
  fixture = {(nullptr == project) ? QString() : project->getKey(), keyOf(trk), keyOf(wpt), keyOf(rte), keyOf(area)};
}

namespace {
/** Reaches QSlider's protected initStyleOption(), which the style needs for the handle's geometry. */
struct CSliderAccess : QSlider {
  static void styleOption(const QSlider& slider, QStyleOptionSlider* option) {
    void (QSlider::*init)(QStyleOptionSlider*) const = &CSliderAccess::initStyleOption;
    (slider.*init)(option);
  }
};

/** @return the area of @p mark in @p w's coordinates, an empty rect when the part is not there */
QRect markArea(const CShotContext::mark_t& mark, QWidget* w) {
  QWidget* widget = mark.widget;
  if (mark.part.isEmpty()) {
    return QRect(widget->mapTo(w, QPoint(0, 0)), widget->size());
  }
  if ("handle" == mark.part) {
    const QSlider* slider = qobject_cast<const QSlider*>(widget);
    if (nullptr == slider) {
      return QRect();
    }
    QStyleOptionSlider option;
    CSliderAccess::styleOption(*slider, &option);
    const QRect handle = slider->style()->subControlRect(QStyle::CC_Slider, &option, QStyle::SC_SliderHandle, slider);
    return handle.translated(slider->mapTo(w, QPoint(0, 0)));
  }
  if ("branch" == mark.part) {
    const QAbstractItemView* view = CShotAddress::itemViewOf(widget);
    const QTreeView* tree = qobject_cast<const QTreeView*>(view);
    if (nullptr == tree) {
      return QRect();
    }
    const QModelIndex& index = CShotAddress::resolveRowPath(*tree->model(), mark.row);
    if (!index.isValid()) {
      return QRect();
    }
    // The branch indicator sits in the indentation left of the row's first cell.
    const QRect cell = tree->visualRect(index.siblingAtColumn(0));
    const QRect branch(cell.left() - tree->indentation(), cell.top(), tree->indentation(), cell.height());
    return branch.translated(tree->viewport()->mapTo(w, QPoint(0, 0)));
  }
  return QRect();
}
}  // namespace

void CShotContext::begin(const QString& id, const QRect& rect, const QList<mark_t>& marks) {
  this->id = id;
  this->rect = rect;
  this->marks = marks;
}

bool CShotContext::shot(QWidget* w, const QSize& size) { return emitImage(w, size, id); }

bool CShotContext::emitImage(QWidget* w, const QSize& size, const QString& stem) const {
  QImage image = CShotWriter::render(w, size);
  if (image.isNull()) {
    qWarning() << "shoot:" << stem << "has no image";
    return false;
  }
  // resize() silently stops at the minimum size.
  if (size.isValid() && image.size() != size) {
    qWarning() << "shoot:" << stem << "asks for" << size << "and the image is" << image.size();
    return false;
  }
  if (!marks.isEmpty()) {
    // Geometry after render(): it resizes the window and re-applies the layout.
    QPainter p(&image);
    p.setRenderHint(QPainter::Antialiasing);
    for (const mark_t& mark : marks) {
      if (mark.widget.isNull() || (mark.widget != w && !w->isAncestorOf(mark.widget)) || !mark.widget->isVisibleTo(w)) {
        qWarning() << "shoot:" << stem << "marks a widget that is not in the image";
        return false;
      }
      const QRect area = markArea(mark, w);
      qDebug() << "shoot:" << stem << "marks" << mark.widget->metaObject()->className() << mark.widget->objectName()
               << mark.part << "at" << area << "visible" << mark.widget->isVisibleTo(w);
      if (area.isEmpty()) {
        qWarning() << "shoot:" << stem << "marks" << mark.part << mark.row << "which its widget does not have";
        return false;
      }
      const QString& shape = mark.shape.isEmpty() ? (mark.part.isEmpty() ? "frame" : "disc") : mark.shape;
      if ("disc" == shape) {
        const qreal radius = qMax(14.0, qMax(area.width(), area.height()) / 2.0 + 6.0);
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(255, 0, 0, 70));
        p.drawEllipse(QRectF(area).center(), radius, radius);
      } else {
        p.setPen(QPen(QColor(255, 0, 0, 200), 3));
        p.setBrush(Qt::NoBrush);
        p.drawRoundedRect(QRectF(area).adjusted(1.5, 1.5, -1.5, -1.5), 4, 4);
      }
    }
  }
  if (!rect.isValid()) {
    return !writer.write(image, stem).isEmpty();
  }
  if (!image.rect().contains(rect)) {
    qWarning() << "shoot:" << stem << "keeps" << rect << "which is not inside" << image.rect();
    return false;
  }
  return !writer.write(image.copy(rect), stem).isEmpty();
}
