/**********************************************************************************************
    Copyright (C) 2020 Oliver Eichler <oliver.eichler@gmx.de>

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

#ifndef CPOIDRAW_H
#define CPOIDRAW_H

#include "canvas/IDrawContext.h"

class CPoiList;
class CPoiItem;

class CPoiDraw : public IDrawContext
{
public:
    CPoiDraw(CCanvas* canvas);
    virtual ~CPoiDraw();

    void saveConfig(QSettings& cfg);
    void loadConfig(QSettings& cfg);

    void loadConfigForPoiItem(CPoiItem* item);

    bool setProjection(const QString& proj) override;

    static const QStringList& getPoiPaths()
    {
        return poiPaths;
    }

    static void setupPoiPath();
    static void setupPoiPath(const QString& path);
    static void setupPoiPath(const QStringList& paths);
    static void savePoiPath(QSettings& cfg);
    static void loadPoiPath(QSettings& cfg);
    static const QStringList& getSupportedFormats()
    {
        return supportedFormats;
    }

    void findPoiCloseBy(const QPoint& px, QSet<poi_t>& poiItems, QList<QPointF>& posPoiHighlight) const;
    void findPoisIn(const QRectF& degRect, QSet<poi_t>& poiItems, QList<QPointF>& posPoiHighlight) const;
    bool getToolTip(const QPoint& px, QString& str);
protected:
    void drawt(buffer_t& currentBuffer) override;

private:
    /**
       @brief Search in paths found in mapPaths for files with supported extensions and add them to mapList.

     */
    void buildPoiList();

    /**
       @brief Save list of active maps to configuration file
     */
    void saveActivePoisList(QStringList& keys, QSettings& cfg);
    void saveActivePoisList(QStringList& keys);
    /**
       @brief Restore list of active maps from configuration file
     */
    void restoreActivePoisList(const QStringList& keys);
    void restoreActivePoisList(const QStringList& keys, QSettings& cfg);

    CPoiList* poiList;

    /// the group label used in QSettings
    QString cfgGroup;

    static QStringList poiPaths;

    static QList<CPoiDraw*> pois;

    /// a list of supported map formats
    static QStringList supportedFormats;
};

#endif //CPOIDRAW_H

