/**********************************************************************************************
    Copyright (C) 2018 Oliver Eichler <oliver.eichler@gmx.de>

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

#ifndef MISC_H
#define MISC_H

#include <QCollator>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include <algorithm>
#include <initializer_list>

#include "CMainWindow.h"

#define QMS_DELETE(p) \
  delete p;           \
  p = nullptr

inline bool sortByString(const QString& str1, const QString& str2) {
  static QCollator collator;
  // this will set collator to natural sorting mode (instead of lexical)
  collator.setNumericMode(true);
  return collator.compare(str1, str2) < 0;
}

template <typename T>
bool sortByName(T* item1, T* item2) {
  return sortByString(item1->getName(), item2->getName());
}

template <typename T>
bool is_in(const T& v, std::initializer_list<T> lst) {
  return std::find(std::begin(lst), std::end(lst), v) != std::end(lst);
}

inline void openFileCheckSuccess(QIODeviceBase::OpenMode mode, QFile& file) {
  if (!file.open(mode)) {
    QMessageBox::warning(CMainWindow::self().getBestWidgetForParent(), CMainWindow::tr("File access failed..."),
                         CMainWindow::tr("Failed to open %1 with error %2").arg(file.fileName(), file.errorString()),
                         QMessageBox::Ok);
  }
}

inline QString toRichText(const QString& text) { return QString("<div>%1</div").arg(text); }

/**
   @brief Bring a path into the form used for storage and display

   Removes a trailing slash, redundant separators and "." / ".." elements. The path stays
   the one the user picked. A symbolic link is not resolved, so a registered path remains
   recognizable and keeps pointing through the link.

   @param path  the path to clean
   @return the cleaned path
 */
inline QString cleanPath(const QString& path) { return path.isEmpty() ? path : QDir::cleanPath(path); }

/**
   @brief Bring a path into the form used for comparison

   A path picked with QFileDialog and the same path built from a directory listing can
   denote one directory and still differ as strings: by a trailing slash, by a symbolic
   link (`/var` vs. `/private/var` on macOS) or by their Unicode normalization (NFC vs. NFD
   on macOS). Resolving both sides removes all three differences.

   A path that does not exist - an unplugged drive - cannot be resolved and is only cleaned.
   Both sides of a comparison have to be resolved at the same time therefore. Else a path
   that became resolvable in between will no longer match.

   @param path  the path to resolve
   @return the resolved path, or the cleaned path if it does not exist (yet)
 */
inline QString resolvedPath(const QString& path) {
  if (path.isEmpty()) {
    return path;
  }
  const QString& resolved = QFileInfo(path).canonicalFilePath();
  return resolved.isEmpty() ? QDir::cleanPath(path) : resolved;
}

/**
   @brief Test if a path is registered already

   Paths are compared by their resolved form. Thus a path and a symbolic link to it count
   as the same registration and the directory's content is not listed twice.

   @param paths  the registered paths
   @param path   the path to look for
   @return true if path denotes one of the registered paths
 */
inline bool containsPath(const QStringList& paths, const QString& path) {
  const QString& resolved = resolvedPath(path);
  return std::any_of(paths.begin(), paths.end(),
                     [&resolved](const QString& other) { return resolvedPath(other) == resolved; });
}

#endif  // MISC_H
