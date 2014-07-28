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

class CGisWidget : public QWidget, private Ui::IGisWidget
{
    Q_OBJECT
    public:
        static CGisWidget& self(){return *pSelf;}
        virtual ~CGisWidget();

        void loadGpx(const QString& filename);
        /**
           @brief Draw all loaded data in the workspace that is visible

           @param p         the painter to be used
           @param viewport  the viewport in units of rad
           @param gis       the draw context to be used
         */
        void draw(QPainter& p, const QRectF& viewport, CGisDraw *gis);
        /**
           @brief Get items close to the given point

           Note: Do not store the pointers in items permanently as they can become invalid
           once you reach the main event loop again.

           @param pos       the position in pixel
           @param items     an empty item list that will get filled with temporary pointers
         */
        void getItemsByPos(const QPointF& pos, QList<IGisItem *> &items);

        IGisItem * getItemByKey(const QString& key);

        void delItemByKey(const QString& key);

    signals:
        void sigChanged();

    public slots:
        void slotSaveAll();


    private:
        friend class CMainWindow;
        CGisWidget(QWidget * parent);

        static CGisWidget * pSelf;
};

#endif //CGISWIDGET_H

