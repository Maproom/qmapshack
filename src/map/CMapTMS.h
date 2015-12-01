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
#include <QTime>

class IDiskCache;
class QListWidgetItem;
class QNetworkAccessManager;
class QNetworkReply;

class CMapTMS : public IMap
{
    Q_OBJECT
public:
    CMapTMS(const QString& filename, CMapDraw *parent);
    virtual ~CMapTMS();

    void draw(IDrawContext::buffer_t& buf);

    void getLayers(QListWidget& list);

    void saveConfig(QSettings& cfg);

    void loadConfig(QSettings& cfg);

signals:
    void sigQueueChanged();

protected:
    void configureCache();

private slots:
    void slotQueueChanged();
    void slotRequestFinished(QNetworkReply* reply);
    void slotLayersChanged(QListWidgetItem * item);


private:
    struct layer_t;
    QString createUrl(const layer_t& layer, int x, int y, int z);

    struct layer_t
    {
        layer_t() : enabled(true), minZoomLevel(0), maxZoomLevel(0)
        {
        }
        bool enabled;
        qint32 minZoomLevel;
        qint32 maxZoomLevel;
        QString title;
        QString strUrl;
        QString script;
    };

    struct rawHeaderItem_t
    {
        QString name;
        QString value;
    };

    QVector<layer_t> layers;

    QString name;

    qint32 minZoomLevel = 1;

    qint32 maxZoomLevel = 21;

    QList<rawHeaderItem_t> rawHeaderItems;

    /// Mutex to control access to url queue
    QMutex mutex {QMutex::Recursive};
    /// a queue with all tile urls to request
    QQueue<QString> urlQueue;
    /// the tile cache
    IDiskCache * diskCache = nullptr;
    /// access manager to request tiles
    QNetworkAccessManager * accessManager = nullptr;

    QList<QString> urlPending;

    bool lastRequest = false;

    QTime timeLastUpdate;
};

#endif //CMAPTMS_H

