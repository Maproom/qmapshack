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

#include "CSvgtIcon.h"

#include <QBuffer>
#include <QDebug>
#include <QEvent>
#include <QFile>
#include <QGuiApplication>
#include <QIcon>
#include <QImage>
#include <QScreen>
#include <QSvgWidget>

#include "CSvgtIconEngine.h"

namespace {

// The source path, kept on the widget itself. Re-theming needs the ORIGINAL document:
// the widget holds the already-recoloured bytes, and recolouring those again would drift.
const char* kPathProperty = "svgtSourcePath";

void apply(QSvgWidget* widget) {
  const QString path = widget->property(kPathProperty).toString();
  QFile f(path);
  if (!f.open(QIODevice::ReadOnly)) {
    // Loudly: a wrong path leaves the widget blank, which reads as "no icon here"
    // rather than as a bug. Note ".svgt" is an alias created by the .qrc -- it
    // resolves under ":/icons", never as a file in src/icons.
    qWarning() << "CSvgtIcon: cannot open" << path;
    return;
  }
  // QIcon::Normal: these are static illustrations in a dialog, never disabled or selected.
  using Role = CSvgtIconEngine::Role;
  widget->load(CSvgtIconEngine::recolored(
      f.readAll(), CSvgtIconEngine::roleColor(Role::Ink), CSvgtIconEngine::roleColor(Role::Paper),
      CSvgtIconEngine::roleColor(Role::Lead), CSvgtIconEngine::roleColor(Role::Mark)));
}

/**
   Re-themes a widget when the palette changes, so a live theme switch does not leave
   stale colours behind.

   Stateless and shared by every themed widget: the path lives on the widget, so one
   instance can serve all of them and there is nothing to keep in sync or clean up.
   Deliberately not a QObject subclass with members -- that would need Q_OBJECT and moc
   for a class that has no signals and no properties of its own.
 */
class CRethemer : public QObject {
 protected:
  bool eventFilter(QObject* watched, QEvent* event) override {
    const QEvent::Type type = event->type();
    if (type == QEvent::ApplicationPaletteChange || type == QEvent::PaletteChange) {
      if (QSvgWidget* widget = qobject_cast<QSvgWidget*>(watched)) {
        apply(widget);
      }
    }
    return QObject::eventFilter(watched, event);
  }
};

CRethemer* rethemer() {
  static CRethemer instance;
  return &instance;
}

}  // namespace

void CSvgtIcon::load(QSvgWidget* widget, const QString& path) {
  if (widget == nullptr) {
    return;
  }

  // Installing the same filter object twice is a no-op in Qt, so a reload is safe.
  const bool first = widget->property(kPathProperty).isNull();
  widget->setProperty(kPathProperty, path);
  if (first) {
    widget->installEventFilter(rethemer());
  }
  apply(widget);
}

QString CSvgtIcon::htmlImageSrc(const QString& resource, int px) {
  // Render at the screen's device pixel ratio so the baked PNG stays crisp on HiDPI; the
  // <img> box is kept at px logical by the caller's width/height.
  qreal dpr = 1.0;
  if (const QScreen* screen = QGuiApplication::primaryScreen()) {
    dpr = screen->devicePixelRatio();
  }
  const QImage img = QIcon(resource).pixmap(QSize(px, px), dpr).toImage();

  QByteArray png;
  QBuffer buf(&png);
  buf.open(QIODevice::WriteOnly);
  img.save(&buf, "PNG");
  return QStringLiteral("data:image/png;base64,") + QString::fromLatin1(png.toBase64());
}
