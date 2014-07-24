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

#ifndef ISCROPT_H
#define ISCROPT_H

#include <QObject>
#include <QPixmap>
#include <QRect>

class IScrOpt : public QObject
{
    public:
        IScrOpt(QObject * parent);
        virtual ~IScrOpt();

        virtual void clear(){items.clear();}
        virtual int  size(){return items.size();}

        virtual void draw(QPainter& p, const QPoint& point) = 0;

    protected:
        struct item_t
        {
            QString name;
            QString key;
            QPixmap icon;
            QRect   area;
            QRect   text;
        };

        QList<item_t> items;
};

#endif //ISCROPT_H

