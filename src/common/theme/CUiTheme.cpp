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

#include "theme/CUiTheme.h"

#include <QApplication>
#include <QEvent>
#include <QLabel>
#include <QWidget>

namespace {
bool forceLight = false;

/** @brief Role a markLabel() label was marked with, so the sweep can resolve it again. */
constexpr char kThemeRoleProperty[] = "qmsThemeRole";

/**
   @brief Re-resolve the two things a QLabel bakes in, in @p root and every label below it.

   - A markLabel() role: the style sheet carries the resolved colour, so it is applied again.
   - An anchor colour: Qt resolves unstyled document text against the palette when it draws, but
     an anchor gets an explicit colour baked into its char format at parse time and QLabel does
     not re-parse. Re-applying the text is the repair, and QLabel ignores setText() with an
     unchanged string, hence the clear-and-restore.
 */
void refreshThemedLabels(QWidget* root) {
  QList<QLabel*> labels = root->findChildren<QLabel*>();
  if (QLabel* const asLabel = qobject_cast<QLabel*>(root); asLabel != nullptr) {
    labels << asLabel;  // findChildren() does not include the root itself
  }
  for (QLabel* const label : labels) {
    const QVariant role = label->property(kThemeRoleProperty);
    if (role.isValid()) {
      CUiTheme::markLabel(label, static_cast<CUiTheme::Role>(role.toInt()));
    }

    const QString text = label->text();
    if (!text.contains("<a ", Qt::CaseInsensitive)) {
      continue;
    }
    label->setText(QString());
    label->setText(text);
  }
}

/**
   @brief Repairs the one thing a widget cannot repair for itself: a QLabel's baked link colour.

   Everything else a scheme change leaves stale belongs in that widget's own changeEvent() - Qt
   delivers PaletteChange to every widget at every depth. This exists because a plain QLabel has
   no such handler and nothing regenerates its markup.

   A filter, because changeEvent() never sees ApplicationPaletteChange; that one arrives once, on
   the application, which is why the sweep is queued rather than run per widget.
 */
class CThemeRefresher : public QObject {
 public:
  explicit CThemeRefresher(QObject* parent) : QObject(parent) {}

 protected:
  bool eventFilter(QObject* watched, QEvent* event) override {
    if (event->type() == QEvent::ApplicationPaletteChange && !pending) {
      pending = true;
      QMetaObject::invokeMethod(this, [this]() { refresh(); }, Qt::QueuedConnection);
    }
    return QObject::eventFilter(watched, event);
  }

 private:
  void refresh() {
    pending = false;
    const QList<QWidget*> windows = QApplication::topLevelWidgets();
    for (QWidget* const window : windows) {
      refreshThemedLabels(window);
    }
  }

  bool pending = false;
};
}  // namespace

CUiTheme::CForceLight::CForceLight(bool active) : active(active) {
  if (active) {
    forceLight = true;
  }
}

CUiTheme::CForceLight::~CForceLight() {
  if (active) {
    forceLight = false;
  }
}

bool CUiTheme::isDark() { return !forceLight && paletteIsDark(); }

QColor CUiTheme::foreground(Role role) {
  const bool dark = isDark();
  switch (role) {
    case Role::eNeutral:
      return dark ? QColor(0xe0, 0xe0, 0xe0) : QColor(0x00, 0x00, 0x00);
    case Role::eOk:
      return dark ? QColor(0xa3, 0xd9, 0xb1) : QColor(0x0f, 0x51, 0x32);
    case Role::eWarn:
      return dark ? QColor(0xff, 0xd8, 0x6b) : QColor(0x66, 0x4d, 0x03);
    case Role::eError:
      return dark ? QColor(0xff, 0x8a, 0x8a) : QColor(0x84, 0x20, 0x29);
    case Role::eInfo:
      return dark ? QColor(0x66, 0xaa, 0xff) : QColor(0x08, 0x42, 0x98);
    case Role::eCode:
      return dark ? QColor(0xe6, 0xe2, 0xc8) : QColor(0x00, 0x00, 0x00);
  }
  return QColor();
}

QColor CUiTheme::background(Role role) {
  const bool dark = isDark();
  switch (role) {
    case Role::eNeutral:
      return dark ? QColor(0x4a, 0x4a, 0x4a) : QColor(0xe0, 0xe0, 0xe0);
    case Role::eOk:
      return dark ? QColor(0x1e, 0x3a, 0x24) : QColor(0xd4, 0xed, 0xda);
    case Role::eWarn:
      return dark ? QColor(0x3d, 0x34, 0x18) : QColor(0xff, 0xf3, 0xcd);
    case Role::eError:
      return dark ? QColor(0x3d, 0x1f, 0x22) : QColor(0xf8, 0xd7, 0xda);
    case Role::eInfo:
      return dark ? QColor(0x1c, 0x2a, 0x3d) : QColor(0xcf, 0xe2, 0xff);
    case Role::eCode:
      // light arm is the lemon the help pages ship with, so light mode renders unchanged
      return dark ? QColor(0x3f, 0x3d, 0x2a) : QColor(0xff, 0xfa, 0xcd);
  }
  return QColor();
}

QString CUiTheme::css(Role role) {
  return QString("color:%1; background-color:%2").arg(foreground(role).name(), background(role).name());
}

QString CUiTheme::cssForeground(Role role) { return QString("color:%1").arg(foreground(role).name()); }

QString CUiTheme::span(Role role, const QString& text) {
  return QString("<span style=\"%1\">%2</span>").arg(cssForeground(role), text);
}

QString CUiTheme::spanBold(Role role, const QString& text) { return span(role, "<b>" + text + "</b>"); }

void CUiTheme::markLabel(QLabel* label, Role role) {
  // The style sheet resolves the colour once. Record the role so the sweep can redo this.
  label->setProperty(kThemeRoleProperty, static_cast<int>(role));
  label->setStyleSheet(cssForeground(role) + "; font-weight:bold;");
}

bool CUiTheme::isPaletteChange(const QEvent* event) {
  const QEvent::Type type = event->type();
  return type == QEvent::ApplicationPaletteChange || type == QEvent::PaletteChange;
}

void CUiTheme::installThemeRefresh() {
  static bool installed = false;
  if (installed) {
    return;
  }
  installed = true;
  qApp->installEventFilter(new CThemeRefresher(qApp));
}
