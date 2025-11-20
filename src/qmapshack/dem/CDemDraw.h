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

#ifndef CDEMDRAW_H
#define CDEMDRAW_H

#include <QStringList>

#include "canvas/IDrawContext.h"

class QPainter;
class CCanvas;
class CDemList;
class QSettings;
class CDemItem;

class CDemDraw : public IDrawContext {
 public:
  CDemDraw(CCanvas* canvas);
  virtual ~CDemDraw();

  void saveConfig(QSettings& cfg);
  void loadConfig(QSettings& cfg);

  qreal getElevationAt(const QPointF& pos, bool checkScale = false);
  void getElevationAt(const QPolygonF& pos, QPolygonF& ele);
  void getElevationAt(SGisLine& line);
  qreal getSlopeAt(const QPointF& pos, bool checkScale = false);
  void getSlopeAt(const QPolygonF& pos, QPolygonF& slope);

  /**
     @brief Set projection of this draw context
     @param proj      a proj4 string
   */
  bool setProjection(const QString& proj) override;

  static const QStringList& getDemPaths() { return demPaths; }

  static void setupDemPath();
  static void setupDemPath(const QString& path);
  static void setupDemPath(const QStringList& paths);
  static void saveDemPath(QSettings& cfg);
  static void loadDemPath(QSettings& cfg);
  static const QStringList& getSupportedFormats() { return supportedFormats; }

 protected:
  void drawt(buffer_t& currentBuffer) override;

 private slots:
  void slotChanged();
  void saveDemList();

 private:
  /**
     @brief Create a CDemItem from a filename

    @param filename    the DEM's filename, can be a resuource, too
    @param fallbackKey a fallback key used if no key can be derived from the file

    @return The created map item.
  */
  CDemItem* createDemItem(const QString& filename, const QString& fallbackKey);

  /**
     @brief Load the list of DEMs including all configurations

   This will parse the map paths for map file and compare it with the
   stored list of map keys in the config. Based on that a list of active,
   inactive, new and missing DEM is created. If possible the last configuration
   of a map is restored.

   */
  void loadDemList(QSettings& cfg);

  /**
   * @brief Load DEM list with configuration from the application's settings.
   */
  void loadDemList();

  /**
     @brief Store the list of DEMs including all configurations

   This will iterate of the list of DEMs, store their configuration
   and store all keys in the same order as the list.

   */
  void saveDemList(QSettings& cfg);

  CDemList* demList;

  /**
   *  @brief delay timer to save config after a change
   *
   * This is used to combine multiple change signals within a second
   * and to give saving a cool down time if anything crashes on
   * a change.
   */
  QTimer* timerDelayedSave;

  /// the list of paths to search maps
  static QStringList demPaths;

	/// all existing CDemDraw instances
  static QList<CDemDraw*> dems;

  /// a list of supported map formats
  static QStringList supportedFormats;
};

#endif  // CDEMDRAW_H
