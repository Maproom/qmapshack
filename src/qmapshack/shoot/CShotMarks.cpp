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
#include <QRawFont>
#include <QSet>
#include <QSlider>
#include <QSplitter>
#include <QStyle>
#include <QStyleOptionSlider>
#include <QToolBar>
#include <QTreeView>
#include <QtMath>
#include <cmath>

#include "CMainWindow.h"
#include "shoot/CShotAddress.h"

namespace {
const QColor kRed(255, 0, 0, 200);
const QColor kDisc(255, 0, 0, 70);
const QColor kLight(255, 255, 255, 190);
constexpr qreal kLine = 3.0;
constexpr qreal kOutline = 2.0;

const QSet<QString> kKeys = {"root", "widget", "part", "row", "action", "between", "shape", "tail", "cursor", "name"};

/** Reaches QSlider's protected initStyleOption(), which the style needs for the handle's geometry. */
struct CSliderAccess : QSlider {
  static void styleOption(const QSlider& slider, QStyleOptionSlider* option) {
    void (QSlider::*init)(QStyleOptionSlider*) const = &CSliderAccess::initStyleOption;
    (slider.*init)(option);
  }
};

/** @return the shape drawn: a disc for a knob, a tree arrow or a splitter, else a frame, unless the mark names one */
QString shapeOf(const CShotMarks::mark_t& mark) {
  if (!mark.shape.isEmpty()) {
    return mark.shape;
  }
  return ("handle" == mark.part || "branch" == mark.part || "splitter" == mark.part) ? "disc" : "frame";
}

QRect globalRect(const QWidget* widget, const QRect& local) {
  return local.translated(widget->mapToGlobal(QPoint(0, 0)));
}

QRect globalRect(const QWidget* widget) { return globalRect(widget, widget->rect()); }

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
      return problem;
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
      return "the handle between them is hidden";
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

QString placeAction(const CShotMarks::mark_t& mark, QWidget* widget, QRect& area) {
  QAction* action = nullptr;
  const QList<QAction*>& actions = widget->actions();
  for (QAction* entry : actions) {
    if (entry->objectName() == mark.action) {
      action = entry;
    }
  }
  if (nullptr == action) {
    return QString("it has no entry %1").arg(mark.action);
  }
  if (!action->isVisible()) {
    return QString("its entry %1 is hidden").arg(mark.action);
  }
  if (const QMenu* menu = qobject_cast<const QMenu*>(widget); nullptr != menu) {
    area = globalRect(widget, menu->actionGeometry(action));
  } else if (const QMenuBar* bar = qobject_cast<const QMenuBar*>(widget); nullptr != bar) {
    area = globalRect(widget, bar->actionGeometry(action));
  } else if (const QToolBar* toolbar = qobject_cast<const QToolBar*>(widget); nullptr != toolbar) {
    QWidget* button = toolbar->widgetForAction(action);
    if (nullptr == button || !button->isVisibleTo(toolbar)) {
      return QString("its entry %1 is not shown").arg(mark.action);
    }
    area = globalRect(button);
  } else {
    return "it is no menu or toolbar";
  }
  return area.isEmpty() ? QString("its entry %1 has no place").arg(mark.action) : QString();
}

QString placePart(const CShotMarks::mark_t& mark, QWidget* widget, QRect& area) {
  if (mark.part.isEmpty()) {
    area = globalRect(widget);
    return QString();
  }
  if ("handle" == mark.part) {
    const QSlider* slider = qobject_cast<const QSlider*>(widget);
    if (nullptr == slider) {
      return "it is no slider";
    }
    QStyleOptionSlider option;
    CSliderAccess::styleOption(*slider, &option);
    area = globalRect(widget,
                      slider->style()->subControlRect(QStyle::CC_Slider, &option, QStyle::SC_SliderHandle, slider));
    return QString();
  }
  if ("branch" == mark.part) {
    const QTreeView* tree = qobject_cast<const QTreeView*>(CShotAddress::itemViewOf(widget));
    if (nullptr == tree) {
      return "it is no tree";
    }
    const QModelIndex& index = CShotAddress::resolveRowPath(*tree->model(), mark.row);
    if (!index.isValid()) {
      return QString("it has no row %1").arg(mark.row);
    }
    // The branch indicator sits in the indentation left of the row's first cell.
    const QRect cell = tree->visualRect(index.siblingAtColumn(0));
    area = globalRect(tree->viewport(),
                      QRect(cell.left() - tree->indentation(), cell.top(), tree->indentation(), cell.height()));
    return QString();
  }
  if ("title" == mark.part) {
    const QDockWidget* dock = qobject_cast<const QDockWidget*>(widget);
    if (nullptr == dock) {
      return "it is no docker";
    }
    if (const QWidget* bar = dock->titleBarWidget(); nullptr != bar) {
      area = globalRect(bar);
      return QString();
    }
    // The dock's own title is what its content leaves free: a strip on the left or the top.
    const QRect content = (nullptr == dock->widget()) ? QRect() : dock->widget()->geometry();
    const QRect title = dock->features().testFlag(QDockWidget::DockWidgetVerticalTitleBar)
                            ? QRect(0, 0, content.left(), dock->height())
                            : QRect(0, 0, dock->width(), content.top());
    if (title.isEmpty()) {
      return "its title bar has no place";
    }
    area = globalRect(widget, title);
    return QString();
  }
  if ("action" == mark.part) {
    return placeAction(mark, widget, area);
  }
  return QString("its part %1 is unknown").arg(mark.part);
}

/** @return the outline of a mouse cursor, its hotspot at the origin */
QPainterPath cursorPath(const QString& kind, Qt::Orientation arrows) {
  QPainterPath path;
  if ("pointer" == kind) {
    path.addPolygon(QPolygonF({{0, 0}, {0, 17}, {4, 13}, {7, 20}, {10, 19}, {7, 12}, {12, 12}, {0, 0}}));
  } else if ("hand" == kind) {
    // The bundled DejaVu Sans' pointing index, read from the file: font matching takes it from another font or
    // none. Its tip is the hotspot.
    const QRawFont font(QString(":/fonts/DejaVuSans.ttf"), 28);
    path = font.pathForGlyph(font.glyphIndexesForString(QString(QChar(0x261D))).value(0));
    QPointF tip(0, 1e9);
    for (qint32 i = 0; i < path.elementCount(); i++) {
      const QPointF p = path.elementAt(i);
      if (p.y() < tip.y()) {
        tip = p;
      }
    }
    path.translate(-tip);
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
  QPainterPath fill;   /**< filled in red, or black for a cursor */
  QPainterPath stroke; /**< stroked in red */
  QRectF bounds;
};

drawing_t drawingOf(const CShotMarks::placed_t& placed, const QPoint& origin) {
  const CShotMarks::mark_t& mark = placed.mark;
  const QRectF area = QRectF(placed.area.translated(-origin));
  const QString& shape = shapeOf(mark);
  drawing_t d;
  const qreal margin = kLine / 2 + kOutline;
  if ("frame" == shape) {
    // Inside the target, so a frame on the photographed widget fits the image; a thin target is framed around.
    QRectF r = area.adjusted(margin, margin, -margin, -margin);
    constexpr qreal least = 6;
    if (r.width() < least) {
      r.adjust(-(least - r.width()) / 2 - margin, 0, (least - r.width()) / 2 + margin, 0);
    }
    if (r.height() < least) {
      r.adjust(0, -(least - r.height()) / 2 - margin, 0, (least - r.height()) / 2 + margin);
    }
    d.stroke.addRoundedRect(r, 4, 4);
  } else if ("disc" == shape) {
    // By the shorter side: a disc on a long, thin target stays a spot.
    const qreal radius = qMax(14.0, qMin(area.width(), area.height()) / 2.0 + 6.0);
    d.fill.addEllipse(area.center(), radius, radius);
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
  }
  QPainterPathStroker stroker;
  stroker.setWidth(kLine + 2 * kOutline);
  d.bounds = d.fill.boundingRect().united(stroker.createStroke(d.stroke).boundingRect());
  if (!d.fill.isEmpty()) {
    d.bounds = d.bounds.united(d.fill.boundingRect().adjusted(-kOutline, -kOutline, kOutline, kOutline));
  }
  return d;
}
}  // namespace

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
  }
  return part.isEmpty() || "splitter" == mark.part ? QString("%1: %2").arg(shape, target)
                                                   : QString("%1: %2 (%3)").arg(shape, target, part);
}

QString CShotMarks::parse(const QJsonArray& marks, QList<mark_t>& out) {
  out.clear();
  for (const QJsonValue& value : marks) {
    if (value.isString()) {
      out << mark_t{"main", value.toString()};
      continue;
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
    for (const QJsonValue& side : object["between"].toArray()) {
      mark.between << side.toString();
    }
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
    if ("branch" == mark.part && mark.row.isEmpty()) {
      return QString("%1 names no row").arg(what);
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
  QString problem;
  QWidget* root = rootOf(mark.root, shot, problem);
  if (nullptr == root) {
    return problem;
  }
  if ("splitter" == mark.part) {
    return placeSplitter(mark, root, out);
  }
  QWidget* widget = CShotAddress::resolveUnique(root, mark.widget, problem);
  if (nullptr == widget) {
    return problem;
  }
  if (!widget->isVisibleTo(widget->window()) && widget != widget->window()) {
    return "it is hidden";
  }
  out.window = widget->window();
  return placePart(mark, widget, out.area);
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
    const drawing_t d = drawingOf(p, origin);
    if (!QRectF(inside).contains(d.bounds)) {
      return QString("%1 is not completely inside the image").arg(describe(p.mark));
    }
    drawings << d;
  }

  QPainter painter(&image);
  painter.setRenderHint(QPainter::Antialiasing);
  for (qsizetype i = 0; i < drawings.size(); i++) {
    const drawing_t& d = drawings.at(i);
    const QString& shape = shapeOf(placed.at(i).mark);
    if (!d.stroke.isEmpty()) {
      painter.strokePath(d.stroke, QPen(kLight, kLine + 2 * kOutline, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
      painter.strokePath(d.stroke, QPen(kRed, kLine, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    }
    if (!d.fill.isEmpty()) {
      painter.strokePath(d.fill, QPen(kLight, 2 * kOutline, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
      painter.fillPath(d.fill, "cursor" == shape ? QColor(Qt::black) : ("disc" == shape ? kDisc : kRed));
    }
    if (nullptr != drawn) {
      *drawn << d.bounds.toAlignedRect();
    }
  }
  return QString();
}
