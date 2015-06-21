/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler oliver.eichler@gmx.de

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

#include "canvas/IDrawContext.h"
#include <QStringList>

class QPainter;
class CCanvas;
class CMapList;
class QSettings;
class CMapItem;

class CMapDraw : public IDrawContext
{
    Q_OBJECT
public:
    CMapDraw(CCanvas * parent);
    virtual ~CMapDraw();

    void saveConfig(QSettings& cfg);
    void loadConfig(QSettings& cfg);
    /**
       @brief This is called most likely from the item itself to call it's loadConfig() method.

       As the setup of a map is stored in the context of the view the correct groups have
       to be set prior to call the item's loadConfig() method. However the item does not know
       all that stuff. That is why it has to ask it's CMapDraw object to prepare the QSettings object
       and to call loadConfig();

       @param item the item to call it's loadConfig() method
     */
    void loadConfigForMapItem(CMapItem * item);

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

    /**
       @brief Set projection of this draw context
       @param proj      a proj4 string
     */
    void setProjection(const QString& proj);

    static void setupMapPath();
    static void setupMapPath(const QString &path);
    static void setupMapPath(const QStringList &paths);
    static void saveMapPath(QSettings &cfg);
    static void loadMapPath(QSettings &cfg);
    static const QStringList& getSupportedFormats()
    {
        return supportedFormats;
    }
    static const QString& getCacheRoot()
    {
        return cachePath;
    }

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
       the threshold. The returned poylline uses lon/lat as coordinates.

       @param pt1           first point in [rad]
       @param pt2           second point in [rad]
       @param threshold     the "close enough" threshold in [pixel]
       @param polyline      the resulting polyline, if any, in [rad]
       @return              Return true if a line has been found.
     */
    bool findPolylineCloseBy(const QPointF& pt1, const QPointF& pt2, qint32 threshold, QPolygonF& polyline);

protected:
    void drawt(buffer_t& currentBuffer);


private:
    /**
       @brief Search in paths found in mapPaths for files with supported extensions and add them to mapList.

     */
    void buildMapList();
    /**
       @brief Save list of active maps to configuration file

       The group context will be appended by the map's key

       @param keys the stored map's MD5 keys will be written to keys
       @param cfg  configuration file with correct group context set.
     */
    void saveActiveMapsList(QStringList &keys, QSettings &cfg);

    /**
       @brief Open configuration befor saving list
       @param keys the stored map's MD5 keys will be written to keys
     */
    void saveActiveMapsList(QStringList &keys);
    /**
       @brief Restore list of active maps from configuration file
       @param keys MD5 hash keys to identify the maps
     */
    void restoreActiveMapsList(const QStringList &keys);

    void restoreActiveMapsList(const QStringList& keys, QSettings& cfg);



    /// the treewidget holding all active and inactive map items
    CMapList * mapList;

    /// the group label used in QSettings
    QString cfgGroup;

    /// the list of paths to serach maps
    static QStringList mapPaths;

    static QString cachePath;

    /// all existing CMapDraw instances
    static QList<CMapDraw*> maps;

    /// a list of supported map formats
    static QStringList supportedFormats;
};

#endif //CMAPDRAW_H

