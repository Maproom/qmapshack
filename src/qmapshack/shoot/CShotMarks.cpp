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

#include "shoot/CShotMarks.h"

#include <QAbstractItemView>
#include <QApplication>
#include <QDockWidget>
#include <QImage>
#include <QJsonArray>
#include <QJsonObject>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QPainter>
#include <QPainterPath>
#include <QPainterPathStroker>
#include <QSet>
#include <QSlider>
#include <QSplitter>
#include <QStyle>
#include <QStyleOptionSlider>
#include <QTabBar>
#include <QTabWidget>
#include <QToolBar>
#include <QTreeView>
#include <QtMath>
#include <cmath>

#include "CMainWindow.h"
#include "canvas/CCanvas.h"
#include "gis/proj_x.h"
#include "shoot/CShotAddress.h"
#include "shoot/CShotHandlers.h"

namespace {
const QColor kRed(255, 0, 0, 200);
const QColor kDisc(255, 0, 0, 70);
const QColor kLight(255, 255, 255, 190);
constexpr qreal kLine = 3.0;
constexpr qreal kOutline = 2.0;
/** A frame's see-through band, so it is no widget border [px]. */
constexpr qreal kBand = 12.0;
/** A cursor's glow [px]. */
constexpr qreal kGlow = 18.0;
/** A target no thicker than this is a line a disc sits across [px]. */
constexpr qint32 kThin = 16;
/** A target thicker than this is no spot a disc can mark [px]. */
constexpr qint32 kSpot = 2 * kThin;

/** @return a disc's radius on @p area: by the shorter side, so a disc on a long, thin target stays a spot */
qreal discRadius(const QRectF& area) { return qMax(14.0, qMin(area.width(), area.height()) / 2.0 + 6.0); }

const QSet<QString> kKeys = {"root",  "widget", "part",   "row",  "action", "button", "between",
                             "shape", "tail",   "cursor", "name", "lat",    "lon"};

/** Reaches QSlider's protected initStyleOption(), which the style needs for the handle's geometry. */
struct CSliderAccess : QSlider {
  static QRect handleOf(const QSlider& slider) {
    QStyleOptionSlider option;
    void (QSlider::*init)(QStyleOptionSlider*) const = &CSliderAccess::initStyleOption;
    (slider.*init)(&option);
    return slider.style()->subControlRect(QStyle::CC_Slider, &option, QStyle::SC_SliderHandle, &slider);
  }
};

/** @return the shape drawn: a disc for a knob, a tree arrow or a splitter, else a frame, unless the mark names one */
QString shapeOf(const CShotMarks::mark_t& mark) {
  if (!mark.shape.isEmpty()) {
    return mark.shape;
  }
  return ("handle" == mark.part || "branch" == mark.part || "splitter" == mark.part || "point" == mark.part) ? "disc"
                                                                                                             : "frame";
}

QRect globalRect(const QWidget* widget, const QRect& local) {
  return local.translated(widget->mapToGlobal(QPoint(0, 0)));
}

QRect globalRect(const QWidget* widget) { return globalRect(widget, widget->rect()); }

/** @return @p why; @p out says its target is there, but not on screen now */
QString closed(CShotMarks::placed_t& out, const QString& why) {
  out.found = CShotMarks::found_e::eClosed;
  return why;
}

/** @return @p why; @p out says it has no target */
QString missing(CShotMarks::placed_t& out, const QString& why) {
  out.found = CShotMarks::found_e::eMissing;
  return why;
}

/** @return @p problem, why @p address below @p root resolves to no single widget; @p out says whether there is none */
QString notResolved(QWidget* root, const QString& address, const QString& problem, CShotMarks::placed_t& out) {
  return (nullptr == CShotAddress::resolve(root, address)) ? missing(out, problem) : problem;
}

QWidget* rootOf(const QString& root, QWidget* shot, QString& problem) {
  QWidget* main = CMainWindow::isNull() ? nullptr : &CMainWindow::self();
  if (root.isEmpty() || "main" == root) {
    if (nullptr == main) {
      problem = "there is no main window";
    }
    return main;
  }
  if ("menu" == root) {
    QMenu* menu = qobject_cast<QMenu*>(QApplication::activePopupWidget());
    if (nullptr == menu) {
      menu = qobject_cast<QMenu*>(shot);
    }
    if (nullptr == menu) {
      problem = "no menu is open";
    }
    return menu;
  }
  if ("window" == root) {
    QWidget* window = (nullptr == shot) ? nullptr : shot->window();
    if (nullptr == window || window == main) {
      problem = "the image shows no window of its own";
      return nullptr;
    }
    return window;
  }
  if (root.startsWith("dock:")) {
    QDockWidget* dock = qobject_cast<QDockWidget*>(CShotAddress::resolveUnique(main, root.mid(5), problem));
    if (nullptr == dock && problem.isEmpty()) {
      problem = QString("%1 is no docker").arg(root.mid(5));
    }
    return dock;
  }
  problem = QString("its root %1 is unknown").arg(root);
  return nullptr;
}

/** @return the docker in front of @p widget's tab group, @p widget itself when it is no docked docker */
QWidget* frontOf(QMainWindow* main, QWidget* widget) {
  QDockWidget* dock = qobject_cast<QDockWidget*>(widget);
  if (nullptr == dock || dock->isFloating()) {
    return widget;
  }
  // A docker behind a tab stays visible, moved out of the window.
  const auto inFront = [main](const QDockWidget* d) {
    return d->isVisible() && main->rect().contains(QRect(d->mapTo(main, QPoint(0, 0)), d->size()));
  };
  if (inFront(dock)) {
    return dock;
  }
  const QList<QDockWidget*>& group = main->tabifiedDockWidgets(dock);
  for (QDockWidget* other : group) {
    if (inFront(other)) {
      return other;
    }
  }
  return dock;
}

/** @return why no separator of @p main lies between @p a and @p b, empty when @p area holds it */
QString separatorBetween(QMainWindow* main, QWidget* a, QWidget* b, QRect& area, Qt::Orientation& arrows) {
  const QRect ra(a->mapTo(main, QPoint(0, 0)), a->size());
  const QRect rb(b->mapTo(main, QPoint(0, 0)), b->size());
  const auto separator = [main](qint32 x, qint32 y) { return main->isSeparator(QPoint(x, y)); };
  // Between neighbours lie only separators and tab bars; another widget means they are none.
  const auto between = [main, a, b](qint32 x, qint32 y) {
    for (const QWidget* w = main->childAt(QPoint(x, y)); nullptr != w && w != main; w = w->parentWidget()) {
      if (w == a || w == b || w->inherits("QTabBar")) {
        return true;
      }
    }
    return nullptr == main->childAt(QPoint(x, y)) || main->isSeparator(QPoint(x, y));
  };
  const auto clear = [&between](const QPoint& from, const QPoint& to) {
    const QPoint step((to.x() > from.x()) - (to.x() < from.x()), (to.y() > from.y()) - (to.y() < from.y()));
    for (QPoint p = from; p != to; p += step) {
      if (!between(p.x(), p.y())) {
        return false;
      }
    }
    return true;
  };

  const qint32 top = qMax(ra.top(), rb.top());
  const qint32 bottom = qMin(ra.bottom(), rb.bottom());
  const qint32 left = qMax(ra.left(), rb.left());
  const qint32 right = qMin(ra.right(), rb.right());
  QRect found;
  bool neighbours = false;
  if (top <= bottom && (ra.right() < rb.left() || rb.right() < ra.left())) {
    // Side by side: a separator between them runs top to bottom.
    const QRect& first = (ra.right() < rb.left()) ? ra : rb;
    const QRect& second = (ra.right() < rb.left()) ? rb : ra;
    const qint32 y = (top + bottom) / 2;
    for (qint32 x = first.right() + 1; x < second.left() && found.isNull(); x++) {
      if (separator(x, y)) {
        qint32 x1 = x;
        while (x1 + 1 < second.left() && separator(x1 + 1, y)) {
          x1++;
        }
        qint32 y0 = y;
        qint32 y1 = y;
        while (y0 > 0 && separator(x, y0 - 1)) {
          y0--;
        }
        while (y1 + 1 < main->height() && separator(x, y1 + 1)) {
          y1++;
        }
        found = QRect(QPoint(x, y0), QPoint(x1, y1));
      }
    }
    neighbours = clear(QPoint(first.right() + 1, y), QPoint(second.left(), y));
    arrows = Qt::Horizontal;
  } else if (left <= right && (ra.bottom() < rb.top() || rb.bottom() < ra.top())) {
    // One above the other, a tab bar possibly between: the separator runs left to right.
    const QRect& first = (ra.bottom() < rb.top()) ? ra : rb;
    const QRect& second = (ra.bottom() < rb.top()) ? rb : ra;
    const qint32 x = (left + right) / 2;
    for (qint32 y = first.bottom() + 1; y < second.top() && found.isNull(); y++) {
      if (separator(x, y)) {
        qint32 y1 = y;
        while (y1 + 1 < second.top() && separator(x, y1 + 1)) {
          y1++;
        }
        qint32 x0 = x;
        qint32 x1 = x;
        while (x0 > 0 && separator(x0 - 1, y)) {
          x0--;
        }
        while (x1 + 1 < main->width() && separator(x1 + 1, y)) {
          x1++;
        }
        found = QRect(QPoint(x0, y), QPoint(x1, y1));
      }
    }
    neighbours = clear(QPoint(x, first.bottom() + 1), QPoint(x, second.top()));
    arrows = Qt::Vertical;
  }
  if (found.isNull() || !neighbours) {
    return "they are no neighbours";
  }
  area = globalRect(main, found);
  return QString();
}

QString placeSplitter(const CShotMarks::mark_t& mark, QWidget* root, CShotMarks::placed_t& out) {
  QWidget* sides[2] = {nullptr, nullptr};
  for (qint32 i = 0; i < 2; i++) {
    QString problem;
    sides[i] = CShotAddress::resolveUnique(root, mark.between.value(i), problem);
    if (nullptr == sides[i]) {
      return notResolved(root, mark.between.value(i), problem, out);
    }
  }

  // Two neighbours in a QSplitter: the handle between them.
  QSplitter* splitter = qobject_cast<QSplitter*>(sides[0]->parentWidget());
  if (nullptr != splitter && splitter == sides[1]->parentWidget()) {
    const qint32 a = splitter->indexOf(sides[0]);
    const qint32 b = splitter->indexOf(sides[1]);
    if (qAbs(a - b) != 1) {
      return "they are no neighbours";
    }
    QWidget* handle = splitter->handle(qMax(a, b));
    if (!handle->isVisibleTo(splitter->window())) {
      return closed(out, "the handle between them is hidden");
    }
    out.area = globalRect(handle);
    out.arrows = splitter->orientation();
    out.window = handle->window();
    return QString();
  }

  QMainWindow* main = qobject_cast<QMainWindow*>(sides[0]->window());
  if (nullptr == main || main != sides[1]->window()) {
    return "they share no window with separators";
  }
  // A docker stands for its tab group: the one in front borders the separator.
  QWidget* a = frontOf(main, sides[0]);
  QWidget* b = frontOf(main, sides[1]);
  out.window = main;
  return separatorBetween(main, a, b, out.area, out.arrows);
}

QString placeAction(const CShotMarks::mark_t& mark, QWidget* widget, CShotMarks::placed_t& out) {
  QAction* action = nullptr;
  const QList<QAction*>& actions = widget->actions();
  for (QAction* entry : actions) {
    if (entry->objectName() == mark.action) {
      action = entry;
    }
  }
  if (nullptr == action) {
    return missing(out, QString("it has no entry %1").arg(mark.action));
  }
  if (!action->isVisible()) {
    return closed(out, QString("its entry %1 is hidden").arg(mark.action));
  }
  if (const QMenu* menu = qobject_cast<const QMenu*>(widget); nullptr != menu) {
    out.area = globalRect(widget, menu->actionGeometry(action));
  } else if (const QMenuBar* bar = qobject_cast<const QMenuBar*>(widget); nullptr != bar) {
    out.area = globalRect(widget, bar->actionGeometry(action));
  } else if (const QToolBar* toolbar = qobject_cast<const QToolBar*>(widget); nullptr != toolbar) {
    QWidget* button = toolbar->widgetForAction(action);
    if (nullptr == button || !button->isVisibleTo(toolbar)) {
      return closed(out, QString("its entry %1 is not shown").arg(mark.action));
    }
    out.area = globalRect(button);
  } else {
    return "it is no menu or toolbar";
  }
  return out.area.isEmpty() ? closed(out, QString("its entry %1 has no place").arg(mark.action)) : QString();
}

/** @return why @p widget, a docked docker or a tab widget's page, shows no tab, empty when @p out holds it */
QString placeTab(QWidget* widget, CShotMarks::placed_t& out) {
  const QTabBar* bar = nullptr;
  qint32 at = -1;
  if (const QDockWidget* dock = qobject_cast<const QDockWidget*>(widget); nullptr != dock) {
    if (nullptr == qobject_cast<const QMainWindow*>(dock->parentWidget()) || dock->isFloating()) {
      return "it is no docked docker";
    }
    if (!dock->isVisibleTo(dock->window())) {
      return closed(out, "it is hidden");
    }
    at = CShotMarks::dockTab(dock, bar);
    if (nullptr == bar) {
      return "it has no tab";
    }
  } else {
    // A page sits in the tab widget's stack.
    const QWidget* stack = widget->parentWidget();
    const QTabWidget* tabs = (nullptr == stack) ? nullptr : qobject_cast<const QTabWidget*>(stack->parentWidget());
    if (nullptr == tabs || tabs->indexOf(widget) < 0) {
      return "it is no docker or tab page";
    }
    if (!tabs->isVisibleTo(tabs->window())) {
      return closed(out, "its tab widget is hidden");
    }
    bar = tabs->tabBar();
    at = tabs->indexOf(widget);
  }
  if (!bar->isTabVisible(at)) {
    return closed(out, "its tab is hidden");
  }
  out.area = globalRect(bar, bar->tabRect(at));
  return out.area.isEmpty() ? closed(out, "its tab has no place") : QString();
}

QString placePart(const CShotMarks::mark_t& mark, QWidget* widget, CShotMarks::placed_t& out) {
  if (mark.part.isEmpty()) {
    out.area = globalRect(widget);
    return QString();
  }
  if ("handle" == mark.part) {
    const QSlider* slider = qobject_cast<const QSlider*>(widget);
    if (nullptr == slider) {
      return "it is no slider";
    }
    out.area = globalRect(widget, CShotMarks::sliderHandle(*slider));
    return QString();
  }
  if ("branch" == mark.part) {
    const QTreeView* tree = qobject_cast<const QTreeView*>(CShotAddress::itemViewOf(widget));
    if (nullptr == tree) {
      return "it is no tree";
    }
    const QModelIndex& index = CShotAddress::resolveRowPath(*tree->model(), mark.row);
    if (!index.isValid()) {
      return missing(out, QString("it has no row %1").arg(mark.row));
    }
    // The branch indicator sits in the indentation left of the row's first cell, centred in the row's height.
    const QRect cell = tree->visualRect(index.siblingAtColumn(0));
    const qint32 side = qMin(tree->indentation(), cell.height());
    const QRect branch(cell.left() - tree->indentation() + (tree->indentation() - side) / 2,
                       cell.center().y() - side / 2, side, side);
    // A row under a closed parent has no place; one scrolled away lies outside the viewport.
    if (cell.isEmpty() || !tree->viewport()->rect().contains(branch)) {
      return closed(out, QString("its row %1 is not shown now").arg(mark.row));
    }
    out.area = globalRect(tree->viewport(), branch);
    return QString();
  }
  if ("title" == mark.part) {
    const QDockWidget* dock = qobject_cast<const QDockWidget*>(widget);
    if (nullptr == dock) {
      return "it is no docker";
    }
    if (const QWidget* bar = dock->titleBarWidget(); nullptr != bar) {
      out.area = globalRect(bar);
      return QString();
    }
    // The dock's own title is what its content leaves free: a strip on the left or the top.
    const QRect content = (nullptr == dock->widget()) ? QRect() : dock->widget()->geometry();
    const QRect title = dock->features().testFlag(QDockWidget::DockWidgetVerticalTitleBar)
                            ? QRect(0, 0, content.left(), dock->height())
                            : QRect(0, 0, dock->width(), content.top());
    if (title.isEmpty()) {
      return closed(out, "its title bar has no place");
    }
    out.area = globalRect(widget, title);
    return QString();
  }
  if ("action" == mark.part) {
    return placeAction(mark, widget, out);
  }
  if ("tab" == mark.part) {
    return placeTab(widget, out);
  }
  if ("point" == mark.part) {
    const CCanvas* canvas = qobject_cast<const CCanvas*>(widget);
    if (nullptr == canvas) {
      return "it is no map";
    }
    QPointF pixel = mark.point * DEG_TO_RAD;
    canvas->convertRad2Px(pixel);
    // A one pixel target: a cursor's hotspot or a disc's centre sits on the point.
    const QRect spot(pixel.toPoint(), QSize(1, 1));
    if (!canvas->rect().contains(spot)) {
      return closed(out, QString("its point %1, %2 is not on the map now").arg(mark.point.y()).arg(mark.point.x()));
    }
    out.area = globalRect(widget, spot);
    return QString();
  }
  if ("button" == mark.part) {
    const QAbstractItemView* view = CShotAddress::itemViewOf(widget);
    if (nullptr == view) {
      return "it is no list";
    }
    const QModelIndex& index = CShotAddress::resolveRowPath(*view->model(), mark.row);
    if (!index.isValid()) {
      return missing(out, QString("it has no row %1").arg(mark.row));
    }
    QString problem;
    const QRect& rect = CShotHandlers::rowButtonRect(view, index, mark.button, problem);
    if (!problem.isEmpty()) {
      return problem;
    }
    if (!rect.isValid() || !view->viewport()->rect().contains(rect)) {
      return closed(out, QString("its row %1 does not show its button %2 now").arg(mark.row, mark.button));
    }
    out.area = globalRect(view->viewport(), rect);
    return QString();
  }
  return QString("its part %1 is unknown").arg(mark.part);
}

QString placeOn(const CShotMarks::mark_t& mark, QWidget* shot, CShotMarks::placed_t& out) {
  QString problem;
  QWidget* root = rootOf(mark.root, shot, problem);
  if (nullptr == root) {
    // A menu or a window of its own is closed now; a docker named as root is not there.
    if ("menu" == mark.root || "window" == mark.root) {
      return closed(out, problem);
    }
    return mark.root.startsWith("dock:") ? missing(out, problem) : problem;
  }
  if ("splitter" == mark.part) {
    return placeSplitter(mark, root, out);
  }
  QWidget* widget = CShotAddress::resolveUnique(root, mark.widget, problem);
  if (nullptr == widget) {
    return notResolved(root, mark.widget, problem, out);
  }
  // A tab widget's pages other than the current one are hidden; their tabs are not.
  if ("tab" != mark.part && !widget->isVisibleTo(widget->window()) && widget != widget->window()) {
    return closed(out, "it is hidden");
  }
  out.window = widget->window();
  return placePart(mark, widget, out);
}

/** @return the outline of a mouse cursor, its hotspot at the origin */
QPainterPath cursorPath(const QString& kind, Qt::Orientation arrows) {
  QPainterPath path;
  if ("pointer" == kind) {
    path.addPolygon(QPolygonF({{0, 0}, {0, 17}, {4, 13}, {7, 20}, {10, 19}, {7, 12}, {12, 12}, {0, 0}}));
  } else if ("hand" == kind) {
    // A pointing hand: the index finger raised, the others bent beside it, the thumb out; the fingertip is the
    // hotspot. DejaVu's U+261D is an outline and fills to a blob.
    const auto part = [](qreal x, qreal y, qreal w, qreal h, qreal r) {
      QPainterPath p;
      p.addRoundedRect(QRectF(x, y, w, h), r, r);
      return p;
    };
    QPainterPath thumb;
    thumb.moveTo(-2, 13);
    thumb.lineTo(-6.5, 9.5);
    thumb.quadTo(-8.5, 9, -7.5, 11.5);
    thumb.lineTo(-2, 19);
    thumb.closeSubpath();
    path = part(-2, 0, 4, 14, 2)
               .united(part(2, 6, 4, 9, 2))
               .united(part(6, 7, 4, 8, 2))
               .united(part(10, 8.5, 3.5, 7, 1.75))
               .united(part(-2, 11, 15.5, 11, 3.5))
               .united(thumb);
  } else {
    path.addPolygon(QPolygonF(
        {{-11, 0}, {-5, -6}, {-5, -2}, {5, -2}, {5, -6}, {11, 0}, {5, 6}, {5, 2}, {-5, 2}, {-5, 6}, {-11, 0}}));
    if (Qt::Vertical == arrows) {
      path = QTransform().rotate(90).map(path);
    }
  }
  return path;
}

/** @return the point where the ray from @p area's centre to @p tail leaves it */
QPointF edgeToward(const QRectF& area, const QPointF& tail) {
  const QPointF c = area.center();
  const QPointF d = tail - c;
  const qreal sx = (0 == d.x()) ? 1e9 : (area.width() / 2) / qAbs(d.x());
  const qreal sy = (0 == d.y()) ? 1e9 : (area.height() / 2) / qAbs(d.y());
  return c + d * qMin(sx, sy);
}

struct drawing_t {
  QPainterPath halo;   /**< filled see-through red under the rest: a frame's band, a cursor's glow */
  QPainterPath fill;   /**< filled in red, see-through for a disc */
  QPainterPath stroke; /**< stroked in red */
  QPainterPath detail; /**< drawn light over the red: a hand's fingers */
  QRectF bounds;
};

drawing_t drawingOf(const CShotMarks::placed_t& placed, const QPoint& origin, const QRectF& fit = QRectF()) {
  const CShotMarks::mark_t& mark = placed.mark;
  const QRectF area = QRectF(placed.area.translated(-origin));
  const QString& shape = shapeOf(mark);
  drawing_t d;
  if ("frame" == shape) {
    // Around the target, the band from its edge outward; where the image ends first, along the image's edge.
    const qreal margin = kBand / 2;
    QRectF r = area.adjusted(-margin, -margin, margin, margin);
    // Only a target inside the image: one partly outside still fails.
    if (fit.isValid() && fit.contains(area)) {
      r = r.intersected(fit.adjusted(margin, margin, -margin, -margin));
    }
    d.stroke.addRoundedRect(r, 4, 4);
    QPainterPathStroker band;
    band.setWidth(kBand);
    band.setJoinStyle(Qt::RoundJoin);
    d.halo = band.createStroke(d.stroke);
  } else if ("disc" == shape) {
    const qreal radius = discRadius(area);
    d.fill.addEllipse(area.center(), radius, radius);
    // A hole where the target is: the knob stays visible.
    QPainterPath hole;
    hole.addEllipse(area.adjusted(-1, -1, 1, 1));
    d.fill = d.fill.subtracted(hole);
  } else if ("arrow" == shape) {
    const QPointF tail = area.center() + QPointF(CShotMarks::snap(mark.tail));
    const QPointF head = edgeToward(area, tail);
    const QLineF line(tail, head);
    const QPointF unit = (line.length() > 0) ? (head - tail) / line.length() : QPointF(1, 0);
    const QPointF normal(-unit.y(), unit.x());
    constexpr qreal length = 14;
    constexpr qreal width = 6;
    d.stroke.moveTo(tail);
    d.stroke.lineTo(head - unit * length);
    d.fill.addPolygon(
        QPolygonF({head, head - unit * length + normal * width, head - unit * length - normal * width, head}));
  } else {
    d.fill = cursorPath(mark.cursor, placed.arrows).translated(area.center());
    d.halo.addEllipse(d.fill.boundingRect().center(), kGlow, kGlow);
    if ("hand" == mark.cursor) {
      d.detail.moveTo(2, 8);
      d.detail.lineTo(2, 13);
      d.detail.moveTo(6, 9);
      d.detail.lineTo(6, 13);
      d.detail.moveTo(10, 10.5);
      d.detail.lineTo(10, 13);
      d.detail.translate(area.center());
    }
  }
  QPainterPathStroker stroker;
  stroker.setWidth(kLine + 2 * kOutline);
  d.bounds = d.fill.boundingRect().united(stroker.createStroke(d.stroke).boundingRect());
  if (!d.fill.isEmpty()) {
    d.bounds = d.bounds.united(d.fill.boundingRect().adjusted(-kOutline, -kOutline, kOutline, kOutline));
  }
  if (!d.halo.isEmpty()) {
    d.bounds = d.bounds.united(d.halo.boundingRect());
  }
  return d;
}
/** @brief Every mark in the one red, a disc see-through; each with the thin light outline. */
void draw(QPainter& painter, const drawing_t& d, const QString& shape) {
  if (!d.halo.isEmpty()) {
    painter.fillPath(d.halo, kDisc);
  }
  if (!d.stroke.isEmpty()) {
    painter.strokePath(d.stroke, QPen(kLight, kLine + 2 * kOutline, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.strokePath(d.stroke, QPen(kRed, kLine, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
  }
  if (!d.fill.isEmpty()) {
    painter.strokePath(d.fill, QPen(kLight, 2 * kOutline, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.fillPath(d.fill, "disc" == shape ? kDisc : kRed);
  }
  if (!d.detail.isEmpty()) {
    painter.strokePath(d.detail, QPen(kLight, 1.2, Qt::SolidLine, Qt::RoundCap));
  }
}
}  // namespace

QImage CShotMarks::icon(const QString& kind, qint32 side) {
  QImage image(side, side, QImage::Format_ARGB32_Premultiplied);
  image.fill(Qt::transparent);
  placed_t p;
  const qint32 c = side / 2;
  if ("frame" == kind || "disc" == kind) {
    p.mark.shape = kind;
    p.area = ("frame" == kind) ? QRect(8, 10, side - 16, side - 20) : QRect(c - 2, c - 2, 4, 4);
  } else if ("arrow" == kind) {
    // At a small target top left, from the bottom right.
    p.mark.shape = kind;
    p.area = QRect(2, 2, 6, 6);
    p.mark.tail = QPoint(side - 10, side - 10);
  } else {
    p.mark.shape = "cursor";
    p.mark.cursor = kind;
    // The hotspot where the cursor's figure sits centred.
    const QRectF figure = cursorPath(kind, Qt::Horizontal).boundingRect();
    p.area = QRect((QPointF(c, c) - figure.center()).toPoint(), QSize(1, 1));
  }
  // Shrunk to fit: a cursor's glow is wider than the button.
  const drawing_t d = drawingOf(p, QPoint());
  const qreal scale = qMin(1.0, (side - 2) / qMax(d.bounds.width(), d.bounds.height()));
  QPainter painter(&image);
  painter.setRenderHint(QPainter::Antialiasing);
  painter.translate(side / 2.0, side / 2.0);
  painter.scale(scale, scale);
  painter.translate(-d.bounds.center());
  draw(painter, d, shapeOf(p.mark));
  return image;
}

QRect CShotMarks::extent(const QList<placed_t>& placed, const QWidget* w) {
  const QPoint origin = w->mapToGlobal(QPoint(0, 0));
  QRectF all;
  for (const placed_t& p : placed) {
    all = all.united(drawingOf(p, origin).bounds);
  }
  return all.toAlignedRect();
}

QPoint CShotMarks::snap(const QPoint& tail) {
  if (tail.isNull()) {
    return tail;
  }
  const qreal length = std::hypot(qreal(tail.x()), qreal(tail.y()));
  const qreal angle = qRound(std::atan2(qreal(tail.y()), qreal(tail.x())) / (M_PI / 4)) * (M_PI / 4);
  return QPoint(qRound(std::cos(angle) * length), qRound(std::sin(angle) * length));
}

QString CShotMarks::describe(const mark_t& mark) {
  if (!mark.name.isEmpty()) {
    return mark.name;
  }
  const QString& shape = shapeOf(mark);
  QString target = ("splitter" == mark.part) ? mark.between.join(" | ") : mark.widget;
  if (target.isEmpty()) {
    target = mark.root.isEmpty() ? "main" : mark.root;
  }
  QString part = mark.part;
  if ("action" == mark.part) {
    part = mark.action;
  } else if ("branch" == mark.part) {
    part = "branch " + mark.row;
  } else if ("button" == mark.part) {
    part = QString("button %1 of row %2").arg(mark.button, mark.row);
  } else if ("point" == mark.part) {
    part = QString("point %1, %2").arg(mark.point.y()).arg(mark.point.x());
  }
  return part.isEmpty() || "splitter" == mark.part ? QString("%1: %2").arg(shape, target)
                                                   : QString("%1: %2 (%3)").arg(shape, target, part);
}

QString CShotMarks::parse(const QJsonArray& marks, QList<mark_t>& out) {
  out.clear();
  for (const QJsonValue& value : marks) {
    if (!value.isObject()) {
      return "a mark is an object with a widget, not a plain value";
    }
    const QJsonObject& object = value.toObject();
    for (const QString& key : object.keys()) {
      if (!kKeys.contains(key)) {
        return QString("a mark has the unknown key %1").arg(key);
      }
    }
    mark_t mark;
    mark.root = object["root"].toString("main");
    mark.widget = object["widget"].toString();
    mark.part = object["part"].toString();
    mark.row = object["row"].toString();
    mark.action = object["action"].toString();
    mark.button = object["button"].toString();
    for (const QJsonValue& side : object["between"].toArray()) {
      mark.between << side.toString();
    }
    mark.point = QPointF(object["lon"].toDouble(), object["lat"].toDouble());
    mark.shape = object["shape"].toString();
    const QJsonArray& tail = object["tail"].toArray();
    mark.tail = snap(QPoint(tail.at(0).toInt(), tail.at(1).toInt()));
    mark.cursor = object["cursor"].toString();
    mark.name = object["name"].toString();

    const QString& what = describe(mark);
    if (!mark.shape.isEmpty() && !QStringList({"frame", "disc", "arrow", "cursor"}).contains(mark.shape)) {
      return QString("%1 has the unknown shape %2").arg(what, mark.shape);
    }
    if ("splitter" == mark.part && mark.between.size() != 2) {
      return QString("%1 names %2 sides, not 2").arg(what).arg(mark.between.size());
    }
    if ("action" == mark.part && mark.action.isEmpty()) {
      return QString("%1 names no entry").arg(what);
    }
    if ("button" == mark.part && (mark.row.isEmpty() || mark.button.isEmpty())) {
      return QString("%1 names no row or no button").arg(what);
    }
    if ("branch" == mark.part && mark.row.isEmpty()) {
      return QString("%1 names no row").arg(what);
    }
    if ("point" == mark.part && (!object["lat"].isDouble() || !object["lon"].isDouble())) {
      return QString("%1 names no lat and lon").arg(what);
    }
    if ("point" != mark.part && (object.contains("lat") || object.contains("lon"))) {
      return QString("%1 has a lat or lon, which only a point takes").arg(what);
    }
    if ("arrow" == mark.shape && mark.tail.isNull()) {
      return QString("%1 has no tail").arg(what);
    }
    if ("cursor" == mark.shape && !QStringList({"pointer", "hand", "resize"}).contains(mark.cursor)) {
      return QString("%1 has the unknown cursor %2").arg(what, mark.cursor);
    }
    if ("resize" == mark.cursor && "splitter" != mark.part) {
      return QString("%1 is a resize cursor on something that is no splitter").arg(what);
    }
    out << mark;
  }
  return QString();
}

QString CShotMarks::place(const mark_t& mark, QWidget* shot, placed_t& out) {
  out = placed_t{mark};
  QString problem = placeOn(mark, shot, out);
  if (problem.isEmpty()) {
    out.found = found_e::eFound;
  }
  return problem;
}

QRect CShotMarks::sliderHandle(const QSlider& slider) { return CSliderAccess::handleOf(slider); }

bool CShotMarks::discFits(const QRect& area) {
  const qreal radius = discRadius(QRectF(area));
  const qint32 thickness = qMin(area.width(), area.height());
  return thickness <= kThin ||
         (thickness <= kSpot && area.width() / 2.0 + 1 <= radius && area.height() / 2.0 + 1 <= radius);
}

qint32 CShotMarks::dockTab(const QDockWidget* dock, const QTabBar*& bar) {
  bar = nullptr;
  const QWidget* main = (nullptr == dock) ? nullptr : dock->parentWidget();
  if (nullptr == main) {
    return -1;
  }
  // A tab of the main window's dock tab bars holds its docker as data.
  for (const QTabBar* candidate : main->findChildren<QTabBar*>(Qt::FindDirectChildrenOnly)) {
    if (!candidate->isVisible()) {
      continue;
    }
    for (qint32 i = 0; i < candidate->count(); i++) {
      if (candidate->tabData(i).value<quintptr>() == quintptr(dock)) {
        bar = candidate;
        return i;
      }
    }
  }
  return -1;
}

QDockWidget* CShotMarks::dockOfTab(const QTabBar* bar, qint32 index) {
  const quintptr data = bar->tabData(index).value<quintptr>();
  const QList<QDockWidget*>& docks = bar->parentWidget()->findChildren<QDockWidget*>();
  for (QDockWidget* dock : docks) {
    if (quintptr(dock) == data) {
      return dock;
    }
  }
  return nullptr;
}

QString CShotMarks::framed(const QImage& image, QWidget* w, const QRect& keep, const QList<placed_t>& placed,
                           const std::function<QImage()>& windowImage, QImage& out) {
  if (keep.isValid() && !image.rect().contains(keep)) {
    return "the part it keeps is not inside it";
  }
  // What the marks mark lies inside the part.
  const QRect kept = keep.isValid() ? keep : image.rect();
  const QRect part = kept.translated(w->mapToGlobal(QPoint(0, 0)));
  for (const placed_t& p : placed) {
    if (p.window != w->window() || !part.contains(p.area)) {
      return QString("it marks %1, which is not in the image").arg(describe(p.mark));
    }
  }
  // A part whose marks reach past it is cut from its window, with room for them.
  const QRect needed = kept.united(extent(placed, w));
  QWidget* window = w->window();
  if (window != w && !kept.contains(needed)) {
    QImage whole = windowImage();
    const QRect crop = needed.translated(w->mapTo(window, QPoint(0, 0))).intersected(whole.rect());
    if (const QString& problem = paint(whole, placed, window, crop); !problem.isEmpty()) {
      return problem;
    }
    out = whole.copy(crop);
    return QString();
  }
  out = image;
  if (const QString& problem = paint(out, placed, w, keep); !problem.isEmpty()) {
    return problem;
  }
  if (keep.isValid()) {
    out = out.copy(keep);
  }
  return QString();
}

QString CShotMarks::paint(QImage& image, const QList<placed_t>& placed, const QWidget* w, const QRect& keep,
                          QList<QRect>* drawn) {
  const QPoint origin = w->mapToGlobal(QPoint(0, 0));
  const QRect inside = keep.isValid() ? keep.intersected(image.rect()) : image.rect();
  QList<drawing_t> drawings;
  for (const placed_t& p : placed) {
    if (p.window != w->window()) {
      return QString("%1 lies in another window than the image").arg(describe(p.mark));
    }
    if ("disc" == shapeOf(p.mark) && !discFits(p.area)) {
      return QString("%1 is too big for a disc; mark it with a frame").arg(describe(p.mark));
    }
    const drawing_t d = drawingOf(p, origin, QRectF(inside));
    if (!QRectF(inside).contains(d.bounds)) {
      return QString("%1 is not completely inside the image").arg(describe(p.mark));
    }
    drawings << d;
  }

  QPainter painter(&image);
  painter.setRenderHint(QPainter::Antialiasing);
  for (qsizetype i = 0; i < drawings.size(); i++) {
    const drawing_t& d = drawings.at(i);
    draw(painter, d, shapeOf(placed.at(i).mark));
    if (nullptr != drawn) {
      *drawn << d.bounds.toAlignedRect();
    }
  }
  return QString();
}
