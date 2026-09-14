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

#include "shoot/CShotPage.h"

#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QPointer>
#include <QRect>
#include <QRegularExpression>
#include <QSet>
#include <QSize>
#include <QVariant>
#include <QWidget>
#include <cmath>

#include "CMainWindow.h"
#include "canvas/CCanvas.h"
#include "gis/proj_x.h"
#include "shoot/CShotContext.h"
#include "shoot/CShotWriter.h"

namespace {
const QSet<QString> kShotKeys = {"exposure", "id", "note", "rect", "scenario", "set", "size", "widget", "window"};

/** Coordinates of two views closer than this are the same place [°]. */
constexpr qreal kViewEpsilon = 1e-9;

/** @return the direct children of @p parent of exactly this class, in construction order */
QList<QWidget*> childrenOfClass(const QWidget* parent, const QString& className) {
  QList<QWidget*> matches;
  const QList<QWidget*>& children = parent->findChildren<QWidget*>(Qt::FindDirectChildrenOnly);
  for (QWidget* child : children) {
    if (QString::fromLatin1(child->metaObject()->className()) == className) {
      matches << child;
    }
  }
  return matches;
}

/** @return true for a name that parses back as a single address part */
bool isAddressName(const QString& name) { return !name.isEmpty() && !name.contains('/') && !name.contains('#'); }

/** @return the active popup, else the active modal dialog, else @p main */
QWidget* topmost(QWidget* main) {
  if (QWidget* popup = QApplication::activePopupWidget(); nullptr != popup) {
    return popup;
  }
  if (QWidget* modal = QApplication::activeModalWidget(); nullptr != modal) {
    return modal;
  }
  return main;
}

/** @return false when the shot has a `size` that is not [width, height] */
bool readSize(const QJsonObject& shot, QSize& size) {
  size = QSize();
  if (!shot.contains("size")) {
    return true;
  }
  const QJsonArray& value = shot["size"].toArray();
  size = QSize(value.at(0).toInt(), value.at(1).toInt());
  return 2 == value.size() && !size.isEmpty();
}

/** @return false when the shot has a `rect` that is not [x, y, width, height] */
bool readRect(const QJsonObject& shot, QRect& rect) {
  rect = QRect();
  if (!shot.contains("rect")) {
    return true;
  }
  const QJsonArray& value = shot["rect"].toArray();
  rect = QRect(value.at(0).toInt(), value.at(1).toInt(), value.at(2).toInt(), value.at(3).toInt());
  return 4 == value.size() && !rect.isEmpty();
}
}  // namespace

std::optional<QString> CShotPage::addressOf(const QWidget* root, const QWidget* widget) {
  if (nullptr == root || nullptr == widget) {
    return std::nullopt;
  }
  if (widget == root) {
    return QString();
  }

  const QString& name = widget->objectName();
  if (isAddressName(name) && root->findChild<QWidget*>(name) == widget) {
    return name;
  }

  const QWidget* parent = widget->parentWidget();
  if (nullptr == parent) {
    return std::nullopt;
  }
  const std::optional<QString>& prefix = addressOf(root, parent);
  if (!prefix.has_value()) {
    return std::nullopt;
  }

  const QString& className = QString::fromLatin1(widget->metaObject()->className());
  const qsizetype index = childrenOfClass(parent, className).indexOf(const_cast<QWidget*>(widget));
  const QString& part = QString("%1#%2").arg(className).arg(index);
  return prefix->isEmpty() ? part : (*prefix + "/" + part);
}

QWidget* CShotPage::resolve(QWidget* root, const QString& address) {
  QWidget* current = root;
  if (nullptr == root || address.isEmpty()) {
    return current;
  }

  const QStringList& parts = address.split('/');
  for (const QString& part : parts) {
    if (nullptr == current) {
      break;
    }
    if (part.contains('#')) {
      bool ok = false;
      const qint32 index = part.section('#', 1).toInt(&ok);
      current = ok ? childrenOfClass(current, part.section('#', 0, 0)).value(index, nullptr) : nullptr;
    } else {
      current = current->findChild<QWidget*>(part);
    }
  }
  return current;
}

bool CShotPage::driveProperty(QObject* target, const QString& property, const QVariant& value) {
  const QByteArray& name = property.toLatin1();
  // setProperty() on a name the class has not got adds a dynamic property and answers false.
  if (nullptr == target || target->metaObject()->indexOfProperty(name.constData()) < 0 ||
      !target->setProperty(name.constData(), value)) {
    return false;
  }
  // JSON has one number type: 1.0 is the same currentIndex as 1.
  const QVariant& now = target->property(name.constData());
  QVariant wanted = value;
  return wanted.convert(now.metaType()) && wanted == now;
}

QJsonObject CShotPage::viewOf(const CCanvas* canvas) {
  const QPointF& focus = canvas->getPosFocus() * RAD_TO_DEG;
  return QJsonObject{{"do", "view"}, {"lat", focus.y()}, {"lon", focus.x()}, {"zoom", canvas->getZoomIndex()}};
}

bool CShotPage::applyView(CCanvas* canvas, const QJsonObject& view) {
  if (nullptr == canvas || !view["lat"].isDouble() || !view["lon"].isDouble() || !view["zoom"].isDouble()) {
    return false;
  }

  canvas->zoom(view["zoom"].toInt());
  canvas->moveTo(QPointF(view["lon"].toDouble(), view["lat"].toDouble()) * DEG_TO_RAD);

  const QJsonObject& now = viewOf(canvas);
  return now["zoom"].toInt() == view["zoom"].toInt() &&
         std::abs(now["lat"].toDouble() - view["lat"].toDouble()) < kViewEpsilon &&
         std::abs(now["lon"].toDouble() - view["lon"].toDouble()) < kViewEpsilon;
}

qint32 CShotPage::shootOne(const QJsonObject& shot, CShotContext& ctx) {
  const QString& id = shot["id"].toString();
  if (id.isEmpty()) {
    qWarning() << "shoot: a shot has no id:" << shot;
    return 1;
  }

  // A misspelt key would silently be ignored.
  qint32 failures = 0;
  const QStringList& keys = shot.keys();
  for (const QString& key : keys) {
    if (!kShotKeys.contains(key)) {
      qWarning() << "shoot:" << id << "has an unknown key" << key;
      failures++;
    }
  }

  QSize size;
  if (!readSize(shot, size)) {
    qWarning() << "shoot:" << id << "has a size that is not [width, height]:" << shot["size"];
    failures++;
  }
  QRect rect;
  if (!readRect(shot, rect)) {
    qWarning() << "shoot:" << id << "has a rect that is not [x, y, width, height]:" << shot["rect"];
    failures++;
  }
  if (shot.contains("set") && !shot["set"].isObject()) {
    qWarning() << "shoot:" << id << "has a set that is not an object:" << shot["set"];
    failures++;
  }
  if (shot.contains("exposure")) {
    qWarning() << "shoot:" << id << "wants the exposure" << shot["exposure"].toString()
               << "and this build has no exposure catalog";
    failures++;
  }
  if (shot.contains("scenario")) {
    qWarning() << "shoot:" << id << "wants the scenario" << shot["scenario"].toString()
               << "and this build cannot replay one";
    failures++;
  }
  if (0 != failures) {
    return failures;
  }

  QWidget* main = CMainWindow::isNull() ? nullptr : &CMainWindow::self();
  const QString& address = shot["widget"].toString();
  QWidget* widget = address.isEmpty() ? topmost(main) : resolve(main, address);
  if (nullptr == widget) {
    qWarning() << "shoot:" << id << "finds no widget at" << address;
    return 1;
  }

  // Guards against photographing the window behind a dialog that failed to open.
  const QString& window = shot["window"].toString();
  const QString& className = QString::fromLatin1(widget->metaObject()->className());
  if (!window.isEmpty() && window != className) {
    qWarning() << "shoot:" << id << "expects" << window << "on top and finds" << className;
    return 1;
  }

  // Main window layout differs per platform, so such a shot must fix the window size.
  const bool sizedByWindow = (widget == main) || (!widget->isWindow() && widget->window() == main);
  if (sizedByWindow) {
    if (!size.isValid()) {
      qWarning() << "shoot:" << id << "is sized by the main window and has no size";
      return 1;
    }
    // Before any `set`: the state depends on the window size.
    main->resize(size);
    CShotWriter::settle(main);
    if (main->size() != size) {
      qWarning() << "shoot:" << id << "asks for a main window of" << size << "and it is" << main->size();
      return 1;
    }
  } else if (!widget->isWindow() && size.isValid()) {
    // A size applies to a window only.
    qWarning() << "shoot:" << id << "has a size and is laid out by" << widget->window()->metaObject()->className();
    return 1;
  }

  // Restored after the picture, so shots stay independent.
  struct restore_t {
    QPointer<QObject> target;
    QString property;
    QVariant value;
  };
  QList<restore_t> restore;

  const QJsonObject& set = shot["set"].toObject();
  for (auto it = set.constBegin(); it != set.constEnd(); ++it) {
    // `child.property` addresses below the photographed widget; a bare property is its own.
    const qsizetype dot = it.key().lastIndexOf('.');
    const QString& target = (dot < 0) ? QString() : it.key().left(dot);
    const QString& property = it.key().mid(dot + 1);
    const QByteArray& name = property.toLatin1();
    QObject* driven = resolve(widget, target);
    if (nullptr == driven) {
      qWarning() << "shoot:" << id << "has no" << target << "to set" << property << "on";
      failures++;
    } else if (driven->metaObject()->indexOfProperty(name.constData()) < 0) {
      qWarning() << "shoot:" << id << "has no property" << property << "on" << driven->metaObject()->className();
      failures++;
    } else {
      // Newest first, read before driving: a failed set may still have changed the value.
      restore.prepend({driven, property, driven->property(name.constData())});
      if (!driveProperty(driven, property, it.value().toVariant())) {
        qWarning() << "shoot:" << id << "cannot set" << it.key() << "to" << it.value().toVariant() << "- it is"
                   << driven->property(name.constData());
        failures++;
      }
    }
  }
  if (0 == failures) {
    CShotWriter::settle(widget);

    ctx.begin(id, rect);
    if (!ctx.shot(widget, widget->isWindow() ? size : QSize())) {
      failures++;
    }
  }

  for (const restore_t& before : std::as_const(restore)) {
    if (!driveProperty(before.target, before.property, before.value)) {
      qWarning() << "shoot:" << id << "cannot put" << before.property << "back to" << before.value;
      failures++;
    }
  }
  return failures;
}

qint32 CShotPage::run(const QString& file, CShotContext& ctx, const QString& only, const QString& scenario) {
  QFile in(file);
  if (!in.open(QIODevice::ReadOnly)) {
    qWarning() << "shoot: cannot read" << file;
    return 1;
  }
  QJsonParseError error;
  const QJsonDocument& document = QJsonDocument::fromJson(in.readAll(), &error);
  if (QJsonParseError::NoError != error.error || !document.object()["shots"].isArray()) {
    qWarning() << "shoot:" << file << "is not a shot file:" << error.errorString() << "at offset" << error.offset;
    return 1;
  }

  // Not a path glob: `*` crosses the `/` between page and name.
  const QRegularExpression pattern(QRegularExpression::wildcardToRegularExpression(
      only.isEmpty() ? QString("*") : only, QRegularExpression::NonPathWildcardConversion));

  qint32 failures = 0;
  qint32 taken = 0;
  QSet<QString> ids;
  QList<QJsonObject> toTake;
  const QJsonArray& shots = document.object()["shots"].toArray();
  for (const QJsonValue& value : shots) {
    const QJsonObject& shot = value.toObject();
    const QString& id = shot["id"].toString();

    // Both would write the same file.
    if (!id.isEmpty() && ids.contains(id)) {
      qWarning() << "shoot:" << id << "is in" << file << "more than once";
      failures++;
      continue;
    }
    ids << id;

    if (!pattern.match(id).hasMatch()) {
      continue;
    }
    if (!scenario.isEmpty()) {
      const QString& own = shot["scenario"].toString();
      if ((kBaseScenario == scenario) ? !own.isEmpty() : (own != scenario)) {
        continue;
      }
    }

    taken++;
    failures += shootOne(shot, ctx);
  }

  if (0 == taken) {
    qWarning() << "shoot: no shot in" << file << "matches" << only << scenario;
    failures++;
  }
  qDebug() << "shoot:" << taken << "shots from" << file << "," << failures << "failures";
  return failures;
}
