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

class QMouseEvent;

class IScrOpt : public QObject
{
    public:
        IScrOpt(QObject * parent);
        virtual ~IScrOpt();


        void setOrigin(const QPoint& pos){origin = pos;}
        const QPoint& getOrigin(){return origin;}

        virtual void draw(QPainter& p) = 0;
        virtual void mouseMoveEvent(QMouseEvent *);

    protected:

        QPoint origin;

        QPoint mousePos;


};

#endif //ISCROPT_H

