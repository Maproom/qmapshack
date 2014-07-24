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

#ifndef CSCROPTUNCLUTTER_H
#define CSCROPTUNCLUTTER_H

#include "mouse/IScrOpt.h"

class IGisItem;


class CScrOptUnclutter : public IScrOpt
{
    public:
        CScrOptUnclutter(QObject * parent);
        virtual ~CScrOptUnclutter();

        void addItem(IGisItem * gisItem);
        QString getItemKey(int index = 0);
        bool selectItem(const QPoint& point);

        void draw(QPainter& p, const QPoint& point);

    private:
        static const QPoint positions[];
};

#endif //CSCROPTUNCLUTTER_H

