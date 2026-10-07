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

#include "shoot/CShotContext.h"

#include <QDebug>
#include <QImage>

#include "CMainWindow.h"
#include "gis/CGisWorkspace.h"
#include "gis/ovl/CGisItemOvlArea.h"
#include "gis/rte/CGisItemRte.h"
#include "gis/trk/CGisItemTrk.h"
#include "gis/wpt/CGisItemWpt.h"
#include "shoot/CShotWriter.h"

CShotContext::CShotContext(const CShotWriter& writer) : writer(writer) {}

CMainWindow* CShotContext::mainWindow() const { return CMainWindow::isNull() ? nullptr : &CMainWindow::self(); }

CCanvas* CShotContext::canvas() const {
  const CMainWindow* main = mainWindow();
  return (nullptr == main) ? nullptr : main->getVisibleCanvas();
}

const CGisListWks* CShotContext::wksList() const {
  return (nullptr == mainWindow()) ? nullptr : &CGisWorkspace::self().getWksList();
}

QList<IGisItem::key_t> CShotContext::keys() const {
  QList<IGisItem::key_t> keys;
  const QList<IGisItem*> items = {fixture.trk, fixture.wpt, fixture.rte, fixture.area};
  for (IGisItem* item : items) {
    if (nullptr != item) {
      keys << item->getKey();
    }
  }
  return keys;
}

void CShotContext::setFixture(IGisProject* project, CGisItemTrk* trk, CGisItemWpt* wpt, CGisItemRte* rte,
                              CGisItemOvlArea* area) {
  fixture = {project, trk, wpt, rte, area};
}

void CShotContext::begin(const QString& id, const QRect& rect) {
  this->id = id;
  this->rect = rect;
  frameNo = 0;
}

bool CShotContext::shot(QWidget* w, const QSize& size) { return emitPicture(w, size, id); }

bool CShotContext::frame(QWidget* w, const QSize& size) {
  const QString& stem = QString("%1.%2").arg(id).arg(frameNo++, 4, 10, QChar('0'));
  return emitPicture(w, size, stem);
}

bool CShotContext::emitPicture(QWidget* w, const QSize& size, const QString& stem) const {
  const QImage& image = CShotWriter::render(w, size);
  if (image.isNull()) {
    qWarning() << "shoot:" << stem << "has no picture";
    return false;
  }
  // resize() silently stops at the minimum size.
  if (size.isValid() && image.size() != size) {
    qWarning() << "shoot:" << stem << "asks for" << size << "and the picture is" << image.size();
    return false;
  }
  if (!rect.isValid()) {
    return !writer.write(image, stem).isEmpty();
  }
  if (!image.rect().contains(rect)) {
    qWarning() << "shoot:" << stem << "keeps" << rect << "which is not inside" << image.rect();
    return false;
  }
  return !writer.write(image.copy(rect), stem).isEmpty();
}
