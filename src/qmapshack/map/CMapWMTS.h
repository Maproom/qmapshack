/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler <oliver.eichler@gmx.de>

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

#ifndef CMAPWMTS_H
#define CMAPWMTS_H
#include "map/IMapOnline.h"
#include <QMap>


class CMapDraw;
class CDiskCache;
class QNetworkAccessManager;
class QNetworkReply;
class QListWidgetItem;


class CMapWMTS : public IMapOnline
{
    Q_OBJECT
public:
    CMapWMTS(const QString& filename, CMapDraw* parent);
    virtual ~CMapWMTS() {}

    void draw(IDrawContext::buffer_t& buf) override;

    void getLayers(QListWidget& list) override;

    void saveConfig(QSettings& cfg) override;
    void loadConfig(QSettings& cfg) override;

private slots:
    void slotLayersChanged(QListWidgetItem* item);

private:
    struct limit_t
    {
        qint32 minTileRow;
        qint32 maxTileRow;
        qint32 minTileCol;
        qint32 maxTileCol;
    };

    struct layer_t
    {
        bool enabled;
        QString title;
        QStringList styles;
        QString tileMatrixSet;
        QRectF boundingBox;
        QString resourceURL;
        QMap<QString, limit_t> limits;
    };

    QList<layer_t> layers;

    struct  tilematrix_t
    {
        QPointF topLeft;
        qreal scale;
        qint32 tileWidth;
        qint32 tileHeight;
        qint32 matrixWidth;
        qint32 matrixHeight;
    };

    struct tileset_t
    {
        CProj proj;
        QMap<QString, tilematrix_t> tilematrix;
    };

    QMap<QString, tileset_t> tilesets;
};

#endif //CMAPWMTS_H

