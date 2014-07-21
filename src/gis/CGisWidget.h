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
        void draw(QPainter& p, const QRectF& viewport, CGisDraw *gis);
        void getItemByPos(const QPointF& pos, QList<IGisItem*> items);

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

