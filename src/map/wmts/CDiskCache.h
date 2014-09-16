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

#ifndef CDISKCACHE_H
#define CDISKCACHE_H

#include "map/wmts/IDiskCache.h"

#include <QDir>
#include <QHash>
#include <QImage>

class QTimer;

class CDiskCache : public IDiskCache
{
    Q_OBJECT
    public:
        CDiskCache(QObject *parent);
        virtual ~CDiskCache();

        virtual void store(const QString& key, QImage& img);
        virtual void restore(const QString& key, QImage& img);
        virtual bool contains(const QString& key);

    private slots:
        void slotCleanup();

    private:
        QDir dir;

        /// hash table to cache images als files on disc
        QHash<QString, QString> table;
        /// hash table to cache loaded images in memory
        QHash<QString, QImage>  cache;

        QTimer * timer;

        QImage dummy;

};

#endif //CDISKCACHE_H

