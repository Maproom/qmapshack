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

#ifndef CMAPITEM_H
#define CMAPITEM_H

#include <QMutex>
#include <QPointer>
#include <QTreeWidgetItem>

class IMap;
class CMapDraw;
class CMapPropSetup;
class QSettings;

class CMapItem : public QTreeWidgetItem
{
public:
    CMapItem(QTreeWidget * parent, CMapDraw *map);
    virtual ~CMapItem();

    void saveConfig(QSettings& cfg);
    void loadConfig(QSettings& cfg);

    /**
       @brief As the drawing thread is using the list widget to iterate of all maps to draw, all access has to be synchronized.
     */
    static QMutex mutexActiveMaps;

    /**
       @brief Query if map objects are loaded
       @return True if the internal list of map objects is not empty.
     */
    bool isActivated();
    /**
       @brief Either loads or destroys internal map objects
       @return True if the internal list of maps is not empty after the operation.
     */
    bool toggleActivate();
    /**
     * @brief Load all internal map objects
     * @return Return true on success.
     */
    bool activate();
    /**
       @brief Delete all internal map objects
     */
    void deactivate();
    /**
       @brief Move item to top of list widget
     */
    void moveToTop();
    /**
       @brief Move item to bottom of active maps list
     */
    void moveToBottom();

    /**
       @brief Set item's icon according to map type and state
     */
    void updateIcon();

    /**
       @brief Show or hide child treewidget items
       @param yes set true to add children, false will remove all children and delete the attached widgets
     */
    void showChildren(bool yes);

private:
    friend class CMapDraw;
    CMapDraw * map;
    /**
       @brief A MD5 hash over the first 1024 bytes of the map file, to identify the map
     */
    QString key;
    /**
       @brief List of map files forming that particular map
     */
    QString filename;
    /**
       @brief List of loaded map objects when map is activated.
     */
    QPointer<IMap> mapfile;
};

#endif //CMAPITEM_H

