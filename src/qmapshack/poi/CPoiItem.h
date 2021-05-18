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

#ifndef CPOIITEM_H
#define CPOIITEM_H

#include "poi/IPoi.h"

#include <QMutex>
#include <QPointer>
#include <QTreeWidgetItem>

class CPoiDraw;
class QSettings;

class CPoiItem : public QTreeWidgetItem
{
public:
    CPoiItem(QTreeWidget* parent, CPoiDraw* poi);
    virtual ~CPoiItem() = default;

    void saveConfig(QSettings& cfg);
    void loadConfig(QSettings& cfg);

    /**
       @brief As the drawing thread is using the list widget to iterate of all maps to draw, all access has to be synchronized.
     */
    static QMutex mutexActivePois;

    /**
       @brief Query if poi objects are loaded
       @return True if the internal list of poi objects is not empty.
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
    void moveToBottom();

    void updateIcon();

    /**
       @brief Show or hide child treewidget items
       @param yes set true to add children, false will remove all children and delete the attached widgets
     */
    void showChildren(bool yes);


    QString getName() const
    {
        return text(0);
    }

    QPointer<IPoi>& getPoifile(){return poifile;}

    ///The POIs can be clustered together, so the icon is not necessarily displayed where the POI is.
    /// Thus the location where to draw the highlight is separately given
    bool findPoiCloseBy(const QPoint& px, QSet<poi_t>& poiItems, QList<QPointF>& posPoiHighlight) const
    {
        return poifile->findPoiCloseBy(px, poiItems, posPoiHighlight);
    }
    ///The POIs can be clustered together, so the icon is not necessarily displayed where the POI is.
    /// Thus the location where to draw the highlight is separately given
    void findPoisIn(const QRectF& degRect, QSet<poi_t>& pois, QList<QPointF>& posPoiHighlight)
    {
        getPoifile()->findPoisIn(degRect, pois, posPoiHighlight);
    }
    bool getToolTip(const QPoint& px, QString& str)
    {
        return getPoifile()->getToolTip(px, str);
    }
private:
    friend class CPoiDraw;
    CPoiDraw* poi;
    /**
       @brief A MD5 hash over the first 1024 bytes of the map file, to identify the map
     */
    QString key;
    /**
       @brief List of map files forming that particular map
     */
    QString filename;
    /**
       @brief the actual poi file object
     */
    QPointer<IPoi> poifile;
};

#endif //CPOIITEM_H

