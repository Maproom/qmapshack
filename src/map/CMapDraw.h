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

#include <QStringList>
#include "IDrawContext.h"

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

           As the setup of a map is stored in the context of the workspace the correct groups have
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

        static void setupMapPath();
        static void saveMapPath(QSettings &cfg);
        static void loadMapPath(QSettings &cfg);
        static const QStringList& getSupportedFormats(){return supportedFormats;}


    protected:
        void drawt(buffer_t& currentBuffer);


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


        /// the treewidget holding all active and inactive map items
        CMapList * mapList;

        /// the group label used in QSettings
        QString cfgGroup;

        /// the list of paths to serach maps
        static QStringList mapPaths;

        /// all existing CMapDraw instances
        static QList<CMapDraw*> maps;

        /// a list of supported map formats
        static QStringList supportedFormats;

};

#endif //CMAPDRAW_H

