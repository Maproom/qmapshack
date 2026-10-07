// Measures, on the installed Qt, whether QApplication::notify() is a frame around the whole
// delivery of one spontaneous input event, and which user-interaction signals fire inside it.
// Build: g++ -std=c++20 -fPIC -DQT_WIDGETS_LIB -DQT_GUI_LIB -DQT_CORE_LIB -DQT_TESTLIB_LIB probe.cpp -o probe \
//   -I/usr/include/x86_64-linux-gnu/qt6{,/QtWidgets,/QtGui,/QtCore,/QtTest} -lQt6Widgets -lQt6Gui -lQt6Core -lQt6Test
// Run:   QT_QPA_PLATFORM=offscreen ./probe
#include <QtWidgets>
#include <QtTest/QtTest>
#include <cstdio>

static int depth = 0;          // nesting of input-event notify frames
static int seq = 0;            // running number of input frames
static QVector<int> frames;    // stack of open frame numbers
static bool inFilter = false;

static const char* name(QEvent::Type t) {
  switch (t) {
    case QEvent::MouseButtonPress: return "Press";
    case QEvent::MouseButtonRelease: return "Release";
    case QEvent::MouseButtonDblClick: return "DblClick";
    case QEvent::MouseMove: return "Move";
    case QEvent::KeyPress: return "KeyPress";
    case QEvent::KeyRelease: return "KeyRelease";
    case QEvent::ContextMenu: return "ContextMenu";
    case QEvent::Shortcut: return "Shortcut";
    case QEvent::ShortcutOverride: return "ShortcutOverride";
    default: return nullptr;
  }
}
static bool isInput(QEvent::Type t) {
  return t == QEvent::MouseButtonPress || t == QEvent::MouseButtonRelease || t == QEvent::MouseButtonDblClick ||
         t == QEvent::KeyPress || t == QEvent::KeyRelease || t == QEvent::ContextMenu || t == QEvent::Shortcut;
}
static QString where() {
  QString s;
  for (int f : frames) s += QString("#%1>").arg(f);
  return s.isEmpty() ? "(none)" : s;
}
static QString fmt(const QString& f){return f;}
template<class... A> QString fmt(const QString& f, A... a){return f.arg(a...);}
#define SIG(...) printf("      signal %-40s frame=%s\n", qPrintable(fmt(__VA_ARGS__)), qPrintable(where()))

class App : public QApplication {
 public:
  using QApplication::QApplication;
  bool notify(QObject* r, QEvent* e) override {
    const QEvent::Type t = e->type();
    const bool input = isInput(t) && e->spontaneous();
    // Only the outermost receiver of one physical event opens a frame: parent propagation re-enters
    // notify() with the same event object? No - QApplication::notify propagates inside one call.
    if (input) {
      frames.push_back(++seq);
      printf("    [notify+] #%d %s spont=%d -> %s(%s)\n", seq, name(t), e->spontaneous(),
             r->metaObject()->className(), qPrintable(r->objectName()));
    } else if (name(t) && t != QEvent::MouseMove && t != QEvent::KeyRelease && t != QEvent::ShortcutOverride) {
      printf("    [notify ] %s spont=%d -> %s(%s) frame=%s\n", name(t), e->spontaneous(),
             r->metaObject()->className(), qPrintable(r->objectName()), qPrintable(where()));
    }
    const bool res = QApplication::notify(r, e);
    if (input) {
      printf("    [notify-] #%d %s\n", frames.last(), name(t));
      frames.pop_back();
    }
    return res;
  }
};

class Filter : public QObject {
 public:
  bool eventFilter(QObject* o, QEvent* e) override {
    if (isInput(e->type()) && e->spontaneous() && e->type() != QEvent::KeyRelease)
      printf("      filter %-10s -> %s(%s) frame=%s\n", name(e->type()), o->metaObject()->className(),
             qPrintable(o->objectName()), qPrintable(where()));
    return false;
  }
};

class Surface : public QWidget {
 public:
  using QWidget::QWidget;
  void mousePressEvent(QMouseEvent* e) override { SIG("Surface::press %1,%2", QString::number(e->pos().x()), QString::number(e->pos().y())); }
  void mouseMoveEvent(QMouseEvent* e) override { SIG("Surface::move %1,%2", QString::number(e->pos().x()), QString::number(e->pos().y())); }
  void mouseReleaseEvent(QMouseEvent*) override { SIG("Surface::release"); }
  void mouseDoubleClickEvent(QMouseEvent*) override { SIG("Surface::dblclick"); }
};

int main(int argc, char** argv) {
  setvbuf(stdout, nullptr, _IONBF, 0);
  App app(argc, argv);
  Filter filter;
  app.installEventFilter(&filter);

  QMainWindow win;
  win.setObjectName("win");
  QWidget* central = new QWidget;
  central->setObjectName("central");
  win.setCentralWidget(central);
  QVBoxLayout* lay = new QVBoxLayout(central);

  QPushButton* button = new QPushButton("button"); button->setObjectName("button"); lay->addWidget(button);
  QCheckBox* check = new QCheckBox("check"); check->setObjectName("check"); lay->addWidget(check);
  QComboBox* combo = new QComboBox; combo->setObjectName("combo"); combo->addItems({"a", "b", "c"}); lay->addWidget(combo);
  QLineEdit* edit = new QLineEdit; edit->setObjectName("edit"); lay->addWidget(edit);
  QSpinBox* spin = new QSpinBox; spin->setObjectName("spin"); lay->addWidget(spin);
  QTreeWidget* tree = new QTreeWidget; tree->setObjectName("tree"); tree->setHeaderLabels({"n"}); lay->addWidget(tree);
  QTreeWidgetItem* root = new QTreeWidgetItem(tree, {"root"}); new QTreeWidgetItem(root, {"child"});
  QTabWidget* tabs = new QTabWidget; tabs->setObjectName("tabs"); tabs->addTab(new QWidget, "t1"); tabs->addTab(new QWidget, "t2"); lay->addWidget(tabs);
  Surface* surface = new Surface; surface->setObjectName("surface"); surface->setMinimumHeight(60); surface->setMouseTracking(true); lay->addWidget(surface);
  QPushButton* modal = new QPushButton("modal"); modal->setObjectName("modal"); lay->addWidget(modal);

  QAction* act = new QAction("act", &win); act->setObjectName("act"); act->setShortcut(QKeySequence("Ctrl+K"));
  QMenu* menu = win.menuBar()->addMenu("Menu"); menu->setObjectName("menu"); menu->addAction(act);
  QToolBar* bar = win.addToolBar("bar"); bar->setObjectName("bar"); bar->addAction(act);

  QObject::connect(button, &QPushButton::clicked, [] { SIG("button clicked"); });
  QObject::connect(button, &QPushButton::pressed, [] { SIG("button pressed"); });
  QObject::connect(check, &QCheckBox::clicked, [](bool b) { SIG("check clicked %1", QString::number(b)); });
  QObject::connect(check, &QCheckBox::toggled, [](bool b) { SIG("check toggled %1", QString::number(b)); });
  QObject::connect(combo, &QComboBox::activated, [](int i) { SIG("combo activated %1", QString::number(i)); });
  QObject::connect(combo, &QComboBox::currentIndexChanged, [](int i) { SIG("combo currentIndexChanged %1", QString::number(i)); });
  QObject::connect(edit, &QLineEdit::textEdited, [](const QString& s) { SIG("edit textEdited '%1'", s); });
  QObject::connect(edit, &QLineEdit::editingFinished, [] { SIG("edit editingFinished"); });
  QObject::connect(edit, &QLineEdit::returnPressed, [] { SIG("edit returnPressed"); });
  QObject::connect(spin, &QSpinBox::valueChanged, [](int v) { SIG("spin valueChanged %1", QString::number(v)); });
  QObject::connect(spin, &QSpinBox::editingFinished, [] { SIG("spin editingFinished"); });
  QObject::connect(tree, &QTreeWidget::itemClicked, [](QTreeWidgetItem* i, int) { SIG("tree itemClicked %1", i->text(0)); });
  QObject::connect(tree, &QTreeWidget::itemExpanded, [](QTreeWidgetItem* i) { SIG("tree itemExpanded %1", i->text(0)); });
  QObject::connect(tree, &QTreeWidget::itemDoubleClicked, [](QTreeWidgetItem* i, int) { SIG("tree itemDoubleClicked %1", i->text(0)); });
  QObject::connect(tree, &QTreeWidget::currentItemChanged, [](QTreeWidgetItem* i, QTreeWidgetItem*) { SIG("tree currentItemChanged %1", i ? i->text(0) : "null"); });
  QObject::connect(tabs->tabBar(), &QTabBar::tabBarClicked, [](int i) { SIG("tabBarClicked %1", QString::number(i)); });
  QObject::connect(tabs, &QTabWidget::currentChanged, [](int i) { SIG("tabs currentChanged %1", QString::number(i)); });
  QObject::connect(tree->header(), &QHeaderView::sectionClicked, [](int i) { SIG("sectionClicked %1", QString::number(i)); });
  QObject::connect(act, &QAction::triggered, [] { SIG("act triggered"); });
  QObject::connect(menu, &QMenu::triggered, [](QAction* a) { SIG("menu triggered %1", a->objectName()); });
  QObject::connect(menu, &QMenu::aboutToShow, [] { SIG("menu aboutToShow"); });
  QObject::connect(modal, &QPushButton::clicked, [&] {
    SIG("modal clicked -> exec dialog");
    QDialog dlg(&win); dlg.setObjectName("dlg");
    QPushButton* ok = new QPushButton("ok", &dlg); ok->setObjectName("ok");
    QObject::connect(ok, &QPushButton::clicked, [&] { SIG("ok clicked"); dlg.accept(); });
    QTimer::singleShot(50, &dlg, [ok] { printf("      (clicking ok inside modal loop)\n"); QTest::mouseClick(ok, Qt::LeftButton); });
    dlg.exec();
    SIG("modal: dialog closed, slot returns");
  });

  win.resize(400, 700);
  win.show();
  QTest::qWaitForWindowExposed(&win);
  QTest::qWait(50);

  // Every case gets a deadline so a hang reports itself instead of stalling.
  QTimer watchdog; watchdog.setSingleShot(true);
  QObject::connect(&watchdog, &QTimer::timeout, [] { printf("!!! WATCHDOG: case hung\n"); fflush(stdout); _exit(3); });
  auto run = [&](const char* title, auto fn) {
    printf("\n=== %s\n", title); watchdog.start(3000); fn(); QTest::qWait(30); watchdog.stop();
  };

  run("1 click QPushButton", [&] { QTest::mouseClick(button, Qt::LeftButton); });
  run("2 click QCheckBox", [&] { QTest::mouseClick(check, Qt::LeftButton); });
  run("3 double click Surface", [&] { QTest::mouseDClick(surface, Qt::LeftButton, {}, QPoint(10, 10)); });
  run("4 press-move-release Surface (drag)", [&] {
    QTest::mousePress(surface, Qt::LeftButton, {}, QPoint(5, 5));
    QTest::mouseMove(surface, QPoint(20, 20));
    QTest::mouseMove(surface, QPoint(40, 40));
    QTest::mouseRelease(surface, Qt::LeftButton, {}, QPoint(40, 40));
  });
  run("5 type into QLineEdit then Enter", [&] { edit->setFocus(); QTest::keyClicks(edit, "hi"); QTest::keyClick(edit, Qt::Key_Return); });
  run("6 spin: type then Tab", [&] { spin->setFocus(); QTest::keyClicks(spin, "7"); QTest::keyClick(spin, Qt::Key_Tab); });
  run("7 click tree row (expand arrow region + text)", [&] {
    const QRect r = tree->visualItemRect(root);
    QTest::mouseClick(tree->viewport(), Qt::LeftButton, {}, r.center());
  });
  run("8 double click tree row", [&] {
    const QRect r = tree->visualItemRect(root);
    QTest::mouseDClick(tree->viewport(), Qt::LeftButton, {}, r.center());
  });
  run("9 click tab 2", [&] { QTest::mouseClick(tabs->tabBar(), Qt::LeftButton, {}, tabs->tabBar()->tabRect(1).center()); });
  run("10 click header section", [&] { QTest::mouseClick(tree->header()->viewport(), Qt::LeftButton, {}, QPoint(10, 5)); });
  run("11 toolbar button (shared action)", [&] {
    QWidget* w = bar->widgetForAction(act); QTest::mouseClick(w, Qt::LeftButton);
  });
  run("12 shortcut Ctrl+K", [&] { QTest::keyClick(&win, Qt::Key_K, Qt::ControlModifier); });
  run("13 combo: click, then click item in popup", [&] {
    QTest::mouseClick(combo, Qt::LeftButton);
    QTest::qWait(50);
    QWidget* popup = QApplication::activePopupWidget();
    printf("      popup=%s\n", popup ? popup->metaObject()->className() : "null");
    if (popup) {
      QAbstractItemView* v = popup->findChild<QAbstractItemView*>();
      if (!v) v = qobject_cast<QAbstractItemView*>(popup);
      if (v) { const QRect r = v->visualRect(v->model()->index(2, 0)); QTest::mouseClick(v->viewport(), Qt::LeftButton, {}, r.center()); }
    }
  });
  run("14 menubar: click title, then click entry", [&] {
    QMenuBar* mb = win.menuBar();
    const QRect r = mb->actionGeometry(menu->menuAction());
    QTest::mouseClick(mb, Qt::LeftButton, {}, r.center());
    QTest::qWait(50);
    printf("      popup=%s\n", QApplication::activePopupWidget() ? QApplication::activePopupWidget()->metaObject()->className() : "null");
    if (menu->isVisible()) { QTest::mouseClick(menu, Qt::LeftButton, {}, menu->actionGeometry(act).center()); }
  });
  run("15 modal dialog from a slot", [&] { QTest::mouseClick(modal, Qt::LeftButton); });
  run("16 context menu on Surface (right click)", [&] { QTest::mouseClick(surface, Qt::RightButton, {}, QPoint(10, 10)); });
  run("17 press on empty area (acts on nothing)", [&] { QTest::mouseClick(central, Qt::LeftButton, {}, QPoint(2, 2)); });

  printf("\nDONE\n");
  return 0;
}
