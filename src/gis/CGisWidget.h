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
#include <QSqlDatabase>
#include <QEvent>
#include "ui_IGisWidget.h"

#include "gis/IGisItem.h"

class CGisDraw;
class IGisProject;

enum event_types_e
{
     eEvtD2WReqInfo     = QEvent::User + 1
    ,eEvtD2WShowFolder  = QEvent::User + 2
    ,eEvtD2WHideFolder  = QEvent::User + 3
    ,eEvtD2WShowItems   = QEvent::User + 4
    ,eEvtD2WHideItems   = QEvent::User + 5

    ,eEvtW2DAckInfo     = QEvent::User + 100
};

struct evt_item_t
{
    evt_item_t(quint64 id, quint32 type) : id(id), type(type){}
    quint64 id;
    quint32 type;
};

class CEvtD2WReqInfo : public QEvent
{
    public:
        CEvtD2WReqInfo(quint64 id, const QString& db) : QEvent(QEvent::Type(eEvtD2WReqInfo)), id(id), db(db){}

        quint64 id;
        QString db;
};

class CEvtD2WShowFolder : public QEvent
{
    public:
        CEvtD2WShowFolder(quint64 id, const QString& db) : QEvent(QEvent::Type(eEvtD2WShowFolder)), id(id), db(db){}

        quint64 id;
        QString db;
};

class CEvtD2WHideFolder : public QEvent
{
    public:
        CEvtD2WHideFolder(quint64 id, const QString& db) : QEvent(QEvent::Type(eEvtD2WHideFolder)), id(id), db(db){}

        quint64 id;
        QString db;
};

class CEvtD2WShowItems : public QEvent
{
    public:
        CEvtD2WShowItems(quint64 id, const QString& db) : QEvent(QEvent::Type(eEvtD2WShowItems)), id(id), db(db){}

        quint64 id;
        QString db;
        QList<evt_item_t> items;
};

class CEvtD2WHideItems : public QEvent
{
    public:
        CEvtD2WHideItems(quint64 id, const QString& db) : QEvent(QEvent::Type(eEvtD2WHideItems)), id(id), db(db){}

        quint64 id;
        QString db;
        QSet<QString> keys;
};

class CEvtW2DAckInfo : public QEvent
{
    public:
        CEvtW2DAckInfo(bool loaded, quint64 id, const QString& db) : QEvent(QEvent::Type(eEvtW2DAckInfo)), isLoaded(loaded), id(id), db(db){}

        bool isLoaded;
        quint64 id;
        QString db;
        QSet<QString> keysChildren;

};


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
        IGisItem * getItemByKey(const IGisItem::key_t &key);

        /**
           @brief Delete all items with matching key from workspace

           As an item can be attached to several projects each one of these items is deleted.
           The method will prompt the user with a message box for each item deleted

           @param key       the item's key as it is returned from IGisItem::getKey()
         */
        void delItemByKey(const IGisItem::key_t &key);

        /**
           @brief Edit / view item details
           @param key       the item's key as it is returned from IGisItem::getKey()
         */
        void editItemByKey(const IGisItem::key_t &key);

        /**
           @brief Clone waypoint and move clone
           @param key       the item's key as it is returned from IGisItem::getKey()
         */
        void projWptByKey(const IGisItem::key_t &key);

        /**
           @brief Move waypoint via mouse
           @param key       the item's key as it is returned from IGisItem::getKey()
         */
        void moveWptByKey(const IGisItem::key_t &key);

        /**
           @brief Set user focus to track
           @param yes       true if focus is set
           @param key       the item's key as it is returned from IGisItem::getKey()
         */
        void focusTrkByKey(bool yes, const IGisItem::key_t &key);


        void cutTrkByKey(const IGisItem::key_t &key);

        void editTrkByKey(const IGisItem::key_t &key);

        void reverseTrkByKey(const IGisItem::key_t &key);

        void combineTrkByKey(const IGisItem::key_t &key);

        void rangeTrkByKey(const IGisItem::key_t &key);

        void editAreaByKey(const IGisItem::key_t &key);

        /**
           @brief Select a project via dialog

           If a new project name is entered a new project is created. Else the pointer
           to an existing project is passed back.

           @return 0 if no project was selected.
         */
        IGisProject * selectProject();

        void postEventForWks(QEvent * event);
        void postEventForDb(QEvent * event);


    signals:
        void sigChanged();

    public slots:
        void slotSaveAll();

    private slots:
        void slotHelpText();


    private:
        friend class CMainWindow;
        CGisWidget(QMenu * menuProject, QWidget * parent);

        static CGisWidget * pSelf;
};

#endif //CGISWIDGET_H

