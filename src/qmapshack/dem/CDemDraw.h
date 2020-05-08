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

#include "canvas/IDrawContext.h"

class QPainter;
class CDemList;
class CCanvas;
class QSettings;
class CDemItem;

class CDemDraw : public IDrawContext
{
public:
    CDemDraw(CCanvas * canvas);
    virtual ~CDemDraw();

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
    void loadConfigForDemItem(CDemItem * item);

    qreal getElevationAt(const QPointF& pos, bool checkScale = false);
    void  getElevationAt(const QPolygonF& pos, QPolygonF& ele);
    void  getElevationAt(SGisLine& line);

    qreal getSlopeAt(const QPointF& pos, bool checkScale = false);
    void  getSlopeAt(const QPolygonF& pos, QPolygonF& slope);

    void setProjection(const QString& proj) override;

    static const QStringList& getDemPaths()
    {
        return demPaths;
    }

    static void setupDemPath();
    static void setupDemPath(const QString &path);
    static void setupDemPath(const QStringList &paths);
    static void saveDemPath(QSettings &cfg);
    static void loadDemPath(QSettings &cfg);
    static const QStringList& getSupportedFormats()
    {
        return supportedFormats;
    }

protected:
    void drawt(buffer_t& currentBuffer) override;

private:
    /**
       @brief Search in paths found in mapPaths for files with supported extensions and add them to mapList.

     */
    void buildMapList();

    /**
       @brief Save list of active maps to configuration file
     */
    void saveActiveMapsList(QStringList &keys, QSettings &cfg);
    void saveActiveMapsList(QStringList &keys);
    /**
       @brief Restore list of active maps from configuration file
     */
    void restoreActiveMapsList(const QStringList &keys);
    void restoreActiveMapsList(const QStringList& keys, QSettings &cfg);

    CDemList * demList;

    /// the group label used in QSettings
    QString cfgGroup;

    static QStringList demPaths;

    static QList<CDemDraw*> dems;

    /// a list of supported map formats
    static QStringList supportedFormats;
};

#endif //CDEMDRAW_H

