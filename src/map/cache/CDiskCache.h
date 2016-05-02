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

#include <QDir>
#include <QHash>
#include <QImage>
#include <QMutex>

class QTimer;

class CDiskCache : public QObject
{
    Q_OBJECT
public:
    CDiskCache(const QString& path, qint32 size, qint32 days, QObject *parent);
    virtual ~CDiskCache() = default;

    void store(const QString& key, QImage& img);
    void restore(const QString& key, QImage& img);
    bool contains(const QString& key) const;

private slots:
    void slotCleanup();

private:
    void removeCacheFile(const QFileInfo &fileinfo);

    QDir dir;

    const qint32 maxSizeMB;      //< maximum cache size in MB
    const qint32 expirationDays; //< expiration time in days

    /// hash table to cache images as files on disc
    QHash<QString, QString> table;
    /// hash table to cache loaded images in memory
    QHash<QString, QImage>  cache;

    QTimer * timer;

    QImage dummy {256,256, QImage::Format_ARGB32};

    mutable QMutex mutex;
};

#endif //CDISKCACHE_H

