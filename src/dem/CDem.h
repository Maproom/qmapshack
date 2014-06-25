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

#ifndef CDEM_H
#define CDEM_H

#include <QThread>

class QPainter;
class CDemList;
class CCanvas;

class CDem : public QThread
{
    public:
        CDem(CCanvas * canvas);
        virtual ~CDem();

        void draw(QPainter& p, bool needsRedraw, const QPointF& f, const QRectF &r);

        qreal getElevation(const QPointF& pos);


    private:
        CDemList * demList;

        static QStringList demPaths;

        static QList<CDem*> dems;

};

#endif //CDEM_H

