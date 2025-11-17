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

#ifndef CMAPDRAW_H
#define CMAPDRAW_H

#include <QStringList>

#include "canvas/IDrawContext.h"

class QPainter;
class CCanvas;
class CMapList;
class QSettings;
class CMapItem;
struct IPoiItem;

class CMapDraw : public IDrawContext {
  Q_OBJECT
 public:
  CMapDraw(CCanvas* parent);
  virtual ~CMapDraw();

  void saveConfig(QSettings& cfg);
  void loadConfig(QSettings& cfg);

  /**
     @brief Get a full detailed info text about objects close to the given point

     This method will call getInfo() of all items in mapList.

     @param px    the point on the screen in pixel
     @param str   a string object to receive all information
   */
  void getInfo(const QPoint& px, QString& str);
  /**
     @brief Get an info text fit for a tool tip

     This method will call getToolTip() of all items in mapList.

     @param px    the point on the screen in pixel
     @param str   a string object to receive all information
   */
  void getToolTip(const QPoint& px, QString& str);

  IPoiItem findPOICloseBy(const QPoint& px) const;

  /**
     @brief Set projection of this draw context
     @param proj      a proj4 string
   */
  bool setProjection(const QString& proj) override;

  static const QStringList& getMapPaths() { return mapPaths; }

  static void setupMapPath();
  static void setupMapPath(const QString& path);
  static void setupMapPath(const QStringList& paths);
  static void saveMapPath(QSettings& cfg);
  static void loadMapPath(QSettings& cfg);
  static const QStringList& getSupportedFormats() { return supportedFormats; }
  static const QString& getCacheRoot() { return cachePath; }

  /**
     @brief Forward messages to CCanvas::reportStatus()

     Messages from various sources will be collected in a list and displayed in the top left corner
     of the widget.

     @note The object reporting has to take care to remove the message by reporting an empty string.

     @param key   the key to identify the reporting object
     @param msg   the message to report
   */
  void reportStatusToCanvas(const QString& key, const QString& msg);

  /**
     @brief Find a matching street polyline

     The polyline must be close enough in terms of pixel to point 1 and 2. "Close enough" is defined by
     the threshold. The returned polyline uses lon/lat as coordinates.

     @param pt1           first point in [rad]
     @param pt2           second point in [rad]
     @param threshold     the "close enough" threshold in [pixel]
     @param polyline      the resulting polyline, if any, in [rad]
     @return              Return true if a line has been found.
   */
  bool findPolylineCloseBy(const QPointF& pt1, const QPointF& pt2, qint32 threshold, QPolygonF& polyline);

  /**
     @brief Build a usable map list from a single map file

    This will clear the map list and add the gven map as the only one.
    The map will be activated, too.

    Used for a map view like in the BRouter wizard.

     @param filename  the map's filename, can be a resource, too
   */
  void buildMapList(const QString& filename);

 signals:
  void sigActiveMapsChanged(bool noActiveMap);

 protected:
  void drawt(buffer_t& currentBuffer) override;

 private slots:
  void slotChanged();
  void saveMapList();

 private:
  /**
     @brief Create a CMapItem from a filename

     @param filename    the map's filename, can be a resuource, too
     @param fallbackKey a fallback key used if no key can be derived from the file

     @return The created map item.
   */
  CMapItem* createMapItem(const QString& filename, const QString& fallbackKey);

  /**
     @brief Load the list of maps including all configurations

    This will parse the map paths for map file and compare it with the
    stored list of map keys in the config. Based on that a list of active,
    inactive, new and missing maps is created. If possible the last configuration
    of a map is restored.

   */
  void loadMapList(QSettings& cfg);

  /**
   * @brief Load map list with configuration from the application's settings.
   */
  void loadMapList();

  /**
     @brief Store the list of maps including all configurations

    This will iterate of the list of maps, store their configuration
    and store all keys in the same order as the list.

   */
  void saveMapList(QSettings& cfg);

  /// the treewidget holding all active and inactive map items
  CMapList* mapList;

  /**
   *  @brief delay timer to save config after a change
   * 
   * This is used to combine multiple change signals within a second
   * and to give saving a cool down time if anything crashes on 
   * a change.
   */
  QTimer* timerDelayedSave;

  /// the list of paths to search maps
  static QStringList mapPaths;

  static QString cachePath;

  /// all existing CMapDraw instances
  static QList<CMapDraw*> maps;

  /// a list of supported map formats
  static QStringList supportedFormats;

  bool hasActiveMap = false;
};

#endif  // CMAPDRAW_H
