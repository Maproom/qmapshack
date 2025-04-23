/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler <oliver.eichler@gmx.de>
    Copyright (C) 2017 Norbert Truchsess <norbert.truchsess@t-online.de>

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

#ifndef IDEVICE_H
#define IDEVICE_H

#include <QDir>
#include <QTreeWidgetItem>

#include "gis/IGisItem.h"
class CGisDraw;
class CGisItemWpt;
class CDeviceGarmin;

class IDevice : public QTreeWidgetItem {
  Q_DECLARE_TR_FUNCTIONS(IDevice)
 public:
  enum type_e { eTypeNone = 0, eTypeGarmin = 1, eTypeTwoNav = 2, eTypeGarminMtp = 3 };

  IDevice(const QString& path, type_e type, const QString& key, QTreeWidget* parent);
  IDevice(const QString& path, type_e type, const QString& key, CDeviceGarmin* parent);
  virtual ~IDevice();

  static void mount(const QString& path);
  static void umount(const QString& path);
  static int count() { return cnt; }

  void mount() { mount(key); }
  void umount() { umount(key); }

  const QString& getKey() const { return key; }

  QString getName() const;

  void getItemsByPos(const QPointF& pos, QList<IGisItem*>& items);
  void getItemsByArea(const QRectF& area, IGisItem::selflags_t flags, QList<IGisItem*>& items);
  void getNogoAreas(QList<IGisItem*>& nogos);
  IGisItem* getItemByKey(const IGisItem::key_t& key);
  void getItemsByKeys(const QList<IGisItem::key_t>& keys, QList<IGisItem*>& items);
  void editItemByKey(const IGisItem::key_t& key);

  void drawItem(QPainter& p, const QPolygonF& viewport, QList<QRectF>& blockedAreas, CGisDraw* gis);
  void drawLabel(QPainter& p, const QPolygonF& viewport, QList<QRectF>& blockedAreas, const QFontMetricsF& fm,
                 CGisDraw* gis);
  void drawItem(QPainter& p, const QRectF& viewport, CGisDraw* gis);

  void insertCopyOfProject(IGisProject* project, int& lastResult);
  void updateProject(IGisProject* project);

  virtual void startSavingProject(IGisProject* project) {}
  virtual void saveImages(CGisItemWpt& wpt) {}
  virtual void loadImages(CGisItemWpt& wpt) {}
  virtual void aboutToRemoveProject(IGisProject* project) {}

  IGisProject* getProjectByKey(const QString& key);

 protected:
  virtual void insertCopyOfProject(IGisProject* project) = 0;
  /**
     @brief Test if a project's filename/path is already used

     This can happen if there is already a project with the same name
     but different or no key.

     @param filename
     @return If the current operation should be aborted return true.
   */
  bool testForExternalProject(const QString& filename);

  static int cnt;

  QDir dir;
  QString key;
};

class CDeviceMountLock {
 public:
  CDeviceMountLock(IDevice& device) : device(device) { device.mount(); }

  ~CDeviceMountLock() { device.umount(); }

 private:
  IDevice& device;
};

#endif  // IDEVICE_H
