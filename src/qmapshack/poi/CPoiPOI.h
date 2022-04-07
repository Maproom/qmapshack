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
#include "poi/CPoiItemPOI.h"
#include "poi/IPoiFile.h"

#include <QCoreApplication>
#include <QMutex>
#include <QTimer>

class CPoiPOI : public IPoiFile
{
    Q_DECLARE_TR_FUNCTIONS(CPoiPOI)
public:
    CPoiPOI(const QString& filename, CPoiDraw* parent);

    void addTreeWidgetItems(QTreeWidget* widget) override;
    // category, minLon multiplied by 10, minLat multiplied by 10. POIs are loaded in squares of degrees (should be fine enough to not hang the system)
    virtual void loadPOIsFromFile(quint64 categoryID, int minLonM10, int minLatM10) override;

    bool getToolTip(const QPoint& px, QString& str) const override;

    static void init()
    {
        tagMap = initTagMap();
    }


private:
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

    void getPoiIcon(QPixmap& icon, const poiGroup_t& poiGroup) override;
    void getPoiIcon(QPixmap& icon, const CPoiItem* poi, const QString& definingTag = "") override;

    QString filename;

    static QMap<QString, CPoiIconCategory> tagMap;
    static QMap<QString, CPoiIconCategory> initTagMap();
};

#endif //CPOIPOI_H

