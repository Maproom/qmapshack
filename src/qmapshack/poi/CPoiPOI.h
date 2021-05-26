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

#include "poi/CPoiIconCategory.h"
#include "poi/CRawPoi.h"
#include "poi/IPoi.h"

#include <QCoreApplication>
#include <QMutex>
#include <QTimer>

class CPoiPOI : public IPoi
{
    Q_DECLARE_TR_FUNCTIONS(CPoiPOI)
public:
    CPoiPOI(const QString& filename, CPoiDraw* parent);
    virtual ~CPoiPOI() = default;

    void addTreeWidgetItems(QTreeWidget* widget) override;
    // category, minLon multiplied by 10, minLat multiplied by 10. POIs are loaded in squares of degrees (should be fine enough to not hang the system)
    void loadPOIsFromFile(quint64 categoryID, int minLonM10, int minLatM10);

    void draw(IDrawContext::buffer_t& buf) override;

    ///The POIs can be clustered together, so the icon is not necessarily displayed where the POI is.
    /// Thus the location where to draw the highlight is separately given
    bool findPoiCloseBy(const QPoint& px, QSet<poi_t>& poiItems, QList<QPointF>& posPoiHighlight) const override;
    ///The POIs can be clustered together, so the icon is not necessarily displayed where the POI is.
    /// Thus the location where to draw the highlight is separately given
    void findPoisIn(const QRectF& degRect, QSet<poi_t>& pois, QList<QPointF>& posPoiHighlight) override;
    bool getToolTip(const QPoint& px, QString& str) const override;

    static void init()
    {
        tagMap = initTagMap();
    }

public slots:
    void slotCheckedStateChanged(QTreeWidgetItem* item) override;

private:
    struct poiGroup_t
    {
        ///Area covered by the icon in pixels
        QRectF iconLocation;
        ///Location of the center of the icon in rad
        QPointF iconCenter;
        QSet<quint64> pois;
    };

    enum SqlColumnPoi_e
    {
        eSqlColumnPoiMaxLat,
        eSqlColumnPoiMaxLon,
        eSqlColumnPoiMinLat,
        eSqlColumnPoiMinLon,
        eSqlColumnPoiData,
        eSqlColumnPoiId
    };
    enum SqlColumnCategory_e
    {
        eSqlColumnCategoryId,
        eSqlColumnCategoryName,
        eSqlColumnCategoryParent
    };

    void getPoiIcon(QPixmap& icon, const poiGroup_t& poiGroup);
    void getPoiIcon(QPixmap& icon, const CRawPoi& poi, const QString& definingTag = "");
    bool overlapsWithIcon(const QRectF& rect) const;
    bool getPoiGroupCloseBy(const QPoint& px, poiGroup_t& poiItem) const;

    mutable QMutex mutex {QMutex::Recursive};
    QString filename;
    QTimer* loadTimer;


    QMap<quint64, Qt::CheckState> categoryActivated;
    QMap<quint64, QString> categoryNames;
    // category, minLon multiplied by 10, minLat multiplied by 10. POIs are loaded in squares of degrees (should be fine enough to not hang the system)
    QMap<quint64, QMap<int, QMap<int, QList<quint64>>>> loadedPoisByArea;
    QMap<quint64, CRawPoi> loadedPois;
    QList<poiGroup_t> displayedPois;
    QRectF bbox;


    static QMap<QString, CPoiIconCategory> tagMap;
    static QMap<QString, CPoiIconCategory> initTagMap();
};

#endif //CPOIPOI_H

