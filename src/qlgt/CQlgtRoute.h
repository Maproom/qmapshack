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

#ifndef CQLGTROUTE_H
#define CQLGTROUTE_H

#include <QObject>
#include <proj_api.h>
#include "qlgt/IItem.h"

class CQlgtRoute : public QObject, public IItem
{
    public:
        CQlgtRoute(quint64 id, QObject * parent);
        virtual ~CQlgtRoute();

        enum type_e {eEnd, eBase, eRtePts, eRteSec};
        struct pt_t
        {
            float lon;
            float lat;

            QString action;

            operator const projXY ()
            {
                projXY p;
                p.u = lon;
                p.v = lat;
                return p;
            }
        };

        /// primary route, just the basic points like A to B via C
        QVector<pt_t> priRoute;

        quint32 ttime;

        QString iconString;

};


QDataStream& operator >>(QDataStream& s, CQlgtRoute& rte);
QDataStream& operator <<(QDataStream& s, CQlgtRoute& rte);

#endif //CQLGTROUTE_H

