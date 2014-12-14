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

#ifndef IQLGTOVERLAY_H
#define IQLGTOVERLAY_H

#include <QObject>
#include <proj_api.h>
#include "qlgt/IItem.h"

class IQlgtOverlay : public QObject, public IItem
{
    public:
        IQlgtOverlay(QObject * parent);
        virtual ~IQlgtOverlay();

        enum type_e {eEnd,eBase};

        struct pt_t : public projXY
        {
            int idx;
        };


        QString type;
        QColor color;
        QList<pt_t> points;
        qint32 style;
        quint32 width;
        quint8 opacity;

};

QDataStream& operator >>(QDataStream& s, IQlgtOverlay& ovl);
QDataStream& operator <<(QDataStream& s, IQlgtOverlay& ovl);


#endif //IQLGTOVERLAY_H

