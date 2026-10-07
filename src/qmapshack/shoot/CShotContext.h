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

#ifndef CSHOTCONTEXT_H
#define CSHOTCONTEXT_H

#include <QList>
#include <QRect>
#include <QSize>
#include <QString>

#include "gis/IGisItem.h"

class CCanvas;
class CGisItemOvlArea;
class CGisItemRte;
class CGisItemTrk;
class CGisItemWpt;
class CGisListWks;
class CMainWindow;
class CShotWriter;
class IGisProject;
class QWidget;

/** @brief What a shot writes its pictures through, and the fixture it is taken against. */
class CShotContext {
 public:
  explicit CShotContext(const CShotWriter& writer);

  /**
     @brief Start shot @p id and restart the frame count.

     @param rect  the crop of every picture, invalid for none
   */
  void begin(const QString& id, const QRect& rect = QRect());

  /**
     @param size  a window's size, invalid for its sizeHint; ignored for a non-window widget
     @return false when no picture was written
   */
  bool shot(QWidget* w, const QSize& size = QSize());

  /** @brief The next picture of the sequence `<id>.0000`, `<id>.0001`, ...; false when none was written. */
  bool frame(QWidget* w, const QSize& size = QSize());

  /** @return the main window, nullptr before it is constructed */
  CMainWindow* mainWindow() const;

  /** @return the canvas of the map view in front, nullptr when there is none */
  CCanvas* canvas() const;

  /** @return the workspace tree, nullptr before the main window is constructed */
  const CGisListWks* wksList() const;

  /** The fixture's items by role; nullptr until CShotFixture has loaded them. */
  IGisProject* project() const { return fixture.project; }
  CGisItemTrk* trk() const { return fixture.trk; }
  CGisItemWpt* wpt() const { return fixture.wpt; }
  CGisItemRte* rte() const { return fixture.rte; }
  CGisItemOvlArea* area() const { return fixture.area; }

  QList<IGisItem::key_t> keys() const;

  void setFixture(IGisProject* project, CGisItemTrk* trk, CGisItemWpt* wpt, CGisItemRte* rte, CGisItemOvlArea* area);

  /**
     @brief The scenario the application is in already; a shot taken in it performs none of its steps.

     Empty for none: a build performs every scenario. A step is not idempotent, so a scenario performed on top of itself
     is another state.
   */
  void setLiveScenario(const QString& name) { live = name; }
  const QString& liveScenario() const { return live; }

 private:
  /** @return false when the picture is refused and nothing was written */
  bool emitPicture(QWidget* w, const QSize& size, const QString& stem) const;

  struct fixture_t {
    IGisProject* project = nullptr;
    CGisItemTrk* trk = nullptr;
    CGisItemWpt* wpt = nullptr;
    CGisItemRte* rte = nullptr;
    CGisItemOvlArea* area = nullptr;
  };

  const CShotWriter& writer;
  QString id;
  QRect rect;
  qint32 frameNo = 0;
  fixture_t fixture;
  QString live;
};

#endif  // CSHOTCONTEXT_H
