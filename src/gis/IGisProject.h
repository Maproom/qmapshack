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

#ifndef IGISPROJECT_H
#define IGISPROJECT_H

#include <QTreeWidgetItem>

class CGisListWks;
class IGisItem;
class CGisDraw;

class IGisProject : public QTreeWidgetItem
{
    public:
        IGisProject(const QString &key, CGisListWks * parent);
        virtual ~IGisProject();

        /**
           @brief Get unique project key.
           @return A MD5 hash string
         */
        const QString& getKey(){return key;}

        /**
           @brief Get a temporary pointer to the item with matching key
           @param key
           @return If no item is found 0 is returned.
        */
        IGisItem * getItemByKey(const QString& key);

        /**
           @brief Get a list of items that are close to a given pixel coordinate of the screen

           @note: The returned pointers are just for temporary use. Best you use them to get the item's key.

           @param pos       the coordinate on the screen in pixel
           @param items     a list the item's pointer is stored to.
        */
        void getItemByPos(const QPointF& pos, QList<IGisItem*>& items);


        /**
           @brief Delete items with matching key
           @param key
        */
        void delItemByKey(const QString& key);

        /**
           @brief Call IGisItem::edit() method for items with given key

           @param key   a MD5 hash key
         */
        void editItemByKey(const QString& key);


        void drawItem(QPainter& p, const QRectF& viewport, QList<QRectF>& blockedAreas, QSet<QString> &seenKeys, CGisDraw * gis);
        void drawLabel(QPainter& p, const QRectF& viewport, QList<QRectF>& blockedAreas, QSet<QString> &seenKeys, const QFontMetricsF& fm, CGisDraw * gis);
        void drawItem(QPainter& p, const QRectF& viewport, CGisDraw * gis);

    private:
        QString key;
};

#endif //IGISPROJECT_H

