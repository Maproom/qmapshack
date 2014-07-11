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

#ifndef IGISITEM_H
#define IGISITEM_H

#include <QTreeWidgetItem>

#include <QDateTime>
#include <QString>
#include <QStringList>
#include <QMap>
#include <QVariant>

class IGisItem : public QTreeWidgetItem
{
    public:
        IGisItem();
        virtual ~IGisItem();

    protected:

        struct wpt_t
        {
            // -- all gpx tags - start
            qreal lat;
            qreal lon;
            qint32 ele;
            QDateTime time;
            qint32 magvar;
            qint32 geoidheight;
            QString name;
            QString cmt;
            QString desc;
            QString src;
            QStringList link;
            QString sym;
            QString type;
            QString fix;
            qint32 sat;
            qint32 hdop;
            qint32 vdop;
            qint32 pdop;
            qint32 ageofdgpsdata;
            qint32 dgpsid;
            // -- all gpx tags - stop
            QMap<QString, QVariant> extensions;
        };

};

#endif //IGISITEM_H

