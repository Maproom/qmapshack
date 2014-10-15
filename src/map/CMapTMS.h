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

#ifndef CMAPTMS_H
#define CMAPTMS_H

#include "map/IMap.h"
#include <QQueue>
#include <QNetworkAccessManager>

class IDiskCache;
class QListWidgetItem;

class CMapTMS : public IMap
{
    Q_OBJECT
    public:
        CMapTMS(const QString& filename, CMapDraw *parent);
        virtual ~CMapTMS();

        void draw(IDrawContext::buffer_t& buf);

        void getLayers(QListWidget& list);

    signals:
        void sigQueueChanged();

    protected:
        void configureCache();

    private slots:
        void slotQueueChanged();
        void slotRequestFinished(QNetworkReply* reply);
        void slotLayersChanged(QListWidgetItem * item);


    private:
        QString name;
        /// Mutex to control access to url queue
        QMutex mutex;
        /// a queue with all tile urls to request
        QQueue<QString> urlQueue;
        /// the tile cache
        IDiskCache * diskCache;
        /// access mangager to request tiles
        QNetworkAccessManager * accessManager;

        QList<QString> urlPending;

        bool lastRequest;

};

#endif //CMAPTMS_H

