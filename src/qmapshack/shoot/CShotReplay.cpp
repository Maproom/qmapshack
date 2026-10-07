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

#include "shoot/CShotReplay.h"

#include <QApplication>
#include <QDebug>
#include <QDialog>
#include <QEventLoop>
#include <QJsonDocument>
#include <QPointer>
#include <QThread>
#include <QTimer>
#include <QWidget>
#include <cstdlib>
#include <memory>

#include "CMainWindow.h"
#include "canvas/CCanvas.h"
#include "gis/CGisListWks.h"
#include "gis/CGisWorkspace.h"
#include "gis/trk/CGisItemTrk.h"
#include "plot/IPlot.h"
#include "shoot/CShotAddress.h"
#include "shoot/CShotContext.h"
#include "shoot/CShotHandlers.h"
#include "shoot/CShotPage.h"
#include "shoot/CShotWriter.h"
#include "units/IUnit.h"

namespace {
QString compact(const QJsonObject& step) {
  return QString::fromUtf8(QJsonDocument(step).toJson(QJsonDocument::Compact));
}

/** @brief Close @p active's windows one by one; one that stays open ends it. */
void closeAll(QWidget* (*active)()) {
  QPointer<QWidget> widget = active();
  while (!widget.isNull()) {
    // A dialog's closeEvent() may refuse; reject() ends its exec() regardless.
    if (QDialog* dialog = qobject_cast<QDialog*>(widget); nullptr != dialog) {
      dialog->reject();
    } else {
      widget->close();
    }
    QWidget* next = active();
    if (next == widget) {
      qWarning() << "shoot: the replay cannot close" << widget->metaObject()->className();
      return;
    }
    widget = next;
  }
}

/** @brief Close what runs an event loop on top of the application, so the loops below can return. */
void closeBlockingWindows() {
  closeAll(&QApplication::activePopupWidget);
  closeAll(&QApplication::activeModalWidget);
}

/** @return the popup, else the modal dialog, that runs an event loop on top; nullptr when none does */
QWidget* blocker() {
  QWidget* popup = QApplication::activePopupWidget();
  return (nullptr != popup) ? popup : QApplication::activeModalWidget();
}

/** A step that has not returned: which one, the event loop level and what was open when it started. */
struct running_t {
  qsizetype index = 0;
  qint32 loopLevel = 0;
  QPointer<QWidget> blocker;
};

/** One perform(), kept alive for a pump still queued when it ended. */
struct queue_t {
  QWidget* root = nullptr;
  QList<QJsonObject> steps;
  std::function<qint32()> whenReady;
  qsizetype next = 0;
  qint32 failures = 0;
  /** The steps that have not returned, innermost last. */
  QList<running_t> running;
  bool done = false;
  std::function<void()> pump;
  QEventLoop loop;
};
}  // namespace

qint32 CShotReplay::perform(QWidget* root, const QList<QJsonObject>& steps, const std::function<qint32()>& whenReady) {
  const std::shared_ptr<queue_t> queue = std::make_shared<queue_t>();
  queue->root = root;
  queue->steps = steps;
  queue->whenReady = whenReady;

  // Weak, or the state would own the function that owns the state.
  const std::weak_ptr<queue_t> weak = queue;
  queue->pump = [weak]() {
    const std::shared_ptr<queue_t> q = weak.lock();
    if (nullptr == q || q->done) {
      return;
    }
    // Only a step waiting in the exec() of a popup or dialog it opened may have the next one run inside it; one that
    // spins events itself - a synthesized click, settle() - has not seen its own outcome yet.
    // A popup open before the step is not one it waits in: its own waits run a loop above it too.
    const bool insideExec = !q->running.isEmpty() &&
                            QThread::currentThread()->loopLevel() > q->running.last().loopLevel &&
                            nullptr != blocker() && blocker() != q->running.last().blocker;
    if (!q->running.isEmpty() && !insideExec) {
      QTimer::singleShot(1, qApp, q->pump);
      return;
    }

    if (q->next < q->steps.size()) {
      const qsizetype index = q->next++;
      // Queued before the step runs: a step inside exec() returns only after a later step closed what it opened.
      QTimer::singleShot(0, qApp, q->pump);
      q->running << running_t{index, QThread::currentThread()->loopLevel(), blocker()};
      q->failures += CShotHandlers::replay(q->root, q->steps.at(index));
      q->running.removeLast();
      return;
    }

    q->done = true;
    if (q->whenReady) {
      q->failures += q->whenReady();
    }
    // The loop below what a step opened cannot return while it is up. Without steps a window up is the writer's own.
    if (!q->steps.isEmpty()) {
      closeBlockingWindows();
    }
    q->loop.quit();
  };

  QTimer deadline;
  deadline.setSingleShot(true);
  // A step that never returns keeps every loop below it running, so nothing short of exiting ends the run.
  // Armed until perform() returns: the picture and the closing of what a step opened can hang too.
  QObject::connect(&deadline, &QTimer::timeout, &queue->loop, [queue]() {
    QWidget* open = QApplication::activePopupWidget();
    if (nullptr == open) {
      open = QApplication::activeModalWidget();
    }
    if (queue->running.isEmpty()) {
      qWarning().noquote() << "shoot: the replay did not finish in" << kDeadlineMs / 1000 << "s with no step running";
    } else {
      const qsizetype index = queue->running.first().index;
      qWarning().noquote() << "shoot: the replay did not finish in" << kDeadlineMs / 1000 << "s - step" << index + 1
                           << "has not returned"
                           << (nullptr == open ? QString()
                                               : QString("and %1 is open").arg(open->metaObject()->className()))
                           << ":" << compact(queue->steps.at(index));
    }
    std::_Exit(kDeadlineExitCode);
  });
  deadline.start(kDeadlineMs);

  QTimer::singleShot(0, qApp, queue->pump);
  queue->loop.exec();
  return queue->failures;
}

void CShotReplay::clear(const QJsonArray& scenario, CShotContext& ctx) {
  CMainWindow* main = ctx.mainWindow();
  if (nullptr == main) {
    return;
  }

  const QList<CCanvas*>& canvases = main->getCanvas();
  for (CCanvas* canvas : canvases) {
    // CMouseNormal closes its screen options on unfocus().
    canvas->abortMouse();
    canvas->resetMouse();
  }
  // A plot owns the range it started, so the track refuses other resets; an idle plot's slotStopRange() resets the
  // track's mode and filter tab.
  for (IPlot* plot : main->findChildren<IPlot*>()) {
    if (plot->isSelectingRange()) {
      QMetaObject::invokeMethod(plot, "slotStopRange");
    }
    if (plot->isZoomed()) {
      QMetaObject::invokeMethod(plot, "slotResetZoom");
    }
  }
  // resetMouse() and slotStopRange() delete later, and processEvents() never delivers DeferredDelete.
  QCoreApplication::sendPostedEvents(nullptr, QEvent::DeferredDelete);

  CGisWorkspace::self().slotWksItemSelectionReset();

  const CGisListWks* wks = ctx.wksList();
  for (const QJsonValue& value : scenario) {
    const QString& hit = value.toObject()["hit"].toString();
    CGisItemTrk* trk = (hit.isEmpty() || nullptr == wks)
                           ? nullptr
                           : dynamic_cast<CGisItemTrk*>(CShotAddress::resolveItemPath(*wks, hit));
    if (nullptr != trk) {
      trk->setMouseFocusByPoint(NOPOINT, CGisItemTrk::eFocusMouseClick, "CShotReplay");
      trk->setMouseFocusByPoint(NOPOINT, CGisItemTrk::eFocusMouseMove, "CShotReplay");
    }
  }
  CShotWriter::settle(main);
}

qint32 CShotReplay::replay(const QJsonArray& scenario, CShotContext& ctx, const std::function<qint32()>& whenReady) {
  CMainWindow* main = ctx.mainWindow();
  if (scenario.isEmpty()) {
    return perform(main, {}, whenReady);
  }
  if (nullptr == main) {
    qWarning() << "shoot: a scenario is replayed and there is no main window";
    return 1;
  }

  clear(scenario, ctx);

  // The recorder puts the start state first.
  qint32 failures = 0;
  QJsonObject layout;
  QList<QJsonObject> steps;
  for (const QJsonValue& value : scenario) {
    const QJsonObject& step = value.toObject();
    const QString& verb = step["do"].toString();
    if (steps.isEmpty() && "layout" == verb && layout.isEmpty()) {
      layout = step;
      if (!CShotPage::applyLayout(*main, layout)) {
        qWarning().noquote() << "shoot: the main window refuses the scenario's layout";
        failures++;
      }
      // Taken when the recording started, so it names only what exists before the steps, which may change it.
      failures += CShotPage::applyArrangement(*main, layout);
    } else if (steps.isEmpty() && "view" == verb) {
      if (!CShotPage::applyView(ctx.canvas(), step)) {
        qWarning().noquote() << "shoot: the canvas does not take the scenario's view:" << compact(step);
        failures++;
      }
    } else {
      steps << step;
    }
  }
  // A step during a map redraw finds the DEM locked by the draw thread: the elevation it shows is missing.
  if (!CShotWriter::settleStable(main)) {
    qWarning().noquote() << "shoot: the map of the scenario's start state does not complete";
    failures++;
  }
  if (0 != failures) {
    return failures;
  }

  return perform(main, steps, whenReady);
}
