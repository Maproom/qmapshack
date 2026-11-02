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

#include "shoot/CShotPrivacy.h"

#include <QAbstractButton>
#include <QAction>
#include <QComboBox>
#include <QDir>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMenu>
#include <QPlainTextEdit>
#include <QPointer>
#include <QRegularExpression>
#include <QSet>
#include <QSignalBlocker>
#include <QTabBar>
#include <QTableWidget>
#include <QTextCursor>
#include <QTextDocument>
#include <QTextEdit>
#include <QTreeWidget>
#include <QTreeWidgetItemIterator>

#include "map/CMapDraw.h"

namespace {
const QString kHome = QStringLiteral("/home/user");
const QString kCache = QStringLiteral("/home/user/.cache/QMapShack");

/** What may follow a path component; a separator or anything that ends the path. */
const QString kEnd = QStringLiteral(R"((?![^/\\\s"'<>]))");
/** A path component. */
const QString kPart = QStringLiteral(R"([^/\\\s"'<>]+)");
/** The directories in front of the part that is matched: whole components only, so a separator or bracket before
    the path and a path before it in a list stay. */
const QString kLead = QStringLiteral(R"((?:(?<![A-Za-z0-9])[A-Za-z]:)?(?:[/\\][^/\\\s"'<>;,()=]+)*?)");

/** @return @p path with either separator matching, so a native path is found too */
QString anySeparator(const QString& path) {
  QStringList parts = QDir::fromNativeSeparators(path).split('/');
  for (QString& part : parts) {
    part = QRegularExpression::escape(part);
  }
  return parts.join(R"([/\\])");
}

/** @brief Append @p root and every widget below it to @p widgets, each once; @p seen holds what was appended. */
void collect(QWidget* root, QList<QWidget*>& widgets, QSet<QWidget*>& seen) {
  if (nullptr == root || seen.contains(root)) {
    return;
  }
  widgets << root;
  seen.insert(root);
  const QList<QWidget*>& children = root->findChildren<QWidget*>();
  for (QWidget* child : children) {
    if (!seen.contains(child)) {
      widgets << child;
      seen.insert(child);
    }
  }
}
}  // namespace

CShotPrivacy::CShotPrivacy(const QList<QWidget*>& roots) {
  const QList<rule_t>& list = rules();
  QList<QWidget*> widgets;
  QSet<QWidget*> seen;
  for (QWidget* root : roots) {
    collect(root, widgets, seen);
  }

  for (QWidget* w : std::as_const(widgets)) {
    const QPointer<QWidget> alive(w);
    // Signals blocked: a text set here is no input, and a slot could store it.
    const auto take = [&](const QString& before, const std::function<void(const QString&)>& set) {
      const QString& after = apply(list, before);
      if (after == before) {
        return;
      }
      {
        const QSignalBlocker blocker(w);
        set(after);
      }
      undo << [alive, set, before]() {
        if (alive) {
          const QSignalBlocker blocker(alive.data());
          set(before);
        }
      };
    };

    // A sorted view re-sorts on every rewritten row: the picture would show another order and the loop skip rows.
    // Pushed before the rows, so it is undone after them.
    const auto holdOrder = [this](auto* view) {
      if (!view->isSortingEnabled()) {
        return;
      }
      view->setSortingEnabled(false);
      const QPointer<QWidget> kept(view);
      undo << [kept, view]() {
        if (kept) {
          view->setSortingEnabled(true);
        }
      };
    };

    if (QLabel* label = qobject_cast<QLabel*>(w); nullptr != label) {
      take(label->text(), [label](const QString& s) { label->setText(s); });
    } else if (QLineEdit* edit = qobject_cast<QLineEdit*>(w); nullptr != edit) {
      // setText() moves the cursor to the end and clears the selection and the modified flag; its undo history is lost.
      const qint32 cursor = edit->cursorPosition();
      const qint32 start = edit->selectionStart();
      const qint32 length = edit->selectionLength();
      const bool modified = edit->isModified();
      const QString original = edit->text();
      take(original, [edit, original, cursor, start, length, modified](const QString& s) {
        edit->setText(s);
        // The positions belong to the original text.
        if (s != original) {
          edit->setModified(modified);
          return;
        }
        if (start < 0) {
          edit->setCursorPosition(cursor);
        } else if (cursor == start) {
          edit->setSelection(start + length, -length);
        } else {
          edit->setSelection(start, length);
        }
        edit->setModified(modified);
      });
    } else if (QAbstractButton* button = qobject_cast<QAbstractButton*>(w); nullptr != button) {
      take(button->text(), [button](const QString& s) { button->setText(s); });
    } else if (QGroupBox* group = qobject_cast<QGroupBox*>(w); nullptr != group) {
      take(group->title(), [group](const QString& s) { group->setTitle(s); });
    } else if (QTabBar* tabs = qobject_cast<QTabBar*>(w); nullptr != tabs) {
      for (qint32 i = 0; i < tabs->count(); i++) {
        take(tabs->tabText(i), [tabs, i](const QString& s) { tabs->setTabText(i, s); });
      }
    } else if (QComboBox* combo = qobject_cast<QComboBox*>(w); nullptr != combo) {
      for (qint32 i = 0; i < combo->count(); i++) {
        take(combo->itemText(i), [combo, i](const QString& s) { combo->setItemText(i, s); });
      }
    } else if (QListWidget* listWidget = qobject_cast<QListWidget*>(w); nullptr != listWidget) {
      holdOrder(listWidget);
      for (qint32 i = 0; i < listWidget->count(); i++) {
        QListWidgetItem* item = listWidget->item(i);
        take(item->text(), [item](const QString& s) { item->setText(s); });
      }
    } else if (QTreeWidget* tree = qobject_cast<QTreeWidget*>(w); nullptr != tree) {
      holdOrder(tree);
      for (QTreeWidgetItemIterator it(tree); nullptr != *it; ++it) {
        QTreeWidgetItem* item = *it;
        for (qint32 column = 0; column < item->columnCount(); column++) {
          take(item->text(column), [item, column](const QString& s) { item->setText(column, s); });
        }
      }
    } else if (QTableWidget* table = qobject_cast<QTableWidget*>(w); nullptr != table) {
      holdOrder(table);
      for (qint32 row = 0; row < table->rowCount(); row++) {
        for (qint32 column = 0; column < table->columnCount(); column++) {
          if (QTableWidgetItem* item = table->item(row, column); nullptr != item) {
            take(item->text(), [item](const QString& s) { item->setText(s); });
          }
        }
      }
    } else if (QPlainTextEdit* plain = qobject_cast<QPlainTextEdit*>(w); nullptr != plain) {
      rewriteDocument(plain, plain->document(), list);
    } else if (QTextEdit* rich = qobject_cast<QTextEdit*>(w); nullptr != rich) {
      rewriteDocument(rich, rich->document(), list);
    } else if (QMenu* menu = qobject_cast<QMenu*>(w); nullptr != menu) {
      const QList<QAction*>& actions = menu->actions();
      for (QAction* action : actions) {
        const QPointer<QAction> kept(action);
        take(action->text(), [kept](const QString& s) {
          if (kept) {
            kept->setText(s);
          }
        });
      }
    }
  }
}

CShotPrivacy::~CShotPrivacy() {
  for (qsizetype i = undo.size() - 1; i >= 0; i--) {
    undo[i]();
  }
}

QList<CShotPrivacy::rule_t> CShotPrivacy::rules() {
  // Literal places first, the most specific first: they match a directory name with a space, which kLead cannot.
  // A scratch directory holds the copied database and the configuration.
  QList<rule_t> list;
  const QString& cacheRoot = CMapDraw::getCacheRoot();
  if (!cacheRoot.isEmpty()) {
    list << rule_t{QRegularExpression(anySeparator(cacheRoot) + kEnd), kCache};
  }
  list << rule_t{QRegularExpression(anySeparator(QDir::tempPath()) + R"([/\\])" + kPart + kEnd), kHome};
  list << rule_t{QRegularExpression(anySeparator(QDir::homePath()) + kEnd), kHome};
  // The fixture and the cache of the checkout, wherever it lies.
  list << rule_t{QRegularExpression(kLead + R"([/\\]doc[/\\]shots[/\\]fixtures[/\\])" + kPart + kEnd), kHome};
  list << rule_t{QRegularExpression(kLead + R"([/\\]doc[/\\]shots[/\\]_cache)" + kEnd), kCache};
  return list;
}

QString CShotPrivacy::apply(const QList<rule_t>& list, const QString& text) {
  QString out = text;
  for (const rule_t& rule : list) {
    out.replace(rule.pattern, rule.replacement);
  }
  return out;
}

void CShotPrivacy::rewriteDocument(QWidget* edit, QTextDocument* doc, const QList<rule_t>& list) {
  if (apply(list, doc->toRawText()) == doc->toRawText()) {
    return;
  }

  // Edited in place, not set anew: setHtml() resets the scroll position, the cursor and the undo history.
  struct replaced_t {
    qint32 position;
    qint32 length;
    QString before;
  };
  QList<replaced_t> replaced;
  const bool modified = doc->isModified();
  {
    const QSignalBlocker blockEdit(edit);
    const QSignalBlocker blockDoc(doc);
    QTextCursor group(doc);
    group.beginEditBlock();
    for (const rule_t& rule : list) {
      QTextCursor found = doc->find(rule.pattern);
      while (!found.isNull()) {
        const QString& before = found.selectedText();
        const QString& after = QString(before).replace(rule.pattern, rule.replacement);
        const qint32 position = found.selectionStart();
        found.insertText(after);
        replaced << replaced_t{position, qint32(after.size()), before};
        found = doc->find(rule.pattern, position + after.size());
      }
    }
    group.endEditBlock();
  }

  const QPointer<QWidget> alive(edit);
  undo << [alive, doc, replaced, modified]() {
    if (!alive) {
      return;
    }
    const QSignalBlocker blockEdit(alive.data());
    const QSignalBlocker blockDoc(doc);
    if (doc->isUndoRedoEnabled()) {
      doc->undo();
      // Or a redo puts the rewritten paths into the document.
      doc->clearUndoRedoStacks(QTextDocument::RedoStack);
    } else {
      for (qsizetype i = replaced.size() - 1; i >= 0; i--) {
        QTextCursor cursor(doc);
        cursor.setPosition(replaced[i].position);
        cursor.setPosition(replaced[i].position + replaced[i].length, QTextCursor::KeepAnchor);
        cursor.insertText(replaced[i].before);
      }
    }
    doc->setModified(modified);
  };
}

QString CShotPrivacy::neutral(const QString& text) { return apply(rules(), text); }
