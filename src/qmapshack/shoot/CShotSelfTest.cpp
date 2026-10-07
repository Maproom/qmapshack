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

#include "shoot/CShotSelfTest.h"

#include <qpa/qwindowsysteminterface.h>

#include <QAbstractItemView>
#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QCompleter>
#include <QDateTimeEdit>
#include <QDebug>
#include <QDialog>
#include <QDialogButtonBox>
#include <QDockWidget>
#include <QDoubleSpinBox>
#include <QElapsedTimer>
#include <QGridLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QImage>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMenu>
#include <QMenuBar>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QScrollArea>
#include <QSlider>
#include <QSpinBox>
#include <QSplitter>
#include <QStyle>
#include <QStyleHints>
#include <QStyleOptionGroupBox>
#include <QStyleOptionToolButton>
#include <QStyleOptionViewItem>
#include <QTabBar>
#include <QTabWidget>
#include <QTimer>
#include <QToolButton>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <algorithm>
#include <functional>
#include <memory>
#include <optional>

#include "CMainWindow.h"
#include "canvas/CCanvas.h"
#include "gis/CDBItemDelegate.h"
#include "gis/CGisListDB.h"
#include "gis/CGisListWks.h"
#include "gis/CGisWorkspace.h"
#include "gis/CWksItemDelegate.h"
#include "gis/IDBItem.h"
#include "gis/IWksItem.h"
#include "gis/proj_x.h"
#include "gis/search/CGeoSearch.h"
#include "gis/trk/CGisItemTrk.h"
#include "helpers/CWptIconManager.h"
#include "map/CMapItemDelegate.h"
#include "map/CMapList.h"
#include "map/IMapItem.h"
#include "mouse/CMouseNormal.h"
#include "plot/CPlotProfile.h"
#include "poi/CPoiCategory.h"
#include "poi/CPoiFileItem.h"
#include "poi/CPoiList.h"
#include "poi/CPoiPropSetup.h"
#include "poi/IPoiFile.h"
#include "poi/IPoiItem.h"
#include "shoot/CShotAddress.h"
#include "shoot/CShotApplication.h"
#include "shoot/CShotContext.h"
#include "shoot/CShotHandlers.h"
#include "shoot/CShotPage.h"
#include "shoot/CShotRecorder.h"
#include "shoot/CShotReplay.h"
#include "shoot/CShotSynth.h"
#include "shoot/CShotWriter.h"
#include "widgets/CIconGrid.h"

namespace {
qint32 failures = 0;
qint32 cases = 0;

QString compact(const QJsonObject& step) {
  return QString::fromUtf8(QJsonDocument(step).toJson(QJsonDocument::Compact));
}

/** @return @p step without the values that depend on the window */
QString signatureOf(QJsonObject step) {
  for (const char* volatile_ : {"at", "lat", "lon", "x", "held", "icon", "state"}) {
    step.remove(QString::fromLatin1(volatile_));
  }
  // Splitter shares follow the layout, a date step follows the cursor's section.
  if ("scroll" == step.value("do").toString()) {
    step.remove("row");
  }
  const QString& property = step.value("property").toString();
  if ("sizes" == property || "dateTime" == property) {
    step.remove("value");
  }
  return compact(step);
}

void verdict(const QString& name, bool ok, const QString& detail = QString()) {
  cases++;
  failures += ok ? 0 : 1;
  qWarning().noquote() << (ok ? "shoot: PASS" : "shoot: FAIL") << name << (detail.isEmpty() ? "" : "| " + detail);
}

void settle(qint32 ms = 60) { QTest::qWait(ms); }

/** Counts the menus shown. */
class CMenuCounter : public QObject {
 public:
  qint32 shown = 0;

 protected:
  bool eventFilter(QObject* watched, QEvent* event) override {
    if (QEvent::Show == event->type() && nullptr != qobject_cast<QMenu*>(watched)) {
      shown++;
    }
    return QObject::eventFilter(watched, event);
  }
};

/** One case: reset, record the input, compare the steps, reset, replay, compare the state. */
struct case_t {
  QString name;
  QStringList wanted;                    /**< the steps, as signatureOf() spells them */
  std::function<void()> reset = []() {}; /**< restores the starting state */
  std::function<void()> perform;         /**< the input */
  std::function<QString()> state = []() { return QString(); };
  bool replays = true;            /**< false where replaying needs the scenario queue */
  bool closesMenus = false;       /**< close exec()'d menus, or the case blocks */
  bool replayClosesMenus = false; /**< the same, for the replay only */
  QString warns;                  /**< a warning the recording must give */
};

/** Collects the warnings given during its lifetime. */
class CWarnings {
 public:
  CWarnings() {
    given.clear();
    previous = qInstallMessageHandler([](QtMsgType type, const QMessageLogContext& context, const QString& message) {
      if (QtWarningMsg == type) {
        given << message;
      }
      previous(type, context, message);
    });
  }
  ~CWarnings() { qInstallMessageHandler(previous); }

  static bool contains(const QString& text) {
    return std::any_of(given.begin(), given.end(), [text](const QString& message) { return message.contains(text); });
  }

 private:
  static inline QStringList given;
  static inline QtMessageHandler previous = nullptr;
};

struct panel_t {
  QWidget* window = nullptr;
  QDoubleSpinBox* doubleSpin = nullptr;
  QComboBox* editable = nullptr;
  QToolButton* menuButton = nullptr;
  QToolButton* actionMenuButton = nullptr;
  QToolButton* delayedButton = nullptr;
  QMenuBar* bar = nullptr;
  QAction* menuEntry = nullptr;
  QPushButton* button = nullptr;
  QCheckBox* check = nullptr;
  QLineEdit* edit = nullptr;
  QSpinBox* spin = nullptr;
  QComboBox* combo = nullptr;
  QToolButton* tool = nullptr;
  QAction* toolAction = nullptr;
  QPushButton* viaSlot = nullptr;
  QAction* slotAction = nullptr;
  QToolButton* shared1 = nullptr;
  QToolButton* shared2 = nullptr;
  QAction* sharedAction1 = nullptr;
  QAction* sharedAction2 = nullptr;
  QToolButton* lonely = nullptr;
  std::unique_ptr<QAction> lonelyAction; /**< parentless: only the widget it is added to finds it */
  QAction* shortcut = nullptr;
  QPushButton* modal = nullptr;
  QLabel* label = nullptr;
  QPushButton* execMenu = nullptr;
  QLineEdit* completed = nullptr;
  QListWidget* longList = nullptr;
  QTabWidget* tabs = nullptr;
  QLineEdit* tabEdit = nullptr;
  QTreeWidget* tree = nullptr;
  QSlider* slider = nullptr;
  QSplitter* splitter = nullptr;
  QDateTimeEdit* date = nullptr;
  QGroupBox* group = nullptr;
  QPlainTextEdit* text = nullptr;
  QDialog* dialog = nullptr;
  QLineEdit* dialogEdit = nullptr;
  QCheckBox* dialogCheck = nullptr;
  QWidget* surfaces = nullptr;
  CPlotProfile* plot = nullptr;
  CIconGrid* grid = nullptr;
  QHash<QString, qint32> counts;
};

void fillTabs(panel_t& p) {
  while (p.tabs->count() > 0) {
    QWidget* page = p.tabs->widget(0);
    p.tabs->removeTab(0);
    if (page != p.tabEdit) {
      delete page;
    }
  }
  p.tabs->addTab(p.tabEdit, "one");
  p.tabs->addTab(new QLabel("two"), "two");
  p.tabs->addTab(new QLabel("three"), "three");
  p.tabs->setCurrentIndex(0);
}

void buildPanel(panel_t& p, QWidget* main, CGisItemTrk* trk) {
  p.window = new QWidget(main, Qt::Window);
  p.window->setObjectName("probePanel");
  QGridLayout* grid = new QGridLayout(p.window);
  qint32 row = 0;
  auto add = [&grid, &row](QWidget* w, const QString& name) {
    w->setObjectName(name);
    grid->addWidget(w, row / 2, row % 2);
    row++;
    return w;
  };

  p.button = static_cast<QPushButton*>(add(new QPushButton("button"), "probeButton"));
  QObject::connect(p.button, &QPushButton::clicked, p.window, [&p]() { p.counts["button"]++; });
  p.check = static_cast<QCheckBox*>(add(new QCheckBox("check"), "probeCheck"));
  p.edit = static_cast<QLineEdit*>(add(new QLineEdit, "probeEdit"));
  QObject::connect(p.edit, &QLineEdit::returnPressed, p.window, [&p]() { p.counts["return"]++; });
  QObject::connect(p.edit, &QLineEdit::editingFinished, p.window, [&p]() { p.counts["editFinished"]++; });
  p.spin = static_cast<QSpinBox*>(add(new QSpinBox, "probeSpin"));
  QObject::connect(p.spin, &QSpinBox::editingFinished, p.window, [&p]() { p.counts["spinFinished"]++; });
  p.combo = static_cast<QComboBox*>(add(new QComboBox, "probeCombo"));
  p.combo->addItems({"one", "two", "three"});
  QObject::connect(p.combo, &QComboBox::activated, p.window, [&p]() { p.counts["activated"]++; });
  QObject::connect(p.combo, &QComboBox::textActivated, p.window, [&p]() { p.counts["textActivated"]++; });

  p.tool = static_cast<QToolButton*>(add(new QToolButton, "probeTool"));
  p.toolAction = new QAction("tool", p.tool);
  p.toolAction->setObjectName("probeToolAction");
  p.toolAction->setCheckable(true);
  p.tool->setDefaultAction(p.toolAction);
  QObject::connect(p.toolAction, &QAction::triggered, p.window, [&p]() { p.counts["toolAction"]++; });

  p.viaSlot = static_cast<QPushButton*>(add(new QPushButton("via slot"), "probeViaSlot"));
  p.slotAction = new QAction("slot", p.window);
  p.slotAction->setObjectName("probeSlotAction");
  QObject::connect(p.slotAction, &QAction::triggered, p.window, [&p]() { p.counts["slotAction"]++; });
  QObject::connect(p.viaSlot, &QPushButton::clicked, p.slotAction, &QAction::trigger);

  p.shared1 = static_cast<QToolButton*>(add(new QToolButton, "probeShared1"));
  p.shared2 = static_cast<QToolButton*>(add(new QToolButton, "probeShared2"));
  p.sharedAction1 = new QAction("shared", p.shared1);
  p.sharedAction2 = new QAction("shared", p.shared2);
  for (QAction* action : {p.sharedAction1, p.sharedAction2}) {
    action->setObjectName("probeShared");
  }
  p.shared1->setDefaultAction(p.sharedAction1);
  p.shared2->setDefaultAction(p.sharedAction2);
  QObject::connect(p.sharedAction1, &QAction::triggered, p.window, [&p]() { p.counts["shared1"]++; });
  QObject::connect(p.sharedAction2, &QAction::triggered, p.window, [&p]() { p.counts["shared2"]++; });

  p.lonely = static_cast<QToolButton*>(add(new QToolButton, "probeLonelyButton"));
  p.lonelyAction = std::make_unique<QAction>("lonely");
  p.lonelyAction->setObjectName("probeLonely");
  p.lonely->setDefaultAction(p.lonelyAction.get());
  QObject::connect(p.lonelyAction.get(), &QAction::triggered, p.window, [&p]() { p.counts["lonely"]++; });

  p.shortcut = new QAction("shortcut", p.window);
  p.shortcut->setObjectName("probeShortcut");
  p.shortcut->setShortcut(QKeySequence("Ctrl+J"));
  p.window->addAction(p.shortcut);
  QObject::connect(p.shortcut, &QAction::triggered, p.window, [&p]() { p.counts["shortcut"]++; });

  // Steps inside a modal dialog are recorded before the click that opened it; frame numbers restore the order.
  p.modal = static_cast<QPushButton*>(add(new QPushButton("modal"), "probeModal"));
  QObject::connect(p.modal, &QPushButton::clicked, p.window, [&p]() {
    QDialog dialog(p.window);
    dialog.setObjectName("probeModalDialog");
    QPushButton* ok = new QPushButton("ok", &dialog);
    ok->setObjectName("probeOk");
    QObject::connect(ok, &QPushButton::clicked, &dialog, &QDialog::accept);
    QTimer::singleShot(100, ok, [ok]() {
      CShotSynth::mouse(QTest::MouseClick, ok, Qt::LeftButton, Qt::NoModifier, ok->rect().center());
    });
    dialog.exec();
  });

  p.label = static_cast<QLabel*>(add(new QLabel("label"), "probeLabel"));

  // A menu decided by exec()'s return value, with no slot on the entry, like the waypoint icon and colour menus.
  p.execMenu = static_cast<QPushButton*>(add(new QPushButton("exec menu"), "probeExecMenu"));
  QMenu* execMenu = new QMenu(p.window);
  execMenu->setObjectName("probeExecMenuMenu");
  execMenu->addAction("one")->setObjectName("probeExecOne");
  execMenu->addAction("two")->setObjectName("probeExecTwo");
  QObject::connect(p.execMenu, &QPushButton::clicked, p.window, [&p, execMenu]() {
    if (const QAction* chosen = execMenu->exec(p.execMenu->mapToGlobal(QPoint(0, p.execMenu->height())));
        nullptr != chosen) {
      p.counts["exec:" + chosen->objectName()]++;
    }
  });

  p.completed = static_cast<QLineEdit*>(add(new QLineEdit, "probeCompleted"));
  QCompleter* completer = new QCompleter(QStringList{"alpha", "apple", "avocado"}, p.completed);
  completer->setCompletionMode(QCompleter::UnfilteredPopupCompletion);
  completer->setFilterMode(Qt::MatchContains);
  p.completed->setCompleter(completer);
  QObject::connect(p.completed, &QLineEdit::returnPressed, p.completed,
                   [edit = p.completed]() { edit->setProperty("returns", edit->property("returns").toInt() + 1); });

  p.longList = static_cast<QListWidget*>(add(new QListWidget, "probeLong"));
  for (qint32 i = 0; i < 200; i++) {
    p.longList->addItem(QString("line %1").arg(i));
  }
  p.longList->setFixedHeight(120);

  p.doubleSpin = static_cast<QDoubleSpinBox*>(add(new QDoubleSpinBox, "probeDoubleSpin"));
  p.doubleSpin->setDecimals(1);
  p.editable = static_cast<QComboBox*>(add(new QComboBox, "probeEditable"));
  p.editable->setEditable(true);
  p.editable->addItems({"alpha", "beta"});
  // Picking an entry closes the menu before the action runs.
  p.menuButton = static_cast<QToolButton*>(add(new QToolButton, "probeMenuButton"));
  p.menuButton->setPopupMode(QToolButton::InstantPopup);
  QMenu* menu = new QMenu(p.menuButton);
  p.menuEntry = menu->addAction("entry");
  p.menuEntry->setObjectName("probeMenuEntry");
  p.menuButton->setMenu(menu);
  QObject::connect(p.menuEntry, &QAction::triggered, p.window, [&p]() { p.counts["menuEntry"]++; });
  // As a toolbar shows an action with a menu, e.g. actionTrackInfo.
  p.actionMenuButton = static_cast<QToolButton*>(add(new QToolButton, "probeActionMenuButton"));
  p.actionMenuButton->setPopupMode(QToolButton::MenuButtonPopup);
  QAction* menuAction = new QAction("action with menu", p.window);
  menuAction->setObjectName("probeMenuAction");
  QMenu* actionMenu = new QMenu(p.window);
  actionMenu->setObjectName("probeActionMenu");
  actionMenu->addAction("entry");
  menuAction->setMenu(actionMenu);
  p.actionMenuButton->setDefaultAction(menuAction);
  p.delayedButton = static_cast<QToolButton*>(add(new QToolButton, "probeDelayedButton"));
  p.delayedButton->setPopupMode(QToolButton::DelayedPopup);
  QMenu* delayedMenu = new QMenu(p.delayedButton);
  delayedMenu->addAction("entry");
  p.delayedButton->setMenu(delayedMenu);

  p.bar = new QMenuBar(p.window);
  p.bar->setObjectName("probeBar");
  grid->setMenuBar(p.bar);
  QMenu* barMenu = p.bar->addMenu("bar menu");
  barMenu->setObjectName("probeBarMenu");
  QMenu* subMenu = barMenu->addMenu("submenu");
  subMenu->setObjectName("probeSubMenu");
  QAction* subEntry = subMenu->addAction("sub entry");
  subEntry->setObjectName("probeSubEntry");
  QObject::connect(subEntry, &QAction::triggered, p.window, [&p]() { p.counts["subEntry"]++; });

  p.tabs = static_cast<QTabWidget*>(add(new QTabWidget, "probeTabs"));
  p.tabs->setTabsClosable(true);
  p.tabEdit = new QLineEdit;
  p.tabEdit->setObjectName("probeTabEdit");
  QObject::connect(p.tabs, &QTabWidget::tabCloseRequested, p.window, [&p](int index) {
    QWidget* page = p.tabs->widget(index);
    p.tabs->removeTab(index);
    if (page != p.tabEdit) {
      page->deleteLater();
    }
  });
  fillTabs(p);

  p.tree = static_cast<QTreeWidget*>(add(new QTreeWidget, "probeTree"));
  p.tree->setHeaderLabels({"name"});
  QTreeWidgetItem* root = new QTreeWidgetItem(p.tree, {"root"});
  new QTreeWidgetItem(root, {"b child"});
  new QTreeWidgetItem(root, {"a child"});
  new QTreeWidgetItem(p.tree, {"zeta"});
  p.tree->setSortingEnabled(true);
  QObject::connect(p.tree, &QTreeWidget::itemClicked, p.window, [&p]() { p.counts["treeClicked"]++; });
  QObject::connect(p.tree, &QTreeWidget::itemDoubleClicked, p.window, [&p]() { p.counts["treeDouble"]++; });

  p.slider = static_cast<QSlider*>(add(new QSlider(Qt::Horizontal), "probeSlider"));
  p.slider->setRange(0, 20);

  p.splitter = static_cast<QSplitter*>(add(new QSplitter(Qt::Horizontal), "probeSplitter"));
  p.splitter->addWidget(new QLabel("left"));
  p.splitter->addWidget(new QLabel("right"));
  p.splitter->setMinimumWidth(200);

  p.date = static_cast<QDateTimeEdit*>(add(new QDateTimeEdit, "probeDate"));
  p.date->setDisplayFormat("yyyy-MM-dd");
  QObject::connect(p.date, &QDateTimeEdit::editingFinished, p.window, [&p]() { p.counts["dateFinished"]++; });

  p.group = static_cast<QGroupBox*>(add(new QGroupBox("group"), "probeGroup"));
  p.group->setCheckable(true);
  p.group->setMinimumHeight(40);

  p.text = static_cast<QPlainTextEdit*>(add(new QPlainTextEdit, "probeText"));
  p.text->setMaximumHeight(50);

  // No overlap: a context menu goes to the topmost window at its position, and offscreen every window is topmost.
  p.window->move(main->geometry().right() + 50, 0);
  p.window->resize(640, 1300);
  p.window->show();
  if (!QTest::qWaitForWindowExposed(p.window)) {
    qWarning() << "shoot: the self test's panel never came up";
  }

  p.dialog = new QDialog(main);
  p.dialog->setObjectName("probeDialog");
  QVBoxLayout* dialogLayout = new QVBoxLayout(p.dialog);
  p.dialogEdit = new QLineEdit;
  p.dialogEdit->setObjectName("probeDialogEdit");
  p.dialogCheck = new QCheckBox("check");
  p.dialogCheck->setObjectName("probeDialogCheck");
  QDialogButtonBox* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  dialogLayout->addWidget(p.dialogEdit);
  dialogLayout->addWidget(p.dialogCheck);
  dialogLayout->addWidget(buttons);
  QObject::connect(buttons, &QDialogButtonBox::accepted, p.dialog, &QDialog::accept);
  QObject::connect(buttons, &QDialogButtonBox::rejected, p.dialog, &QDialog::reject);
  QObject::connect(p.dialog, &QDialog::accepted, p.window, [&p]() { p.counts["accepted"]++; });
  QObject::connect(p.dialog, &QDialog::rejected, p.window, [&p]() { p.counts["rejected"]++; });

  // The plot and the icon grid need a window of their own for room.
  p.surfaces = new QWidget(main, Qt::Window);
  p.surfaces->setObjectName("probeSurfaces");
  QVBoxLayout* surfaces = new QVBoxLayout(p.surfaces);
  p.plot = new CPlotProfile(trk, trk->limitsGraph1, IPlot::eModeNormal, p.surfaces);
  p.plot->setObjectName("probePlot");
  p.plot->setMinimumSize(500, 220);
  surfaces->addWidget(p.plot);
  // As CWptIconSelectWidget: the area's width gives the grid its columns.
  QScrollArea* area = new QScrollArea(p.surfaces);
  area->setObjectName("probeIconArea");
  area->setWidgetResizable(true);
  area->setMinimumHeight(CIconGrid::kVisibleRows * CIconGrid::kTileSize);
  area->setMinimumWidth(CIconGrid::kVisibleCols * CIconGrid::kTileSize);
  p.grid = new CIconGrid(area);
  p.grid->setObjectName("probeIconGrid");
  p.grid->updateIconList(CWptIconManager::self().getWptIcons());
  area->setWidget(p.grid);
  surfaces->addWidget(area);
  QObject::connect(p.grid, &CIconGrid::sigSelectedIcon, p.surfaces,
                   [&p](const QString& name) { p.counts["icon:" + name]++; });
  p.surfaces->move(p.window->geometry().right() + 50, 0);
  p.surfaces->resize(620, 620);
  p.surfaces->show();
  if (!QTest::qWaitForWindowExposed(p.surfaces)) {
    qWarning() << "shoot: the self test's surface window never came up";
  }
  settle(300);
}

QString countsOf(const panel_t& p, const QStringList& names) {
  QStringList values;
  for (const QString& name : names) {
    values << QString("%1=%2").arg(name).arg(p.counts.value(name));
  }
  return values.join(' ');
}

void zeroCounts(panel_t& p) {
  for (auto it = p.counts.begin(); it != p.counts.end(); ++it) {
    it.value() = 0;
  }
}

QRect groupCheckRect(QGroupBox* group) {
  QStyleOptionGroupBox option;
  option.initFrom(group);
  option.text = group->title();
  option.lineWidth = 1;
  option.textAlignment = group->alignment();
  option.subControls = QStyle::SC_GroupBoxFrame | QStyle::SC_GroupBoxCheckBox | QStyle::SC_GroupBoxLabel;
  option.state |= group->isChecked() ? QStyle::State_On : QStyle::State_Off;
  return group->style()->subControlRect(QStyle::CC_GroupBox, &option, QStyle::SC_GroupBoxCheckBox, group);
}

QString viewState(CCanvas* canvas) {
  const QJsonObject& view = CShotPage::viewOf(canvas);
  return QString("%1 %2 %3")
      .arg(view["lat"].toDouble(), 0, 'f', 6)
      .arg(view["lon"].toDouble(), 0, 'f', 6)
      .arg(view["zoom"].toInt());
}

/** @return a point on @p canvas with no item near it, off the track's line */
QPoint emptyPlace(CCanvas* canvas) {
  for (qint32 y = canvas->height() - 30; y > 30; y -= 20) {
    for (qint32 x = 20; x < canvas->width() - 20; x += 20) {
      QList<IGisItem*> items;
      CGisWorkspace::self().getItemsByPos(QPointF(x, y), items);
      if (items.isEmpty()) {
        return QPoint(x, y);
      }
    }
  }
  return QPoint(canvas->width() / 2, canvas->height() - 30);
}
}  // namespace

qint32 CShotSelfTest::run(CShotContext& ctx) {
  failures = 0;
  cases = 0;

  CMainWindow* main = ctx.mainWindow();
  CGisListWks* wks = const_cast<CGisListWks*>(ctx.wksList());
  CCanvas* canvas = ctx.canvas();
  CGisItemTrk* trk = ctx.trk();
  const bool trackUsable =
      nullptr != trk && !trk->getTrackData().segs.isEmpty() && trk->getTrackData().segs.first().pts.size() >= 3;
  if (nullptr == main || nullptr == wks || nullptr == canvas || !trackUsable || 0 == wks->topLevelItemCount()) {
    verdict("the fixture has what the cases need: a main window, a map, a project and a track of three points", false);
    return failures;
  }

  // An item's screen options open beside the click; in a narrow map they fall outside it and cannot be clicked.
  main->showNormal();
  main->resize(1600, 1000);
  settle(500);

  panel_t p;
  buildPanel(p, main, trk);
  CShotRecorder recorder(ctx, main);
  CMenuCounter menus;
  qApp->installEventFilter(&menus);
  QTimer menuCloser;
  menuCloser.setInterval(50);
  QObject::connect(&menuCloser, &QTimer::timeout, &menuCloser, []() {
    if (QMenu* menu = qobject_cast<QMenu*>(QApplication::activePopupWidget()); nullptr != menu) {
      menu->close();
    }
  });

  QTreeWidgetItem* project = wks->topLevelItem(0);
  const QString projectPath = CShotAddress::itemPathOf(project);
  // Placeholders in the wanted steps, resolved for this run.
  QHash<QString, QString> names{{"%TRK%", CShotAddress::itemPathOf(trk)},
                                {"%MAP%", CShotAddress::addressOf(main, canvas).value_or(QString())}};

  auto check = [&](const case_t& c) {
    // Leftover focus would end an edit in the next case.
    while (QWidget* popup = QApplication::activePopupWidget()) {
      popup->close();
    }
    if (QWidget* focus = QApplication::focusWidget(); nullptr != focus) {
      focus->clearFocus();
    }
    c.reset();
    menus.shown = 0;
    settle();
    if (c.closesMenus) {
      menuCloser.start();
    }
    std::optional<CWarnings> warnings;
    warnings.emplace();
    recorder.start();
    c.perform();
    settle(120);
    const QJsonArray& actions = recorder.stop();
    const bool warned = c.warns.isEmpty() || CWarnings::contains(c.warns);
    warnings.reset();
    menuCloser.stop();
    const QString recorded = c.state();

    QList<QJsonObject> steps;
    QJsonObject view;
    for (const QJsonValue& value : actions) {
      const QJsonObject& step = value.toObject();
      const QString& verb = step["do"].toString();
      if ("view" == verb) {
        view = step;
      } else if ("layout" != verb) {
        steps << step;
      }
    }
    QStringList got;
    QStringList full;
    for (const QJsonObject& step : std::as_const(steps)) {
      got << signatureOf(step);
      full << compact(step);
    }
    qDebug().noquote() << "shoot: case" << c.name << "recorded" << full.join(" ");
    QStringList wanted = c.wanted;
    for (QString& spelled : wanted) {
      for (auto it = names.constBegin(); it != names.constEnd(); ++it) {
        spelled.replace(it.key(), it.value());
      }
    }
    if (got != wanted) {
      verdict(c.name, false,
              QString("wanted: %1 | got: %2")
                  .arg(wanted.isEmpty() ? "(nothing)" : wanted.join(" "), got.isEmpty() ? "(nothing)" : got.join(" ")));
      return;
    }
    if (!warned) {
      verdict(c.name, false, "no warning says: " + c.warns);
      return;
    }
    if (!c.replays) {
      verdict(c.name, true);
      return;
    }

    if (QWidget* focus = QApplication::focusWidget(); nullptr != focus) {
      focus->clearFocus();
    }
    c.reset();
    menus.shown = 0;
    settle();
    if (!view.isEmpty() && !CShotPage::applyView(canvas, view)) {
      verdict(c.name, false, "the recording's start view could not be applied");
      return;
    }
    if (c.closesMenus || c.replayClosesMenus) {
      menuCloser.start();
    }
    const qint32 replayFailures = CShotReplay::perform(main, steps);
    settle(120);
    menuCloser.stop();
    const QString replayed = c.state();
    verdict(c.name, 0 == replayFailures && replayed == recorded,
            (0 == replayFailures && replayed == recorded)
                ? QString()
                : QString("recorded '%1', replayed '%2', %3 replay failure(s)")
                      .arg(recorded, replayed)
                      .arg(replayFailures));
  };

  auto click = [](QWidget* w, const QPoint& at, Qt::MouseButton button = Qt::LeftButton) {
    // A case that clicks where its widget is not would test another widget.
    if (const QString& other = CShotSynth::missed(w, at); !other.isEmpty()) {
      qWarning().noquote() << "shoot: the case clicks" << w->metaObject()->className() << "where" << other << "is";
    }
    CShotSynth::mouse(QTest::MouseClick, w, button, Qt::NoModifier, at);
  };
  auto center = [](QWidget* w) { return w->rect().center(); };

  // ---- Qt's controls

  check({"a button is a click",
         {R"({"do":"click","widget":"probeButton"})"},
         [&p]() { zeroCounts(p); },
         [&]() { click(p.button, center(p.button)); },
         [&p]() { return countsOf(p, {"button"}); }});

  check({"a check box is a click that says the state it left",
         {R"({"checked":true,"do":"click","widget":"probeCheck"})"},
         [&p]() { p.check->setChecked(false); },
         // A check box is clickable only on its box and text.
         [&]() { click(p.check, QPoint(8, p.check->height() / 2)); },
         [&p]() { return QString::number(p.check->isChecked()); }});

  check({"typing is one step with the final text",
         {R"({"do":"key","text":"hello","widget":"probeEdit"})"},
         [&p]() { p.edit->clear(); },
         [&p]() { CShotSynth::type(p.edit, "hello"); },
         [&p]() { return p.edit->text(); }});

  check({"Enter ends typing and is part of it",
         {R"({"do":"key","enter":true,"text":"world","widget":"probeEdit"})"},
         [&p]() {
           p.edit->clear();
           zeroCounts(p);
         },
         [&p]() {
           CShotSynth::type(p.edit, "world");
           CShotSynth::key(p.edit, Qt::Key_Return);
         },
         [&p]() { return p.edit->text() + " " + countsOf(p, {"return", "editFinished"}); }});

  check({"typing ended by a click elsewhere ends it there",
         {R"({"do":"key","text":"abc","widget":"probeEdit"})", R"({"do":"endedit","widget":"probeEdit"})",
          R"({"do":"click","widget":"probeButton"})"},
         [&p]() {
           p.edit->clear();
           zeroCounts(p);
         },
         [&]() {
           CShotSynth::type(p.edit, "abc");
           click(p.button, center(p.button));
         },
         [&p]() { return p.edit->text() + " " + countsOf(p, {"editFinished", "button"}); }});

  check({"a spin box typed into is its text",
         {R"({"do":"key","text":"7","widget":"probeSpin"})"},
         [&p]() { p.spin->clear(); },
         [&p]() { CShotSynth::type(p.spin, "7"); },
         [&p]() { return QString::number(p.spin->value()); }});

  check({"a spin box stepped with the up key is its text",
         {R"({"do":"key","text":"1","widget":"probeSpin"})"},
         [&p]() { p.spin->setValue(0); },
         [&p]() { CShotSynth::key(p.spin, Qt::Key_Up); },
         [&p]() { return QString::number(p.spin->value()); }});

  check({"a spin box stepped with its up button is its text",
         {R"({"do":"key","text":"1","widget":"probeSpin"})"},
         [&p]() { p.spin->setValue(0); },
         [&]() { click(p.spin, QPoint(p.spin->width() - 6, 4)); },
         [&p]() { return QString::number(p.spin->value()); }});

  check({"a spin box left with Tab ends its editing",
         {R"({"do":"key","text":"5","widget":"probeSpin"})", R"({"do":"endedit","widget":"probeSpin"})"},
         [&p]() {
           p.spin->clear();
           zeroCounts(p);
         },
         [&p]() {
           CShotSynth::type(p.spin, "5");
           CShotSynth::key(p.spin, Qt::Key_Tab);
         },
         [&p]() { return QString::number(p.spin->value()) + " " + countsOf(p, {"spinFinished"}); }});

  check({"a combo box chosen with the keyboard",
         {R"({"do":"set","property":"currentIndex","value":1,"widget":"probeCombo"})"},
         [&p]() {
           p.combo->setCurrentIndex(0);
           zeroCounts(p);
         },
         [&p]() { CShotSynth::key(p.combo, Qt::Key_Down); },
         [&p]() {
           return QString::number(p.combo->currentIndex()) + " " + countsOf(p, {"activated", "textActivated"});
         }});

  check({"a combo box chosen in its popup",
         {R"({"do":"set","property":"currentIndex","value":2,"widget":"probeCombo"})"},
         [&p]() {
           p.combo->setCurrentIndex(0);
           zeroCounts(p);
         },
         [&]() {
           click(p.combo, center(p.combo));
           // The popup ignores a release within its block timer.
           settle(500);
           QWidget* popup = QApplication::activePopupWidget();
           QAbstractItemView* view = (nullptr == popup) ? nullptr : popup->findChild<QAbstractItemView*>();
           if (nullptr != view) {
             click(view->viewport(), view->visualRect(view->model()->index(2, 0)).center());
           }
         },
         [&p]() {
           return QString::number(p.combo->currentIndex()) + " " + countsOf(p, {"activated", "textActivated"});
         }});

  check({"a checkable action from its tool button says the state it left",
         {R"({"action":"probeToolAction","checked":true,"do":"trigger"})"},
         [&p]() {
           p.toolAction->setChecked(false);
           zeroCounts(p);
         },
         [&]() { click(p.tool, center(p.tool)); },
         [&p]() { return QString::number(p.toolAction->isChecked()) + " " + countsOf(p, {"toolAction"}); }});

  check({"an action a slot triggers is the click that ran the slot",
         {R"({"do":"click","widget":"probeViaSlot"})"},
         [&p]() { zeroCounts(p); },
         [&]() { click(p.viaSlot, center(p.viaSlot)); },
         [&p]() { return countsOf(p, {"slotAction"}); }});

  check({"an action whose name is not unique names the widget it is in",
         {R"({"action":"probeShared","do":"trigger","widget":"probeShared2"})"},
         [&p]() { zeroCounts(p); },
         [&]() { click(p.shared2, center(p.shared2)); },
         [&p]() { return countsOf(p, {"shared1", "shared2"}); }});

  check({"an action with no parent is found through the widget it was added to",
         {R"({"action":"probeLonely","do":"trigger","widget":"probeLonelyButton"})"},
         [&p]() { zeroCounts(p); },
         [&]() { click(p.lonely, center(p.lonely)); },
         [&p]() { return countsOf(p, {"lonely"}); }});

  check({"a shortcut is its action, not its key",
         {R"({"action":"probeShortcut","do":"trigger"})"},
         [&p]() { zeroCounts(p); },
         [&p]() { CShotSynth::key(p.edit, Qt::Key_J, Qt::ControlModifier); },
         [&p]() { return countsOf(p, {"shortcut"}); }});

  check({"a modal dialog: the click that opened it comes before what was done in it",
         {R"({"do":"click","widget":"probeModal"})", R"({"do":"click","widget":"probeOk"})"},
         []() {},
         [&]() { click(p.modal, center(p.modal)); },
         []() { return QString(); },
         false});

  check({"a press on a label no handler covers records nothing",
         {},
         []() {},
         [&]() { click(p.label, center(p.label)); }});

  check({"a context menu request no menu answers is no step",
         {},
         []() {},
         [&]() { click(p.label, center(p.label), Qt::RightButton); },
         [&menus]() { return QString::number(menus.shown); }});

  check({"a right click on a tab switches nothing",
         {},
         [&p]() { fillTabs(p); },
         [&]() { click(p.tabs->tabBar(), p.tabs->tabBar()->tabRect(2).center(), Qt::RightButton); },
         [&p]() { return QString::number(p.tabs->currentIndex()); }});

  check({"Ctrl+Tab in a tab's page switches the tab",
         {R"({"do":"set","property":"currentIndex","value":1,"widget":"probeTabs/QTabBar#0"})"},
         [&p]() { fillTabs(p); },
         [&p]() { CShotSynth::key(p.tabEdit, Qt::Key_Tab, Qt::ControlModifier); },
         [&p]() { return QString::number(p.tabs->currentIndex()); }});

  // In a page, like a link in a project's details that opens a track's tab.
  check({"a tab the application switches to after a click in a page is no step",
         {R"({"do":"click","widget":"probeTabOpener"})"},
         [&p]() {
           fillTabs(p);
           QPushButton* opener = new QPushButton("open three", p.tabs->widget(1));
           opener->setObjectName("probeTabOpener");
           QObject::connect(opener, &QPushButton::clicked, p.tabs, [&p]() { p.tabs->setCurrentIndex(2); });
           p.tabs->setCurrentIndex(1);
           opener->show();
         },
         [&]() {
           QPushButton* opener = p.tabs->findChild<QPushButton*>("probeTabOpener");
           click(opener, center(opener));
         },
         [&p]() { return QString::number(p.tabs->currentIndex()); }});

  check({"closing a tab is one step, the switch it causes is none",
         {R"({"do":"close","index":0,"widget":"probeTabs/QTabBar#0"})"},
         [&p]() {
           fillTabs(p);
           p.tabs->setCurrentIndex(1);
         },
         [&]() {
           QTabBar* bar = p.tabs->tabBar();
           QWidget* close = bar->tabButton(0, QTabBar::RightSide);
           if (nullptr == close) {
             close = bar->tabButton(0, QTabBar::LeftSide);
           }
           if (nullptr != close) {
             click(close, center(close));
           }
         },
         [&p]() { return QString("%1 %2").arg(p.tabs->count()).arg(p.tabs->tabText(p.tabs->currentIndex())); }});

  check({"a header click sorts",
         {R"({"do":"click","section":0,"widget":"probeTree/QHeaderView#0"})"},
         [&p]() { p.tree->sortByColumn(0, Qt::AscendingOrder); },
         [&]() {
           QHeaderView* header = p.tree->header();
           click(header->viewport(), QPoint(header->sectionViewportPosition(0) + 10, header->height() / 2));
         },
         [&p]() { return QString::number(int(p.tree->header()->sortIndicatorOrder())); }});

  check({"a double click on a row is one step, and expands it",
         {R"({"do":"dclick","row":"0:0","widget":"probeTree"})", R"({"do":"expand","row":"0:0","widget":"probeTree"})"},
         [&p]() {
           p.tree->collapseAll();
           p.tree->sortByColumn(0, Qt::AscendingOrder);
           zeroCounts(p);
         },
         [&]() {
           const QRect& rect = p.tree->visualItemRect(p.tree->topLevelItem(0));
           CShotSynth::mouse(QTest::MouseDClick, p.tree->viewport(), Qt::LeftButton, Qt::NoModifier, rect.center());
         },
         [&p]() {
           return QString::number(p.tree->topLevelItem(0)->isExpanded()) + " " +
                  countsOf(p, {"treeClicked", "treeDouble"});
         }});

  check({"a slider moved with the keys is one value",
         {R"({"do":"set","property":"value","value":3,"widget":"probeSlider"})"},
         [&p]() { p.slider->setValue(0); },
         [&p]() {
           for (qint32 i = 0; i < 3; i++) {
             CShotSynth::key(p.slider, Qt::Key_Right);
           }
         },
         [&p]() { return QString::number(p.slider->value()); }});

  check({"a splitter dragged is the shares it was left at",
         {R"({"do":"set","property":"sizes","widget":"probeSplitter"})"},
         [&p]() {
           const qint32 half = p.splitter->width() / 2;
           p.splitter->setSizes({half, half});
         },
         [&]() {
           QWidget* handle = p.splitter->handle(1);
           const QPoint& at = center(handle);
           CShotSynth::mouse(QTest::MousePress, handle, Qt::LeftButton, Qt::NoModifier, at);
           CShotSynth::move(handle, at + QPoint(20, 0));
           CShotSynth::move(handle, at + QPoint(40, 0));
           CShotSynth::mouse(QTest::MouseRelease, handle, Qt::LeftButton, Qt::NoModifier, at + QPoint(40, 0));
         },
         [&p]() {
           const QList<int>& sizes = p.splitter->sizes();
           return QString("%1 %2").arg(sizes.value(0)).arg(sizes.value(1));
         }});

  check({"a date stepped with a key is its value, and ends with the click that leaves it",
         {R"({"do":"set","property":"dateTime","widget":"probeDate"})", R"({"do":"endedit","widget":"probeDate"})",
          R"({"do":"click","widget":"probeButton"})"},
         [&p]() {
           p.date->setDateTime(QDateTime(QDate(2026, 1, 1), QTime(0, 0)));
           zeroCounts(p);
         },
         [&]() {
           CShotSynth::key(p.date, Qt::Key_Up);
           click(p.button, center(p.button));
         },
         [&p]() { return p.date->dateTime().toString(Qt::ISODate) + " " + countsOf(p, {"dateFinished", "button"}); }});

  check({"a checkable group box clicked on its check box",
         {R"({"checked":false,"do":"click","widget":"probeGroup"})"},
         [&p]() { p.group->setChecked(true); },
         [&]() { click(p.group, groupCheckRect(p.group).center()); },
         [&p]() { return QString::number(p.group->isChecked()); }});

  check({"typing into a widget no handler covers is its key presses",
         {R"({"do":"keypress","key":"A","text":"a","widget":"probeText"})",
          R"({"do":"keypress","key":"B","text":"b","widget":"probeText"})"},
         [&p]() { p.text->clear(); },
         [&p]() { CShotSynth::type(p.text, "ab"); },
         [&p]() { return p.text->toPlainText(); }});

  auto showDialog = [&p]() {
    p.dialogEdit->clear();
    p.dialogCheck->setChecked(false);
    zeroCounts(p);
    p.dialog->move(p.window->geometry().left(), p.window->geometry().bottom() + 50);
    p.dialog->show();
    if (!QTest::qWaitForWindowExposed(p.dialog)) {
      qWarning() << "shoot: the self test's dialog never came up";
    }
  };

  check({"Enter in a dialog's line edit is the typing, not the default button",
         {R"({"do":"key","enter":true,"text":"x","widget":"probeDialogEdit"})"},
         showDialog,
         [&p]() {
           CShotSynth::type(p.dialogEdit, "x");
           CShotSynth::key(p.dialogEdit, Qt::Key_Return);
         },
         [&p]() { return countsOf(p, {"accepted", "rejected"}) + " " + QString::number(p.dialog->isVisible()); }});

  check({"Escape in a dialog is a key press",
         {R"({"do":"keypress","key":"Esc","widget":"probeDialogEdit"})"},
         showDialog,
         [&p]() { CShotSynth::key(p.dialogEdit, Qt::Key_Escape); },
         [&p]() { return countsOf(p, {"accepted", "rejected"}) + " " + QString::number(p.dialog->isVisible()); }});

  check({"Return on a dialog's check box is a key press",
         {R"({"do":"keypress","key":"Return","widget":"probeDialogCheck"})"},
         showDialog,
         [&p]() { CShotSynth::key(p.dialogCheck, Qt::Key_Return); },
         [&p]() { return countsOf(p, {"accepted", "rejected"}) + " " + QString::number(p.dialog->isVisible()); }});

  check({"a double spin box typed into is its text",
         {R"({"do":"key","text":"2.5","widget":"probeDoubleSpin"})"},
         [&p]() { p.doubleSpin->clear(); },
         [&p]() { CShotSynth::type(p.doubleSpin, "2.5"); },
         [&p]() { return QString::number(p.doubleSpin->value()); }});

  check({"typing into an editable combo box is its text",
         {R"({"do":"key","text":"gamma","widget":"probeEditable"})"},
         [&p]() { p.editable->setEditText(QString()); },
         [&p]() { CShotSynth::type(p.editable, "gamma"); },
         [&p]() { return p.editable->currentText(); }});

  check({"an entry picked in a tool button's menu is the menu opened and the action",
         {R"({"do":"openmenu","widget":"probeMenuButton"})", R"({"action":"probeMenuEntry","do":"trigger"})"},
         [&p]() { zeroCounts(p); },
         [&]() {
           // An instant popup exec()s its menu from the press; the entry is picked inside that loop.
           QTimer::singleShot(300, p.window, [&p]() {
             QMenu* menu = p.menuButton->menu();
             if (menu->isVisible()) {
               CShotSynth::mouse(QTest::MouseClick, menu, Qt::LeftButton, Qt::NoModifier,
                                 menu->actionGeometry(p.menuEntry).center());
             }
           });
           click(p.menuButton, center(p.menuButton));
         },
         [&p]() { return countsOf(p, {"menuEntry"}) + " " + QString::number(p.menuButton->menu()->isVisible()); }});

  check({"a context menu on a widget with no rows is a place in it",
         {R"({"do":"menu","widget":"probeEdit"})"},
         []() {},
         [&]() { click(p.edit, center(p.edit), Qt::RightButton); },
         [&menus]() { return QString::number(menus.shown); },
         true,
         true});

  check({"a menu a tool button opens and Escape closes is opened again by the replay",
         {R"({"do":"openmenu","widget":"probeMenuButton"})",
          R"({"do":"keypress","key":"Esc","widget":"probeMenuButton/QMenu#0"})"},
         []() {},
         [&]() {
           QTimer::singleShot(300, p.window, [&p]() {
             if (QMenu* menu = p.menuButton->menu(); menu->isVisible()) {
               QTest::keyClick(menu->windowHandle(), Qt::Key_Escape);
             }
           });
           click(p.menuButton, center(p.menuButton));
           settle(200);
         },
         [&menus]() { return QString::number(menus.shown); }});

  // xcb and Wayland queue the context menu before the menu key itself (qxcbkeyboard.cpp).
  auto escapeMenu = [](QWidget* at, qint32 after = 300) {
    QTimer::singleShot(after, at, []() {
      if (QMenu* menu = qobject_cast<QMenu*>(QApplication::activePopupWidget()); nullptr != menu) {
        QTest::keyClick(menu->windowHandle(), Qt::Key_Escape);
      }
    });
  };

  check({"a menu an action's tool button opens on its arrow is the action's",
         {R"({"action":"probeMenuAction","do":"openmenu"})",
          R"({"do":"keypress","key":"Esc","widget":"probeActionMenu"})"},
         []() {},
         [&]() {
           QStyleOptionToolButton option;
           option.initFrom(p.actionMenuButton);
           option.subControls = QStyle::SC_All;
           option.features = QStyleOptionToolButton::MenuButtonPopup | QStyleOptionToolButton::HasMenu;
           const QRect& arrow = p.actionMenuButton->style()->subControlRect(
               QStyle::CC_ToolButton, &option, QStyle::SC_ToolButtonMenu, p.actionMenuButton);
           escapeMenu(p.window);
           click(p.actionMenuButton, arrow.center());
           settle(200);
         },
         [&menus]() { return QString::number(menus.shown); }});

  check({"a menu a tool button opens after the press is held is opened again by the replay",
         {R"({"do":"openmenu","widget":"probeDelayedButton"})",
          R"({"do":"keypress","key":"Esc","widget":"probeDelayedButton/QMenu#0"})"},
         []() {},
         [&]() {
           escapeMenu(p.window, p.delayedButton->style()->styleHint(QStyle::SH_ToolButton_PopupDelay) + 300);
           CShotSynth::mouse(QTest::MousePress, p.delayedButton, Qt::LeftButton, Qt::NoModifier,
                             center(p.delayedButton));
           QTest::qWait(p.delayedButton->style()->styleHint(QStyle::SH_ToolButton_PopupDelay) + 600);
           CShotSynth::mouse(QTest::MouseRelease, p.delayedButton, Qt::LeftButton, Qt::NoModifier,
                             center(p.delayedButton));
           settle(200);
         },
         [&menus]() { return QString::number(menus.shown); }});

  check({"a menu bar's menu, a submenu of it and an entry in that are each a step",
         {R"({"do":"openmenu","menu":"probeBarMenu","widget":"probeBar"})",
          R"({"do":"openmenu","menu":"probeSubMenu"})", R"({"action":"probeSubEntry","do":"trigger"})"},
         [&p]() { zeroCounts(p); },
         [&]() {
           QMenu* barMenu = p.window->findChild<QMenu*>("probeBarMenu");
           QMenu* subMenu = p.window->findChild<QMenu*>("probeSubMenu");
           click(p.bar, p.bar->actionGeometry(barMenu->menuAction()).center());
           settle(300);
           if (!barMenu->isVisible()) {
             return;
           }
           CShotSynth::arrive(barMenu, barMenu->actionGeometry(subMenu->menuAction()).center());
           QTest::qWait(barMenu->style()->styleHint(QStyle::SH_Menu_SubMenuPopupDelay, nullptr, barMenu) + 600);
           if (!subMenu->isVisible()) {
             return;
           }
           const QPoint& entry = subMenu->actionGeometry(subMenu->actions().first()).center();
           CShotSynth::arrive(subMenu, entry);
           CShotSynth::mouse(QTest::MouseClick, subMenu, Qt::LeftButton, Qt::NoModifier, entry);
           settle(300);
         },
         [&p]() {
           return countsOf(p, {"subEntry"}) + " " + QString::number(nullptr != QApplication::activePopupWidget());
         }});

  check({"the menu key is the menu it opens",
         {R"({"do":"menu","widget":"probeEdit"})"},
         []() {},
         [&]() {
           CShotSynth::focus(p.edit);
           QWindow* window = p.window->windowHandle();
           const QPoint& pos = p.edit->mapTo(p.window, center(p.edit));
           QWindowSystemInterface::handleContextMenuEvent(window, false, pos, window->mapToGlobal(pos), Qt::NoModifier);
           QWindowSystemInterface::handleKeyEvent(window, QEvent::KeyPress, Qt::Key_Menu, Qt::NoModifier);
           QWindowSystemInterface::handleKeyEvent(window, QEvent::KeyRelease, Qt::Key_Menu, Qt::NoModifier);
           settle(200);
         },
         [&menus]() { return QString::number(menus.shown); },
         true,
         true});

  check({"a completion picked with the keys is the text it ends with",
         {R"({"do":"key","enter":true,"text":"alpha","widget":"probeCompleted"})"},
         [&p]() {
           p.completed->clear();
           p.completed->setProperty("returns", 0);
         },
         [&]() {
           CShotSynth::type(p.completed, "a");
           settle(200);
           if (QAbstractItemView* popup = p.completed->completer()->popup(); popup->isVisible()) {
             QTest::keyClick(popup->windowHandle(), Qt::Key_Down);
             QTest::keyClick(popup->windowHandle(), Qt::Key_Return);
           }
           settle(200);
         },
         [&p]() {
           return p.completed->text() + " " + QString::number(p.completed->completer()->popup()->isVisible()) +
                  " returns " + QString::number(p.completed->property("returns").toInt());
         }});

  {
    auto* doomed = new CShotRecorder(ctx);
    doomed->start();
    delete doomed;
    // A listener left behind calls into the deleted recorder.
    const bool left = CShotApplication::hasListener();
    if (!left) {
      click(p.button, center(p.button));
      settle();
    }
    verdict("a recorder destroyed while recording leaves nothing behind", !left);
  }

  // As CProjectFilterItem::showLineEdit(): created on the tree, then put into a row, replaced when shown again.
  auto showFilter = [&p]() {
    QTreeWidgetItem* row = nullptr;
    for (qint32 i = 0; i < p.tree->topLevelItemCount(); i++) {
      if ("zz filter" == p.tree->topLevelItem(i)->text(0)) {
        row = p.tree->topLevelItem(i);
      }
    }
    if (nullptr == row) {
      row = new QTreeWidgetItem(p.tree, {"zz filter"});
    }
    if (QWidget* old = p.tree->itemWidget(row, 0); nullptr != old) {
      old->setObjectName(QString());
      old->deleteLater();
    }
    QLineEdit* filter = new QLineEdit(p.tree);
    filter->setObjectName("probeFilter");
    p.tree->setItemWidget(row, 0, filter);
    return filter;
  };

  check({"typing into a filter box put into a row is typing, not a cell editor",
         {R"({"do":"key","text":"abc","widget":"probeFilter"})"},
         [&showFilter]() {
           showFilter();
           settle(100);
         },
         [&]() {
           QLineEdit* filter = showFilter();
           settle(100);
           CShotSynth::type(filter, "abc");
         },
         [&p]() {
           const QLineEdit* filter = p.tree->findChild<QLineEdit*>("probeFilter");
           return (nullptr == filter) ? QString("no filter") : filter->text();
         }});

  check({"an entry picked in a menu decided by exec()'s return value",
         {R"({"do":"click","widget":"probeExecMenu"})", R"({"action":"probeExecTwo","do":"trigger"})"},
         [&p]() { zeroCounts(p); },
         [&]() {
           QTimer::singleShot(200, p.window, [&p]() {
             QMenu* menu = p.window->findChild<QMenu*>("probeExecMenuMenu");
             const QAction* two = p.window->findChild<QAction*>("probeExecTwo");
             if (nullptr != menu && menu->isVisible()) {
               CShotSynth::mouse(QTest::MouseClick, menu, Qt::LeftButton, Qt::NoModifier,
                                 menu->actionGeometry(const_cast<QAction*>(two)).center());
             }
           });
           click(p.execMenu, center(p.execMenu));
           settle(300);
         },
         [&p]() { return countsOf(p, {"exec:probeExecOne", "exec:probeExecTwo"}); }});

  check({"a completion picked from a line edit's completer is its text",
         {R"({"do":"key","text":"apple","widget":"probeCompleted"})"},
         [&p]() { p.completed->clear(); },
         [&]() {
           CShotSynth::type(p.completed, "a");
           settle(200);
           QAbstractItemView* popup = p.completed->completer()->popup();
           if (popup->isVisible()) {
             for (qint32 i = 0; i < popup->model()->rowCount(); i++) {
               const QModelIndex& index = popup->model()->index(i, 0);
               if ("apple" == index.data().toString()) {
                 click(popup->viewport(), popup->visualRect(index).center());
               }
             }
           }
           settle(200);
         },
         [&p]() { return p.completed->text(); }});

  check({"a list scrolled with the wheel is the row it shows at the top",
         {R"({"do":"scroll","widget":"probeLong"})"},
         [&p]() { p.longList->scrollToTop(); },
         [&]() {
           const QPoint& at = center(p.longList->viewport());
           CShotSynth::move(p.longList->viewport(), at);
           CShotSynth::wheel(p.longList->viewport(), at, QPoint(0, -120), Qt::NoModifier);
           CShotSynth::wheel(p.longList->viewport(), at, QPoint(0, -120), Qt::NoModifier);
           settle(200);
         },
         [&p]() { return p.longList->indexAt(QPoint(1, 1)).data().toString(); }});

  {
    const std::optional<QString>& address = recorder.address(main);
    const bool named = address.has_value() && address->isEmpty() &&
                       CShotHandlers::resolve(main, QJsonObject{{"widget", *address}}) == main;
    verdict("the main window is named by the empty address, and found by it", named);
  }

  check({"Space on a check box is its click, not a key press as well",
         {R"({"checked":true,"do":"click","widget":"probeCheck"})"},
         [&p]() { p.check->setChecked(false); },
         [&p]() { CShotSynth::key(p.check, Qt::Key_Space); },
         [&p]() { return QString::number(p.check->isChecked()); }});

  check({"Space held on a button while the pointer moves is its click alone",
         {R"({"do":"click","widget":"probeButton"})"},
         [&p]() { zeroCounts(p); },
         [&]() {
           CShotSynth::focus(p.button);
           QWindow* window = p.button->window()->windowHandle();
           QTest::keyPress(window, Qt::Key_Space);
           CShotSynth::move(p.button, center(p.button) + QPoint(2, 0));
           QTest::keyRelease(window, Qt::Key_Space);
         },
         [&p]() { return countsOf(p, {"button"}); }});

  check({"after a completion, the next click goes where it was aimed",
         {R"({"do":"key","text":"apple","widget":"probeCompleted"})", R"({"do":"endedit","widget":"probeCompleted"})",
          R"({"do":"select","row":"2:0","widget":"probeLong"})"},
         [&p]() {
           p.completed->clear();
           p.longList->scrollToTop();
           p.longList->setCurrentRow(0);
         },
         [&]() {
           CShotSynth::type(p.completed, "a");
           settle(200);
           QAbstractItemView* popup = p.completed->completer()->popup();
           for (qint32 i = 0; popup->isVisible() && i < popup->model()->rowCount(); i++) {
             const QModelIndex& index = popup->model()->index(i, 0);
             if ("apple" == index.data().toString()) {
               click(popup->viewport(), popup->visualRect(index).center());
             }
           }
           settle(200);
           click(p.longList->viewport(), p.longList->visualItemRect(p.longList->item(2)).center());
         },
         [&p]() { return p.completed->text() + " " + QString::number(p.longList->currentRow()); }});

  check(
      {"arrow keys in a menu and Enter pick the entry they reach",
       {R"({"do":"click","widget":"probeExecMenu"})", R"({"do":"keypress","key":"Down","widget":"probeExecMenuMenu"})",
        R"({"action":"probeExecOne","do":"trigger"})"},
       [&p]() { zeroCounts(p); },
       [&]() {
         QTimer::singleShot(200, p.window, [&p]() {
           QMenu* menu = p.window->findChild<QMenu*>("probeExecMenuMenu");
           if (nullptr != menu && menu->isVisible()) {
             QTest::keyClick(menu->windowHandle(), Qt::Key_Down);
             QTest::keyClick(menu->windowHandle(), Qt::Key_Return);
           }
         });
         click(p.execMenu, center(p.execMenu));
         settle(300);
       },
       [&p]() { return countsOf(p, {"exec:probeExecOne", "exec:probeExecTwo"}); }});

  // ---- the workspace

  check(
      {"a workspace row is its name path",
       {QString(R"({"do":"select","row":"%1","widget":"treeWks"})").arg(projectPath)},
       [wks]() { wks->setCurrentItem(nullptr); },
       [&]() { click(wks->viewport(), wks->visualItemRect(project).center()); },
       [wks]() { return (nullptr == wks->currentItem()) ? QString() : CShotAddress::itemPathOf(wks->currentItem()); }});

  check({"expanding a project by its branch",
         {QString(R"({"do":"expand","row":"%1","widget":"treeWks"})").arg(projectPath)},
         [project]() { project->setExpanded(false); },
         [&]() {
           const QRect& rect = wks->visualItemRect(project);
           click(wks->viewport(), QPoint(rect.left() - 8, rect.center().y()));
         },
         [project]() { return QString::number(project->isExpanded()); }});

  check({"a context menu on a workspace row",
         {QString(R"({"do":"menu","row":"%1","widget":"treeWks"})").arg(projectPath)},
         []() {},
         [&]() { click(wks->viewport(), wks->visualItemRect(project).center(), Qt::RightButton); },
         [&menus]() { return QString::number(menus.shown); },
         true,
         true});

  // ---- the map

  const QVector<CTrackData::trkpt_t>& pts = trk->getTrackData().segs.first().pts;
  const CTrackData::trkpt_t& first = pts[pts.size() / 3];
  const CTrackData::trkpt_t& second = pts[2 * pts.size() / 3];
  const QPointF middle = QPointF((first.lon + second.lon) / 2, (first.lat + second.lat) / 2) * DEG_TO_RAD;
  auto resetMap = [canvas, middle]() {
    CGisWorkspace::self().slotWksItemSelectionReset();
    canvas->resetMouse();
    QApplication::sendPostedEvents(nullptr, QEvent::DeferredDelete);
    canvas->moveTo(middle);
    QTest::qWait(300);
  };
  auto pixelOf = [canvas](const CTrackData::trkpt_t& pt) {
    QPointF pos(pt.lon, pt.lat);
    pos *= DEG_TO_RAD;
    canvas->convertRad2Px(pos);
    return pos.toPoint();
  };
  auto hover = [canvas](const QPoint& at) {
    CShotSynth::move(canvas, at);
    // A hover focuses an item at the next paint, not at the move.
    QTest::qWait(250);
  };

  check({"the map's address is its place, not the key it names itself with",
         {},
         []() {},
         []() {},
         [main, canvas]() {
           const QString& address = CShotAddress::addressOf(main, canvas).value_or(QString());
           return QString("%1 %2").arg(address.contains("key_")).arg(CShotAddress::resolve(main, address) == canvas);
         }});

  check({"a click on the map is the hover before it and the click",
         {R"({"do":"move","widget":"%MAP%"})", R"({"button":"left","do":"click","widget":"%MAP%"})"},
         resetMap,
         [&]() {
           const QPoint& at = emptyPlace(canvas);
           hover(at + QPoint(0, -15));
           hover(at);
           click(canvas, at);
         },
         [canvas]() { return viewState(canvas); }});

  check({"the wheel zooms from the view the recording started in",
         {R"({"do":"move","widget":"%MAP%"})", R"({"angle":[0,120],"do":"wheel","widget":"%MAP%"})"},
         resetMap,
         [&]() {
           const QPoint& at = emptyPlace(canvas);
           hover(at + QPoint(0, -15));
           hover(at);
           CShotSynth::wheel(canvas, at, QPoint(0, 120), Qt::NoModifier);
           settle(300);
         },
         [canvas]() { return viewState(canvas); }});

  check({"dragging the map is a drag, and pans as far again",
         {R"({"do":"move","widget":"%MAP%"})", R"({"button":"left","do":"drag","dx":40,"dy":60,"widget":"%MAP%"})"},
         resetMap,
         [&]() {
           const QPoint& at = emptyPlace(canvas);
           hover(at + QPoint(0, -15));
           hover(at);
           CShotSynth::mouse(QTest::MousePress, canvas, Qt::LeftButton, Qt::NoModifier, at);
           CShotSynth::move(canvas, at + QPoint(20, 30));
           CShotSynth::move(canvas, at + QPoint(40, 60));
           CShotSynth::mouse(QTest::MouseRelease, canvas, Qt::LeftButton, Qt::NoModifier, at + QPoint(40, 60));
           settle(300);
         },
         [canvas]() { return viewState(canvas); }});

  check({"a wheel turned while the button is down on the map is between the press and the release",
         {R"({"do":"move","widget":"%MAP%"})", R"({"button":"left","do":"press","widget":"%MAP%"})",
          R"({"angle":[0,120],"do":"wheel","widget":"%MAP%"})",
          R"({"button":"left","do":"release","dx":0,"dy":0,"widget":"%MAP%"})"},
         resetMap,
         [&]() {
           const QPoint& at = emptyPlace(canvas);
           hover(at + QPoint(0, -15));
           hover(at);
           CShotSynth::mouse(QTest::MousePress, canvas, Qt::LeftButton, Qt::NoModifier, at);
           CShotSynth::wheel(canvas, at, QPoint(0, 120), Qt::NoModifier);
           settle(300);
           CShotSynth::mouse(QTest::MouseRelease, canvas, Qt::LeftButton, Qt::NoModifier, at);
           settle(300);
         },
         [canvas]() { return viewState(canvas); }});

  check({"a wheel turned during a drag on the map is where it was in the drag",
         {R"({"do":"move","widget":"%MAP%"})", R"({"button":"left","do":"press","widget":"%MAP%"})",
          R"({"do":"move","dx":20,"dy":-30,"widget":"%MAP%"})", R"({"angle":[0,120],"do":"wheel","widget":"%MAP%"})",
          R"({"do":"move","dx":40,"dy":-60,"widget":"%MAP%"})",
          R"({"button":"left","do":"release","dx":40,"dy":-60,"widget":"%MAP%"})"},
         resetMap,
         [&]() {
           const QPoint& at = emptyPlace(canvas);
           hover(at + QPoint(0, -15));
           hover(at);
           CShotSynth::mouse(QTest::MousePress, canvas, Qt::LeftButton, Qt::NoModifier, at);
           CShotSynth::move(canvas, at + QPoint(10, -15));
           CShotSynth::move(canvas, at + QPoint(20, -30));
           CShotSynth::wheel(canvas, at + QPoint(20, -30), QPoint(0, 120), Qt::NoModifier);
           settle(300);
           CShotSynth::move(canvas, at + QPoint(30, -45));
           CShotSynth::move(canvas, at + QPoint(40, -60));
           CShotSynth::mouse(QTest::MouseRelease, canvas, Qt::LeftButton, Qt::NoModifier, at + QPoint(40, -60));
           settle(300);
         },
         [canvas]() { return viewState(canvas); }});

  check({"an arrow key during a drag on the map is where it was in the drag",
         {R"({"do":"move","widget":"%MAP%"})", R"({"button":"left","do":"press","widget":"%MAP%"})",
          R"({"do":"move","dx":20,"dy":-30,"widget":"%MAP%"})", R"({"do":"keypress","key":"Up","widget":"%MAP%"})",
          R"({"do":"move","dx":40,"dy":-60,"widget":"%MAP%"})",
          R"({"button":"left","do":"release","dx":40,"dy":-60,"widget":"%MAP%"})"},
         resetMap,
         [&]() {
           const QPoint& at = emptyPlace(canvas);
           hover(at + QPoint(0, -15));
           hover(at);
           CShotSynth::mouse(QTest::MousePress, canvas, Qt::LeftButton, Qt::NoModifier, at);
           CShotSynth::move(canvas, at + QPoint(10, -15));
           CShotSynth::move(canvas, at + QPoint(20, -30));
           QTest::keyClick(canvas->window()->windowHandle(), Qt::Key_Up);
           settle(300);
           CShotSynth::move(canvas, at + QPoint(30, -45));
           CShotSynth::move(canvas, at + QPoint(40, -60));
           CShotSynth::mouse(QTest::MouseRelease, canvas, Qt::LeftButton, Qt::NoModifier, at + QPoint(40, -60));
           settle(300);
         },
         [canvas]() { return viewState(canvas); }});

  check({"the arrow keys pan the map, as key presses",
         {R"({"do":"keypress","key":"Up","widget":"%MAP%"})"},
         resetMap,
         [canvas]() {
           CShotSynth::key(canvas, Qt::Key_Up);
           QTest::qWait(300);
         },
         [canvas]() { return viewState(canvas); }});

  check({"a range selected on the track, from the hover to the second click",
         {R"({"do":"move","hit":"%TRK%","widget":"%MAP%"})",
          R"({"button":"left","do":"click","hit":"%TRK%","widget":"%MAP%"})", R"({"do":"click","widget":"%RANGE%"})",
          R"({"do":"move","hit":"%TRK%","widget":"%MAP%"})",
          R"({"button":"left","do":"click","hit":"%TRK%","widget":"%MAP%"})",
          R"({"do":"move","hit":"%TRK%","widget":"%MAP%"})",
          R"({"button":"left","do":"click","hit":"%TRK%","widget":"%MAP%"})"},
         resetMap,
         [&]() {
           const QPoint& p1 = pixelOf(first);
           const QPoint& p2 = pixelOf(second);
           hover(p1);
           click(canvas, p1);
           QTest::qWait(250);
           if (QToolButton* range = canvas->findChild<QToolButton*>("toolRange"); nullptr != range) {
             names["%RANGE%"] = CShotAddress::addressOf(main, range).value_or(QString());
             click(range, center(range));
             QTest::qWait(250);
           }
           // Off the track and back: a move to the current position is no move.
           hover(p1 + QPoint(0, 40));
           hover(p1);
           click(canvas, p1);
           QTest::qWait(250);
           hover(p2);
           click(canvas, p2);
           QTest::qWait(250);
         },
         [trk]() { return QString::number(int(trk->getRangState())); }});

  check({"a wheel turned while the button is down on the track is no click on it",
         {R"({"do":"move","hit":"%TRK%","widget":"%MAP%"})",
          R"({"button":"left","do":"press","hit":"%TRK%","widget":"%MAP%"})",
          R"({"angle":[0,60],"do":"wheel","hit":"%TRK%","widget":"%MAP%"})",
          R"({"button":"left","do":"release","dx":0,"dy":0,"widget":"%MAP%"})"},
         resetMap,
         [&]() {
           const QPoint& p1 = pixelOf(first);
           hover(p1);
           CShotSynth::mouse(QTest::MousePress, canvas, Qt::LeftButton, Qt::NoModifier, p1);
           // Less than a zoom step: CMouseAdapter treats any wheel as "no click".
           CShotSynth::wheel(canvas, p1, QPoint(0, 60), Qt::NoModifier);
           settle(100);
           CShotSynth::mouse(QTest::MouseRelease, canvas, Qt::LeftButton, Qt::NoModifier, p1);
           QTest::qWait(250);
         },
         [canvas]() {
           const QToolButton* range = canvas->findChild<QToolButton*>("toolRange");
           return QString("screen options shown: %1").arg(nullptr != range && range->isVisible());
         }});

  check({"a press still held when the recording stops is no step",
         {R"({"do":"move","widget":"%MAP%"})"},
         resetMap,
         [&]() {
           const QPoint& at = emptyPlace(canvas);
           hover(at + QPoint(0, -15));
           hover(at);
           CShotSynth::mouse(QTest::MousePress, canvas, Qt::LeftButton, Qt::NoModifier, at);
           // The recording stops with the button down; the next case's reset releases it.
           QTimer::singleShot(400, canvas, [canvas, at]() {
             CShotSynth::mouse(QTest::MouseRelease, canvas, Qt::LeftButton, Qt::NoModifier, at);
           });
         },
         []() { return QString(); },
         false});
  settle(500);

  check({"a hover goes on after a key press as a hover of its own",
         {R"({"do":"move","widget":"%MAP%"})", R"({"do":"keypress","key":"Up","widget":"%MAP%"})",
          R"({"do":"move","widget":"%MAP%"})"},
         resetMap,
         [&]() {
           const QPoint& at = emptyPlace(canvas);
           hover(at + QPoint(0, -15));
           hover(at);
           CShotSynth::key(canvas, Qt::Key_Up);
           QTest::qWait(300);
           hover(at + QPoint(0, 15));
         },
         [canvas]() { return viewState(canvas); }});

  check({"a pinch on the map is reported, as no step holds it",
         {},
         resetMap,
         [&]() {
           static QPointingDevice* const touchscreen = QTest::createTouchDevice();
           const QPoint& at = emptyPlace(canvas);
           const QString before = viewState(canvas);
           // A child widget has no window handle, which the widget sequence requires.
           QTest::touchEvent(canvas->window(), touchscreen)
               .press(0, at - QPoint(20, 0), canvas)
               .press(1, at + QPoint(20, 0), canvas);
           for (qint32 i = 1; i <= 10; i++) {
             QTest::touchEvent(canvas->window(), touchscreen)
                 .move(0, at - QPoint(20 + 8 * i, 0), canvas)
                 .move(1, at + QPoint(20 + 8 * i, 0), canvas);
             settle(20);
           }
           QTest::touchEvent(canvas->window(), touchscreen)
               .release(0, at - QPoint(100, 0), canvas)
               .release(1, at + QPoint(100, 0), canvas);
           settle(300);
           qDebug().noquote() << "shoot: the pinch changed the view:" << (before != viewState(canvas));
         },
         [canvas]() { return viewState(canvas); },
         false,
         false,
         false,
         "touch and touchpad gestures are not recorded"});

  check({"a double click on the map is one step",
         {R"({"do":"move","widget":"%MAP%"})", R"({"button":"left","do":"dclick","widget":"%MAP%"})"},
         resetMap,
         [&]() {
           const QPoint& at = emptyPlace(canvas);
           hover(at + QPoint(0, -15));
           hover(at);
           CShotSynth::mouse(QTest::MouseDClick, canvas, Qt::LeftButton, Qt::NoModifier, at);
           settle(300);
         },
         [canvas]() { return viewState(canvas); }});

  check({"a double click with a move between its clicks is one step",
         {R"({"do":"move","widget":"%MAP%"})", R"({"do":"move","widget":"%MAP%"})",
          R"({"button":"left","do":"dclick","widget":"%MAP%"})"},
         resetMap,
         [&]() {
           const QPoint& at = emptyPlace(canvas);
           hover(at + QPoint(0, -15));
           hover(at);
           CShotSynth::mouse(QTest::MousePress, canvas, Qt::LeftButton, Qt::NoModifier, at, 10);
           CShotSynth::mouse(QTest::MouseRelease, canvas, Qt::LeftButton, Qt::NoModifier, at, 10);
           CShotSynth::move(canvas, at + QPoint(1, 0));
           CShotSynth::mouse(QTest::MousePress, canvas, Qt::LeftButton, Qt::NoModifier, at + QPoint(1, 0), 10);
           CShotSynth::mouse(QTest::MouseRelease, canvas, Qt::LeftButton, Qt::NoModifier, at + QPoint(1, 0), 10);
           settle(300);
         },
         [canvas]() { return viewState(canvas); }});

  check({"a press held on the map says how long",
         {R"({"do":"move","widget":"%MAP%"})", R"({"button":"left","do":"click","widget":"%MAP%"})"},
         resetMap,
         [&]() {
           const QPoint& at = emptyPlace(canvas);
           hover(at + QPoint(0, -15));
           hover(at);
           CShotSynth::mouse(QTest::MousePress, canvas, Qt::LeftButton, Qt::NoModifier, at);
           QTest::qWait(900);
           CShotSynth::mouse(QTest::MouseRelease, canvas, Qt::LeftButton, Qt::NoModifier, at);
           settle(300);
         },
         // A press held past CMouseAdapter::longButtonPressTimeout opens the map's context menu.
         [canvas, &menus]() { return viewState(canvas) + " menus=" + QString::number(menus.shown); },
         true,
         true});

  {
    QDockWidget* dock = nullptr;
    const QList<QDockWidget*>& docks = main->findChildren<QDockWidget*>();
    for (QDockWidget* candidate : docks) {
      if (candidate->isVisible() && !candidate->isFloating()) {
        dock = candidate;
        break;
      }
    }
    if (nullptr == dock) {
      verdict("a dock made to float by its title is one step", false, "the main window has no dock");
    } else {
      const QString dockAddress = CShotAddress::addressOf(main, dock).value_or(QString());
      const Qt::DockWidgetArea area = main->dockWidgetArea(dock);
      check({"a dock made to float by its title is one step",
             {QString(R"({"area":%1,"do":"arrange","floating":true,"widget":"%2"})").arg(int(area)).arg(dockAddress)},
             [dock]() {
               dock->setFloating(false);
               QTest::qWait(200);
             },
             [&]() {
               // A title bar double click floats the dock; QMapShack's title bars are vertical, on the left.
               const bool vertical = dock->features().testFlag(QDockWidget::DockWidgetVerticalTitleBar);
               const QPoint at = vertical ? QPoint(6, dock->height() / 2) : QPoint(dock->width() / 2, 6);
               if (nullptr != dock->childAt(at)) {
                 qWarning() << "shoot: the case double clicks the dock's content, not its title bar";
               }
               CShotSynth::mouse(QTest::MouseDClick, dock, Qt::LeftButton, Qt::NoModifier, at);
               settle(300);
             },
             [dock, main]() { return QString("%1 %2").arg(dock->isFloating()).arg(int(main->dockWidgetArea(dock))); }});
      dock->setFloating(false);
    }
  }

  // ---- the plot and the icon grid

  auto resetPlot = [&p, trk, canvas]() {
    canvas->resetMouse();
    QApplication::sendPostedEvents(nullptr, QEvent::DeferredDelete);
    // Slots, not actions: IPlot enables its actions only while its context menu is open.
    QMetaObject::invokeMethod(p.plot, "slotStopRange");
    QMetaObject::invokeMethod(p.plot, "slotResetZoom");
    QTest::qWait(200);
  };
  auto plotState = [&p, trk]() {
    const CTrackData::trkpt_t* focus = trk->getMouseMoveFocusPoint();
    const QPoint middleOfPlot(p.plot->width() / 2, p.plot->height() / 2);
    return QString("%1 %2 %3")
        .arg(nullptr == focus ? -1 : focus->idxTotal)
        .arg(p.plot->xValueAt(middleOfPlot), 0, 'f', 1)
        .arg(int(trk->getRangState()));
  };

  check({"a plot click is its x value",
         {R"({"do":"move","widget":"probePlot"})", R"({"button":"left","do":"click","widget":"probePlot"})"},
         resetPlot,
         [&]() {
           const QPoint& at = center(p.plot);
           CShotSynth::move(p.plot, at);
           click(p.plot, at);
         },
         plotState});

  check({"a plot click beside its graph area reads the edge",
         {R"({"do":"move","widget":"probePlot"})", R"({"button":"left","do":"click","widget":"probePlot"})"},
         resetPlot,
         [&]() {
           const QPoint at(2, p.plot->height() / 2);
           CShotSynth::move(p.plot, at);
           click(p.plot, at);
         },
         plotState});

  check({"dragging a zoomed plot is a drag, and pans as far again",
         {R"({"do":"move","widget":"probePlot"})", R"({"angle":[0,120],"do":"wheel","widget":"probePlot"})",
          R"({"button":"left","do":"drag","dx":60,"dy":0,"widget":"probePlot"})"},
         resetPlot,
         [&]() {
           const QPoint& at = center(p.plot);
           CShotSynth::move(p.plot, at);
           CShotSynth::wheel(p.plot, at, QPoint(0, 120), Qt::NoModifier);
           settle(150);
           CShotSynth::mouse(QTest::MousePress, p.plot, Qt::LeftButton, Qt::NoModifier, at);
           CShotSynth::move(p.plot, at + QPoint(30, 0));
           CShotSynth::move(p.plot, at + QPoint(60, 0));
           CShotSynth::mouse(QTest::MouseRelease, p.plot, Qt::LeftButton, Qt::NoModifier, at + QPoint(60, 0));
         },
         plotState});

  check({"after a right click opened the plot's menu, the plot is still recorded",
         {R"({"do":"move","widget":"probePlot"})", R"({"button":"right","do":"press","widget":"probePlot"})",
          R"({"do":"keypress","key":"Esc","widget":"probePlot/QMenu#0"})",
          R"({"button":"right","do":"release","dx":0,"dy":0,"widget":"probePlot"})",
          R"({"button":"left","do":"click","widget":"probePlot"})"},
         resetPlot,
         [&]() {
           const QPoint& at = center(p.plot);
           CShotSynth::move(p.plot, at + QPoint(-10, 0));
           CShotSynth::move(p.plot, at);
           // The menu opens on the press; the release comes while it is shown.
           QTimer::singleShot(150, p.plot, [&p, at]() {
             CShotSynth::mouse(QTest::MouseRelease, p.plot, Qt::RightButton, Qt::NoModifier, at);
             QTimer::singleShot(150, p.plot, []() {
               if (QMenu* menu = qobject_cast<QMenu*>(QApplication::activePopupWidget()); nullptr != menu) {
                 QTest::keyClick(menu->windowHandle(), Qt::Key_Escape);
               }
             });
           });
           CShotSynth::mouse(QTest::MousePress, p.plot, Qt::RightButton, Qt::NoModifier, at);
           settle(400);
           click(p.plot, at);
         },
         [&menus, plotState]() { return plotState() + " menus=" + QString::number(menus.shown); },
         true,
         false,
         false});

  check({"a wheel on the plot with Alt held zooms one axis, and replays so",
         {R"({"do":"move","widget":"probePlot"})",
          R"({"angle":[120,0],"do":"wheel","mods":["alt"],"widget":"probePlot"})"},
         resetPlot,
         [&]() {
           const QPoint& at = center(p.plot);
           CShotSynth::move(p.plot, at + QPoint(-10, 0));
           CShotSynth::move(p.plot, at);
           CShotSynth::wheel(p.plot, at, QPoint(120, 0), Qt::AltModifier);
         },
         plotState});

  {
    // An empty grid place with another empty one beside it.
    QPoint empty(-1, -1);
    for (qint32 x = p.grid->width() - 3; x > 2 && empty.x() < 0; x--) {
      const QPoint at(x, CIconGrid::kTileSize / 2);
      if (p.grid->iconAt(at).isEmpty() && p.grid->iconAt(at - QPoint(2, 0)).isEmpty() &&
          CShotSynth::missed(p.grid, at).isEmpty()) {
        empty = at;
      }
    }
    if (empty.x() < 0) {
      verdict("a hover with Shift over the grid where no tile is records nothing", false,
              QString("the grid, %1 wide, has no place without a tile").arg(p.grid->width()));
    } else {
      check({"a hover with Shift over the grid where no tile is records nothing",
             {},
             []() {},
             [&p, empty]() {
               CShotSynth::move(p.grid, empty - QPoint(2, 0), Qt::ShiftModifier);
               CShotSynth::move(p.grid, empty, Qt::ShiftModifier);
             }});
    }
  }

  check({"an icon grid tile is its icon",
         {R"({"do":"move","widget":"probeIconGrid"})", R"({"button":"left","do":"click","widget":"probeIconGrid"})"},
         [&p]() { zeroCounts(p); },
         [&]() {
           const QPoint at(CIconGrid::kTileSize * 3 / 2, CIconGrid::kTileSize / 2);
           click(p.grid, at);
         },
         [&p]() {
           QStringList picked;
           for (auto it = p.counts.constBegin(); it != p.counts.constEnd(); ++it) {
             if (it.key().startsWith("icon:") && 0 != it.value()) {
               picked << QString("%1=%2").arg(it.key()).arg(it.value());
             }
           }
           return picked.join(' ');
         }});

  // ---- the painted row buttons

  // The option is built as QAbstractItemViewPrivate::sendDelegateEvent() builds it (Qt 6.10.2); an independent copy, so
  // a recording that misses the button fails.
  auto buttonPoint = [](QTreeWidget* tree, QTreeWidgetItem* item,
                        const std::function<QRect(const QStyleOptionViewItem&, const QModelIndex&)>& rectOf) {
    tree->scrollToItem(item);
    const QRect& row = tree->visualItemRect(item);
    const QModelIndex& index = tree->indexAt(row.center());
    QStyleOptionViewItem opt;
    opt.initFrom(tree);
    opt.font = tree->font();
    opt.rect = row;
    opt.state &= ~QStyle::State_HasFocus;
    if (index == tree->currentIndex()) {
      opt.state |= QStyle::State_HasFocus;
    }
    const QRect& rect = rectOf(opt, index);
    if (!rect.isValid()) {
      qWarning() << "shoot: the case finds no button on the row";
    }
    return rect.center();
  };
  // Show the tree's tab page and dock; which one is visible is the layout's business.
  auto bringUp = [](QWidget* w) {
    for (QWidget* above = w->parentWidget(); nullptr != above; above = above->parentWidget()) {
      if (QTabWidget* tabs = qobject_cast<QTabWidget*>(above); nullptr != tabs) {
        for (QWidget* page = w; nullptr != page && page != tabs; page = page->parentWidget()) {
          if (tabs->indexOf(page) >= 0) {
            tabs->setCurrentWidget(page);
          }
        }
      } else if (QDockWidget* dock = qobject_cast<QDockWidget*>(above); nullptr != dock) {
        dock->show();
        dock->raise();
      }
    }
    QTest::qWait(100);
  };

  CWksItemDelegate* wksDelegate = qobject_cast<CWksItemDelegate*>(wks->itemDelegate());
  IWksItem* wksProject = dynamic_cast<IWksItem*>(project);
  if (nullptr == wksDelegate || nullptr == wksProject) {
    verdict("the workspace paints its row buttons with CWksItemDelegate", false);
  } else {
    auto visiblePoint = [&, wksDelegate]() {
      return buttonPoint(wks, project, [wksDelegate](const QStyleOptionViewItem& opt, const QModelIndex& index) {
        return wksDelegate->buttonRect(opt, index, CWksItemDelegate::button_e::eVisible);
      });
    };
    auto resetWks = [wks, wksProject]() {
      wks->setCurrentItem(nullptr);
      wksProject->setVisibility(true);
    };

    check({"a workspace row's button is the row and the button, not a select",
           {QString(R"({"button":"visible","do":"click","row":"%1","widget":"treeWks"})").arg(projectPath)},
           resetWks,
           [&, visiblePoint]() { click(wks->viewport(), visiblePoint()); },
           [wksProject]() { return QString::number(wksProject->isVisible()); }});

    check({"a click on the row after a click on its button is a select",
           {QString(R"({"button":"visible","do":"click","row":"%1","widget":"treeWks"})").arg(projectPath),
            QString(R"({"do":"select","row":"%1","widget":"treeWks"})").arg(projectPath)},
           resetWks,
           [&, visiblePoint]() {
             click(wks->viewport(), visiblePoint());
             settle(200);
             click(wks->viewport(), wks->visualItemRect(project).center());
           },
           [wks, wksProject]() {
             const QString& current =
                 (nullptr == wks->currentItem()) ? QString() : CShotAddress::itemPathOf(wks->currentItem());
             return QString("%1 %2").arg(wksProject->isVisible()).arg(current);
           }});
    check(
        {"a double click on a row button is one step, and collapses the row as any double click does",
         {QString(R"({"button":"visible","do":"dclick","row":"%1","widget":"treeWks"})").arg(projectPath),
          QString(R"({"do":"collapse","row":"%1","widget":"treeWks"})").arg(projectPath)},
         [resetWks, project]() {
           resetWks();
           project->setExpanded(true);
         },
         [&, visiblePoint]() {
           CShotSynth::mouse(QTest::MouseDClick, wks->viewport(), Qt::LeftButton, Qt::NoModifier, visiblePoint());
           settle(300);
         },
         [wksProject, project]() { return QString("%1 %2").arg(wksProject->isVisible()).arg(project->isExpanded()); }});

    check({"a right button press on a row button is that button, and its menu no step of its own",
           {QString(R"({"button":"visible","do":"click","mouse":"right","row":"%1","widget":"treeWks"})")
                .arg(projectPath)},
           resetWks,
           [&, visiblePoint]() {
             click(wks->viewport(), visiblePoint(), Qt::RightButton);
             settle(300);
           },
           [wksProject, &menus]() { return QString("%1 %2").arg(wksProject->isVisible()).arg(menus.shown); },
           true,
           true});
    resetWks();
  }

  CGisListDB* db = main->findChild<CGisListDB*>();
  CDBItemDelegate* dbDelegate = (nullptr == db) ? nullptr : qobject_cast<CDBItemDelegate*>(db->itemDelegate());
  const QString dbPath = "Example";
  const QString groupPath = dbPath + "/Projects";
  const QString projectRowPath = groupPath + "/Einstein";
  if (nullptr == dbDelegate || nullptr == CShotAddress::resolveNamePath(*db, dbPath)) {
    verdict("the database tree lists the fixture's database Example", false);
  } else {
    names.insert("%DB%", CShotAddress::addressOf(main, db).value_or(QString()));
    auto dbRow = [db](const QString& path) { return dynamic_cast<IDBItem*>(CShotAddress::resolveNamePath(*db, path)); };
    // Unloaded as the button does it, which the database tree reacts to.
    auto uncheck = [db](IDBItem* item) {
      if (nullptr != item && Qt::Unchecked != item->getCheckState()) {
        item->setCheckState(Qt::Unchecked);
        emit db->itemChanged(item, IDBItem::eColumn);
      }
    };
    auto checkStatePoint = [&, dbDelegate](IDBItem* item) {
      return buttonPoint(db, item, [dbDelegate](const QStyleOptionViewItem& opt, const QModelIndex& index) {
        return dbDelegate->buttonRect(opt, index, CDBItemDelegate::button_e::eCheckState);
      });
    };
    auto resetDb = [&, db]() {
      bringUp(db);
      db->setCurrentItem(nullptr);
      if (IDBItem* top = dbRow(dbPath); nullptr != top) {
        top->setExpanded(true);
      }
      if (IDBItem* group = dbRow(groupPath); nullptr != group) {
        group->setExpanded(true);
        uncheck(dbRow(projectRowPath));
        group->setExpanded(false);
      }
      settle(300);
    };

    check({"a database folder expanded and a project's check state toggled",
           {QString(R"({"do":"expand","row":"%1","widget":"%DB%"})").arg(groupPath),
            QString(R"({"button":"checkState","do":"click","row":"%1","widget":"%DB%"})").arg(projectRowPath)},
           resetDb,
           [&, db]() {
             const QRect& rect = db->visualItemRect(dbRow(groupPath));
             click(db->viewport(), QPoint(rect.left() - 8, rect.center().y()));
             settle(500);
             click(db->viewport(), checkStatePoint(dbRow(projectRowPath)));
             settle(500);
           },
           [&]() {
             const IDBItem* group = dbRow(groupPath);
             const IDBItem* einstein = dbRow(projectRowPath);
             return QString("%1 %2")
                 .arg((nullptr == group) ? -1 : qint32(group->isExpanded()))
                 .arg((nullptr == einstein) ? -1 : qint32(einstein->getCheckState()));
           }});

    // The first item the database lists for the project.
    auto resetItem = [&, db]() {
      resetDb();
      if (IDBItem* group = dbRow(groupPath); nullptr != group) {
        group->setExpanded(true);
      }
      if (IDBItem* einstein = dbRow(projectRowPath); nullptr != einstein) {
        einstein->setExpanded(true);
        settle(300);
        if (einstein->childCount() > 0) {
          uncheck(dynamic_cast<IDBItem*>(einstein->child(0)));
          names.insert("%DBITEM%", CShotAddress::namePathOf(einstein->child(0)));
        }
      }
      settle(300);
    };
    auto itemRow = [&]() {
      IDBItem* einstein = dbRow(projectRowPath);
      return (nullptr == einstein || 0 == einstein->childCount()) ? nullptr
                                                                  : dynamic_cast<IDBItem*>(einstein->child(0));
    };
    check({"a double click on a database item is a double click, not its button",
           {R"({"do":"dclick","row":"%DBITEM%","widget":"%DB%"})"},
           resetItem,
           [&, db]() {
             IDBItem* item = itemRow();
             if (nullptr == item) {
               return;
             }
             db->scrollToItem(item);
             CShotSynth::mouse(QTest::MouseDClick, db->viewport(), Qt::LeftButton, Qt::NoModifier,
                               db->visualItemRect(item).center());
             settle(500);
           },
           [&]() {
             const IDBItem* item = itemRow();
             return (nullptr == item) ? QString() : QString::number(item->getCheckState());
           }});
    check({"a double click on a database item's button is one step",
           {R"({"button":"checkState","do":"dclick","row":"%DBITEM%","widget":"%DB%"})"},
           resetItem,
           [&, db]() {
             IDBItem* item = itemRow();
             if (nullptr == item) {
               return;
             }
             CShotSynth::mouse(QTest::MouseDClick, db->viewport(), Qt::LeftButton, Qt::NoModifier,
                               checkStatePoint(item));
             settle(500);
           },
           [&]() {
             const IDBItem* item = itemRow();
             return (nullptr == item) ? QString() : QString::number(item->getCheckState());
           }});
    resetItem();
    resetDb();
    if (IDBItem* einstein = dbRow(projectRowPath); nullptr != einstein) {
      einstein->setExpanded(false);
    }
  }

  CMapTreeWidget* maps = main->findChild<CMapTreeWidget*>();
  CMapItemDelegate* mapDelegate = (nullptr == maps) ? nullptr : qobject_cast<CMapItemDelegate*>(maps->itemDelegate());
  const QString mapName =
      (nullptr == maps || 0 == maps->topLevelItemCount()) ? QString() : CShotAddress::namePathOf(maps->topLevelItem(0));
  if (nullptr == mapDelegate || mapName.isEmpty()) {
    verdict("the map tree lists a map with a name of its own", false);
  } else {
    names.insert("%MAPROW%", mapName);
    auto mapRow = [maps, mapName]() { return CShotAddress::resolveNamePath(*maps, mapName); };
    auto resetMaps = [&, maps]() {
      bringUp(maps);
      // An objectName addresses only while unique, so it is read when the case runs.
      names.insert("%MAPS%", CShotAddress::addressOf(main, maps).value_or(QString()));
      maps->setCurrentItem(nullptr);
      if (IMapItem* map = dynamic_cast<IMapItem*>(mapRow());
          nullptr != map && IMapItem::eStatus::Active != map->getStatus()) {
        map->activate(true);
      }
      settle(300);
    };
    check({"a map row's activate button is the row and the button",
           {R"({"button":"activate","do":"click","row":"%MAPROW%","widget":"%MAPS%"})"},
           resetMaps,
           [&, maps, mapDelegate]() {
             const QPoint& at =
                 buttonPoint(maps, mapRow(), [mapDelegate](const QStyleOptionViewItem& opt, const QModelIndex& index) {
                   return mapDelegate->buttonRect(opt, index, CMapItemDelegate::button_e::eActivate);
                 });
             click(maps->viewport(), at);
             settle(300);
           },
           [&]() {
             const IMapItem* map = dynamic_cast<IMapItem*>(mapRow());
             return (nullptr == map) ? QString() : QString::number(qint32(map->getStatus()));
           }});
    resetMaps();
  }

  // ---- menus a row button opens

  {
    // A leftover menu shares action names with the next, which is then addressed by position.
    QMetaObject::invokeMethod(wks, "slotGeoSearch", Q_ARG(bool, true));
    CGeoSearch* search = nullptr;
    for (qint32 i = 0; i < wks->topLevelItemCount() && nullptr == search; i++) {
      search = dynamic_cast<CGeoSearch*>(wks->topLevelItem(i));
    }
    if (nullptr == search) {
      verdict("the geo search's menus are gone once they close", false, "no geo search in the workspace");
    } else {
      auto menusOfTree = [wks]() { return wks->findChildren<QMenu*>(Qt::FindDirectChildrenOnly).size(); };
      const qsizetype before = menusOfTree();
      menuCloser.start();
      for (qint32 i = 0; i < 2; i++) {
        search->selectService(QRect());
        search->changeSymbol();
      }
      menuCloser.stop();
      const qsizetype after = menusOfTree();
      verdict("the geo search's menus are gone once they close", before == after,
              QString("%1 menus below the workspace before, %2 after").arg(before).arg(after));
    }
    QMetaObject::invokeMethod(wks, "slotGeoSearch", Q_ARG(bool, false));
  }

  // ---- POIs in the map's context menu

  {
    // The fixture has several routes called Memory within metres, two at one position, in category 321.
    CPoiList* poiList = main->findChild<CPoiList*>();
    IPoiFile* poiFile = (nullptr == poiList || 0 == poiList->count() || nullptr == poiList->item(0))
                            ? nullptr
                            : poiList->item(0)->getPoifile().data();
    if (nullptr == poiFile) {
      verdict("POIs close by with one name get menu entries of their own", false, "the fixture has no POI file");
    } else {
      QTreeWidget categories;
      CPoiCategory category("climbing", 321, &categories);
      category.setCheckState(CPoiPropSetup::eTreeColumnCheckbox, Qt::Checked);
      poiFile->slotCheckedStateChanged(&category);

      QSet<IPoiItem> pois;
      QList<QPointF> highlight;
      poiFile->findPoisIn(QRectF(QPointF(10.6190, 47.5005), QPointF(10.6210, 47.4985)), pois, highlight);
      QSet<QString> actionNames;
      qint32 memories = 0;
      for (const IPoiItem& poi : std::as_const(pois)) {
        if ("Memory" == poi.name) {
          memories++;
          actionNames.insert(CMouseNormal::poiActionName(poi));
        }
      }
      category.setCheckState(CPoiPropSetup::eTreeColumnCheckbox, Qt::Unchecked);
      poiFile->slotCheckedStateChanged(&category);

      verdict("POIs close by with one name get menu entries of their own",
              memories > 1 && actionNames.size() == memories,
              QString("%1 POIs called Memory, %2 names: %3")
                  .arg(memories)
                  .arg(actionNames.size())
                  .arg(QStringList(actionNames.begin(), actionNames.end()).join(' ')));
    }
  }

  // ---- a scenario replayed twice in one process

  {
    const QString name = "a scenario that opens a details tab replays the same the second time";
    resetMap();
    project->setExpanded(true);
    wks->scrollToItem(trk);
    settle(200);

    recorder.start();
    QTimer::singleShot(300, wks, []() {
      QMenu* menu = qobject_cast<QMenu*>(QApplication::activePopupWidget());
      const QList<QAction*>& entries = (nullptr == menu) ? QList<QAction*>() : menu->actions();
      for (QAction* entry : entries) {
        if ("actionEditDetails" == entry->objectName()) {
          const QPoint& at = menu->actionGeometry(entry).center();
          CShotSynth::arrive(menu, at);
          CShotSynth::mouse(QTest::MouseClick, menu, Qt::LeftButton, Qt::NoModifier, at);
          return;
        }
      }
      if (nullptr != menu) {
        menu->close();
      }
    });
    click(wks->viewport(), wks->visualItemRect(trk).center(), Qt::RightButton);
    settle(500);
    const QJsonArray& details = recorder.stop();
    qDebug().noquote() << "shoot: case" << name << "recorded" << QJsonDocument(details).toJson(QJsonDocument::Compact);

    QTabWidget* tabs = nullptr;
    for (QWidget* w = canvas->parentWidget(); nullptr != w && nullptr == tabs; w = w->parentWidget()) {
      tabs = qobject_cast<QTabWidget*>(w);
    }
    // The recording opened the tab; the replays must find the window without it.
    const auto closeDetails = [tabs]() {
      while (nullptr != tabs && tabs->count() > 1) {
        emit tabs->tabCloseRequested(tabs->count() - 1);
        settle(100);
      }
    };
    closeDetails();
    const qint32 before = (nullptr == tabs) ? -1 : tabs->count();
    const auto pictureInto = [main, tabs](QImage& image, qint32& tabCount) {
      return [main, tabs, &image, &tabCount]() -> qint32 {
        tabCount = (nullptr == tabs) ? -1 : tabs->count();
        image = CShotWriter::render(main, main->size());
        return image.isNull() ? 1 : 0;
      };
    };
    QImage first;
    QImage second;
    qint32 firstTabs = 0;
    qint32 secondTabs = 0;
    qint32 replayFailures = CShotReplay::replay(details, ctx, pictureInto(first, firstTabs));
    CShotReplay::clear(details, ctx);
    replayFailures += CShotReplay::replay(details, ctx, pictureInto(second, secondTabs));
    CShotReplay::clear(details, ctx);
    closeDetails();

    verdict(
        name,
        0 == replayFailures && !first.isNull() && first == second && before + 1 == firstTabs && firstTabs == secondTabs,
        QString("%1 replay failure(s), %2 tabs before, %3 then %4, pictures %5")
            .arg(replayFailures)
            .arg(before)
            .arg(firstTabs)
            .arg(secondTabs)
            .arg(first == second ? "equal" : "differ"));
  }

  {
    const QString name = "the parts around the map render as render() does them, waiting for the map once";
    resetMap();
    QList<QWidget*> parts;
    for (QWidget* w = canvas; nullptr != w && w != main; w = w->parentWidget()) {
      parts << w;
    }
    parts << main;
    QElapsedTimer timer;
    timer.start();
    QList<QImage> each;
    for (QWidget* part : std::as_const(parts)) {
      each << CShotWriter::render(part, part->isWindow() ? part->size() : QSize());
    }
    const qint64 eachMs = timer.restart();
    const QHash<const QWidget*, QImage>& all = CShotWriter::renderAll(parts);
    const qint64 allMs = timer.elapsed();
    qint32 equal = 0;
    for (qsizetype i = 0; i < parts.size(); i++) {
      equal += (!each.at(i).isNull() && each.at(i) == all.value(parts.at(i))) ? 1 : 0;
    }
    verdict(name, equal == parts.size() && allMs < eachMs,
            QString("%1 of %2 parts equal, %3 ms one by one, %4 ms together")
                .arg(equal)
                .arg(parts.size())
                .arg(eachMs)
                .arg(allMs));
  }

  {
    const QString name = "a range started in a plot is undone before the scenario replays again";
    resetMap();
    trk->edit();
    settle(500);
    CPlotProfile* plot = nullptr;
    const QList<CPlotProfile*>& plots = main->findChildren<CPlotProfile*>();
    for (CPlotProfile* candidate : plots) {
      if (candidate->isVisible() && NOFLOAT != candidate->xValueAt(center(candidate))) {
        plot = candidate;
        break;
      }
    }
    if (nullptr == plot) {
      verdict(name, false, "the details tab shows no profile");
    } else {
      recorder.start();
      CShotSynth::move(plot, center(plot));
      click(plot, center(plot));
      settle(200);
      const QJsonArray& steps = recorder.stop();
      qDebug().noquote() << "shoot: case" << name << "recorded" << QJsonDocument(steps).toJson(QJsonDocument::Compact);
      const auto rangeState = [plot, trk]() {
        return QString("%1/%2").arg(plot->isSelectingRange()).arg(int(trk->getRangState()));
      };
      const QString recorded = rangeState();
      qint32 replayFailures = CShotReplay::replay(steps, ctx, {});
      const QString firstState = rangeState();
      replayFailures += CShotReplay::replay(steps, ctx, {});
      const QString secondState = rangeState();
      CShotReplay::clear(steps, ctx);
      const QString cleared = rangeState();
      verdict(name,
              0 == replayFailures && recorded.startsWith("1/") && recorded == firstState && firstState == secondState &&
                  !plot->isSelectingRange(),
              QString("%1 replay failure(s), recorded %2, replays %3 then %4, cleared %5")
                  .arg(replayFailures)
                  .arg(recorded, firstState, secondState, cleared));
    }
    QTabWidget* tabs = nullptr;
    for (QWidget* w = canvas->parentWidget(); nullptr != w && nullptr == tabs; w = w->parentWidget()) {
      tabs = qobject_cast<QTabWidget*>(w);
    }
    while (nullptr != tabs && tabs->count() > 1) {
      emit tabs->tabCloseRequested(tabs->count() - 1);
      settle(100);
    }
  }

  qApp->removeEventFilter(&menus);
  resetPlot();
  canvas->resetMouse();
  p.dialog->hide();
  p.surfaces->close();
  p.window->close();
  delete p.surfaces;
  delete p.dialog;
  delete p.window;
  qWarning().noquote() << "shoot: self test" << (cases - failures) << "of" << cases << "cases pass";
  return failures;
}
