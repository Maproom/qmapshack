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

#include "shoot/CShotHandlers.h"

#include <QAbstractButton>
#include <QAbstractItemView>
#include <QAbstractScrollArea>
#include <QAbstractSlider>
#include <QAbstractSpinBox>
#include <QAction>
#include <QApplication>
#include <QComboBox>
#include <QCompleter>
#include <QDateTimeEdit>
#include <QDebug>
#include <QDockWidget>
#include <QDoubleSpinBox>
#include <QElapsedTimer>
#include <QEventLoop>
#include <QGroupBox>
#include <QHash>
#include <QHeaderView>
#include <QJsonArray>
#include <QJsonDocument>
#include <QKeySequence>
#include <QLineEdit>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QMouseEvent>
#include <QPersistentModelIndex>
#include <QScrollBar>
#include <QSpinBox>
#include <QSplitter>
#include <QStyleOptionViewItem>
#include <QTabBar>
#include <QTabWidget>
#include <QTimer>
#include <QToolButton>
#include <QTreeView>
#include <QTreeWidget>
#include <QWheelEvent>
#include <functional>
#include <memory>
#include <numeric>
#include <optional>

#include "CMainWindow.h"
#include "canvas/CCanvas.h"
#include "gis/CDBItemDelegate.h"
#include "gis/CGisListDB.h"
#include "gis/CGisListWks.h"
#include "gis/CGisWorkspace.h"
#include "gis/CWksItemDelegate.h"
#include "gis/IGisItem.h"
#include "gis/proj_x.h"
#include "map/CMapItemDelegate.h"
#include "map/CMapList.h"
#include "mouse/CMouseAdapter.h"
#include "plot/IPlot.h"
#include "shoot/CShotAddress.h"
#include "shoot/CShotApplication.h"
#include "shoot/CShotContext.h"
#include "shoot/CShotInput.h"
#include "shoot/CShotRecorder.h"
#include "shoot/CShotStep.h"
#include "shoot/CShotSynth.h"
#include "shoot/IShotHandler.h"
#include "units/IUnit.h"
#include "widgets/CIconGrid.h"

namespace {
/** How long a canvas step waits for the map to finish drawing, and how often it looks [ms]. */
constexpr qint32 kCanvasSettleMs = 20000;
constexpr qint32 kCanvasPollMs = 20;

qint32 fail(const QString& what) {
  qWarning().noquote() << "shoot:" << what;
  return 1;
}

QString compact(const QJsonObject& step) {
  return QString::fromUtf8(QJsonDocument(step).toJson(QJsonDocument::Compact));
}

QString classOf(const QObject* object) { return QString::fromLatin1(object->metaObject()->className()); }

/** @return a failure unless a user could act on @p widget now */
qint32 failUnlessUsable(const QWidget* widget, const QJsonObject& step) {
  if (!widget->isVisible()) {
    return fail("the step cannot be performed, its widget is not shown: " + compact(step));
  }
  if (!widget->isEnabled()) {
    return fail("the step cannot be performed, its widget is disabled: " + compact(step));
  }
  return 0;
}

/** @return a failure when input at @p pos of @p widget would reach another widget than @p widget */
qint32 failUnlessReached(QWidget* widget, const QPoint& pos, const QJsonObject& step) {
  const QString& other = CShotSynth::missed(widget, pos);
  return other.isEmpty() ? 0 : fail(QString("the step would reach %1 instead: %2").arg(other, compact(step)));
}

QPoint pointAt(const QRect& rect, const QJsonArray& at) {
  return rect.topLeft() +
         QPoint(qRound(at.at(0).toDouble() * rect.width()), qRound(at.at(1).toDouble() * rect.height()));
}

QJsonArray fractionIn(const QRect& rect, const QPoint& pos) {
  const QPoint& inside = pos - rect.topLeft();
  return QJsonArray({double(inside.x()) / qMax(1, rect.width()), double(inside.y()) / qMax(1, rect.height())});
}

// ---- how a step spells buttons and modifiers

QString buttonName(Qt::MouseButton button) {
  switch (button) {
    case Qt::LeftButton:
      return "left";
    case Qt::RightButton:
      return "right";
    case Qt::MiddleButton:
      return "middle";
    default:
      return QString();
  }
}

Qt::MouseButton buttonOf(const QString& name) {
  if ("left" == name) {
    return Qt::LeftButton;
  }
  if ("right" == name) {
    return Qt::RightButton;
  }
  if ("middle" == name) {
    return Qt::MiddleButton;
  }
  return Qt::NoButton;
}

void putModNames(QJsonObject& step, Qt::KeyboardModifiers mods) {
  QJsonArray names;
  if (mods & Qt::ControlModifier) {
    names << "ctrl";
  }
  if (mods & Qt::ShiftModifier) {
    names << "shift";
  }
  if (mods & Qt::AltModifier) {
    names << "alt";
  }
  if (mods & Qt::MetaModifier) {
    names << "meta";
  }
  if (!names.isEmpty()) {
    step["mods"] = names;
  }
}

Qt::KeyboardModifiers modsOf(const QJsonObject& step) {
  Qt::KeyboardModifiers mods = Qt::NoModifier;
  const QJsonArray& names = step["mods"].toArray();
  for (const QJsonValue& name : names) {
    if ("ctrl" == name.toString()) {
      mods |= Qt::ControlModifier;
    } else if ("shift" == name.toString()) {
      mods |= Qt::ShiftModifier;
    } else if ("alt" == name.toString()) {
      mods |= Qt::AltModifier;
    } else if ("meta" == name.toString()) {
      mods |= Qt::MetaModifier;
    }
  }
  return mods;
}

// ---- typing

/** @return the list @p control's completer shows, nullptr while it has shown none */
QAbstractItemView* completerPopupOf(const QWidget* control) {
  QCompleter* completer = nullptr;
  if (const QLineEdit* edit = qobject_cast<const QLineEdit*>(control); nullptr != edit) {
    completer = edit->completer();
  } else if (const QComboBox* combo = qobject_cast<const QComboBox*>(control); nullptr != combo) {
    completer = combo->completer();
  }
  return (nullptr == completer) ? nullptr : completer->popup();
}

/** @brief Record typing into @p control as one `key` step holding the final value */
void watchTyping(QWidget* control, QLineEdit* edit, CShotRecorder& recorder, const std::function<QString()>& valueOf) {
  QObject::connect(edit, &QLineEdit::textChanged, &recorder, [control, &recorder, valueOf](const QString&) {
    if (const std::optional<QString>& address = recorder.address(control); address.has_value()) {
      recorder.amend(control, CShotStep::key(*address, valueOf(), false));
    }
  });
}

/**
   @brief Editing of @p control ended: by Enter, part of the `key` step, or by the focus leaving, an `endedit` step.

   A replayed click or trigger moves no focus, so a focus loss needs a step of its own.
 */
void endTyping(QWidget* control, CShotRecorder& recorder, const std::function<QString()>& valueOf) {
  const std::optional<QString>& address = recorder.address(control);
  if (!address.has_value()) {
    return;
  }
  const SShotFrame& frame = CShotApplication::frame();
  // Enter picking a completion goes to the completer's list.
  const QAbstractItemView* popup = completerPopupOf(control);
  const bool enter =
      QEvent::KeyPress == frame.type && (Qt::Key_Return == frame.key || Qt::Key_Enter == frame.key) &&
      (CShotApplication::frameWithin(control) || (nullptr != popup && CShotApplication::frameWithin(popup)));
  if (enter) {
    if (QJsonObject* open = recorder.openStep(control, "key"); nullptr != open) {
      (*open)["text"] = valueOf();
      (*open)["enter"] = true;
    } else {
      recorder.amend(control, CShotStep::key(*address, valueOf(), true));
    }
    recorder.close(control);
    return;
  }
  recorder.close(control);
  recorder.recordOutcome(control, QJsonObject{{"do", "endedit"}, {"widget", *address}});
}

/** @brief Replace @p control's text with @p step's by typing */
qint32 replayTyping(QWidget* control, const QJsonObject& step, const std::function<QString()>& valueOf) {
  if (const qint32 failed = failUnlessUsable(control, step); 0 != failed) {
    return failed;
  }
  const QString& text = step["text"].toString();
  CShotSynth::key(control, Qt::Key_A, Qt::ControlModifier);
  CShotSynth::key(control, Qt::Key_Backspace);
  CShotSynth::type(control, text);
  // An open completer list would take the next click.
  if (QAbstractItemView* popup = completerPopupOf(control); nullptr != popup && popup->isVisible()) {
    popup->hide();
  }
  // Read before Enter, which can accept a dialog and take the input with it.
  if (valueOf() != text) {
    return fail(QString("%1 holds '%2' after '%3' was typed").arg(step["widget"].toString(), valueOf(), text));
  }
  if (step["enter"].toBool()) {
    CShotSynth::key(control, Qt::Key_Return);
  }
  return 0;
}

qint32 replayEndEdit(QWidget* control) {
  // No focus: a replayed input moved it and the signal was emitted.
  if (control->hasFocus()) {
    control->clearFocus();
  }
  return 0;
}

// ---- Qt's controls

/** @return the action named @p name in @p owner, as a child or as one added to it */
QAction* actionIn(const QWidget* owner, const QString& name) {
  if (nullptr == owner) {
    return nullptr;
  }
  if (QAction* child = owner->findChild<QAction*>(name); nullptr != child) {
    return child;
  }
  const QList<QAction*>& added = owner->actions();
  for (QAction* action : added) {
    if (action->objectName() == name) {
      return action;
    }
  }
  return nullptr;
}

/** An action, from a menu entry, a tool button or a shortcut. */
class CActionHandler : public IShotHandler {
 public:
  const QMetaObject* handles() const override { return &QAction::staticMetaObject; }

  void watch(QObject* object, CShotRecorder& recorder) const override {
    QAction* action = static_cast<QAction*>(object);
    QObject::connect(action, &QAction::triggered, &recorder, [action, &recorder](bool checked) {
      // The text is translated and no address.
      const QString& name = action->objectName();
      if (name.isEmpty()) {
        qWarning() << "shoot: the entry" << action->text() << "has no objectName and is not recorded";
        return;
      }
      QJsonObject step = CShotStep::trigger(name);
      if (action->isCheckable()) {
        step["checked"] = checked;
      }
      // Plots share action names, so a name alone addresses only the first one found.
      const QWidget* main = recorder.context().mainWindow();
      if (nullptr == main || main->findChild<QAction*>(name) != action) {
        const std::optional<QString>& owner = ownerOf(action, main, recorder);
        if (!owner.has_value()) {
          qWarning() << "shoot: the action" << name << "is not recorded: no widget it sits in names it alone";
          return;
        }
        step["widget"] = *owner;
      }
      recorder.record(action, step);
    });
  }

  qint32 replay(const QJsonObject& step, QObject* target) const override {
    QAction* action = static_cast<QAction*>(target);
    if (!action->isEnabled()) {
      return fail("the action is disabled: " + compact(step));
    }
    if (QMenu* menu = qobject_cast<QMenu*>(QApplication::activePopupWidget());
        nullptr != menu && menu->actions().contains(action)) {
      // A click, because only that makes exec() return the action; some menus read nothing else.
      const QPoint& entry = menu->actionGeometry(action).center();
      if (const qint32 failed = failUnlessReached(menu, entry, step); 0 != failed) {
        return failed;
      }
      qDebug().noquote() << "shoot: replay picks" << action->objectName() << "in the open menu at" << entry;
      CShotSynth::arrive(menu, entry);
      CShotSynth::mouse(QTest::MouseClick, menu, Qt::LeftButton, Qt::NoModifier, entry);
    } else {
      qDebug().noquote() << "shoot: replay triggers" << action->objectName() << "- the open popup is"
                         << (nullptr == QApplication::activePopupWidget()
                                 ? QString("none")
                                 : QString(QApplication::activePopupWidget()->metaObject()->className()));
      // As a shortcut; a menu left open would block everything after it.
      while (QMenu* open = qobject_cast<QMenu*>(QApplication::activePopupWidget())) {
        open->close();
      }
      action->trigger();
    }
    if (step.contains("checked") && action->isChecked() != step["checked"].toBool()) {
      return fail("the action did not end up as recorded: " + compact(step));
    }
    return 0;
  }

 private:
  /** @return the address of a widget @p action sits in that finds it by name, nothing when none does */
  static std::optional<QString> ownerOf(const QAction* action, const QWidget* main, const CShotRecorder& recorder) {
    QList<const QWidget*> candidates;
    for (const QObject* o = action->parent(); nullptr != o; o = o->parent()) {
      if (const QWidget* widget = qobject_cast<const QWidget*>(o); nullptr != widget) {
        candidates << widget;
        break;
      }
    }
    const QList<QObject*>& associated = action->associatedObjects();
    for (const QObject* o : associated) {
      if (const QWidget* widget = qobject_cast<const QWidget*>(o); nullptr != widget) {
        candidates << widget;
      }
    }
    for (const QWidget* candidate : std::as_const(candidates)) {
      const std::optional<QString>& address = CShotAddress::addressOf(main, candidate);
      if (address.has_value() && !address->isEmpty() && actionIn(candidate, action->objectName()) == action) {
        return address;
      }
    }
    Q_UNUSED(recorder)
    return std::nullopt;
  }
};

/** A button of any kind; a checkable one says the state the click left it in. */
class CButtonHandler : public IShotHandler {
 public:
  const QMetaObject* handles() const override { return &QAbstractButton::staticMetaObject; }

  void watch(QObject* object, CShotRecorder& recorder) const override {
    QAbstractButton* button = static_cast<QAbstractButton*>(object);
    if (isPartOfAnother(button)) {
      return;
    }
    QObject::connect(button, &QAbstractButton::clicked, &recorder, [button, &recorder](bool checked) {
      const std::optional<QString>& address = recorder.address(button);
      if (!address.has_value()) {
        return;
      }
      QJsonObject step = CShotStep::click(*address);
      if (button->isCheckable()) {
        step["checked"] = checked;
      }
      recorder.record(button, step);
    });
  }

  qint32 replay(const QJsonObject& step, QObject* target) const override {
    QAbstractButton* button = static_cast<QAbstractButton*>(target);
    if (const qint32 failed = failUnlessUsable(button, step); 0 != failed) {
      return failed;
    }
    // Emits clicked(), as the keyboard does.
    button->click();
    if (step.contains("checked") && button->isChecked() != step["checked"].toBool()) {
      return fail("the button did not end up as recorded: " + compact(step));
    }
    return 0;
  }

 private:
  /** @return true for a tool button with a default action or a tab bar's close button, recorded by their owner */
  static bool isPartOfAnother(const QAbstractButton* button) {
    const QToolButton* tool = qobject_cast<const QToolButton*>(button);
    if (nullptr != tool && nullptr != tool->defaultAction()) {
      return true;
    }
    return nullptr != qobject_cast<const QTabBar*>(button->parentWidget());
  }
};

/** A checkable group box. */
class CGroupBoxHandler : public IShotHandler {
 public:
  const QMetaObject* handles() const override { return &QGroupBox::staticMetaObject; }

  void watch(QObject* object, CShotRecorder& recorder) const override {
    QGroupBox* group = static_cast<QGroupBox*>(object);
    QObject::connect(group, &QGroupBox::clicked, &recorder, [group, &recorder](bool checked) {
      if (const std::optional<QString>& address = recorder.address(group); address.has_value()) {
        QJsonObject step = CShotStep::click(*address);
        step["checked"] = checked;
        recorder.record(group, step);
      }
    });
  }

  qint32 replay(const QJsonObject& step, QObject* target) const override {
    QGroupBox* group = static_cast<QGroupBox*>(target);
    if (const qint32 failed = failUnlessUsable(group, step); 0 != failed) {
      return failed;
    }
    // Space clicks it without knowing where the check box is drawn.
    CShotSynth::key(group, Qt::Key_Space);
    if (group->isChecked() != step["checked"].toBool()) {
      return fail("the group box did not end up as recorded: " + compact(step));
    }
    return 0;
  }
};

/** A combo box: the entry picked, and what was typed into an editable one. */
class CComboBoxHandler : public IShotHandler {
 public:
  const QMetaObject* handles() const override { return &QComboBox::staticMetaObject; }

  void watch(QObject* object, CShotRecorder& recorder) const override {
    QComboBox* combo = static_cast<QComboBox*>(object);
    QObject::connect(combo, &QComboBox::activated, &recorder, [combo, &recorder](int index) {
      if (const std::optional<QString>& address = recorder.address(combo); address.has_value()) {
        recorder.record(combo, CShotStep::set(*address, "currentIndex", index));
      }
    });
    if (nullptr != combo->lineEdit()) {
      const std::function<QString()> valueOf = [combo]() { return combo->currentText(); };
      watchTyping(combo, combo->lineEdit(), recorder, valueOf);
      QObject::connect(combo->lineEdit(), &QLineEdit::editingFinished, &recorder,
                       [combo, &recorder, valueOf]() { endTyping(combo, recorder, valueOf); });
    }
  }

  qint32 replay(const QJsonObject& step, QObject* target) const override {
    QComboBox* combo = static_cast<QComboBox*>(target);
    const QString& verb = step["do"].toString();
    if ("key" == verb) {
      return replayTyping(combo, step, [combo]() { return combo->currentText(); });
    }
    if ("endedit" == verb) {
      return replayEndEdit(combo);
    }
    if (const qint32 failed = failUnlessUsable(combo, step); 0 != failed) {
      return failed;
    }
    const int index = step["value"].toInt();
    if (index < 0 || index >= combo->count()) {
      return fail(QString("the combo box has no entry %1: %2").arg(index).arg(compact(step)));
    }
    combo->setCurrentIndex(index);
    // What a pick in the popup emits.
    emit combo->activated(index);
    emit combo->textActivated(combo->itemText(index));
    return 0;
  }
};

/** A line edit of its own; one inside a spin box or a combo box belongs to that control. */
class CLineEditHandler : public IShotHandler {
 public:
  const QMetaObject* handles() const override { return &QLineEdit::staticMetaObject; }

  void watch(QObject* object, CShotRecorder& recorder) const override {
    QLineEdit* edit = static_cast<QLineEdit*>(object);
    const QWidget* parent = edit->parentWidget();
    if (nullptr != qobject_cast<const QAbstractSpinBox*>(parent) || nullptr != qobject_cast<const QComboBox*>(parent)) {
      return;
    }
    // Asked at typing time: a view places a widget into its row only after it is shown. A cell editor is gone on
    // replay.
    const std::function<QString()> valueOf = [edit]() { return edit->text(); };
    QObject::connect(edit, &QLineEdit::textChanged, &recorder, [edit, &recorder, valueOf](const QString&) {
      if (CShotInput::isCellEditor(edit)) {
        qWarning() << "shoot: typing into a cell editor is not recorded: it exists only while the edit is open";
        return;
      }
      if (const std::optional<QString>& address = recorder.address(edit); address.has_value()) {
        // The completer's parentless popup counts as the line edit.
        recorder.amend(edit, CShotStep::key(*address, valueOf(), false), completerPopupOf(edit));
      }
    });
    QObject::connect(edit, &QLineEdit::editingFinished, &recorder, [edit, &recorder, valueOf]() {
      if (!CShotInput::isCellEditor(edit)) {
        endTyping(edit, recorder, valueOf);
      }
    });
  }

  qint32 replay(const QJsonObject& step, QObject* target) const override {
    QLineEdit* edit = static_cast<QLineEdit*>(target);
    if ("endedit" == step["do"].toString()) {
      return replayEndEdit(edit);
    }
    return replayTyping(edit, step, [edit]() { return edit->text(); });
  }
};

/**
   A spin box, as the text it ends up showing.

   Typing via textEdited() (a key can change the text but not the value); stepping via the value signal, because
   updateEdit() blocks the line edit's signals.
 */
class CSpinBoxHandler : public IShotHandler {
 public:
  void watch(QObject* object, CShotRecorder& recorder) const override {
    QAbstractSpinBox* spin = static_cast<QAbstractSpinBox*>(object);
    if (QLineEdit* edit = spin->findChild<QLineEdit*>(QString(), Qt::FindDirectChildrenOnly); nullptr != edit) {
      QObject::connect(edit, &QLineEdit::textEdited, &recorder,
                       [spin, &recorder](const QString&) { recordValue(spin, recorder); });
    }
    QObject::connect(spin, &QAbstractSpinBox::editingFinished, &recorder, [spin, &recorder]() {
      endTyping(spin, recorder, [spin]() { return CShotInput::keyValueOf(spin); });
    });
  }

  qint32 replay(const QJsonObject& step, QObject* target) const override {
    QAbstractSpinBox* spin = static_cast<QAbstractSpinBox*>(target);
    if ("endedit" == step["do"].toString()) {
      return replayEndEdit(spin);
    }
    return replayTyping(spin, step, [spin]() { return CShotInput::keyValueOf(spin); });
  }

 protected:
  static void recordValue(QAbstractSpinBox* spin, CShotRecorder& recorder) {
    if (const std::optional<QString>& address = recorder.address(spin); address.has_value()) {
      recorder.amend(spin, CShotStep::key(*address, CShotInput::keyValueOf(spin), false));
    }
  }
};

class CIntSpinBoxHandler : public CSpinBoxHandler {
 public:
  const QMetaObject* handles() const override { return &QSpinBox::staticMetaObject; }

  void watch(QObject* object, CShotRecorder& recorder) const override {
    CSpinBoxHandler::watch(object, recorder);
    QSpinBox* spin = static_cast<QSpinBox*>(object);
    QObject::connect(spin, &QSpinBox::valueChanged, &recorder, [spin, &recorder](int) { recordValue(spin, recorder); });
  }
};

class CDoubleSpinBoxHandler : public CSpinBoxHandler {
 public:
  const QMetaObject* handles() const override { return &QDoubleSpinBox::staticMetaObject; }

  void watch(QObject* object, CShotRecorder& recorder) const override {
    CSpinBoxHandler::watch(object, recorder);
    QDoubleSpinBox* spin = static_cast<QDoubleSpinBox*>(object);
    QObject::connect(spin, &QDoubleSpinBox::valueChanged, &recorder,
                     [spin, &recorder](double) { recordValue(spin, recorder); });
  }
};

/** A date and time edit, as its value: it takes keys per section, so typing the text back does not reproduce it. */
class CDateTimeEditHandler : public IShotHandler {
 public:
  const QMetaObject* handles() const override { return &QDateTimeEdit::staticMetaObject; }

  void watch(QObject* object, CShotRecorder& recorder) const override {
    QDateTimeEdit* edit = static_cast<QDateTimeEdit*>(object);
    QObject::connect(edit, &QDateTimeEdit::dateTimeChanged, &recorder, [edit, &recorder](const QDateTime& value) {
      if (const std::optional<QString>& address = recorder.address(edit); address.has_value()) {
        recorder.amend(edit, CShotStep::set(*address, "dateTime", value.toString(Qt::ISODateWithMs)));
      }
    });
    QObject::connect(edit, &QAbstractSpinBox::editingFinished, &recorder, [edit, &recorder]() {
      recorder.close(edit);
      if (const std::optional<QString>& address = recorder.address(edit); address.has_value()) {
        recorder.recordOutcome(edit, QJsonObject{{"do", "endedit"}, {"widget", *address}});
      }
    });
  }

  qint32 replay(const QJsonObject& step, QObject* target) const override {
    QDateTimeEdit* edit = static_cast<QDateTimeEdit*>(target);
    if ("endedit" == step["do"].toString()) {
      return replayEndEdit(edit);
    }
    if (const qint32 failed = failUnlessUsable(edit, step); 0 != failed) {
      return failed;
    }
    const QDateTime& value = QDateTime::fromString(step["value"].toString(), Qt::ISODateWithMs);
    if (!value.isValid()) {
      return fail("the recorded date is no date: " + compact(step));
    }
    CShotSynth::focus(edit);
    edit->setDateTime(value);
    return (edit->dateTime() == value) ? 0 : fail("the date edit did not take the value: " + compact(step));
  }
};

/** @return the scroll area @p bar scrolls, nullptr for a slider that is no scroll area's */
QAbstractScrollArea* scrollAreaOf(const QAbstractSlider* bar) {
  for (QWidget* w = bar->parentWidget(); nullptr != w; w = w->parentWidget()) {
    if (QAbstractScrollArea* area = qobject_cast<QAbstractScrollArea*>(w); nullptr != area) {
      return (area->verticalScrollBar() == bar || area->horizontalScrollBar() == bar) ? area : nullptr;
    }
  }
  return nullptr;
}

/**
   A slider, as its value; a scroll bar, as the share of its range, which is machine-dependent pixels.

   Input to the scroll area counts. An item view records its own scrolling; a key's scroll is replayed by the key.
 */
class CSliderHandler : public IShotHandler {
 public:
  const QMetaObject* handles() const override { return &QAbstractSlider::staticMetaObject; }

  void watch(QObject* object, CShotRecorder& recorder) const override {
    QAbstractSlider* slider = static_cast<QAbstractSlider*>(object);
    QAbstractScrollArea* area = scrollAreaOf(slider);
    if (nullptr != qobject_cast<QAbstractItemView*>(area)) {
      return;
    }
    QObject::connect(slider, &QAbstractSlider::valueChanged, &recorder, [slider, area, &recorder](int value) {
      const std::optional<QString>& address = recorder.address(slider);
      if (!address.has_value()) {
        return;
      }
      if (nullptr == area) {
        recorder.amend(slider, CShotStep::set(*address, "value", value));
      } else if (QEvent::KeyPress != CShotApplication::inputFrame().type && slider->maximum() > slider->minimum()) {
        const double share = double(value - slider->minimum()) / (slider->maximum() - slider->minimum());
        recorder.amend(slider, CShotStep::set(*address, "share", share), area);
      }
    });
  }

  qint32 replay(const QJsonObject& step, QObject* target) const override {
    QAbstractSlider* slider = static_cast<QAbstractSlider*>(target);
    if ("share" == step["property"].toString()) {
      const int value = slider->minimum() + qRound(step["value"].toDouble() * (slider->maximum() - slider->minimum()));
      slider->setValue(value);
      return (qAbs(slider->value() - value) <= 1) ? 0 : fail("the scroll bar did not take the share: " + compact(step));
    }
    if (const qint32 failed = failUnlessUsable(slider, step); 0 != failed) {
      return failed;
    }
    slider->setValue(step["value"].toInt());
    return (slider->value() == step["value"].toInt()) ? 0 : fail("the slider did not take the value: " + compact(step));
  }
};

/** A splitter, as each part's share of its length. */
class CSplitterHandler : public IShotHandler {
 public:
  const QMetaObject* handles() const override { return &QSplitter::staticMetaObject; }

  void watch(QObject* object, CShotRecorder& recorder) const override {
    QSplitter* splitter = static_cast<QSplitter*>(object);
    QObject::connect(splitter, &QSplitter::splitterMoved, &recorder, [splitter, &recorder](int, int) {
      const std::optional<QString>& address = recorder.address(splitter);
      const QList<int>& sizes = splitter->sizes();
      const qint32 total = std::accumulate(sizes.begin(), sizes.end(), 0);
      if (!address.has_value() || total <= 0) {
        return;
      }
      QJsonArray shares;
      for (const int size : sizes) {
        shares << double(size) / total;
      }
      recorder.amend(splitter, CShotStep::set(*address, "sizes", shares.toVariantList()));
    });
  }

  qint32 replay(const QJsonObject& step, QObject* target) const override {
    QSplitter* splitter = static_cast<QSplitter*>(target);
    if (const qint32 failed = failUnlessUsable(splitter, step); 0 != failed) {
      return failed;
    }
    const QJsonArray& shares = step["value"].toArray();
    QList<int> sizes = splitter->sizes();
    const qint32 total = std::accumulate(sizes.begin(), sizes.end(), 0);
    if (shares.size() != sizes.size() || total <= 0) {
      return fail("the splitter has other parts than when it was recorded: " + compact(step));
    }
    for (qsizetype i = 0; i < sizes.size(); i++) {
      sizes[i] = qRound(shares.at(i).toDouble() * total);
    }
    splitter->setSizes(sizes);
    // A minimum size wins over the share; one pixel is rounding.
    const QList<int>& taken = splitter->sizes();
    for (qsizetype i = 0; i < sizes.size(); i++) {
      if (qAbs(taken[i] - sizes[i]) > 1) {
        return fail("the splitter could not be put where it was recorded: " + compact(step));
      }
    }
    return 0;
  }
};

/**
   A dock floated, docked, moved or tabbed by its title bar, recorded as the main window's saveState(): nothing else
   says where in an area it went. Floating and area are kept for the replay to check.
 */
class CDockWidgetHandler : public IShotHandler {
 public:
  const QMetaObject* handles() const override { return &QDockWidget::staticMetaObject; }

  void watch(QObject* object, CShotRecorder& recorder) const override {
    QDockWidget* dock = static_cast<QDockWidget*>(object);
    QObject::connect(dock, &QDockWidget::topLevelChanged, &recorder,
                     [dock, &recorder](bool) { arrange(dock, recorder); });
    QObject::connect(dock, &QDockWidget::dockLocationChanged, &recorder,
                     [dock, &recorder](Qt::DockWidgetArea) { arrange(dock, recorder); });
  }

  qint32 replay(const QJsonObject& step, QObject* target) const override {
    QDockWidget* dock = static_cast<QDockWidget*>(target);
    QMainWindow* main = qobject_cast<QMainWindow*>(dock->parentWidget());
    if (nullptr == main) {
      return fail("the dock is in no main window: " + compact(step));
    }
    if (!main->restoreState(QByteArray::fromBase64(step["state"].toString().toLatin1()))) {
      return fail("the main window did not take the arrangement: " + compact(step));
    }
    const bool floating = step["floating"].toBool();
    if (dock->isFloating() != floating || (!floating && int(main->dockWidgetArea(dock)) != step["area"].toInt())) {
      return fail("the dock did not end up where it was recorded: " + compact(step));
    }
    return 0;
  }

 private:
  static void arrange(QDockWidget* dock, CShotRecorder& recorder) {
    const QMainWindow* main = qobject_cast<const QMainWindow*>(dock->parentWidget());
    const std::optional<QString>& address = recorder.address(dock);
    if (nullptr == main || !address.has_value()) {
      return;
    }
    // Amended: a drag reports floating first, then the area.
    recorder.amend(dock, QJsonObject{{"do", "arrange"},
                                     {"widget", *address},
                                     {"state", QString::fromLatin1(main->saveState().toBase64())},
                                     {"floating", dock->isFloating()},
                                     {"area", int(main->dockWidgetArea(dock))}});
  }
};

/** A tab bar: which tab is current, and a tab asked to close. */
class CTabBarHandler : public IShotHandler {
 public:
  const QMetaObject* handles() const override { return &QTabBar::staticMetaObject; }

  void watch(QObject* object, CShotRecorder& recorder) const override {
    QTabBar* bar = static_cast<QTabBar*>(object);
    // The tab widget, if any: Ctrl+Tab goes to the page.
    QObject* scope = (nullptr != qobject_cast<QTabWidget*>(bar->parentWidget())) ? bar->parentWidget() : bar;
    // Not tabBarClicked(): it fires for any button and on the current tab.
    QObject::connect(bar, &QTabBar::currentChanged, &recorder, [bar, scope, &recorder](int index) {
      const std::optional<QString>& address = recorder.address(bar);
      if (index >= 0 && address.has_value()) {
        // Only Ctrl+Tab reaches the page: a switch after a click in it is the application's answer.
        const SShotFrame& frame = CShotApplication::frame();
        const bool ctrlTab = QEvent::KeyPress == frame.type && frame.mods.testFlag(Qt::ControlModifier) &&
                             (Qt::Key_Tab == frame.key || Qt::Key_Backtab == frame.key);
        recorder.record(ctrlTab ? scope : bar, CShotStep::set(*address, "currentIndex", index));
      }
    });
    QObject::connect(bar, &QTabBar::tabCloseRequested, &recorder, [bar, &recorder](int index) {
      const std::optional<QString>& address = recorder.address(bar);
      if (!address.has_value()) {
        return;
      }
      // The tab switch the close caused is part of the close.
      recorder.retractLast(
          bar, [](const QJsonObject& step, bool sameFrame) { return sameFrame && "set" == step["do"].toString(); });
      recorder.record(bar, CShotStep::closeTab(*address, index));
    });
  }

  qint32 replay(const QJsonObject& step, QObject* target) const override {
    QTabBar* bar = static_cast<QTabBar*>(target);
    if (const qint32 failed = failUnlessUsable(bar, step); 0 != failed) {
      return failed;
    }
    const bool close = "close" == step["do"].toString();
    const int index = close ? step["index"].toInt() : step["value"].toInt();
    if (index < 0 || index >= bar->count()) {
      return fail(QString("the tab bar has no tab %1: %2").arg(index).arg(compact(step)));
    }
    if (close) {
      // The close button's position is up to the style.
      emit bar->tabCloseRequested(index);
      return 0;
    }
    bar->setCurrentIndex(index);
    return (bar->currentIndex() == index) ? 0 : fail("the tab bar did not go to the tab: " + compact(step));
  }
};

/** A header, by the section clicked. */
class CHeaderViewHandler : public IShotHandler {
 public:
  const QMetaObject* handles() const override { return &QHeaderView::staticMetaObject; }

  void watch(QObject* object, CShotRecorder& recorder) const override {
    QHeaderView* header = static_cast<QHeaderView*>(object);
    // A resized section is layout, not a step.
    QObject::connect(header, &QHeaderView::sectionClicked, &recorder, [header, &recorder](int section) {
      if (const std::optional<QString>& address = recorder.address(header); address.has_value()) {
        recorder.record(header, CShotStep::clickSection(*address, section));
      }
    });
  }

  qint32 replay(const QJsonObject& step, QObject* target) const override {
    QHeaderView* header = static_cast<QHeaderView*>(target);
    if (const qint32 failed = failUnlessUsable(header, step); 0 != failed) {
      return failed;
    }
    const int section = step["section"].toInt();
    if (section < 0 || section >= header->count() || header->isSectionHidden(section)) {
      return fail(QString("the header has no section %1 to click: %2").arg(section).arg(compact(step)));
    }
    const QPoint at(header->sectionViewportPosition(section) + header->sectionSize(section) / 2,
                    header->viewport()->height() / 2);
    if (const qint32 failed = failUnlessReached(header->viewport(), at, step); 0 != failed) {
      return failed;
    }
    CShotSynth::mouse(QTest::MouseClick, header->viewport(), Qt::LeftButton, Qt::NoModifier, at);
    if (header->isSortIndicatorShown() && header->sortIndicatorSection() != section) {
      return fail("the header did not sort by the section: " + compact(step));
    }
    return 0;
  }
};

/** Sees a press on a widget before the widget does. */
class CPressWatch : public QObject {
 public:
  /** @brief Watch @p viewport for @p recorder; gone with whichever of the two goes first. */
  static void install(QObject* recorder, QWidget* viewport, const std::function<void(const QMouseEvent*)>& pressed) {
    CPressWatch* watch = new CPressWatch(recorder, pressed);
    viewport->installEventFilter(watch);
    QObject::connect(viewport, &QObject::destroyed, watch, [watch]() { delete watch; });
  }

 private:
  CPressWatch(QObject* parent, const std::function<void(const QMouseEvent*)>& pressed)
      : QObject(parent), pressed(pressed) {}

 protected:
  bool eventFilter(QObject* watched, QEvent* event) override {
    if (QEvent::MouseButtonPress == event->type() || QEvent::MouseButtonDblClick == event->type()) {
      pressed(static_cast<const QMouseEvent*>(event));
    }
    return QObject::eventFilter(watched, event);
  }

 private:
  std::function<void(const QMouseEvent*)> pressed;
};

/** An item view, by the row acted on and the place in it. */
class CItemViewHandler : public IShotHandler {
 public:
  const QMetaObject* handles() const override { return &QAbstractItemView::staticMetaObject; }

  void watch(QObject* object, CShotRecorder& recorder) const override {
    QAbstractItemView* view = static_cast<QAbstractItemView*>(object);
    // A parentless view is a completer's list, recorded as the input's text.
    if (nullptr == view->parentWidget()) {
      return;
    }
    // A combo box's popup is recorded as the combo box.
    for (const QWidget* w = view; nullptr != w; w = w->parentWidget()) {
      if (nullptr != qobject_cast<const QComboBox*>(w)) {
        return;
      }
    }
    // The row double clicked, until the next press: when the delegate takes a double click, its release still emits
    // `clicked`.
    const std::shared_ptr<QPersistentModelIndex> doubled = std::make_shared<QPersistentModelIndex>();
    CPressWatch::install(&recorder, view->viewport(),
                         [doubled](const QMouseEvent*) { *doubled = QPersistentModelIndex(); });

    // Emitted for any button; the right one is the `menu` step.
    QObject::connect(view, &QAbstractItemView::clicked, &recorder,
                     [this, view, doubled, &recorder](const QModelIndex& index) {
                       if (doubled->isValid() && *doubled == index) {
                         return;
                       }
                       if (Qt::LeftButton == CShotApplication::frame().button) {
                         recordRow(view, index, recorder, "select");
                       }
                     });
    QObject::connect(view, &QAbstractItemView::doubleClicked, &recorder,
                     [this, view, doubled, &recorder](const QModelIndex& index) {
                       if (Qt::LeftButton != CShotApplication::frame().button) {
                         return;
                       }
                       *doubled = index;
                       // The pair's first click is part of the double click.
                       const QString& row = rowOf(view, index);
                       recorder.retractLast(view, [row](const QJsonObject& step, bool) {
                         return "select" == step["do"].toString() && row == step["row"].toString();
                       });
                       recordRow(view, index, recorder, "dclick");
                     });

    // Scrolled: the top row, which unlike pixels is the same on every machine. A key's scroll is the key's.
    const auto scrolled = [this, view, &recorder]() {
      if (QEvent::KeyPress == CShotApplication::inputFrame().type) {
        return;
      }
      const std::optional<QString>& address = recorder.address(view);
      const QString& row = rowOf(view, view->indexAt(QPoint(1, 1)));
      if (!address.has_value() || row.isEmpty()) {
        return;
      }
      QJsonObject step{{"do", "scroll"}, {"widget", *address}, {"row", row}};
      const QScrollBar* sideways = view->horizontalScrollBar();
      if (sideways->maximum() > sideways->minimum()) {
        step["x"] = double(sideways->value() - sideways->minimum()) / (sideways->maximum() - sideways->minimum());
      }
      recorder.amend(view, step);
    };
    QObject::connect(view->verticalScrollBar(), &QScrollBar::valueChanged, &recorder, scrolled);
    QObject::connect(view->horizontalScrollBar(), &QScrollBar::valueChanged, &recorder, scrolled);

    QTreeView* tree = qobject_cast<QTreeView*>(view);
    if (nullptr == tree) {
      return;
    }
    QObject::connect(tree, &QTreeView::expanded, &recorder,
                     [this, view, &recorder](const QModelIndex& index) { recordRow(view, index, recorder, "expand"); });
    QObject::connect(tree, &QTreeView::collapsed, &recorder, [this, view, &recorder](const QModelIndex& index) {
      recordRow(view, index, recorder, "collapse");
    });
  }

  QJsonObject contextMenu(QWidget* target, const QPoint& pos, const CShotRecorder& recorder) const override {
    const QAbstractItemView* view = static_cast<const QAbstractItemView*>(target);
    const std::optional<QString>& address = recorder.address(view);
    if (!address.has_value()) {
      return QJsonObject();
    }
    const QPoint& inViewport = view->viewport()->mapFrom(view, pos);
    const QString& row = rowOf(view, view->indexAt(inViewport));
    QJsonObject step = CShotStep::menu(*address, row);
    step["at"] =
        row.isEmpty() ? fractionIn(view->viewport()->rect(), inViewport) : fractionIn(rectOf(view, row), inViewport);
    return step;
  }

  qint32 replay(const QJsonObject& step, QObject* target) const override {
    QAbstractItemView* view = static_cast<QAbstractItemView*>(target);
    if (const qint32 failed = failUnlessUsable(view, step); 0 != failed) {
      return failed;
    }
    const QString& verb = step["do"].toString();
    const QString& row = step["row"].toString();

    if ("scroll" == verb) {
      if (!scrollToTop(view, row)) {
        return fail("the view has no such row to scroll to: " + compact(step));
      }
      if (step.contains("x")) {
        QScrollBar* sideways = view->horizontalScrollBar();
        sideways->setValue(sideways->minimum() +
                           qRound(step["x"].toDouble() * (sideways->maximum() - sideways->minimum())));
      }
      // Stopped short by the list's end: the recording came from another state.
      return (rowOf(view, view->indexAt(QPoint(1, 1))) == row)
                 ? 0
                 : fail("the view did not scroll there: " + compact(step));
    }

    QPoint at;
    if (row.isEmpty()) {
      at = pointAt(view->viewport()->rect(), step["at"].toArray());
    } else {
      // Measured after the scroll. A row under a collapsed parent was expanded by hand; scrollTo() expands to it.
      if (!rectOf(view, row).isValid()) {
        reveal(view, row);
      }
      const QRect& before = rectOf(view, row);
      if (!before.isValid()) {
        return fail("the view has no such row: " + compact(step));
      }
      view->scrollTo(view->indexAt(before.center()));
      const QRect& rect = rectOf(view, row);
      at = step.contains("at") ? pointAt(rect, step["at"].toArray()) : rect.center();
    }

    if ("expand" != verb && "collapse" != verb) {
      if (const qint32 failed = failUnlessReached(view->viewport(), at, step); 0 != failed) {
        return failed;
      }
    }
    if ("expand" == verb || "collapse" == verb) {
      QTreeView* tree = qobject_cast<QTreeView*>(view);
      if (nullptr == tree) {
        return fail("the view is no tree: " + compact(step));
      }
      const QModelIndex& index = view->indexAt(at);
      tree->setExpanded(index, "expand" == verb);
      return (tree->isExpanded(index) == ("expand" == verb)) ? 0
                                                             : fail("the row did not " + verb + ": " + compact(step));
    }
    if ("menu" == verb) {
      CShotSynth::arrive(view->viewport(), at);
      CShotSynth::mouse(QTest::MouseClick, view->viewport(), Qt::RightButton, Qt::NoModifier, at);
      return 0;
    }
    if ("select" != verb && "dclick" != verb) {
      return fail("an item view cannot " + verb);
    }
    CShotSynth::mouse(("dclick" == verb) ? QTest::MouseDClick : QTest::MouseClick, view->viewport(), Qt::LeftButton,
                      Qt::NoModifier, at);
    return (rowOf(view, view->currentIndex()) == row) ? 0 : fail("the row did not become current: " + compact(step));
  }

 protected:
  /** @return the row path of @p index */
  virtual QString rowOf(const QAbstractItemView* view, const QModelIndex& index) const {
    Q_UNUSED(view)
    return index.isValid() ? CShotAddress::rowPathOf(index) : QString();
  }

  /** @return the row's rectangle in the viewport, invalid when there is no such row */
  virtual QRect rectOf(const QAbstractItemView* view, const QString& row) const {
    const QModelIndex& index = CShotAddress::resolveRowPath(*view->model(), row);
    return index.isValid() ? view->visualRect(index) : QRect();
  }

  /** @brief Scroll to @p row, which expands its collapsed parents in a tree */
  virtual void reveal(QAbstractItemView* view, const QString& row) const {
    const QModelIndex& index = CShotAddress::resolveRowPath(*view->model(), row);
    if (index.isValid()) {
      view->scrollTo(index);
    }
  }

  /** @brief Scroll @p row to the top; false when there is no such row */
  virtual bool scrollToTop(QAbstractItemView* view, const QString& row) const {
    const QModelIndex& index = CShotAddress::resolveRowPath(*view->model(), row);
    if (!index.isValid()) {
      return false;
    }
    view->scrollTo(index, QAbstractItemView::PositionAtTop);
    return true;
  }

 private:
  /** @brief Record @p verb on the row @p index, with where in the row the input was */
  void recordRow(QAbstractItemView* view, const QModelIndex& index, CShotRecorder& recorder,
                 const QString& verb) const {
    const std::optional<QString>& address = recorder.address(view);
    if (!address.has_value()) {
      return;
    }
    const QString& row = rowOf(view, index);
    if (row.isEmpty()) {
      qWarning() << "shoot: a row of" << view->metaObject()->className() << "has no name and is not recorded";
      return;
    }
    QJsonObject step{{"do", verb}, {"widget", *address}, {"row", row}};
    // A click on a check box or a delegate's button differs from one on the text.
    const SShotFrame& frame = CShotApplication::frame();
    if (("select" == verb || "dclick" == verb) && !frame.receiver.isNull()) {
      if (const QWidget* receiver = qobject_cast<const QWidget*>(frame.receiver); nullptr != receiver) {
        const QPoint& inViewport = view->viewport()->mapFromGlobal(receiver->mapToGlobal(frame.pos));
        step["at"] = fractionIn(rectOf(view, row), inViewport);
      }
    }
    recorder.record(view, step);
  }
};

/**
   @return the option a view hands its delegate's editorEvent(), rebuilt because initViewItemOption() is protected; the
           delegates read only rect, font and focus
 */
QStyleOptionViewItem delegateOptionOf(const QAbstractItemView* view, const QModelIndex& index) {
  QStyleOptionViewItem opt;
  opt.initFrom(view);
  opt.font = view->font();
  opt.rect = view->visualRect(index);
  opt.state &= ~QStyle::State_HasFocus;
  if (index == view->currentIndex()) {
    opt.state |= QStyle::State_HasFocus;
  }
  return opt;
}

/**
   A tree whose delegate paints buttons into its rows: `sigButtonPressed` is a `click` with row and button name, and
   replaces the row's `select`. Replayed as a click on the button's current rect, checked by the signal.
 */
template <typename Delegate>
class CRowButtonHandler : public CItemViewHandler {
 public:
  void watch(QObject* object, CShotRecorder& recorder) const override {
    CItemViewHandler::watch(object, recorder);
    QTreeWidget* tree = static_cast<QTreeWidget*>(object);
    Delegate* delegate = qobject_cast<Delegate*>(tree->itemDelegate());
    if (nullptr == delegate) {
      return;
    }

    // Named before the delegate acts, which can rename or remove the row. Kept until the next press: the release
    // still emits `clicked`.
    const std::shared_ptr<press_t> press = std::make_shared<press_t>();
    const key_t key(&recorder, tree);
    presses().insert(key, press);
    QObject::connect(tree, &QObject::destroyed, &recorder, [key]() { presses().remove(key); });
    QObject::connect(&recorder, &QObject::destroyed, [key]() { presses().remove(key); });
    CPressWatch::install(&recorder, tree->viewport(), [this, tree, press](const QMouseEvent* event) {
      const QModelIndex& index = tree->indexAt(event->position().toPoint());
      // The second press of a double click arrives only as the double click.
      if (QEvent::MouseButtonDblClick == event->type() && press->took && press->index == index) {
        press->doubled = true;
        return;
      }
      press->index = index;
      press->row = rowOf(tree, index);
      press->button = event->button();
      press->name.clear();
      press->took = false;
      press->doubled = false;
    });

    QObject::connect(
        delegate, &Delegate::sigButtonPressed, &recorder,
        [tree, press, &recorder](const QModelIndex& index, typename Delegate::button_e button) {
          // Compared, never dereferenced: the row may be gone.
          if (press->index != index) {
            return;
          }
          press->took = true;
          press->name = Delegate::buttonName(button);
          const std::optional<QString>& address = recorder.address(tree);
          if (!address.has_value()) {
            return;
          }
          if (press->row.isEmpty()) {
            qWarning() << "shoot: a row of" << tree->metaObject()->className() << "has no name, its button"
                       << Delegate::buttonName(button) << "is not recorded";
            return;
          }
          QJsonObject step{
              {"do", "click"}, {"widget", *address}, {"row", press->row}, {"button", Delegate::buttonName(button)}};
          // A delegate acts on a press of any mouse button.
          if (Qt::LeftButton != press->button) {
            step["mouse"] = buttonName(press->button);
          }
          recorder.record(tree, step);
        });
    // After the view handler's connection, so its `select` exists to retract.
    QObject::connect(tree, &QAbstractItemView::clicked, &recorder,
                     [this, tree, press, &recorder](const QModelIndex& index) {
                       // Left set for a following double click.
                       if (!press->took || press->index != index) {
                         return;
                       }
                       const QString& row = rowOf(tree, index);
                       recorder.retractLast(tree, [row](const QJsonObject& step, bool sameFrame) {
                         return sameFrame && "select" == step["do"].toString() && row == step["row"].toString();
                       });
                     });
    // A double click on a button is one step, replacing the row's `dclick` and the button's `click`.
    QObject::connect(
        tree, &QAbstractItemView::doubleClicked, &recorder, [this, tree, press, &recorder](const QModelIndex& index) {
          if (!press->doubled || press->index != index) {
            return;
          }
          press->doubled = false;
          const QString& row = rowOf(tree, index);
          recorder.retractLast(tree, [row](const QJsonObject& step, bool sameFrame) {
            return sameFrame && "dclick" == step["do"].toString() && row == step["row"].toString();
          });
          const QString name = press->name;
          const QString pressedRow = press->row;
          const bool retracted = recorder.retractLast(tree, [name, pressedRow](const QJsonObject& step, bool) {
            return "click" == step["do"].toString() && name == step["button"].toString() &&
                   pressedRow == step["row"].toString();
          });
          const std::optional<QString>& address = recorder.address(tree);
          if (!retracted || !address.has_value()) {
            return;
          }
          QJsonObject step{{"do", "dclick"}, {"widget", *address}, {"row", pressedRow}, {"button", name}};
          if (Qt::LeftButton != press->button) {
            step["mouse"] = buttonName(press->button);
          }
          recorder.record(tree, step);
        });
  }

  /** No `menu` step for a right press on a row button: the button step replays it. */
  QJsonObject contextMenu(QWidget* target, const QPoint& pos, const CShotRecorder& recorder) const override {
    const std::shared_ptr<press_t>& press = presses().value(key_t(&recorder, target));
    const QEvent::Type input = CShotApplication::inputFrame().type;
    if (nullptr != press && press->took && Qt::RightButton == press->button &&
        (QEvent::MouseButtonPress == input || QEvent::MouseButtonRelease == input)) {
      return QJsonObject();
    }
    return CItemViewHandler::contextMenu(target, pos, recorder);
  }

  qint32 replay(const QJsonObject& step, QObject* target) const override {
    if (!step.contains("button")) {
      return CItemViewHandler::replay(step, target);
    }
    QTreeWidget* tree = static_cast<QTreeWidget*>(target);
    if (const qint32 failed = failUnlessUsable(tree, step); 0 != failed) {
      return failed;
    }
    Delegate* delegate = qobject_cast<Delegate*>(tree->itemDelegate());
    if (nullptr == delegate) {
      return fail("the view paints no row buttons: " + compact(step));
    }
    const typename Delegate::button_e button = Delegate::buttonByName(step["button"].toString());
    if (Delegate::button_e::eNone == button) {
      return fail("no row button has that name: " + compact(step));
    }
    QTreeWidgetItem* item = itemOf(tree, step["row"].toString());
    if (nullptr == item) {
      return fail("the view has no such row: " + compact(step));
    }

    // Measured after the scroll.
    tree->scrollToItem(item);
    const QModelIndex& index = tree->indexAt(tree->visualItemRect(item).center());
    const QRect& rect = delegate->buttonRect(delegateOptionOf(tree, index), index, button);
    if (!rect.isValid()) {
      return fail("the row does not show the button now: " + compact(step));
    }
    if (const qint32 failed = failUnlessReached(tree->viewport(), rect.center(), step); 0 != failed) {
      return failed;
    }

    const Qt::MouseButton mouse = step.contains("mouse") ? buttonOf(step["mouse"].toString()) : Qt::LeftButton;
    if (Qt::NoButton == mouse) {
      return fail("the step names no mouse button: " + compact(step));
    }
    if (Qt::RightButton == mouse) {
      // A context menu goes to the widget last entered.
      CShotSynth::arrive(tree->viewport(), rect.center());
    }

    bool pressed = false;
    const QPersistentModelIndex wanted(index);
    const QMetaObject::Connection connection =
        QObject::connect(delegate, &Delegate::sigButtonPressed,
                         [&pressed, wanted, button](const QModelIndex& at, typename Delegate::button_e which) {
                           pressed = pressed || (wanted == at && button == which);
                         });
    CShotSynth::mouse(("dclick" == step["do"].toString()) ? QTest::MouseDClick : QTest::MouseClick, tree->viewport(),
                      mouse, Qt::NoModifier, rect.center());
    QObject::disconnect(connection);
    return pressed ? 0 : fail("the button did not act: " + compact(step));
  }

 protected:
  /** @return the row @p row names in @p tree, nullptr when there is none */
  virtual QTreeWidgetItem* itemOf(const QTreeWidget* tree, const QString& row) const = 0;

 private:
  /** The last press on a tree, read before the delegate acted. */
  struct press_t {
    QPersistentModelIndex index;
    QString row;
    Qt::MouseButton button = Qt::NoButton;
    QString name;         /**< the button that acted */
    bool took = false;    /**< a row button acted */
    bool doubled = false; /**< a double click followed on the same row */
  };

  /** Per recorder and tree. */
  using key_t = QPair<const CShotRecorder*, const QObject*>;

  /** @return each watched tree's last press */
  static QHash<key_t, std::shared_ptr<press_t>>& presses() {
    static QHash<key_t, std::shared_ptr<press_t>> byTree;
    return byTree;
  }
};

/** The workspace, whose rows are named by item path. */
class CWksListHandler : public CRowButtonHandler<CWksItemDelegate> {
 public:
  const QMetaObject* handles() const override { return &CGisListWks::staticMetaObject; }

 protected:
  QString rowOf(const QAbstractItemView* view, const QModelIndex& index) const override {
    if (!index.isValid()) {
      return QString();
    }
    // QTreeWidget::itemFromIndex() is protected.
    const QTreeWidget* tree = static_cast<const QTreeWidget*>(view);
    return CShotAddress::itemPathOf(tree->itemAt(view->visualRect(index).center()));
  }

  QRect rectOf(const QAbstractItemView* view, const QString& row) const override {
    const CGisListWks* list = static_cast<const CGisListWks*>(view);
    QTreeWidgetItem* item = CShotAddress::resolveItemPath(*list, row);
    return (nullptr == item) ? QRect() : list->visualItemRect(item);
  }

  void reveal(QAbstractItemView* view, const QString& row) const override {
    CGisListWks* list = static_cast<CGisListWks*>(view);
    if (QTreeWidgetItem* item = CShotAddress::resolveItemPath(*list, row); nullptr != item) {
      list->scrollToItem(item);
    }
  }

  bool scrollToTop(QAbstractItemView* view, const QString& row) const override {
    CGisListWks* list = static_cast<CGisListWks*>(view);
    QTreeWidgetItem* item = CShotAddress::resolveItemPath(*list, row);
    if (nullptr == item) {
      return false;
    }
    list->scrollToItem(item, QAbstractItemView::PositionAtTop);
    return true;
  }

  QTreeWidgetItem* itemOf(const QTreeWidget* tree, const QString& row) const override {
    return CShotAddress::resolveItemPath(*static_cast<const CGisListWks*>(tree), row);
  }
};

/** The database and the map tree, whose rows are named by their text. */
template <typename Tree, typename Delegate>
class CNamedRowsHandler : public CRowButtonHandler<Delegate> {
 public:
  const QMetaObject* handles() const override { return &Tree::staticMetaObject; }

 protected:
  QString rowOf(const QAbstractItemView* view, const QModelIndex& index) const override {
    if (!index.isValid()) {
      return QString();
    }
    const QTreeWidget* tree = static_cast<const QTreeWidget*>(view);
    return CShotAddress::namePathOf(tree->itemAt(view->visualRect(index).center()));
  }

  QRect rectOf(const QAbstractItemView* view, const QString& row) const override {
    const QTreeWidget* tree = static_cast<const QTreeWidget*>(view);
    QTreeWidgetItem* item = CShotAddress::resolveNamePath(*tree, row);
    return (nullptr == item) ? QRect() : tree->visualItemRect(item);
  }

  bool scrollToTop(QAbstractItemView* view, const QString& row) const override {
    QTreeWidget* tree = static_cast<QTreeWidget*>(view);
    QTreeWidgetItem* item = CShotAddress::resolveNamePath(*tree, row);
    if (nullptr == item) {
      return false;
    }
    tree->scrollToItem(item, QAbstractItemView::PositionAtTop);
    return true;
  }

  QTreeWidgetItem* itemOf(const QTreeWidget* tree, const QString& row) const override {
    return CShotAddress::resolveNamePath(*tree, row);
  }
};

// ---- surfaces

/**
   A self-painted widget without meaningful signals: its input is recorded in the surface's own units and replayed as
   input.

   Press to release is one `click`, `drag` or `dclick`: the press in surface units, the release as a pixel offset,
   because a drag moves the content. `held` is kept past CMouseAdapter::clickTimeout. A hover is one amended `move`.
   A right click replays its context menu, so there is no `menu` step.
 */
class CSurfaceHandler : public IShotHandler {
 public:
  void watch(QObject* object, CShotRecorder& recorder) const override {
    Q_UNUSED(object)
    Q_UNUSED(recorder)
  }

  QJsonObject contextMenu(QWidget* target, const QPoint& pos, const CShotRecorder& recorder) const override {
    Q_UNUSED(target)
    Q_UNUSED(pos)
    Q_UNUSED(recorder)
    return QJsonObject();
  }

  void input(QWidget* surface, const QEvent* event, CShotRecorder& recorder) const override {
    switch (event->type()) {
      case QEvent::Wheel:
        wheel(surface, static_cast<const QWheelEvent*>(event), recorder);
        break;
      case QEvent::MouseMove:
      case QEvent::MouseButtonPress:
      case QEvent::MouseButtonDblClick:
      case QEvent::MouseButtonRelease:
        mouse(surface, static_cast<const QMouseEvent*>(event), recorder);
        break;
      default:
        break;
    }
  }

  qint32 replay(const QJsonObject& step, QObject* target) const override {
    QWidget* surface = static_cast<QWidget*>(target);
    if (const qint32 failed = failUnlessUsable(surface, step); 0 != failed) {
      return failed;
    }
    if (!settle(surface)) {
      return fail(classOf(surface) + " did not finish drawing before the step: " + compact(step));
    }
    const QString& verb = step["do"].toString();
    const Qt::KeyboardModifiers mods = modsOf(step);
    if (("move" == verb || "release" == verb) && step.contains("dx")) {
      // Part of a split gesture: offset from the replayed press.
      const auto pressed = pressedAt().constFind(surface);
      if (pressed == pressedAt().constEnd()) {
        return fail("the step belongs to a press that was not replayed: " + compact(step));
      }
      const QPoint& pixel = pressed.value() + QPoint(step["dx"].toInt(), step["dy"].toInt());
      if ("move" == verb) {
        CShotSynth::move(surface, pixel, mods);
        return 0;
      }
      pressedAt().remove(surface);
      CShotSynth::mouse(QTest::MouseRelease, surface, buttonOf(step["button"].toString()), mods, pixel);
      return 0;
    }
    const std::optional<QPoint>& at = pixelOf(surface, step);
    if (!at.has_value()) {
      return fail(classOf(surface) + " does not show where the step was: " + compact(step));
    }
    if (const qint32 failed = failUnlessReached(surface, *at, step); 0 != failed) {
      return failed;
    }
    if (const QString& hit = step["hit"].toString(); !hit.isEmpty()) {
      if (const QString& found = hitAt(surface, *at); found != hit) {
        return fail(QString("the step expects %1 under the pointer and finds %2: %3")
                        .arg(hit, found.isEmpty() ? "nothing" : found, compact(step)));
      }
    }

    if ("move" == verb) {
      CShotSynth::move(surface, *at);
      return 0;
    }
    if ("wheel" == verb) {
      const QJsonArray& angle = step["angle"].toArray();
      CShotSynth::wheel(surface, *at, QPoint(angle.at(0).toInt(), angle.at(1).toInt()), mods);
      return 0;
    }
    const Qt::MouseButton button = buttonOf(step["button"].toString());
    if (Qt::NoButton == button) {
      return fail("the step names no button: " + compact(step));
    }
    if ("press" == verb) {
      pressedAt().insert(surface, *at);
      CShotSynth::mouse(QTest::MousePress, surface, button, mods, *at);
      return 0;
    }
    if ("dclick" == verb) {
      CShotSynth::mouse(QTest::MouseDClick, surface, button, mods, *at);
      return 0;
    }
    if ("click" != verb && "drag" != verb) {
      return fail(classOf(surface) + " cannot " + verb);
    }
    const QPoint& end = *at + QPoint(step["dx"].toInt(), step["dy"].toInt());
    CShotSynth::mouse(QTest::MousePress, surface, button, mods, *at);
    if (end != *at) {
      CShotSynth::move(surface, end);
    }
    if (step.contains("held")) {
      QTest::qWait(step["held"].toInt());
    }
    CShotSynth::mouse(QTest::MouseRelease, surface, button, mods, end);
    return 0;
  }

 protected:
  /** @return @p pixel in the surface's units; empty where the surface ignores input */
  virtual QJsonObject where(QWidget* surface, const QPoint& pixel) const = 0;

  /** @return what a replay must find under @p pixel; empty for nothing */
  virtual QString hitAt(QWidget* surface, const QPoint& pixel) const {
    Q_UNUSED(surface)
    Q_UNUSED(pixel)
    return QString();
  }

  /** @return the pixel of @p step's position now; nothing when not shown */
  virtual std::optional<QPoint> pixelOf(QWidget* surface, const QJsonObject& step) const = 0;

  /** @brief Wait until @p surface shows what the previous step did; false when it never does */
  virtual bool settle(QWidget* surface) const {
    Q_UNUSED(surface)
    return true;
  }

 private:
  /** @return each surface's replayed `press` until its `release` */
  static QHash<const QWidget*, QPoint>& pressedAt() {
    static QHash<const QWidget*, QPoint> pixels;
    return pixels;
  }

  QJsonObject stepAt(const QString& verb, QWidget* surface, const QPoint& pixel, const CShotRecorder& recorder) const {
    const QJsonObject& position = where(surface, pixel);
    const std::optional<QString>& address = position.isEmpty() ? std::nullopt : recorder.address(surface);
    if (!address.has_value()) {
      return QJsonObject();
    }
    QJsonObject step = CShotStep::surface(verb, *address, position);
    if (const QString& hit = hitAt(surface, pixel); !hit.isEmpty()) {
      step["hit"] = hit;
    }
    return step;
  }

  void wheel(QWidget* surface, const QWheelEvent* wheel, CShotRecorder& recorder) const {
    QJsonObject step = stepAt("wheel", surface, wheel->position().toPoint(), recorder);
    if (step.isEmpty()) {
      return;
    }
    // Both deltas: Alt turns the wheel sideways, and IPlot zooms one axis by it.
    step["angle"] = QJsonArray({wheel->angleDelta().x(), wheel->angleDelta().y()});
    putModNames(step, wheel->modifiers());
    recorder.record(surface, step);
  }

  void mouse(QWidget* surface, const QMouseEvent* mouse, CShotRecorder& recorder) const {
    const QPoint& pixel = mouse->position().toPoint();
    CShotRecorder::gesture_t* held = recorder.gesture(surface);

    // A menu the press opened may take the release: any event without the gesture's button ends it.
    if (nullptr != held && QEvent::MouseButtonRelease != mouse->type() &&
        !(mouse->buttons() & buttonOf(held->step["button"].toString()))) {
      finish(held, held->pixel + QPoint(held->step["dx"].toInt(), held->step["dy"].toInt()));
      recorder.endGesture(surface);
      held = nullptr;
    }

    if (QEvent::MouseMove == mouse->type()) {
      if (nullptr != held) {
        const QPoint& offset = pixel - held->pixel;
        held->step["dx"] = offset.x();
        held->step["dy"] = offset.y();
        // For a split gesture; an offset, because a dragged surface moves under the pointer.
        QJsonObject step = held->step;
        step["do"] = "move";
        step["dx"] = offset.x();
        step["dy"] = offset.y();
        for (const char* key : {"lat", "lon", "x", "at", "icon", "hit", "button", "held"}) {
          step.remove(key);
        }
        held->moved = step;
        if (held->split) {
          recorder.amend(surface, step);
        }
      } else if (Qt::NoButton == mouse->buttons()) {
        QJsonObject step = stepAt("move", surface, pixel, recorder);
        if (step.isEmpty()) {
          return;
        }
        putModNames(step, mouse->modifiers());
        recorder.amend(surface, step);
      }
      return;
    }

    if (QEvent::MouseButtonRelease == mouse->type()) {
      if (nullptr == held || held->step["button"].toString() != buttonName(mouse->button())) {
        return;
      }
      finish(held, pixel);
      recorder.endGesture(surface);
      return;
    }

    // A second button belongs to the held gesture.
    if (nullptr != held || buttonName(mouse->button()).isEmpty()) {
      return;
    }
    const bool twice = QEvent::MouseButtonDblClick == mouse->type();
    QJsonObject step = stepAt(twice ? "dclick" : "click", surface, pixel, recorder);
    if (step.isEmpty()) {
      return;
    }
    step["button"] = buttonName(mouse->button());
    putModNames(step, mouse->modifiers());
    if (twice) {
      // The pair's first click is part of the double click; a hover may lie between them.
      const QString& button = step["button"].toString();
      recorder.retractLast(
          surface,
          [button](const QJsonObject& previous, bool) {
            return "click" == previous["do"].toString() && button == previous["button"].toString();
          },
          [](const QJsonObject& passed) { return "move" == passed["do"].toString(); });
    }
    recorder.beginGesture(surface, step, pixel);
  }

  /** @brief The gesture @p held is complete, its button up at @p pixel of the surface */
  static void finish(CShotRecorder::gesture_t* held, const QPoint& pixel) {
    const QPoint& offset = pixel - held->pixel;
    held->step.remove("dx");
    held->step.remove("dy");
    if (!offset.isNull()) {
      held->step["dx"] = offset.x();
      held->step["dy"] = offset.y();
    }
    if ("click" == held->step["do"].toString() &&
        offset.manhattanLength() >= CMouseAdapter::minimalMouseMovingDistance) {
      held->step["do"] = "drag";
    }
    // The monotonic clock, as CMouseAdapter: QTest makes up event timestamps.
    const qint64 duration = CShotRecorder::now() - held->pressedAt;
    if (duration >= CMouseAdapter::clickTimeout) {
      held->step["held"] = duration;
    }
  }
};

/** The map: degrees, and the item under the point. */
class CCanvasHandler : public CSurfaceHandler {
 public:
  const QMetaObject* handles() const override { return &CCanvas::staticMetaObject; }

 protected:
  QJsonObject where(QWidget* surface, const QPoint& pixel) const override {
    QPointF pos(pixel);
    static_cast<CCanvas*>(surface)->convertPx2Rad(pos);
    pos *= RAD_TO_DEG;
    return QJsonObject{{"lat", pos.y()}, {"lon", pos.x()}};
  }

  /** The one item the map would act on at the point (within 20 px); empty for none or several. */
  QString hitAt(QWidget* surface, const QPoint& pixel) const override {
    Q_UNUSED(surface)
    QList<IGisItem*> items;
    CGisWorkspace::self().getItemsByPos(QPointF(pixel), items);
    return (1 == items.size()) ? CShotAddress::itemPathOf(items.first()) : QString();
  }

  /** An item's pixels are updated by the draw, so a hit or click after a zoom or pan needs it finished. */
  bool settle(QWidget* surface) const override {
    const CCanvas* canvas = static_cast<CCanvas*>(surface);
    QElapsedTimer timer;
    timer.start();
    while (!canvas->isDrawComplete()) {
      if (timer.hasExpired(kCanvasSettleMs)) {
        return false;
      }
      QEventLoop loop;
      QTimer::singleShot(kCanvasPollMs, &loop, &QEventLoop::quit);
      loop.exec(QEventLoop::ExcludeUserInputEvents);
    }
    return true;
  }

  std::optional<QPoint> pixelOf(QWidget* surface, const QJsonObject& step) const override {
    if (!step["lat"].isDouble() || !step["lon"].isDouble()) {
      return std::nullopt;
    }
    QPointF pos(step["lon"].toDouble(), step["lat"].toDouble());
    pos *= DEG_TO_RAD;
    static_cast<CCanvas*>(surface)->convertRad2Px(pos);
    const QPoint& pixel = pos.toPoint();
    return surface->rect().contains(pixel) ? std::optional<QPoint>(pixel) : std::nullopt;
  }
};

/** A track plot: the x axis value where IPlot::xValueAt() reads one, else a fraction of the widget. */
class CPlotHandler : public CSurfaceHandler {
 public:
  const QMetaObject* handles() const override { return &IPlot::staticMetaObject; }

 protected:
  QJsonObject where(QWidget* surface, const QPoint& pixel) const override {
    const qreal x = static_cast<IPlot*>(surface)->xValueAt(pixel);
    if (NOFLOAT != x) {
      return QJsonObject{{"x", x}};
    }
    return QJsonObject{{"at", fractionIn(surface->rect(), pixel)}};
  }

  std::optional<QPoint> pixelOf(QWidget* surface, const QJsonObject& step) const override {
    if (step.contains("x")) {
      const QPoint& pixel = static_cast<IPlot*>(surface)->pointOfXValue(step["x"].toDouble());
      return (NOPOINT == pixel) ? std::nullopt : std::optional<QPoint>(pixel);
    }
    if (step["at"].isArray()) {
      return pointAt(surface->rect(), step["at"].toArray());
    }
    return std::nullopt;
  }
};

/** The waypoint icon grid, by icon name: the grid reflows with its width. */
class CIconGridHandler : public CSurfaceHandler {
 public:
  const QMetaObject* handles() const override { return &CIconGrid::staticMetaObject; }

 protected:
  QJsonObject where(QWidget* surface, const QPoint& pixel) const override {
    const QString& icon = static_cast<CIconGrid*>(surface)->iconAt(pixel);
    return icon.isEmpty() ? QJsonObject() : QJsonObject{{"icon", icon}};
  }

  std::optional<QPoint> pixelOf(QWidget* surface, const QJsonObject& step) const override {
    const QRect& rect = static_cast<CIconGrid*>(surface)->rectOfIcon(step["icon"].toString());
    return rect.isValid() ? std::optional<QPoint>(rect.center()) : std::nullopt;
  }
};

/** @return the entry of a menu bar or menu @p owner whose menu is named @p name */
QAction* menuEntryIn(const QWidget* owner, const QString& name) {
  const QList<QAction*>& entries = owner->actions();
  for (QAction* entry : entries) {
    if (nullptr != entry->menu() && entry->menu()->objectName() == name) {
      return entry;
    }
  }
  return nullptr;
}

/** @brief Open a menu: a tool button's by showMenu(), a menu bar's or a submenu by a click on its entry */
qint32 replayOpenMenu(QWidget* root, const QJsonObject& step) {
  const QString& name = step["menu"].toString();
  if (!step.contains("widget") && !step.contains("action")) {
    QMenu* open = qobject_cast<QMenu*>(QApplication::activePopupWidget());
    QAction* entry = (nullptr == open) ? nullptr : menuEntryIn(open, name);
    if (nullptr == entry) {
      return fail("no open menu has the submenu: " + compact(step));
    }
    const QPoint& at = open->actionGeometry(entry).center();
    if (const qint32 failed = failUnlessReached(open, at, step); 0 != failed) {
      return failed;
    }
    CShotSynth::arrive(open, at);
    CShotSynth::mouse(QTest::MouseClick, open, Qt::LeftButton, Qt::NoModifier, at);
    return entry->menu()->isVisible() ? 0 : fail("the submenu did not open: " + compact(step));
  }

  QObject* target = CShotHandlers::resolve(root, step);
  if (QAction* action = qobject_cast<QAction*>(target); nullptr != action) {
    target = nullptr;
    const QList<QObject*>& owners = action->associatedObjects();
    for (QObject* owner : owners) {
      if (QToolButton* tool = qobject_cast<QToolButton*>(owner); nullptr != tool && tool->isVisible()) {
        target = tool;
        break;
      }
    }
  }
  if (QToolButton* tool = qobject_cast<QToolButton*>(target); nullptr != tool) {
    if (const qint32 failed = failUnlessUsable(tool, step); 0 != failed) {
      return failed;
    }
    tool->showMenu();
    return 0;
  }
  if (QMenuBar* bar = qobject_cast<QMenuBar*>(target); nullptr != bar) {
    if (const qint32 failed = failUnlessUsable(bar, step); 0 != failed) {
      return failed;
    }
    QAction* entry = menuEntryIn(bar, name);
    if (nullptr == entry) {
      return fail("the menu bar has no such menu: " + compact(step));
    }
    const QPoint& at = bar->actionGeometry(entry).center();
    if (const qint32 failed = failUnlessReached(bar, at, step); 0 != failed) {
      return failed;
    }
    CShotSynth::mouse(QTest::MouseClick, bar, Qt::LeftButton, Qt::NoModifier, at);
    return entry->menu()->isVisible() ? 0 : fail("the menu did not open: " + compact(step));
  }
  return fail("nothing opens the menu: " + compact(step));
}

const QHash<const QMetaObject*, const IShotHandler*>& handlers() {
  static const CActionHandler action;
  static const CButtonHandler button;
  static const CGroupBoxHandler group;
  static const CComboBoxHandler combo;
  static const CLineEditHandler edit;
  static const CIntSpinBoxHandler spin;
  static const CDoubleSpinBoxHandler doubleSpin;
  static const CDateTimeEditHandler dateTime;
  static const CSliderHandler slider;
  static const CSplitterHandler splitter;
  static const CDockWidgetHandler dock;
  static const CTabBarHandler tabs;
  static const CHeaderViewHandler header;
  static const CItemViewHandler view;
  static const CWksListHandler wks;
  static const CNamedRowsHandler<CGisListDB, CDBItemDelegate> database;
  static const CNamedRowsHandler<CMapTreeWidget, CMapItemDelegate> maps;
  static const CCanvasHandler canvas;
  static const CPlotHandler plot;
  static const CIconGridHandler grid;
  static const QList<const IShotHandler*> all = {&action,   &button,   &group,    &combo,  &edit, &spin,   &doubleSpin,
                                                 &dateTime, &slider,   &splitter, &dock,   &tabs, &header, &view,
                                                 &wks,      &database, &maps,     &canvas, &plot, &grid};

  static QHash<const QMetaObject*, const IShotHandler*> byClass;
  if (byClass.isEmpty()) {
    for (const IShotHandler* handler : all) {
      byClass.insert(handler->handles(), handler);
    }
  }
  return byClass;
}
}  // namespace

const IShotHandler* CShotHandlers::of(const QObject* object) {
  if (nullptr == object) {
    return nullptr;
  }
  const QHash<const QMetaObject*, const IShotHandler*>& byClass = handlers();
  // The nearest handler up the class hierarchy.
  for (const QMetaObject* meta = object->metaObject(); nullptr != meta; meta = meta->superClass()) {
    if (const IShotHandler* handler = byClass.value(meta, nullptr); nullptr != handler) {
      return handler;
    }
  }
  return nullptr;
}

QWidget* CShotHandlers::ownerOf(QWidget* widget) {
  if (nullptr == widget) {
    return nullptr;
  }
  QAbstractScrollArea* area = qobject_cast<QAbstractScrollArea*>(widget->parentWidget());
  return (nullptr != area && area->viewport() == widget) ? area : widget;
}

QString CShotHandlers::keyName(int key) {
  switch (key) {
    case 0:
    case Qt::Key_unknown:
    case Qt::Key_Shift:
    case Qt::Key_Control:
    case Qt::Key_Alt:
    case Qt::Key_AltGr:
    case Qt::Key_Meta:
    case Qt::Key_CapsLock:
    case Qt::Key_NumLock:
    case Qt::Key_ScrollLock:
      return QString();
    default:
      return QKeySequence(key).toString(QKeySequence::PortableText);
  }
}

void CShotHandlers::putMods(QJsonObject& step, Qt::KeyboardModifiers mods) { putModNames(step, mods); }

QObject* CShotHandlers::resolve(QWidget* root, const QJsonObject& step) {
  const QWidget* widget = CShotAddress::resolve(root, step["widget"].toString());
  if (step.contains("action")) {
    return actionIn(widget, step["action"].toString());
  }
  return step.contains("widget") ? const_cast<QWidget*>(widget) : nullptr;
}

qint32 CShotHandlers::replay(QWidget* root, const QJsonObject& step) {
  const QString& verb = step["do"].toString();
  if ("layout" == verb || "view" == verb) {
    return fail("a " + verb + " is the state the steps start from and is applied before them, not replayed as a step");
  }
  if ("openmenu" == verb) {
    return replayOpenMenu(root, step);
  }
  QObject* target = resolve(root, step);
  if (nullptr == target) {
    return fail("what the step was done to is not there: " + compact(step));
  }

  // For any widget class.
  QWidget* widget = qobject_cast<QWidget*>(target);
  if (nullptr != widget && "keypress" == verb) {
    if (const qint32 failed = failUnlessUsable(widget, step); 0 != failed) {
      return failed;
    }
    const QKeySequence& sequence = QKeySequence::fromString(step["key"].toString(), QKeySequence::PortableText);
    if (sequence.isEmpty()) {
      return fail("the step names no key: " + compact(step));
    }
    CShotSynth::key(widget, sequence[0].key(), modsOf(step), step["text"].toString());
    return 0;
  }
  if (nullptr != widget && "menu" == verb && !step.contains("row")) {
    if (const qint32 failed = failUnlessUsable(widget, step); 0 != failed) {
      return failed;
    }
    const QPoint& at = pointAt(widget->rect(), step["at"].toArray());
    if (const qint32 failed = failUnlessReached(widget, at, step); 0 != failed) {
      return failed;
    }
    CShotSynth::arrive(widget, at);
    CShotSynth::mouse(QTest::MouseClick, widget, Qt::RightButton, Qt::NoModifier, at);
    return 0;
  }

  const IShotHandler* handler = of(target);
  if (nullptr == handler) {
    return fail("no handler performs " + verb + " on a " + classOf(target) + ": " + compact(step));
  }
  return handler->replay(step, target);
}
