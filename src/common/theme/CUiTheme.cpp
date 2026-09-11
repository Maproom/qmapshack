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
#include <QStyleHints>
#include <QWidget>

namespace {
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

bool CUiTheme::forceLight = false;

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

void CUiTheme::pinColorScheme(bool dark) {
  QGuiApplication::styleHints()->setColorScheme(dark ? Qt::ColorScheme::Dark : Qt::ColorScheme::Light);

  // Fusion's palette; QStyle::standardPalette() would ask the platform theme for the scheme.
  const QColor windowText = dark ? QColor(240, 240, 240) : QColor(Qt::black);
  const QColor button = dark ? QColor(50, 50, 50) : QColor(239, 239, 239);
  const QColor light = button.lighter(150);
  const QColor mid = button.darker(130);
  const QColor shade = button.darker(150);
  const QColor base = dark ? button.darker(140) : QColor(Qt::white);
  const QColor highlight(48, 140, 198);
  const QColor disabledText = dark ? QColor(130, 130, 130) : QColor(190, 190, 190);
  const QColor shadow = shade.darker(135);
  QColor placeholder = windowText;
  placeholder.setAlpha(128);

  // The seven-colour ctor takes Window, which paletteIsDark() reads, from `button`.
  QPalette palette(windowText, button, light, shade, mid, windowText, base);
  palette.setBrush(QPalette::Midlight, mid.lighter(110));
  palette.setBrush(QPalette::Button, button);
  palette.setBrush(QPalette::Shadow, shadow);
  palette.setBrush(QPalette::HighlightedText, dark ? windowText : QColor(Qt::white));
  // The seven-colour ctor takes BrightText from `light`, too dark on a dark window.
  palette.setBrush(QPalette::BrightText, QColor(Qt::white));
  palette.setBrush(QPalette::PlaceholderText, placeholder);

  palette.setBrush(QPalette::Disabled, QPalette::Text, disabledText);
  palette.setBrush(QPalette::Disabled, QPalette::WindowText, disabledText);
  palette.setBrush(QPalette::Disabled, QPalette::ButtonText, disabledText);
  palette.setBrush(QPalette::Disabled, QPalette::Base, button);
  // Light arm: Fusion's literal. Dark arm derived, the literal is a near-white edge there.
  palette.setBrush(QPalette::Disabled, QPalette::Dark, dark ? shade.lighter(120) : QColor(209, 209, 209).darker(110));
  palette.setBrush(QPalette::Disabled, QPalette::Shadow, shadow.lighter(150));

  const QColor disabledHighlight(145, 145, 145);
  palette.setBrush(QPalette::Active, QPalette::Highlight, highlight);
  palette.setBrush(QPalette::Inactive, QPalette::Highlight, highlight);
  palette.setBrush(QPalette::Disabled, QPalette::Highlight, disabledHighlight);
  palette.setBrush(QPalette::Active, QPalette::Accent, highlight);
  palette.setBrush(QPalette::Inactive, QPalette::Accent, highlight);
  palette.setBrush(QPalette::Disabled, QPalette::Accent, disabledHighlight);

  // Qt::blue, the default, is unreadable on a dark window.
  if (dark) {
    palette.setBrush(QPalette::Link, highlight);
  }

  QApplication::setPalette(palette);
}

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
