/**********************************************************************************************
    Copyright (C) 2020 Oliver Eichler <oliver.eichler@gmx.de>
    Copyright (C) 2021 Henri Hornburg <pingurus@t-online.de>

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

#ifndef CPOIPOI_H
#define CPOIPOI_H

#include "poi/IPoi.h"

#include <QMutex>
#include <QTimer>

class CPoiPOI : public IPoi
{
public:
    CPoiPOI(const QString& filename, CPoiDraw *parent);
    virtual ~CPoiPOI() = default;

    void addTreeWidgetItems(QTreeWidget *widget) override;
    // category, minLon multiplied by 10, minLat multiplied by 10. POIs are loaded in squares of degrees (should be fine enough to not hang the system)
    void loadPOIsFromFile(const QString &category, int minLonM10, int minLatM10);

    void draw(IDrawContext::buffer_t& buf) override;

    virtual bool findPoiCloseBy(const QPoint& px, poi_t& poiItem) const override;
    virtual void findPoisIn(const QRectF& degRect, QList<poi_t>&pois) override;
    virtual bool getToolTip(const QPoint& px, QString& str) const override;

    static void init()
    {
        tagMap = initTagMap();
    }

public slots:
    void slotCheckedStateChanged(QTreeWidgetItem*item) override;

private:
    struct rawPoi_t
    {
        QStringList data;
        QPointF coordinates; // in radians
        poi_t toPoi(const QString &defaultName) const;
    };
    enum SqlColumn_e
    {
        eSqlColumnMaxLat,
        eSqlColumnMaxLon,
        eSqlColumnMinLat,
        eSqlColumnMinLon,
        eSqlColumnData
    };

    QMutex mutex;
    QString filename;
    QTimer* loadTimer;
    QMap<QString, Qt::CheckState> categoryActivated;
    // category, minLon multiplied by 10, minLat multiplied by 10. POIs are loaded in squares of degrees (should be fine enough to not hang the system)
    QMap<QString, QMap<int, QMap<int, QList<rawPoi_t> > > > loadedPOIs;

    static QMap<QString, QString> tagMap;
    static QMap<QString, QString> initTagMap();
};

#endif //CPOIPOI_H

