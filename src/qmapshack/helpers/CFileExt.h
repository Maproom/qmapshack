/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler <oliver.eichler@gmx.de>

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

#ifndef CFILEEXT_H
#define CFILEEXT_H

#include <QFile>
#include <QtCore>

class CFileExt : public QFile {
 public:
  CFileExt(const QString& filename) : QFile(filename), mapped(nullptr) { cnt++; }

  ~CFileExt() { cnt--; }

#ifndef Q_OS_WIN32
  // data access function
  const char* data(qint64 offset, qint64 s) {
    mapped = map(offset, s);
    mappedSections << mapped;
    return (const char*)mapped;
  }

  void free() {
    for (uchar* p : std::as_const(mappedSections)) {
      unmap(p);
    }
    mappedSections.clear();
  }

#else
  // data access function
  const char* data(qint64 offset, qint64 s) {
    uchar* p = map(offset, s);
    return (const char*)p;
  }
#endif

 private:
  static int cnt;

  uchar* mapped;
  QSet<uchar*> mappedSections;
};

#endif  // CFILEEXT_H
