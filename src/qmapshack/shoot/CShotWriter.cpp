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

#include "shoot/CShotWriter.h"

#include <QAbstractAnimation>
#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QElapsedTimer>
#include <QEventLoop>
#include <QFileInfo>
#include <QLayout>
#include <QSaveFile>
#include <QTimer>
#include <QWidget>

#include "canvas/CCanvas.h"
#include "shoot/CShotFiles.h"

namespace {
constexpr qint32 kPollMs = 250;
constexpr qint32 kTimeoutMs = 20000;

bool holdsCanvas(QWidget* w) { return nullptr != qobject_cast<CCanvas*>(w) || nullptr != w->findChild<CCanvas*>(); }

/** @return true when no map in @p w is drawing; one on a page that is not current is not in the picture */
bool drawComplete(QWidget* w) {
  QList<CCanvas*> canvases = w->findChildren<CCanvas*>();
  if (CCanvas* canvas = qobject_cast<CCanvas*>(w); nullptr != canvas) {
    canvases << canvas;
  }
  for (const CCanvas* canvas : std::as_const(canvases)) {
    if ((canvas == w || canvas->isVisibleTo(w)) && !canvas->isDrawComplete()) {
      return false;
    }
  }
  return true;
}
}  // namespace

CShotWriter::CShotWriter(const QString& outDir, const QString& lang) : outDir(outDir), lang(lang) {}

void CShotWriter::settle(QWidget* w) {
  w->ensurePolished();

  // Animations run on wall time; event loop passes do not end them.
  const QList<QAbstractAnimation*>& animations = w->window()->findChildren<QAbstractAnimation*>();
  for (QAbstractAnimation* animation : animations) {
    const qint32 total = animation->totalDuration();
    if (QAbstractAnimation::Running == animation->state() && total >= 0) {
      animation->setCurrentTime(QAbstractAnimation::Forward == animation->direction() ? total : 0);
    }
  }

  // A layout activates from a posted LayoutRequest.
  for (qint32 i = 0; i < 3; i++) {
    if (nullptr != w->layout()) {
      w->layout()->activate();
    }
    QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
  }
}

bool CShotWriter::settleStable(QWidget* w) {
  QList<CCanvas*> candidates = w->findChildren<CCanvas*>();
  if (CCanvas* canvas = qobject_cast<CCanvas*>(w); nullptr != canvas) {
    candidates << canvas;
  }

  QList<CCanvas*> canvases;
  for (CCanvas* canvas : std::as_const(candidates)) {
    // A canvas on a page that is not current is not in the picture.
    if (canvas != w && !canvas->isVisibleTo(w)) {
      continue;
    }
    // A hidden canvas never paints, so its redraw never completes.
    if (!canvas->isVisible()) {
      qWarning() << "shoot: a map in the picture is hidden and paints nothing";
      return false;
    }
    canvases << canvas;
  }

  QElapsedTimer timer;
  timer.start();
  QImage last;
  while (!timer.hasExpired(kTimeoutMs)) {
    // processEvents() returns once nothing is pending; tiles need a real wait.
    QEventLoop loop;
    QTimer::singleShot(kPollMs, &loop, &QEventLoop::quit);
    loop.exec(QEventLoop::ExcludeUserInputEvents);

    // Before rendering: a draw finishing mid-render swaps the buffer after it was read.
    bool complete = true;
    for (const CCanvas* canvas : std::as_const(canvases)) {
      complete = complete && canvas->isDrawComplete();
    }

    // Rendering paints the canvas, which starts an outstanding redraw.
    const QImage& image = renderAtDpr1(w);

    if (complete && image == last) {
      qint32 failed = 0;
      for (const CCanvas* canvas : std::as_const(canvases)) {
        failed += canvas->failedTiles();
      }
      if (0 != failed) {
        qWarning() << "shoot:" << failed << "tiles in the picture are holes; the map is incomplete";
        return false;
      }
      return true;
    }
    last = image;
  }

  qWarning() << "shoot: the map did not complete within" << kTimeoutMs << "ms";
  return false;
}

QImage CShotWriter::render(QWidget* w, const QSize& size) {
  settle(w);
  // A child keeps its layout size; resizing it would outlive the shot.
  if (w->isWindow()) {
    w->resize(size.isValid() ? size : w->size().expandedTo(w->sizeHint()));
    settle(w);
  }

  if (holdsCanvas(w) && !settleStable(w)) {
    return QImage();
  }
  return renderAtDpr1(w);
}

QHash<const QWidget*, QImage> CShotWriter::renderAll(const QList<QWidget*>& parts) {
  // settleStable() takes two passes of its own; a window's maps are those of every part in it.
  QHash<const QWidget*, bool> mapsComplete;
  QHash<const QWidget*, QImage> pictures;
  for (QWidget* w : parts) {
    settle(w);
    if (holdsCanvas(w)) {
      QWidget* window = w->window();
      if (!mapsComplete.contains(window)) {
        mapsComplete.insert(window, settleStable(window));
      }
      // Rendering a part paints its map, which may start a redraw the next part would catch half done.
      if (!mapsComplete.value(window) || (!drawComplete(w) && !settleStable(w))) {
        pictures.insert(w, QImage());
        continue;
      }
    }
    pictures.insert(w, renderAtDpr1(w));
  }
  return pictures;
}

QString CShotWriter::write(const QImage& image, const QString& id) const {
  if (image.isNull()) {
    qWarning() << "shoot:" << id << "has no picture";
    return QString();
  }

  if (!CShotFiles::staysInside(id)) {
    qWarning() << "shoot:" << id << "names a place outside" << outDir;
    return QString();
  }
  const QString& suffix = ("en" == lang) ? QString(".png") : ("." + lang + ".png");
  const QString& path = QDir(outDir).absoluteFilePath(id + suffix);
  // QSaveFile: publish may move the file at any moment and must never find half a picture.
  QSaveFile out(path);
  // Twice: publish removes a directory it emptied, possibly between mkpath() and open().
  bool opened = false;
  for (qint32 attempt = 0; attempt < 2 && !opened; attempt++) {
    opened = QDir().mkpath(QFileInfo(path).absolutePath()) && out.open(QIODevice::WriteOnly);
  }
  if (!opened || !image.save(&out, "PNG") || !out.commit()) {
    qWarning() << "shoot:" << id << "cannot be written to" << path;
    return QString();
  }

  qDebug() << "shoot:" << id << image.size() << path;
  return path;
}

QImage CShotWriter::renderAtDpr1(QWidget* w) {
  QImage image(w->size(), QImage::Format_ARGB32_Premultiplied);
  image.fill(Qt::transparent);
  w->render(&image);

  // Drop the alpha channel of an opaque picture.
  for (qint32 y = 0; y < image.height(); y++) {
    const QRgb* line = reinterpret_cast<const QRgb*>(image.constScanLine(y));
    for (qint32 x = 0; x < image.width(); x++) {
      if (255 != qAlpha(line[x])) {
        return image;
      }
    }
  }
  return image.convertToFormat(QImage::Format_RGB32);
}
