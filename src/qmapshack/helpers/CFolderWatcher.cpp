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

#include "helpers/CFolderWatcher.h"

#include <QApplication>
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QFileInfo>

#include "helpers/CProgressDialog.h"

namespace {
constexpr qint32 kPollMs = 5000;
}  // namespace

CFolderWatcher::CFolderWatcher(const QStringList& nameFilters, QObject* parent)
    : QObject(parent), nameFilters(nameFilters) {
  timerPoll.setInterval(kPollMs);
  connect(&watcher, &QFileSystemWatcher::directoryChanged, this, &CFolderWatcher::slotDirectoryChanged);
  connect(&timerPoll, &QTimer::timeout, this, &CFolderWatcher::slotPoll);
}

void CFolderWatcher::setPaths(const QStringList& newPaths) {
  QStringList existing;
  for (const QString& path : newPaths) {
    const QFileInfo fi(path);
    if (fi.isDir() && !existing.contains(fi.absoluteFilePath())) {
      existing << fi.absoluteFilePath();
    }
  }

  if (existing != paths) {
    const QStringList& watched = watcher.directories();
    if (!watched.isEmpty()) {
      watcher.removePaths(watched);
    }
    paths = existing;
    if (!paths.isEmpty()) {
      const QStringList& failed = watcher.addPaths(paths);
      if (!failed.isEmpty()) {
        qWarning() << "Changes in these folders are noticed only at the next start:" << failed;
      }
    }
  }

  // A poll still running keeps going: a copy may not be finished yet.
  known = snapshot(nameFilters);
}

void CFolderWatcher::slotDirectoryChanged() {
  if (timerPoll.isActive()) {
    return;
  }
  polled = snapshot({});
  timerPoll.start();
}

void CFolderWatcher::slotPoll() {
  // all files: tiles beside a .vrt must be complete too
  const QStringList& all = snapshot({});
  // a progress dialog runs nested event loops even while it is not shown yet
  if (all != polled || QApplication::activeModalWidget() != nullptr || CProgressDialog::self() != nullptr) {
    polled = all;
    return;
  }

  timerPoll.stop();
  const QStringList& current = snapshot(nameFilters);
  if (current != known) {
    known = current;
    qDebug() << "Files changed in" << paths;
    emit sigChanged();
  }
}

QStringList CFolderWatcher::snapshot(const QStringList& filters) const {
  QStringList entries;
  for (const QString& path : paths) {
    const QDir dir(path);
    const QStringList& names = dir.entryList(filters, QDir::Files | QDir::Readable, QDir::Name);
    for (const QString& name : names) {
      // A fresh stat: a directory listing on Windows may report the size of a file still being written late.
      const QFileInfo fi(dir.filePath(name));
      entries
          << QString("%1|%2|%3").arg(fi.absoluteFilePath()).arg(fi.size()).arg(fi.lastModified().toMSecsSinceEpoch());
    }
  }
  return entries;
}
