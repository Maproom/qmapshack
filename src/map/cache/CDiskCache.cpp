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

#include "CDiskCache.h"

#include <QtWidgets>

CDiskCache::CDiskCache(const QString &path, qint32 size, qint32 days, QObject * parent)
    : IDiskCache(parent)
    , dir(path)
    , size(size)
    , expiration(days)
    , dummy(256,256, QImage::Format_ARGB32)
{
    dummy.fill(Qt::transparent);

    dir.mkpath(dir.path());
    QFileInfoList files = dir.entryInfoList(QStringList("*.png"), QDir::Files);
    foreach(const QFileInfo &fileinfo, files)
    {
        QString hash    = fileinfo.baseName();
        table[hash]     = fileinfo.fileName();
    }

    timer = new QTimer(this);
    timer->setSingleShot(false);
    timer->start(20000);
    connect(timer, SIGNAL(timeout()), this, SLOT(slotCleanup()));
}

CDiskCache::~CDiskCache()
{
}

void CDiskCache::store(const QString& key, QImage& img)
{
    QMutexLocker lock(&mutex);

    QCryptographicHash md5(QCryptographicHash::Md5);
    md5.addData(key.toLatin1());

    QString hash        = md5.result().toHex();
    QString filename    = QString("%1.png").arg(hash);

    if(!img.isNull())
    {
        img.save(dir.absoluteFilePath(filename));
        table[hash] = filename;
        cache[hash] = img;
    }
    else
    {
        cache[hash] = dummy;
    }
}

void CDiskCache::restore(const QString& key, QImage& img)
{
    QMutexLocker lock(&mutex);

    QCryptographicHash md5(QCryptographicHash::Md5);
    md5.addData(key.toLatin1());

    QString hash = md5.result().toHex();

    if(cache.contains(hash))
    {
        img = cache[hash];
    }
    else if(table.contains(hash))
    {
        img.load(dir.absoluteFilePath(table[hash]));
        if(!cache.contains(hash))
        {
            cache[hash] = img;
        }
    }
    else
    {
        img = QImage();
    }
}

bool CDiskCache::contains(const QString& key)
{
    QMutexLocker lock(&mutex);

    QCryptographicHash md5(QCryptographicHash::Md5);
    md5.addData(key.toLatin1());

    QString hash = md5.result().toHex();
    return table.contains(hash) || cache.contains(hash);
}

void CDiskCache::slotCleanup()
{
    QMutexLocker lock(&mutex);

    QFileInfoList files = dir.entryInfoList(QStringList("*.png"), QDir::Files);
    QDateTime now       = QDateTime::currentDateTime();
    int days            = expiration;
    qint32 maxSize      = size * 1024 * 1024;
    qint32 tmpSize      = 0;
    // expire old files and calculate cache size
    foreach(const QFileInfo &fileinfo, files)
    {
        if(fileinfo.lastModified().daysTo(now) > days)
        {
            QString hash = fileinfo.baseName();
            table.remove(hash);
            cache.remove(hash);
            QFile::remove(fileinfo.absoluteFilePath());
        }
        else
        {
            tmpSize += fileinfo.size();
        }
    }

    if(tmpSize > maxSize)
    {
        // if cache is still too large remove oldest files
        foreach(const QFileInfo &fileinfo, files)
        {
            QString hash = fileinfo.baseName();
            table.remove(hash);
            cache.remove(hash);
            QFile::remove(fileinfo.absoluteFilePath());

            qDebug() << "remove" << fileinfo.absoluteFilePath();

            size -= fileinfo.size();

            if(size < maxSize)
            {
                break;
            }
        }
    }
}
