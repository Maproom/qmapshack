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

#ifndef CQLGTWPT_H
#define CQLGTWPT_H

#include "qlgt/IItem.h"

class CQlgtWpt : public QObject, public IItem
{
public:
    CQlgtWpt(quint64 id, QObject * parent);
    virtual ~CQlgtWpt();

    void setIcon(const QString& str);

    enum geocacheservice_e {eGC, eOC, eTC};
    enum type_e {eEnd,eBase,eImage,eGeoCache};

    struct geocachelog_t
    {
        geocachelog_t() : id(0)
        {
        }
        quint32 id;
        QString date;
        QString type;
        QString finderId;
        QString finder;
        QString text;
    };

    struct geocache_t
    {
        geocache_t() : service(eOC), hasData(false), id(0), available(true), archived(false), difficulty(0), terrain(0), exportBuddies(false)
        {
        }
        geocacheservice_e service;
        bool hasData;
        quint32 id;
        bool available;
        bool archived;
        float difficulty;
        float terrain;
        QString status;
        QString name;
        QString owner;
        QString ownerId;
        QString type;
        QString container;
        QString shortDesc;
        QString longDesc;
        QString hint;
        QString country;
        QString state;
        QString locale;
        QList<geocachelog_t> logs;
        bool exportBuddies;
    };


    geocache_t geocache;

    quint32 selected;
    quint32 sticky;
    float lat;                   ///< [deg]
    float lon;                   ///< [deg]
    float ele;                   ///< [m]
    float prx;                   ///< [m]
    float dir;                   ///< [deg]
    QString link;
    QString urlname;
    QString type;

    struct image_t
    {
        quint32 offset;
        QString info;
        QPixmap pixmap;
        QString filePath;
        QString fileName;
    };
    QList<image_t> images;

    struct buddy_t
    {
        QString name;
        QSet<QString> pos;
        float lon;
        float lat;
    };

    QList<buddy_t> buddies;
};

QDataStream& operator >>(QDataStream& s, CQlgtWpt& wpt);
QDataStream& operator <<(QDataStream& s, CQlgtWpt& wpt);


#endif //CQLGTWPT_H

