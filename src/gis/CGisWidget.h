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

#ifndef CGISWIDGET_H
#define CGISWIDGET_H

#include <QWidget>
#include "ui_IGisWidget.h"

class CGisDraw;
class IGisItem;
class IGisProject;

class CGisWidget : public QWidget, private Ui::IGisWidget
{
    Q_OBJECT
    public:
        static CGisWidget& self(){return *pSelf;}
        virtual ~CGisWidget();

        void loadGisProject(const QString& filename);
        /**
           @brief Draw all loaded data in the workspace that is visible

           This method is called from The CGisDraw thread. The thread has to make sure
           that everything is thread safe.

           @param p         the painter to be used
           @param viewport  the viewport in units of rad
           @param gis       the draw context to be used
        */
        void draw(QPainter& p, const QRectF& viewport, CGisDraw *gis);


        /**
           @brief Draw all data that is time variant and can't wait for a full update

           This method is called directly from the main thread's paintEvent() method.

           @param p         the painter to be used
           @param viewport  the viewport in units of rad
           @param gis       the draw context to be used
        */
        void fastDraw(QPainter& p, const QRectF& viewport, CGisDraw *gis);

        /**
           @brief Get items close to the given point

           Note: Do not store the pointers of items permanently as they can become invalid
           once you reach the main event loop again. Store the key instead.

           @param pos       the position in pixel
           @param items     an empty item list that will get filled with temporary pointers
        */
        void getItemsByPos(const QPointF& pos, QList<IGisItem *> &items);

        /**
           @brief Find first item with matching key
           @param key       the item's key as it is returned from IGisItem::getKey()
           @return If no item is found 0 is returned.
        */
        IGisItem * getItemByKey(const QString& key);

        /**
           @brief Delete all items with matching key from workspace

           As an item can be attached to several projects each one of these items is deleted.
           The method will prompt the user with a message box for each item deleted

           @param key       the item's key as it is returned from IGisItem::getKey()
         */
        void delItemByKey(const QString& key);

        /**
           @brief Edit / view item details
           @param key       the item's key as it is returned from IGisItem::getKey()
         */
        void editItemByKey(const QString& key);

        /**
           @brief Clone waypoint and move clone
           @param key       the item's key as it is returned from IGisItem::getKey()
         */
        void projWptByKey(const QString& key);

        /**
           @brief Move waypoint via mouse
           @param key       the item's key as it is returned from IGisItem::getKey()
         */
        void moveWptByKey(const QString& key);

        /**
           @brief Set user focus to track
           @param yes       true if focus is set
           @param key       the item's key as it is returned from IGisItem::getKey()
         */
        void focusTrkByKey(bool yes, const QString& key);


        void cutTrkByKey(const QString& key);

        void editTrkByKey(const QString& key);

        void reverseTrkByKey(const QString& key);

        void combineTrkByKey(const QString& key);

        void rangeTrkByKey(const QString& key);

        void editAreaByKey(const QString& key);



        /**
           @brief Select a project via dialog

           If a new project name is entered a new project is created. Else the pointer
           to an existing project is passed back.

           @return 0 if no project was selected.
         */
        IGisProject * selectProject();

    signals:
        void sigChanged();

    public slots:
        void slotSaveAll();


    private:
        friend class CMainWindow;
        CGisWidget(QMenu * menuProject, QWidget * parent);

        static CGisWidget * pSelf;
};

#endif //CGISWIDGET_H

