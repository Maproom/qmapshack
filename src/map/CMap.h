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

#ifndef CMAP_H
#define CMAP_H

#include <QThread>

class QPainter;
class CCanvas;
class QListWidget;

class CMap : public QThread
{
    public:
        CMap(CCanvas * parent);
        virtual ~CMap();

        void draw(QPainter& p, bool needsRedraw);

    protected:
        CCanvas * canvas;

        QListWidget * maps;
};

#endif //CMAP_H

