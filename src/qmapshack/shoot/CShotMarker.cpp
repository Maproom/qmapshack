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

#include "shoot/CShotMarker.h"

#include <QAbstractItemView>
#include <QApplication>
#include <QCursor>
#include <QDockWidget>
#include <QImage>
#include <QJsonDocument>
#include <QKeyEvent>
#include <QLineEdit>
#include <QMainWindow>
#include <QMenu>
#include <QMouseEvent>
#include <QPainter>
#include <QRegularExpression>
#include <QSlider>
#include <QSplitter>
#include <QTabBar>
#include <QTabWidget>
#include <QTextDocumentFragment>
#include <QTimer>
#include <QToolBar>
#include <QToolButton>
#include <QToolTip>
#include <QTreeView>
#include <QTreeWidget>

#include "CMainWindow.h"
#include "canvas/CCanvas.h"
#include "gis/proj_x.h"
#include "shoot/CShotAddress.h"
#include "shoot/CShotDocMode.h"
#include "shoot/CShotHandlers.h"
#include "shoot/CShotMarks.h"

namespace {
constexpr qint32 kRefreshMs = 300;
/** How long a hint stays at the pointer [ms]. */
constexpr qint32 kHintMs = 6000;
constexpr Qt::Key kMarkKey = Qt::Key_F10;
constexpr Qt::KeyboardModifiers kMarkModifiers = Qt::ControlModifier | Qt::ShiftModifier;

/** @brief Draws the selected image's marks over a window; mouse events pass through it. */
class CShotMarkOverlay : public QWidget {
 public:
  explicit CShotMarkOverlay(QWidget* window) : QWidget(window) {
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setAttribute(Qt::WA_NoSystemBackground);
    setFocusPolicy(Qt::NoFocus);
  }

  void show(const QList<CShotMarks::placed_t>& marks, const QRect& candidate) {
    this->marks = marks;
    this->candidate = candidate;
    setGeometry(parentWidget()->rect());
    raise();
    update();
  }

 protected:
  void paintEvent(QPaintEvent*) override {
    QImage image(size(), QImage::Format_ARGB32_Premultiplied);
    image.fill(Qt::transparent);
    // One at a time: a mark that does not fit is left out, the others are drawn.
    for (const CShotMarks::placed_t& mark : std::as_const(marks)) {
      CShotMarks::paint(image, {mark}, this, QRect());
    }
    QPainter p(this);
    p.drawImage(0, 0, image);
    if (candidate.isValid()) {
      const QRect r(mapFromGlobal(candidate.topLeft()), candidate.size());
      p.setPen(QPen(QColor(0, 120, 215), 2, Qt::DashLine));
      p.setBrush(QColor(0, 120, 215, 40));
      p.drawRect(r.adjusted(1, 1, -1, -1));
    }
  }

 private:
  QList<CShotMarks::placed_t> marks;
  QRect candidate;
};

QWidget* mainWindow() { return CMainWindow::isNull() ? nullptr : &CMainWindow::self(); }

/** @return what the writer sees @p w called, else what it is and where */
/** @return the first sentence of the tooltip @p tip, plain text without its closing stop */
QString firstSentence(const QString& tip) {
  QString text = Qt::mightBeRichText(tip) ? QTextDocumentFragment::fromHtml(tip).toPlainText() : tip;
  text = text.section('\n', 0, 0).trimmed();
  if (const qsizetype stop = text.indexOf(". "); stop > 0) {
    text.truncate(stop);
  }
  while (text.endsWith('.') || text.endsWith(':')) {
    text.chop(1);
  }
  return text.trimmed();
}

QString nameOf(const QWidget* w) {
  const QString& name = CShotDocMode::onScreenName(w);
  if (!name.isEmpty()) {
    return name;
  }
  if (const QAbstractButton* button = qobject_cast<const QAbstractButton*>(w); nullptr != button) {
    // Designer gives a tool button the text "..." until it has an icon.
    static const QRegularExpression placeholder(R"(^[.\s]*$)");
    // A copy: remove() returns a reference into the temporary.
    const QString text = QString(button->text()).remove('&');
    if (!text.contains(placeholder)) {
      return text;
    }
    if (const QString& tip = firstSentence(button->toolTip()); !tip.isEmpty()) {
      return tip;
    }
  }
  if (const QMainWindow* main = qobject_cast<const QMainWindow*>(mainWindow()); nullptr != main) {
    if (w == main->centralWidget()) {
      return "the map";
    }
  }
  // Named by the docker, tab or group it sits in.
  QString where;
  for (const QWidget* up = w->parentWidget(); nullptr != up && where.isEmpty(); up = up->parentWidget()) {
    where = CShotDocMode::onScreenName(up);
  }
  QString what = "the " + QString::fromLatin1(w->metaObject()->className());
  if (nullptr != qobject_cast<const QAbstractItemView*>(w)) {
    what = "the list";
  } else if (nullptr != qobject_cast<const QAbstractSlider*>(w)) {
    what = "the slider";
  } else if (nullptr != qobject_cast<const QLineEdit*>(w)) {
    what = "the text field";
  } else if (const QDockWidget* dock = qobject_cast<const QDockWidget*>(w->parentWidget());
             nullptr != dock && dock->widget() == w) {
    what = "the docker's content";
  }
  return where.isEmpty() ? what : QString("%1 in %2").arg(what, where);
}

/** @return the text of row @p index, else its path as a step names it: workspace rows paint their name themselves */
QString rowNameOf(const QAbstractItemView* view, const QModelIndex& index) {
  const QString& text = index.siblingAtColumn(0).data().toString();
  if (!text.isEmpty()) {
    return text;
  }
  const QTreeWidget* tree = qobject_cast<const QTreeWidget*>(view);
  const QTreeWidgetItem* item = (nullptr == tree) ? nullptr : tree->itemAt(view->visualRect(index).center());
  const QString& path = CShotAddress::itemPathOf(item);
  return path.isEmpty() ? CShotAddress::namePathOf(item) : path;
}

/** @return the docker or central widget of @p main at @p p, skipping tab bars toward @p step; nullptr for none */
QWidget* neighbourAt(QWidget* main, QPoint p, const QPoint& step) {
  const QMainWindow* window = qobject_cast<const QMainWindow*>(main);
  for (qint32 i = 0; i < 200 && main->rect().contains(p); i++, p += step) {
    QWidget* w = main->childAt(p);
    while (nullptr != w && w->parentWidget() != main) {
      w = w->parentWidget();
    }
    if (nullptr == w || qobject_cast<QTabBar*>(w) != nullptr || window->isSeparator(p)) {
      continue;
    }
    if (qobject_cast<QDockWidget*>(w) != nullptr || w == window->centralWidget()) {
      return w;
    }
    return nullptr;
  }
  return nullptr;
}

/** @return the status a mark of the selected image shows in the panel */
QString statusOf(const QString& problem, const CShotMarks::placed_t& placed) {
  switch (placed.found) {
    case CShotMarks::found_e::eFound: {
      const QWidget* window = placed.window;
      const bool onScreen =
          nullptr != window && window->isVisible() &&
          window->rect().contains(QRect(window->mapFromGlobal(placed.area.topLeft()), placed.area.size()));
      return onScreen ? "shown" : "not on screen";
    }
    case CShotMarks::found_e::eClosed:
      return "not on screen";
    case CShotMarks::found_e::eMissing:
      return "not found";
    case CShotMarks::found_e::eWrong:
      break;
  }
  return "fails: " + problem;
}
}  // namespace

CShotMarker::CShotMarker(std::function<void(const QString&)> send, std::function<void(const QString&)> report,
                         QObject* parent)
    : QObject(parent), send(std::move(send)), report(std::move(report)) {
  timer = new QTimer(this);
  timer->setInterval(kRefreshMs);
  connect(timer, &QTimer::timeout, this, &CShotMarker::refresh);
  timer->start();
}

CShotMarker::~CShotMarker() {
  // No refresh: it would draw the marks again.
  if (eIdle != mode || !candidates.isEmpty()) {
    qApp->removeEventFilter(this);
  }
  delete overlay.data();
}

void CShotMarker::setMarker(const QString& kind) {
  marker = kind;
  if (eIdle != mode) {
    cancel();
  }
}

void CShotMarker::setMarks(const QJsonArray& marks) {
  this->marks = marks;
  if (const QString& problem = CShotMarks::parse(marks, selectedMarks); !problem.isEmpty()) {
    report(QString("The selected image's marks cannot be read: %1.").arg(problem));
  }
  statuses.clear();
  refresh();
}

void CShotMarker::setShown(bool on) {
  shown = on;
  refresh();
}

void CShotMarker::setHidden(bool on) {
  hidden = on;
  if (on && !overlay.isNull()) {
    overlay->hide();
  }
  if (!on) {
    refresh();
  }
}

QJsonObject CShotMarker::shaped(const QJsonObject& mark) const {
  QJsonObject out = mark;
  if ("frame" == marker || "disc" == marker || "arrow" == marker) {
    out["shape"] = marker;
  } else {
    out["shape"] = "cursor";
    out["cursor"] = marker;
  }
  return out;
}

void CShotMarker::refuse(const QString& why) {
  report(why);
  hint(why);
}

void CShotMarker::hint(const QString& text) {
  // The writer looks at the pointer, not at the panel's status line. A plain tooltip does not wrap.
  QStringList lines;
  for (const QString& word : text.split(' ', Qt::SkipEmptyParts)) {
    if (lines.isEmpty() || lines.last().size() + 1 + word.size() > kHintColumns) {
      lines << word;
    } else {
      lines.last() += ' ' + word;
    }
  }
  QToolTip::showText(QCursor::pos(), lines.join('\n'), nullptr, QRect(), kHintMs);
}

void CShotMarker::markAtPointer(const QString& row) {
  if (eChoosing == mode) {
    step();
    return;
  }
  if (eTail == mode) {
    return;
  }
  if (marker.isEmpty()) {
    refuse("Pick a marker first.");
    return;
  }
  QWidget* under = QApplication::widgetAt(QCursor::pos());
  pointedWindow = (nullptr == under) ? nullptr : under->window();
  QList<candidate_t> found = candidatesAt(QCursor::pos());
  if ("resize" == marker) {
    found.erase(std::remove_if(found.begin(), found.end(),
                               [](const candidate_t& c) { return "splitter" != c.mark["part"].toString(); }),
                found.end());
    if (found.isEmpty()) {
      refuse("A resize cursor goes on a splitter; point at one.");
      return;
    }
  }
  if ("disc" == marker && !found.isEmpty()) {
    found.erase(
        std::remove_if(found.begin(), found.end(), [](const candidate_t& c) { return !CShotMarks::discFits(c.area); }),
        found.end());
    if (found.isEmpty()) {
      refuse("A disc goes on something small, a knob or the arrow of a list entry; mark this with a frame.");
      return;
    }
  }
  if (found.isEmpty()) {
    refuse("Nothing under the pointer can be marked.");
    return;
  }
  this->row = row;
  candidates = found;
  current = 0;
  qApp->installEventFilter(this);
  if (1 == candidates.size()) {
    accept();
    return;
  }
  mode = eChoosing;
  send("marking 1");
  refresh();
  report(QString("Mark %1 (1 of %2)? Ctrl+Shift+F10 for the next, a click or Return takes it, Escape cancels.")
             .arg(candidates.first().name)
             .arg(candidates.size()));
}

void CShotMarker::step() {
  if (candidates.isEmpty()) {
    return;
  }
  current = (current + 1) % candidates.size();
  refresh();
  report(QString("Mark %1 (%2 of %3)? Ctrl+Shift+F10 for the next, a click or Return takes it, Escape cancels.")
             .arg(candidates.at(current).name)
             .arg(current + 1)
             .arg(candidates.size()));
}

void CShotMarker::accept() {
  const candidate_t& chosen = candidates.at(current);
  QJsonObject mark = shaped(chosen.mark);
  mark["name"] = chosen.name;
  if ("arrow" == marker) {
    head = mark;
    if (eIdle == mode) {
      send("marking 1");
    }
    mode = eTail;
    refresh();
    report(QString("Click where the arrow to %1 starts. Escape cancels.").arg(chosen.name));
    return;
  }
  finish(mark);
}

void CShotMarker::finish(const QJsonObject& mark) {
  // Read before leaving: that forgets the window pointed at.
  QWidget* window = windowShot();
  leaveMode();
  QList<CShotMarks::mark_t> parsed;
  QString problem = CShotMarks::parse(QJsonArray{mark}, parsed);
  CShotMarks::placed_t placed;
  if (problem.isEmpty()) {
    problem = CShotMarks::place(parsed.first(), window, placed);
  }
  if (!problem.isEmpty()) {
    refuse(QString("The mark cannot be placed: %1.").arg(problem));
    return;
  }
  send(QString("mark-placed %1\t%2").arg(row, QString::fromUtf8(QJsonDocument(mark).toJson(QJsonDocument::Compact))));
  if (!inImage(placed)) {
    hint(QString("%1 is marked, but it is not in this image. Take the image again with what it should show.")
             .arg(mark["name"].toString()));
  }
}

void CShotMarker::confirm() {
  if (eChoosing == mode) {
    accept();
  }
}

void CShotMarker::cancel(const QString& why) {
  if (eIdle == mode) {
    return;
  }
  leaveMode();
  report(why.isEmpty() ? QString("No mark was placed.") : why);
}

void CShotMarker::leaveMode() {
  if (eIdle != mode || !candidates.isEmpty()) {
    qApp->removeEventFilter(this);
  }
  if (eIdle != mode) {
    send("marking 0");
  }
  mode = eIdle;
  candidates.clear();
  head = QJsonObject();
  pointedWindow = nullptr;
  refresh();
}

bool CShotMarker::eventFilter(QObject* watched, QEvent* event) {
  if (eIdle == mode) {
    return QObject::eventFilter(watched, event);
  }
  switch (event->type()) {
    case QEvent::KeyPress: {
      const QKeyEvent* key = static_cast<QKeyEvent*>(event);
      if (kMarkKey == key->key() && kMarkModifiers == (key->modifiers() & kMarkModifiers)) {
        if (eChoosing == mode && !key->isAutoRepeat()) {
          step();
        }
        return true;
      }
      if (Qt::Key_Escape == key->key()) {
        cancel();
        return true;
      }
      if ((Qt::Key_Return == key->key() || Qt::Key_Enter == key->key()) && eChoosing == mode) {
        accept();
        return true;
      }
      // The rest would act on an open menu, or on what the pointer is on.
      return true;
    }
    case QEvent::KeyRelease:
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonDblClick:
      return true;
    case QEvent::MouseButtonRelease: {
      if (eChoosing == mode) {
        accept();
      } else if (eTail == mode) {
        // Placed as an arrow with any tail, for its target's centre.
        QJsonObject probe = head;
        probe["tail"] = QJsonArray{1, 0};
        QList<CShotMarks::mark_t> parsed;
        CShotMarks::placed_t placed;
        if (CShotMarks::parse(QJsonArray{probe}, parsed).isEmpty() &&
            CShotMarks::place(parsed.first(), windowShot(), placed).isEmpty()) {
          const QPoint tail =
              CShotMarks::snap(static_cast<QMouseEvent*>(event)->globalPosition().toPoint() - placed.area.center());
          if (placed.area.contains(placed.area.center() + tail)) {
            refuse("The arrow has to start outside what it points at; click further away. Escape cancels.");
            return true;
          }
          QJsonObject mark = head;
          mark["tail"] = QJsonArray{tail.x(), tail.y()};
          finish(mark);
        } else {
          cancel("What the arrow points at is gone; no mark was placed.");
        }
      }
      return true;
    }
    default:
      return QObject::eventFilter(watched, event);
  }
}

QList<CShotMarker::candidate_t> CShotMarker::candidatesAt(const QPoint& global) const {
  QList<candidate_t> out;
  QWidget* main = mainWindow();
  if (nullptr == main) {
    return out;
  }
  // Placed on the window pointed at: a `window` mark is found in it.
  QWidget* under = QApplication::widgetAt(global);
  QWidget* shotOn = (nullptr == under) ? main : under->window();
  const auto add = [&out, shotOn](const QJsonObject& mark, const QString& name) {
    QList<CShotMarks::mark_t> parsed;
    CShotMarks::placed_t placed;
    if (CShotMarks::parse(QJsonArray{mark}, parsed).isEmpty() &&
        CShotMarks::place(parsed.first(), shotOn, placed).isEmpty()) {
      out << candidate_t{mark, name, placed.area};
    }
  };

  // An open menu: its entry, or the whole menu.
  if (QMenu* menu = qobject_cast<QMenu*>(QApplication::activePopupWidget()); nullptr != menu) {
    if (menu->geometry().contains(global)) {
      if (const QAction* action = menu->actionAt(menu->mapFromGlobal(global));
          nullptr != action && !action->objectName().isEmpty()) {
        add({{"root", "menu"}, {"part", "action"}, {"action", action->objectName()}},
            QString("the entry \"%1\"").arg(QString(action->text()).remove('&')));
      }
      add({{"root", "menu"}}, "the whole menu");
    }
    return out;
  }

  // A separator of the main window: between its neighbours on either side.
  const QMainWindow* window = qobject_cast<const QMainWindow*>(main);
  if (const QPoint local = main->mapFromGlobal(global); main->rect().contains(local) && window->isSeparator(local)) {
    // Side by side when the separator runs on above and below the pointer.
    const bool upright = window->isSeparator(local + QPoint(0, 8)) || window->isSeparator(local - QPoint(0, 8));
    const QPoint step = upright ? QPoint(1, 0) : QPoint(0, 1);
    QWidget* a = neighbourAt(main, local - step, -step);
    QWidget* b = neighbourAt(main, local + step, step);
    if (nullptr != a && nullptr != b) {
      add({{"root", "main"},
           {"part", "splitter"},
           {"between", QJsonArray{CShotAddress::addressOf(main, a).value_or(QString()),
                                  CShotAddress::addressOf(main, b).value_or(QString())}}},
          QString("the separator between %1 and %2").arg(nameOf(a), nameOf(b)));
    }
    return out;
  }

  QWidget* w = QApplication::widgetAt(global);
  if (nullptr == w) {
    return out;
  }
  // The root the address starts from: the main window, a floating docker, or the window of its own.
  QWidget* top = w->window();
  QString root = "main";
  if (top != main) {
    if (const QDockWidget* dock = qobject_cast<const QDockWidget*>(top); nullptr != dock) {
      root = "dock:" + CShotAddress::addressOf(main, top).value_or(QString());
    } else {
      root = "window";
    }
  }
  const auto address = [top](const QWidget* x) { return CShotAddress::addressOf(top, x).value_or(QString()); };

  // A QSplitter's handle: between the widgets on either side.
  if (const QSplitterHandle* handle = qobject_cast<const QSplitterHandle*>(w); nullptr != handle) {
    const QSplitter* splitter = handle->splitter();
    const qint32 index = splitter->indexOf(const_cast<QSplitterHandle*>(handle));
    if (index > 0) {
      add({{"root", root},
           {"part", "splitter"},
           {"between", QJsonArray{address(splitter->widget(index - 1)), address(splitter->widget(index))}}},
          QString("the splitter between %1 and %2")
              .arg(nameOf(splitter->widget(index - 1)), nameOf(splitter->widget(index))));
    }
  }
  // A place on the map: the point under the pointer, rounded to about 0.1 m.
  if (const CCanvas* canvas = qobject_cast<const CCanvas*>(w); nullptr != canvas) {
    QPointF pos = canvas->mapFromGlobal(QPointF(global));
    canvas->convertPx2Rad(pos);
    pos /= DEG_TO_RAD;
    const qreal lat = qRound64(pos.y() * 1e6) / 1e6;
    const qreal lon = qRound64(pos.x() * 1e6) / 1e6;
    add({{"root", root}, {"widget", address(canvas)}, {"part", "point"}, {"lat", lat}, {"lon", lon}},
        QString("the place %1, %2 on the map").arg(lat, 0, 'f', 5).arg(lon, 0, 'f', 5));
  }
  // A slider's knob.
  if (const QSlider* slider = qobject_cast<const QSlider*>(w); nullptr != slider) {
    if (CShotMarks::sliderHandle(*slider).contains(slider->mapFromGlobal(global))) {
      add({{"root", root}, {"widget", address(slider)}, {"part", "handle"}},
          QString("the knob of %1").arg(nameOf(slider)));
    }
  }
  // A tab: a docker's in the main window's tab bars, else a tab widget's page.
  if (const QTabBar* bar = qobject_cast<const QTabBar*>(w); nullptr != bar) {
    const qint32 i = bar->tabAt(bar->mapFromGlobal(global));
    QWidget* page = nullptr;
    if (const QTabWidget* tabs = qobject_cast<const QTabWidget*>(bar->parentWidget());
        nullptr != tabs && tabs->tabBar() == bar) {
      page = tabs->widget(i);
    } else if (nullptr != qobject_cast<const QMainWindow*>(bar->parentWidget()) && i >= 0) {
      page = CShotMarks::dockOfTab(bar, i);
    }
    if (nullptr != page) {
      add({{"root", root}, {"widget", address(page)}, {"part", "tab"}},
          QString("the tab \"%1\"").arg(bar->tabText(i).remove('&')));
    }
  }
  // A button a list's delegate paints into a row.
  if (const QAbstractItemView* view = CShotAddress::itemViewOf(w); nullptr != view && view->viewport() == w) {
    const QPoint at = w->mapFromGlobal(global);
    const QModelIndex& index = view->indexAt(at);
    if (index.isValid()) {
      if (const QString& button = CShotHandlers::rowButtonAt(view, index, at); !button.isEmpty()) {
        add({{"root", root},
             {"widget", address(view)},
             {"part", "button"},
             {"row", CShotAddress::rowPathOf(index)},
             {"button", button}},
            QString("the button %1 of \"%2\"").arg(button, rowNameOf(view, index)));
      }
    }
  }
  // A tree's expand arrow.
  if (const QTreeView* tree = qobject_cast<const QTreeView*>(w->parentWidget());
      nullptr != tree && tree->viewport() == w) {
    const QPoint at = tree->viewport()->mapFromGlobal(global);
    const QModelIndex& index = tree->indexAt(at);
    if (index.isValid()) {
      const QRect cell = tree->visualRect(index.siblingAtColumn(0));
      if (at.x() < cell.left() && at.x() >= cell.left() - tree->indentation()) {
        add({{"root", root}, {"widget", address(tree)}, {"part", "branch"}, {"row", CShotAddress::rowPathOf(index)}},
            QString("the arrow of \"%1\"").arg(rowNameOf(tree, index)));
      }
    }
  }
  // A docker's title bar: the docker itself, or one of its title buttons.
  for (QWidget* x = w; nullptr != x && x != top->parentWidget(); x = x->parentWidget()) {
    if (const QDockWidget* dock = qobject_cast<const QDockWidget*>(x); nullptr != dock) {
      const bool onTitle = (x == w) || w->objectName().startsWith("qt_dockwidget_");
      if (onTitle) {
        add({{"root", "main"}, {"widget", CShotAddress::addressOf(main, dock).value_or(QString())}, {"part", "title"}},
            QString("the title bar of %1").arg(nameOf(dock)));
      }
      break;
    }
  }
  // A toolbar's action.
  if (const QToolButton* button = qobject_cast<const QToolButton*>(w); nullptr != button) {
    if (const QToolBar* bar = qobject_cast<const QToolBar*>(button->parentWidget()); nullptr != bar) {
      if (const QAction* action = button->defaultAction(); nullptr != action && !action->objectName().isEmpty()) {
        add({{"root", root}, {"widget", address(bar)}, {"part", "action"}, {"action", action->objectName()}},
            QString("the tool \"%1\"").arg(QString(action->text()).remove('&')));
      }
    }
  }
  // The widget and the ones around it that a shot can find again.
  qint32 wholes = 0;
  for (QWidget* x = w; nullptr != x && wholes < 3; x = x->parentWidget()) {
    if (x->objectName().startsWith("qt_")) {
      continue;
    }
    const QString& at = address(x);
    if (at.isEmpty() && x != top) {
      continue;
    }
    add({{"root", root}, {"widget", at}}, nameOf(x));
    wholes++;
    if (x == top) {
      break;
    }
  }
  return out;
}

QWidget* CShotMarker::windowShot() const {
  if (QWidget* modal = QApplication::activeModalWidget(); nullptr != modal) {
    return modal;
  }
  if (!pointedWindow.isNull()) {
    return pointedWindow;
  }
  // The selected image's window, named by its class.
  if (const QString& name = shot["window"].toString(); !name.isEmpty()) {
    for (QWidget* window : QApplication::topLevelWidgets()) {
      if (window->isVisible() && name == window->metaObject()->className()) {
        return window;
      }
    }
  }
  QWidget* active = QApplication::activeWindow();
  return (nullptr == active || nullptr != qobject_cast<QMenu*>(active)) ? mainWindow() : active;
}

bool CShotMarker::inImage(const CShotMarks::placed_t& placed) const {
  // A row without a shot, or an exposure built only when taken, has no image on screen to compare with.
  if (!shot.contains("widget")) {
    return true;
  }
  QWidget* main = mainWindow();
  const QString& address = shot["widget"].toString();
  QWidget* image = address.isEmpty() ? main : CShotAddress::resolve(main, address);
  // A window a scenario opens is named by its class.
  if (shot.contains("window")) {
    image = (nullptr != placed.window && shot["window"].toString() == placed.window->metaObject()->className())
                ? placed.window
                : nullptr;
    if (nullptr == image) {
      return false;
    }
  }
  if (nullptr == image) {
    return true;
  }
  if (placed.window != image->window()) {
    return false;
  }
  QRect area(image->mapToGlobal(QPoint(0, 0)), image->size());
  if (const QJsonArray& rect = shot["rect"].toArray(); 4 == rect.size()) {
    area = QRect(area.topLeft() + QPoint(rect[0].toInt(), rect[1].toInt()), QSize(rect[2].toInt(), rect[3].toInt()));
  }
  return area.contains(placed.area);
}

void CShotMarker::refresh() {
  if (nullptr == mainWindow()) {
    return;
  }
  // The overlay covers one window: the one a `window` mark lies in.
  QWidget* window = windowShot();
  QList<CShotMarks::placed_t> placed;
  QStringList now;
  for (const CShotMarks::mark_t& mark : std::as_const(selectedMarks)) {
    CShotMarks::placed_t p;
    const QString& problem = CShotMarks::place(mark, window, p);
    QString status = statusOf(problem, p);
    if ("shown" == status && !inImage(p)) {
      status = "not in this image";
    }
    now << status;
    if (problem.isEmpty() && p.window == window) {
      placed << p;
    }
  }
  if (now != statuses) {
    statuses = now;
    send("mark-status " +
         QString::fromUtf8(QJsonDocument(QJsonArray::fromStringList(statuses)).toJson(QJsonDocument::Compact)));
  }

  QRect highlight;
  if (eIdle != mode && !candidates.isEmpty()) {
    QList<CShotMarks::mark_t> one;
    CShotMarks::placed_t p;
    // The candidate as a plain frame: an arrow without its tail cannot be placed yet.
    if (CShotMarks::parse(QJsonArray{candidates.at(current).mark}, one).isEmpty() && !one.isEmpty() &&
        CShotMarks::place(one.first(), window, p).isEmpty() && p.window == window) {
      highlight = p.area;
    }
  }

  const bool visible = !hidden && ((shown && !placed.isEmpty()) || highlight.isValid());
  if (!visible) {
    if (!overlay.isNull()) {
      overlay->hide();
    }
    return;
  }
  if (!overlay.isNull() && overlay->parentWidget() != window) {
    delete overlay.data();
  }
  if (overlay.isNull()) {
    overlay = new CShotMarkOverlay(window);
  }
  static_cast<CShotMarkOverlay*>(overlay.data())->show(shown ? placed : QList<CShotMarks::placed_t>(), highlight);
  overlay->setVisible(true);
}
